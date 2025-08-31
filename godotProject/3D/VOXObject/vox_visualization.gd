class_name VOXVisualization extends visualization_object

var voxel_triangulation = preload("res://3D/VOXObject/VOXTriangulation.tscn")

var drawing : DrawingVOX = null;
var vox_vis : vox_triangulation_scene = null

func set_drawing(d : Drawing)->void:
	if (drawing != d):
		drawing = d;
		if (drawing.shape):
			build_children()

func reset_drawing()-> void:
	drawing = null
	if(vox_vis):
		remove_child(vox_vis)
		vox_vis.queue_free()

func build_children()->void:
	vox_vis = voxel_triangulation.instantiate() as vox_triangulation_scene
	vox_vis.volume = drawing.shape
	add_child(vox_vis)
	bounding_box = drawing.shape.get_vox_aabb()
		
