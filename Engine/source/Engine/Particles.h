#pragma once

#include "Base.h"
#include "Constants.h"
#include "Distribution.h"
#include <vector>

class Shader;
class BufferLayout;

// order matters
struct Particle {
	m4 model;
	v4 color;
	v2 pos;
	v2 size;
	v2 direction;
	f32 velocity;
	f32 ttl_s;

	void UpdateMatrix() {
		model = GetTransformMatrix(pos, size);
	}
};

typedef void (*pParticlesUpdate)(std::vector<Particle>&, f32 dt, v2 pos, v2 size);

struct ColorChange {
	v4 from;
	v4 to;
	f32 duration_ms;
};

class ParticlesEmitter {
	u32 VAO, VBO, EBO, instanced_VBO;
	u64 n     = 0;
	std::vector<Particle> particles;

	Distribution* placement_x;
	Distribution* placement_y;
	Distribution* direction;
	Distribution* velocity;
	Distribution* ttl;
	Distribution* rgb;

	v2 pos;
	v2 size;

	public:
		pParticlesUpdate update;

		ParticlesEmitter(
				Distribution* placement_x, 
				Distribution* placement_y, 
				Distribution* direction, 
				Distribution* velocity, 
				Distribution* ttl,
				Distribution* rgb) {

			this->direction   = direction;
			this->placement_x = placement_x;
			this->placement_y = placement_y;
			this->velocity    = velocity;
			this->ttl         = ttl;
			this->rgb         = rgb;
		};

		std::vector<Particle> CreateParticles(u64 n);
		void Init(u64 n, v2 pos, v2 size);
		void Update(f32 dt);
		void Allocate(BufferLayout);
		void Flush();
		void Draw(Shader& shader, const m4& projection);
};
