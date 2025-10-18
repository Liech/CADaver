class_name face_scene extends Node3D

var face_material : Material = preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Face/face_material.tres")
var face_hover_script : Script = preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Face/face_collider.gd")

@onready var mesh : MeshInstance3D = $Mesh

var face : CADFace;

func _ready()->void:
	var cadmesh = face.get_cad_triangulation(0.1).get_array_mesh()
	mesh.mesh = cadmesh
	for x in mesh.get_children():
		mesh.remove_child(x)
		x.queue_free()
	mesh.create_trimesh_collision()
	if (mesh.get_child_count() > 0):
		mesh.get_child(0).set_script(face_hover_script)
		mesh.get_child(0).start()
