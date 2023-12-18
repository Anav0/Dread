#ifndef constexprants_H
#define constexprants_H

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

typedef int ID;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef float f32;
typedef double f64;

typedef glm::vec4 v4;
typedef glm::vec3 v3;
typedef glm::vec2 v2;

typedef glm::mat4 m4;
typedef glm::mat3 m3;
typedef glm::mat2 m2;

constexpr v4 YELLOW = v4(0.96f, 0.76f, 0.47f, 1.0f);
constexpr v4 RED = v4(0.58f, 0.03f, 0.01f, 1.0f);
constexpr v4 WHITE = v4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr v4 BLACK = v4(0.0f, 0.0f, 0.0f, 1.0f);
constexpr v4 GOLD = v4(0.92f, 0.71f, 0.23f, 1.0f);
constexpr v4 GREY = v4(0.5f, 0.5, 0.5, 1.0f);

#endif
