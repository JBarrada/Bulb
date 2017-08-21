#version 400

// Standard matrices

// Return rotation matrix for rotating around vector v by angle
mat3  rotationMatrix3(vec3 v, float angle)
{
	float c = cos(radians(angle));
	float s = sin(radians(angle));
	
	return mat3(c + (1.0 - c) * v.x * v.x, (1.0 - c) * v.x * v.y - s * v.z, (1.0 - c) * v.x * v.z + s * v.y,
		(1.0 - c) * v.x * v.y + s * v.z, c + (1.0 - c) * v.y * v.y, (1.0 - c) * v.y * v.z - s * v.x,
		(1.0 - c) * v.x * v.z - s * v.y, (1.0 - c) * v.y * v.z + s * v.x, c + (1.0 - c) * v.z * v.z
		);
}

mat3 rotationMatrixXYZ(vec3 v) {
	return rotationMatrix3(vec3(1.0,0.0,0.0), v.x)*
	rotationMatrix3(vec3(0.0,1.0,0.0), v.y)*
	rotationMatrix3(vec3(0.0,0.0,1.0), v.z);
}

// Return rotation matrix for rotating around vector v by angle
mat4  rotationMatrix(vec3 v, float angle)
{
	float c = cos(radians(angle));
	float s = sin(radians(angle));
	
	return mat4(c + (1.0 - c) * v.x * v.x, (1.0 - c) * v.x * v.y - s * v.z, (1.0 - c) * v.x * v.z + s * v.y, 0.0,
		(1.0 - c) * v.x * v.y + s * v.z, c + (1.0 - c) * v.y * v.y, (1.0 - c) * v.y * v.z - s * v.x, 0.0,
		(1.0 - c) * v.x * v.z - s * v.y, (1.0 - c) * v.y * v.z + s * v.x, c + (1.0 - c) * v.z * v.z, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

mat4 translate(vec3 v) {
	return mat4(1.0,0.0,0.0,0.0,
		0.0,1.0,0.0,0.0,
		0.0,0.0,1.0,0.0,
		v.x,v.y,v.z,1.0);
}

mat4 scale4(float s) {
	return mat4(s,0.0,0.0,0.0,
		0.0,s,0.0,0.0,
		0.0,0.0,s,0.0,
		0.0,0.0,0.0,1.0);
}

//FRACTAL_FILE

uniform float Scale = 1.5; //~Fractal|default|1.5|0|5
uniform int Iterations = 10; //~Fractal|default|10|0|200

uniform vec3 Offset = vec3(0.0,1.0,1.0); //~Fractal|default|0,1,1|-1,-1,-1|1,1,1
uniform vec3 Offset2 = vec3(1.0,-0.3,-0.3); //~Fractal|default|1.0,-0.3,-0.3|-1,-1,-1|1,1,1
uniform float Qube = 0.1; //~Fractal|default|0.1|-1|1

uniform float Angle1 = 0.0; //~Fractal|default|0|-180|180
uniform vec3 Rot1 = vec3(1.0,1.0,1.0); //~Fractal|default|1,1,1|-1,-1,-1|1,1,1

mat3 fracRotation1;

vec4 orbitTrap = vec4(10000.0);

void init() {
	fracRotation1 = Scale * rotationMatrix3(normalize(Rot1), Angle1);
}

float DE(vec3 z) {
	float t; int n = 0;
	float scalep = 1;

	vec3 z0=z;
	z = abs(z);
	//z -= (1,1,1);
	if (z.y>z.x) z.xy =z.yx;
	if (z.z>z.x) z.xz = z.zx;
	if (z.y>z.x) z.xy =z.yx;
	float DE1 =1.0-z.x;
	z = z0;
	// Folds.
	//Dodecahedral
	while (n < Iterations) {
		z *= fracRotation1;
		z = abs(z);
		z -= Offset;
		if (z.y>z.x) z.xy =z.yx;
		if (z.z>z.x) z.xz = z.zx;
		if (z.y>z.x) z.xy =z.yx;
		z -= Offset2;
		if (z.y>z.x) z.xy =z.yx;
		if (z.z>z.x) z.xz = z.zx;
		if (z.y>z.x) z.xy =z.yx;

		n++;  scalep *= Scale;
		DE1 = abs(min(Qube/n-DE1,(+z.x)/scalep));
	}
	
	return DE1;
}

out vec4 frag_color;

// Camera
uniform vec3 camera_eye;
in vec3 camera_ray;


// Raytrace
uniform float Dither = 0.1; //~Raytrace|default|0.1|0|1|
uniform float Detail = -2.5; //~Raytrace|default|-2.5|-7|0|
uniform float DetailAO = -0.5; //~Raytrace|default|-0.5|-7|0|

float minDist = pow(10.0, Detail); 
float aoEps = pow(10.0, DetailAO);
float MaxDistance = 100000.0;

uniform int MaxRaySteps = 110; //~Raytrace|default|110|0|500|
uniform float FudgeFactor = 1.0; //~Raytrace|default|1|0|1|

uniform float NormalBackStep = 1.0; //~Raytrace|default|1|0|10|


// Color
uniform vec3 BackgroundColor = vec3(0.8); //~Color|color|0.8,0.8,0.8|0,0,0|1,1,1|
uniform vec3 BaseColor = vec3(0.5, 0.1, 0.7); //~Color|color|0.5,0.1,0.7|0,0,0|1,1,1|

uniform float OrbitStrength = 0.5; //~Color|default|0.5|0|1|

uniform bool CycleColors = false; //~Color|default|false|false|true|
uniform float Cycles = 1.1; //~Color|default|1.1|0.1|30|

uniform vec4 X = vec4(0.5,0.6,0.6,0.7); //~Color|color4|0.5,0.6,0.6,0.7|0,0,0,-1|1,1,1,1|
uniform vec4 Y = vec4(1.0,0.6,0.0,0.4); //~Color|color4|1.0,0.6,0.0,0.4|0,0,0,-1|1,1,1,1|
uniform vec4 Z = vec4(0.8,0.78,1.0,0.5); //~Color|color4|0.8,0.78,1.0,0.5|0,0,0,-1|1,1,1,1|
uniform vec4 R = vec4(0.4,0.7,1.0,0.12); //~Color|color4|0.4,0.7,1.0,0.12|0,0,0,-1|1,1,1,1|


// Lighting
uniform vec2 SpotLightDir = vec2(0.1, 0.1); //~Lighting|default|0.1,0.1|-1,-1|1,1|
uniform vec4 SpotLight = vec4(1,1,1,0.4); //~Lighting|color4|1,1,1,0.4|0,0,0,0|1,1,1,1|

uniform vec4 CamLight = vec4(1.0); //~Lighting|color4|1,1,1,1|0,0,0,0|1,1,1,2|
uniform float CamLightMin = 0.5; //~Lighting|default|0.5|0|1|

uniform float ShadowSoft = 2.0; //~Lighting|default|2|0|20|
uniform float HardShadow = 0.5; //~Lighting|default|0.5|0|1|

uniform float Specular = 0.4; //~Lighting|default|0.4|0|1|
uniform float SpecularExp = 16.0; //~Lighting|default|16|0|100|
uniform float SpecularMax = 10.0; //~Lighting|default|10|0|100|

uniform vec4 AO = vec4(0.0,0.0,0.0,0.7); //~Lighting|color4|0,0,0,0.7|0,0,0,0|1,1,1,1|

uniform float Fog = 0.4; //~Lighting|default|0.4|0|2|


float fSteps = 0.0;

float DEF(vec3 p) {
	fSteps++;
	return DE(p);
}

float shadow(vec3 pos, vec3 sdir, float eps) {
	float totalDist =2.0*eps;
	float s = 1.0;
	for (int steps=0; steps < (MaxRaySteps / 2) && totalDist < MaxDistance; steps++) {
		vec3 p = pos + totalDist * sdir;
		float dist = DE(p);
		if (dist < eps)  return 1.0;
		s = min(s, ShadowSoft*pow((dist/totalDist),0.5));
		totalDist += dist;
	}
	return 1.0-s;
}

vec3 lighting(vec3 n, vec3 color, vec3 pos, vec3 dir, float eps, out float shadowStrength) {
	shadowStrength = 0.0;
	vec3 spotDir = vec3(sin(SpotLightDir.x*3.1415)*cos(SpotLightDir.y*3.1415/2.0), sin(SpotLightDir.y*3.1415/2.0)*sin(SpotLightDir.x*3.1415), cos(SpotLightDir.x*3.1415));
	spotDir = normalize(spotDir);
	
	float nDotL = max(0.0,dot(n,spotDir));
	vec3 halfVector = normalize(-dir+spotDir);
	float diffuse = nDotL*SpotLight.w;
	float ambient = max(CamLightMin,dot(-n, dir))*CamLight.w;
	float hDotN = max(0.,dot(n,halfVector));

	float specular =((SpecularExp+2.)/8.)*pow(hDotN,SpecularExp)*
	(SpecularExp + (1.-SpecularExp)*pow(1.-hDotN,5.))*
	nDotL*Specular;
	specular = min(SpecularMax,specular);
	
	if (HardShadow>0.0) {
		shadowStrength = shadow(pos+n*eps, spotDir, eps);
		ambient = mix(ambient,0.0,HardShadow*shadowStrength);
		diffuse = mix(diffuse,0.0,HardShadow*shadowStrength);
		if (shadowStrength>0.0) specular = 0.0;
	}
	
	return (SpotLight.xyz*diffuse+CamLight.xyz*ambient+ specular*SpotLight.xyz)*color;
}

float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float ambientOcclusion(vec3 p, vec3 n) {
	float ao = 0.0;
	float de = DEF(p);
	float wSum = 0.0;
	float w = 1.0;
	float d = 1.0-(Dither*rand(p.xy));
	for (float i =1.0; i <6.0; i++) {
		float D = (DEF(p+ d*n*i*i*aoEps) -de)/(d*i*i*aoEps);
		w *= 0.6;
		ao += w*clamp(1.0-D,0.0,1.0);
		wSum += w;
	}
	return clamp(AO.w*ao/wSum, 0.0, 1.0);
}

vec3 cycle(vec3 c, float s) {
	return vec3(0.5)+0.5*vec3(cos(s*Cycles+c.x),cos(s*Cycles+c.y),cos(s*Cycles+c.z));
}

vec3 getColor() {
	orbitTrap.w = sqrt(orbitTrap.w);
	
	vec3 orbitColor;
	if (CycleColors) {
		orbitColor = cycle(X.xyz,orbitTrap.x)*X.w*orbitTrap.x +
		cycle(Y.xyz,orbitTrap.y)*Y.w*orbitTrap.y +
		cycle(Z.xyz,orbitTrap.z)*Z.w*orbitTrap.z +
		cycle(R.xyz,orbitTrap.w)*R.w*orbitTrap.w;
	} else {
		orbitColor = X.xyz*X.w*orbitTrap.x +
		Y.xyz*Y.w*orbitTrap.y +
		Z.xyz*Z.w*orbitTrap.z +
		R.xyz*R.w*orbitTrap.w;
	}
	
	vec3 color = mix(BaseColor, 3.0*orbitColor,  OrbitStrength);
	return color;
}

vec3 normal(vec3 pos, float normalDistance) {
	normalDistance = max(normalDistance*0.5, 1.0e-7);
	vec3 e = vec3(0.0,normalDistance,0.0);
	vec3 n = vec3(DE(pos+e.yxx)-DE(pos-e.yxx),
		DE(pos+e.xyx)-DE(pos-e.xyx),
		DE(pos+e.xxy)-DE(pos-e.xxy));
	n =  normalize(n);
	return n;
}

vec4 trace(vec3 from, vec3 dir, inout vec3 hit, inout vec3 hitNormal) {
	hit = vec3(0.0);
	orbitTrap = vec4(10000.0);
	vec3 direction = normalize(dir);

	float dist = 0.0;
	float totalDist = 0.0;
	
	int steps;
	
	float eps = minDist;
	float epsModified = 0.0;
	
	for (steps=0; steps<MaxRaySteps; steps++) {
		orbitTrap = vec4(10000.0);
		vec3 p = from + totalDist * direction;
		dist = DEF(p);
		dist *= FudgeFactor;
		
		if (steps == 0) { 
			dist*=(Dither*rand(direction.xy))+(1.0-Dither);
		}
		totalDist += dist;
		epsModified = pow(totalDist, 1.0)*eps;
		if (dist < epsModified) {
			totalDist -= (epsModified-dist);
			break;
		}
		if (totalDist > MaxDistance) {
			fSteps -= (totalDist-MaxDistance)/dist;
			break;
		}
	}

	vec3 hitColor;
	float stepFactor = clamp(fSteps / 20.0,0.0,1.0);
	vec3 backColor = BackgroundColor;
	
	if (steps == MaxRaySteps) orbitTrap = vec4(0.0);
	
	float shadowStrength = 0.0;
	if (dist < epsModified) {
		hit = from + totalDist * direction;
		float ao = AO.w*stepFactor;
		
		// Orbit coloring
		hitColor = getColor();
		
		// Lighting
		if (DetailAO < 0.0) {
			ao = ambientOcclusion(hit, hitNormal);
		}
		hitColor = mix(hitColor, AO.xyz, ao);
		hitNormal= normal(hit-NormalBackStep*epsModified*direction, epsModified);
		hitColor = lighting(hitNormal, hitColor, hit, direction, epsModified, shadowStrength);
		
		// Fog
		float f = totalDist;
		hitColor = mix(hitColor, backColor, 1.0-exp(-pow(Fog,4.0)*f*f));
	} else {
		hitColor = backColor;
		hitNormal = vec3(0.0);
	}
	
	return vec4(hitColor, totalDist);
}

void main() {
	init(); // some fractals require init
	
	vec3 hit_point;
	vec3 hit_normal;
	//gl_FragColor = trace(camera_eye, camera_ray, hit_point, hit_normal);
	frag_color = trace(camera_eye, camera_ray, hit_point, hit_normal);
}
