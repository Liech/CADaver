class_name ApplicationMenuBar extends PanelContainer

@export var window : WindowContainer

@onready var view : ViewMenu = $MenuBar/View
@onready var file : FileMenu = $MenuBar/File


func _on_window_content_fullscreen_mode_set(on: bool) -> void:
	view.set_fullscreen(on)

func close_drawing_request(index: int) -> void:
	file.close_drawing(index)
	
func close_request() -> void:
	file.quit_pressed()
