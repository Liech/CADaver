class_name float_export_uielement extends Control

@export var title : String;
@export var content : float;
@export var target : Node;

@onready var t : Label   = $Title
@onready var Val : SpinBox = $Val

static func make(v : Dictionary, input_target : Node) -> Control:
	var result = preload("res://UI/Util/ExportDialog/UIElements/Float/float_export_uielement.tscn").instantiate() as float_export_uielement
	result.title = v.name
	result.content = input_target.get(v.name)
	result.target = input_target;
	return result

func _ready() -> void:
	t.text = title
	Val.value = content;
	
func update_target()->void:
	target.set(title,content);
	
func _on_val_value_changed(value: float) -> void:
	content = value
	update_target()
