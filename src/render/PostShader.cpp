#include "PostShader.h"
#include "Renderer.h"
#include "libs.h"
#include <vector>

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
	while (!m_uniforms.empty()) delete m_uniforms.back(), m_uniforms.pop_back();
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
	std::vector<Uniform*>::const_iterator it = m_uniforms.begin();
	for(; it != m_uniforms.end(); ++it) {
		(*it)->InvalidateLocation();
	}
	Compile();
}

void Shader::InvalidateLocations()
{

}

class ShaderSource {
public:
	ShaderSource(const std::string &filename, GLenum type) :
		m_shader(0),
		m_code(0)
	{
		m_code = load_shader_source(filename.c_str());
		if (!m_code) throw RenderException("Could not load shader source");

		std::vector<const char*> shader_src;

		m_shader = glCreateShader(type);
		shader_src.push_back(m_code);
		glShaderSource(m_shader, shader_src.size(), &shader_src[0], 0);
		glCompileShader(m_shader);
		GLint status;
		glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);
		if (!status) throw ShaderException("Couldn't compile shader", m_shader);
	}

	~ShaderSource() {
		glDeleteShader(m_shader);
		free(m_code);
	}

	GLuint GetCompiledShader() const { return m_shader; }

private:
	static __attribute((malloc)) char *load_shader_source(const char *filename) {
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

	char *m_code;
	GLuint m_shader;
};

void Shader::Compile()
{
	const std::string path = PIONEER_DATA_DIR"/shaders/";

	try {
		ShaderSource vs(path + m_vertName, GL_VERTEX_SHADER);
		ShaderSource fs(path + m_fragName, GL_FRAGMENT_SHADER);

		m_program = glCreateProgram();
		glAttachShader(m_program, vs.GetCompiledShader());
		glAttachShader(m_program, fs.GetCompiledShader());
		glLinkProgram(m_program);
		GLint status;
		glGetProgramiv(m_program, GL_LINK_STATUS, &status);
		if (!status) throw ShaderException("Couldn't link shader", m_program);

	} catch(ShaderException &ex) {
		//print the compile error, clean up, rethrow
		int infologLength = 0;
		char infoLog[1024];
		if (glIsShader(ex.Obj()))
			glGetShaderInfoLog(ex.Obj(), 1024, &infologLength, infoLog);
		else
			glGetProgramInfoLog(ex.Obj(), 1024, &infologLength, infoLog);

		if (infologLength > 0)
			fprintf(stderr, "%s: %s\n", ex.what(), infoLog);

		glDeleteProgram(m_program);
		m_program = 0;

		throw;
	}
}

Uniform* Shader::AddUniform(const std::string &name)
{
	Uniform *uf = new Uniform(name);
	uf->m_parent = this;
	m_uniforms.push_back(uf);
	return uf;
}

} //namespace Post

Uniform::Uniform(const std::string &name) :
	m_name(name),
	m_location(0),
	m_parent(0)
{

}

void Uniform::InvalidateLocation()
{
	m_location = 0;
}

GLuint Uniform::GetLocation()
{
	m_location = glGetUniformLocation(m_parent->GetProgram(), m_name.c_str());
	return m_location;
}

void Uniform::Set(float f)
{
	glUniform1f(GetLocation(), f);
}

void Uniform::Set(int i)
{
	glUniform1i(GetLocation(), i);
}

} //namespace Render
