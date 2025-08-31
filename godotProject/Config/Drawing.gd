class_name Drawing extends Node

var draw_name : String = "new";
var save_path : String = "";
var dirty     : bool = false; # Save needed?

func load_from_file()->bool: # return success
	return false;
