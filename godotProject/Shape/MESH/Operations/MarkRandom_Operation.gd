class_name MarkRandom_Operation extends TopLevelOperation

func getName():
	return "Mark Triangles Random";
	
func doShow(d : Drawing):
	if (d is DrawingMESH):
		return true;
	return false;
	
func modify_mesh_faces(mi: MeshInstance3D):
	var mdt = MeshDataTool.new()
	var original_mesh = mi.mesh
	
	mdt.create_from_surface(original_mesh, 0)
	for i in range(mdt.get_face_count()):
		var color = Color(randf(), randf(), randf()) # Random color per face
		for v in range(3):
			var vertex_idx = mdt.get_face_vertex(i, v)
			mdt.set_vertex_color(vertex_idx, color)
	
	var new_mesh = ArrayMesh.new()
	mdt.commit_to_surface(new_mesh)
	mi.mesh = new_mesh
	
	var mat = StandardMaterial3D.new()
	mat.vertex_color_use_as_albedo = true
	mi.set_surface_override_material(0, mat)
	
func execute(scene : DrawingScene):
	var m = scene.vis as MeshScene
	modify_mesh_faces(m.tri_vis.mesh)
