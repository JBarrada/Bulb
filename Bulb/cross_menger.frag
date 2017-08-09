//FRACTAL_FILE

uniform int Iterations = 15; //~Fractal,default,15|0|200
uniform float Gap = 1.0; //~Fractal,default,1|0|1

uniform vec3 Rot1 = vec3(0.0); //~Fractal,default,0,0,0|-180,-180,-180|180,180,180
uniform vec3 Rot2 = vec3(0.0); //~Fractal,default,0,0,0|-180,-180,-180|180,180,180

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
