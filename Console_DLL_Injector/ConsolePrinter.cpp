#include "header.h"


void ConsolePrinter::PrintIntroMsg()
{
	system("CLS");

	std::wcout << "---------------------->> CONSOLE INJECTOR by KALVIN <<---------------------------- \n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
	std::wcout << " >> /!\\ Put the DLL and the .EXE shorcut in the same folder of the injector /!\\ <<\n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintHelpMsg()
{
	std::wcout << "--------------->> (PRESS [F5] to REFRESH / [F6] to SELECT) <<--------------------- \n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintHelpDLLMsg()
{
	std::wcout << "------------------------>> (PRESS [F5] to REFRESH ) <<---------------------------- \n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintOutroMsg(int actualPage, int pagesNb)
{
	std::wcout << "------------------------------->> [PAGE " << actualPage << "-" << pagesNb << "] <<---------------------------------- \n";
	std::wcout << "--------------->> PRESS [F2] for NEXT page / [F1] for FORMER page. <<------------- \n";
}

void ConsolePrinter::PrintProcess(int iterator, std::vector<PROCESSENTRY32> procList)
{
		std::wcout << "[" << iterator << "]";

		if (iterator < 9)
			std::wcout << " . ";
		else
			std::wcout << ". ";

		std::wcout << procList[iterator].szExeFile << " -------------> [ID]: " << procList[iterator].th32ProcessID << "\n";
}

void ConsolePrinter::PrintOutroDllMsg()
{
	std::wcout << "-------------------->> [PRESS F6 TO INJECT THE DLL PRINTED!] <<------------------- \n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintDLL()
{
	std::wstring dllName{ MemManagement::GetDLLName() };

	std::wcout << "----- The directory contain: \n";
	std::wcout << "-----> " << dllName << "\n";

}
