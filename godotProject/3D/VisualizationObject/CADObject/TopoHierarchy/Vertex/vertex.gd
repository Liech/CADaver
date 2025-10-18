class_name vertex_scene extends Node3D

var vertex : CADVertex;


func _ready()->void:
	position = vertex.get_cad_position()
