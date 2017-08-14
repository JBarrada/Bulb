//FRACTAL_FILE

uniform float Scale = 1.5; //~Fractal,default,1.5|0|5
uniform int Iterations = 10; //~Fractal,default,10|0|200

uniform vec3 Offset = vec3(0.0,1.0,1.0); //~Fractal,default,0,1,1|-1,-1,-1|1,1,1
uniform vec3 Offset2 = vec3(1.0,-0.3,-0.3); //~Fractal,default,1.0,-0.3,-0.3|-1,-1,-1|1,1,1
uniform float Qube = 0.1; //~Fractal,default,0.1|-1|1

uniform float Angle1 = 0.0; //~Fractal,default,0|-180|180
uniform vec3 Rot1 = vec3(1.0,1.0,1.0); //~Fractal,default,1,1,1|-1,-1,-1|1,1,1

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