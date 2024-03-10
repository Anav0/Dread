#pragma once

#include "Base.h"
#include "Constants.h"

class Shader;
class BufferLayout;

// order matters
struct Particle {
	m4 model;
	v4 color;
	v2 pos;
	v2 direction;
	f32 velocity;
	f32 ttl_s;
};

class ParticlesEmitter {
	u32 VAO, VBO, EBO, instanced_VBO;
	u64 n     = 0;
	u32 ttl_s = 0;
	std::vector<Particle> particles;

	public:
		ParticlesEmitter(v2 pos, v2 size, u64 n, f32 ttl_s) {
			this->ttl_s = ttl_s;
      this->n = n;
			this->particles.reserve(n);
			for(u64 i = 0; i < n; i++) {
				auto model = GetTransformMatrix(pos, size);
				particles.push_back({ model, YELLOW, pos, {0, 1}, 0.02f, ttl_s });
				pos.x += 5;
			}
		}

		void Update(f32 dt);
		void Allocate(BufferLayout);
		void Flush();
		void Draw(Shader& shader, const m4& projection);
};
