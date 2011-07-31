#include "PostFilter.h"
#include "RenderTarget.h"

namespace Render {
namespace Post {

Filter::Filter(FilterSource source, FilterTarget target) :
	m_source(source),
	m_target(target)
{

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
	//todo: use some generic SAQ function here
	const int width = 100;
	const int height = 100;
	const float x = 0.f;
	const float y = 0.f;
	const float w = 100.f;
	const float h = 100.f;

	glColor4f(1.f, 1.f, 0.f, 1.f); //the surprise is yellow

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
	m_target->EndRTT();
}

Blur::Blur(FilterSource source, FilterTarget target) :
	Filter(source, target)
{
	//init the shader
	m_shader = new Blursh("blurShader");
}

void Blur::Execute()
{
	m_target->BeginRTT();
	//todo: use some generic SAQ function here
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

	//bind shader & set tex
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_source->GetTexture());
	glUseProgram(m_shader->GetProgram());
	m_shader->set_tex(0);

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

	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
	m_target->EndRTT();
}

}
}