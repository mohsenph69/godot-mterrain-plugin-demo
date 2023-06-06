extends Object
class_name MTerrainChunkGenerator

### 6 LOD in Total from 0 to 5

var path = "res://addons/m_terrain/meshes/"

var lod_mat = [
	preload("res://addons/m_terrain/debug_mat/lod0.material"),
	preload("res://addons/m_terrain/debug_mat/lod1.material"),
	preload("res://addons/m_terrain/debug_mat/lod2.material"),
	preload("res://addons/m_terrain/debug_mat/lod3.material"),
	preload("res://addons/m_terrain/debug_mat/lod4.material"),
	preload("res://addons/m_terrain/debug_mat/lod5.material")
]


var h_scale_lod = {
	1:0,
	2:1,
	4:2,
	8:3,
	16:4,
}

var build_info := {
	32:[1,2,4,8,16],
	64:[2,4,8,16],
	128:[4,8,16],
	256:[8,16],
	512:[16],
	1024:[16]
}


func generate_all():
	var mesh:ArrayMesh= null
	for size in build_info.keys():
		for h_scale in build_info[size]:
			var file_path = path + str(size) + "_" + str(h_scale_lod[h_scale])
			for i in range(0,10):
				var final_path = file_path + "_" + str(i) +".res"
				if i == 0:
					mesh = generate_chunk(size,h_scale,false,false,false,false)
				elif i == 1:
					mesh = generate_chunk(size,h_scale,true,false,false,false)
				elif i == 2:
					mesh = generate_chunk(size,h_scale,false,true,false,false)
				elif i == 3:
					mesh = generate_chunk(size,h_scale,false,false,true,false)
				elif i == 4:
					mesh = generate_chunk(size,h_scale,false,false,false,true)
				elif i == 5:
					mesh = generate_chunk(size,h_scale,true,false,true,false)
				elif i == 6:
					mesh = generate_chunk(size,h_scale,false,true,true,false)
				elif i == 7:
					mesh = generate_chunk(size,h_scale,true,false,false,true)
				elif i == 8:
					mesh = generate_chunk(size,h_scale,false,true,false,true)
				elif i == 9:
					mesh = generate_chunk(size,h_scale,true,true,true,true)
				
				
				mesh.surface_set_material(0, lod_mat[h_scale_lod[h_scale]])
				mesh.custom_aabb = AABB(Vector3(0,-5000,0), Vector3(size,10000,size) )
				ResourceSaver.save(mesh,final_path)
				
				


func generate_chunk(size:float, h_scale:float,el=false, er=false,et=false, eb=false)->ArrayMesh:
	var vertices:PackedVector3Array
	var normals:PackedVector3Array
	var tangents:PackedFloat32Array
	var uvs:PackedVector2Array
	var indices:PackedInt32Array
	if fmod(size, h_scale) != 0 :
		print("fmod(size, h_scale) is not zero")
		return
	var vert_num:int = size/h_scale + 1
	
	var index = -1
	var rows := []
	for y in range(0, vert_num):
		var row :=[]
		for x in range(0, vert_num):
			index += 1
			var is_main = fmod(fmod(x,2)+fmod(y,2), 2) == 0
			var drop_x = false
			var drop_y = false
			if el and x == 0:
				drop_x = true
			if er and x == vert_num - 1:
				drop_x = true
			if et and y == 0:
				drop_y = true
			if eb and y == vert_num - 1:
				drop_y = true
			################
			if (drop_x or drop_y) and not is_main:
				index -= 1
				row.append(-1)
				continue
			row.append(index)
			vertices.append(Vector3(x,0,y)*h_scale)
			normals.append(Vector3(0,1,0))
			tangents.append_array([1.0,0,0,1.0])
			var uv = Vector2(float(x),float(y))/float(vert_num-1)
			uvs.append(uv)
		rows.append(row)
	
	for y in range(0, vert_num):
		for x in range(0, vert_num):
			var is_main = fmod(x,2) == 0 and fmod(y,2) == 0
			index = rows[y][x]
			if is_main and not (x == vert_num-1 or y==vert_num-1 or x == 0 or y == 0):
				indices.append_array([index,rows[y][x-1],rows[y-1][x-1]])

				indices.append_array([index,rows[y-1][x-1],rows[y-1][x]])
				indices.append_array([index,rows[y-1][x],rows[y-1][x+1]])
				indices.append_array([index,rows[y-1][x+1],rows[y][x+1]])
				indices.append_array([index,rows[y][x+1],rows[y+1][x+1]])
				indices.append_array([index,rows[y+1][x+1],rows[y+1][x]])
				indices.append_array([index,rows[y+1][x],rows[y+1][x-1]])
				indices.append_array([index,rows[y+1][x-1],rows[y][x-1]])
			
			is_main = false
			if x == 1:
				if y != 0 and y != vert_num -1:
					if fmod(y,2) == 0:
						indices.append_array([index,rows[y][x-1],rows[y-1][x]])
						indices.append_array([index,rows[y+1][x],rows[y][x-1]])
					elif el:
						indices.append_array([index,rows[y+1][x-1],rows[y-1][x-1]])
					else:
						indices.append_array([index,rows[y][x-1],rows[y-1][x-1]])
						indices.append_array([index,rows[y+1][x-1],rows[y][x-1]])
			if y == 1:
				if x != 0 and x != vert_num - 1:
					if fmod(x,2) == 0:
						indices.append_array([index,rows[y][x-1],rows[y-1][x]])
						indices.append_array([index,rows[y-1][x],rows[y][x+1]])
					elif et:
						indices.append_array([index,rows[y-1][x-1],rows[y-1][x+1]])
					else:
						indices.append_array([index,rows[y-1][x-1],rows[y-1][x]])
						indices.append_array([index,rows[y-1][x],rows[y-1][x+1]])
			if x == vert_num - 2:
				if y != 0 and y != vert_num - 1:
					if fmod(y,2) == 0:
						indices.append_array([index,rows[y-1][x],rows[y][x+1]])
						indices.append_array([index,rows[y][x+1],rows[y+1][x]])
					elif er:
						indices.append_array([index,rows[y-1][x+1],rows[y+1][x+1]])
					else:
						indices.append_array([index,rows[y-1][x+1],rows[y][x+1]])
						indices.append_array([index,rows[y][x+1],rows[y+1][x+1]])
			if y == vert_num - 2:
				if x != 0 and x != vert_num - 1:
					if fmod(x,2) == 0:
						indices.append_array([index,rows[y][x+1],rows[y+1][x]])
						indices.append_array([index,rows[y+1][x],rows[y][x-1]])
					elif eb:
						indices.append_array([index,rows[y+1][x+1],rows[y+1][x-1]])
					else:
						indices.append_array([index,rows[y+1][x+1],rows[y+1][x]])
						indices.append_array([index,rows[y+1][x],rows[y+1][x-1]])
	
	var arr_mesh = ArrayMesh.new()
	var surface_array:Array
	surface_array.resize(Mesh.ARRAY_MAX)
	surface_array[Mesh.ARRAY_VERTEX] = vertices
	surface_array[Mesh.ARRAY_INDEX] = indices
	surface_array[Mesh.ARRAY_NORMAL] = normals
	surface_array[Mesh.ARRAY_TANGENT] = tangents
	surface_array[Mesh.ARRAY_TEX_UV] = uvs
	arr_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, surface_array)
	return arr_mesh
