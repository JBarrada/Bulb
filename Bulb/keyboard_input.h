#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

struct KeyboardState {
	bool keyboard[256];
	bool keyboard_last[256];

	bool special[256];
	bool special_last[256];
	

	void reset() {
		for (int i = 0; i < 256; ++i) {
			keyboard[i] = false;
			keyboard_last[i] = false;
			
			special[i] = false;
			special_last[i] = false;
		}
	}

	void next() {
		for (int i = 0; i < 256; ++i) {
			keyboard_last[i] = keyboard[i];
			special_last[i] = special[i];
		}
	}

	void keyboard_down(unsigned char key) {
		keyboard_last[key] = keyboard[key];
		keyboard[key] = true;
	}
	void keyboard_up(unsigned char key) {
		keyboard_last[key] = keyboard[key];
		keyboard[key] = false;
	}

	void special_down(int key) {
		special_last[key] = special[key];
		special[key] = true;
	}
	void special_up(int key) {
		special_last[key] = special[key];
		special[key] = false;
	}

	bool pressed_keyboard(int key) {
		return (!keyboard_last[key] && keyboard[key]);
	}
	bool released_keyboard(int key) {
		return (keyboard_last[key] && !keyboard[key]);
	}

	bool pressed_special(int key) {
		return (!special_last[key] && special[key]);
	}
	bool released_special(int key) {
		return (special_last[key] && !special[key]);
	}
};

#endif