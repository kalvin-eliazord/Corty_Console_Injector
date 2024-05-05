#include "ScreenState.h"

void ScreenState::SwitchToDllScreen()
{
	bScreenProcess = false;
	bScreenDLL = true;
	bValidInput = true;
}

void ScreenState::SwitchToProcessScreen()
{
	bScreenDLL = false;
	bScreenProcess = true;
}