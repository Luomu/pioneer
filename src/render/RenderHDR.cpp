#include "RenderHDR.h"
#include "RenderPostProgram.h"
#include "RenderPostControl.h"

namespace Render { namespace Post {

LuminanceTarget::LuminanceTarget(int w, int h)
{
	m_w      = w;
	m_h      = h;
	m_target = GL_TEXTURE_2D;

	//m_format = TextureFormat(GL_RGB, GL_RGB16F_ARB, GL_HALF_FLOAT_ARB);
	m_format = TextureFormat(GL_RGB, GL_RGB32F, GL_FLOAT);

	glGenFramebuffersEXT(1, &m_fbo);
	glGenTextures(1, &m_texture);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

	BindTexture();
	SetWrapMode(CLAMP);
	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(m_target, 0, m_format.internalFormat, w, h, 0,
			m_format.format, m_format.type, 0);
	glGenerateMipmapEXT(m_target);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		m_target, m_texture, 0);
	UnbindTexture();

	CheckCompleteness();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void LuminanceTarget::EndRTT()
{
	//restore viewport and unbind fbo
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	BindTexture();
	glGenerateMipmapEXT(m_target);
	float avgLum[4];
	glGetTexImage(m_target, 7, m_format.format, m_format.type, avgLum);
	avgLum[0] = std::max(float(exp(avgLum[0])), 0.03f);
	const float midGrey = 1.03f - 2.0f/(2.0f+log10(avgLum[0] + 1.0f));

	averageLuminance = avgLum[0];
	middleGrey       = midGrey;

	UnbindTexture();
}

AverageLuminance::AverageLuminance(const std::string &name, RefCountedPtr<LuminanceTarget> t)
{
	m_name = name;
	luminanceTexture = t;
}

void AverageLuminance::Set()
{
	m_program->SetUniform1f(m_name.c_str(), luminanceTexture->averageLuminance);
}

MiddleGrey::MiddleGrey(const std::string &name, RefCountedPtr<LuminanceTarget> t) :
	AverageLuminance(name, t)
{
}

void MiddleGrey::Set()
{
	m_program->SetUniform1f(m_name.c_str(), luminanceTexture->middleGrey);
}

} }
