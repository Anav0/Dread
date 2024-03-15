#include "Particles.h"
#include "Buffers.h"

void ParticlesEmitter::Init(std::vector<Particle> particles) {
			this->particles = particles;
			this->n         = particles.size();
}

void ParticlesEmitter::Allocate(BufferLayout layout) {
	printf("N: %llu\n\n", n);
	constexpr f32 vertices[] = {
		0.5f, 0.5f, 0.0f, // top right
		0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f // top left
	};

	constexpr u32 indices[] = {
		// note that we start from 0!
		0, 1, 3, // first triangle
		1, 2, 3 // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &instanced_VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
	glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);
	glBufferData(GL_ARRAY_BUFFER, n * sizeof(Particle), NULL, GL_DYNAMIC_DRAW);

	u8 position = 3;
	for (BufferElement& el : layout.elements) {
		GLenum gl_type = BufferElementTypeToOpenGLType(el.type);
		printf("glEnableVertexAttribArray(%i)\n", position);
		printf("glVertexAttribPointer(%i, %i, %i, GL_FALSE, %u, (void*)%u);\n", position, el.length, gl_type, layout.size, el.offset);
		printf("glVertexAttribDivisor(%i, 1)\n", position);
		printf("\n");
			
		glEnableVertexAttribArray(position);

		if (el.IsInt()) {
			glVertexAttribIPointer(position, el.length, gl_type, layout.size, (void*)el.offset);
		} else {
			glVertexAttribPointer(position, el.length, gl_type, GL_FALSE, layout.size, (void*)el.offset);
		}
		
		glVertexAttribDivisor(position, 1);
		position++;
	}

	glBindVertexArray(0);
}

void ParticlesEmitter::Flush() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);

	u64 size = n * sizeof(Particle);
	
	assert(size > 0);

	glBufferSubData(GL_ARRAY_BUFFER, 0, size, &particles[0]);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void ParticlesEmitter::Draw(Shader& shader, const m4& projection) { 
	shader.Use();
	shader.setMat4("projection", projection);

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, n);
	glBindVertexArray(0);
}

void ParticlesEmitter::Update(f32 dt) {
		this->update(this->particles, dt);
}
