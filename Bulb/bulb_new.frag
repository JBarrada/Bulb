#version 400

uniform vec3 camera_eye;

in vec3 camera_ray;


vec4 orbitTrap = vec4(10000.0);
uniform float OrbitStrength = 0.5;

uniform float Dither = 0.1;
uniform float Detail = -2.5; // may remove
uniform float DetailAO = -0.5;

float minDist = pow(10.0, Detail);
float aoEps = pow(10.0, DetailAO);
float MaxDistance = 100000.0;

uniform int MaxRaySteps = 110;
uniform float FudgeFactor = 1.0;

uniform float NormalBackStep = 1.0;

uniform vec3 BackgroundColor = vec3(0.8);
uniform vec3 BaseColor = vec3(0.5, 0.1, 0.7);

uniform bool CycleColors = false;
uniform float Cycles = 1.1;

uniform vec4 X = vec4(0.5,0.6,0.6,0.7);
uniform vec4 Y = vec4(1.0,0.6,0.0,0.4);
uniform vec4 Z = vec4(0.8,0.78,1.0,0.5);
uniform vec4 R = vec4(0.4,0.7,1.0,0.12);


uniform vec2 SpotLightDir = vec2(0.1, 0.1);
uniform vec4 SpotLight = vec4(1,1,1,0.4);

uniform vec4 CamLight = vec4(1.0);
uniform float CamLightMin = 0.0;

uniform float ShadowSoft = 2.0;
uniform float HardShadow = 0.5;

uniform float Specular = 0.4;
uniform float SpecularExp = 16.0;
uniform float SpecularMax = 10;


uniform float Fog = 0.5;
uniform vec4 AO = vec4(0,0,0,0.7);


// bulb
uniform float Power = 8.0;
uniform int Iterations = 10;
uniform int ColorIterations = 3;
uniform float Bailout = 5.0;

// box
uniform float MinRad2 = 0.25;
uniform float Scale = 3.0;
vec4 scale = vec4(Scale, Scale, Scale, abs(Scale)) / MinRad2;
float absScalem1 = abs(Scale - 1.0);
float AbsScaleRaisedTo1mIters = pow(abs(Scale), float(1-Iterations));

float DE(vec3 pos) {
	vec4 p = vec4(pos,1), p0 = p;  // p.w is the distance estimate
	
	for (int i=0; i<Iterations; i++) {
		//p.xyz*=rot;
		p.xyz = clamp(p.xyz, -1.0, 1.0) * 2.0 - p.xyz;  // min;max;mad
		float r2 = dot(p.xyz, p.xyz);
		if (i<ColorIterations) orbitTrap = min(orbitTrap, abs(vec4(p.xyz,r2)));
		p *= clamp(max(MinRad2/r2, MinRad2), 0.0, 1.0);  // dp3,div,max.sat,mul
		p = p*scale + p0;
             if ( r2>1000.0) break;
		
	}
	return ((length(p.xyz) - absScalem1) / p.w - AbsScaleRaisedTo1mIters);
}


void powN1(inout vec3 z, float r, inout float dr) {
	// extract polar coordinates
	float theta = acos(z.z/r);
	float phi = atan(z.y,z.x);
	dr =  pow( r, Power-1.0)*Power*dr + 1.0;
	
	// scale and rotate the point
	float zr = pow( r,Power);
	theta = theta*Power;
	phi = phi*Power;
	
	// convert back to cartesian coordinates
	z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
}

float DEBulb(vec3 pos) {
	vec3 z=pos;
	float r;
	float dr=1.0;
	int i=0;
	r=length(z);
	while(r<Bailout && (i<Iterations)) {
		powN1(z,r,dr);
		z += pos;
		r = length(z);
		if (i<ColorIterations) orbitTrap = min(orbitTrap, abs(vec4(z.x,z.y,z.z,r*r)));
		i++;
	}
	return 0.5*log(r)*r/dr;
}


float shadow(vec3 pos, vec3 sdir, float eps) {
	float totalDist =2.0*eps;
	float s = 1.0;
	for (int steps=0; steps<(MaxRaySteps/2.0) && totalDist<MaxDistance; steps++) {
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
	
	// An attempt at Physcical Based Specular Shading:
	// http://renderwonk.com/publications/s2010-shading-course/
	// (Blinn-Phong with Schickl term and physical normalization)
	float specular =((SpecularExp+2.)/8.)*pow(hDotN,SpecularExp)*
	(SpecularExp + (1.-SpecularExp)*pow(1.-hDotN,5.))*
	nDotL*Specular;
	specular = min(SpecularMax,specular);
	
	if (HardShadow>0.0) {
		// check path from pos to spotDir
		shadowStrength = shadow(pos+n*eps, spotDir, eps);
		ambient = mix(ambient,0.0,HardShadow*shadowStrength);
		diffuse = mix(diffuse,0.0,HardShadow*shadowStrength);
		// specular = mix(specular,0.0,HardShadow*f);
		if (shadowStrength>0.0) specular = 0.0; // always turn off specular, if blocked
	}
	
	return (SpotLight.xyz*diffuse+CamLight.xyz*ambient+ specular*SpotLight.xyz)*color;
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

float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 trace(vec3 from, vec3 dir, inout vec3 hit, inout vec3 hitNormal) {
	hit = vec3(0.0);
	orbitTrap = vec4(10000.0);
	vec3 direction = normalize(dir);
	//floorHit = false;
	//floorDist = 0.0;
	
	float dist = 0.0;
	float totalDist = 0.0;
	
	int steps;
	//colorBase = vec3(0.0,0.0,0.0);
	
	float eps = minDist;
	float epsModified = 0.0;
	
	for (steps=0; steps<MaxRaySteps; steps++) {
		orbitTrap = vec4(10000.0);
		vec3 p = from + totalDist * direction;
		dist = DE(p);
		dist *= FudgeFactor;
		
		if (steps == 0) dist*=(Dither*rand(direction.xy))+(1.0-Dither);
		totalDist += dist;
		epsModified = pow(totalDist, 1.0)*eps;
		if (dist < epsModified) {
			// move back
			totalDist -= (epsModified-dist);
			break;
		}
		if (totalDist > MaxDistance) {
			//fSteps -= (totalDist-MaxDistance)/dist;
			break;
		}
	}
	//if (EnableFloor && dist ==floorDist*FudgeFactor) floorHit = true;
	vec3 hitColor;
	//float stepFactor = clamp((fSteps)/float(GlowMax),0.0,1.0);
	vec3 backColor = BackgroundColor;
	//if (GradientBackground>0.0) {
	//	float t = length(coord);
	//	backColor = mix(backColor, vec3(0.0,0.0,0.0), t*GradientBackground);
	//}
	
	if (steps == MaxRaySteps) orbitTrap = vec4(0.0);
	
	float shadowStrength = 0.0;
	if ( dist < epsModified) {
		// We hit something, or reached MaxRaySteps
		hit = from + totalDist * direction;
		//float ao = AO.w*stepFactor ;
		
		hitNormal= normal(hit-NormalBackStep*epsModified*direction, epsModified); // /*normalE*epsModified/eps*/
		
		
		
		//#ifdef  providesColor
		//hitColor = mix(BaseColor,  baseColor(hit,hitNormal),  OrbitStrength);
		//#else
		hitColor = getColor();
		//#endif
		//#ifndef linearGamma
		//hitColor = pow(clamp(hitColor,0.0,1.0),vec3(Gamma));
		//#endif
		//if (DetailAO<0.0) ao = ambientOcclusion(hit, hitNormal);
		//if (floorHit) {
		//	hitColor = FloorColor;
		//}
		
		//hitColor = mix(hitColor, AO.xyz ,ao);
		hitColor = lighting(hitNormal, hitColor,  hit,  direction,epsModified,shadowStrength);
		
		// OpenGL  GL_EXP2 like fog
		float f = totalDist;
		hitColor = mix(hitColor, backColor, 1.0-exp(-pow(Fog,4.0)*f*f));
		
		//if (floorHit ) {
		//	hitColor +=Glow.xyz*stepFactor* Glow.w*(1.0-shadowStrength);
		//}
	}
	else {
		//#ifdef providesBackground
		//hitColor = backgroundColor(dir);
		//#else
		hitColor = backColor;
		//#endif
		//hitColor +=Glow.xyz*stepFactor* Glow.w*(1.0-shadowStrength);
		hitNormal = vec3(0.0);
		//if (DebugSun) {
		//	vec3 spotDir = vec3(sin(SpotLightDir.x*3.1415)*cos(SpotLightDir.y*3.1415/2.0), sin(SpotLightDir.y*3.1415/2.0)*sin(SpotLightDir.x*3.1415), cos(SpotLightDir.x*3.1415));
		//	spotDir = normalize(spotDir);
		//	if (dot(spotDir,normalize(dir))>0.9) hitColor= vec3(100.,0.,0.);
		//}
	}
	
	return vec4(hitColor, totalDist);
}

void main() {
	vec3 hit_point;
	vec3 hit_normal;
	gl_FragColor = trace(camera_eye, camera_ray, hit_point, hit_normal);
}
