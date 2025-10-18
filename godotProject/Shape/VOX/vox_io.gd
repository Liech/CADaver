class_name vox_io extends mesh_io



func load_drawing() -> Drawing:
	var result := DrawingVOX.new()
	result.draw_name =  path_util.get_file_name_without_extension(filename);
	result.save_path = filename;
	var success : bool = result.load_from_file();
	if (!success):
		message = "Loading failed"
		return null;
	return result;
