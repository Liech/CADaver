class_name HubClass extends Node

var file : file_config = file_config.new();
# view
signal current_drawing_changed; # void
signal show_tabs_changed;

var background_color : Color = Color.BLACK	
var show_tabs = true

var root_node : Node;
var extra_windows : Array[ExtraWindow]
