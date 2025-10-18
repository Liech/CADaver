class_name DrawingMESH extends Drawing

var shape : TriangleShape = null;

func load_from_file()->bool: # return success
	shape = TriangleShape.load_tri_from_file(save_path)
	if (!shape):
		return false;
	dirty = false;
	return true
