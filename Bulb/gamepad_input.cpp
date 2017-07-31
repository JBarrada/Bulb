#include "gamepad_input.h"

bool GamePadXbox::is_connected() {
	memset(&_controllerState, 0, sizeof(XINPUT_STATE));

	DWORD Result = XInputGetState(_controllerNum, &_controllerState);

	if (Result == ERROR_SUCCESS) return true;
	else return false;
}

void GamePadXbox::vibrate(float leftmotor, float rightmotor) {
	XINPUT_VIBRATION Vibration;

	memset(&Vibration, 0, sizeof(XINPUT_VIBRATION));

	Vibration.wLeftMotorSpeed = (int)(leftmotor*65535.0);
	Vibration.wRightMotorSpeed = (int)(rightmotor*65535.0);

	XInputSetState((int)_controllerNum, &Vibration);
}

void GamePadXbox::update() {
	State.reset();
	
	if (_controllerState.Gamepad.bRightTrigger && _controllerState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		State.rt = _controllerState.Gamepad.bRightTrigger / 255.0f;
	}
	if (_controllerState.Gamepad.bLeftTrigger && _controllerState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		State.lt = _controllerState.Gamepad.bLeftTrigger / 255.0f;
	}

	for (int i = 0; i < (int)GamePadButton_Max; ++i) {
		State.buttons_last[i] = (bool)State.buttons[i];
		State.buttons[i] = false;
	}

	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START) State.buttons[GamePad_Button_START] = true;
	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) State.buttons[GamePad_Button_BACK] = true;

	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) State.buttons[GamePad_Button_DPAD_UP] = true;
	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) State.buttons[GamePad_Button_DPAD_DOWN] = true;
	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) State.buttons[GamePad_Button_DPAD_LEFT] = true;
	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) State.buttons[GamePad_Button_DPAD_RIGHT] = true;

	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) State.buttons[GamePad_Button_LEFT_THUMB] = true;
	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) State.buttons[GamePad_Button_RIGHT_THUMB] = true;

	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) State.buttons[GamePad_Button_LEFT_SHOULDER] = true;
	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) State.buttons[GamePad_Button_RIGHT_SHOULDER] = true;

	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) State.buttons[GamePad_Button_A] = true;
	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B) State.buttons[GamePad_Button_B] = true;
	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X) State.buttons[GamePad_Button_X] = true;
	if (_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) State.buttons[GamePad_Button_Y] = true;


	float lstick_x = _controllerState.Gamepad.sThumbLX;
	float lstick_y = _controllerState.Gamepad.sThumbLY;
	if ((lstick_x < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && lstick_x > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(lstick_y < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && lstick_y > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
		_controllerState.Gamepad.sThumbLX = 0;
		_controllerState.Gamepad.sThumbLY = 0;
	}

	lstick_x = _controllerState.Gamepad.sThumbLX;
	if (lstick_x) {
		State.lstick_x = lstick_x / 32768.0f;
	}
	lstick_y = _controllerState.Gamepad.sThumbLY;
	if (lstick_y) {
		State.lstick_y = lstick_y / 32768.0f;
	}

	float rstick_x = _controllerState.Gamepad.sThumbRX;
	float rstick_y = _controllerState.Gamepad.sThumbRY;
	if ((rstick_x < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && rstick_x > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
		(rstick_y < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && rstick_y > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {
		_controllerState.Gamepad.sThumbRX = 0;
		_controllerState.Gamepad.sThumbRY = 0;
	}

	rstick_x = _controllerState.Gamepad.sThumbRX;
	if (rstick_x) {
		State.rstick_x = rstick_x / 32768.0f;
	}
	rstick_y = _controllerState.Gamepad.sThumbRY;
	if (rstick_y) {
		State.rstick_y = rstick_y / 32768.0f;
	}
}