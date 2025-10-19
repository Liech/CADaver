class_name triangulate_cad_converter extends converter

func get_converter_name()->String:
	return "Triangulate"

func convert_drawing(input : Drawing) -> Drawing:
	var s = input as DrawingCAD
	var newShape = s.shape.get_cad_triangulation(0.1)
	var result = DrawingMESH.new()
	result.save_path = s.save_path
	result.name = s.name
	result.shape = newShape;
	return result

func execute_dialog() -> void:
	pass
