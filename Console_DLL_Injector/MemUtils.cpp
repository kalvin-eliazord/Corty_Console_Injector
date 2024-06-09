#include "MemUtils.h"

namespace fs = std::filesystem;

bool MemUtils::WriteMem(auto* pAddr, auto* pValue, auto pSize)
{
	if (!(pAddr && pValue && pSize))
	{
		std::cerr << "[-] Cannot write memory, one of the input is invalid. \n";
		return false;
	}

	if (!this->dataProc.hProc)
	{
		std::cerr << "[-] Process invalid, can't write to memory. \n";
		return false;
	}

	SIZE_T byteW{ 0 };
	if (!WriteProcessMemory(this->dataProc.hProc, pAddr, pValue, pSize, &byteW) || byteW == 0)
	{
		std::cerr << "[-] Memory writing failed\n";
		return false;
	}

	return true;
}

std::vector<PROCESSENTRY32> MemUtils::GetProcList()
{
	std::vector<PROCESSENTRY32> procList{};

	HANDLE hSnap{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL) };

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do {
				procList.push_back(procEntry);
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procList;
}

bool MemUtils::SetDllName()
{
	std::string currentDir{ "./" };

	for (const auto& file : fs::directory_iterator(currentDir))
	{
		if (file.path().extension() == ".dll")
		{
			std::string nameBuffer = file.path().generic_string();
			this->dataDll.name = nameBuffer.substr(2, nameBuffer.length());
			return true;
		}
	}

	this->dataDll.name = "No Dll file in the directory. \n";
	return false;
}

void MemUtils::SetDllPath()
{
	if (SetDllName())
		this->dataDll.path = GetCurrDirectory() + "\\" + this->dataDll.name;
}

void MemUtils::InitDataProc(PageProcess* pPageProc)
{
	HANDLE hProc{ OpenProcess(PROCESS_ALL_ACCESS, 0, pPageProc->GetUserIdProc()) };

	if (hProc == INVALID_HANDLE_VALUE)
	{
		std::cerr << "[!] Failed to open process. Error: \n" << GetLastError();;
		system("PAUSE");
		return;
	}

	this->dataProc.hProc = hProc;
	this->dataProc.id = pPageProc->GetUserIdProc();
	this->dataProc.name = pPageProc->GetUserProcName();
}

std::string MemUtils::GetCurrDirectory()
{
	constexpr int bufferSize{ 1024 };
	char bufferDir[bufferSize];

	if (!_getcwd(bufferDir, bufferSize))
	{
		std::cerr << "[!] Can't get current directory path.";
		return std::string();
	}

	return bufferDir;
}

bool MemUtils::WinAPI_Inject_Start()
{
	void* memAlloc{ VirtualAllocEx(this->dataProc.hProc, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) };

	if (!memAlloc)
	{
		std::cerr << "[!] Failed to allocate memory. Error: \n" << GetLastError();
		CloseHandle(this->dataProc.hProc);
		return false;
	}

	SIZE_T bytesWritten{ 0 };
	if (!WriteMem(memAlloc, this->dataDll.path.c_str(), strlen(this->dataDll.path.c_str()) + 1))
	{
		std::cerr << "[!] Failed to write process memory. Error: \n" << GetLastError();
		VirtualFreeEx(this->dataProc.hProc, memAlloc, 0, MEM_RELEASE);
		CloseHandle(this->dataProc.hProc);
		return false;
	}

	HANDLE remoteThread{ CreateRemoteThread(this->dataProc.hProc, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, memAlloc,  NULL, nullptr) };
	if (!remoteThread || remoteThread == INVALID_HANDLE_VALUE)
	{
		std::cerr << "[!] Failed to create remote thread. Error:\n" << GetLastError();;
		VirtualFreeEx(this->dataProc.hProc, memAlloc, 0, MEM_RELEASE);
		CloseHandle(this->dataProc.hProc);
		return false;
	}

	WaitForSingleObject(remoteThread, INFINITE);
	CloseHandle(remoteThread);

	VirtualFreeEx(this->dataProc.hProc, memAlloc, 0, MEM_RELEASE);
	CloseHandle(this->dataProc.hProc);

	return true;
}

bool PE_Header::Init(BYTE* pBase)
{
	constexpr int valid_EXE_id{ 0x5A4D };
	if (reinterpret_cast<IMAGE_DOS_HEADER*>(pBase)->e_magic != valid_EXE_id)
	{
		std::cerr << "[-] The module is not a valid executable. \n";
		return false;
	}

	this->nt = reinterpret_cast<IMAGE_NT_HEADERS*>(pBase + reinterpret_cast<IMAGE_DOS_HEADER*>(pBase)->e_lfanew);
	this->opt = &nt->OptionalHeader;
	this->file = &nt->FileHeader;

	if (this->file->Machine != CURRENT_ARCH)
	{
		std::cerr << "[-] This is not a Windows DLL. \n";
		return false;
	}

	return true;
}

bool MemUtils::ManualMap_Start()
{
	if (!DllMap()) return false;
	if (!ImportAndShellCodeMap()) return false;

	return true;
}

bool MemUtils::DllMap()
{
	std::ifstream fDll(this->dataDll.path, std::ios::binary | std::ios::ate);
	if (fDll.fail())
	{
		std::cerr << "[-] Cannot open the DLL file. \n";
		fDll.close();
		return false;
	}

	const auto fSize{ fDll.tellg() };
	if (fSize < 0x1000)
	{
		std::cerr << "[-] Invalid file size. \n";
		fDll.close();
		return false;
	}

	BYTE* dllBuffer{ new BYTE[(UINT_PTR)fSize] };
	if (!dllBuffer)
	{
		std::cerr << "[-] Cannot initialize DLL buffer. \n";
		fDll.close();
		delete[] dllBuffer;
		return false;
	}

	fDll.seekg(0, std::ios::beg);
	fDll.read(reinterpret_cast<char*>(dllBuffer), fSize);
	fDll.close();

	if (!this->pe_head.Init(dllBuffer))
	{
		std::cerr << "[-] Cannot initialize the DLL PE Header. \n";
		delete[] dllBuffer;
		return false;
	}

	this->mAllocProc = reinterpret_cast<BYTE*>(VirtualAllocEx(
		this->dataProc.hProc,
		&this->pe_head.opt->ImageBase,
		this->pe_head.opt->SizeOfImage,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE));

	if (!this->mAllocProc)
	{
		this->mAllocProc = reinterpret_cast<BYTE*>(VirtualAllocEx(
			this->dataProc.hProc,
			nullptr,
			this->pe_head.opt->SizeOfImage,
			MEM_RESERVE | MEM_COMMIT,
			PAGE_EXECUTE_READWRITE));

		if (!this->mAllocProc)
		{
			std::cerr << "[-] Cannot allocate memory for the Dll. \n";
			delete[] dllBuffer;
			return false;
		}
	}

	// PE Header mapping
	if (!WriteMem(this->mAllocProc, dllBuffer, 0x1000))
	{
		std::cout << "[-] Can't write file header : " << GetLastError() << '\n';
		VirtualFreeEx(this->dataProc.hProc, this->mAllocProc, 0, MEM_RELEASE);
		delete[] dllBuffer;
		return false;
	}

	// Dll mapping
	auto* currSection{ IMAGE_FIRST_SECTION(this->pe_head.nt) };
	for (UINT i{ 0 }; i < this->pe_head.file->NumberOfSections; ++currSection, ++i)
	{
		if (!currSection->SizeOfRawData)
			continue;

		auto* currAddr{ reinterpret_cast<intptr_t*>(this->mAllocProc + currSection->VirtualAddress) };
		auto* currData{ reinterpret_cast<intptr_t*>(dllBuffer + currSection->PointerToRawData) };
		if (!WriteMem(currAddr, currData, currSection->SizeOfRawData))
		{
			std::cerr << "[-] Cannot map DLL sections into the target process. \n";
			VirtualFreeEx(this->dataProc.hProc, this->mAllocProc, 0, MEM_RELEASE);
			delete[] dllBuffer;
			return false;
		}
	}

	return true;
}

bool MemUtils::ImportAndShellCodeMap()
{
	auto& hProc{ this->dataProc.hProc };

	DataImport data{ 0 };
	data._LoadLibraryA = LoadLibraryA;
	data._GetProcAddress = GetProcAddress;
	data.baseAddr = this->mAllocProc;

	BYTE* mAllocImport{ reinterpret_cast<BYTE*>(VirtualAllocEx(hProc, nullptr, sizeof(DataImport), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) };
	if (!mAllocImport) {
		std::cerr << "[-] Import Data memory allocation failed \n" << GetLastError();
		VirtualFreeEx(hProc, this->mAllocProc, 0, MEM_RELEASE);
		return false;
	}

	if (!WriteMem(mAllocImport, &data, sizeof(DataImport)))
	{
		std::cerr << "[-] Can't map Import Data : " << GetLastError() << '\n';
		VirtualFreeEx(hProc, this->mAllocProc, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, mAllocImport, 0, MEM_RELEASE);
		return false;
	}

	void* mAllocShell{ VirtualAllocEx(hProc, nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE) };
	if (!mAllocShell)
	{
		std::cerr << "[-] Memory shellcode allocation failed : " << GetLastError() << '\n';
		VirtualFreeEx(hProc, this->mAllocProc, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, mAllocImport, 0, MEM_RELEASE);
		return false;
	}

	if (!WriteMem(mAllocShell, ShellLoader, 0x1000))
	{
		std::cerr << "[-] Can't write shellcode : " << GetLastError() << '\n';
		VirtualFreeEx(hProc, this->mAllocProc, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, mAllocImport, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, mAllocShell, 0, MEM_RELEASE);
		return false;
	}

	std::cout << "[+] Mapping process finished.\n";

#ifdef _DEBUG
	std::cout << "[+] My shellcode pointer : " << ShellLoader << '\n';
	std::cout << "[+] Target point : " << mAllocShell << '\n';
	system("pause");
#endif

	HANDLE hThread{ CreateRemoteThread(hProc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(mAllocShell), mAllocImport, 0, nullptr) };
	if (!hThread) {
		std::cout << "Thread creation failed " << GetLastError() << '\n';
		VirtualFreeEx(hProc, this->mAllocProc, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, mAllocImport, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, mAllocShell, 0, MEM_RELEASE);
		return false;
	}
	CloseHandle(hThread);

	std::cout << "Thread created at: " << mAllocShell << " waiting for return...\n";

	HINSTANCE hCheck{ NULL };
	while (!hCheck) {
		DWORD exitcode = 0;
		GetExitCodeProcess(hProc, &exitcode);
		if (exitcode != STILL_ACTIVE) {
			std::cerr << "Process crashed, exit code: " << exitcode << '\n';
			return false;
		}

		DataImport data_checked{ 0 };
		ReadProcessMemory(hProc, mAllocImport, &data_checked, sizeof(data_checked), nullptr);
		hCheck = data_checked.hMod;

		if (hCheck == (HINSTANCE)0x404040) {
			std::cout << "Wrong mapping ptr\n";
			VirtualFreeEx(hProc, this->mAllocProc, 0, MEM_RELEASE);
			VirtualFreeEx(hProc, mAllocImport, 0, MEM_RELEASE);
			VirtualFreeEx(hProc, mAllocShell, 0, MEM_RELEASE);
			return false;
		}
		else if (hCheck == (HINSTANCE)0x505050) {
			std::cout << "WARNING: Exception support failed!\n";
		}

		Sleep(10);
	}

	BYTE* emptyBuffer = (BYTE*)malloc(1024 * 1024 * 20);
	if (emptyBuffer == nullptr) {
		std::cerr << "Unable to allocate memory\n";
		return false;
	}
	memset(emptyBuffer, 0, 1024 * 1024 * 20);

	if (!WriteMem(mAllocShell, emptyBuffer, 0x1000))
		std::cerr << "WARNING: Can't clear shellcode\n";

	if (!VirtualFreeEx(hProc, mAllocShell, 0, MEM_RELEASE))
		std::cerr << "WARNING: can't release shell code memory\n";

	if (!VirtualFreeEx(hProc, mAllocImport, 0, MEM_RELEASE))
		std::cerr << "WARNING: can't release mapping data memory\n";

	return true;
}

#pragma runtime_checks( "", off )
#pragma optimize( "", off )
void _stdcall ShellLoader(DataImport* pDataImport)
{
	if (!pDataImport)
	{
		pDataImport->hMod = (HINSTANCE)0x404040;
		return;
	}

	auto* pBase{ pDataImport->baseAddr };
	IMAGE_OPTIONAL_HEADER* pOpt{ &reinterpret_cast<IMAGE_NT_HEADERS*>(pBase + reinterpret_cast<IMAGE_DOS_HEADER*>(pBase)->e_lfanew)->OptionalHeader };

	// Relocation
	BYTE* relativePrefAddr{ pBase - pOpt->ImageBase };
	if (relativePrefAddr && pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
	{
		auto* relocBase{ reinterpret_cast<IMAGE_BASE_RELOCATION*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) };
		while (relocBase->VirtualAddress)
		{
			const UINT maxEntries{ (relocBase->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD) };
			WORD* relocEntry{ reinterpret_cast<WORD*>(relocBase + 1) };
			for (UINT i{ 0 }; i < maxEntries; ++i, ++relocEntry)
			{
				if (!RELOC_FLAG(*relocEntry)) continue;

				const WORD relocRVA{ static_cast<WORD>(*relocEntry & 0xFFF) };
				UINT_PTR* relocEntryAddr{ reinterpret_cast<UINT_PTR*>(pBase + relocBase->VirtualAddress + relocRVA) };
				*relocEntryAddr += reinterpret_cast<UINT_PTR>(relativePrefAddr);
			}
			relocBase = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<BYTE*>(relocBase) + relocBase->SizeOfBlock);
		}
	}

	// Import fixing
	if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		auto* importDesc{ reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) };

		auto _LoadLibA{ pDataImport->_LoadLibraryA };
		auto _GetProcAddr{ pDataImport->_GetProcAddress };

		while (importDesc->Name)
		{
			HINSTANCE hMod{ _LoadLibA(reinterpret_cast<char*>(pBase + importDesc->Name)) };

			ULONG_PTR* IAT{ reinterpret_cast<ULONG_PTR*>(pBase + importDesc->OriginalFirstThunk) };
			ULONG_PTR* ILT{ reinterpret_cast<ULONG_PTR*>(pBase + importDesc->FirstThunk) };

			if (!IAT) IAT = ILT;

			for (; *IAT; ++IAT, ++ILT)
			{
				if (IMAGE_SNAP_BY_ORDINAL(*IAT))
				{
					*ILT = (ULONG_PTR)_GetProcAddr(hMod, reinterpret_cast<char*>(*IAT & 0xFFFF));
				}
				else
				{
					auto* importName{ reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(pBase + *IAT) };
					*ILT = (ULONG_PTR)_GetProcAddr(hMod, importName->Name);
				}
			}
			++importDesc;
		}
	}

	// Callback executing
	if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
	{
		auto* tlsBase{ reinterpret_cast<IMAGE_TLS_DIRECTORY*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress) };
		auto* callBack{ reinterpret_cast<PIMAGE_TLS_CALLBACK*>(tlsBase->AddressOfCallBacks) };
		for (; callBack && *callBack; ++callBack)
			(*callBack)(pBase, pDataImport->fdwReasonParam, pDataImport->reservedParam);
	}

	// Entry point calling
	auto _DllMain{ reinterpret_cast<t_DLL_ENTRY_POINT>(pBase + pOpt->AddressOfEntryPoint) };
	_DllMain(pDataImport->baseAddr, DLL_PROCESS_ATTACH, nullptr);
	pDataImport->hMod = reinterpret_cast<HINSTANCE>(pDataImport->baseAddr);
}