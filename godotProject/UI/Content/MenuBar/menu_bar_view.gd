class_name ViewMenu extends PopupMenu

@export var bar : ApplicationMenuBar;

var extra_window_scene : PackedScene = load("res://UI/ExtraWindow/extra_window.tscn")

var index_fullscreen     = 0;
var index_show_tabs      = 1;
var index_backgroundclr  = 2;
var index_add_window     = 3;

func _ready():
	RenderingServer.set_default_clear_color(Hub.background_color)

func _on_index_pressed(index: int) -> void:
	match index:
		index_fullscreen:
			toggle_fullscreen()	
		index_show_tabs:
			Hub.show_tabs = !Hub.show_tabs;
			set_item_checked(index_show_tabs,Hub.show_tabs)
			Hub.show_tabs_changed.emit()
		index_add_window:
			add_extra_window();
		index_backgroundclr:
			choose_background_color()

func add_extra_window()->void:
	var window := extra_window_scene.instantiate()
	Hub.root_node.add_child(window);
	

func toggle_fullscreen() -> void:		
	bar.window.toggle_fullscreen.emit()

func set_fullscreen(on:bool)-> void:
	set_item_checked(index_fullscreen,on);

func choose_background_color()->void:
	Hub.background_color = await Color_Picker_Popup.make(Hub.background_color)
	RenderingServer.set_default_clear_color(Hub.background_color)
