class_name shape_io extends Node

var filename : String;

static func make_shape_io(filename) ->shape_io:
	var ext := path_util.get_extension(filename).to_lower();
	var result : shape_io;
	if (ext == ".stl"):
		result = mesh_io.new();
	elif (ext == ".vox"):
		result = vox_io.new()
	elif (ext ==".step" or ext == ".stp"):
		result = cad_io.new();
	else:
		return null
	result.filename = filename;
	return result;

static func make_load_file_dialog() -> LoadFileDialog:
	var dlg = LoadFileDialog.new()
	dlg.add_filter("All Supported Files", ["stp","step","stl","vox"])
	dlg.add_filter("Step File", ["stp","step"])
	dlg.add_filter("Triangle Mesh File", ["stl"])
	dlg.add_filter("Magicka Vox File", ["vox"])
	return dlg;

func load_drawing() -> Drawing:
	return null
