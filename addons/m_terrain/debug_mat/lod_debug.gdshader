shader_type spatial;

uniform sampler2D lod_num_tex;


float rand(float seed)
{
    return fract((sin(seed + 78.233)) * 43758.5453);
}

void fragment() {
	ALBEDO = texture(lod_num_tex, UV).rgb;
	//ALBEDO = mix(ALBEDO, ALBEDO+vec3(1.0,0.0,0.0), select);
	//vec3 rand_col = vec3(rand(select),rand(select+0.2),rand(select+0.9));
	ALBEDO += vec3(0.3,0.3,0.0);
}
