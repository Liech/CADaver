class_name MeshReport_Operation extends TopLevelOperation

func getName():
	return "Mesh Report";
	
func doShow(d : Drawing):
	if (d is DrawingMESH):
		return true;
	return false;
	
func execute(scene : DrawingScene):
	var shape = (scene.drawing as DrawingMESH).shape
	OKPopup.make("Report\n" + shape.get_mesh_report());
