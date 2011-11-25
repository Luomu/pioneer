#include "RenderGLSLFilter.h"
#include "RenderShader.h"
#include "RenderTarget.h"
#include "Render.h"
#include "RenderTexture.h"

namespace Render {

GLSLFilter::GLSLFilter(const std::string &vertname, const std::string &fragname, RenderTarget *rt) :
	Filter(rt)
{
	m_program = new Post::Program(vertname, fragname);
}

GLSLFilter::GLSLFilter(Post::Program *prog, RenderTarget *rt) :
	Filter(rt),
	m_program(prog)
{

}

GLSLFilter::~GLSLFilter()
{
	if (0 != m_program)
		delete m_program;
	delete m_textures[0];
}

void GLSLFilter::SetProgramParameters()
{
	m_source->GetTexture(0)->Bind();
	m_program->SetUniform1i("fboTex", 0);
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

	m_program->Use();

	SetProgramParameters();

	DoQuad(x, y, w, h);

	glActiveTexture(GL_TEXTURE0);

	m_program->Unuse();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
	static_cast<RenderTarget*>(m_textures[0])->EndRTT();
}

ColorLUTFilter::ColorLUTFilter(RenderTarget *t) :
	GLSLFilter("ExampleFilter.vert", "ExampleFilterLUT.frag", t)
{
	m_lut.SetFilterMode(Texture::NEAREST);
	m_lut.Load(PIONEER_DATA_DIR"/textures/colortable.png");
}

void ColorLUTFilter::SetProgramParameters()
{
	m_source->GetTexture(0)->Bind();
	m_program->SetUniform1i("texture0", 0);
	glActiveTexture(GL_TEXTURE1);
	m_lut.Bind();
	m_program->SetUniform1i("texture1", 1);
}

MultiBlurFilter::MultiBlurFilter(RenderTarget *finalTarget) :
	Filter(finalTarget)
{
	m_program = new Post::Program("ExampleFilter.vert", "ExampleFilterBlur.frag");
	m_filter1 = new GLSLFilter(m_program,
		new RenderTarget(finalTarget->Width(), finalTarget->Height(), GL_RGB, GL_RGB, GL_FLOAT));
	m_filter2 = new GLSLFilter(m_program, finalTarget);
}

MultiBlurFilter::~MultiBlurFilter()
{
	//temp hack
	m_filter1->SetProgram(0);
	delete m_filter1;
	m_filter2->SetProgram(0);
	delete m_filter2;
	delete m_program;
}

void MultiBlurFilter::Execute()
{
	m_filter1->SetSource(m_source);
	m_filter1->Execute();
	m_filter2->SetSource(m_filter1);
	m_filter2->Execute();
}

}
