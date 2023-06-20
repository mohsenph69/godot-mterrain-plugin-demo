#ifndef MREGION
#define MREGION

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/templates/vset.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include "mbound.h"

using namespace godot;


struct MImageInfo {
    String uniform;
    Image::Format format;
    int32_t size;
    PackedByteArray image;
};

struct MUpdateInfo
{
    String uniform;
    Ref<ImageTexture> tex;
};




class MRegion : public Object{
    GDCLASS(MRegion, Object);

    private:
    Ref<ShaderMaterial> _material;
    Vector<MImageInfo*> images;
    Vector<MUpdateInfo> update_info;
    String shader_code;
    VSet<int8_t>* lods;
    int8_t last_lod = -2;


    protected:
    static void _bind_methods(){}

    public:
    MGridPos pos;
    Vector3 world_pos;
    int32_t region_size_meter;
    MRegion* left = nullptr;
    MRegion* right = nullptr;
    MRegion* top = nullptr;
    MRegion* bottom = nullptr;
    
    
    MRegion();
    ~MRegion();
    void set_material(const Ref<ShaderMaterial> input);
    RID get_material_rid();
    void update_material(Ref<ShaderMaterial> mat);
    String get_shader_code(int32_t img_size);
    void set_image_info(MImageInfo* input);
    void update_region();
    void insert_lod(const int8_t& input);
    void apply_update();
    Ref<ImageTexture> get_texture(MImageInfo* info,int8_t lod);

    // This function exist in godot source code
    // But unfortunatlly it is not expose in GDExtension
    // Beacause of this I have to copy that here
    // later if they expose this we can remove this
    static int get_format_pixel_size(Image::Format p_format);
};
#endif