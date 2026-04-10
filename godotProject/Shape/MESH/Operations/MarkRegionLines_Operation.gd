class_name MarkRegionLines_Operation extends TopLevelOperation

@export var threshold : float = 0.90;
@export var allowHoles : bool = true;
@export var regionNumber : int = 0

func getName():
	return "Mark Region Lines";
	
func doShow(d : Drawing):
	if (d is DrawingMESH):
		return true;
	return false;
	
func execute(scene : DrawingScene):
	var success :export_dialog.result_state= await export_dialog.make(self)
	
	if (regionNumber < -1):
		OKPopup.make("Failed\n Either -1 for all or >0 for specific region");
		return;
		
	if (success == export_dialog.result_state.Success):
		var shape = (scene.drawing as DrawingMESH).shape
		var patches = shape.normal_cluster(func(curr, cand):
			return curr.dot(cand) > threshold
		, allowHoles)
		if (regionNumber >= len(patches)):
			OKPopup.make("Failed\nAmount Regions: " +str(len(patches)) + "\nAmount -1 for all (Performance warning)");
			return
			
		var drawRegionBorders = func(subRegionNumber):
			var lines = shape.cluster_border(patches[subRegionNumber])
			var dbg = DebugDraw.make(scene.vis)
			
			for line in lines:
				var l = DebugDraw.LineData.new();
				l.color = Color.RED
				for index in line:
					var p = shape.get_vertex(index)
					l.points.push_back(p)
				l.points.push_back(shape.get_vertex(line[0]))
				dbg.lines.push_back(l)
		if (regionNumber >= 0):
			drawRegionBorders.call(regionNumber)
		else:
			for i in range(len(patches)):
				drawRegionBorders.call(i)
		
		
