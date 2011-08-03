#include "PostShader.h"
#include "Renderer.h"

namespace Render {
namespace Post {

class ShaderException : public RenderException {
protected:
	GLuint m_obj;
public:
	ShaderException(const char *what, GLuint obj = 0) :
	  RenderException(what),
	  m_obj(obj) { }
	GLuint Obj() const { return m_obj; }
};

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
	if (!m_program) return;
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

static __attribute((malloc)) char *load_shader_source(const char *filename)
{
	FILE *f = fopen(filename, "rb");
	if (!f)
		return 0;
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buf = static_cast<char*>(malloc(sizeof(char) * (len+1)));
	fread(buf, 1, len, f);
	fclose(f);
	buf[len] = 0;
	return buf;
}

void Shader::Compile()
{
	GLuint vs, ps = 0;
	std::vector<const char*> shader_src;
	const std::string path = PIONEER_DATA_DIR"/shaders/";

	char *pscode = 0;
	char *vscode = 0;

	try {
		vscode = load_shader_source((path + m_vertName).c_str());
		pscode = load_shader_source((path + m_fragName).c_str());
		if (!vscode || !pscode) throw RenderException("Could not load shader source");

		vs = glCreateShader(GL_VERTEX_SHADER);
		shader_src.push_back(vscode);
		glShaderSource(vs, shader_src.size(), &shader_src[0], 0);
		glCompileShader(vs);
		GLint status;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
		if (!status) throw ShaderException("Couldn't compile vertex shader", vs);

		shader_src.clear();
		ps = glCreateShader(GL_FRAGMENT_SHADER);
		shader_src.push_back(pscode);
		glShaderSource(ps, shader_src.size(), &shader_src[0], 0);
		glCompileShader(ps);
		glGetShaderiv(ps, GL_COMPILE_STATUS, &status);
		if (!status) throw ShaderException("Couldn't compile fragment shader", ps);

		m_program = glCreateProgram();
		glAttachShader(m_program, vs);
		glAttachShader(m_program, ps);
		glLinkProgram(m_program);
		glGetProgramiv(m_program, GL_LINK_STATUS, &status);
		if (!status) throw ShaderException("Couldn't link shader", m_program);

	} catch(ShaderException &ex) {
		//print the compile error, clean up and rethrow
		int infologLength = 0;
		char infoLog[1024];
		if (glIsShader(ex.Obj()))
			glGetShaderInfoLog(ex.Obj(), 1024, &infologLength, infoLog);
		else
			glGetProgramInfoLog(ex.Obj(), 1024, &infologLength, infoLog);

		if (infologLength > 0)
			fprintf(stderr, "%s: %s\n", ex.what(), infoLog);

		glDeleteShader(vs);
		glDeleteShader(ps);
		glDeleteProgram(m_program);
		m_program = 0;
		free(pscode);
		free(vscode);

		throw;
	}

	free(pscode);
	free(vscode);
}

}
}