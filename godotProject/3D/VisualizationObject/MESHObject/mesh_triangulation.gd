class_name mesh_triangulation extends Node3D

@onready var mesh : MeshInstance3D = $Mesh

var shape : TriangleShape;

func _ready()->void:
	var m = shape.get_array_mesh()
	mesh.mesh = m
	for x in mesh.get_children():
		mesh.remove_child(x)
		x.queue_free()
	mesh.create_trimesh_collision()
