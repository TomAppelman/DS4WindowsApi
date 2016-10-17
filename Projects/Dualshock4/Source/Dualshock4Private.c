#include "Dualshock4Private.h"



// put cod below inside a c unit file
#define REPORT_TYPE_USB 0x05
#define REPORT_TYPE_BT 0x11



// global instance of the app state
RAWINPUT_STATE sRawInputState = { 0 };


/*===============================
	Dualshock 4 Lights Default
===============================*/
DUALSHOCK4_LIGHT sDualShockDefaultLightData[] =
{
	{ 0x00, 0x00, 0xFF, 0x00, },	// BLUE
	{ 0xFF, 0x00, 0x00, 0x00, },	// RED
	{ 0xFF, 0xFF, 0x00, 0x00, },	// GREEN
	{ 0xFF, 0x80, 0x80, 0x00, },	// PINK
};

DUALSHOCK4_LIGHT sDualShockDefaultLightOffData[] =
{
	{ 0x00, 0x00, 0x0a, 0x00, },	// BLUE
	{ 0x0a, 0x00, 0x00, 0x00, },	// RED
	{ 0x00, 0x0a, 0x00, 0x00, },	// GREEN
	{ 0x0f, 0x0a, 0x0a, 0x00, },	// PINK
};

/*===============================
	 Dualshock 4 Event functions
===============================*/
int DualShock4OnDeviceAdded(HANDLE hDevice) 
{
	RID_DEVICE_INFO device_info = { 0 };
	unsigned int SizeInBytes = sizeof(RID_DEVICE_INFO);

	// acquire vendor and product ids
	GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, &device_info, &SizeInBytes);

	// this part belongs to the dualshock internaly stuff
	if (device_info.hid.dwVendorId == DUALSHOCK4_DEVICE_VENDOR_ID &&
		device_info.hid.dwProductId == DUALSHOCK4_DEVICE_PRODUCT_ID)
	{
		//int controller_index = -1;/
		BOOL device_found = FALSE;
		uint32_t device_index = 0;
		for (device_index = 0; device_index < DUALSHOCK4_MAX_USERS; device_index++)
		{
			if (sRawInputState.DeviceInfo[device_index].Flags == 0) 
			{
				device_found = TRUE;
				break;
			}
		}
		if (!device_found) 
		{
			return FALSE;
		}

		// get device name for connection
		wchar_t DeviceName[250] = L"";
		unsigned int DeviceNameLenght = 250;
		GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, &DeviceName, &DeviceNameLenght);

		// create device connection
		HANDLE HIDHandle = CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		sRawInputState.DeviceHandle[device_index] = hDevice;
		sRawInputState.HIDHandle[device_index] = HIDHandle;
		sRawInputState.DeviceConnectedNum++;
		// clear the device info
		memset(&sRawInputState.DeviceInfo[device_index], 0, sizeof(DUALSHOCK4_STATE));

		// check if connection is wired or wireless
		sRawInputState.DeviceInfo[device_index].Flags = DUALSHOCK4_DEVICE_FLAG_CONNECTED;
		sRawInputState.DeviceInfo[device_index].Flags |= (DeviceName[8] == '{') ? DUALSHOCK4_DEVICE_FLAG_TYPE_BT : DUALSHOCK4_DEVICE_FLAG_TYPE_USB;

		if (sRawInputState.DeviceInfo[device_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_BT)
		{
			sRawInputState.DeviceInfo[device_index].ReportInfo.type = 0x11;
			sRawInputState.DeviceInfo[device_index].ReportInfo.offset = 2;
			sRawInputState.DeviceInfo[device_index].ReportInfo.pack_size = 78;
			sRawInputState.DeviceInfo[device_index].BatteryInfo.Wired = FALSE;
		}
		else if (sRawInputState.DeviceInfo[device_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_USB)
		{
			sRawInputState.DeviceInfo[device_index].ReportInfo.type = 0x05;
			sRawInputState.DeviceInfo[device_index].ReportInfo.offset = 0;
			sRawInputState.DeviceInfo[device_index].ReportInfo.pack_size = 64;
			sRawInputState.DeviceInfo[device_index].BatteryInfo.Wired = TRUE;
		}
		DualShock4SetLight(device_index, &sDualShockDefaultLightData[device_index]);
		DualShock4SetLightBlink(device_index, 0);
		return TRUE;
	}

	return FALSE;
}
int Dualshock4OnDeviceRemoved(HANDLE hDevice) 
{
	// look for the device with the same handle as the device that is disconnected

	for (unsigned int i = 0; i < 4; i++) {
		if (sRawInputState.DeviceHandle[i] == hDevice) {
			DUALSHOCK4_VIBRATION nullVib = { 0 };
			DUALSHOCK4_LIGHT nullLight = { 0 };
			DualShock4SetLight(i, &nullLight);
			DualShock4SetLightBlink(i, 255);
			DualShock4SetVibration(i, &nullVib);
			sRawInputState.DeviceHandle[i] = 0;
			CloseHandle(sRawInputState.HIDHandle[i]);
			sRawInputState.HIDHandle[i] = NULL;
			memset(&sRawInputState.DeviceInfo[i], 0, sizeof(DUALSHOCK4_STATE));
			sRawInputState.DeviceConnectedNum--;
			return TRUE;
		}
	}

	// device was not found
	return FALSE;
}
int Dualshock4OnDeviceInput(HRAWINPUT* RawDataInputPtr)
{
	RID_DEVICE_INFO device_info = { 0 };
	unsigned int SizeInBytes = sizeof(RID_DEVICE_INFO);
	uint32_t device_index = UINT32_MAX;
	DUALSHOCK4_STATE* device_state_ptr = 0;
	uint32_t offset = 0;	
	RAWINPUT* pRaw = (RAWINPUT*)sRawInputState.Buffer;
	uint8_t* RawDataPtr = (uint8_t*)pRaw->data.hid.bRawData;
	UINT dwSize = 0;
	UINT readSize = 0;
	
	// acquire rawinput hid device data
	GetRawInputData(*RawDataInputPtr, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	memset(&sRawInputState.Buffer, 0, sizeof(sRawInputState.Buffer));
	readSize = GetRawInputData((HRAWINPUT)*RawDataInputPtr, RID_INPUT, sRawInputState.Buffer, &dwSize, sizeof(RAWINPUTHEADER));
	if (readSize != dwSize) 
	{
		return FALSE;
	}

	if (pRaw->header.dwType == RIM_TYPEHID)
	{

		// find the controller that is related to the handle
		for (device_index = 0; device_index < 4; device_index++)
		{
			if (sRawInputState.DeviceHandle[device_index] == pRaw->header.hDevice) // && sRawInputState.DeviceInfo[i].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED
			{
				device_state_ptr = &sRawInputState.DeviceInfo[device_index].State;
				offset = sRawInputState.DeviceInfo[device_index].ReportInfo.offset;
				break;
			}
		}


		if (device_state_ptr == 0)
		{
			// wrong device or just not found
			return FALSE;
		}



		// get package id
		device_state_ptr->PackageId = (DWORD)RawDataPtr[offset + 7];

		// get dpad data
		device_state_ptr->Gamepad.Pad = (WORD)RawDataPtr[offset + 5] % 16;//? why %16

		// get buttons data
		device_state_ptr->Gamepad.Buttons = 0;
		device_state_ptr->Gamepad.Buttons |= (RawDataPtr[offset + 5] & 0xf0);
		device_state_ptr->Gamepad.Buttons |= RawDataPtr[offset + 6] << 8;
		device_state_ptr->Gamepad.Buttons |= RawDataPtr[offset + 7] & 0x1 == 1 ? DUALSHOCK4_BUTTON_PS : 0;
		device_state_ptr->Gamepad.Buttons |= ((RawDataPtr[offset + 7] & 0x2) == 2) ? DUALSHOCK4_BUTTON_TOUCHPAD : 0;

		// get analog stick data
		device_state_ptr->Gamepad.StickLeftX = (-128 + RawDataPtr[offset + 1]) * 255;
		device_state_ptr->Gamepad.StickLeftY = -(-128 + RawDataPtr[offset + 2]) * 255;
		device_state_ptr->Gamepad.StickRightX = (-128 + RawDataPtr[offset + 3]) * 255;
		device_state_ptr->Gamepad.StickRightY = -(-128 + RawDataPtr[offset + 4]) * 255;

		// get gyroscope data
		device_state_ptr->Gamepad.GyroPitch = (*(short*)&RawDataPtr[offset + 23]);
		device_state_ptr->Gamepad.GyroYaw = (*(short*)&RawDataPtr[offset + 21]);
		device_state_ptr->Gamepad.GyroRoll = (*(short*)&RawDataPtr[offset + 19]);

		// get accelerometer data
		device_state_ptr->Gamepad.AccelPitch = *(short*)&RawDataPtr[offset + 13];
		device_state_ptr->Gamepad.AccelYaw = *(short*)&RawDataPtr[offset + 15];
		device_state_ptr->Gamepad.AccelRoll = *(short*)&RawDataPtr[offset + 17];

		// get trigger data
		device_state_ptr->Gamepad.LeftTrigger = RawDataPtr[offset + 8];
		device_state_ptr->Gamepad.RightTrigger = RawDataPtr[offset + 9];


		// its a messy way to set the flags for dpag states so that multiple buttons can be teste at once
		// for example DUALSHOCK4_BUTTON_UP & DUALSHOCK4_BUTTON_CROSS
		if (device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP_LEFT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP_RIGHT)
		{
			device_state_ptr->Gamepad.Buttons |= DUALSHOCK4_BUTTON_UP;
		}

		if (device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_RIGHT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN_RIGHT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP_RIGHT)
		{
			device_state_ptr->Gamepad.Buttons |= DUALSHOCK4_BUTTON_RIGHT;
		}

		if (device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN_LEFT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN_RIGHT)
		{
			device_state_ptr->Gamepad.Buttons |= DUALSHOCK4_BUTTON_DOWN;
		}

		if (device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_LEFT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP_LEFT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN_LEFT)
		{
			device_state_ptr->Gamepad.Buttons |= DUALSHOCK4_BUTTON_LEFT;
		}
		//__u8 index = rd[30] <= 5 ? rd[30] : 5;
		sRawInputState.DeviceInfo[device_index].BatteryInfo.Level = RawDataPtr[offset + 30];// <= 5 ? RawDataPtr[offset + batcap_offset] : 5;//  (BYTE)RawDataPtr[offset + 12];


		//
		// get touchpad information
		device_state_ptr->Gamepad.TouchX[0] = RawDataPtr[offset + 36] | ((RawDataPtr[offset + 37] & 0x0f) << 8);
		device_state_ptr->Gamepad.TouchY[0] = (RawDataPtr[offset + 38]) << 4 | (RawDataPtr[offset + 37] & 0x0f);
		device_state_ptr->Gamepad.TouchX[1] = RawDataPtr[offset + 40] | ((RawDataPtr[offset + 41] & 0x0f) << 8);
		device_state_ptr->Gamepad.TouchY[1] = (RawDataPtr[offset + 42]) << 4 | (RawDataPtr[offset + 41] & 0x0f);
		device_state_ptr->Gamepad.TouchTrackId[0] = RawDataPtr[offset + 35] & 0x7F;
		device_state_ptr->Gamepad.TouchTrackId[1] = RawDataPtr[offset + 39] & 0x7F;
		device_state_ptr->Gamepad.TouchCount = 0;
		device_state_ptr->Gamepad.TouchCount += (RawDataPtr[offset + 35] >> 7) == 0 ? 1 : 0;
		device_state_ptr->Gamepad.TouchCount += (RawDataPtr[offset + 39] >> 7) == 0 ? 1 : 0;		
		

		// battery is empty
		if(sRawInputState.DiconnectOnEmptyBattery){
			if (sRawInputState.DeviceInfo[device_index].BatteryInfo.Level == 0) {
				sRawInputState.DeviceHandle[device_index] = 0;
				CloseHandle(sRawInputState.HIDHandle[device_index]);
				sRawInputState.HIDHandle[device_index] = NULL;
				memset(&sRawInputState.DeviceInfo[device_index], 0, sizeof(DUALSHOCK4_STATE));
				sRawInputState.DeviceConnectedNum--;
				memset(device_state_ptr, 0, sizeof(DUALSHOCK4_STATE));
			}
		}

		return TRUE;
	}

	return FALSE;
}
int Dualshock4InputRegister(HWND hwnd) {

	
	memset(&sRawInputState, 0, sizeof(sRawInputState));
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x05;
	Rid[0].dwFlags = RIDEV_DEVNOTIFY;
	Rid[0].hwndTarget = hwnd;
	if (!RegisterRawInputDevices(Rid, 1, sizeof(RAWINPUTDEVICE))) {
		return FALSE;
	}
	sRawInputState.Initialized = TRUE;
	sRawInputState.DiconnectOnEmptyBattery = FALSE;
	return TRUE;
}

int Dualshock4InputUnregister(HWND hwnd) {
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x05;
	Rid[0].dwFlags = RIDEV_REMOVE;
	Rid[0].hwndTarget = 0;
	RegisterRawInputDevices(Rid, 1, sizeof(RAWINPUTDEVICE));
	sRawInputState.Initialized = FALSE;
	memset(&sRawInputState, 0, sizeof(sRawInputState));
	return TRUE;
}

//
LRESULT CALLBACK Dualshock4MsgCallback(HWND hWnd, UINT MessageID, WPARAM wParam, LPARAM lParam) 
{
	switch (MessageID) {
	default:break;
	case WM_CREATE:
		Dualshock4InputRegister(hWnd);
		break;
	case WM_DESTROY:
		Dualshock4InputUnregister(hWnd);
		return 1;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_INPUT:
		Dualshock4OnDeviceInput((HRAWINPUT*)&lParam);
		break;
	case WM_INPUT_DEVICE_CHANGE:
		if (wParam == GIDC_ARRIVAL)
		{
			DualShock4OnDeviceAdded((HANDLE)lParam);
		}
		else if (wParam == GIDC_REMOVAL)
		{
			Dualshock4OnDeviceRemoved((HANDLE)lParam);
		}
		break;
	}
	return DefWindowProc(hWnd, MessageID, wParam, lParam);
}

//
int CreateDualshock4InputWindow()
{
	ATOM result = 0;
	WNDCLASSEXA wc;
	
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(0);

	wc.lpszClassName = "DS4WND_V3";
	wc.lpfnWndProc = Dualshock4MsgCallback;

 	result = RegisterClassExA(&wc);
	if (!result) {
		return FALSE;
	}
	sRawInputState.WindowHandle = CreateWindowExA(0, wc.lpszClassName, "", 0, 0, 0, 0, 0, NULL, 0, GetModuleHandle(0), 0);
	if (!sRawInputState.WindowHandle) {
		return FALSE;
	}
	UpdateWindow(sRawInputState.WindowHandle);
	ShowWindow(sRawInputState.WindowHandle, SW_HIDE);
	return TRUE;
}

//
DWORD WINAPI Dualshock4InputThread(LPVOID pParameter)
{
	MSG msg = { 0 };
	if (!CreateDualshock4InputWindow()) {
		return FALSE;
	}
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//Sleep(0);
	}
	return (int)msg.wParam;
}

//
// Global api initializer
uint32_t Dualshock4Init()
{
	// only initialize if api is not initialized already
	if(!sRawInputState.Initialized)
	{
		sRawInputState.ThreadHandle = CreateThread(0, 0, Dualshock4InputThread, 0, 0, 0);
		if (!sRawInputState.ThreadHandle)
		{
			return FALSE;
		}
	}
	return TRUE;
}

//
// Global api shutdown
void Dualshock4Shutdown() 
{
	if(sRawInputState.Initialized)
	{		
		// close thread
		CloseHandle(sRawInputState.ThreadHandle);
		WaitForSingleObject(sRawInputState.ThreadHandle, INFINITE);
		
		// disconenct all disconnect devices(or just turn down the light a bit
		DUALSHOCK4_VIBRATION nullVib = { 0 };
		for (UINT i = 0; i < DUALSHOCK4_MAX_USERS; i++)
		{
			//DualShock4SetLight(i, &sDualShockDefaultLightOffData[i]);
			//DualShock4SetVibration(i, &nullVib);
			
			DUALSHOCK4_VIBRATION nullVib = { 0 };
			DUALSHOCK4_LIGHT nullLight = { 0 };
			DualShock4SetLightBlink(i, 0);
			DualShock4SetLight(i, &sDualShockDefaultLightOffData[i]);
			
			DualShock4SetVibration(i, &nullVib);
			sRawInputState.DeviceHandle[i] = 0;
			CloseHandle(sRawInputState.HIDHandle[i]);
			sRawInputState.HIDHandle[i] = NULL;
			memset(&sRawInputState.DeviceInfo[i], 0, sizeof(DUALSHOCK4_STATE));
			sRawInputState.DeviceConnectedNum--;
		}

		// unregister rawinput listener
		//Dualshock4InputUnregister(sRawInputState.WindowHandle);

		// close window
		SendMessage(sRawInputState.WindowHandle, WM_CLOSE, 0, 0);
		UpdateWindow(sRawInputState.WindowHandle);


	}
}

//
//
//
#ifndef DUALSHOCK4_STATIC_LIB
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	UNREFERENCED_PARAMETER(lpReserved);

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hModule);
			Dualshock4Init();
			break;
		case DLL_PROCESS_DETACH:
			Dualshock4Shutdown();
			break;
	}

	return TRUE;
}
#endif