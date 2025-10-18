class_name mesh_io extends shape_io

func get_shape_name() -> String:
	return "Triangular Mesh"


func load_drawing() -> Drawing:
	var result := DrawingMESH.new()
	result.draw_name =  path_util.get_file_name_without_extension(filename);
	result.save_path = filename;
	var success : bool = result.load_from_file();
	if (!success):
		message = "Loading failed"
		return null;
	return result;

func make_save_file_dialog():
	var dlg = SaveFileDialog.new()
	dlg.add_filter("STL File", ["stl"])
	dlg.set_save_file_name(drawing.draw_name + ".stl");
	dlg.set_path(path_util.get_path_without_filename(drawing.save_path));
	return dlg
	
func save_drawing() -> bool:
	var extension := path_util.get_extension(filename).to_lower()
	if (extension == ".stl"):
		drawing.shape.save_tri_to_file(filename)
	else:
		message = "Unkown extension"
		return false
	return true

func get_converter() -> Array[converter]:
	return [vox_converter.new()]
