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

//varying vec3 vp;
varying vec3 camera_ray;

in vec3 VertexPosition;

mat3 calcLookAtMatrix(vec3 origin, vec3 target, float roll) {
  vec3 rr = vec3(sin(roll), cos(roll), 0.0);
  vec3 ww = normalize(target - origin);
  vec3 uu = normalize(cross(ww, rr));
  vec3 vv = normalize(cross(uu, ww));

  return mat3(uu, vv, ww);
}

vec3 get_ray(float fov, float aspect, vec2 pos) {
	vec3 dir = vec3(1,0,0);
	
	float fov_2 = (fov / 2.0) * DEG_TO_RAD;
	
	float theta = atan(dir.y, dir.x) + fov_2 * (pos.x / 1.0);
	float phi = atan(length(dir.xy) / dir.z) + fov_2 * (pos.y / 1.0);
	
	vec3 ray = vec3(sin(phi)*cos(theta), sin(theta)*sin(phi), cos(phi));
	
	return normalize(ray);
}

void main(void) {
	gl_Position = vec4(VertexPosition, 1.0);
    
	mat3 camera_lookat = calcLookAtMatrix(camera_eye, camera_target, PI / 2.0);
	
	camera_ray = normalize(camera_lookat* vec3(VertexPosition.x, VertexPosition.y / camera_aspect, 2.0));
}