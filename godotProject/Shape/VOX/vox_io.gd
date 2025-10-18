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

func make_save_file_dialog():
	var dlg = SaveFileDialog.new()
	dlg.add_filter("Vox File", ["vox"])
	dlg.add_filter("Blocky STL File", ["STL"])
	dlg.add_filter("Smooth stl File", ["stl"])
	dlg.set_save_file_name(drawing.draw_name + ".vox");
	dlg.set_path(path_util.get_path_without_filename(drawing.save_path));
	return dlg
	
func save_drawing() -> bool:
	var extension := path_util.get_extension(filename)
	if (extension.to_lower() == ".vox"):
		drawing.shape.save_vox_to_file(filename)
	elif (extension ==".STL"):	
		drawing.shape.get_vox_triangulation_blocky().save_tri_to_file(filename)
		drawing.dirty = false
		drawing.name = path_util.get_file_name_without_extension(filename);
	elif (extension ==".stl"):	
		drawing.shape.get_vox_triangulation_round().save_tri_to_file(filename)
		drawing.dirty = false
		drawing.name = path_util.get_file_name_without_extension(filename);
	else:
		message = "Unkown extension"
		return false
	return true
