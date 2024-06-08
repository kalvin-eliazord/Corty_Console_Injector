#include "MemUtils.h"

namespace fs = std::filesystem;

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
	if (!WriteProcessMemory(this->dataProc.hProc, memAlloc, this->dataDll.path.c_str(), strlen(this->dataDll.path.c_str()) + 1, &bytesWritten) || bytesWritten == 0)
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

void _stdcall ShellLoader(DataImport* pDataImport);

#if defined(DISABLE_OUTPUT)
#define ILog(data, ...)
#else
#define ILog(text, ...) printf(text, __VA_ARGS__);
#endif

#ifdef _WIN64
#define CURRENT_ARCH IMAGE_FILE_MACHINE_AMD64
#else
#define CURRENT_ARCH IMAGE_FILE_MACHINE_I386
#endif

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

	this->dataDll.fSize = fDll.tellg();
	if (this->dataDll.fSize < 0x1000)
	{
		std::cerr << "[-] Invalid file size. \n";
		fDll.close();
		return false;
	}

	this->dataDll.buffer = new BYTE[(UINT_PTR)this->dataDll.fSize];
	if (!this->dataDll.buffer)
	{
		std::cerr << "[-] Cannot initialize DLL buffer. \n";
		fDll.close();
		delete[] this->dataDll.buffer;
		return false;
	}

	fDll.seekg(0, std::ios::beg);
	fDll.read(reinterpret_cast<char*>(this->dataDll.buffer), this->dataDll.fSize);
	fDll.close();

	if (!this->pe_head.Init(this->dataDll.buffer))
	{
		std::cerr << "[-] Cannot initialize the DLL PE Header. \n";
		delete[] this->dataDll.buffer;
		return false;
	}

	this->memAllocProc = reinterpret_cast<BYTE*>(VirtualAllocEx(
		this->dataProc.hProc,
		&this->pe_head.opt->ImageBase,
		this->pe_head.opt->SizeOfImage,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE));

	if (!this->memAllocProc)
	{
		this->memAllocProc = reinterpret_cast<BYTE*>(VirtualAllocEx(
			this->dataProc.hProc,
			nullptr,
			this->pe_head.opt->SizeOfImage,
			MEM_RESERVE | MEM_COMMIT,
			PAGE_READWRITE));

		if (!this->memAllocProc)
		{
			std::cerr << "[-] Cannot allocate memory into the target process. \n";
			delete[] this->dataDll.buffer;
			return false;
		}
	}

	DWORD oldp = 0;
	VirtualProtectEx(this->dataProc.hProc, this->memAllocProc, this->pe_head.opt->SizeOfImage, PAGE_EXECUTE_READWRITE, &oldp);

	// PE Header mapping
	if (!WriteProcessMemory(this->dataProc.hProc, this->memAllocProc, this->dataDll.buffer, 0x1000, nullptr)) { 
		ILog("Can't write file header 0x%X\n", GetLastError());
		VirtualFreeEx(this->dataProc.hProc, this->memAllocProc, 0, MEM_RELEASE);
		delete[] this->dataDll.buffer;
		return false;
	}

	auto* currSection{ IMAGE_FIRST_SECTION(this->pe_head.nt) };
	for (UINT i{ 0 }; i < this->pe_head.file->NumberOfSections; ++currSection, ++i)
	{
		if (!currSection->SizeOfRawData)
			continue;

		auto* currAddr{ reinterpret_cast<intptr_t*>(this->memAllocProc + currSection->VirtualAddress) };
		auto* currData{ reinterpret_cast<intptr_t*>(this->dataDll.buffer + currSection->PointerToRawData) };
		SIZE_T bytesW{ 0 };
		if (!WriteProcessMemory(this->dataProc.hProc, currAddr, currData, currSection->SizeOfRawData, &bytesW) || bytesW == 0)
		{
			std::cerr << "[-] Cannot map DLL sections into the target process. \n";
			VirtualFreeEx(this->dataProc.hProc, this->memAllocProc, 0, MEM_RELEASE);
			delete[] this->dataDll.buffer;
			return false;
		}
	}

	std::cout << "[+] Dll mapped. \n";
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

#ifdef _WIN64
	if (this->file->Machine != IMAGE_FILE_MACHINE_AMD64)
	{
		std::cerr << "[-] This is not a Windows DLL. \n";
		return false;
	}
#else
	if (this->file->Machine != IMAGE_FILE_MACHINE_I386)
	{
		std::cerr << "[-] This is not a Windows DLL. \n";
		return false;
	}
#endif

	return true;
}

bool MemUtils::ImportAndShellCodeMap()
{
	auto& hProc = this->dataProc.hProc;
	auto& pSrcData = this->dataDll.buffer;
	auto& pTargetBase = this->memAllocProc;

	// START IMPORT MAPPING
	DataImport data{ 0 };
	data._LoadLibraryA = LoadLibraryA;
	data._GetProcAddress = GetProcAddress;
	data.pbase = pTargetBase;

	BYTE* MappingDataAlloc = reinterpret_cast<BYTE*>(VirtualAllocEx(hProc, nullptr, sizeof(DataImport), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	if (!MappingDataAlloc) {
		ILog("Target process mapping allocation failed (ex) 0x%X\n", GetLastError());
		VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
		return false;
	}

	if (!WriteProcessMemory(hProc, MappingDataAlloc, &data, sizeof(DataImport), nullptr)) {
		ILog("Can't write mapping 0x%X\n", GetLastError());
		VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, MappingDataAlloc, 0, MEM_RELEASE);
		return false;
	}
	//END Mapping params

	//START Shell code
	void* pShellcode = VirtualAllocEx(hProc, nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pShellcode) {
		ILog("Memory shellcode allocation failed (ex) 0x%X\n", GetLastError());
		VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, MappingDataAlloc, 0, MEM_RELEASE);
		return false;
	}

	if (!WriteProcessMemory(hProc, pShellcode, ShellLoader, 0x1000, nullptr)) {
		ILog("Can't write shellcode 0x%X\n", GetLastError());
		VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, MappingDataAlloc, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, pShellcode, 0, MEM_RELEASE);
		return false;
	}

	ILog("Mapped DLL at %p\n", pTargetBase);
	ILog("Mapping info at %p\n", MappingDataAlloc);
	ILog("Shell code at %p\n", pShellcode);

	ILog("Data allocated\n");

#ifdef _DEBUG
	ILog("My shellcode pointer %p\n", ShellLoader);
	ILog("Target point %p\n", pShellcode);
	system("pause");
#endif

	HANDLE hThread = CreateRemoteThread(hProc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(pShellcode), MappingDataAlloc, 0, nullptr);
	if (!hThread) {
		ILog("Thread creation failed 0x%X\n", GetLastError());
		VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, MappingDataAlloc, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, pShellcode, 0, MEM_RELEASE);
		return false;
	}
	CloseHandle(hThread);
	//END Shell code

	ILog("Thread created at: %p, waiting for return...\n", pShellcode);

	HINSTANCE hCheck = NULL;
	while (!hCheck) {
		DWORD exitcode = 0;
		GetExitCodeProcess(hProc, &exitcode);
		if (exitcode != STILL_ACTIVE) {
			ILog("Process crashed, exit code: %d\n", exitcode);
			return false;
		}

		DataImport data_checked{ 0 };
		ReadProcessMemory(hProc, MappingDataAlloc, &data_checked, sizeof(data_checked), nullptr);
		hCheck = data_checked.hMod;

		if (hCheck == (HINSTANCE)0x404040) {
			ILog("Wrong mapping ptr\n");
			VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
			VirtualFreeEx(hProc, MappingDataAlloc, 0, MEM_RELEASE);
			VirtualFreeEx(hProc, pShellcode, 0, MEM_RELEASE);
			return false;
		}
		else if (hCheck == (HINSTANCE)0x505050) {
			ILog("WARNING: Exception support failed!\n");
		}

		Sleep(10);
	}

	//CLEAR PE HEAD (optional)
	BYTE* emptyBuffer = (BYTE*)malloc(1024 * 1024 * 20);
	if (emptyBuffer == nullptr) {
		ILog("Unable to allocate memory\n");
		return false;
	}
	memset(emptyBuffer, 0, 1024 * 1024 * 20);

	if (!WriteProcessMemory(hProc, pShellcode, emptyBuffer, 0x1000, nullptr)) {
		ILog("WARNING: Can't clear shellcode\n");
	}
	if (!VirtualFreeEx(hProc, pShellcode, 0, MEM_RELEASE)) {
		ILog("WARNING: can't release shell code memory\n");
	}
	if (!VirtualFreeEx(hProc, MappingDataAlloc, 0, MEM_RELEASE)) {
		ILog("WARNING: can't release mapping data memory\n");
	}

	return true;
}

#define RELOC_FLAG32(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_HIGHLOW)
#define RELOC_FLAG64(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_DIR64)

#ifdef _WIN64
#define RELOC_FLAG RELOC_FLAG64
#else
#define RELOC_FLAG RELOC_FLAG32
#endif

#pragma runtime_checks( "", off )
#pragma optimize( "", off )
void _stdcall ShellLoader(DataImport* pDataImport)
{
	if (!pDataImport)
	{
		pDataImport->hMod = (HINSTANCE)0x404040;
		return;
	}

	auto* pBase{ pDataImport->pbase };
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
	_DllMain(pDataImport->pbase, DLL_PROCESS_ATTACH, nullptr);
	pDataImport->hMod = reinterpret_cast<HINSTANCE>(pDataImport->pbase);
}