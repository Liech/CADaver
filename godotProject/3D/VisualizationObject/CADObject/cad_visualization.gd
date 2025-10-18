class_name CADVisualization extends visualization_object

@onready var child_pool : Node = $ChildPool

var drawing : Drawing = null;
var factory : CADSceneFactory = CADSceneFactory.new();

func set_drawing(d : Drawing)->void:
	if (drawing != d):
		drawing = d;
		reset_children()
		if (drawing.shape):
			build_children()

func reset_drawing()-> void:
	drawing = null
	reset_children()

func reset_children()->void:
	for n in child_pool.get_children():
		child_pool.remove_child(n)
		n.queue_free()

func build(shape :CADShape, parent : Node):
	var current := factory.make(shape);
	if (!current):
		return;
	parent.add_child(current)
	var childs := shape.get_cad_children()	
	for child in childs:
		build(child,current)

func build_children()->void:
	build(drawing.shape,child_pool)
	bounding_box = drawing.shape.get_cad_aabb()
		
