class_name Color_Picker_Popup extends Control

@export var clr : Color = Color.GRAY
@onready var picker : ColorPicker = $PanelContainer2/PanelContainer/ColorPicker

signal dialog_finished;

static func make(inputclr : Color) -> Color:
	ExtraWindow.disable_all()
	var dlg := preload("res://UI/Util/ColorPickerPopup/ColorPickerPopup.tscn").instantiate()
	dlg.clr = inputclr
	Hub.root_node.add_child(dlg)
	await dlg.dialog_finished;
	var result = dlg.picker.color
	dlg.queue_free()
	ExtraWindow.enable_all()
	return result

func _ready() -> void:
	picker.color = clr

func _on_yes_pressed() -> void:
	dialog_finished.emit()
