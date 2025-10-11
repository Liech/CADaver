extends MeshInstance3D

func set_shape(shape : CADShape):
	mesh = shape.get_cad_triangulation(0.01).get_array_mesh()
