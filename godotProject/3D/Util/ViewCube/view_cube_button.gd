class_name view_cube_button extends StaticBody3D

@onready var hover_mesh : MeshInstance3D = $HoverMesh

@export var trans : Transform3D

func _ready() -> void:
	#hover_mesh.scale = scale
	pass


func hover()->void:
	hover_mesh.visible = true
	
func unhover()->void:
	hover_mesh.visible = false
