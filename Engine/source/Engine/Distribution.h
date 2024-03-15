#pragma once

#include "Base.h"
#include <vector>
#include <random>

class Distribution {
	public:
		virtual std::vector<f32> Generate() = 0;
};

class RandomDist : public Distribution {
	u64 n;
	u64 from; u64 to;
  std::random_device rd;  // a seed source for the random number engine

	public:
		RandomDist(u64 n, u64 from, u64 to) : n(n), from(from), to(to) {};

		virtual std::vector<f32> Generate();

		void SetParams(u64 n, f32 from, f32 to) {
			this->n = n;
			this->from = from;
			this->to = to;
		}

		void SetParams(f32 from, f32 to) {
			this->from = from;
			this->to = to;
		}
};
