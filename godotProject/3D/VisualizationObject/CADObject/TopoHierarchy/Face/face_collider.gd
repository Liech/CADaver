extends Selectable

var face_material : Material = preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Face/face_material.tres")
var face_material_hover : Material = preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Face/face_material_hover.tres")
var face_material_select : Material = preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Face/face_material_select.tres")

func start() -> void:
	get_parent().material_override = face_material

func hover_object()->void:
	get_parent().material_override = face_material_hover
	
func reset_object()->void:
	get_parent().material_override = face_material

func select_object()->void:
	get_parent().material_override = face_material_select
	
func get_shape()  -> CADShape:
	return get_parent().get_parent().face as CADShape
