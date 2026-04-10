class_name HalfEdgeReport_Operation extends TopLevelOperation

func getName():
	return "HalfEdge Report";
	
func doShow(d : Drawing):
	if (d is DrawingMESH):
		return true;
	return false;
	
func execute(scene : DrawingScene):
	var shape = (scene.drawing as DrawingMESH).shape
	OKPopup.make("Report\n" + shape.get_halfedge_report());
