/*




*/
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUALSHOCK4_API_VERSION 4
#define DUALSHOCK4_MAX_USERS 4
#define DUALSHOCK4_WIRED 0
#define DUALSHOCK4_WIRELESS 1

#define DUALSHOCK4_SUCCEEDED 1
#define DUALSHOCK4_ERROR_INVALID_PARAMETER	0x0001
#define DUALSHOCK4_ERROR_DISCONNECTED		0x0002
#define DUALSHOCK4_ERROR_SEND_FAILED			0x0004
#define DUALSHOCK4_ERROR_INVALID_USER_INDEX	0x0008

#define DUALSHOCK4_ANALOG_RANGE 32767
#define DUALSHOCK4_ANALOG_DEADZONE 7849
#define DUALSHOCK4_TRIGGER_RANGE 255
#define DUALSHOCK4_GYRO_RANGE 3000
#define DUALSHOCK4_TOUCH_WIDTH 1920
#define DUALSHOCK4_TOUCH_HEIGHT 932


// Directional Pad 
#define DUALSHOCK4_DPAD_UP			0
#define DUALSHOCK4_DPAD_UP_RIGHT		1
#define DUALSHOCK4_DPAD_RIGHT		2
#define DUALSHOCK4_DPAD_DOWN_RIGHT	3
#define DUALSHOCK4_DPAD_DOWN			4
#define DUALSHOCK4_DPAD_DOWN_LEFT	5
#define DUALSHOCK4_DPAD_LEFT			6
#define DUALSHOCK4_DPAD_UP_LEFT		7

//buttons
//test
#define DUALSHOCK4_BUTTON_UP 0x00000001
#define DUALSHOCK4_BUTTON_RIGHT 0x00000002
#define DUALSHOCK4_BUTTON_DOWN 0x00000004
#define DUALSHOCK4_BUTTON_LEFT 0x00000008

// Gamepad Buttons
#define DUALSHOCK4_BUTTON_SQUARE 0x00000010
#define DUALSHOCK4_BUTTON_CROSS 0x00000020
#define DUALSHOCK4_BUTTON_CIRCLE 0x00000040
#define DUALSHOCK4_BUTTON_TRIANGLE 0x00000080
#define DUALSHOCK4_BUTTON_L1 0x00000100
#define DUALSHOCK4_BUTTON_R1 0x00000200
#define DUALSHOCK4_BUTTON_L2 0x00000400
#define DUALSHOCK4_BUTTON_R2 0x00000800
#define DUALSHOCK4_BUTTON_SHARE 0x00001000
#define DUALSHOCK4_BUTTON_OPTION 0x00002000
#define DUALSHOCK4_BUTTON_L3 0x00004000
#define DUALSHOCK4_BUTTON_R3 0x00008000
#define DUALSHOCK4_BUTTON_PS 0x00010000
#define DUALSHOCK4_BUTTON_TOUCHPAD 0x00100000


// Dualshock 4 controller data
typedef struct _DUALSHOCK4_GAMEPAD
{
	// Digital buttons
	uint16_t Pad;
	uint32_t Buttons;

	// Triggers
	uint8_t LeftTrigger;
	uint8_t RightTrigger;

	// Analog sticks
	int16_t StickLeftX;
	int16_t StickLeftY;
	int16_t StickRightX;
	int16_t StickRightY;
	
	// Gyroscope
	int16_t GyroPitch;
	int16_t GyroYaw;
	int16_t GyroRoll;

	// Accelerometer
	int16_t AccelPitch;
	int16_t AccelYaw;
	int16_t AccelRoll;

	// Touchpad
	uint16_t TouchX[2];
	uint16_t TouchY[2];
	uint16_t TouchTrackId[2];
	uint8_t	 TouchCount;
	
}DUALSHOCK4_GAMEPAD;


typedef struct _DUALSHOCK4_STATE {
	uint32_t PackageId;
	DUALSHOCK4_GAMEPAD Gamepad;
}DUALSHOCK4_STATE;

typedef struct _DUALSHOCK4_VIBRATION 
{
	uint8_t LeftMotorSpeed;
	uint8_t RightMotorSpeed;
}DUALSHOCK4_VIBRATION;

typedef struct _DUALSHOCK4_LIGHT
{
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
	uint8_t BlinkTime;
}DUALSHOCK4_LIGHT;

typedef struct _DUALSHOCK4_BATTERY_INFORMATION
{
	uint8_t Wired;
	uint8_t Level;
}DUALSHOCK4_BATTERY_INFORMATION;

//
//
//
uint32_t  Dualshock4Init();
void 	 Dualshock4Shutdown();
uint32_t DualShock4GetVersion();
uint32_t DualShock4GetNumDevices();
uint32_t Dualshock4DisconnectOnEmptyBattery(uint8_t enable);
uint32_t DualShock4GetState( uint32_t user_index, DUALSHOCK4_STATE* state );
uint32_t DualShock4GetDefaultLight( uint32_t user_index, DUALSHOCK4_LIGHT* light);
uint32_t DualShock4GetBatteryInfo( uint32_t user_index, DUALSHOCK4_BATTERY_INFORMATION* battery_info );
uint32_t DualShock4SetVibration( uint32_t user_index, DUALSHOCK4_VIBRATION* vib);
uint32_t DualShock4SetLight(uint32_t user_index, DUALSHOCK4_LIGHT* light);
uint32_t DualShock4SetLightBlink( uint32_t user_index, uint8_t time);

#ifdef __cplusplus
}
#endif