#pragma once
struct ScreenState
{
	bool bScreenDLL{ false };
	bool bScreenProcess{ true };
	bool bStillChoosingProc;

	void SwitchToDllScreen();
	void SwitchToProcessScreen();
};