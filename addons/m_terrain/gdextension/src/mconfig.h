#ifndef MCONFIG
#define MCONFIG

#define M_DEBUG
//#undef M_DEBUG

#define M_SIZE_LIST_STRING "8,16,32,64,128,256,512,1024"
#define M_SIZE_LIST        {8,16,32,64,128,256,512,1024}
#define M_H_SCALE_LIST_STRING "0.25,0.5,1,2,4,8,16,32"
#define M_H_SCALE_LIST        {0.25,0.5,1,2,4,8,16,32}

#define M_MESH_FOLDER godot::String("res://addons/m_terrain/meshes/")


//#define M_BASE_SIZE 32
//#define M_SIZE_COUNT 6
#define M_SIZES {32, 64, 128, 256,512,1024}
#define M_SIZES_LOD {{true,true,true,true,true},\
                     {false,true,true,true,true},\
                     {false,false,true,true,true},\
                      {false,false,false,true,true},\
                      {false,false,false,false,true}}

//#define M_MAX_LOD 5
#define M_DEF_LOD_DISTANC {2,3,4,5,6,7,8,9,10,11,12,13}
#define M_MAX_DEF_LOD_DISTANC 12


#define M_MAIN 0
#define M_L 1
#define M_R 2
#define M_T 3
#define M_B 4
#define M_LT 5
#define M_RT 6
#define M_LB 7
#define M_RB 8
#define M_LRTB 9
#define M_MAX_EDGE 10

#endif