class_name Hierarchy extends Control

@onready var tree : Tree = $PanelContainer/MarginContainer/VBoxContainer/MarginContainer/Hierarchy
@onready var content : PanelContainer = $PanelContainer
@onready var window : Window = $Window
@onready var windowPanel : PanelContainer = $Window/PanelContainer

@export var drawing : Drawing;

var factory : CADSceneFactory = CADSceneFactory.new();

var start_size : Vector2;

var root : TreeItem;

func _ready()->void:
	root = tree.create_item()
	var child1 = tree.create_item(root)
	child1.set_text(0,"Kartoffel")

func drawing_changed()->void:
	reset_children()
	if (!drawing):
		return
	if (drawing.shape is CADShape):
		build_CAD_children()

func build_CAD(shape :CADShape, t : TreeItem):
	var current = tree.create_item(t);
	var n := shape.get_cad_type()
	if (shape.get_cad_type()=="TopAbs_SOLID"):
		n = (shape as CADSolid).get_cad_name()
	current.set_text(0,n)
	var childs := shape.get_cad_children()	
	for child in childs:
		build_CAD(child,current)

func build_CAD_children()->void:
	build_CAD(drawing.shape,root)
	for x in root.get_children():
		x.set_collapsed_recursive(true)

func reset_children()->void:
	tree.clear()
	root = tree.create_item()

func _on_button_pressed() -> void:
	if (content.get_parent() != self):
		_on_window_close_requested()
		return
	start_size = content.size
	remove_child(content)
	windowPanel.add_child(content)
	window.visible = true

func _on_window_close_requested() -> void:
	windowPanel.remove_child(content)
	add_child(content)
	content.size = start_size
	window.visible = false
	
