#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <stl2gcode.h>

using namespace std;
using std::filesystem::exists;

/**
 * Simple demo of distributed 3d print slicing queue system
 * using 0MQ.
 * Two data queues are set up (vectors of arrays) containing
 * glm vectors for transformation. Clients will be given
 * sections data off the the input queue, transform them, and
 * put the transformed data into the output queue. This is just
 * an initial proof of concept, so everything happens here, no
 * clients, yet. And no messaging, yet.
 */
int
main(int argc, char** argv)
{
	bool fExist = false;

	if(argc != 3) {
		cout << "Usage: <inputstl> <outputstl>" << endl;
		return EXIT_FAILURE;
	}

	if(exists(argv[1])) fExist = true;
	if(!fExist) {
		cout << "Can't find model file" << endl;
		return EXIT_FAILURE;
	}

	STL2gcodeParams parameters{};
	vector<string> models = { argv[1] };

	parameters.nozzle_diameter = 0.4f;
	parameters.thread_thickness = 2.85f;

	STL2gcode mesh(models[0], parameters);

	mesh.convert(argv[2]);
	// Idea here is to pull out the vectors(arrays) in groups based on the number of nodes available to share
	// the work, have each node transform them, and put the transformed vectors into the recieving queue. The
	// node maintaining the queues will be the master, and use 0MQ to talk to each slave node.
	//for(std::vector<std::array<float, 3>>::const_iterator it = slicesin.begin(); it != slicesin.end(); ++it)
	//	PrintArray(*it);

	return EXIT_SUCCESS;
}
