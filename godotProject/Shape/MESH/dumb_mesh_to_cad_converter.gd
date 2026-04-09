class_name dumb_mesh_to_cad_converter extends converter

func get_converter_name()->String:
	return "To CAD (dumb)"

func convert_drawing(input : Drawing) -> Drawing:
	var s = input as DrawingMESH
	var newShape = s.shape.to_cad_dumb()
	var result = DrawingCAD.new()
	result.save_path = s.save_path
	result.draw_name = s.draw_name
	result.shape = newShape;
	return result

func execute_dialog(_input : Drawing) -> export_dialog.result_state:
	return export_dialog.result_state.Success
