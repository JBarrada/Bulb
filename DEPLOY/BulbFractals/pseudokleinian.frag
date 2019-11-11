//FRACTAL_FILE

uniform int Iterations = 5; //~Fractal|default|5|0|200
uniform float Size = 1.0; //~Fractal|default|1|0|2
uniform vec3 CSize = vec3(1.0); //~Fractal|default|1,1,1|0,0,0|2,2,2
uniform vec3 C = vec3(0.0); //~Fractal|default|0,0,0|-2,-2,-2|2,2,2
uniform float TThickness = 0.01; //~Fractal|default|0.01|0|2
uniform float DEoffset = 0.0; //~Fractal|default|0|0|0.01
uniform vec3 Offset = vec3(0.0); //~Fractal|default|1,1,1|-1,-1,-1|1,1,1

vec4 orbitTrap = vec4(10000.0);

void init() {}

float RoundBox(vec3 p, vec3 csize, float offset) {
	vec3 di = abs(p) - csize;
	float k=max(di.x,max(di.y,di.z));
	return abs(k*float(k<0.0)+ length(max(di,0.0))-offset);
}

float Thingy(vec3 p, float e) {
	p-=Offset;
	return (abs(length(p.xy)*p.z)-e) / sqrt(dot(p,p)+abs(e));
}

float Thing2(vec3 p) {
	float DEfactor=1.0;
   	vec3 ap=p+1.0;
	for(int i=0;i<Iterations && ap!=p;i++){
		ap=p;
		p=2.0*clamp(p, -CSize, CSize)-p;
      
		float r2=dot(p,p);
		orbitTrap = min(orbitTrap, abs(vec4(p,r2)));
		float k=max(Size/r2,1.0);

		p*=k;DEfactor*=k;
      
		p+=C;
		orbitTrap = min(orbitTrap, abs(vec4(p,dot(p,p))));
	}
	return abs(0.5*Thingy(p,TThickness)/DEfactor-DEoffset);

	//return abs(0.5*abs(p.z-Offset.z)/DEfactor-DEoffset);
}

float DE(vec3 p){
	return Thing2(p);
}
