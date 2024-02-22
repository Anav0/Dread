#ifndef SHADER_H
#define SHADER_H

#pragma once

#include "Constants.h"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std::filesystem;

class Shader {
public:
    u32 ID;
    file_time_type vertex_last_written;
    file_time_type fragment_last_written;
    file_time_type geometry_last_written;

    std::string vertexPath;
    std::string fragmentPath;

    Shader() { }

    bool Load(u32 program_id, std::string path, u32 shader_flag, const char* shader_code, u32& id)
    {
        std::string shaderRawText;
        std::ifstream shaderFile;

        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            shaderFile.open(path);
            std::stringstream stream;
            stream << shaderFile.rdbuf();
            shaderFile.close();
            shaderRawText = stream.str();

        } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* c_shaderRawText = shaderRawText.c_str();

        u32 shader_id = glCreateShader(shader_flag);
        glShaderSource(shader_id, 1, &c_shaderRawText, NULL);
        glCompileShader(shader_id);
        bool success = checkCompileErrors(shader_id, shader_code);

        if (success)
            glAttachShader(program_id, shader_id);

        id = shader_id;

        return success;
    }

    Shader(const char* vertexPath, const char* fragmentPath)
    {
        this->vertexPath   = vertexPath;
        this->fragmentPath = fragmentPath;

        ID = glCreateProgram();

        u32 vertex, fragment;
        bool v_success = Load(ID, vertexPath, GL_VERTEX_SHADER, "VERTEX", vertex);
        bool f_success = Load(ID, fragmentPath, GL_FRAGMENT_SHADER, "FRAGMENT", fragment);

        if (!v_success) {
            printf("ERROR: %s shader failed to compile!\n", vertexPath);
        }
        if (!f_success) {
            printf("ERROR: %s shader failed to compile!\n", fragmentPath);
        }

        vertex_last_written = last_write_time(vertexPath);
        fragment_last_written = last_write_time(fragmentPath);

        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);

		setInt("imageSampler", 0);
    }

#pragma region Setters

// activate the shader
    // ------------------------------------------------------------------------
    void Use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, f32 value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const v2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, f32 x, f32 y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const v3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, f32 x, f32 y, f32 z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const v4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, f32 x, f32 y, f32 z, f32 w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const m4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


#pragma endregion

private:
    bool checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        return success;
    }
};
#endif
