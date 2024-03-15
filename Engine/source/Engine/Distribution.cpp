#pragma once

#include "Distribution.h"

std::vector<f32> RandomDist::Generate() {
	std::vector<f32> numbers;
	numbers.reserve(n);

  std::mt19937 gen(rd()); 
	std::uniform_int_distribution<> distrib(from, to);

	for(u64 i = 0; i < n; i++) {
		f32 x = static_cast<f32>(distrib(gen));
		numbers.push_back(x);
	}

	return numbers;
};
