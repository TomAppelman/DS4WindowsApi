#include "DualShock4.h"
#include "Dualshock4Private.h"


#define VIB_MASK 0xf1
#define LED_MASK ((1 << 1) | (1 << 3) |(1 << 5) | (1 << 7))
#define LED_BLINK_MASK (1 << 2)//((1 << 1) | (1 << 2) |(1 << 5) | (1 << 7))

#pragma comment(lib, "Setupapi.lib" )
#pragma comment(lib, "Hid.lib" )

/*-----------------------------
	Dualshock api functions
--------------------------------*/
uint32_t DualShock4GetVersion()
{
	return DUALSHOCK4_API_VERSION;
}

uint32_t DualShock4GetNumDevices()
{
	return sRawInputState.DeviceConnectedNum;
}

uint32_t DualShock4GetState(uint32_t user_index, DUALSHOCK4_STATE* state)
{
	if (user_index >= DUALSHOCK4_MAX_USERS)
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (state == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (sRawInputState.DeviceInfo[user_index].Flags == 0)
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}
	memcpy(state, &sRawInputState.DeviceInfo[user_index].State, sizeof(DUALSHOCK4_STATE));
	return DUALSHOCK4_SUCCEEDED;
}

uint32_t DualShock4GetBatteryInfo(uint32_t user_index, DUALSHOCK4_BATTERY_INFORMATION* battery_info)
{
	if (user_index >= DUALSHOCK4_MAX_USERS) 
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (battery_info == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}
	battery_info->Wired = sRawInputState.DeviceInfo[user_index].BatteryInfo.Wired;
	battery_info->Level = sRawInputState.DeviceInfo[user_index].BatteryInfo.Level;
	return DUALSHOCK4_SUCCEEDED;
}


uint32_t DualShock4GetDefaultLight(uint32_t user_index, _Out_ DUALSHOCK4_LIGHT* light) 
{
	if (user_index >= DUALSHOCK4_MAX_USERS || light == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (light == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}
	memcpy(light, &sRawInputState.DeviceInfo[user_index].Light, sizeof(DUALSHOCK4_LIGHT));
	return DUALSHOCK4_SUCCEEDED;
}


//
uint32_t DualShock4SetVibration(uint32_t user_index, _Out_ DUALSHOCK4_VIBRATION* vib)
{
	DUALSHOCK4_DEVICE_REPORT_OUT* report_ptr = 0;
	DWORD byte_written = 0;
	uint32_t offset = 0;
	if (user_index >= DUALSHOCK4_MAX_USERS || vib == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (vib == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}
	
	DUALSHOCK4_DEVICE_REPORT_OUT reportOut = { 0 };
	report_ptr = &reportOut;
	offset = sRawInputState.DeviceInfo[user_index].ReportInfo.offset;

	report_ptr->id = sRawInputState.DeviceInfo[user_index].ReportInfo.type;
	report_ptr->data[offset] = VIB_MASK;
	report_ptr->data[offset + 4] = vib->LeftMotorSpeed;
	report_ptr->data[offset + 3] = vib->RightMotorSpeed;
	
	if (sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_BT) 
	{
		// if bluetooth device the first byte needs to be a different value
		report_ptr->data[0] = 128;
		if (!HidD_SetOutputReport(sRawInputState.HIDHandle[user_index], report_ptr, sizeof(DUALSHOCK4_DEVICE_REPORT_OUT)))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}
	else 
	{
		if (!WriteFile(sRawInputState.HIDHandle[user_index], report_ptr, sizeof(DUALSHOCK4_DEVICE_REPORT_OUT), &byte_written, NULL))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}

	return DUALSHOCK4_SUCCEEDED;
}

uint32_t DualShock4SetLight(uint32_t user_index, DUALSHOCK4_LIGHT* light) {
	DUALSHOCK4_DEVICE_REPORT_OUT* report_ptr = 0;
	DWORD byte_written = 0;
	uint32_t offset = 0;

	if (user_index >= DUALSHOCK4_MAX_USERS) 
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (light == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}

	DUALSHOCK4_DEVICE_REPORT_OUT reportOut = { 0 };
	report_ptr = &reportOut;
	offset = sRawInputState.DeviceInfo[user_index].ReportInfo.offset;
	report_ptr->id = sRawInputState.DeviceInfo[user_index].ReportInfo.type;
	report_ptr->data[offset] = LED_MASK ;// light->BlinkTime > 1 ? LED_BLINK_MASK : LED_MASK;

	report_ptr->data[offset + 5] = light->Red;
	report_ptr->data[offset + 6] = light->Green;
	report_ptr->data[offset + 7] = light->Blue;
	
	//report_ptr->data[offset + 8] = light->BlinkTime;
	//report_ptr->data[offset + 9] = light->BlinkTime;


	if (sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_BT)
	{
		// if bluetooth device the first byte needs to be a different value
		report_ptr->data[0] = 128;// 0x0a | 0x02;
		uint32_t size = sRawInputState.DeviceInfo[user_index].ReportInfo.pack_size;
		if (!HidD_SetOutputReport(sRawInputState.HIDHandle[user_index], report_ptr, size))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}
	else 
	{
		uint32_t size = sRawInputState.DeviceInfo[user_index].ReportInfo.pack_size;
		if (!WriteFile(sRawInputState.HIDHandle[user_index], report_ptr, size, &byte_written, NULL))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}

	return DUALSHOCK4_SUCCEEDED;
}
uint32_t Dualshock4DisconnectOnEmptyBattery(uint8_t enable){
	return sRawInputState.DiconnectOnEmptyBattery = enable;
}

uint32_t DualShock4SetLightBlink(uint32_t user_index, uint8_t time){
	DUALSHOCK4_DEVICE_REPORT_OUT* report_ptr = 0;
	DWORD byte_written = 0;
	uint32_t offset = 0;

	if (user_index >= DUALSHOCK4_MAX_USERS)
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}

	DUALSHOCK4_DEVICE_REPORT_OUT reportOut = { 0 };
	report_ptr = &reportOut;
	offset = sRawInputState.DeviceInfo[user_index].ReportInfo.offset;
	report_ptr->id = sRawInputState.DeviceInfo[user_index].ReportInfo.type;
	report_ptr->data[offset] = LED_BLINK_MASK;
	report_ptr->data[offset + 8] = time;
	report_ptr->data[offset + 9] = time;


	if (sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_BT)
	{
		// if bluetooth device the first byte needs to be a different value
		report_ptr->data[0] = 128;// 0x0a | 0x02;
		uint32_t size = sRawInputState.DeviceInfo[user_index].ReportInfo.pack_size;
		if (!HidD_SetOutputReport(sRawInputState.HIDHandle[user_index], report_ptr, size))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}
	else
	{
		uint32_t size = sRawInputState.DeviceInfo[user_index].ReportInfo.pack_size;
		if (!WriteFile(sRawInputState.HIDHandle[user_index], report_ptr, size, &byte_written, NULL))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}

	return DUALSHOCK4_SUCCEEDED;
}

