#include "HDRRenderer.h"
#include "RenderTarget.h"
#include "PostFilter.h"
#include "PostShader.h"
#include <GL/glew.h>
#include "Texture.h"

namespace Render {

namespace Post {
namespace ClassicHDR {

class Compose : public ShaderFilter {
public:
	//needs: FilterSource bloom
	//maybe: FilterSource luminance for avglum + middlegrey
	/*FilterSource bloom, FilterSource luminance,*/
	Compose(FilterSource source, FilterTarget target) :
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
		m_source->GetTexture()->Bind();
		glUniform1i(loc, 0);
		//texture 0: fbotex
		//texture 1: bloomtex
		//avglum
		//middlegrey
	}
};

} //namespace ClassicHDR
} //namespace Post

class HDRRenderTarget : public RenderTarget {
public:
	HDRRenderTarget(int w, int h) :
		RenderTarget(w, h, GL_RGB, GL_RGB16F, GL_HALF_FLOAT)
	{ }
};

HDRRenderer::HDRRenderer(int w, int h) :
	Renderer(w, h)
{
	//using FBOs like this is questionable. It would be better to shuffle attachments
	//or just flip textures assuming the dimensions/formats are the same
	m_target = new HDRRenderTarget(w, h);
	//build filter chain here
	m_filters.push_back(new Post::ClassicHDR::Compose(m_target, 0));
}

HDRRenderer::~HDRRenderer()
{
	delete m_target;
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
