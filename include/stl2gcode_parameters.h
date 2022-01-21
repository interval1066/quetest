#ifndef PARAMETERS_H_
#define PARAMETERS_H_

// Will be replaced by a json file or something a a little
// less static
struct STL2gcodeParams
{
	float layer_height = 0.2f;
	float nozzle_diameter = 0.25f;
	float thread_thickness = 1.75f;

	float top_bottom_thickness = 0.4f;
	float filling_density = 0.15;
	int nozzle_temperature = 205;
	int table_temperature = 55;

	int printing_speed = 35;
	int filling_speed = 40;
	int moving_speed = 90;

	int printer_width = 215;
	int printer_depth = 215;
	int printer_height = 300;
};

#endif
