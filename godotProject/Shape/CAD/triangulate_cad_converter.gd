class_name triangulate_cad_converter extends converter

@export var Precision : float = 0.1;

func get_converter_name()->String:
	return "Triangulate"

func convert_drawing(input : Drawing) -> Drawing:
	var s = input as DrawingCAD
	var newShape = s.shape.get_cad_triangulation(Precision)
	var result = DrawingMESH.new()
	result.save_path = s.save_path
	result.name = s.name
	result.shape = newShape;
	return result

func execute_dialog(_input : Drawing) -> export_dialog.result_state:
	var success :export_dialog.result_state= await export_dialog.make(self)
	if (Precision < 0.00001):
		OKPopup.make("Conversion Failed\nPrecision to small");
		return export_dialog.result_state.Failed
	return success
