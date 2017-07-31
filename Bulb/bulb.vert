#version 400

const float PI = 3.14159265359;
const float DEG_TO_RAD = PI / 180.0;

/*
uniform vec3 camera_pos = vec3(-1.5,0,0);
vec3 camera_up;
vec3 camera_dir = vec3(1,0,0);
*/

uniform vec3 camera_eye;
uniform vec3 camera_target;
uniform vec3 camera_up;

uniform float camera_fov = 90.0;
uniform float camera_aspect = 1.0;

out vec3 camera_ray;

in vec3 VertexPosition;

mat3 look_at_3(vec3 origin, vec3 target, vec3 up) {
  vec3 ww = normalize(target - origin);
  vec3 uu = normalize(cross(ww, up));
  vec3 vv = normalize(cross(uu, ww));

  return mat3(uu, vv, ww);
}

void main(void) {
	gl_Position = vec4(VertexPosition, 1.0);
    
	mat3 camera_lookat = look_at_3(camera_eye, camera_target, camera_up);
	camera_ray = normalize(camera_lookat* vec3(VertexPosition.x, VertexPosition.y / camera_aspect, 2.0));
}