class_name mesh_converter extends converter

func get_converter_name()->String:
	return "Meshify"

func convert_drawing(input : Drawing) -> Drawing:
	if (input is DrawingCAD):
		var s = input as DrawingCAD
		var newShape = s.shape.get_cad_triangulation()
		var result = DrawingMESH.new()
		result.save_path = s.save_path
		result.name = s.name
		result.shape = newShape;
		return result
	elif (input is DrawingVOX):
		var s = input as DrawingVOX
		var newShape = s.shape.get_vox_triangulation_round()
		var result = DrawingMESH.new()
		result.save_path = s.save_path
		result.name = s.name
		result.shape = newShape;
		return result
	else:
		return null
	return null

func execute_dialog() -> void:
	pass
