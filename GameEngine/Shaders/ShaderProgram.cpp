#include "ShaderProgram.h"
#include "../Debug_/Log.h"
#include "glm/gtc/type_ptr.hpp"
#include "../Engine/Configuration.h"
#include <string>
#include <fstream>

CShaderProgram::~CShaderProgram()
{
	Stop();

    for (auto& id : shaderObjectsList)
	{
        glDetachShader(programID, id);
		glDeleteShader(id);
	}

    if (programID != 0)
        glDeleteProgram(programID);
}
bool CShaderProgram::CreateProgram()
{
    programID = glCreateProgram();
    if (programID == 0)
	{
		Error("[Error] Error creating shader program.");
		return false;
	}
	return true;
}

bool CShaderProgram::AddShader(const std::string& filename, GLenum mode)
{
	name = filename;

    auto root_folder = EngineConf.shadersFilesLocation;
    auto full_path = root_folder + filename;
    std::string source = Utils::ReadFile(full_path);

	uint32 id;
	id = glCreateShader(mode);

	if (id == 0)
	{
		Error("[Error] Error creating shader type " + mode);
		return false;
	}

    shaderObjectsList.push_back(id);

	const char* csource = source.c_str();

	glShaderSource(id, 1, &csource, NULL);

	glCompileShader(id);

	GLint compiled = GL_FALSE;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		char err[1000];
		int length = 0;
		glGetShaderInfoLog(id, 1000, &length, err);
		Log("[Error] ERRORS in Shader! \nFile name:\t" + filename + "\nCompile status: \n\n" + err);
		return false;
	}
	if (id == GL_FALSE)
	{
	}
    glAttachShader(programID, id);

	return true;
}

bool CShaderProgram::FinalizeShader()
{
	BindAttributes();

    glLinkProgram(programID);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

    glGetProgramiv(programID, GL_LINK_STATUS, &Success);
	if (Success == 0)
	{
        glGetProgramInfoLog(programID, sizeof(ErrorLog), NULL, ErrorLog);
		Error("[Error] Error linking shader program: " + name + " : " + std::string(ErrorLog));
		return false;
	}

    glUseProgram(programID);

	GetAllUniformLocations();
	ConnectTextureUnits();

    glValidateProgram(programID);
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &Success);

	if (!Success)
	{
        glGetProgramInfoLog(programID, sizeof(ErrorLog), NULL, ErrorLog);
		Error("[Error] Invalid shader program : " + name + " : " + std::string(ErrorLog));
		return false;
	}

    for (auto& id : shaderObjectsList)
		glDeleteShader(id);

    shaderObjectsList.clear();

	if (glGetError() != GL_NO_ERROR)
	{
		Error("[Error] Invalid shader program. " + name );
		return false;
	}

	glUseProgram(0);

	return true;
}

void CShaderProgram::Start() const
{
    glUseProgram(programID);
}
void CShaderProgram::Stop() const
{
	glUseProgram(0);
}
int CShaderProgram::GetUniformLocation(const std::string& uniformName) const
{
    return glGetUniformLocation(programID, uniformName.c_str());
}
void CShaderProgram::BindAttribute(int attribute, const std::string& variableName) const
{
    glBindAttribLocation(programID, attribute, variableName.c_str());
}
void CShaderProgram::LoadValue(uint32 loacation, const glm::mat4& value) const
{
	glUniformMatrix4fv(loacation, 1, GL_FALSE, glm::value_ptr(value));
}
void CShaderProgram::LoadValue(uint32 loacation, const glm::mat3& value) const
{
	glUniformMatrix3fv(loacation, 1, GL_FALSE, glm::value_ptr(value));
}

void CShaderProgram::LoadValue(uint32 loacation, const float& value) const
{
	glUniform1f(loacation, value);
}

void CShaderProgram::LoadValue(uint32 loacation, const int& value) const
{
	glUniform1i(loacation, value);
}

void CShaderProgram::LoadValue(uint32 loacation, const glm::vec2& value) const
{
	glUniform2fv(loacation, 1, glm::value_ptr(value));
}

void CShaderProgram::LoadValue(uint32 loacation, const glm::vec3& value) const
{
	glUniform3fv(loacation, 1, glm::value_ptr(value));
}

void CShaderProgram::LoadValue(uint32 loacation, const glm::vec4& value) const
{
	glUniform4fv(loacation, 1, glm::value_ptr(value));
}
