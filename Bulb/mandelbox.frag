uniform int Iterations = 10;
uniform int ColorIterations = 3;
uniform float MinRad2 = 0.25;
uniform float Scale = 3.0;

vec4 orbitTrap = vec4(10000.0);

vec4 scale = vec4(Scale, Scale, Scale, abs(Scale)) / MinRad2;
float absScalem1 = abs(Scale - 1.0);
float AbsScaleRaisedTo1mIters = pow(abs(Scale), float(1-Iterations));

float DE(vec3 pos) {
	vec4 p = vec4(pos,1), p0 = p;
	
	for (int i=0; i<Iterations; i++) {
		p.xyz = clamp(p.xyz, -1.0, 1.0) * 2.0 - p.xyz;
		float r2 = dot(p.xyz, p.xyz);
		
		if (i < ColorIterations) {
			orbitTrap = min(orbitTrap, abs(vec4(p.xyz,r2)));
		}
		
		p *= clamp(max(MinRad2/r2, MinRad2), 0.0, 1.0);
		p = p*scale + p0;
		
        if (r2 > 1000.0) {
			break;
		}
	}
	return ((length(p.xyz) - absScalem1) / p.w - AbsScaleRaisedTo1mIters);
}
