class_name view_cube_control extends Control

signal transform_changed(trans : Transform3D);
signal reset_view()

@export var cam : Camera3D 

@onready var cube : Node3D = $VBoxContainer/SubViewportContainer/SubViewport/ViewCube
@onready var view_button : Button = $VBoxContainer/HBoxContainer/View
@onready var ortho_button : Button = $VBoxContainer/HBoxContainer/Orthographic

func _ready() -> void:
	ortho_button.button_pressed = true

func _process(_delta: float) -> void:
	pass

func _on_view_button_down() -> void:
	cam.projection = Camera3D.ProjectionType.PROJECTION_PERSPECTIVE
	ortho_button.button_pressed = false;

func _on_orthographic_button_down() -> void:
	cam.projection = Camera3D.ProjectionType.PROJECTION_ORTHOGONAL
	view_button.button_pressed = false;
	
func _on_view_cube_transformation_changed(trans: Transform3D) -> void:
	transform_changed.emit(trans.inverse())
	
func _on_camera_rotation_changed() -> void:
	var trans := cam.transform.inverse();
	trans = trans.translated(-trans.origin);
	cube.transform = trans;
	

func _on_rotate_90_counter_pressed() -> void:
	cube.rotate_z(-PI/4)
	transform_changed.emit(cube.transform.inverse())

func _on_rotate_90_pressed() -> void:
	cube.rotate_z(PI/4)
	transform_changed.emit(cube.transform.inverse())


func _on_view_cube_reset_view() -> void:
	reset_view.emit()
