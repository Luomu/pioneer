#include "RenderPostPass.h"
#include "RenderPostProgram.h"
#include "RenderTarget.h"
#include "RenderPostControl.h"

namespace Render { namespace Post {

Pass::Pass(Control *c, RefCountedPtr<Program> p) :
	renderToScreen(false),
	m_target(0),
	m_program(p),
	m_control(c)
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

	// HACK
	glActiveTexture(GL_TEXTURE0);
}

void Pass::SetProgramParameters()
{
	m_program->SetUniform2f("viewportSize",
		m_control->GetViewportWidth(),
		m_control->GetViewportHeight());
	for (std::vector<Uniform>::size_type i = 0; i != m_uniforms.size(); ++i) {
		m_uniforms[i].Set();
	}
	for (std::vector<Sampler>::size_type i = 0; i != m_samplers.size(); ++i) {
		m_samplers[i].Set(i);
	}
}

void Pass::CleanUpProgramParameters()
{
	for (std::vector<Sampler>::size_type i = 0; i != m_samplers.size(); ++i) {
		m_samplers[i].Unset(i);
	}
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

void Pass::AddSampler(const std::string &name, Texture *tex)
{
	Sampler s;//(name, tex);
	s.m_name = name;
	s.m_texture = tex;
	s.m_location = 0;
	s.m_program = m_program;
	m_samplers.push_back(s);
}

void Pass::AddSampler(const std::string &name, Pass *pass)
{
	AddSampler(name, pass->GetTarget().Get());
}

void Pass::AddUniform(const std::string &name, float value)
{
	Uniform u;
	u.m_name     = name;
	u.m_location = 0;
	u.m_program  = m_program;
	u.m_value    = value;
	m_uniforms.push_back(u);
}

void Sampler::Set(int texunit)
{
	/*if (m_location < 1) {
		m_program->GetLocation(m_name.c_str());
	}*/
	glActiveTexture(GL_TEXTURE0 + texunit);
	m_texture->Bind();
	m_program->SetUniform1i(m_name.c_str(), texunit);
}

void Sampler::Unset(int texunit)
{
	m_texture->Unbind();
	if (texunit > 0)
		glActiveTexture(GL_TEXTURE0 + texunit - 1);
}

void Uniform::Set()
{
	m_program->SetUniform1f(m_name.c_str(), m_value);
}

}}
