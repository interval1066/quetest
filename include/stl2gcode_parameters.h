#ifndef STL2GCODE_STL2GCODE_PARAMETERS_H
#define STL2GCODE_STL2GCODE_PARAMETERS_H

// Will be replaced by a json file or something a a little
// less static
struct stl2gcode_parameters
{
	float layer_height = 0.2f;
	float nozzle_diameter = 0.25f;
	float thread_thickness = 1.75f;

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