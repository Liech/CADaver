class_name view_cube extends MeshInstance3D

signal transformation_changed(trans : Transform3D)
signal reset_view()

@export var sensitivity: float = 0.01

@export var trans: Transform3D

var is_dragging: bool = false
var last_mouse_pos: Vector2
var there_was_motion : bool = false
var hover_button : view_cube_button = null

func _input(event):
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT:
			if event.pressed:
				is_dragging = true
				last_mouse_pos = event.position
				there_was_motion = false
			else:
				is_dragging = false
				if (!there_was_motion):
					var col : view_cube_button = pick(event.position)
					if(col):
						transform = col.trans
						reset_view.emit()
						transformation_changed.emit(transform)
	if event is InputEventMouseMotion and is_dragging:
		there_was_motion = true
		var current_mouse_pos = event.position
		var delta : Vector2 = current_mouse_pos - last_mouse_pos
		last_mouse_pos = current_mouse_pos
		rotate_y(delta.x * sensitivity)
		rotate_x(delta.y * sensitivity)
		if(delta.length_squared()>0):
			transformation_changed.emit(transform)
	if (event is InputEventMouseMotion):
		var col := pick(event.position)
		if (hover_button != col and hover_button):
			hover_button.unhover()
			hover_button = null
		if (col):
			hover_button = col;
			hover_button.hover()

func _process(_delta: float) -> void:
	trans = transform
		
func pick(pos : Vector2) -> view_cube_button:
	var viewport = get_viewport()
	var mouse_position = pos
	var ray_origin = viewport.get_camera_3d().project_ray_origin(mouse_position)
	var ray_direction = viewport.get_camera_3d().project_ray_normal(mouse_position)
	var space_state = get_world_3d().direct_space_state
	var query_parameters = PhysicsRayQueryParameters3D.new()
	query_parameters.from = ray_origin
	query_parameters.to = ray_origin + ray_direction * 1000 # Raycast 1000 Einheiten weit
	var result = space_state.intersect_ray(query_parameters)
	if result and result.collider and result.collider is view_cube_button:
		return result.collider
	return null
