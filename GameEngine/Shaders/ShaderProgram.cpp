#include "ShaderProgram.h"
#include "../Debug_/Log.h"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <fstream>

CShaderProgram::~CShaderProgram()
{
	Stop();

	for (auto& id : m_ShaderObjectsList)
	{
		glDetachShader(m_ProgramID, id);
		glDeleteShader(id);
	}

	if (m_ProgramID != 0)
		glDeleteProgram(m_ProgramID);
}
bool CShaderProgram::CreateProgram()
{
	m_ProgramID = glCreateProgram();
	if (m_ProgramID == 0)
	{
		Error("[Error] Error creating shader program.");
		return false;
	}
	return true;
}

bool CShaderProgram::AddShader(const std::string& filename, GLenum mode)
{
	name = filename;

	std::string source = Utils::ReadFile(filename);

	uint id;
	id = glCreateShader(mode);

	if (id == 0)
	{
		Error("[Error] Error creating shader type " + mode);
		return false;
	}

	m_ShaderObjectsList.push_back(id);

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
	glAttachShader(m_ProgramID, id);

	return true;
}

bool CShaderProgram::FinalizeShader()
{
	BindAttributes();

	glLinkProgram(m_ProgramID);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0)
	{
		glGetProgramInfoLog(m_ProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		Error("[Error] Error linking shader program: " + name + " : " + std::string(ErrorLog));
		return false;
	}

	glUseProgram(m_ProgramID);

	GetAllUniformLocations();
	ConnectTextureUnits();

	glValidateProgram(m_ProgramID);
	glGetProgramiv(m_ProgramID, GL_VALIDATE_STATUS, &Success);

	if (!Success)
	{
		glGetProgramInfoLog(m_ProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		Error("[Error] Invalid shader program : " + name + " : " + std::string(ErrorLog));
		return false;
	}

	for (auto& id : m_ShaderObjectsList)
		glDeleteShader(id);

	m_ShaderObjectsList.clear();

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
	glUseProgram(m_ProgramID);
}
void CShaderProgram::Stop() const
{
	glUseProgram(0);
}
int CShaderProgram::GetUniformLocation(const std::string& uniformName) const
{
	return glGetUniformLocation(m_ProgramID, uniformName.c_str());
}
void CShaderProgram::BindAttribute(int attribute, const std::string& variableName) const
{
	glBindAttribLocation(m_ProgramID, attribute, variableName.c_str());
}
void CShaderProgram::LoadValue(uint loacation, const glm::mat4& value) const
{
	glUniformMatrix4fv(loacation, 1, GL_FALSE, glm::value_ptr(value));
}
void CShaderProgram::LoadValue(uint loacation, const glm::mat3& value) const
{
	glUniformMatrix3fv(loacation, 1, GL_FALSE, glm::value_ptr(value));
}

void CShaderProgram::LoadValue(uint loacation, const float& value) const
{
	glUniform1f(loacation, value);
}

void CShaderProgram::LoadValue(uint loacation, const int& value) const
{
	glUniform1i(loacation, value);
}

void CShaderProgram::LoadValue(uint loacation, const glm::vec2& value) const
{
	glUniform2fv(loacation, 1, glm::value_ptr(value));
}

void CShaderProgram::LoadValue(uint loacation, const glm::vec3& value) const
{
	glUniform3fv(loacation, 1, glm::value_ptr(value));
}

void CShaderProgram::LoadValue(uint loacation, const glm::vec4& value) const
{
	glUniform4fv(loacation, 1, glm::value_ptr(value));
}
