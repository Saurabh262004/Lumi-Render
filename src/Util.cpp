#include <fstream>
#include <sstream>
#include <string>

#include <lumi_render/Util.hpp>

std::string Util::readFile(const std::string& path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open " + path);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}

int Util::randI(int min, int max) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> distrib(min, max);

	return distrib(gen);
}

std::vector<int> Util::randIV(int min, int max, int count) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> distrib(min, max);

	std::vector<int> nums;

	for (int i = 0; i < count; i++) {
		nums.push_back(distrib(gen));
	}

	return nums;
}

float Util::randF(float min, float max) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<float> distrib(min, max);

	return distrib(gen);
}

std::vector<float> Util::randFV(float min,  float max, int count) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<float> distrib(min, max);

	std::vector<float> nums;

	for (int i = 0; i < count; i++) {
		nums.push_back(distrib(gen));
	}

	return nums;
}
