class_name marching_cubes_vox_converter extends converter

func get_converter_name()->String:
	return "Smooth Triangulate"

func convert_drawing(input : Drawing) -> Drawing:
	var s = input as DrawingVOX
	if (!s):
		return null;
	var newShape = s.shape.get_vox_triangulation_round()
	var result = DrawingMESH.new()
	result.save_path = s.save_path
	result.draw_name = s.draw_name
	result.shape = newShape;
	return result
