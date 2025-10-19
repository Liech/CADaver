class_name vector3i_export_uielement extends Control

@export var title : String;
@export var content : Vector3i;
@export var target : Node;

@onready var t : Label   = $Title
@onready var X : SpinBox = $X
@onready var Y : SpinBox = $Y
@onready var Z : SpinBox = $Z

static func make(v : Dictionary, input_target : Node) -> Control:
	var result = preload("res://UI/Util/ExportDialog/UIElements/Vector3i/vector3i_export_uielement.tscn").instantiate() as vector3i_export_uielement
	result.title = v.name
	result.content = input_target.get(v.name)
	result.target = input_target;
	return result

func _ready() -> void:
	t.text = title
	X.value = content.x;
	Y.value = content.y;
	Z.value = content.z;
	
func update_target()->void:
	target.set(title,content);
	
func _on_x_value_changed(value: float) -> void:
	@warning_ignore("narrowing_conversion")
	content.x = value
	update_target()

func _on_y_value_changed(value: float) -> void:
	@warning_ignore("narrowing_conversion")
	content.y = value
	update_target()

func _on_z_value_changed(value: float) -> void:
	@warning_ignore("narrowing_conversion")
	content.z = value
	update_target()
