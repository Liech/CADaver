extends Node

signal fullscreen_mode_changed(bool)
signal quit_requested();

func _ready():
	get_tree().set_auto_accept_quit(false)
	Hub.root_node = self

func _on_window_content_toggle_fullscreen() -> void:
	if (DisplayServer.window_get_mode() != DisplayServer.WINDOW_MODE_FULLSCREEN): 
		DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_FULLSCREEN)
		fullscreen_mode_changed.emit(true);
	else:
		DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_WINDOWED)
		fullscreen_mode_changed.emit(false);

func _notification(what : int):
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		quit_requested.emit()
