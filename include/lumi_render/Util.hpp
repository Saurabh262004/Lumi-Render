#ifndef UTIL_HPP
#define UTIL_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <vector>

struct Util {
public:
	static std::string readFile(const std::string& path);

	static int randI(int min, int max);
	static std::vector<int> randIV(int min, int max, int count);

	static float randF(float min, float max);
	static std::vector<float> randFV(float min, float max, int count);
};

#endif