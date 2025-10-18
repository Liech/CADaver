class_name vox_triangulation extends Node3D

@onready var mesh : MeshInstance3D = $Mesh

var volume : VoxelShape;

func _ready()->void:
	var m = volume.get_vox_triangulation_blocky().get_array_mesh()
	mesh.mesh = m
	for x in mesh.get_children():
		mesh.remove_child(x)
		x.queue_free()
	mesh.create_trimesh_collision()
