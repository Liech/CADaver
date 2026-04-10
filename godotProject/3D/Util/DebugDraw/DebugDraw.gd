class_name DebugDraw extends MeshInstance3D

# Define a simple data container
class LineData extends Resource:
	@export var points: PackedVector3Array
	@export var color: Color = Color.WHITE

# Now the Inspector will show a nice list of "LineData" objects
@export var lines: Array[LineData]

var imm_mesh: ImmediateMesh

func _ready():
	imm_mesh = ImmediateMesh.new()
	self.mesh = imm_mesh
	
	var mat = StandardMaterial3D.new()
	mat.no_depth_test = true 
	mat.render_priority = 10
	mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mat.vertex_color_use_as_albedo = true
	self.material_override = mat

static func make(parent : Node): #static
	var scene = preload("res://3D/Util/DebugDraw/DebugDraw.tscn")
	var inst = scene.instantiate() as DebugDraw
	parent.add_child(inst)
	return inst

func _process(_delta):
	imm_mesh.clear_surfaces()
	
	for line in lines:
		if line.points.size() < 2: continue
		
		imm_mesh.surface_begin(Mesh.PRIMITIVE_LINE_STRIP) # Use STRIP for connected points
		imm_mesh.surface_set_color(line.color)
		
		for pt in line.points:
			imm_mesh.surface_add_vertex(pt)
			
		imm_mesh.surface_end()
