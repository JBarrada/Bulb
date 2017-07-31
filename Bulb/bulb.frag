#version 130

precision highp float;

uniform vec3 camera_eye;
/*
vec3 camera_up;
vec3 camera_at = vec3(0.0,0.0,0.0);
vec3 camera_dir = vec3(1,0,0);
float fov = 90.0;
float aspect_ratio = 1.0;
*/

int de_iterations = 10;
int max_ray_steps = 50;
const float min_distance = 0.001;

const float bailout = 2.0;
const float power = 8.0;

in vec3 camera_ray;

vec3 march_point = vec3(0.0);
vec3 light_point = vec3(-6, 2, 8);

out vec4 out_0;

float DE(vec3 z) {
	vec3 z_temp = vec3(z);
	float dr = 1.0;
	float r = 0.0;
	
	float theta, phi, zr;
	
	for (int i = 0; i < de_iterations; i++) {
		r = length(z_temp);
		
		if (r > bailout) {
			break;
		}
		
		theta = acos(z_temp.z / r);
		phi = atan(z_temp.y, z_temp.x);
		dr = pow(r, power - 1.0) * power * dr + 1.0;
		
		zr = pow(r, power);
		theta *= power;
		phi *= power;
		
		z_temp = vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)) * zr;
		z_temp += z;
	}
	
	return 0.5 * log(r) * r / dr;
}

float DECube(vec3 z) {
	vec3 d = abs(z) - vec3(0.5);

	float inside_distance = min(max(d.x, max(d.y, d.z)), 0.0);
	float outside_distance = length(max(d, 0.0));

	return inside_distance + outside_distance;
}

float DESpheres(vec3 z) {
	z.xy = mod((z.xy), 1.0) - vec2(0.5);
	return length(z) - 0.3;
}

float ray_march(vec3 from, vec3 dir) {
	float total_distance = 0.0;
	float distance = 0.0;
	int steps = 0;
	
	for (steps = 0; steps < max_ray_steps; steps++) {
		march_point = from + (total_distance * dir);
		distance = DESpheres(march_point);
		total_distance += distance;
		if (distance < min_distance) {
			break;
		}
	}
	
	return (1.0 - (float(steps) / float(max_ray_steps)));
}

void main() {
	float ray_steps = ray_march(camera_eye, camera_ray);
	float brightness = ray_steps;
	
	/*
	if (ray_steps != 0.0) {
		vec3 contact_point = (-min_distance * camera_ray) + march_point;
		vec3 light_ray = normalize(light_point - contact_point);
		float light_steps = ray_march(contact_point, light_ray);
		
		if (light_steps != 0) {
			brightness *= max((1.0 - light_steps), 0.5);
		}
	}
	*/
	
	//gl_FragColor = vec4(vec3(brightness), 1.0);
	out_0 = vec4(vec3(brightness), 1.0);
}
