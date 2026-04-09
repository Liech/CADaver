class_name OperationMenu extends PopupMenu

@export var bar : ApplicationMenuBar

# We store instances here so they persist until the menu is closed/used
var _active_instances: Array[TopLevelOperation] = []

func _ready() -> void:
	about_to_popup.connect(_on_about_to_popup)
	index_pressed.connect(_on_index_pressed)

func _on_about_to_popup() -> void:
	clear()
	_active_instances.clear()
	
	if not bar or not bar.window or not bar.window.scene:
		return
	var drawing = bar.window.scene.drawing

	# 1. Get the list of all global classes registered with 'class_name'
	var global_classes = ProjectSettings.get_global_class_list()
	
	for class_data in global_classes:
		# 2. Check if this class inherits from our base class
		# class_data contains: { "base": "Node", "class": "MyOp", "language": "GDScript", "path": "..." }
		if _is_subclass_of(class_data["class"], "TopLevelOperation"):
			
			# 3. Instantiate the class to check its logic
			var script = load(class_data["path"])
			var instance = script.new()
			
			if instance is TopLevelOperation:
				if instance.doShow(drawing):
					_active_instances.append(instance)
					add_item(instance.getName())
				else:
					# Clean up instances that we don't need to show
					instance.queue_free()

func _on_index_pressed(index: int) -> void:
	var scene = bar.window.scene
	if index < _active_instances.size():
		var op = _active_instances[index]
		await op.execute(scene)
		
	# Clean up instances after execution
	for inst in _active_instances:
		inst.queue_free()
	_active_instances.clear()

# Helper function to check inheritance via the global class list
func _is_subclass_of(class_name_to_check: String, target_base: String) -> bool:
	if class_name_to_check == target_base:
		return true
		
	var global_classes = ProjectSettings.get_global_class_list()
	for c in global_classes:
		if c["class"] == class_name_to_check:
			if c["base"] == target_base:
				return true
			elif c["base"] != "":
				# Recursively check the hierarchy
				return _is_subclass_of(c["base"], target_base)
	return false
