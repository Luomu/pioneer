#include "RenderPostProgram.h"
#include "SmartPtr.h"
#include "Render.h"
#include "StringF.h"
#include <fstream>

namespace Render {
namespace Post {

Program::Program() :
	m_program(0)
{
}

Program::Program(const std::string &vtxName, const std::string &fragName) :
	m_program(0)
{
	Load(vtxName, fragName);
}

Program::~Program()
{
	glDeleteProgram(m_program);
}

void Program::Load(const std::string &vtxName, const std::string &fragName)
{
	if (0 != m_program)
		glDeleteProgram(m_program);

	m_program = glCreateProgram();

	Shader vs(vtxName,  GL_VERTEX_SHADER);
	Shader fs(fragName, GL_FRAGMENT_SHADER);

	glAttachShader(m_program, vs.GetShader());
	glAttachShader(m_program, fs.GetShader());

	glLinkProgram(m_program);
}

void Program::Use()
{
	glUseProgram(m_program);
}

void Program::Unuse()
{
	glUseProgram(0);
}

GLuint Program::GetLocation(const char *name)
{
	return glGetUniformLocation(m_program, name);
}

void Program::SetUniform1i(const char *name, int value)
{
	glUniform1i(GetLocation(name), value);
}

void Program::SetUniform1f(const char *name, float value)
{
	glUniform1f(GetLocation(name), value);
}

void Program::SetUniform2f(const char *name, float val1, float val2)
{
	glUniform2f(GetLocation(name), val1, val2);
}

Shader::Shader(const std::string &filename, GLenum shaderType) :
	m_shader(0)
{
	m_shader = glCreateShader(shaderType);
	SetSource(LoadSource(std::string()+PIONEER_DATA_DIR+"/shaders/"+filename));
	glCompileShader(m_shader);
	GLint status = GL_FALSE;
	glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);
	if (GL_TRUE != status) {
		GLsizei infologLength = 0;
		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1) {
			ScopedArray<char> infoLog(new char[infologLength]);
			glGetShaderInfoLog(m_shader, infologLength, &infologLength, infoLog.Get());
			// Getting this error:success issue on Radeon HD 4200 & FGLRX
			if (strcmp(infoLog.Get(), "Vertex shader was successfully compiled to run on hardware.") != 0 &&
				strcmp(infoLog.Get(), "Vertex shader was successfully compiled to run on hardware.\n") != 0 &&
				strcmp(infoLog.Get(), "Fragment shader was successfully compiled to run on hardware.\n"))
			{
				throw Render::Exception(stringf("Error compiling %0: %1\n", filename, std::string(infoLog.Get())));
			}
		}
	}
}

Shader::~Shader()
{
	glDeleteShader(m_shader);
}

void Shader::SetSource(const std::string &sourceString)
{
	GLchar const *src  = sourceString.c_str();
	GLint const length = sourceString.size();
	glShaderSource(m_shader, 1, &src, &length);
}

std::string Shader::LoadSource(const std::string &fileName)
{
	std::ifstream infile(fileName.c_str());
	std::string source;

	while (infile.good()) {
		std::string line;
		std::getline(infile, line);
		source.append(line + "\n");
	}
	return source;
}

} } // namespace Render::Post
