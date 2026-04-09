class_name MarkRegions_Operation extends TopLevelOperation

func getName():
	return "Mark Regions";
	
func doShow(d : Drawing):
	if (d is DrawingMESH):
		return true;
	return false;
	
func execute():
	OKPopup.make("Not implemented yet!");
	pass
