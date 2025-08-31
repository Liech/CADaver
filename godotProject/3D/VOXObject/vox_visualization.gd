class_name VOXVisualization extends visualization_object

var drawing : DrawingVOX = null;

func set_drawing(d : Drawing)->void:
	if (drawing != d):
		drawing = d;
		if (drawing.shape):
			build_children()

func reset_drawing()-> void:
	drawing = null

func build_children()->void:
	print(drawing.shape.get_vox_aabb())
	bounding_box = drawing.shape.get_vox_aabb()
		
