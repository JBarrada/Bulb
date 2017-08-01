#version 400

precision highp float;
uniform vec3 camera_eye;

uniform int de_iterations = 10;
uniform int max_ray_steps = 50;
uniform float min_distance = 0.001;

const float bailout = 2.0;
const float power = 8.0;

in vec3 camera_ray;

vec3 march_point = vec3(0.0);
vec3 light_point = vec3(-6, 2, 8);

int max_iter = 0;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


float DEMandelbox(vec3 pos) {
	float Scale = 3.0;
	float minrad2 = 0.25;
	vec4 scale = vec4(Scale, Scale, Scale, abs(Scale)) / minrad2;
	float absScalem1 = abs(Scale - 1.0);
	float AbsScaleRaisedTo1mIters = pow(abs(Scale), float(1-de_iterations));
	
	
	
	vec4 p = vec4(pos,1), p0 = p;  // p.w is the distance estimate
	
	for (int i=0; i<de_iterations; i++) {
		//p.xyz*=rot;
		p.xyz = clamp(p.xyz, -1.0, 1.0) * 2.0 - p.xyz;  // min;max;mad
		float r2 = dot(p.xyz, p.xyz);
		//if (i<ColorIterations) orbitTrap = min(orbitTrap, abs(vec4(p.xyz,r2)));
		p *= clamp(max(minrad2/r2, minrad2), 0.0, 1.0);  // dp3,div,max.sat,mul
		p = p*scale + p0;
             if ( r2>1000.0) break;
		
	}
	return ((length(p.xyz) - absScalem1) / p.w - AbsScaleRaisedTo1mIters);
}

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
		
		if (i > max_iter) {
			max_iter = i;
		}
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

vec2 ray_march(vec3 from, vec3 dir, int max_steps) {
	float total_distance = 0.0;
	float distance = 0.0;
	int steps = 0;
	
	for (steps = 0; steps < max_steps; steps++) {
		march_point = from + (total_distance * dir);
		distance = DE(march_point);
		total_distance += distance;
		if (distance < min_distance) {
			break;
		}
	}
	
	return vec2(1.0 - (float(steps) / float(max_steps)), total_distance);
}

void main() {
	vec2 ray_data = ray_march(camera_eye, camera_ray, max_ray_steps);
	float brightness = ray_data.x;
	
	vec3 color = hsv2rgb(vec3( 2.0 * (float(max_iter) / float(de_iterations)), 0.8, 0.8));
	

	if (brightness != 0.0) {
		vec3 light_ray = normalize(light_point - march_point);
		vec3 contact_point = (min_distance * light_ray) + march_point;
		float light_steps = ray_march(contact_point, light_ray, max_ray_steps / 2).x;
		
		if (light_steps != 0.0) {
			color = mix(color * 0.2, color, 1.0 - light_steps);
		}
	}
	
	
	gl_FragColor = vec4(mix(vec3(0.8), color, brightness), ray_data.y);
}
