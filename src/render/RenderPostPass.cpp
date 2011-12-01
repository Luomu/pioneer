#include "RenderPostPass.h"
#include "RenderPostProgram.h"
#include "RenderTarget.h"

namespace Render { namespace Post {

Pass::Pass(Program *p) :
	renderToScreen(true),
	m_target(0),
	m_program(p)
{

}

Pass::~Pass()
{

}

void Pass::Execute()
{
	if (!renderToScreen)
		m_target->BeginRTT();

	const float x = 0.f;
	const float y = 0.f;
	const float w = 1.f;
	const float h = 1.f;
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.f, w , 0.f, h, 0.1f, 2.f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.f, 0.f, -1.f);

	m_program->Use();

	SetProgramParameters();

	DoQuad(x, y, w, h);

	m_program->Unuse();

	CleanUpProgramParameters();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();

	if (!renderToScreen)
		m_target->EndRTT();
}

void Pass::SetProgramParameters()
{

}

void Pass::CleanUpProgramParameters()
{

}

void Pass::DoQuad(const float x, const float y, const float w, const float h)
{
	//quad
	const GLfloat vertices[] = {
		x,     y,     0.f,
		x + w, y,     0.f,
		x + w, y + h, 0.f,
		x,     y + h, 0.f
	};
	const GLfloat texcoords[] = {
		0.f, 0.f,
		1.f, 0.f,
		1.f, 1.f,
		0.f, 1.f,
	};
	glColor3f(1.f, 1.f, 1.f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

}}
