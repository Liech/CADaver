class_name export_uielement_factory extends Node



static func make(v : Dictionary, target : Node)->Control:
	match v.type:
		Variant.Type.TYPE_VECTOR3I:
			return vector3i_export_uielement.make(v, target);
	return null
