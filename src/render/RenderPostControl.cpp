#include "RenderPostControl.h"
#include "RenderPostPass.h"
#include "RenderPostProgram.h"
#include "RefCounted.h"
#include "RenderResourceManager.h"
#include "Render.h"
#include "RenderHDR.h"

namespace Render {

PostSceneTarget::PostSceneTarget(int width, int height)
{
	const GLint format         = GL_RGB;
	const GLint internalFormat = GL_RGB;
	const GLenum type          = GL_FLOAT;
	m_w = width;
	m_h = height;

	glGenFramebuffersEXT(1, &m_fbo);
	glGenTextures(1, &m_texture);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
	Bind();
	SetWrapMode(CLAMP);
	SetFilterMode(LINEAR);
	glTexImage2D(m_target, 0, internalFormat, m_w, m_h, 0,
		format, type, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		m_target, m_texture, 0);

	m_depthTexture = new Texture(m_w, m_h, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_UNSIGNED_BYTE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
		m_target, m_depthTexture->GetGLTexture(), 0);
	/*
	Renderbuffer instead of texture:
	glGenRenderbuffersEXT(1, &m_depth);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_w, m_h);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
		GL_RENDERBUFFER_EXT, m_depth);
	*/

	CheckCompleteness();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

PostSceneTarget::~PostSceneTarget()
{
	delete m_depthTexture;
}

namespace Post {

Control::Control(int width, int height) :
	m_viewWidth(width),
	m_viewHeight(height)
{
	// request a SceneTarget of appropriate size from some manager
	// request programs and the rest of targets from some manager

	m_sceneTarget = new PostSceneTarget(width, height);
	SetUpPasses();
}

Control::~Control()
{
	while (!m_passes.empty()) delete m_passes.back(), m_passes.pop_back();
}

void Control::BeginFrame()
{
	m_sceneTarget->BeginRTT();
}

void Control::EndFrame()
{
	m_sceneTarget->EndRTT();
	PostProcess();
}

void Control::PostProcess()
{
	std::vector<Post::Pass*>::iterator it = m_passes.begin();
	for(; it != m_passes.end(); ++it) {
		(*it)->Execute();
	}
}

void Control::SetUpPasses()
{
	return SetUpClassicHDR();

	const int w = m_viewWidth;
	const int h = m_viewHeight;
	ResourceManager *rm = Render::resourceManager;
	RefCountedPtr<RenderTarget> t1 = rm->RequestRenderTarget(w, h);
	RefCountedPtr<RenderTarget> t2 = rm->RequestRenderTarget(w, h);
	RefCountedPtr<RenderTarget> t3 = rm->RequestRenderTarget(w, h);

	Post::Pass *p1 = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/Grayscale.frag"));
	p1->AddSampler("texture0", m_sceneTarget);
	p1->SetTarget(t1);
	m_passes.push_back(p1);

	Post::Pass *p2 = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/rmBlur.frag"));
	p2->AddSampler("texture0", p1->GetTarget().Get()); // XXX yes, Get, I know
	p2->AddUniform("sampleDist", 0.01f);
	p2->SetTarget(t2);
	m_passes.push_back(p2);

	Post::Pass *p3 = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/rmBlur.frag"));
	p3->AddSampler("texture0", p2->GetTarget().Get());
	p3->AddUniform("sampleDist", 0.02f);
	p3->SetTarget(t3);
	//p3->renderToScreen = true;
	m_passes.push_back(p3);

	Post::Pass *p4 = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/composite2.frag"));
	p4->AddSampler("texture0", p1->GetTarget().Get());
	p4->AddSampler("texture1", p3->GetTarget().Get());
	p4->AddUniform("mixFactor", 0.5f);
	p4->renderToScreen = true;
	m_passes.push_back(p4);
}

void Control::SetUpClassicHDR()
{
	const int w = m_viewWidth;
	const int h = m_viewHeight;
	ResourceManager *rm = Render::resourceManager;

	/*RefCountedPtr<RenderTarget> tinyTarget = rm->RequestRenderTarget(128, 128,
		TextureFormat(GL_RGB, GL_RGB16F_ARB, GL_HALF_FLOAT_ARB), true);*/
	RefCountedPtr<RenderTarget> luminanceTarget(new LuminanceTarget(128, 128));
	RefCountedPtr<LuminanceTarget> luminanceTexture(static_cast<LuminanceTarget*>(luminanceTarget.Get())); // bleh
	TextureFormat fmt(GL_RGB, GL_RGB16F_ARB, GL_HALF_FLOAT_ARB);
	fmt = TextureFormat(GL_RGB, GL_RGB32F, GL_FLOAT);
	RefCountedPtr<RenderTarget> brightT =
		rm->RequestRenderTarget(w, h, fmt);
	RefCountedPtr<RenderTarget> bloomT1 =
		rm->RequestRenderTarget(w>>1, h>>1, fmt);
	RefCountedPtr<RenderTarget> bloomT2 =
		rm->RequestRenderTarget(w>>1, h>>1, fmt);

	//luminance pass
	Pass *lum = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/luminance.frag"));
	lum->AddSampler("sceneTex", m_sceneTarget);
	lum->SetTarget(luminanceTarget);
	m_passes.push_back(lum);

	//bloom brightpass
	Pass *bp = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/brightpass.frag"));
	bp->AddSampler("sceneTex", m_sceneTarget);
	bp->SetTarget(brightT);
	bp->AddUniform(new AverageLuminance("avgLum", luminanceTexture));
	m_passes.push_back(bp);

	//first blur
	Pass *blur01 = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/blurV.frag"));
	blur01->AddSampler("texture0", bp);
	blur01->AddUniform("sampleDist", 0.005f);
	blur01->SetTarget(bloomT1);
	m_passes.push_back(blur01);

	//second blur
	Pass *blur02 = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/blurH.frag"));
	blur02->AddSampler("texture0", blur01);
	blur02->AddUniform("sampleDist", 0.005f);
	blur02->SetTarget(bloomT2);
	m_passes.push_back(blur02);

	Pass *comp = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/compose.frag"));
	comp->AddSampler("sceneTex", m_sceneTarget);
	comp->AddSampler("bloomTex01", blur02);
	comp->AddUniform(new AverageLuminance("avgLum", luminanceTexture));
	comp->AddUniform(new MiddleGrey("middleGrey", luminanceTexture));
	comp->renderToScreen = true;
	m_passes.push_back(comp);
}

} }
