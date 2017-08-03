uniform int Iterations = 10;
uniform int ColorIterations = 3;
uniform float MinRad2 = 0.25;
uniform float Scale = 1.5;

vec4 orbitTrap = vec4(10000.0);

uniform float Phi = 1.618; //slider[-5,1.618,5]

vec3 n1 = normalize(vec3(-Phi,Phi-1.0,1.0));
vec3 n2 = normalize(vec3(1.0,-Phi,Phi+1.0));
vec3 n3 = normalize(vec3(0.0,0.0,-1.0));

mat4  rotationMatrix(vec3 v, float angle) {
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

uniform vec3 Offset = vec3(0.850650808,0.525731112,0);

uniform float Angle1 = 0; //slider[-180,0,180]
uniform vec3 Rot1 = vec3(1); //slider[(-1,-1,-1),(1,1,1),(1,1,1)]
uniform float Angle2 = 0; //slider[-180,0,180]
uniform vec3 Rot2 = vec3(1); //slider[(-1,-1,-1),(1,1,1),(1,1,1)]

mat4 fracRotation2 = rotationMatrix(normalize(Rot2), Angle2);
mat4 fracRotation1 = rotationMatrix(normalize(Rot1), Angle1);
mat4 M = fracRotation2 * translate(Offset) * scale4(Scale) * translate(-Offset) * fracRotation1;


float DE(vec3 z)
{
	float t;
	
	// Prefolds.
	z = abs(z);
	t=dot(z,n1); if (t>0.0) { z-=2.0*t*n1; }
	t=dot(z,n2); if (t>0.0) { z-=2.0*t*n2; }
	t =dot(z,n3); if (t>0.0) { z-=2.0*t*n3; }
	t =dot(z,n2); if (t>0.0) { z-=2.0*t*n2; }
	
	// Iterate to compute the distance estimator.
	int n = 0;
	while (n < Iterations) {
		// Fold
		z = abs(z);
		t =dot(z,n1); if (t>0.0) { z-=2.0*t*n1; }
		
		// Rotate, scale, rotate (we need to cast to a 4-component vector).
		z = (M*vec4(z,1.0)).xyz;
		n++;
		if (n < ColorIterations) orbitTrap = min(orbitTrap, abs(vec4(z,dot(z,z))));
	}
	
	return (length(z) ) * pow(Scale,  float(-n));
}
