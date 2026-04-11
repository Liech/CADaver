class_name cluster_mesh_to_cad_converter extends converter

@export var threshold : float = 0.90;

func get_converter_name()->String:
	return "To CAD (cluster)"

func convert_drawing(input : Drawing) -> Drawing:
	var s = input as DrawingMESH
	var shape = (input as DrawingMESH).shape
	var newShape = s.shape.to_cad_cluster(func(curr, cand):
		return curr.dot(cand) > threshold
		)
	var result = DrawingCAD.new()
	result.save_path = s.save_path
	result.draw_name = s.draw_name
	result.shape = newShape;
	return result

func execute_dialog(_input : Drawing) -> export_dialog.result_state:
	OKPopup.make("Usage\nDry run on Operations/MarkRegions to find good thresholds");
	var success :export_dialog.result_state= await export_dialog.make(self)
	return success
