//FRACTAL_FILE

uniform int Iterations = 15; //~Fractal|default|15|0|200
uniform bool Julia = true; //~Fractal|default|true|false|true
uniform vec3 JuliaC = vec3(0.0); //~Fractal|default|0,0,0|-2,-2,-2|2,2,2
//uniform int ColorIterations = 3; //~Fractal|default|3|0|200
uniform float fold = 0.25; //~Fractal|default|0.25|-1|5
//uniform float Scale = 3.0; //~Fractal|default|3|-5|5

//uniform vec3 RotVector = vec3(0.0); //~Fractal|default|1,1,1|0,0,0|1,1,1
//uniform float RotAngle = 0.0; //~Fractal|default|0|0|180

//mat3 rot;

vec4 orbitTrap = vec4(10000.0);

//vec4 scale = vec4(Scale, Scale, Scale, abs(Scale)) / MinRad2;
//float absScalem1 = abs(Scale - 1.0);
//float AbsScaleRaisedTo1mIters = pow(abs(Scale), float(1-Iterations));

void init() {
	//rot = rotationMatrix3(normalize(RotVector), RotAngle);
}

float DE(vec3 z) {
	vec3 pos = z;
	float DF = 1.0;
	for (int i = 0; i < Iterations ; i++) 
	{
		z.xyz = clamp(z.xyz, -fold, fold)*2.0 - z.xyz;
		float r = dot(z, z);
		z /=-r; 
		DF /= r;
		z.xz *= -1.0;
		z += (Julia ? JuliaC : pos);
		orbitTrap = min(orbitTrap, abs(vec4(z,dot(z,z))));
	}	
	float r = length(z); //(dot2(z));
	return 0.2*sqrt(r)/DF;
}
