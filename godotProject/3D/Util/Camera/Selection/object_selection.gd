class_name object_selection extends Node3D

@export var camera : Camera3D

var current_hover : Selectable = null
var current_selected : Array[Selectable] = []

func hover_object(pos : Vector2):
	var result = camera.pick(pos)
	var success : bool = false
	if result and result.has("collider"):
		var col : Node = result["collider"]
		if (col is Selectable):
			if (current_hover and !current_selected.has(current_hover)):
				current_hover.reset_object()
			if(!current_selected.has(col)):
				col.hover_object()
			success = true
			current_hover = col
	if (!success):
		if (current_hover and !current_selected.has(current_hover)):
			current_hover.reset_object()
		current_hover = null

func select_object(_pos : Vector2, modifier : KeyModifierMask):
	var ctrl : bool = modifier & KEY_MASK_CTRL;
	var shift : bool = modifier & KEY_MASK_SHIFT;
	
	if (current_hover):
		if (!current_selected.has(current_hover) or ctrl):
			if (current_selected and !shift and !ctrl):
				clear_selection()
			if (!ctrl):
				current_selected.push_back(current_hover);
				current_hover.select_object()
			else:
				current_selected.erase(current_hover);
				current_hover.hover_object()
				
	elif (current_selected and !current_hover):
		clear_selection()

func clear_selection()->void:
	for x in current_selected:
		x.reset_object()
	current_selected = []
	
func reset()->void:
	current_selected = []
	current_hover = null
