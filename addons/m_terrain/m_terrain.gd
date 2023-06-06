@tool
extends EditorPlugin

var terrain:MTerrain
var is_cam_set = false

func _enter_tree():
	# Initialization of the plugin goes here.
	pass


func _exit_tree():
	# Clean-up of the plugin goes here.
	pass


func _forward_3d_gui_input(viewport_camera, event):
	if terrain and not is_cam_set:
		terrain.set_editor_camera(viewport_camera)
		terrain.editor_cam = viewport_camera
		is_cam_set = true
		print("Viewport camera is set")


func _handles(object):
	if object.has_method("set_editor_camera"):
		terrain = object
		print("done")
		return true
	return false
