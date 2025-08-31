class_name DrawingVOX extends Drawing

var shape     : VoxelShape = null;

func load_from_file()->bool: # return success
	shape = VoxelShape.load_vox_from_file(save_path)
	if (!shape):
		return false;
	dirty = false;
	return true
