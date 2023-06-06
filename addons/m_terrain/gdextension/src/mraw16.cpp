#include "mraw16.h"

#include <iostream>
#include <godot_cpp/variant/utility_functions.hpp>


void MRaw16::_bind_methods() {
   ClassDB::bind_static_method("MRaw16", D_METHOD("get_texture","file_path"), &MRaw16::get_texture); 
}


MRaw16::MRaw16()
{
}

MRaw16::~MRaw16()
{
}


Ref<Image> MRaw16::get_texture(const String& file_path) {
    Ref<Image> tex;
    Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
    uint64_t size = file->get_length();
    uint64_t size16 = size/2;
    uint64_t width = sqrt(size16);
    if(width*width != size16){
        ERR_FAIL_COND_V("Image is not square or data is not valid", tex);
    }
    PackedFloat32Array data;
    data.resize(size16);
    for(int i=0;i<size16;i++){
        data[i] = ((double)file->get_16())/65535;
    }
    PackedByteArray dataByte = data.to_byte_array();
    UtilityFunctions::print("width ", itos(width));
    Ref<Image> img(memnew(Image));
    img->create_from_data(width,width, false, Image::FORMAT_RF, dataByte);
    return img;
}