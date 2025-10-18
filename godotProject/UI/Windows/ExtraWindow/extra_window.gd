class_name ExtraWindow extends Window

signal fullscreen_mode_set(on:bool);

func _on_window_content_toggle_fullscreen() -> void:
	if (mode == MODE_FULLSCREEN):
		mode = MODE_WINDOWED
		fullscreen_mode_set.emit(false)
	else:
		mode = MODE_FULLSCREEN
		fullscreen_mode_set.emit(true)
	
func _notification(what : int):
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		queue_free()

func _on_tree_entered() -> void:
	Hub.extra_windows.append(self)
	

func _on_tree_exited() -> void:
	Hub.extra_windows.erase(self)

static func disable_all():
	for x in Hub.extra_windows:
		x.visible = false
		
static func enable_all():
	for x in Hub.extra_windows:
		x.visible = true
