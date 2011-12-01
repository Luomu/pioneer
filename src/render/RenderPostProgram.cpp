#include "RenderPostProgram.h"
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

void Program::SetUniform1i(const char *name, int value)
{
	GLuint loc = glGetUniformLocation(m_program, name);
	glUniform1i(loc, value);
}

Shader::Shader(const std::string &filename, GLenum shaderType) :
	m_shader(0)
{
	if (GL_VERTEX_SHADER != shaderType && GL_FRAGMENT_SHADER != shaderType)
		throw 50;

	m_shader = glCreateShader(shaderType);
	SetSource(LoadSource(std::string()+PIONEER_DATA_DIR+"/shaders/"+filename));
	glCompileShader(m_shader);
	GLint status = GL_FALSE;
	glGetProgramiv(m_shader, GL_COMPILE_STATUS, &status);
	if (GL_TRUE != status) {
		int infologLength = 0;
		char infoLog[1024];
		glGetShaderInfoLog(m_shader, 1024, &infologLength, infoLog);
		if (infologLength > 0) {
			printf("Error compiling %s: %s\n", filename.c_str(), infoLog);
			throw 50;
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
