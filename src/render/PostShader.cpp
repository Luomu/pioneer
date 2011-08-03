#include "PostShader.h"

namespace Render {
namespace Post {

Shader::Shader(const std::string &vname, const std::string &fname) :
	m_program(0),
	m_vertName(vname),
	m_fragName(fname)
{
	Compile();
}

Shader::~Shader()
{
	glDeleteProgram(m_program);
}

void Shader::Bind()
{
	glUseProgram(m_program);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::Recompile()
{
	glDeleteProgram(m_program);
	InvalidateLocations();
	Compile();
}

void Shader::InvalidateLocations()
{

}

void Shader::Compile()
{

}

}
}