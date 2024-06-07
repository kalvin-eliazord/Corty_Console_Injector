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

	SIZE_T bytesWritten;
	if (!WriteProcessMemory(this->dataProc.hProc, memAlloc, this->dataDll.path.c_str(), strlen(this->dataDll.path.c_str()) + 1, &bytesWritten) || !bytesWritten)
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

bool MemUtils::ManualMap_Start()
{
	if (!DllMap()) return false;
	if (!ImportMap()) return false;
	if (!ShellCodeMap()) return false;

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

	this->dataDll.buffer = new BYTE[this->dataDll.fSize];
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
		PAGE_EXECUTE_READWRITE));

	if (!this->memAllocProc)
	{
		this->memAllocProc = reinterpret_cast<BYTE*>(VirtualAllocEx(
			this->dataProc.hProc,
			nullptr,
			this->pe_head.opt->SizeOfImage,
			MEM_RESERVE | MEM_COMMIT,
			PAGE_EXECUTE_READWRITE));

		if (!this->memAllocProc)
		{
			std::cerr << "[-] Cannot allocate memory into the target process. \n";
			delete[] this->dataDll.buffer;
			return false;
		}
	}

	auto* currSection{  IMAGE_FIRST_SECTION(this->pe_head.nt) };
	for (UINT i{ 0 }; i < this->pe_head.file->NumberOfSections; ++currSection, ++i)
	{
		if (!currSection->SizeOfRawData)
			continue;

		auto* currAddr{ reinterpret_cast<intptr_t*>(this->memAllocProc + currSection->VirtualAddress) };
		auto* currData{ reinterpret_cast<intptr_t*>(this->dataDll.buffer + currSection->PointerToRawData) };
		SIZE_T* bytesW{nullptr};
		if (!WriteProcessMemory(this->dataProc.hProc, currAddr, currData, currSection->SizeOfRawData, bytesW) || !bytesW)
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

bool PE_Header::Init(BYTE* pModBase)
{
	constexpr int valid_EXE_id{ 0x5A4D };
	if (reinterpret_cast<IMAGE_DOS_HEADER*>(pModBase)->e_magic != valid_EXE_id)
	{
		std::cerr << "[-] The module is not a valid executable. \n";
		return false;
	}

	this->nt = reinterpret_cast<IMAGE_NT_HEADERS*>(pModBase + reinterpret_cast<IMAGE_DOS_HEADER*>(pModBase)->e_lfanew);
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

bool MemUtils::ImportMap()
{
	// Init Import Data
	DataImport dataImport{ 0 };
	dataImport._LoadLibraryA = LoadLibraryA;
	dataImport._GetProcAddress = reinterpret_cast<t_GetProcAddress>(GetProcAddress);

	memcpy(this->dataDll.buffer, &dataImport, sizeof(DataImport));
	if (!WriteProcessMemory(this->dataProc.hProc, this->memAllocProc, this->dataDll.buffer, 0x1000, nullptr))
	{
		std::cerr << "[-] Cannot write Import data into the target process. \n";
		VirtualFreeEx(this->dataProc.hProc, this->memAllocProc, 0, MEM_RELEASE);
		delete[] this->dataDll.buffer;
		return false;
	}

	delete[] this->dataDll.buffer;
	std::cout << "[+] Imports mapped. \n";
	return true;
}

bool MemUtils::ShellCodeMap()
{
	void* memAllocShell{ VirtualAllocEx(this->dataProc.hProc, nullptr, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE) };
	if (!memAllocShell)
	{
		std::cerr << "[-] Cannot allocate memory for the shellcode. \n";
		VirtualFreeEx(this->dataProc.hProc, this->memAllocProc, 0, MEM_RELEASE);
		return false;
	}

	if (!WriteProcessMemory(this->dataProc.hProc, memAllocShell, ShellLoader, 0x1000, nullptr))
	{
		std::cerr << "[-] Cannot map the shellcode into the target process. \n";
		VirtualFreeEx(this->dataProc.hProc, memAllocShell, 0, MEM_RELEASE);
		VirtualFreeEx(this->dataProc.hProc, this->memAllocProc, 0, MEM_RELEASE);
		return false;
	}

	HANDLE hThread{ CreateRemoteThread(this->dataProc.hProc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(memAllocShell), this->memAllocProc, 0, nullptr) };
	if (!hThread)
	{
		std::cerr << "[-] Shellcode's thread failed to initialize. \n";
		VirtualFreeEx(this->dataProc.hProc, memAllocShell, 0, MEM_RELEASE);
		VirtualFreeEx(this->dataProc.hProc, this->memAllocProc, 0, MEM_RELEASE);
		return false;
	}

	std::cout << "[+] Shellcode mapped. \n";

	HINSTANCE hModCheck{ NULL };
	while (!hModCheck)
	{
		DataImport dataCheck{ 0 };
		ReadProcessMemory(this->dataProc.hProc, this->memAllocProc, &dataCheck, sizeof(dataCheck), nullptr);
		hModCheck = dataCheck.hMod;
		std::cout << "[+] Checking... \r";
		Sleep(10);
	}

	std::cout << "[+] Checking done. \t";

	VirtualFreeEx(this->dataProc.hProc, memAllocShell, 0, MEM_RELEASE);
	CloseHandle(hThread);
	return true;
}

void ShellLoader(DataImport* pDataImport)
{
	if (!pDataImport) return;

	BYTE* modBase{ reinterpret_cast<BYTE*>(pDataImport) };
	auto* optHeader{ &reinterpret_cast<IMAGE_NT_HEADERS*>(modBase + reinterpret_cast<IMAGE_DOS_HEADER*>(modBase)->e_lfanew)->OptionalHeader };

	RelocationImport(modBase, optHeader);
	FixImports(modBase, pDataImport, optHeader);
	TLS_Import(modBase, optHeader);
	CallEntryPoint(modBase, pDataImport, optHeader);
}

inline void RelocationImport(BYTE* pModBase, IMAGE_OPTIONAL_HEADER* pOptHeader)
{
	BYTE* relativePrefAddr{ pModBase - pOptHeader->ImageBase };
	if (!relativePrefAddr) return;

	auto* relocBase{ reinterpret_cast<IMAGE_BASE_RELOCATION*>(pModBase + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) };

	while (relocBase->VirtualAddress)
	{
		const UINT maxEntries{ (relocBase->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD) };

		WORD* relocEntry{ reinterpret_cast<WORD*>(relocBase) + 1 };
		for (UINT i{ 0 }; i < maxEntries; ++i, ++relocEntry)
		{
			if (!RELOC_FLAG(*relocEntry)) continue;

			const UINT_PTR relocRVA{ static_cast<UINT_PTR>(((*relocEntry) & 0xFFF)) };
			UINT_PTR* relocEntryAddr{ reinterpret_cast<UINT_PTR*>(pModBase + relocBase->VirtualAddress + relocRVA) };
			*relocEntryAddr += reinterpret_cast<UINT_PTR>(relativePrefAddr);
		}
		relocBase = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<BYTE*>(relocBase) + relocBase->SizeOfBlock);
	}
}

inline void FixImports(BYTE* pModBase, DataImport* pDataImport, IMAGE_OPTIONAL_HEADER* pOptHeader)
{
	if (!pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size) return;
	auto* importDesc{ reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pModBase + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) };

	auto _LoadLibA{ pDataImport->_LoadLibraryA };
	auto _GetProcAddr{ pDataImport->_GetProcAddress };

	while (importDesc->Name)
	{
		HINSTANCE hMod{ _LoadLibA(reinterpret_cast<char*>(pModBase + importDesc->Name)) };

		ULONG_PTR* IAT{ reinterpret_cast<ULONG_PTR*>(pModBase + importDesc->OriginalFirstThunk) };
		ULONG_PTR* ILT{ reinterpret_cast<ULONG_PTR*>(pModBase + importDesc->FirstThunk) };

		if (!IAT) IAT = ILT;

		for (; *IAT; ++IAT, ++ILT)
		{
			if (IMAGE_SNAP_BY_ORDINAL(*IAT))
			{
				*ILT = _GetProcAddr(hMod, reinterpret_cast<char*>(*IAT & 0xFFFF));
			}
			else
			{
				auto* importName{ reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(pModBase + *IAT) };
				*ILT = _GetProcAddr(hMod, importName->Name);
			}
		}
		++importDesc;
	}
}

inline void TLS_Import(BYTE* pModBase, IMAGE_OPTIONAL_HEADER* pOptHeader)
{
	if (!pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
		return;

	auto* tlsBase{ reinterpret_cast<IMAGE_TLS_DIRECTORY*>(pModBase + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress) };
	auto* callBack{ reinterpret_cast<PIMAGE_TLS_CALLBACK*>(tlsBase->AddressOfCallBacks) };
	for (; callBack && *callBack; ++callBack)
		(*callBack)(pModBase, DLL_PROCESS_ATTACH, nullptr);
}

inline void CallEntryPoint(BYTE* pModBase, DataImport* pDataImport, IMAGE_OPTIONAL_HEADER* pOptHeader)
{
	auto _DllMain{ reinterpret_cast<t_DLL_ENTRY_POINT>(pModBase + pOptHeader->AddressOfEntryPoint) };
	(*_DllMain)(reinterpret_cast<HINSTANCE>(pModBase), DLL_PROCESS_ATTACH, nullptr);
	pDataImport->hMod = reinterpret_cast<HINSTANCE>(pModBase);
}