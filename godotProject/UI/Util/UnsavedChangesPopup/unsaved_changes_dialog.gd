class_name UnsavedChangesDialog extends Control

@export var hint : String  = "Drawing: new";

@onready var hint_label = $PanelContainer/VBoxContainer/MarginContainer/VBoxContainer/Hint

signal dialog_finished;


var yes_pressed : bool = false;
var no_pressed: bool = false;
var cancel_pressed: bool = false;

static func make(inputhint : String) -> UnsavedChangesDialog:
	var dlg := preload("res://UI/Util/UnsavedChangesPopup/UnsavedChangesDialog.tscn").instantiate()
	dlg.hint = inputhint;
	Hub.root_node.add_child(dlg)
	return dlg;

func _ready() -> void:
	hint_label.text = hint

func _on_yes_pressed() -> void:
	yes_pressed = true;
	dialog_finished.emit()
	
func _on_no_pressed() -> void:
	no_pressed = true;
	dialog_finished.emit()
	
func _on_cancel_pressed() -> void:
	cancel_pressed = true;
	dialog_finished.emit()
	
