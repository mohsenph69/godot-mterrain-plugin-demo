shader_type spatial;
//render_mode unshaded;


uniform sampler2D mterrain_1_heightmap:filter_nearest;
uniform sampler2D mterrain_0_normalmap:filter_nearest;

varying vec2 terrain_heightmap_uv;
varying vec2 terrain_splatmap_uv;
void vertex(){
	terrain_heightmap_uv; //TERRAIN_HEIGHTMAP
	terrain_splatmap_uv; //TERRAIN_SPLATMAP
	/*
	vec3 pos = NODE_POSITION_WORLD + VERTEX + terrain_offset;
	terrain_uv = pos.xz/8192.0;
	float h = texture(mterrain_1_heightmap, terrain_heightmap_uv).r;
	h *= height_scale;
	//VERTEX.y += h - 500.0;
	vec3 n = texture(mterrain_0_normalmap, terrain_uv).xzy;
	n = n*2.0 - 1.0;
	n.z *= -1.0;
	//NORMAL = n;
	foo = n;
	tex_uv = terrain_uv*1000.0;
	*/
}

void fragment() {
	ALBEDO = texture(mterrain_0_normalmap, terrain_splatmap_uv).rgb;
}
