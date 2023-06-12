extends EditorImportPlugin


func _get_importer_name():
	return "mterrain.raw16.tex"

func _get_visible_name():
	return "mterrain raw16 Texture"

func _get_recognized_extensions():
	return ["r16", "R16"]

func _get_save_extension():
	return "res"

func _get_resource_type():
	return "ImageTexture"

func _get_priority():
	return 1

func _get_import_order():
	return 0

func _get_preset_count():
	return 2

func _get_preset_name(i):
	if(i==0):
		return "width"
	elif(i==1):
		return "height"

func _get_import_options(path, preset_index):
	return [{"name":"width","default_value":0},{"name":"height","default_value":0}]

func _get_option_visibility(path, option_name, options):
	return true

func _import(source_file, save_path, options, platform_variants, gen_files):
	print("importing ", source_file)
	var tex = MRaw16.get_texture(source_file, options.width, options.height)
	if tex==null:
		tex = load("res://addons/m_terrain/importer/import_error.png")
	var filename = save_path + "." + _get_save_extension()
	return ResourceSaver.save(tex, filename)
	
