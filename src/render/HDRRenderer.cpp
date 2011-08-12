#include "HDRRenderer.h"
#include "RenderTarget.h"
#include "PostFilter.h"
#include "PostShader.h"
#include <GL/glew.h>
#include "Texture.h"

namespace Render {

namespace Post {
namespace ClassicHDR {

class Luminance : public ShaderFilter {
public:
	Luminance(FilterSource source, FilterTarget target) :
		ShaderFilter(source, target,
			"hdr/compose.vert", "hdr/luminance.frag") {

	}
};

class Compose : public ShaderFilter {
public:
	//needs: FilterSource bloom
	//maybe: FilterSource luminance for avglum + middlegrey
	/*FilterSource bloom, FilterSource luminance,*/
	Compose(FilterSource source, LuminanceRenderTarget *luminance,
		FilterTarget target) :
		ShaderFilter(source, target,
			"hdr/compose.vert", "hdr/compose.frag") {

	}
	//could render directly to fbo0
	void Execute() {
		//m_target->BeginRTT();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_shader->Bind();
		SetUniforms();
		ScreenAlignedQuad();//  using viewport
		m_shader->Unbind();
		glBindTexture(GL_TEXTURE_2D, 0);
		//m_target->EndRTT();
	}

protected:
	void SetUniforms() {
		//poop
		GLuint loc = glGetUniformLocation(m_shader->GetProgram(), "fboTex");
		//glBindTexture(GL_TEXTURE_RECTANGLE, m_source->GetTexture());
		m_source->Bind();
		glUniform1i(loc, 0);
		//texture 0: fbotex
		//texture 1: bloomtex
		//avglum
		//middlegrey
	}

private:
	LuminanceRenderTarget *m_luminance;
};

} //namespace ClassicHDR
} //namespace Post

class LuminanceRenderTarget : public RenderTarget {
public:
	LuminanceRenderTarget(int w, int h) :
		RenderTarget(w, h, GL_RGB, GL_RGB16F, GL_FLOAT),
		midGrey(0.f)
	{ }

	void EndRTT() {
		RenderTarget::EndRTT();
		Bind();
		glGenerateMipmap(GL_TEXTURE_2D);
		//extract average luminance & calculate middle grey
		glGetTexImage(GL_TEXTURE_2D, 7, GL_RGB, GL_FLOAT, avgLum);
		Unbind();
		avgLum[0] = std::max(float(exp(avgLum[0])), 0.03f);
		midGrey = 1.03f - 2.0f/(2.0f+log10(avgLum[0] + 1.0f));
	}

	float GetAverageLuminance() const { return avgLum[0]; }
	float GetMiddleGrey() const { return midGrey; }

protected:
	void SetParameters() {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		doMipmaps = true;
	}

private:
	float avgLum[4];
	float midGrey;
};

class HDRRenderTarget : public RenderTarget {
public:
	HDRRenderTarget(int w, int h) :
		RenderTarget(w, h, GL_RGB, GL_RGB16F, GL_HALF_FLOAT)
	{ }
};

HDRRenderer::HDRRenderer(int w, int h) :
	Renderer(w, h)
{
	using namespace Post::ClassicHDR;
	//using FBOs like this is questionable. It would be better to shuffle attachments
	//or just flip textures assuming the dimensions/formats are the same
	m_target = new HDRRenderTarget(w, h);
	m_luminanceTarget = new LuminanceRenderTarget(128, 128);

	//build filter chain here
	m_filters.push_back(new Luminance(m_target, m_luminanceTarget));
	m_filters.push_back(new Compose(m_target, m_luminanceTarget, 0));
}

HDRRenderer::~HDRRenderer()
{
	delete m_target;
	delete m_luminanceTarget;
	while (!m_filters.empty()) delete m_filters.back(), m_filters.pop_back();
}

void HDRRenderer::BeginFrame()
{
	m_target->BeginRTT();
}

void HDRRenderer::EndFrame()
{
	m_target->EndRTT();
	for(std::vector<Post::Filter*>::iterator it = m_filters.begin();
		it != m_filters.end(); ++it) {
		(*it)->Execute();
	}

	//Post::Present present(m_target);
	//present.Execute();
	//~ m_target->Show(0.f, 0.f, 30.f, 30.f);
}

void HDRRenderer::ReloadShaders()
{
	for(std::vector<Post::Filter*>::iterator it = m_filters.begin();
		it != m_filters.end(); ++it) {
		(*it)->Reload();
	}
}

}
