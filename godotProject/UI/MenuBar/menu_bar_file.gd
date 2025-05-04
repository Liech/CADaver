extends PopupMenu

@export var bar : ApplicationMenuBar;

var index_new  = 0;
var index_load = 1;
var index_save = 2;
var index_quit = 3;

var continue_closing := false


func _ready() -> void:
	Hub.file.drawings_changed.connect(on_drawings_changed);
	on_drawings_changed()

func on_drawings_changed() -> void:
	set_item_disabled(index_save,Hub.file.drawings.size() == 0)

func _on_index_pressed(index: int) -> void:
	match index:
		index_new:
			new_pressed();
		index_load:
			load_pressed();
		index_save:
			save_pressed();
		index_quit:
			quit_pressed()
			
func new_pressed():
	new_drawing()
	
func load_pressed():
	on_load_drawing()
	
func save_pressed():
	invokeSaveFileDialog(bar.window.scene.drawing)

func new_drawing() -> void:
	var newOne := Drawing.new()
	newOne.draw_name = "new"
	bar.window.scene.drawing = newOne	
	Hub.file.drawings.append(newOne);
	Hub.file.drawings_changed.emit()
	
func on_load_drawing() -> void:
	var dlg = LoadFileDialog.new()
	dlg.add_filter("Step File", ["stp","step"])
	dlg.execute()
	
	if (!dlg.is_canceled()):
		var newOne := Drawing.new()
		newOne.draw_name =  path_util.get_file_name_without_extension(dlg.get_result_path());
		newOne.save_path = dlg.get_result_path();
		var success : bool = newOne.load_from_step_file();
		if (!success):
			OKPopup.make("Loading failed");
			return;
		Hub.file.drawings.append(newOne);	
		bar.window.scene.drawing = newOne	
		Hub.file.drawings_changed.emit()
		
func invokeSaveFileDialog(drawing : Drawing):
	var dlg = SaveFileDialog.new()
	dlg.add_filter("Step File", ["stp","step"])
	dlg.set_save_file_name(drawing.draw_name + ".step");
	dlg.set_path(path_util.get_path_without_filename(drawing.save_path));
	dlg.execute();
	
	if (!dlg.is_canceled()):
		drawing.shape.save_cad_to_file(dlg.get_result_path())
		drawing.dirty = false
		drawing.name = path_util.get_file_name_without_extension(dlg.get_result_path());
		Hub.file.dirty_changed.emit()

func quit_pressed():
	continue_closing = true
	
	while(continue_closing and Hub.file.drawings.size() > 0):
		await close_drawing(0)
		
	if (continue_closing): # somebody canceled?
		Hub.root_node.get_tree().quit()


func _on_menu_bar_filemenuquit() -> void:
	quit_pressed()

func close_drawing(index) -> void:
	if (Hub.file.drawings.size() <= index):
		continue_closing = false
		return
	var drawing := Hub.file.drawings[index]

	if (drawing.dirty):
		await save_confirmation(drawing);
	else:
		Hub.file.drawings.erase(drawing)
		Hub.file.drawings_changed.emit()

func save_confirmation(drawing : Drawing) -> void:
	var dlg := UnsavedChangesDialog.make("Drawing: " + drawing.draw_name)
	await dlg.dialog_finished
	if (dlg.yes_pressed):
		invokeSaveFileDialog(drawing)
	if (!dlg.cancel_pressed):	
		close_drawing(Hub.file.drawings.find(drawing))
	if (dlg.cancel_pressed):
		continue_closing = false
	dlg.queue_free()
	
	
