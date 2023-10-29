#include "header.h"

int main()
{
	bool bScreenProcess{ true };
	bool bScreenDLL{ false };

	// SCREEN 1 (PROCESS LIST)
	ConsolePrinter::PrintIntroMsg();
	ConsolePrinter::PrintHelpMsg();

	std::vector<PROCESSENTRY32> procList{ MemManagement::GetProcList() };

	// TO DO: there is one page to print sometimes, figure out
	int pagesNb{ (static_cast<int>(procList.size()) / 20) };
	int actualPage{ 1 };

	int iProc{ 0 };
	int maxProcess{ 20 };

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
				ConsolePrinter::PrintHelpMsg();
				ConsolePrinter::PrintDLL();
			}

		}

		if (GetAsyncKeyState(VK_F6) & 1)
		{
			if (bScreenProcess)
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
						ConsolePrinter::PrintHelpMsg();
						std::wcout << "------> Process: \"" << procList[procNbChoice].szExeFile << "\" - ID: "
							<< procList[procNbChoice].th32ProcessID << " selected! \n";

						bWorkingChoice = true;
					}
					else
					{
						std::wcout << "Wrong entry, please retry. \r";
						Sleep(2000);
					}
				} while (!bWorkingChoice);

				// Screen 2 : DLL CHOICE
				bScreenProcess = false;
				bScreenDLL = true;
				ConsolePrinter::PrintDLL();
			}

			if (bScreenDLL)
			{
				bool bWorkingChoice{false};
				do
				{
					int dllNbChoice{};
					std::cin >> dllNbChoice;

					// wrong todo adapt to nb of dll
					if (dllNbChoice >= 0 &&
						dllNbChoice <= static_cast<int>(procList.size()))
					{

					}
					else
					{
						std::wcout << "Wrong entry, please retry. \r";
						Sleep(2000);
					}
				} while (!bWorkingChoice);

			}

			Sleep(5);
		}

		Sleep(20);
	}

	return 0;
}