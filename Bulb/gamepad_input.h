#ifndef GAMEPAD_INPUT_H
#define GAMEPAD_INPUT_H

#include <Windows.h>
#include <XInput.h>

typedef enum {
	GamePad_Button_DPAD_UP = 0,
	GamePad_Button_DPAD_DOWN = 1,
	GamePad_Button_DPAD_LEFT = 2,
	GamePad_Button_DPAD_RIGHT = 3,
	GamePad_Button_START = 4,
	GamePad_Button_BACK = 5,
	GamePad_Button_LEFT_THUMB = 6,
	GamePad_Button_RIGHT_THUMB = 7,
	GamePad_Button_LEFT_SHOULDER = 8,
	GamePad_Button_RIGHT_SHOULDER = 9,
	GamePad_Button_A = 10,
	GamePad_Button_B = 11,
	GamePad_Button_X = 12,
	GamePad_Button_Y = 13,
	GamePadButton_Max = 14
} GamePadButton;

typedef enum {
	GamePadIndex_One = 0,
	GamePadIndex_Two = 1,
	GamePadIndex_Three = 2,
	GamePadIndex_Four = 3,
} GamePadIndex;

struct GamePadState {
	bool buttons[GamePadButton_Max];
	bool buttons_last[GamePadButton_Max];
	
	float lstick_x;
	float lstick_y;

	float rstick_x;
	float rstick_y;

	float lt;
	float rt;

	void reset() {
		for (int i = 0; i < (int)GamePadButton_Max; ++i) {
			//buttons[i] = false;
			//buttons_last[i] = false;
		}
		
		lstick_x = lstick_y = 0.0;
		rstick_x = rstick_y = 0.0;

		lt = rt = 0.0;
	}

	bool pressed(GamePadButton button) {
		return (!buttons_last[button] && buttons[button]);
	}
	bool released(GamePadButton button) {
		return (buttons_last[button] && !buttons[button]);
	}
};

class GamePadXbox {
private:
	XINPUT_STATE _controllerState;
	GamePadIndex _controllerNum;
public:
	GamePadState State;

	GamePadXbox(GamePadIndex controller) {
		_controllerNum = controller;
		State.reset();
	}

	virtual ~GamePadXbox(void) {
		if (is_connected()) vibrate(0.0f, 0.0f);
	}

	bool is_connected();
	void vibrate(float, float);
	void update();
};

#endif
