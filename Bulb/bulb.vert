#version 130

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

//varying vec3 vp;
out vec3 camera_ray;

in vec3 VertexPosition;

mat3 calcLookAtMatrix(vec3 origin, vec3 target, float roll) {
  vec3 rr = vec3(sin(roll), cos(roll), 0.0);
  vec3 ww = normalize(target - origin);
  vec3 uu = normalize(cross(ww, rr));
  vec3 vv = normalize(cross(uu, ww));

  return mat3(uu, vv, ww);
}

mat3 look_at_3(vec3 origin, vec3 target, vec3 up) {
  vec3 ww = normalize(target - origin);
  vec3 uu = normalize(cross(ww, up));
  vec3 vv = normalize(cross(uu, ww));

  return mat3(uu, vv, ww);
}

mat4 look_at(vec3 eye, vec3 center, vec3 up) {
	vec3 f = normalize(center - eye);
	vec3 s = normalize(cross(f, up));
	vec3 u = cross(s, f);

	return mat4(vec4(s, 0.0), vec4(u, 0.0), vec4(-f, 0.0), vec4(0,0,0,1));
}

void main(void) {
	gl_Position = vec4(VertexPosition, 1.0);
    
	//mat3 camera_lookat = calcLookAtMatrix(camera_eye, camera_target, 0.0);
	//camera_ray = normalize(camera_lookat* vec3(VertexPosition.x, VertexPosition.y / camera_aspect, 2.0));

	mat3 camera_lookat = look_at_3(camera_eye, camera_target, camera_up);
	camera_ray = normalize(camera_lookat* vec3(VertexPosition.x, VertexPosition.y / camera_aspect, 2.0));

	//mat4 camera_lookat = look_at(camera_eye, camera_target, camera_up);	
	//camera_ray = normalize((camera_lookat * vec4(VertexPosition.x, VertexPosition.y / camera_aspect, 2.0, 1.0)).xyz);
}