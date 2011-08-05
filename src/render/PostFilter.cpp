#include "PostFilter.h"
#include "RenderTarget.h"
#include "PostShader.h"

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
	glBindTexture(GL_TEXTURE_2D, m_source->GetTexture());

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
	glBindTexture(GL_TEXTURE_2D, m_source->GetTexture());

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
	glBindTexture(GL_TEXTURE_2D, m_source->GetTexture());
	m_shader->Bind();
	m_shader->set_tex(0);
	ScreenAlignedQuad();
	m_shader->Unbind();

	m_target->EndRTT();
}

}
}