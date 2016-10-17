#include <cstdio>
#include <Windows.h>
#include <Dualshock4.h>
#include <Dualshock4.hpp>

HWND windowHandle = 0;
DUALSHOCK4_STATE state = { 0 };
const uint32_t user_index = 0;
Dualshock4State DS4State(0);

VOID CALLBACK TickEventProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	DS4State.Poll();
	
	// button test
	if (DS4State.isButtonPressed(Dualshock4Button::eCross))
	{
		DestroyWindow(windowHandle);
	}
	if (DS4State.isButtonPressed(Dualshock4Button::eCircle))
	{
		DS4State.SetLight(0, 0, 200);
	}
	if (DS4State.isButtonPressed(Dualshock4Button::eSquare))
	{
		DS4State.SetLight(200, 0, 0);
		DS4State.SetVibration(100, 0);
	}
	if (DS4State.isButtonReleased(Dualshock4Button::eSquare)) 
	{
		DS4State.SetVibration(0, 0);
		DS4State.SetLight(10, 0, 0);
	}

	// touchpad motion example
	static bool recordMotion = 0;
	static int motion_begin_x = 0;
	static int motion_end_x = 0;
	if (DS4State.TouchCount() == 0 && recordMotion) {
		recordMotion = 0;
		motion_end_x = DS4State.TouchX();
		int motion_dir = motion_begin_x - motion_end_x;
		//
		if (motion_dir > 0) {
			DS4State.SetLight(0, 200, 0);
		}
		if (motion_dir < 0) {
			DS4State.SetLight(200, 0, 0);
		}
	}
	if (DS4State.TouchCount() >= 1) {
		if (!recordMotion) {
			recordMotion = 1;
			motion_begin_x = DS4State.TouchX();
		}
	}

}


LRESULT CALLBACK AppMsgCallback(HWND hWnd, UINT MessageID, WPARAM wParam, LPARAM lParam)
{
	switch (MessageID) {
		default:break;
		case WM_CREATE:
			Dualshock4Init();
			printf("Dualshock 4 api for windows version %i\n", DualShock4GetVersion());
			Sleep(100);// this hold the application so the dualshock api can load its thread
			printf("Dualshock 4 num controllers found : %i\n", DualShock4GetNumDevices());
			SetTimer(hWnd, 1, 66, TickEventProc);
			break;
		case WM_DESTROY:
			Dualshock4Shutdown();
			exit(0);
			return 1;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
	}
	return DefWindowProc(hWnd, MessageID, wParam, lParam);
}

int CreateAppWnd()
{
	ATOM result = 0;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(0);
	wc.lpszClassName = L"APPWND";
	wc.lpfnWndProc = AppMsgCallback;

	result = RegisterClassEx(&wc);
	if (!result) {
		return FALSE;
	}
	windowHandle = CreateWindowEx(0, L"APPWND", L"Sample App", 0, 0, 0, 320, 240, NULL, 0, GetModuleHandle(0), 0);
	if (!windowHandle) {
		return FALSE;
	}
	UpdateWindow(windowHandle);
	ShowWindow(windowHandle, SW_SHOWDEFAULT);
	return TRUE;
}

int main(){
	MSG msg = { 0 };
	if (!CreateAppWnd()) {
		return FALSE;
	}
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	printf("end of thread\n");
	return (int)msg.wParam;
}