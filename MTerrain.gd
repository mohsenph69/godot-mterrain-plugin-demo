extends MTerrain

@export var update_u=true:
	set(input):
		update_uniforms()

# Called when the node enters the scene tree for the first time.
func _ready():
	start()
	return
	var code := material.shader.code
	var reg = RegEx.new()
	reg.compile(";\\s*//\\s*HEIGHTMAP_UV")
	var str = reg.sub(code, " = some formula for;")
	print(str)

