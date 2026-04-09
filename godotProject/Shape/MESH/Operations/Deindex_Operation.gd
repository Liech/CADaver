class_name Deindex_Operation extends TopLevelOperation

func getName():
	return "De-Index";
	
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
	
func execute(scene : DrawingScene):
	var m = scene.vis as MeshScene
	m.tri_vis.mesh.mesh = deindex_mesh(m.tri_vis.mesh.mesh)
