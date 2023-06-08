shader_type spatial;
//render_mode unshaded;

uniform sampler2D heightmap_tex:repeat_disable;
uniform sampler2D heightmap_normal_tex;
uniform float height_scale = 30.0;
uniform vec3 terrain_offset;


uniform sampler2D splatmap_tex;

uniform sampler2D tex0_albedo;
uniform sampler2D tex0_normalmap:hint_normal;
uniform sampler2D tex0_roughness;
uniform sampler2D tex0_ao;

uniform sampler2D tex1_albedo;
uniform sampler2D tex1_normalmap:hint_normal;
uniform sampler2D tex1_roughness;
uniform sampler2D tex1_ao;

uniform sampler2D tex2_albedo;
uniform sampler2D tex2_normalmap:hint_normal;
uniform sampler2D tex2_roughness;
uniform sampler2D tex2_ao;

uniform sampler2D tex3_albedo;
uniform sampler2D tex3_normalmap:hint_normal;
uniform sampler2D tex3_roughness;
uniform sampler2D tex3_ao;


varying vec2 terrain_uv;
varying vec2 tex_uv;
varying vec3 foo;
void vertex(){
	vec3 pos = NODE_POSITION_WORLD + VERTEX + terrain_offset;
	terrain_uv = pos.xz/8193.0;
	float h = texture(heightmap_tex, terrain_uv).r;
	h *= height_scale;
	VERTEX.y += h - 500.0;
	vec3 n = texture(heightmap_normal_tex, terrain_uv).xzy;
	n = n*2.0 - 1.0;
	n.z *= -1.0;
	NORMAL = n;
	foo = n;
	tex_uv = terrain_uv*1000.0;
}

void fragment() {
	vec3 n = texture(heightmap_normal_tex, terrain_uv).xzy;
	n = n*2.0 - 1.0;
	mat3 t = mat3(TANGENT, NORMAL, BINORMAL);
	NORMAL = t*n;
	vec3 splatmap = texture(splatmap_tex, terrain_uv).rgb;
	splatmap.r = smoothstep(0.8,0.81,splatmap.r);
	splatmap.g = smoothstep(0.1,0.3,splatmap.g);
	splatmap.b = smoothstep(0.8,1.0,splatmap.b);
	ALBEDO = texture(tex0_albedo,tex_uv).rgb;
	ROUGHNESS = texture(tex0_roughness,tex_uv).r;
	AO = texture(tex1_ao,tex_uv).r;
	NORMAL_MAP = texture(tex0_normalmap,tex_uv).rgb;
	//tex1
	ALBEDO = mix(ALBEDO,texture(tex1_albedo,tex_uv).rgb, splatmap.r);
	ROUGHNESS = mix(ROUGHNESS,texture(tex1_roughness,tex_uv).r, splatmap.r)*1.5;
	SPECULAR = 0.1;
	AO = mix(AO,texture(tex1_ao,tex_uv).r,splatmap.r);
	NORMAL_MAP = mix(NORMAL_MAP,texture(tex1_normalmap,tex_uv).xyz,splatmap.r);
	//tex2
	ALBEDO = mix(ALBEDO,texture(tex2_albedo,tex_uv).rgb*0.5, splatmap.g);
	ROUGHNESS = mix(ROUGHNESS,texture(tex2_roughness,tex_uv).r, splatmap.r)*1.5;
	SPECULAR = 0.1;
	AO = mix(AO,texture(tex1_ao,tex_uv).r,splatmap.g);
	NORMAL_MAP = mix(NORMAL_MAP,texture(tex2_normalmap,tex_uv).xyz,splatmap.g);
	NORMAL_MAP_DEPTH = 2.0;
	//tex2
	ALBEDO = mix(ALBEDO,texture(tex3_albedo,tex_uv).rgb*0.5, splatmap.b);
	ROUGHNESS = mix(ROUGHNESS,texture(tex3_roughness,tex_uv).r, splatmap.r)*1.5;
	SPECULAR = 0.1;
	AO = mix(AO,texture(tex3_ao,tex_uv).r,splatmap.b);
	NORMAL_MAP = mix(NORMAL_MAP,texture(tex3_normalmap,tex_uv).xyz,splatmap.b);
	NORMAL_MAP_DEPTH = 2.0;
}
