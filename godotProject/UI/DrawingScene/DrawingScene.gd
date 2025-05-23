class_name DrawingScene extends Control

@onready var camera : Camera3D = $SubViewportContainer/Viewport/MainCamera
@onready var child_pool : Node = $SubViewportContainer/Viewport/ChildPool
@onready var viewport : Viewport = $SubViewportContainer/Viewport
@onready var hierarchy : Hierarchy = $Hierarchy

var cad_vis : PackedScene = preload("res://3D/CADObject/CADVisualization.tscn")

var drawing : Drawing = null;

func _ready()->void:
	if (Hub.file.drawings.size() > 0):
		_on_tab_bar_drawing_changed(0)

func _on_dirtymaker_pressed() -> void:
	if (drawing and not drawing.dirty):
		drawing.dirty = true;
		Hub.file.dirty_changed.emit();
	
func reset()->void:
	for n in child_pool.get_children():
		child_pool.remove_child(n)
		n.queue_free()
	camera.reset()
		
func _on_tab_bar_drawing_changed(index: Variant) -> void:
	if (!child_pool):
		return;
	reset()
	if (index != -1):
		var d := Hub.file.drawings[index]
		var vis := cad_vis.instantiate() as CADVisualization
		child_pool.add_child(vis)
		vis.set_drawing(d)		
		drawing = d;
		camera.set_aabb(vis.bounding_box)
	hierarchy.drawing = drawing;
	hierarchy.drawing_changed()
