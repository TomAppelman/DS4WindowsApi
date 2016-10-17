/*
	C++ style code for better organisation of code;

	TODO:
	add inline macro
*/
#pragma once

#include "Dualshock4.h"

//
struct Dualshock4ApiInfo {
	enum Identifier {
		eApiVersion = DUALSHOCK4_API_VERSION,
		eMaxUsers	= DUALSHOCK4_MAX_USERS,
	};
};

//
struct Dualshock4Limits{
	enum Identifier {
		eAnalogRange	= DUALSHOCK4_ANALOG_RANGE,
		eTriggerRange	= DUALSHOCK4_TRIGGER_RANGE,
		eAnalogDeadzone = DUALSHOCK4_ANALOG_DEADZONE,
		eGyroRange		= DUALSHOCK4_GYRO_RANGE,
		eTouchWidth		= DUALSHOCK4_TOUCH_WIDTH,
		eTouchHeight	= DUALSHOCK4_TOUCH_HEIGHT,
	};
};

//
struct Dualshock4Result {
	enum Identifier {
		eSucceded			= DUALSHOCK4_SUCCEEDED,
		eInvalidParameter	= DUALSHOCK4_ERROR_INVALID_PARAMETER,
		eErrorDisconnected	= DUALSHOCK4_ERROR_DISCONNECTED,
		eErrorSendFailed	= DUALSHOCK4_ERROR_SEND_FAILED,
	};
};
struct Dualshock4Pad {
	enum Identifier {
		eUp			= DUALSHOCK4_DPAD_UP,
		eUpRight	= DUALSHOCK4_DPAD_UP_RIGHT,
		eRight		= DUALSHOCK4_DPAD_RIGHT,
		eDownRight	= DUALSHOCK4_DPAD_DOWN_RIGHT,
		eDown		= DUALSHOCK4_DPAD_DOWN,
		eDownLeft	= DUALSHOCK4_DPAD_DOWN_LEFT,
		eLeft		= DUALSHOCK4_DPAD_LEFT,
		eUpLeft		= DUALSHOCK4_DPAD_UP_LEFT,
	};
};
struct Dualshock4Button {
	enum Identifier {
		eUp			= DUALSHOCK4_BUTTON_UP,
		eRight		= DUALSHOCK4_BUTTON_RIGHT,
		eDown		= DUALSHOCK4_BUTTON_DOWN,
		eLeft		= DUALSHOCK4_BUTTON_LEFT,
		eSquare		= DUALSHOCK4_BUTTON_SQUARE,
		eCross		= DUALSHOCK4_BUTTON_CROSS,
		eCircle		= DUALSHOCK4_BUTTON_CIRCLE,
		eTriangle	= DUALSHOCK4_BUTTON_TRIANGLE,
		eL1			= DUALSHOCK4_BUTTON_L1,
		eR1			= DUALSHOCK4_BUTTON_R1,
		eL2			= DUALSHOCK4_BUTTON_L2,
		eR2			= DUALSHOCK4_BUTTON_R2,
		eShare		= DUALSHOCK4_BUTTON_SHARE,
		eOption		= DUALSHOCK4_BUTTON_OPTION,
		eL3			= DUALSHOCK4_BUTTON_L3,
		eR3			= DUALSHOCK4_BUTTON_R3,
		ePS			= DUALSHOCK4_BUTTON_PS,
	};
};

class Dualshock4State{
public:
	Dualshock4State(const unsigned int user) {
		mUser = user;
		mCurPtr = &mState[0];
		mPrevPtr = &mState[1];
	}

	bool Poll() {
		DUALSHOCK4_STATE* tmpPtr = mPrevPtr;
		mPrevPtr = mCurPtr;
		mCurPtr = tmpPtr;
		return DualShock4GetState(mUser, mCurPtr) != Dualshock4Result::eErrorDisconnected;
	}

	const uint32_t GetUserIndex()const{
		return mUser;
	}
	
	//
	uint32_t isButtonDown(const unsigned int button)const {
		return (mCurPtr->Gamepad.Buttons & button);
	}
	uint32_t isButtonUp(const unsigned int button)const {
		return !(mCurPtr->Gamepad.Buttons & button);
	}
	uint32_t isButtonPressed(const unsigned int button)const {
		return (mCurPtr->Gamepad.Buttons & button) && !(mPrevPtr->Gamepad.Buttons & button);
	}
	uint32_t isButtonReleased(const unsigned int button)const {
		return !(mCurPtr->Gamepad.Buttons & button) && (mPrevPtr->Gamepad.Buttons & button);
	}

	//
	const unsigned int PadDirection() const {
		return mCurPtr->Gamepad.Pad;
	}

	//
	const float LeftTrigger()const {
		return mCurPtr->Gamepad.LeftTrigger;
	}
	const float RightTrigger()const {
		return mCurPtr->Gamepad.RightTrigger;
	}

	//
	const float NormalizedLeftTrigger()const {
		return mCurPtr->Gamepad.LeftTrigger / (float)Dualshock4Limits::eTriggerRange;
	}
	const float NormalizedRightTrigger()const {
		return  mCurPtr->Gamepad.RightTrigger / (float)Dualshock4Limits::eTriggerRange;
	}

	// analog sticks
	const short StickLeftX()const {
		return mCurPtr->Gamepad.StickLeftX;
	}
	const short StickLeftY()const {
		return mCurPtr->Gamepad.StickLeftY;
	}
	const short StickRightX()const {
		return mCurPtr->Gamepad.StickRightX;
	}
	const short StickRightY()const {
		return mCurPtr->Gamepad.StickRightY;
	}

	// normalized -- deadzone is not used here
	const float StickLeftXNormalized()const {
		if (abs(mCurPtr->Gamepad.StickLeftX) < Dualshock4Limits::eAnalogDeadzone) {
			return 0.f;
		}
		return mCurPtr->Gamepad.StickLeftX / float(Dualshock4Limits::eAnalogRange);
	}
	const float StickLeftYNormalized()const {
		if (abs(mCurPtr->Gamepad.StickLeftY) < Dualshock4Limits::eAnalogDeadzone) {
			return 0.f;
		}
		return mCurPtr->Gamepad.StickLeftY / float(Dualshock4Limits::eAnalogRange);
	}
	const float StickRightXNormalized()const {
		if (abs(mCurPtr->Gamepad.StickRightX) < Dualshock4Limits::eAnalogDeadzone) {
			return 0.f;
		}
		return mCurPtr->Gamepad.StickRightX / float(Dualshock4Limits::eAnalogRange);
	}
	const float StickRightYNormalized()const {
		if (abs(mCurPtr->Gamepad.StickRightY) < Dualshock4Limits::eAnalogDeadzone) {
			return 0.f;
		}
		return mCurPtr->Gamepad.StickRightY / float(Dualshock4Limits::eAnalogRange);
	}

	//
	const short AccelerometerPitch()const {
		return mCurPtr->Gamepad.AccelPitch;
	}
	const short AccelerometerYaw()const {
		return mCurPtr->Gamepad.AccelYaw;
	}
	const short AccelerometerRoll()const {
		return mCurPtr->Gamepad.AccelRoll;
	}

	//
	const short GyroPitch()const {
		return mCurPtr->Gamepad.GyroPitch;
	}
	const short GyroYaw()const {
		return mCurPtr->Gamepad.GyroYaw;
	}
	const short GyroRoll()const {
		return mCurPtr->Gamepad.GyroRoll;
	}

	//
	const float NormalizedGyroX()const {
		return mCurPtr->Gamepad.GyroPitch / float(Dualshock4Limits::eGyroRange);
	}
	const float NormalizedGyroY()const {
		return mCurPtr->Gamepad.GyroYaw / float(Dualshock4Limits::eGyroRange);
	}
	const float NormalizedGyroZ()const {
		return mCurPtr->Gamepad.GyroRoll / float(Dualshock4Limits::eGyroRange);
	}

	const uint16_t TouchX(const unsigned int index = 0)const
	{
		return mCurPtr->Gamepad.TouchX[index];
	}
	const uint16_t TouchY(const unsigned int index = 0)const
	{
		return mCurPtr->Gamepad.TouchY[index];
	}

	const float TouchXNormalized(const unsigned int index = 0)const
	{
		return mCurPtr->Gamepad.TouchX[index] / float(Dualshock4Limits::eTouchWidth);
	}
	const float TouchYNormalized(const unsigned int index = 0)const
	{
		return mCurPtr->Gamepad.TouchY[index] / float(Dualshock4Limits::eTouchHeight);
	}

	const float TouchXMotionNormalized(const unsigned int index = 0)const
	{
		return mPrevPtr->Gamepad.TouchX[index] / float(Dualshock4Limits::eTouchWidth) - (mCurPtr->Gamepad.TouchX[index] / float(Dualshock4Limits::eTouchWidth));
	}
	const float TouchYMotionNormalized(const unsigned int index = 0)const
	{
		return mPrevPtr->Gamepad.TouchY[index] / float(Dualshock4Limits::eTouchWidth) - (mCurPtr->Gamepad.TouchY[index] / float(Dualshock4Limits::eTouchWidth));
	}
	

	const uint32_t TouchCount()const 
	{
		return mCurPtr->Gamepad.TouchCount;
	}

	//
	bool SetLight(const unsigned char red, const unsigned char green, const unsigned char blue) {
		DUALSHOCK4_LIGHT Light;
		Light.BlinkTime = 0;
		Light.Red = red;
		Light.Green = green;
		Light.Blue = blue;
		return DualShock4SetLight(mUser, &Light) == Dualshock4Result::eSucceded;
	}

	//
	bool SetVibration(const byte strong, const byte light) {
		DUALSHOCK4_VIBRATION	Vib = { strong, light };
		return DualShock4SetVibration(mUser, &Vib) == Dualshock4Result::eSucceded;
	}

	//move the inverse to the function used above it;
	const DWORD GetBatteryLevel() {
		DUALSHOCK4_BATTERY_INFORMATION BatteryInfo;
		DualShock4GetBatteryInfo(mUser, &BatteryInfo);
		return BatteryInfo.Level;
	}

private:
	uint32_t	 mUser;
	DUALSHOCK4_STATE	 mState[2];
	DUALSHOCK4_STATE* mCurPtr;
	DUALSHOCK4_STATE* mPrevPtr;
};