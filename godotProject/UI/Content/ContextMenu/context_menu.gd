class_name main_context_menu extends PopupMenu

@export var selected : Array[Selectable];
@export var child_pool : Node;

var index_extrude : int = 0


func _on_index_pressed(index: int) -> void:
		match index:
			index_extrude:
				for x in selected:
					var result := CADOperation.extrude(x.get_shape(),Vector3(0,5,0))
					if (result):
						var scene := preload("res://3D/Util/PreviewMesh/PreviewMesh.tscn").instantiate()
						scene.set_shape(result)
						child_pool.add_child(scene);
