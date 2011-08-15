#include "libs.h"
#include "PostFilter.h"
#include "PostShader.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "HDRRenderer.h"

namespace Render {

class LuminanceRenderTarget : public RenderTarget {
public:
	LuminanceRenderTarget(int w, int h) :
		RenderTarget(w, h),
		m_luminanceBias(7.f)
	{
		glGenFramebuffers(1, &m_fbo);
		glGenTextures(1, &m_texture);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, m_w, m_h, 0, GL_RGB, GL_FLOAT, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
		CheckCompleteness();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	float GetLuminanceBias() const  { return m_luminanceBias; }
	void SetLuminanceBias(float b)  { m_luminanceBias = b;    }

private:
	float m_luminanceBias;

	void EndRTT() {
		RenderTarget::EndRTT();
		Bind();
		glGenerateMipmap(GL_TEXTURE_2D);
		//extract average luminance & calculate middle grey
		/*glGetTexImage(GL_TEXTURE_2D, 7, GL_RGB, GL_FLOAT, avgLum);
		Unbind();
		avgLum[0] = std::max(float(exp(avgLum[0])), 0.03f);
		midGrey = 1.03f - 2.0f/(2.0f+log10(avgLum[0] + 1.0f));*/
	}
/*
	float GetAverageLuminance() const { return avgLum[0]; }
	float GetMiddleGrey() const { return midGrey; }

private:
	float avgLum[4];
	float midGrey;
*/
};

namespace ClassicHDR {
namespace Shaders {

/*
 * Final shader of ClassicHDR
 * Inputs: scene texture, bloom texture,
 * average luminance, middle grey
 */
class Compose : public Post::Shader {
public:
	Compose(const std::string &v, const std::string &f) :
		Shader(v, f)
	{
		sceneTexture     = AddUniform("sceneTexture");
		luminanceTexture = AddUniform("luminanceTexture");
		luminanceBias    = AddUniform("luminanceBias");
		//~ averageLuminance = AddUniform("avgLum");
		//~ middleGrey = AddUniform("middleGrey");
	}
	Uniform *sceneTexture;
	Uniform *luminanceTexture;
	Uniform *luminanceBias;
	//~ Uniform *middleGrey;
	//~ Uniform *averageLuminance;
};

} //namespace Shaders

namespace Filters {

class Luminance : public Post::ShaderFilter {
public:
	Luminance(Post::FilterSource source, LuminanceRenderTarget *target) :
		ShaderFilter(source, target,
			"hdr/compose.vert", "hdr/luminance.frag") {

	}
};

class Compose : public Post::ShaderFilter {
public:
	Compose(Post::FilterSource source, LuminanceRenderTarget *luminance,
		Post::FilterTarget target) :
		ShaderFilter(source, target, new Shaders::Compose("hdr/compose.vert", "hdr/compose.frag")),
		m_luminance(luminance)
	{	}

protected:
	void SetUniforms() {
		m_source->Bind();
		Shaders::Compose *shader = reinterpret_cast<Shaders::Compose*>(m_shader);
		shader->sceneTexture->Set(0);
		glActiveTexture(GL_TEXTURE1);
		m_luminance->Bind();
		shader->luminanceTexture->Set(1);
		shader->luminanceBias->Set(m_luminance->GetLuminanceBias());
		//~ shader->averageLuminance->Set(m_luminance->GetAverageLuminance());
		//~ shader->middleGrey->Set(m_luminance->GetMiddleGrey());
	}

	void CleanUp() {
		m_luminance->Unbind();
		glActiveTexture(GL_TEXTURE0);
		m_source->Unbind();
	}

private:
	LuminanceRenderTarget *m_luminance;
};

} //namespace Filters
} //namespace ClassicHDR

class HDRRenderTarget : public RenderTarget {
public:
	HDRRenderTarget(int w, int h) :
		RenderTarget(w, h, GL_RGB, GL_RGB16F, GL_HALF_FLOAT)
	{ }
};

HDRRenderer::HDRRenderer(int w, int h) :
	Renderer(w, h)
{
	using namespace ClassicHDR;
	try {
		//using FBOs like this is questionable. It would be better to shuffle attachments
		//or just flip textures assuming the dimensions/formats are the same
		m_target = new HDRRenderTarget(w, h);
		m_luminanceTarget = new LuminanceRenderTarget(1024, 1024);

		//build filter chain here
		m_filters.push_back(new Filters::Luminance(m_target, m_luminanceTarget));
		m_filters.push_back(new Filters::Compose(m_target, m_luminanceTarget, 0));
	} catch (RenderException &ex) {
		fprintf(stderr, "Render init fail: %s, aborting\n", ex.what());
		exit(1);
	}
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
	glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, m_luminanceTarget->GetLuminanceBias());
	m_luminanceTarget->Show(0.f, 0.f, 30.f, 30.f);
	glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0.f);
}

void HDRRenderer::ReloadShaders()
{
	for(std::vector<Post::Filter*>::iterator it = m_filters.begin();
		it != m_filters.end(); ++it) {
		(*it)->Reload();
	}
}

void HDRRenderer::SetLuminanceBias(float b)
{
	m_luminanceTarget->SetLuminanceBias(b);
}

}
