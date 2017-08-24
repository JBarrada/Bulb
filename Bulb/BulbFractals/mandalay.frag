//FRACTAL_FILE

uniform int Iterations = 15; //~Fractal|default|15|0|200
uniform bool Julia = false; //~Fractal|default|false|false|true
uniform vec3 JuliaC = vec3(1.0); //~Fractal|default|1,1,1|-2,-2,-2|2,2,2
uniform float Scale = 3.0; //~Fractal|default|3|-5|5
uniform float MinRad2 = 0.25; //~Fractal|default|0.25|0|2

uniform vec3 RotVector = vec3(0.0); //~Fractal|default|1,1,1|0,0,0|1,1,1
uniform float RotAngle = 0.0; //~Fractal|default|0|0|180

mat3 rot;

vec4 orbitTrap = vec4(10000.0);

uniform vec3 fo = vec3(0.5); //~Fractal|default|0.5,0.5,0.5|0,0,0|2,2,2
uniform vec3 g = vec3(0.1); //~Fractal|default|0.1,0.1,0.1|0,0,0|2,2,2

uniform bool Serial = false; //~Fractal|default|false|false|true


float sr;
void init() {
	rot = rotationMatrix3(normalize(RotVector), RotAngle);
	sr = 30.0;
}

//DarkBeam's "fold"... reinterpreted... it's more than a fold, much more! Just awesome!
float DBFold(vec3 p, float fo, float g){
	if(p.z>p.y) p.yz = p.zy;
	float vx = p.x-2.0*fo;
	float vy = p.y-4.0*fo;
	float v = abs(max(vx,vy)+fo)-fo;
	float v1 = max(vx-g,p.y);
	v = min(v,v1);
	v1 = max(vx+g,2.0*fo-p.y);
	return min(v,v1);
}

//the coordinates are pushed/pulled in parallel
vec3 DBFoldParallel(vec3 p, vec3 fo, vec3 g){
	vec3 p1 = p;
	p.x = DBFold(p1,fo.x,g.x);
	p.y = DBFold(p1.yzx,fo.y,g.y);
	p.z = DBFold(p1.zxy,fo.z,g.z);
	return p;
}

//serial version
vec3 DBFoldSerial(vec3 p, vec3 fo, vec3 g){
	p.x = DBFold(p,fo.x,g.x);
	p.y = DBFold(p.yzx,fo.y,g.y);
	p.z = DBFold(p.zxy,fo.z,g.z);
	return p;
}
float DE(vec3 p) {
	vec4 JC = Julia ? vec4(JuliaC, 0.0) : vec4(p, 1.0);
	float r2 = dot(p,p);
	float dd = 1.0;
	for(int i = 0; i < Iterations && r2 < 10000.0; i++) {
		p = abs(p);

		if (Serial) p = DBFoldSerial(p,fo,g);
		else p = DBFoldParallel(p,fo,g);

		r2 = dot(p, p);
		//orbitTrap = min(orbitTrap, abs(vec4(p.x,p.y,p.z,r2)));	
		float t = clamp(1.0/r2, 1.0, 1.0/MinRad2);
		p *= t; dd *= t;

		p = p*Scale+JC.xyz; dd = dd*abs(Scale)+JC.w;
		p = rot*p;
		r2 = dot(p,p);
		orbitTrap = min(orbitTrap, abs(vec4(p.x,p.y,p.z,r2)));	
	}
#if 1
	return (sqrt(r2)-sr) / dd; //bounding volume is a sphere
#else
	p=abs(p); return (max(p.x,max(p.y,p.z))-sr)/dd;//bounding volume is a cube
#endif
}

