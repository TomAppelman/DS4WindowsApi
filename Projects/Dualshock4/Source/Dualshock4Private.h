/*

*/
#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <hidsdi.h>
#include <hidusage.h>
#include <hidpi.h>
#include <hidclass.h>

#include "Dualshock4.h"

#define CLAMP(a,b,x) (x < a)? a:(x > b) ? b: x;

// Device ids
#define DUALSHOCK4_DEVICE_PRODUCT_ID 1476
#define DUALSHOCK4V2_DEVICE_PRODUCT_ID 2508
#define DUALSHOCK4_DEVICE_VENDOR_ID 1356 

// Device flags
#define DUALSHOCK4_DEVICE_FLAG_CONNECTED 0x0001
#define DUALSHOCK4_DEVICE_FLAG_TYPE_USB	 0x0010
#define DUALSHOCK4_DEVICE_FLAG_TYPE_BT	 0x0020

//
//	Device Report
//
typedef struct _DUALSHOCK4_DEVICE_INPUT_REPORT_INFO
{
	uint8_t type;
	uint8_t offset;
	uint8_t pack_size;
	uint8_t unused;
}DUALSHOCK4_DEVICE_INPUT_REPORT_INFO;

//
//	Device Report Data
//
typedef struct _DUALSHOCK4_DEVICE_REPORT_OUT
{
	uint8_t id;
	uint8_t data[128];
	
}DUALSHOCK4_DEVICE_REPORT_OUT;


//
//	Device Info, this is a local backup of the device state
//
typedef struct _DUALSHOCK4_DEVICE_INFO
{
	uint16_t Flags;
	DUALSHOCK4_STATE State;
	DUALSHOCK4_LIGHT Light;
	DUALSHOCK4_VIBRATION Vibration;
	DUALSHOCK4_BATTERY_INFORMATION BatteryInfo;
	DUALSHOCK4_DEVICE_INPUT_REPORT_INFO ReportInfo;
}DUALSHOCK4_DEVICE_INFO;

//
//	Rawinput state, the state of the application
// this is the application interface
//
typedef struct _RAWINPUT_STATE
{
	uint32_t Initialized;
	HWND WindowHandle;
	HANDLE ThreadHandle;
	BYTE Buffer[8192];
	uint8_t DiconnectOnEmptyBattery;
	uint8_t DeviceConnectedNum;
	HANDLE DeviceHandle[DUALSHOCK4_MAX_USERS];
	HANDLE HIDHandle[DUALSHOCK4_MAX_USERS];
	DUALSHOCK4_DEVICE_INFO DeviceInfo[DUALSHOCK4_MAX_USERS];// might change this to DeviceData, makes more sense
	uint32_t ReportID[DUALSHOCK4_MAX_USERS];
}RAWINPUT_STATE;


extern RAWINPUT_STATE sRawInputState;

#if 0 
// internal device state
typedef struct _Dualshock4DeviceConnectionInfo {
	BYTE				Connected;
	BYTE				ConnectionType;
	HANDLE				ConnectionHandle;
	HANDLE				DeviceHandle;
	ReportInfo			Info;
	Report				ReportData;

}Dualshock4DeviceConnectionInfo;

typedef struct _DualShock4DeviceState {
	// gamepad state
	DUALSHOCK_STATE		State;
	DUALSHOCK_LIGHT		Light;
	DUALSHOCK_VIBRATION Vibration;
	BYTE				BatteryLevel;
	BYTE				Wired;
}DualShock4DeviceState;



// internaly used globals
extern DualShock4DeviceState			sDualShockDeviceState[DUALSHOCK_MAX_USERS];
extern Dualshock4DeviceConnectionInfo	sDualShockDeviceConnectionInfo[DUALSHOCK_MAX_USERS];
extern DUALSHOCK_LIGHT					sDualShockDefaultLightData[DUALSHOCK_MAX_USERS];
extern DUALSHOCK_LIGHT					sDualShockDefaultLightOffData[DUALSHOCK_MAX_USERS];

//
extern int DS4ParseRawInput(Dualshock4DeviceConnectionInfo* info, DualShock4DeviceState* state, char* rawdata);
extern int DS4ConnectDevice(HANDLE hDevice);
extern int DS4DisconnectDevice(HANDLE hDevice);

#endif