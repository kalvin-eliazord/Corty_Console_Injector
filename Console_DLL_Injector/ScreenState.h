#pragma once
struct ScreenState
{
	bool bScreenDLL{ false };
	bool bScreenProcess{ true };
	bool bValidInput;

	void SwitchToDllScreen();
	void SwitchToProcessScreen();
};