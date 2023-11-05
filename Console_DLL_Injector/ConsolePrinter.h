#pragma once
namespace ConsolePrinter
{
	void PrintIntroMsg();
	void PrintHelpMsg();
	void PrintHelpDLLMsg();
	void PrintOutroMsg(int actualPage, int pagesNb);
	void PrintProcess(int iterator, std::vector<PROCESSENTRY32> procList);
	void PrintOutroDllMsg();
	void PrintDLL();
};