class_name CADSceneFactory extends Node

var compound := preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Compound/CADCompound.tscn")
var compsolid := preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/CompSolid/CompSolid.tscn")
var solid := preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Solid/CADSolid.tscn")
var edge := preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Edge/Edge.tscn")
var face := preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Face/Face.tscn")
var shell := preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Shell/Shell.tscn")
var vertex := preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Vertex/Vertex.tscn")
var wire := preload("res://3D/VisualizationObject/CADObject/TopoHierarchy/Wire/Wire.tscn")

func make(cad : CADShape) -> Node3D:
	if (cad is CADSolid):
		var result := solid.instantiate() as solid_scene;
		result.solid = cad;
		return result;
	elif (cad is CADCompSolid):
		var result := compsolid.instantiate() as compsolid_scene;
		result.comp_solid = cad;
		return result;
	elif (cad is CADCompound):
		var result := compound.instantiate() as compound_scene;
		result.compound = cad;
		return result;
	elif (cad is CADEdge):
		var result := edge.instantiate() as edge_scene;
		result.edge = cad;
		return result;
	elif (cad is CADShell):
		var result := shell.instantiate() as shell_scene;
		result.shell = cad;
		return result;
	elif (cad is CADVertex):
		var result := vertex.instantiate() as vertex_scene;
		result.vertex = cad;
		return result;
	elif (cad is CADWire):
		var result := wire.instantiate() as wire_scene;
		result.wire = cad;
		return result;
	elif (cad is CADFace):
		var result := face.instantiate() as face_scene;
		result.face = cad;
		return result;
	else:
		return null
