class_name shape_io extends Node

var filename : String;
var drawing : Drawing;
var message : String

static func make_from_filename(inputFilename : String) -> shape_io:
	var ext := path_util.get_extension(inputFilename).to_lower();
	var result : shape_io;
	if (ext == ".stl"):
		result = mesh_io.new();
	elif (ext == ".vox"):
		result = vox_io.new()
	elif (ext ==".step" or ext == ".stp"):
		result = cad_io.new();
	else:
		return null
	result.filename = inputFilename;
	return result;

static func make_from_drawing(input : Drawing) ->shape_io:
	var result : shape_io;
	if (input is DrawingMESH):
		result = mesh_io.new();
	elif (input is DrawingVOX):
		result = vox_io.new()
	elif (input is DrawingCAD):
		result = cad_io.new();
	else:
		return null
	result.drawing = input;
	return result

static func make_load_file_dialog() -> LoadFileDialog:
	var dlg = LoadFileDialog.new()
	dlg.add_filter("All Supported Files", ["stp","step","stl","vox"])
	dlg.add_filter("Step File", ["stp","step"])
	dlg.add_filter("Triangle Mesh File", ["stl"])
	dlg.add_filter("Magicka Vox File", ["vox"])
	return dlg;

func make_save_file_dialog() -> SaveFileDialog:
	message = "Not implemented!"
	return null

func save_drawing() -> bool:
	message = "Not implemented!"
	return false

func load_drawing() -> Drawing:
	message = "Not implemented!"
	return null
