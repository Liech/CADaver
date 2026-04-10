class_name bool_export_uielement extends Control

@export var title : String;
@export var content : bool;
@export var target : Node;

@onready var t : Label   = $Title
@onready var Val : CheckBox = $Val

static func make(v : Dictionary, input_target : Node) -> Control:
	var result = preload("res://UI/Util/ExportDialog/UIElements/Bool/bool_export_uielement.tscn").instantiate() as bool_export_uielement
	result.title = v.name
	result.content = input_target.get(v.name)
	result.target = input_target;
	return result

func _ready() -> void:
	t.text = title
	Val.button_pressed = content;
	
func update_target()->void:
	target.set(title,content);

func _on_val_toggled(toggled_on: bool) -> void:
	content = toggled_on
	update_target()
