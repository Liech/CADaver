class_name mesh_io extends shape_io


func load_drawing() -> Drawing:
	var result := DrawingMESH.new()
	result.draw_name =  path_util.get_file_name_without_extension(filename);
	result.save_path = filename;
	var success : bool = result.load_from_file();
	if (!success):
		return null;
	return result;
