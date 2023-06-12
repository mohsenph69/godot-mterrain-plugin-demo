@tool
extends EditorPlugin

var raw_img_importer = null
var raw_tex_importer = null

func _enter_tree():
	raw_img_importer = load("res://addons/m_terrain/importer/raw16_img.gd").new()
	raw_tex_importer = load("res://addons/m_terrain/importer/raw16_tex.gd").new()
	add_import_plugin(raw_img_importer)
	add_import_plugin(raw_tex_importer)


func _exit_tree():
	remove_import_plugin(raw_img_importer)
	remove_import_plugin(raw_tex_importer)
	raw_img_importer = null
	raw_img_importer = null


