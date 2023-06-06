#ifndef MGRID
#define MGRID

//#define NO_MERGE

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>


#include "mchunks.h"
#include "mconfig.h"
#include "mbound.h"


using namespace godot;





// size -1 means it has been merged
// lod -1 means it is out of range
// lod -2 means it should be droped and never been drawn
struct MPoint
{
    RID instance = RID();
    RID mesh = RID();
    int8_t lod = -1;
    int8_t size=0;

    _FORCE_INLINE_ bool has_instance(){
        return instance != RID();
    }
    void create_instance(const Vector3& pos,const RID& scenario,const RID& material){
        Transform3D xform(Basis(), pos);
        RenderingServer* rs = RenderingServer::get_singleton();
        instance = rs->instance_create();
        rs->instance_set_scenario(instance, scenario);
        rs->instance_set_transform(instance, xform);
        if(material != RID())
            rs->instance_geometry_set_material_override(instance, material);
        
    }

    ~MPoint(){
        RenderingServer::get_singleton()->free_rid(instance);
    }
};



class MGrid : public Object {
    GDCLASS(MGrid, Object);
    private:
    bool current_update = true;
    bool is_dirty = false;
    MChunks* _chunks;
    MGridPos _size;
    MBound _grid_bound;
    MBound _search_bound;
    MBound _last_search_bound;
    MGridPos _cam_pos;
    Vector3 _cam_pos_real;
    MGridPos _lowest_distance;
    RID _scenario;
    int32_t num_chunks = 0;
    int32_t chunk_counter = 0;
    
    Vector<RID> remove_instance_list;
    Vector<RID> update_mesh_list;
    RID _material;




    protected:
    static void _bind_methods(){};

    public:
    PackedInt32Array lod_distance;
    Vector3 offset;
    int32_t max_range = 32;
    //int32_t chunk_size = M_BASE_SIZE;
    MPoint** points;
    MGrid();
    ~MGrid();
    void clear();
    bool is_created();
    MGridPos get_size();
    void set_scenario(RID scenario);
    void create(const int32_t &width,const int32_t& height, MChunks* chunks);
    Vector3 get_world_pos(const int32_t &x,const int32_t& y,const int32_t& z);
    Vector3 get_world_pos(const MGridPos& pos);
    MGridPos get_grid_pos(const Vector3& pos);
    int8_t get_lod_by_distance(const int32_t& dis);
    void set_cam_pos(const Vector3& cam_world_pos);
    void update_search_bound();
    void cull_out_of_bound();
    void update_lods();
    void merge_chunks();
    bool check_bigger_size(const int8_t& lod,const int8_t& size, const MBound& bound);
    int8_t get_edge_num(const bool& left,const bool& right,const bool& top,const bool& bottom);
    void update_meshes();


    void set_material(RID material);
    RID get_material();

};
#endif