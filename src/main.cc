#include <random>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * Simple demo of distributed 3d print slicing queue system
 * using 0MQ.
 * Two data queues are set up (vectors of arrays) containing
 * glm vectors for transformation. Clients will be given
 * sections data off the the input queue, transform them, and
 * put the transformed data into the output queue. This is just
 * an initial proof of concept, so everything happens here, no
 * clients, yet. And no messaging, yet.
 *
 * Test data, below to be replaced by vectors from the input queue.
 */
template<typename Numeric, typename Generator = std::mt19937>
Numeric random(Numeric from, Numeric to)
{
    thread_local static Generator gen(std::random_device{}());

    using dist_type = typename std::conditional
    <
        std::is_integral<Numeric>::value
        , std::uniform_int_distribution<Numeric>
        , std::uniform_real_distribution<Numeric>
    >::type;

    thread_local static dist_type dist;

    return dist(gen, typename dist_type::param_type{from, to});
}

template<typename T>
void PrintArray(const T& e) {
    std::cout << std::setprecision(12) << e << ' ';
}

template<typename T, std::size_t N>
void PrintArray(const std::array<T,N>& A) {
    for (const auto& e : A) 
        PrintArray(e);
    std::cout << std::endl;
}

int
main(int argc, char** argv)
{
	std::array<double, 3> glmvec;
	std::vector<std::array<double, 3>> slicesin;
	std::vector<std::array<double, 3>> slicesout;

	for(auto i = 0U; i < 1000000U; ++i) {
		glmvec = { random<double>(1.000000, 10000.000000),
			random<double>(1.000000, 10000.000000),
			random<double>(1.000000, 10000.000000) };
		slicesin.push_back(glmvec);
	}

	slicesout.resize(slicesin.size());
	std::cout << "Contents:" << std::endl;

	// Idea here is to pull out the vectors(arrays) in groups based on the number of nodes available to share
	// the work, have each node transform them, and put the transformed vectors into the recieving queue. The
	// node maintaining the queues will be the master, and use 0MQ to talk to each slave node.
	for(std::vector<std::array<double, 3>>::const_iterator it = slicesin.begin(); it != slicesin.end(); ++it)
		PrintArray(*it);

	return EXIT_SUCCESS;
}
