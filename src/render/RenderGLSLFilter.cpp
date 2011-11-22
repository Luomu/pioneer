#include "RenderGLSLFilter.h"
#include "RenderShader.h"
#include "RenderTarget.h"
#include "Render.h"
#include "RenderTexture.h"

namespace Render {

SHADER_CLASS_BEGIN(GLSLFilterProgram)
	SHADER_UNIFORM_SAMPLER(texture0)
	SHADER_UNIFORM_SAMPLER(texture1)
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

void GLSLFilter::SetProgramParameters()
{
	m_source->GetTexture(0)->Bind();
	m_program->set_texture0(0);
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

	SetProgramParameters();

	DoQuad(x, y, w, h);

	glActiveTexture(GL_TEXTURE0);

	Render::State::UseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
	static_cast<RenderTarget*>(m_textures[0])->EndRTT();
}

ColorLUTFilter::ColorLUTFilter(const std::string &progName, RenderTarget *t) : GLSLFilter(progName, t)
{
	m_lut.Load(PIONEER_DATA_DIR"/textures/colortable.png");
}

void ColorLUTFilter::SetProgramParameters()
{
	m_source->GetTexture(0)->Bind();
	m_program->set_texture0(0);
	glActiveTexture(GL_TEXTURE1);
	m_lut.Bind();
	m_program->set_texture1(1);
}

}
