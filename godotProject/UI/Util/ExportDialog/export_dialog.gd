class_name export_dialog extends Node

var target : Node;
var succes : result_state = result_state.Success
enum result_state { Success, Abort, Failed}

signal dialog_finished(result_state);

@onready var elements : VBoxContainer = $margin/V/Elements

var base_properties := ["Node",
"_import_path",
"name",
"unique_name_in_owner",
"scene_file_path",
"owner",
"multiplayer",
"Process",
"process_mode",
"process_priority",
"process_physics_priority",
"Thread Group",
"process_thread_group",
"process_thread_group_order",
"process_thread_messages",
"Physics Interpolation",
"physics_interpolation_mode",
"Auto Translate",
"auto_translate_mode",
"Editor Description",
"editor_description",
"script"]

static func make(node : Node) -> result_state:
	var dlg := preload("res://UI/Util/ExportDialog/export_dialog.tscn").instantiate()
	dlg.target = node;
	dlg.position = Hub.root_node.get_viewport().get_mouse_position()
	dlg.position.x += 15	
	Hub.root_node.add_child(dlg)	
	var success : result_state = await dlg.dialog_finished;
	dlg.queue_free()
	return success

func _ready()->void:
	if (!target):
		return;
	var exports = target.get_property_list()
	for export in exports:
		var export_name : String = export["name"];
		if (base_properties.has(export_name) or name.ends_with(".gd")):
			continue
		var elm = export_uielement_factory.make(export, target)
		if (elm):
			elements.add_child(elm)

func _on_cancel_pressed() -> void:
	dialog_finished.emit(result_state.Abort)

func _on_ok_pressed() -> void:
	dialog_finished.emit(result_state.Success)
