class_name cad_io extends mesh_io

func get_shape_name() -> String:
	return "CAD BREP"

func load_drawing() -> Drawing:
	var result := DrawingCAD.new()
	result.draw_name =  path_util.get_file_name_without_extension(filename);
	result.save_path = filename;
	var success : bool = result.load_from_file();
	if (!success):
		message = "Loading failed"
		return null;
	return result;

func make_save_file_dialog():
	var dlg = SaveFileDialog.new()
	dlg.add_filter("Step File", ["stp","step"])
	dlg.add_filter("STL File", ["stl"])
	dlg.set_save_file_name(drawing.draw_name + ".step");
	dlg.set_path(path_util.get_path_without_filename(drawing.save_path));
	return dlg
	
func save_drawing() -> bool:
	var extension := path_util.get_extension(filename).to_lower()
	if (extension == ".stl"):
		drawing.shape.get_cad_triangulation(0.1).save_tri_to_file(filename)
	elif (extension ==".stp" or extension == ".step"):	
		drawing.shape.save_cad_to_file(filename)
		drawing.dirty = false
		drawing.name = path_util.get_file_name_without_extension(filename);
	else:
		message = "Unkown extension"
		return false
	return true

func get_converter() -> Array[converter]:
	return [triangulate_cad_converter.new()]
