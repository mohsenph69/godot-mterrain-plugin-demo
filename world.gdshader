shader_type spatial;
render_mode unshaded;


uniform sampler2D mterrain_1_heightmap_tex:filter_nearest;
uniform vec3 color;

varying vec2 heightmap_uv;
varying vec2 splat_uv;
void vertex(){
	heightmap_uv;    //  HEIGHTMAP_UV
	splat_uv; // SPLAT_UV
	//heightmap_uv = ((NODE_POSITION_WORLD + VERTEX).xz + vec2(0.0,0.0)) + 0.5/33;
}


void fragment() {
	ALBEDO = texture(mterrain_1_heightmap_tex, splat_uv).rgb;
}
