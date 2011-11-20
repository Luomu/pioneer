#include "RenderFilter.h"
#include "RenderTexture.h"
#include "libs.h"

namespace Render {

/* Render the source texture to screen */
void PresentOperator::Execute()
{
	const float x = 0.f;
	const float y = 0.f;
	const float w = 1.f;
	const float h = 1.f;
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.f, w , 0.f, h, 0.1f, 2.f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.f, 0.f, -1.f);

	//bind source texture
	glEnable(GL_TEXTURE_2D);
	m_source->GetTexture(0)->Bind();

	DoQuad(x, y, w, h);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

void Sink::DoQuad(const float x, const float y, const float w, const float h)
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

} // namespace Render
