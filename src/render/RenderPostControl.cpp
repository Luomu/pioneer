#include "RenderPostControl.h"
#include "RenderPostPass.h"
#include "RenderPostProgram.h"
#include "RefCounted.h"
#include "RenderResourceManager.h"
#include "Render.h"
#include "RenderHDR.h"

namespace Render {

namespace Post {

Control::Control(int width, int height) :
	m_viewWidth(width),
	m_viewHeight(height),
	m_tf(GL_RGB, GL_RGB8, GL_UNSIGNED_BYTE),
	m_hdrtf(GL_RGB, GL_RGB32F, GL_FLOAT)
{
	m_sceneTarget = new SceneTarget(width, height, m_hdrtf);
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

Pass* Control::AddPass(RefCountedPtr<Program> prog)
{
	Pass *pass = new Pass(this, prog);
	m_passes.push_back(pass);
	return pass;
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

	RefCountedPtr<RenderTarget> luminanceTarget(new LuminanceTarget(m_hdrtf));
	RefCountedPtr<LuminanceTarget> luminanceTexture(static_cast<LuminanceTarget*>(luminanceTarget.Get())); // bleh
	RefCountedPtr<RenderTarget> halfT =
		rm->RequestRenderTarget(w>>1, h>>1, m_hdrtf);
	RefCountedPtr<RenderTarget> brightT =
		rm->RequestRenderTarget(w>>1, h>>1, m_hdrtf);
	RefCountedPtr<RenderTarget> bloomT1 =
		rm->RequestRenderTarget(w>>1, h>>1, m_hdrtf);
	RefCountedPtr<RenderTarget> bloomT2 =
		rm->RequestRenderTarget(w>>1, h>>1, m_hdrtf);

	//luminance pass
	Pass *lum = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/luminance.frag"));
	lum->AddSampler("sceneTex", m_sceneTarget);
	lum->SetTarget(luminanceTarget);
	m_passes.push_back(lum);

	//downsample
	Pass *ds = AddPass(rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/downsample2.frag"));
	ds->SetTarget(halfT);
	ds->AddSampler("texture0", m_sceneTarget);

	//bloom brightpass
	Pass *bp = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/brightpass.frag"));
	bp->SetTarget(brightT);
	bp->AddSampler("sceneTex", ds);
	bp->AddUniform(new AverageLuminance("avgLum", luminanceTexture));
	m_passes.push_back(bp);

	//first blur
	Pass *blur01 = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/blurV.frag"));
	blur01->AddSampler("texture0", bp);
	blur01->AddUniform("sampleDist", 0.0015f);
	blur01->SetTarget(bloomT1);
	m_passes.push_back(blur01);

	//second blur
	Pass *blur02 = new Pass(this, rm->RequestProgram("filters/Quad.vert", "filters/classicHDR/blurH.frag"));
	blur02->AddSampler("texture0", blur01);
	blur02->AddUniform("sampleDist", 0.0015f);
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