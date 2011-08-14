#include "PostFilter.h"
#include "RenderTarget.h"
#include "PostShader.h"
#include "Texture.h"

namespace Render {
namespace Post {

Filter::Filter(FilterSource source, FilterTarget target) :
	m_source(source),
	m_target(target)
{

}

/*
 * Set shader, texture and colour beforehand
 */
void Filter::ScreenAlignedQuad()
{
	const int width = 100;
	const int height = 100;
	const float x = 0.f;
	const float y = 0.f;
	const float w = 100.f;
	const float h = 100.f;

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();

	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 1.f);
	 glVertex2f(x, y);
	glTexCoord2f(0.f, 0.f);
	 glVertex2f(x, y + h);
	glTexCoord2f(1.f, 0.f);
	 glVertex2f(x + w, y + h);
	glTexCoord2f(1.f, 1.f);
	 glVertex2f(x + w, y);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

ShaderFilter::ShaderFilter(FilterSource source, FilterTarget target,
	const std::string &vert, const std::string &frag) :
	Filter(source, target)
{
	m_shader = new Post::Shader(vert, frag);
}

ShaderFilter::ShaderFilter(FilterSource source, FilterTarget target,
	Post::Shader *shader) :
	Filter(source, target)
{
	m_shader = shader;
}

ShaderFilter::~ShaderFilter()
{
	delete m_shader;
}

void ShaderFilter::Execute()
{
	if (m_target)
		m_target->BeginRTT();
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_shader->Bind();
	SetUniforms();
	ScreenAlignedQuad();
	m_shader->Unbind();
	if (m_target)
		m_target->EndRTT();
}

//Set uniforms for already bound shader
void ShaderFilter::SetUniforms()
{
	GLuint loc = glGetUniformLocation(m_shader->GetProgram(), "texture0");
	m_source->Bind();
	glUniform1i(loc, 0);
}

void ShaderFilter::CleanUp()
{
	m_source->Unbind();
}

void ShaderFilter::Reload()
{
	m_shader->Recompile();
}

Present::Present(FilterSource source) :
	Filter(source, 0)
{

}

void Present::Execute()
{
	const int width = 100;
	const int height = 100;
	const float x = 0.f;
	const float y = 0.f;
	const float w = 100.f;
	const float h = 100.f;

	glColor4f(1.f, 1.f, 1.f, 1.f);

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	m_source->Bind();

	glLoadIdentity();

	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 1.f);
	 glVertex2f(x, y);
	glTexCoord2f(0.f, 0.f);
	 glVertex2f(x, y + h);
	glTexCoord2f(1.f, 0.f);
	 glVertex2f(x + w, y + h);
	glTexCoord2f(1.f, 1.f);
	 glVertex2f(x + w, y);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

Tint::Tint(FilterSource source, FilterTarget target) :
	Filter(source, target)
{

}

void Tint::Execute()
{
	m_target->BeginRTT();

	glColor4f(1.f, 1.f, 0.f, 1.f); //the surprise is yellow
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	m_source->Bind();

	ScreenAlignedQuad();

	m_target->EndRTT();
}

class Blursh : public Render::Post::Shader {
public:
	Blursh(const std::string &v, const std::string &f) :
	  Shader(v, f) { }
	void set_tex(int i) {
		const GLuint loc = glGetUniformLocation(m_program, "tex");
		glUniform1i(loc, i);
	}
};

Blur::Blur(FilterSource source, FilterTarget target) :
	Filter(source, target)
{
	//init the shader
	m_shader = new Blursh("blurShader.vert.glsl", "blurShader.frag.glsl");
}

void Blur::Execute()
{
	m_target->BeginRTT();

	//bind shader & set tex
	glActiveTexture(GL_TEXTURE0);
	m_source->Bind();
	m_shader->Bind();
	m_shader->set_tex(0);
	ScreenAlignedQuad();
	m_shader->Unbind();

	m_target->EndRTT();
}

}
}
