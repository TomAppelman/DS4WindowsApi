#include "Dualshock4.h"
#include <Windows.h>
#include <Xinput.h>

#define XINPUT_STATE_MAX_USESR  4
#define XINPUT_GAMEPAD_GUIDE 0x00400

typedef struct _XInputStateWrapper
{
	XINPUT_STATE State[XINPUT_STATE_MAX_USESR];
	XINPUT_VIBRATION Vib[XINPUT_STATE_MAX_USESR];
	DUALSHOCK4_STATE DS4State[XINPUT_STATE_MAX_USESR];
	BOOL XinputEnabled;
}XInputStateWrapper;

XInputStateWrapper sXinputStateWrapper = { 0 };

DWORD ParseDualshockState(DUALSHOCK4_STATE* ds4_state, XINPUT_STATE* xinput_state) {
	xinput_state->dwPacketNumber = ds4_state->PackageId;
	xinput_state->Gamepad.wButtons = 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_UP) ? XINPUT_GAMEPAD_DPAD_UP : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_DOWN) ? XINPUT_GAMEPAD_DPAD_DOWN : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_LEFT) ? XINPUT_GAMEPAD_DPAD_LEFT : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_RIGHT) ? XINPUT_GAMEPAD_DPAD_RIGHT : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_CROSS) ? XINPUT_GAMEPAD_A : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_CIRCLE) ? XINPUT_GAMEPAD_B : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_SQUARE) ? XINPUT_GAMEPAD_X : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_TRIANGLE) ? XINPUT_GAMEPAD_Y : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_L1) ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_L3) ? XINPUT_GAMEPAD_LEFT_THUMB : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_R1) ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_R3) ? XINPUT_GAMEPAD_RIGHT_THUMB : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_OPTION) ? XINPUT_GAMEPAD_START : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_SHARE) ? XINPUT_GAMEPAD_BACK : 0;
	xinput_state->Gamepad.wButtons |= (ds4_state->Gamepad.Buttons & DUALSHOCK4_BUTTON_PS) ? XINPUT_GAMEPAD_GUIDE : 0;

	xinput_state->Gamepad.bLeftTrigger = ds4_state->Gamepad.LeftTrigger;
	xinput_state->Gamepad.bRightTrigger = ds4_state->Gamepad.RightTrigger;
	xinput_state->Gamepad.sThumbLX = ds4_state->Gamepad.StickLeftX;
	xinput_state->Gamepad.sThumbLY = ds4_state->Gamepad.StickLeftY;
	xinput_state->Gamepad.sThumbRX = ds4_state->Gamepad.StickRightX;
	xinput_state->Gamepad.sThumbRY = ds4_state->Gamepad.StickRightY;

	return TRUE;
}

////
//

DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
	if (sXinputStateWrapper.XinputEnabled) {
		DWORD result = DualShock4GetState(dwUserIndex, &sXinputStateWrapper.DS4State[dwUserIndex]);
		if (result == DUALSHOCK4_ERROR_DISCONNECTED) {
			return ERROR_DEVICE_NOT_CONNECTED;
		}
	}
	ParseDualshockState(&sXinputStateWrapper.DS4State[dwUserIndex], pState);

	return ERROR_SUCCESS;
}
DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration) {
	if (pVibration == 0) {
		return ERROR_SUCCESS;
	}
	float max_value = 65535.f;
	DUALSHOCK4_VIBRATION vib = { (pVibration->wLeftMotorSpeed / max_value) * 255.f, (pVibration->wRightMotorSpeed / max_value) * 255.f };
	if (DualShock4SetVibration(dwUserIndex, &vib) == DUALSHOCK4_ERROR_DISCONNECTED) {
		return ERROR_DEVICE_NOT_CONNECTED;
	}
	sXinputStateWrapper.Vib[dwUserIndex] = *pVibration;
	return ERROR_SUCCESS;
}
DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities) {
	if (dwFlags == XINPUT_FLAG_GAMEPAD) {
		pCapabilities->Type = XINPUT_DEVTYPE_GAMEPAD;
		pCapabilities->SubType = XINPUT_DEVSUBTYPE_GAMEPAD;
		pCapabilities->Flags = XINPUT_CAPS_FFB_SUPPORTED;

		DUALSHOCK4_BATTERY_INFORMATION battery_info = { 0 };		
		DualShock4GetBatteryInfo(dwUserIndex, &battery_info);
		if (!battery_info.Wired) {
			pCapabilities->Flags = XINPUT_CAPS_WIRELESS;
		}

		XINPUT_STATE state = { 0 };
		ParseDualshockState(&sXinputStateWrapper.DS4State[dwUserIndex], &state);
		pCapabilities->Gamepad = state.Gamepad;
		pCapabilities->Vibration = sXinputStateWrapper.Vib[dwUserIndex];
	}
	return ERROR_SUCCESS;
}
VOID WINAPI XInputEnable(BOOL enable) {
	sXinputStateWrapper.XinputEnabled = enable;
}
DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid) {
	return ERROR_SUCCESS;
}
DWORD WINAPI XInputGetBatteryInformation(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation) {
	DUALSHOCK4_BATTERY_INFORMATION battery_info = { 0 };
	if (DualShock4GetBatteryInfo(dwUserIndex, &battery_info) == DUALSHOCK4_ERROR_DISCONNECTED) {
		return ERROR_DEVICE_NOT_CONNECTED;
	}
	if (devType == BATTERY_DEVTYPE_GAMEPAD) {
		if (battery_info.Level == 0) {
			pBatteryInformation->BatteryLevel = BATTERY_LEVEL_EMPTY;
		}
		if (battery_info.Level > 0 && battery_info.Level < 100) {
			pBatteryInformation->BatteryLevel = BATTERY_LEVEL_LOW;
		}
		if (battery_info.Level >= 100 || battery_info.Level < 200) {
			pBatteryInformation->BatteryLevel = BATTERY_LEVEL_MEDIUM;
		}
		if (battery_info.Level >= 200 || battery_info.Level < 255) {
			pBatteryInformation->BatteryLevel = BATTERY_LEVEL_FULL;
		}
		
		if (battery_info.Wired) {
			pBatteryInformation->BatteryType = BATTERY_TYPE_WIRED;

		}else {
			pBatteryInformation->BatteryType = BATTERY_TYPE_UNKNOWN;
		}
	}
	return ERROR_SUCCESS;
}
DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke) {
	return 0;
}
DWORD WINAPI XInputGetStateEx(DWORD dwUserIndex, XINPUT_STATE *pState) {
	return XInputGetState(dwUserIndex, pState);
}
DWORD WINAPI XInputWaitForGuideButton(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid) {
	return ERROR_SUCCESS;
}
DWORD WINAPI XInputCancelGuideButtonWait(DWORD dwUserIndex) {
	return ERROR_SUCCESS;
}
DWORD WINAPI XInputPowerOffController(DWORD dwUserIndex) {
	return ERROR_SUCCESS;
}
DWORD WINAPI XInputGetAudioDeviceIds(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId, UINT* pCaptureCount) {
	return ERROR_SUCCESS;
}
DWORD WINAPI XInputGetBaseBusInformation(DWORD dwUserIndex, struct XINPUT_BUSINFO* pBusinfo) {
	return ERROR_SUCCESS;
}
DWORD WINAPI XInputGetCapabilitiesEx(DWORD unk1, DWORD dwUserIndex, DWORD dwFlags, struct XINPUT_CAPABILITIESEX* pCapabilitiesEx) {

	return ERROR_SUCCESS;
}



//
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	UNREFERENCED_PARAMETER(lpReserved);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		Dualshock4Init();
		sXinputStateWrapper.XinputEnabled = TRUE;
		break;
	case DLL_PROCESS_DETACH:
		Dualshock4Shutdown();
		break;
	}

	return TRUE;
}