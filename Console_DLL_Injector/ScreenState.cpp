#include "ScreenState.h"

void ScreenState::SwitchToDllScreen()
{
	bScreenProcess = false;
	bScreenDLL = true;
	bStillChoosingProc = false;
}

void ScreenState::SwitchToProcessScreen()
{
	bScreenDLL = false;
	bScreenProcess = true;
}