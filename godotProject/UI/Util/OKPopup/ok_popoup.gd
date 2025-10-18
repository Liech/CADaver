class_name OKPopup extends Control

@export var description : String  = "Operation Failed!";

@onready var description_label = $PanelContainer2/PanelContainer/MarginContainer/VBoxContainer/Title

signal dialog_finished;

static func make(inputhint : String) -> void:
	ExtraWindow.disable_all()
	var dlg := preload("res://UI/Util/OKPopup/OKPopoup.tscn").instantiate()
	dlg.description = inputhint;
	Hub.root_node.add_child(dlg)
	await dlg.dialog_finished;
	dlg.queue_free()
	ExtraWindow.enable_all()

func _ready() -> void:
	description_label.text = description

func _on_yes_pressed() -> void:
	dialog_finished.emit()
