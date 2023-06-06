#include "mterrain.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/engine.hpp>


void MTerrain::_bind_methods() {
    ClassDB::bind_method(D_METHOD("finish_terrain"), &MTerrain::finish_terrain);
    ClassDB::bind_method(D_METHOD("start"), &MTerrain::start);
    ClassDB::bind_method(D_METHOD("update"), &MTerrain::update);
    ClassDB::bind_method(D_METHOD("get_update_remove_chunks"), &MTerrain::get_update_remove_chunks);

    ClassDB::bind_method(D_METHOD("get_material"), &MTerrain::get_material);
    ClassDB::bind_method(D_METHOD("set_material", "terrain_material"), &MTerrain::set_material);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "ShaderMaterial,BaseMaterial3D"), "set_material", "get_material");
    ClassDB::bind_method(D_METHOD("get_terrain_size"), &MTerrain::get_terrain_size);
    ClassDB::bind_method(D_METHOD("set_terrain_size", "size"), &MTerrain::set_terrain_size);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I,"terrain_size"), "set_terrain_size", "get_terrain_size");
    ClassDB::bind_method(D_METHOD("set_offset", "offset"), &MTerrain::set_offset);
    ClassDB::bind_method(D_METHOD("get_offset"), &MTerrain::get_offset);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "offset"), "set_offset", "get_offset");
    
    
    ClassDB::bind_method(D_METHOD("set_max_range", "max_range"), &MTerrain::set_max_range);
    ClassDB::bind_method(D_METHOD("get_max_range"), &MTerrain::get_max_range);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_range"), "set_max_range", "get_max_range");
    ClassDB::bind_method(D_METHOD("set_custom_camera", "camera"), &MTerrain::set_custom_camera);
    ClassDB::bind_method(D_METHOD("set_editor_camera", "camera"), &MTerrain::set_editor_camera);

    
    ClassDB::bind_method(D_METHOD("set_min_size","index"), &MTerrain::set_min_size);
    ClassDB::bind_method(D_METHOD("get_min_size"), &MTerrain::get_min_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"min_size",PROPERTY_HINT_ENUM, M_SIZE_LIST_STRING), "set_min_size", "get_min_size");

    ClassDB::bind_method(D_METHOD("set_max_size","index"), &MTerrain::set_max_size);
    ClassDB::bind_method(D_METHOD("get_max_size"), &MTerrain::get_max_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"max_size",PROPERTY_HINT_ENUM, M_SIZE_LIST_STRING), "set_max_size", "get_max_size");
    
    ClassDB::bind_method(D_METHOD("set_min_h_scale","index"), &MTerrain::set_min_h_scale);
    ClassDB::bind_method(D_METHOD("get_min_h_scale"), &MTerrain::get_min_h_scale);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"min_h_scale",PROPERTY_HINT_ENUM, M_H_SCALE_LIST_STRING), "set_min_h_scale", "get_min_h_scale");

    ClassDB::bind_method(D_METHOD("set_max_h_scale","index"), &MTerrain::set_max_h_scale);
    ClassDB::bind_method(D_METHOD("get_max_h_scale"), &MTerrain::get_max_h_scale);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"max_h_scale",PROPERTY_HINT_ENUM, M_H_SCALE_LIST_STRING), "set_max_h_scale", "get_max_h_scale");

    ClassDB::bind_method(D_METHOD("set_size_info", "size_info"), &MTerrain::set_size_info);
    ClassDB::bind_method(D_METHOD("get_size_info"), &MTerrain::get_size_info);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "size_info",PROPERTY_HINT_NONE,"",PROPERTY_USAGE_STORAGE), "set_size_info", "get_size_info");

    ClassDB::bind_method(D_METHOD("set_lod_distance", "lod_distance"), &MTerrain::set_lod_distance);
    ClassDB::bind_method(D_METHOD("get_lod_distance"), &MTerrain::get_lod_distance);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "lod_distance",PROPERTY_HINT_NONE,"", PROPERTY_USAGE_STORAGE),"set_lod_distance","get_lod_distance");
}

MTerrain::MTerrain() {
    //connect("ready", Callable(this, "start"));
    //connect("tree_exiting", Callable(this, "finish_terrain"));
    recalculate_terrain_config(true);
    grid = memnew(MGrid);
    update_timer = memnew(Timer);
    update_timer->set_wait_time(0.2);
    add_child(update_timer);
    update_timer->connect("timeout", Callable(this, "update"));
}

MTerrain::~MTerrain() {
    memdelete(grid);

}


void MTerrain::finish_terrain() {

}

void MTerrain::start() {
    //UtilityFunctions::print("POITN size ", itos(sizeof(MPoint)));
    //grid->set_scenario(get_world_3d()->get_scenario());
    //grid->offset = offset;
    //grid->create(terrain_size.x,terrain_size.y);
    //if(material.is_valid()){
    //    grid->set_material(material->get_rid());
    //}
    //get_update_remove_chunks();
    //grid->update_meshes();
    //continue_update();
    create_grid();
}

void MTerrain::create_grid(){
    _chunks = memnew(MChunks);
    _chunks->create_chunks(size_list[min_size_index],size_list[max_size_index],h_scale_list[min_h_scale_index],h_scale_list[max_h_scale_index],size_info);
    grid->set_scenario(get_world_3d()->get_scenario());
    grid->offset = offset;
    if(material.is_valid()){
        grid->set_material(material->get_rid());
    }
    grid->lod_distance = lod_distance;
    grid->create(terrain_size.x,terrain_size.y,_chunks);
    get_update_remove_chunks();
    grid->update_meshes();
    continue_update();
    UtilityFunctions::print("Chunks has been created ");
}

void MTerrain::update() {
    std::future_status status = update_thread_future.wait_for(std::chrono::microseconds(1));
    if(status == std::future_status::ready){
        grid->update_meshes();
        get_cam_pos();
        update_thread_future = std::async(std::launch::async, &MTerrain::get_update_remove_chunks, this);
    }
    
}

void MTerrain::stop_update() {
    if(update_thread_future.valid() && grid->is_created()){
        update_timer->stop();
        update_thread_future.wait();
    }
}

void MTerrain::continue_update() {
    if (grid->is_created()){
        update_timer->start();
        update_thread_future = std::async(std::launch::async, &MTerrain::get_update_remove_chunks, this);
    }
}

void MTerrain::get_update_remove_chunks() {
    get_cam_pos();
    grid->set_cam_pos(cam_pos);
    grid->update_search_bound();
    grid->cull_out_of_bound();
    grid->update_lods();
    grid->merge_chunks();
}

void MTerrain::get_cam_pos() {
    if(custom_camera != nullptr){
        cam_pos = custom_camera->get_position();
        return;
    }
    if(editor_camera !=nullptr){
        cam_pos = editor_camera->get_position();
        return;
    }
    Viewport* v = get_viewport();
    Camera3D* c = v->get_camera_3d();
    cam_pos = c->get_position();
}



Ref<Material> MTerrain::get_material(){
    return material;
}

void MTerrain::set_material(Ref<Material> m){
    material = m;
    if(!grid->is_created()){
        return;
    }
    stop_update();
    if(m.is_valid()){
        grid->set_material(material->get_rid());
    } else {
         grid->set_material(RID());
    }
    continue_update();
}

Vector2i MTerrain::get_terrain_size(){
    return terrain_size;
}

void MTerrain::set_terrain_size(Vector2i size){
    if(size.x < 1 || size.y < 1){
        return;
    }
    if(size == terrain_size){
        return;
    }
    terrain_size = size;
    if(grid->is_created()){
        stop_update();
        grid->clear();
        //grid->create(size.x,size.y);
        continue_update();
    }
}

void MTerrain::set_max_range(int32_t input) {
    max_range = input;
    grid->max_range = input;
}

int32_t MTerrain::get_max_range() {
    return max_range;
}

void MTerrain::set_editor_camera(Node3D* camera_node){
    editor_camera = camera_node;
}
void MTerrain::set_custom_camera(Node3D* camera_node){
    custom_camera = camera_node;
}

void MTerrain::set_offset(Vector3 input){
    offset = input;
    if(grid->is_created()){
        stop_update();
        grid->clear();
        grid->offset = offset;
        //grid->create(terrain_size.x, terrain_size.y);

    }
}

Vector3 MTerrain::get_offset(){
    return offset;
}


void MTerrain::recalculate_terrain_config(const bool& force_calculate) {
    if(!is_inside_tree() && !force_calculate){
        return;
    }
    // Calculating max size
    max_size = (int8_t)(max_size_index - min_size_index);
    // Calculating max lod
    max_lod = (int8_t)(max_h_scale_index - min_h_scale_index);
    if(h_scale_list[max_h_scale_index] > size_list[min_size_index]){
        size_info.clear();
        notify_property_list_changed();
        ERR_FAIL_COND("min size is smaller than max h scale");
    }
    size_info.clear();
    size_info.resize(max_size+1);
    for(int i=0;i<size_info.size();i++){
        Array lod;
        lod.resize(max_lod+1);
        for(int j=0;j<lod.size();j++){
            if(j==lod.size()-1){
                lod[j] = true;
                continue;
            }
            lod[j] = i <=j;
        }
        size_info[i] = lod;
    }

    /// Calculating LOD distance
    lod_distance.resize(max_lod);
    int32_t ll = lod_distance[0];
    UtilityFunctions::print("lod 0 ", ll);
    for(int i=1;i<lod_distance.size();i++){
        if(lod_distance[i] <= ll){
            lod_distance[i] = ll + 1; 
        }
        ll = lod_distance[i];
    }
    notify_property_list_changed();
}



int MTerrain::get_min_size() {
    return min_size_index;
}

void MTerrain::set_min_size(int index) {
    if(index >= max_size_index){
        return;
    }
    min_size_index = index;
    recalculate_terrain_config(false);
}

int MTerrain::get_max_size() {
    return max_size_index;
}

void MTerrain::set_max_size(int index) {
    if(index <= min_size_index){
        return;
    }
    max_size_index = index;
    recalculate_terrain_config(false);
}

void MTerrain::set_min_h_scale(int index) {
    if(index >= max_h_scale_index){
        return;
    }
    min_h_scale_index = index;
    recalculate_terrain_config(false);
}

int MTerrain::get_min_h_scale() {
    return min_h_scale_index;
}

void MTerrain::set_max_h_scale(int index) {
    if(index <= min_h_scale_index){
        return;
    }
    max_h_scale_index = index;
    recalculate_terrain_config(false);
}

int MTerrain::get_max_h_scale(){
    return max_h_scale_index;
}


void MTerrain::set_size_info(const Array& arr) {
    size_info = arr;
}
Array MTerrain::get_size_info() {
    return size_info;
}

void MTerrain::set_lod_distance(const PackedInt32Array& input){
    lod_distance = input;
}

PackedInt32Array MTerrain::get_lod_distance() {
    return lod_distance;
}

void MTerrain::_get_property_list(List<PropertyInfo> *p_list) const {
    //Adding lod distance property
    PropertyInfo sub_lod(Variant::INT, "LOD distance", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SUBGROUP);
    p_list->push_back(sub_lod);
    for(int i=0; i<lod_distance.size();i++){
        PropertyInfo p(Variant::INT,"M_LOD_"+itos(i),PROPERTY_HINT_NONE,"",PROPERTY_USAGE_EDITOR);
        p_list->push_back(p);
    }
    //Adding size info property
    for(int size=0;size<size_info.size();size++){
        Array lod_info = size_info[size];
        PropertyInfo sub(Variant::INT, "Size "+itos(size_list[size+min_size_index]), PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SUBGROUP);
        p_list->push_back(sub);
        for(int lod=0;lod<lod_info.size();lod++){
            PropertyInfo p(Variant::BOOL,"SIZE_"+itos(size)+"_LOD_"+itos(lod)+"_HSCALE_"+itos(h_scale_list[lod+min_h_scale_index]),PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR);
            p_list->push_back(p);
        }
    }
}

bool MTerrain::_get(const StringName &p_name, Variant &r_ret) const {
    if(p_name.begins_with("SIZE_")){
        PackedStringArray parts = p_name.split("_");
        if(parts.size() != 6){
            return false;
        }
        int64_t size = parts[1].to_int();
        int64_t lod = parts[3].to_int();
        Array lod_info = size_info[size];
        r_ret = lod_info[lod];
        return true;
    }
    if(p_name.begins_with("M_LOD_")){
        int64_t index = p_name.get_slicec('_',2).to_int();
        r_ret = (float)lod_distance[index];
        return true;
    }
    return false;
}


bool MTerrain::_set(const StringName &p_name, const Variant &p_value) {
    if(p_name.begins_with("SIZE_")){
        PackedStringArray parts = p_name.split("_");
        if(parts.size() != 6){
            return false;
        }
        int64_t size = parts[1].to_int();
        if(size==0){
            return false;
        }
        int64_t lod = parts[3].to_int();
        Array lod_info = size_info[size];
        lod_info[lod] = p_value;
        size_info[size] = lod_info;
        return true;
    }
    if(p_name.begins_with("M_LOD_")){
        int64_t index = p_name.get_slicec('_',2).to_int();
        int32_t val = p_value;
        if(val<0){
            return false;
        }
        lod_distance[index] = val;
        return true;
    }
    return false;
}