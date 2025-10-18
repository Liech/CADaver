class_name vox_converter extends converter

var resolution : Vector3i = Vector3i(100,100,100)

func get_converter_name()->String:
	return "Voxelizer"

func convert_drawing(input : Drawing) -> Drawing:
	if (input is DrawingCAD):
		var s = input as DrawingCAD
		var newShape = s.shape.get_cad_triangulation().to_vox(resolution)
		var result = DrawingMESH.new()
		result.save_path = s.save_path
		result.name = s.name
		result.shape = newShape;
		return result
	elif (input is DrawingMESH):
		var s = input as DrawingMESH
		var newShape = s.shape.to_vox(resolution)
		var result = DrawingMESH.new()
		result.save_path = s.save_path
		result.name = s.name
		result.shape = newShape;
		return result
	else:
		return null

func execute_dialog() -> void:
	pass
