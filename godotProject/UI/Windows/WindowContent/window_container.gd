class_name WindowContainer extends Control

@export var menu : ApplicationMenuBar
@export var tabs : OpenDrawingsTabs  
@export var scene : DrawingScene    

signal toggle_fullscreen;
signal fullscreen_mode_set(on:bool)

func _ready():
	pass

func _on_editor_fullscreen_mode_changed(val:bool) -> void:
	fullscreen_mode_set.emit(val)

func _on_main_window_quit_requested() -> void:
	menu.close_request()


func _on_tree_entered() -> void:
	pass # Replace with function body.
