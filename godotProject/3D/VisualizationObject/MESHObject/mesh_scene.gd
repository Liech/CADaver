class_name MeshScene extends visualization_object

var triangulationScene = preload("res://3D/VisualizationObject/MESHObject/MESHTriangulation.tscn")

var drawing : DrawingMESH = null;
var tri_vis : mesh_triangulation = null

func set_drawing(d : Drawing)->void:
	if (drawing != d):
		drawing = d;
		if (drawing.shape):
			build_children()

func reset_drawing()-> void:
	drawing = null
	if(tri_vis):
		remove_child(tri_vis)
		tri_vis.queue_free()

func build_children()->void:
	tri_vis = triangulationScene.instantiate() as mesh_triangulation
	tri_vis.shape = drawing.shape
	add_child(tri_vis)
	bounding_box = drawing.shape.get_tri_aabb()
		
