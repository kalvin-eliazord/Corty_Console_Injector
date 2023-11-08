#include "header.h"

int main()
{
	bool bScreenProcess{ true };
	bool bScreenDLL{ false };

	// SCREEN 1 (PROCESS LIST)
	ConsolePrinter::PrintIntroMsg();
	ConsolePrinter::PrintHelpMsg();

	std::vector<PROCESSENTRY32> procList{ MemManagement::GetProcList() };

	int pagesNb{ (static_cast<int>(procList.size()) / 20) };
	int actualPage{ 1 };

	int iProc{ 0 };
	int maxProcess{ 20 };
	PROCESSENTRY32 procChoose{ NULL };

	for (iProc; iProc < maxProcess; ++iProc)
		ConsolePrinter::PrintProcess(iProc, procList);

	ConsolePrinter::PrintOutroMsg(actualPage, pagesNb);
	
	while (!GetAsyncKeyState(VK_DELETE) & 1)
	{
		// FORMER page
		if (GetAsyncKeyState(VK_F1)& 1)
		{
			if (bScreenProcess)
			{
				ConsolePrinter::PrintIntroMsg();
				ConsolePrinter::PrintHelpMsg();

				actualPage -= 1;

				// looping to the last page.
				if (actualPage == 0)
					actualPage = pagesNb;

				// list of process printed
				maxProcess -= 20;
				iProc -= 40;

				// looping to the last page.
				if (maxProcess == 0)
					maxProcess = procList.size();

				if (actualPage != pagesNb)
				{
					while (maxProcess % 10 != 0)
						maxProcess -= 1;
				}

				iProc = maxProcess - 20;

				for (iProc; iProc < maxProcess; ++iProc)
					ConsolePrinter::PrintProcess(iProc, procList);

				//std::wcout << iProc <<" + " << maxProcess << "\n";
				ConsolePrinter::PrintOutroMsg(actualPage, pagesNb);
			}
		}

		// NEXT page
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			if (bScreenProcess)
			{
				ConsolePrinter::PrintIntroMsg();
				ConsolePrinter::PrintHelpMsg();

				actualPage += 1;

				// looping to the first page
				if (actualPage > pagesNb)
					actualPage = 1;

				// list of process printed
				maxProcess += 20;

				if (maxProcess > static_cast<int>(procList.size()) && actualPage == 1)
				{
					maxProcess = 20;
					iProc = 0;
				}

				// print maximum of process into the last page
				if (actualPage == pagesNb)
				{
					maxProcess = procList.size();
					iProc = maxProcess - 20;
				}
				for (iProc; iProc < maxProcess; ++iProc)
					ConsolePrinter::PrintProcess(iProc, procList);

				//std::wcout << iProc << " + " << maxProcess << "\n";

				ConsolePrinter::PrintOutroMsg(actualPage, pagesNb);
			}
		}

		// refresh process snapshot
		if (GetAsyncKeyState(VK_F5) & 1)
		{

			if (bScreenProcess)
			{

				procList = MemManagement::GetProcList();
				pagesNb = procList.size() / 20;

				ConsolePrinter::PrintIntroMsg();
				ConsolePrinter::PrintHelpMsg();

				iProc = 0;
				maxProcess = 20;

				for (iProc; iProc < maxProcess; ++iProc)
					ConsolePrinter::PrintProcess(iProc, procList);

				//std::wcout << iProc << " + " << maxProcess << "\n";

				ConsolePrinter::PrintOutroMsg(1, pagesNb);
			}

			if (bScreenDLL)
			{
				ConsolePrinter::PrintIntroMsg();
				ConsolePrinter::PrintHelpDLLMsg();
				ConsolePrinter::PrintDLL();
				ConsolePrinter::PrintOutroDllMsg();
			}

		}

		// select process 
		if (GetAsyncKeyState(VK_F6) & 1 && bScreenProcess)
		{
			bool bWorkingChoice{ false };

			do
			{
				int procNbChoice{};
				std::cin >> procNbChoice;

				if (procNbChoice >= 0 &&
					procNbChoice <= static_cast<int>(procList.size()))
				{
					ConsolePrinter::PrintIntroMsg();
					ConsolePrinter::PrintHelpDLLMsg();
					std::wcout << "------> Process: \"" << procList[procNbChoice].szExeFile << "\" - ID: "
						<< procList[procNbChoice].th32ProcessID << " selected! \n";

					bWorkingChoice = true;
					procChoose = procList[procNbChoice];

					// switching to dll selection screen
					bScreenProcess = false;
					bScreenDLL = true;
					ConsolePrinter::PrintDLL();
					ConsolePrinter::PrintOutroDllMsg();
				}
				else
				{
					std::wcout << "Wrong entry, please retry. \r";
					Sleep(2000);
				}
			} while (!bWorkingChoice);
		}

			if (bScreenDLL)
			{
				std::wstring noDllFoundWord{ L"There" };
				std::wstring dllName{ MemManagement::GetDllName() };
				bool bNoDllFound{ dllName.find_first_of(L"T") != std::string::npos};

				Sleep(5);

				if (!bNoDllFound)
				{
					std::wcout << dllName << "\r";
					Sleep(10000);
				}
				else
				{
					break;
				}
			}
	}

	MemManagement::InjectDllInto(procChoose);

	// print : dll injected! 
	std::wcout << "The Dll \"" << MemManagement::GetDllName() << "\" is loaded into " <<
		procChoose.szExeFile << " process! \r";

	return 0;
}