class_name converter extends Node

func get_converter_name()->String:
	return "CONVERTER"

func convert_drawing(_input : Drawing) -> Drawing:
	return null

func execute_dialog(_input : Drawing) -> export_dialog.result_state: 
	return export_dialog.result_state.Success # continue?
