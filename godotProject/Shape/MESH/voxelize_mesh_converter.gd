class_name voxelize_mesh_converter extends converter

@export var resolution : Vector3i = Vector3i(100,100,100);

func get_converter_name()->String:
	return "Voxelize"

func convert_drawing(input : Drawing) -> Drawing:
	var s = input as DrawingMESH
	var newShape = s.shape.to_vox(resolution)
	var result = DrawingVOX.new()
	result.save_path = s.save_path
	result.draw_name = s.draw_name
	result.shape = newShape;
	return result

func execute_dialog(_input : Drawing) -> export_dialog.result_state:
	var success :export_dialog.result_state= await export_dialog.make(self)
	if (resolution.x<= 0 or resolution.y<=0 or resolution.z <= 0):
		OKPopup.make("Conversion Failed\nNon Positive Resolution");
		return export_dialog.result_state.Failed
	if (resolution.x>> 8096 or resolution.y>> 8096 or resolution.z> 8096):
		OKPopup.make("Conversion Failed\nResolution Maximum");
		return export_dialog.result_state.Failed
	return success
