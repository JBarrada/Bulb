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

uniform int Iterations = 15; //~Fractal|default|15|0|200
uniform float Gap = 1.0; //~Fractal|default|1|0|1

uniform vec3 Rot1 = vec3(0.0); //~Fractal|default|0,0,0|-180,-180,-180|180,180,180
uniform vec3 Rot2 = vec3(0.0); //~Fractal|default|0,0,0|-180,-180,-180|180,180,180

mat3 fracRotation1;
mat3 fracRotation2;

vec4 orbitTrap = vec4(10000.0);

void init() {
	fracRotation1 = rotationMatrixXYZ(vec3(Rot1.x,0.0,0.0))*rotationMatrixXYZ(vec3(0.0,Rot1.y,0.0))*rotationMatrixXYZ(vec3(0.0,0.0,Rot1.z));
	fracRotation2 = rotationMatrixXYZ(vec3(Rot2.x,0.0,0.0))*rotationMatrixXYZ(vec3(0.0,Rot2.y,0.0))*rotationMatrixXYZ(vec3(0.0,0.0,Rot2.z));
}

float baseshape(vec3 p, float s) {
	p.yz = abs(p.yz);
	float t = 2.0 * max(0.0, dot(p.xy, vec2(-sqrt(3.0) * 0.5, 0.5)));
    p.xy -= t * vec2(-sqrt(3.0), 1.0) * 0.5;
	p.y = abs(p.y);
	
	if (p.y > p.z) p.yz = p.zy;
	
	p -= s * vec3(0.5*sqrt(3.0), 1.5, 1.5);
	
	if (p.z > p.x) p.xz = p.zx;
	
	if (p.x < 0.0) return p.x;
	
	p.yz = max(vec2(0.0), p.yz);
	return length(p);
}

float DE(vec3 p) {
	float dd = 1.0;
	for(int i = 0; i<Iterations; i++){
		p=fracRotation1*p;
		p.yz=abs(p.yz); 
		float t = 2.0 * max(0.0, dot(p.xy, vec2(-sqrt(3.0) * 0.5, 0.5)) );
		p.xy -= t*vec2(-sqrt(3.0),1.0)*0.5;
		p.y=abs(p.y);
		
		if(p.y>p.z) p.yz=p.zy;
		p.y=abs(p.y-0.5)+0.5; 
		p-=vec3(0.5*sqrt(3.0),1.5,1.5);
		
		p*=3.0;
		dd*=1.0/3.0;
		p+=vec3(0.5*sqrt(3.0),1.5,1.5);
		p=fracRotation2*p;
		orbitTrap = min(orbitTrap, abs(vec4(p.xyz,dot(p,p))));
	}
	return dd*baseshape(p,Gap);
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
uniform bool BackgroundImageEnable = false; //~Color|default|false|false|true|
uniform sampler2D BackgroundImage; //~Color|default|BulbImages\space.bmp|||
uniform float BackgroundImageDiffuse = 1.0; //~Color|default|1|0|1|
uniform float BackgroundImageSpecular = 1.0; //~Color|default|1|0|1|
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


#define PI  3.14159265358979323846264

vec3 equirectangularMap(sampler2D sampler, vec3 dir) {
	// Convert (normalized) dir to spherical coordinates.
	dir = normalize(dir);
	vec2 longlat = vec2(atan(dir.y,dir.x),acos(dir.z));
	// Normalize, and lookup in equirectangular map.
 	return texture2D(sampler, longlat/vec2(2.0*PI,PI)).xyz;
}

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
	float hDotN = max(0.0,dot(n,halfVector));

	float specular =((SpecularExp+2.0)/8.0)*pow(hDotN,SpecularExp)*(SpecularExp + (1.-SpecularExp)*pow(1.-hDotN,5.))*nDotL*Specular;
	specular = min(SpecularMax,specular);
	
	if (HardShadow>0.0) {
		shadowStrength = shadow(pos+n*eps, spotDir, eps);
		ambient = mix(ambient,0.0,HardShadow*shadowStrength);
		diffuse = mix(diffuse,0.0,HardShadow*shadowStrength);
		if (shadowStrength>0.0) specular = 0.0;
	}
	
	return (SpotLight.xyz*diffuse+CamLight.xyz*ambient+ specular*SpotLight.xyz)*color;
}

vec3 lightingImage(vec3 n, vec3 color, vec3 pos, vec3 dir, float eps, out float shadowStrength) {
	shadowStrength = 0.0;
	vec3 spotDir = vec3(sin(SpotLightDir.x*3.1415)*cos(SpotLightDir.y*3.1415/2.0), sin(SpotLightDir.y*3.1415/2.0)*sin(SpotLightDir.x*3.1415), cos(SpotLightDir.x*3.1415));
	float Shadow = 0.0;
	float ambient = max(CamLightMin,dot(-n, dir))*CamLight.w;
	vec3 reflected = -2.0*dot(dir,n)*n+dir;
	vec3 diffuse =  BackgroundImageDiffuse*equirectangularMap(BackgroundImage,n);
	vec3 specular = BackgroundImageSpecular*equirectangularMap(BackgroundImage,reflected);
	specular = min(vec3(SpecularMax),specular);

	if (Shadow>0.0) {
		// check path from pos to spotDir
		shadowStrength = 1.0-shadow(pos+n*eps, spotDir, eps);
		ambient = mix(ambient,0.0,Shadow*shadowStrength);
		diffuse = mix(diffuse,vec3(0.0),Shadow*shadowStrength);
		specular = mix(specular,vec3(0.0),Shadow*shadowStrength);
	}

	return (diffuse+CamLight.xyz*ambient)*color+specular;
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
	// return vec3(0.5) + cos(dot(c, vec3(s*Cycles))) * 0.5; // maybe optimized
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
	//vec3 backColor = BackgroundColor;
	vec3 backColor = (BackgroundImageEnable) ? equirectangularMap(BackgroundImage, dir) : BackgroundColor;
	
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
		if (BackgroundImageEnable) {
			hitColor = lightingImage(hitNormal, hitColor, hit, direction, epsModified, shadowStrength);
		} else {
			hitColor = lighting(hitNormal, hitColor, hit, direction, epsModified, shadowStrength);
		}
		
		// Fog
		float f = totalDist;
		//hitColor = mix(hitColor, backColor, 1.0-exp(-pow(Fog,4.0)*f*f));
		hitColor = mix(hitColor, BackgroundColor, 1.0-exp(-pow(Fog,4.0)*f*f));
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
