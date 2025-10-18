class_name main_camera extends Camera3D

@onready var selection : object_selection = $Selection
@onready var context_menu : main_context_menu = $ContextMenu

@export var child_pool : Node;
@export var sensitivity: float = 0.005
@export var zoom_speed: float = 0.05
@export var pan_speed: float = 0.1
@export var start_rotation : Transform3D;

var camera_rotation : Transform3D;

signal rotation_changed()

var zoom: float = 5.0
var offset : Vector3;
var bounding_box : AABB;
var is_dragging: bool = false
var last_mouse_pos: Vector2
var is_panning: bool = false
var pan_origin: Vector2
var there_was_motion : bool = false

func _ready() -> void:
	size = zoom
	context_menu.child_pool = child_pool

func _process(_delta: float) -> void:
	pass

func _input(event):
	if event is InputEventMouseButton:
		if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_WHEEL_UP:
			zoom = zoom * (1-zoom_speed);
			update_camera()
		elif event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
			zoom = zoom * (1+zoom_speed);
			update_camera()
		elif event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.double_click:
			var result = pick(event.position)
			if result and result.has("position"):
				offset = result["position"]
				update_camera()
		elif event.button_index == MOUSE_BUTTON_LEFT:
			if event.pressed:
				is_dragging = true
				last_mouse_pos = event.position
				there_was_motion = false
			else:
				is_dragging = false
				if (!there_was_motion):
					selection.select_object(event.position, event.get_modifiers_mask())
		elif event.button_index == MOUSE_BUTTON_RIGHT:
			if event.pressed:
				is_panning = true
				pan_origin = event.position
				there_was_motion = false
			else:
				is_panning = false
				if (!there_was_motion):
					context_menu.selected = selection.current_selected 
					context_menu.position = DisplayServer.mouse_get_position()
					context_menu.show()

	if event is InputEventMouseMotion and is_dragging:
		there_was_motion = true
		var current_mouse_pos = event.position
		var delta : Vector2 = current_mouse_pos - last_mouse_pos
		last_mouse_pos = current_mouse_pos
		camera_rotation = camera_rotation.rotated_local(Vector3(0,1,0),-delta.x * sensitivity)
		camera_rotation = camera_rotation.rotated_local(Vector3(1,0,0),-delta.y * sensitivity)
		update_camera()
		rotation_changed.emit()
	elif event is InputEventMouseMotion and is_panning:
		var delta = event.relative * pan_speed * get_process_delta_time()
		offset = offset + (transform.basis.x * -delta.x) * zoom
		offset = offset + (transform.basis.y * delta.y) * zoom
		update_camera()
		there_was_motion = true
	elif event is InputEventMouseMotion:
		selection.hover_object(event.position)
		
func _on_view_cube_transform_changed(trans: Transform3D) -> void:
	camera_rotation = trans
	update_camera()
	
func reset_view()->void:
	offset = bounding_box.get_center()
	zoom = bounding_box.get_longest_axis_size()
	
func update_camera()->void:
		transform = Transform3D.IDENTITY.translated(offset)*camera_rotation*Transform3D.IDENTITY.translated(Vector3(0,0,zoom))
		size = zoom
		look_at(offset,camera_rotation.basis.y)

func set_aabb(aabb : AABB)->void:
	bounding_box = aabb;
	reset_view()
	camera_rotation = start_rotation.inverse()
	update_camera()
	rotation_changed.emit()
	
func _on_view_cube_reset_view() -> void:
	reset_view()

func pick(pos : Vector2) -> Dictionary:				
	var ray = project_ray_origin(pos)
	var to = ray + project_ray_normal(pos) * 1000 # Ray length
	var space_state = get_world_3d().get_direct_space_state()
	var query = PhysicsRayQueryParameters3D.create(ray, to)
	return space_state.intersect_ray(query)
	
func reset()->void:
	selection.reset()
