#include "mgrid.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <iostream>


MGrid::MGrid(){}
MGrid::~MGrid() {
    clear();
}

void MGrid::clear() {
    if(is_dirty){
        RenderingServer* rs = RenderingServer::get_singleton();
        for(int32_t z=_search_bound.top; z <=_search_bound.bottom; z++)
        {
            for(int32_t x=_search_bound.left; x <=_search_bound.right; x++){
                if(points[z][x].has_instance()){
                    rs->free_rid(points[z][x].instance);
                    points[z][x].instance = RID();
                    points[z][x].mesh = RID();
                }
            }
        }
        for(int32_t z=0; z <_size.z; z++){
                memdelete_arr<MPoint>(points[z]);
            }
            memdelete_arr<MPoint*>(points);
            memdelete_arr<MRegion>(regions);
    }
    _size.x = 0;
    _size.y = 0;
    _size.z = 0;
    _grid_bound.clear();
    _search_bound.clear();
    _last_search_bound.clear();
    is_dirty = false;
}

bool MGrid::is_created() {
    return is_dirty;
}

MGridPos MGrid::get_size() {
    return _size;
}

void MGrid::set_scenario(RID scenario){
    _scenario = scenario;
}

void MGrid::create(const int32_t& width,const int32_t& height, MChunks* chunks) {
    if (width == 0 || height == 0) return;
    UtilityFunctions::print("Size of point ", sizeof(MPoint));
    _chunks = chunks;
    _size.x = width;
    _size.z = height;
    _size_meter.x = width*_chunks->base_size_meter;
    _size_meter.z = height*_chunks->base_size_meter;
    _vertex_size.x = (_size_meter.x/chunks->h_scale) + 1;
    _vertex_size.z = (_size_meter.z/chunks->h_scale) + 1;
    _region_grid_size.x = _size.x/region_size + _size.x%region_size;
    _region_grid_size.z = _size.z/region_size + _size.z%region_size;
    _regions_count = _region_grid_size.x*_region_grid_size.z;
    _region_size_meter = region_size*_chunks->base_size_meter;
    _grid_bound = MBound(0,width-1, 0, height-1);
    regions = memnew_arr(MRegion, _regions_count);
    points = memnew_arr(MPoint*, _size.z);
    for (int32_t z=0; z<_size.z; z++){
        points[z] = memnew_arr(MPoint, _size.x);
    }
    //Init Regions
    int index = 0;
    for(int32_t z=0; z<_region_grid_size.z; z++){
        for(int32_t x=0; x<_region_grid_size.x; x++){
            //UtilityFunctions::print("create region ");
            regions[index].pos = MGridPos(x,0,z);
            regions[index].world_pos = get_world_pos(x*region_size,0,z*region_size);
            regions[index].region_size_meter = region_size*_chunks->base_size_meter;
            if(x!=0){
                regions[index].left = get_region(x-1,z);
            }
            if(x!=_region_grid_size.x-1){
                regions[index].right = get_region(x+1,z);
            }
            if(z!=0){
                regions[index].top = get_region(x,z-1);
            }
            if(z!=_region_grid_size.z-1){
                regions[index].bottom = get_region(x,z+1);
            }
            if(_material.is_valid()){
                regions[index].set_material(_material->duplicate());
            }
            index++;
        }
    }
    is_dirty = true;
}

void MGrid::update_regions_uniforms(Dictionary input) {
    UtilityFunctions::print("updating regions");
    Array uniforms = input.keys();
    for(int i=0;i<uniforms.size();i++){
        String uniform = uniforms[i];
        StringName path = input[uniform];
        //Later Here we should call another function if these images are in different files
        //OR in another word they are tiled
        update_region_no_tiles(uniform, path);
    }
}

void MGrid::update_region_no_tiles(const String& unifrom, const StringName& path) {
    ResourceLoader* rl = ResourceLoader::get_singleton();
    String spath = String(path);
    if(!rl->exists(spath)){
        ERR_FAIL_COND("File does not exist");
    }
    Ref<Image> img;
    Ref<Resource> res = rl->load(spath);
    if(res->is_class("Image")){
        img = res;
    } else if (res->is_class("Texture2D"))
    {
        Ref<Texture2D> tex = res;
        img = tex->get_image();
    } else {
        ERR_FAIL_COND("Unknown File Format MTerrain accept only Image and Texture2D class");
    }
    // Add one because we start pixels from zero
    Vector2i size = img->get_size();
    bool is_vertex_size = true;
    Vector2i region_img_size(_region_size_meter/_chunks->h_scale,_region_size_meter/_chunks->h_scale);
    int32_t region_offset = region_img_size.x;
    //In case image size match _vertex_size they have a common vertex
    //And code down here is for that
    //And if it is one less that that it used for thing like splatkmap and
    //they don' have a common vertex
    UtilityFunctions::print("image size ", size);
    UtilityFunctions::print("vertex_size size ", _vertex_size.x);
    if(size.x == _vertex_size.x && size.y == _vertex_size.z){
        region_img_size += Vector2(1,1);
    } else if (size.x == _vertex_size.x - 1 && size.y == _vertex_size.z - 1)
    {
        is_vertex_size = false;
    } else {
        ERR_FAIL_COND("Image size does not match terrain vertex size");
    }
    if(size.x != size.y){
        ERR_FAIL_COND("width and height of images should be equale to use in MTerrain plugin");
    }
    UtilityFunctions::print(region_img_size);
    bool is_odd = size.x%2 != 0;
    int index = 0;
    for(int32_t z=0; z<_region_grid_size.z; z++){
        for(int32_t x=0; x<_region_grid_size.x; x++){
            MRegion* region = regions + index;
            Vector2i pos(x,z);
            pos *= region_offset;
            //pos *= region_size*4;
            Rect2i rect(pos, Vector2i(region_img_size));
            UtilityFunctions::print(rect);
            Ref<Image> region_img = img->get_region(rect);
            MImageInfo* info = memnew(MImageInfo);
            info->uniform = unifrom;
            info->format = region_img->get_format();
            info->size = region_img->get_size().x;
            info->image = region_img->get_data();
            region->set_image_info(info);
            index++;
        }
    }
    for(int i=0; i < _regions_count; i++){
        //regions[i].update_region();
    }
}


Vector3 MGrid::get_world_pos(const int32_t &x,const int32_t& y,const int32_t& z) {
    return Vector3(x,y,z)*_chunks->base_size_meter + offset;
}

Vector3 MGrid::get_world_pos(const MGridPos& pos){
    return Vector3(pos.x,pos.y,pos.z)*_chunks->base_size_meter + offset;
}

MGridPos MGrid::get_grid_pos(const Vector3& pos) {
    MGridPos p;
    Vector3 rp = pos - offset;
    p.x = ((int32_t)(rp.x))/_chunks->base_size_meter;
    p.y = ((int32_t)(rp.y))/_chunks->base_size_meter;
    p.z = ((int32_t)(rp.z))/_chunks->base_size_meter;
    
    return p;
}

int32_t MGrid::get_region_id_by_point(const int32_t &x, const int32_t& z) {
    return x/region_size + (z/region_size)*_region_grid_size.x;
}

MRegion* MGrid::get_region_by_point(const int32_t &x, const int32_t& z){
    int32_t id = x/region_size + (z/region_size)*_region_grid_size.x;
    return regions + id;
}

MRegion* MGrid::get_region(const int32_t &x, const int32_t& z){
    int32_t id = x + z*_region_grid_size.x;
    return regions + id;
}


int8_t MGrid::get_lod_by_distance(const int32_t& dis) {
    for(int8_t i=0 ; i < lod_distance.size(); i++){
        if(dis < lod_distance[i]){
            return i;
        }
    }
    return _chunks->max_lod;
}


void MGrid::set_cam_pos(const Vector3& cam_world_pos) {
    _cam_pos = get_grid_pos(cam_world_pos);
    _cam_pos_real = cam_world_pos;
}



void MGrid::update_search_bound() {
    num_chunks = 0;
    update_mesh_list.clear();
    remove_instance_list.clear();
    MBound sb(_cam_pos, max_range, _size);
    _last_search_bound = _search_bound;
    _search_bound = sb;
    //_search_bound = _grid_bound; /// For now just for debuging
}

void MGrid::cull_out_of_bound() {
    RenderingServer* rs = RenderingServer::get_singleton();
    for(int32_t z=_last_search_bound.top; z <=_last_search_bound.bottom; z++)
    {
        for(int32_t x=_last_search_bound.left; x <=_last_search_bound.right; x++){
            if (!_search_bound.has_point(x,z)){
                if(points[z][x].has_instance()){
                    //rs->free_rid(points[z][x].instance);
                    remove_instance_list.append(points[z][x].instance);
                    points[z][x].instance = RID();
                    points[z][x].mesh = RID();
                }
            }

        }
    }
}


void MGrid::update_lods() {
    /// First Some Clean up
    for(int32_t z=_search_bound.top; z <=_search_bound.bottom; z++){
        for(int32_t x=_search_bound.left; x <=_search_bound.right; x++){
            points[z][x].size = 0;
        }
    }
    //// Now Update LOD
    MGridPos closest = _grid_bound.closest_point_on_ground(_cam_pos);

    MBound m(closest);
    int8_t current_lod = 0;
    current_lod = get_lod_by_distance(m.center.get_distance(_cam_pos));
    if(!_grid_bound.has_point(_cam_pos))
    {
        Vector3 closest_real = get_world_pos(closest);
        Vector3 diff = _cam_pos_real - closest_real;
        m.grow_when_outside(diff.x, diff.z,_cam_pos, _search_bound,_chunks->base_size_meter);
        for(int32_t z =m.top; z <= m.bottom; z++){
            for(int32_t x=m.left; x <= m.right; x++){
                points[z][x].lod = current_lod;
                get_region_by_point(x,z)->insert_lod(current_lod);
            }
        }
    } else {
        points[m.center.z][m.center.x].lod = current_lod;
        get_region_by_point(m.center.x,m.center.z)->insert_lod(current_lod);
    }
    while (m.grow(_search_bound,1,1))
    {
        int8_t l;
        if(current_lod != _chunks->max_lod){
            MGridPos e = m.get_edge_point();
            int32_t dis = e.get_distance(_cam_pos);
            l = get_lod_by_distance(dis);
            //make sure that we are going one lod by one lod not jumping two lod
            if (l > current_lod +1){
                l = current_lod + 1;
            }
            // Also make sure to not jump back to lower lod when growing
            if(l<current_lod){
                l = current_lod;
            }
            current_lod = l;
        } else {
            l = _chunks->max_lod;
        }
        if(m.grow_left){
            for(int32_t z=m.top; z<=m.bottom;z++){
                points[z][m.left].lod = l;
                get_region_by_point(m.left,z)->insert_lod(l);
            }
        }
        if(m.grow_right){
            for(int32_t z=m.top; z<=m.bottom;z++){
                points[z][m.right].lod = l;
                get_region_by_point(m.right,z)->insert_lod(l);
            }
        }
        if(m.grow_top){
            for(int32_t x=m.left; x<=m.right; x++){
                points[m.top][x].lod = l;
                get_region_by_point(x,m.top)->insert_lod(l);
            }
        }
        if(m.grow_bottom){
            for(int32_t x=m.left; x<=m.right; x++){
                points[m.bottom][x].lod = l;
                get_region_by_point(x,m.bottom)->insert_lod(l);
            }
        }
    }
}

///////////////////////////////////////////////////////
////////////////// MERGE //////////////////////////////
void MGrid::merge_chunks() {
    for(int i=0; i < _regions_count; i++){
        regions[i].update_region();
    }
    for(int32_t z=_search_bound.top; z<=_search_bound.bottom; z++){
        for(int32_t x=_search_bound.left; x<=_search_bound.right; x++){
            int8_t lod = points[z][x].lod;
            MBound mb(x,z);
            int32_t region_id = get_region_id_by_point(x,z);
            #ifdef NO_MERGE
            check_bigger_size(lod,0, mb);
            num_chunks +=1;
            #else
            for(int8_t s=_chunks->max_size; s>=0; s--){
                if(_chunks->sizes[s].lods[lod].meshes.size()){
                    MBound test_bound = mb;
                    if(test_bound.grow_positive(pow(2,s) - 1, _search_bound)){
                        if(check_bigger_size(lod,s,region_id, test_bound)){
                            num_chunks +=1;
                            break;
                        }
                    }
                }
            }
            #endif
        }
    }
}

//This will check if all lod in this bound are the same if not return false
// also check if the right, bottom, top, left neighbor has the same lod level
// OR on lod level up otherwise return false
// Also if All condition are correct then we can merge to bigger size
// So this will set the size of all points except the first one to -1
// Also Here we should detrmine the edge of each mesh
bool MGrid::check_bigger_size(const int8_t& lod,const int8_t& size,const int32_t& region_id, const MBound& bound) {
    for(int32_t z=bound.top; z<=bound.bottom; z++){
        for(int32_t x=bound.left; x<=bound.right; x++){
            if (points[z][x].lod != lod || points[z][x].size == -1 || get_region_id_by_point(x,z) != region_id)
            {
                return false;
            }
        }
    }
    // So these will determine if left, right, top, bottom edge should adjust to upper LOD
    bool right_edge = false;
    bool left_edge = false;
    bool top_edge = false;
    bool bottom_edge = false;
    // Check right neighbors
    int32_t right_neighbor = bound.right + 1;
    if (right_neighbor <= _search_bound.right){
        // Grab one sample from right neghbor
        int8_t last_right_lod = points[bound.bottom][right_neighbor].lod;
        // Now we don't care what is that all should be same
        for(int32_t z=bound.top; z<bound.bottom; z++){
            if(points[z][right_neighbor].lod != last_right_lod){
                return false;
            }
        }
        right_edge = (last_right_lod == lod + 1);
    }

    // Doing the same for bottom neighbor
    int32_t bottom_neighbor = bound.bottom + 1;
    if(bottom_neighbor <= _search_bound.bottom){
        int8_t last_bottom_lod = points[bottom_neighbor][bound.right].lod;
        for(int32_t x=bound.left; x<bound.right;x++){
            if(points[bottom_neighbor][x].lod != last_bottom_lod){
                return false;
            }
        }
        bottom_edge = (last_bottom_lod == lod + 1);
    }

    // Doing the same for left
    int32_t left_neighbor = bound.left - 1;
    if(left_neighbor >= _search_bound.left){
        int8_t last_left_lod = points[bound.bottom][left_neighbor].lod;
        for(int32_t z=bound.top;z<bound.bottom;z++){
            if(points[z][left_neighbor].lod != last_left_lod){
                return false;
            }
        }
        left_edge = (last_left_lod == lod + 1);
    }
    // WOW finally top neighbor
    int32_t top_neighbor = bound.top - 1;
    if(top_neighbor >= _search_bound.top){
        int8_t last_top_lod = points[top_neighbor][bound.right].lod;
        for(int32_t x=bound.left; x<bound.right; x++){
            if(points[top_neighbor][x].lod != last_top_lod){
                return false;
            }
        }
        top_edge = (last_top_lod == lod + 1);
    }
    // Now all the condition for creating this chunk with this size is true
    // So we start to build that
    // Top left corner will have one chunk with this size
    RenderingServer* rs = RenderingServer::get_singleton();
    for(int32_t z=bound.top; z<=bound.bottom; z++){
        for(int32_t x=bound.left;x<=bound.right;x++){
            if(z==bound.top && x==bound.left){
                points[z][x].size = size;
                int8_t edge_num = get_edge_num(left_edge,right_edge,top_edge,bottom_edge);
                RID mesh = _chunks->sizes[size].lods[lod].meshes[edge_num];
                if(points[z][x].mesh != mesh){
                    points[z][x].mesh = mesh;
                    if(points[z][x].has_instance()){
                        remove_instance_list.append(points[z][x].instance);
                        points[z][x].instance = RID(); 
                    }
                    MRegion* region = get_region_by_point(x,z);
                    region->insert_lod(lod);
                    points[z][x].create_instance(get_world_pos(x,0,z), _scenario, region->get_material_rid());
                    rs->instance_set_visible(points[z][x].instance, false);
                    rs->instance_set_base(points[z][x].instance, mesh);
                    update_mesh_list.append(points[z][x].instance);
                }
            } else {
                points[z][x].size = -1;
                if(points[z][x].has_instance()){
                    remove_instance_list.append(points[z][x].instance);
                    points[z][x].instance = RID();
                    points[z][x].mesh = RID();
                }
            }
        }
    }
    return true;
}

int8_t MGrid::get_edge_num(const bool& left,const bool& right,const bool& top,const bool& bottom) {
    if(!left && !right && !top && !bottom){
        return M_MAIN;
    }
    if(left && !right && !top && !bottom){
            return M_L;
    }
    if(!left && right && !top && !bottom){
            return M_R;
    }
    if(!left && !right && top && !bottom){
            return M_T;
    }
    if(!left && !right && !top && bottom){
            return M_B;
    }
    if(left && !right && top && !bottom){
            return M_LT;
    }
    if(!left && right && top && !bottom){
            return M_RT;
    }
    if(left && !right && !top && bottom){
            return M_LB;
    }
    if(!left && right && !top && bottom){
            return M_RB;
    }
    if(left && right && top && bottom){
            return M_LRTB;
    }
    UtilityFunctions::print("Error Can not find correct Edge");
    UtilityFunctions::print(left, " ", right, " ", top, " ", bottom);
    return -1;
}


void MGrid::update_meshes() {
    for(int i=0; i < _regions_count; i++){
        regions[i].apply_update();
    }
    RenderingServer* rs = RenderingServer::get_singleton();
    for(RID rm: remove_instance_list){
        rs->free_rid(rm);
    }
    for(RID add : update_mesh_list){
        rs->instance_set_visible(add, true);
    }
    update_count++;
    total_add+= update_mesh_list.size();
    total_remove += remove_instance_list.size();
    total_chunks += num_chunks;
    if(remove_instance_list.size() != 0 || update_mesh_list.size() != 0){
        uint64_t a_add = total_add/update_count;
        uint64_t a_remove = total_remove/update_count;
        uint64_t a_chunks = total_chunks/update_count;
        UtilityFunctions::print(
            "a_add ",a_add,
            " a_remove ",a_remove,
            " a_chunks ",a_chunks
        );
    }
}

Ref<ShaderMaterial> MGrid::get_material() {
    return _material;
}



void MGrid::set_material(Ref<ShaderMaterial> material) {
    /*
    if(is_dirty){
        RenderingServer* rs = RenderingServer::get_singleton();
        for(int32_t z=_search_bound.top; z <=_search_bound.bottom; z++){
            for(int32_t x=_search_bound.left; x <=_search_bound.right; x++){
                UtilityFunctions::print("HHHHH ");
                if(points[z][x].has_instance()){
                    RID instance = points[z][x].instance;
                    rs->instance_geometry_set_material_override(instance, material->get_rid());
                }
            }
        }
    }
    */
    _material = material;
}