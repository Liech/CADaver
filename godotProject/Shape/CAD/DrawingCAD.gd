class_name DrawingCAD extends Drawing

var shape     : CADShape = null;

func load_from_file()->bool: # return success
	shape = CADShape.load_cad_from_file(save_path)
	if (!shape):
		return false;
	dirty = false;
	return true

func get_converter() -> Array[converter]:
	return [mesh_converter.new(), vox_converter.new()]
