#include <iostream>
#include <cmath>
#include <time.h>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "shader_utils.h"
#include "gamepad_input.h"

#include "drawing_tools.h"
#include "bulb_shader.h"
#include "bulb_settings.h"

int SCREEN_W = 640;
int SCREEN_H = 480;
float ASPECT = (float)SCREEN_W / (float)SCREEN_H;

bool fullscreen = false;

GamePadXbox* pad = new GamePadXbox(GamePadIndex_One);

glm::vec3 camera_eye = glm::vec3(0.0, 4.0, 10.0);
glm::vec3 camera_target = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 camera_up = glm::vec3(0, 0, 1);

glm::vec3 camera_eye_prev = glm::vec3(camera_eye);
glm::vec3 camera_velocity_prev = glm::vec3(0.0);

glm::mat4 camera_orientation = glm::mat4(1.0);

float camera_prox = 1.0;
float camera_prox_target = 1.0;

float frames = 0;
clock_t frame_time;

DrawingTools drawing_tools;
BulbShader bulb_shader;
BulbControlSettings control_settings;
BulbSettings bulb_settings(&bulb_shader, &control_settings, &drawing_tools);

float get_avg_dist() {
	float depth_total = 0.0;
	int depth_samples = 0;
	GLfloat depth_comp;
	for (int i = 0; i < 100; i++) {
		glReadPixels(rand() % SCREEN_W, rand() % SCREEN_H, 1, 1, GL_ALPHA, GL_FLOAT, &depth_comp);
		depth_total += depth_comp;
		depth_samples++;
	}

	if (depth_samples != 0) {
		return (depth_total / (float)depth_samples);
	} else {
		return 1.0;
	}
}

void frame_counter() {
	frames++;
	float time_seconds = (float)(clock() - frame_time) / CLOCKS_PER_SEC;
	if (time_seconds > 0.25) {
		float fps = frames / time_seconds;
		printf("%0.1f\n", fps);

		frames = 0;
		frame_time = clock();
	}
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	bulb_shader.draw();

	bulb_shader.update_control_variables(camera_eye, camera_target, camera_up, control_settings.camera_fov[0], ASPECT);
	bulb_shader.update_shader_variables();

	if (bulb_settings.settings_open) bulb_settings.draw();

	glutSwapBuffers();
	
	camera_prox_target = get_avg_dist();
	float prox_delta = (camera_prox_target - camera_prox);
	if (prox_delta > 0) {
		camera_prox +=  (prox_delta / 160.0f);
	} else {
		camera_prox +=  (prox_delta / 8.0f);
	}

	//frame_counter();
}

void keyboard_down(unsigned char key, int x, int y) {
	glm::vec3 forward_direction = glm::vec3(camera_orientation * glm::vec4(0, 1, 0, 0));
	glm::vec3 left_direction = glm::vec3(camera_orientation * glm::vec4(1, 0, 0, 0));

		
	float avg_dist = glm::max(camera_prox, 0.0001f);

	float move_amount = 0.02f * avg_dist;

	if (key == 'w') {
		camera_eye -= move_amount * forward_direction;
	}
	if (key == 's') {
		camera_eye += move_amount * forward_direction;
	}
	if (key == 'a') {
		camera_eye += move_amount * left_direction;
	}
	if (key == 'd') {
		camera_eye -= move_amount * left_direction;
	}
	
	if (key == ' ') {
		camera_eye += move_amount * camera_up;
	}

	if (key == 'q') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), 0.02f, glm::vec3(0, 0, 1));
	}
	if (key == 'e') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), -0.02f, glm::vec3(0, 0, 1));
	}

	if (key == 'r') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), 0.02f, glm::vec3(-1, 0, 0));
	}
	if (key == 'f') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), -0.02f, glm::vec3(-1, 0, 0));
	}

	camera_target = camera_eye + glm::vec3(camera_orientation * glm::vec4(0, -1, 0, 0));
	camera_up = glm::vec3(camera_orientation * glm::vec4(0, 0, 1, 0));
}

float expo(float value) {
	if (value < 0 && (control_settings.control_expo_power[0] % 2 == 0)) {
		return -1.0f * pow(value, control_settings.control_expo_power[0]);
	} else {
		return pow(value, control_settings.control_expo_power[0]);
	}
}

void update_gamepad_control(GamePadState *state, bool sticks_only) {
	glm::vec3 forward_direction = glm::vec3(camera_orientation * glm::vec4(0, 1, 0, 0));
	glm::vec3 left_direction = glm::vec3(camera_orientation * glm::vec4(1, 0, 0, 0));

	float prox_speed_forward = control_settings.control_move_speed_forward[0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_lateral = control_settings.control_move_speed_lateral[0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_vertical = control_settings.control_move_speed_vertical[0] * glm::max(camera_prox, 0.0001f);

	// forward
	if (!sticks_only) {
		camera_eye -= (prox_speed_forward * (expo(state->rt) - expo(state->lt))) * forward_direction;
	}

	// vertical
	camera_eye += (prox_speed_vertical * expo(state->lstick_y)) * camera_up;

	// lateral & yaw
	if (state->buttons[GamePad_Button_LEFT_SHOULDER] || state->buttons[GamePad_Button_RIGHT_SHOULDER]) {
		camera_eye -= (prox_speed_lateral * expo(state->lstick_x)) * left_direction; // may be inverted
	} else {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_settings.control_yaw_speed[0] * expo(state->lstick_x), glm::vec3(0, 0, -1));
	}

	// pitch
	camera_orientation *= glm::rotate(glm::mat4(1.0), control_settings.control_pitch_speed[0] * expo(state->rstick_y), glm::vec3(1, 0, 0));

	// roll
	camera_orientation *= glm::rotate(glm::mat4(1.0), control_settings.control_roll_speed[0] * expo(state->rstick_x), glm::vec3(0, -1, 0));

	// update
	camera_target = camera_eye + glm::vec3(camera_orientation * glm::vec4(0, -1, 0, 0));
	camera_up = glm::vec3(camera_orientation * glm::vec4(0, 0, 1, 0));


	// velocity & accel
	glm::vec3 velocity = camera_eye - camera_eye_prev;
	camera_eye_prev = glm::vec3(camera_eye);

	float accel = length(velocity - camera_velocity_prev);
	camera_velocity_prev = glm::vec3(velocity);

	// vibrate
	if (control_settings.control_vibrate) {
		pad->vibrate(accel * 100.0f, accel * 100.0f);
	}	
}

void force_redraw(int value) {
	glutPostRedisplay();
	
	if (pad->is_connected()) {
		pad->update();

		if (bulb_settings.settings_open) {
			bulb_settings.gamepad_update(&pad->State);

			update_gamepad_control(&pad->State, true);
		} else {
			if (pad->State.pressed(GamePad_Button_START)) {
				bulb_settings.settings_open = true;
			} else if (pad->State.pressed(GamePad_Button_BACK)) {
				if (fullscreen) {
					glutReshapeWindow(800, 600);
					glutPositionWindow(0, 0);
					fullscreen = false;
				} else {
					glutFullScreen();
					fullscreen = true;
				}
			} else {
				update_gamepad_control(&pad->State, false);
			}
		}
	}

	glutTimerFunc(20, force_redraw, 0);
}

void reshape(int width, int height) {
	SCREEN_W = width;
	SCREEN_H = height;
	ASPECT = (float)SCREEN_W / (float)SCREEN_H;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	drawing_tools.reshape((float)SCREEN_W, (float)SCREEN_H);
}

int main(int argc, const char * argv[]) {
	glutInit(&argc, const_cast<char**>(argv));
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutCreateWindow("BULB");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	GLuint error = glewInit();

	glutDisplayFunc(render);
	glutTimerFunc(20, force_redraw, 0);
	glutKeyboardFunc(keyboard_down);
	glutReshapeFunc(reshape);

	glEnable(GL_BLEND);

	bulb_shader.load();

	bulb_shader.shader_variables[0].get_bulb_save_string();

	glutMainLoop();

	return 0;
}
