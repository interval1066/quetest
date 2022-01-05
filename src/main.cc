#include <random>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <slicer.h>

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
	std::array<float, 3> glmvec;
	std::vector<std::array<float, 3>> slicesin;
	std::vector<std::array<float, 3>> slicesout;

	// read in the stl file
	// then process
	/*for(auto i = 0; i < 1000000; ++i) {
		glmvec = { random<float>(1.000000, 10000.000000),
			random<float>(1.000000, 10000.000000),
			random<float>(1.000000, 10000.000000) };
		slicesin.push_back(glmvec);
	}*/

	slicesout.resize(slicesin.size());
	std::cout << "Contents:" << std::endl;

	// Idea here is to pull out the vectors(arrays) in groups based on the number of nodes available to share
	// the work, have each node transform them, and put the transformed vectors into the recieving queue. The
	// node maintaining the queues will be the master, and use 0MQ to talk to each slave node.
	//for(std::vector<std::array<float, 3>>::const_iterator it = slicesin.begin(); it != slicesin.end(); ++it)
	//	PrintArray(*it);

	return EXIT_SUCCESS;
}
