#include "mregion.h"

#include <godot_cpp/classes/reg_ex.hpp>
#include <godot_cpp/variant/color.hpp>

MRegion::MRegion(){
    lods = memnew(VSet<int8_t>);
}

MRegion::~MRegion(){
    memdelete(lods);
}

void MRegion::set_material(const Ref<ShaderMaterial> input) {
    if(!input.is_valid()){
        return;
    }
    shader_code = input->get_shader()->get_code();
    _material = input;
    _material->set_shader_parameter("region_size", region_size_meter);
    _material->set_shader_parameter("region_world_position", world_pos);
}

String MRegion::get_shader_code(int32_t img_size) {
    String terrain_uv = " = (((NODE_POSITION_WORLD - vec3({0}.,0.,{1}.) + VERTEX)/{2}.)).xz*({3}./{4}.) + 0.5/{4}.;";
    String splat_uv = " = (((NODE_POSITION_WORLD - vec3({0}.,0.,{1}.) + VERTEX)/{2}.)).xz;";
    Array inputs;
    inputs.append(rtos(world_pos.x));
    inputs.append(rtos(world_pos.z));
    inputs.append(itos(region_size_meter));
    splat_uv = splat_uv.format(inputs);
    inputs.append(itos(img_size -1));
    inputs.append(itos(img_size));
    terrain_uv = terrain_uv.format(inputs);
    RegEx reg;
    reg.compile(";\\s*//\\s*HEIGHTMAP_UV");
    String out = reg.sub(shader_code, terrain_uv);
    reg.compile(";\\s*//\\s*SPLAT_UV");
    out = reg.sub(out, splat_uv);
    UtilityFunctions::print(splat_uv);
    return out;
}

RID MRegion::get_material_rid() {
    if(_material.is_valid()){
        return _material->get_rid();
    }
    return RID();
}

void MRegion::update_material(Ref<ShaderMaterial> mat) {
    double rand = UtilityFunctions::randf();
    Vector3 rand_color(rand,rand,rand);
    mat->set_shader_parameter("color", rand_color);
}


void MRegion::set_image_info(MImageInfo* input) {
    images.append(input);
}

void MRegion::update_region() {
    int8_t curren_lod = (lods->is_empty()) ? -1 : (*lods)[0];
    if(last_lod != curren_lod){
        for(int i=0; i < images.size(); i++){
            MImageInfo* info = images[i];
            Ref<ImageTexture> tex;
            // if out of bound then we just add an empty texture
            if(curren_lod == -1){
                update_info.append({info->uniform, tex});
                continue;
            }
            tex = get_texture(info, curren_lod);
            update_info.append({info->uniform, tex});
        }
        last_lod = curren_lod;
    }
    memdelete(lods);
    lods = memnew(VSet<int8_t>);
}

void MRegion::insert_lod(const int8_t& input) {
    lods->insert(input);
}

void MRegion::apply_update() {
    for(int i=0; i < update_info.size(); i++){
        _material->set_shader_parameter(update_info[i].uniform, update_info[i].tex);
    }
    update_info.clear();
}

Ref<ImageTexture> MRegion::get_texture(MImageInfo* info,int8_t lod) {
    int32_t scale = pow(2, (int32_t)lod);
    int32_t pixel_size = get_format_pixel_size(info->format);
    Ref<Texture2D> tex;
    if(info->size%2!=0){
        int32_t new_size = ((info->size - 1)/scale) + 1;
        PackedByteArray new_data;
        new_data.resize(new_size*new_size*pixel_size);
        for(int32_t y=0; y < new_size; y++){
            for(int32_t x=0; x < new_size; x++){
                int32_t main_offset = (scale*x + info->size*y*scale)*pixel_size;
                int32_t new_offset = (x+y*new_size)*pixel_size;
                for(int32_t i=0; i < pixel_size; i++){
                    new_data[new_offset+i] = info->image[main_offset+i];
                }
            }
        }
        Ref<Image> new_img;
        new_img = Image::create_from_data(new_size,new_size,false,info->format, new_data);
        tex = ImageTexture::create_from_image(new_img);
    }
    return tex;
}

int MRegion::get_format_pixel_size(Image::Format p_format){
switch (p_format) {
		case Image::FORMAT_L8:
			return 1; //luminance
		case Image::FORMAT_LA8:
			return 2; //luminance-alpha
		case Image::FORMAT_R8:
			return 1;
		case Image::FORMAT_RG8:
			return 2;
		case Image::FORMAT_RGB8:
			return 3;
		case Image::FORMAT_RGBA8:
			return 4;
		case Image::FORMAT_RGBA4444:
			return 2;
		case Image::FORMAT_RGB565:
			return 2;
		case Image::FORMAT_RF:
			return 4; //float
		case Image::FORMAT_RGF:
			return 8;
		case Image::FORMAT_RGBF:
			return 12;
		case Image::FORMAT_RGBAF:
			return 16;
		case Image::FORMAT_RH:
			return 2; //half float
		case Image::FORMAT_RGH:
			return 4;
		case Image::FORMAT_RGBH:
			return 6;
		case Image::FORMAT_RGBAH:
			return 8;
		case Image::FORMAT_RGBE9995:
			return 4;
		case Image::FORMAT_DXT1:
			return 1; //s3tc bc1
		case Image::FORMAT_DXT3:
			return 1; //bc2
		case Image::FORMAT_DXT5:
			return 1; //bc3
		case Image::FORMAT_RGTC_R:
			return 1; //bc4
		case Image::FORMAT_RGTC_RG:
			return 1; //bc5
		case Image::FORMAT_BPTC_RGBA:
			return 1; //btpc bc6h
		case Image::FORMAT_BPTC_RGBF:
			return 1; //float /
		case Image::FORMAT_BPTC_RGBFU:
			return 1; //unsigned float
		case Image::FORMAT_ETC:
			return 1; //etc1
		case Image::FORMAT_ETC2_R11:
			return 1; //etc2
		case Image::FORMAT_ETC2_R11S:
			return 1; //signed: return 1; NOT srgb.
		case Image::FORMAT_ETC2_RG11:
			return 1;
		case Image::FORMAT_ETC2_RG11S:
			return 1;
		case Image::FORMAT_ETC2_RGB8:
			return 1;
		case Image::FORMAT_ETC2_RGBA8:
			return 1;
		case Image::FORMAT_ETC2_RGB8A1:
			return 1;
		case Image::FORMAT_ETC2_RA_AS_RG:
			return 1;
		case Image::FORMAT_DXT5_RA_AS_RG:
			return 1;
		case Image::FORMAT_ASTC_4x4:
			return 1;
		case Image::FORMAT_ASTC_4x4_HDR:
			return 1;
		case Image::FORMAT_ASTC_8x8:
			return 1;
		case Image::FORMAT_ASTC_8x8_HDR:
			return 1;
		case Image::FORMAT_MAX: {
		}
	}
	return 0;
}