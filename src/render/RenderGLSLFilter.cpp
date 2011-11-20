#include "RenderGLSLFilter.h"
#include "RenderShader.h"
#include "RenderTarget.h"
#include "Render.h"

namespace Render {

SHADER_CLASS_BEGIN(GLSLFilterProgram)
	SHADER_UNIFORM_SAMPLER(fboTex)
SHADER_CLASS_END()

GLSLFilter::GLSLFilter(const std::string &progname, RenderTarget *rt) :
	Filter(rt)
{
	m_program = new GLSLFilterProgram(progname.c_str(), "#version 120\n");
}

GLSLFilter::~GLSLFilter()
{
	delete m_program;
	delete m_textures[0];
}

void GLSLFilter::Execute()
{
	static_cast<RenderTarget*>(m_textures[0])->BeginRTT();
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

	Render::State::UseProgram(m_program);

	//bind source texture
	m_source->GetTexture(0)->Bind();
	m_program->set_fboTex(0);

	DoQuad(x, y, w, h);

	Render::State::UseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
	static_cast<RenderTarget*>(m_textures[0])->EndRTT();
}

}
