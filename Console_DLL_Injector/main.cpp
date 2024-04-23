#include "ConsolePrinter.h"
#include "ScreenState.h"
#include "MemoryUtils.h"
#include "PagesManager.h"
#include <cassert>

int main()
{
	// Get process entries
	MemoryUtils memUtils{};
	std::vector<PROCESSENTRY32> procEntryList{ memUtils.GetProcList() };
	assert(!procEntryList.empty() && "No process found.");

	PagesManager pagesManager(procEntryList);

	// First screen
	ConsolePrinter::PrintHeaderProc();
	ConsolePrinter::PrintProcess(&pagesManager);
	ConsolePrinter::PrintFooterProc(pagesManager.GetCurrentPage(), pagesManager.GetTotalPages());

	while (!GetAsyncKeyState(VK_DELETE) & 1)
	{
		if (ScreenState::bScreenProcess)
		{
			// PREVIOUS page
			if (GetAsyncKeyState(VK_F1) & 1)
			{
				pagesManager.GoPreviousPage();

				ConsolePrinter::PrintHeaderProc();

				ConsolePrinter::PrintProcess(&pagesManager);

				ConsolePrinter::PrintFooterProc(pagesManager.GetCurrentPage(), pagesManager.GetTotalPages());

			} // NEXT page
			else if (GetAsyncKeyState(VK_F2) & 1)
			{
				pagesManager.GoNextPage();

				ConsolePrinter::PrintHeaderProc();

				ConsolePrinter::PrintProcess(&pagesManager);

				ConsolePrinter::PrintFooterProc(pagesManager.GetCurrentPage(), pagesManager.GetTotalPages());

			} // REFRESH process entries
			else if (GetAsyncKeyState(VK_F5) & 1)
			{
				PagesManager pagesManager(memUtils.GetProcList());

				ConsolePrinter::PrintHeaderProc();

				ConsolePrinter::PrintProcess(&pagesManager);

				ConsolePrinter::PrintFooterProc(pagesManager.GetCurrentPage(), pagesManager.GetTotalPages());

			} // SELECT process 
			else if (GetAsyncKeyState(VK_F6) & 1)
			{
				//	bool bWorkingChoice{ false };

				//	do
				//	{
				//		int procNbChoice{};
				//		std::cin >> procNbChoice;

				//		if (procNbChoice >= 0 &&
				//			procNbChoice <= static_cast<int>(pagesManager.GetTotalProcess()))
				//		{
				//			ConsolePrinter::PrintHeaderProc();
				//			ConsolePrinter::PrintHeaderDll();
				//			std::wcout << "------> Process: \"" << pagesManager.GetProcNamesAndId()[procNbChoice].szExeFile << "\" - ID: "
				//				<< pagesManager.GetProcNamesAndId()[procNbChoice].th32ProcessID << " selected! \n";

				//			bWorkingChoice = true;
				//			procChoose = pagesManager.GetProcNamesAndId()[procNbChoice];

				//			// switching to dll selection screen
				//			ScreenState::bScreenProcess = false;
				//			ScreenState::bScreenDLL = true;
				//			ConsolePrinter::PrintDLL();
				//			ConsolePrinter::PrintFooterDll();
				//		}
				//		else
				//		{
				//			std::wcout << "Wrong entry, please retry. \r";
				//			Sleep(2000);
				//		}
				//	} while (!bWorkingChoice);
			}
		}
		else if (ScreenState::bScreenDLL)
		{
			//if (GetAsyncKeyState(VK_F5) & 1)
			//{
			//	if (ScreenState::bScreenDLL)
			//	{
			//		ConsolePrinter::PrintHeaderProc();
			//		ConsolePrinter::PrintHeaderDll();
			//		ConsolePrinter::PrintDLL();
			//		ConsolePrinter::PrintFooterDll();
			//	}
			//}

			//std::wstring noDllFoundWord{ L"There" };
			//std::wstring dllName{ memUtils.GetDllName() };
			//bool bNoDllFound{ dllName.find_first_of(L"T") != std::string::npos };

			//Sleep(5);

			//if (!bNoDllFound)
			//{
			//	std::wcout << dllName << "\r";
			//	Sleep(10000);
			//}
			//else
			//{
			//	break;
			//}

			//memUtils.InjectDllInto(pagesManager.GetProcIdChoosen());

			//// print: dll injected! 
			//std::wcout << "The Dll \"" << memUtils.GetDllName() << "\" is loaded into " <<
			//	procChoose.szExeFile << " process! \r";
		}

		Sleep(100);
	}

	return 0;
}