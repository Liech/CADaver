class_name MarkRegion_Operation extends TopLevelOperation

@export var threshold : float = 0.90;

func getName():
	return "Mark Region";
	
func doShow(d : Drawing):
	if (d is DrawingMESH):
		return true;
	return false;
	
func deindex_mesh(original_mesh: Mesh) -> ArrayMesh:
	var st = SurfaceTool.new()
	st.create_from(original_mesh, 0)
	st.deindex()
	st.generate_normals()
	return st.commit()
	
func color_mesh(mi: MeshInstance3D, patches: Array):
	var mdt = MeshDataTool.new()
	var original_mesh = mi.mesh
	
	# Important: MeshDataTool works on a per-surface basis
	mdt.create_from_surface(original_mesh, 0)
	
	for patch_idx in range(patches.size()):
		var patch = patches[patch_idx]
		# Generate a unique color for this specific patch
		var patch_color = Color.from_hsv(float(patch_idx) / patches.size(), 0.8, 0.9)
		
		for face_idx in patch:
			# Safety check: Ensure the index is within bounds
			if face_idx < mdt.get_face_count():
				for v in range(3):
					var vertex_idx = mdt.get_face_vertex(face_idx, v)
					mdt.set_vertex_color(vertex_idx, patch_color)
	
	var new_mesh = ArrayMesh.new()
	# Note: commit_to_surface will automatically try to handle vertex sharing,
	# but if colors differ at the same vertex, it might create "seams" (which is what we want!)
	mdt.commit_to_surface(new_mesh)
	mi.mesh = new_mesh
	
	var mat = StandardMaterial3D.new()
	mat.vertex_color_use_as_albedo = true
	# Optional: Use shading to see the geometry better
	mat.shading_mode = StandardMaterial3D.SHADING_MODE_PER_PIXEL 
	mi.set_surface_override_material(0, mat)
	
func execute(scene : DrawingScene):
	var success :export_dialog.result_state= await export_dialog.make(self)
	if (success == export_dialog.result_state.Success):
		var m = scene.vis as MeshScene
		var shape = (scene.drawing as DrawingMESH).shape
		var patches = shape.normal_cluster(func(curr, cand):
			return curr.dot(cand) > threshold
		)
		m.tri_vis.mesh.mesh = deindex_mesh(m.tri_vis.mesh.mesh)
		color_mesh(m.tri_vis.mesh, patches)
