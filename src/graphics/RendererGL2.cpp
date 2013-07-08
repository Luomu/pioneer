// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "RendererGL2.h"
#include "Graphics.h"
#include "Material.h"
#include "RendererGLBuffers.h"
#include "Texture.h"
#include "TextureGL.h"
#include "VertexArray.h"
#include "TextureBuilder.h"
#include "gl2/GeoSphereMaterial.h"
#include "gl2/GL2Material.h"
#include "gl2/GL2RenderTarget.h"
#include "gl2/MultiMaterial.h"
#include "gl2/Program.h"
#include "gl2/RingMaterial.h"
#include "gl2/StarfieldMaterial.h"

namespace Graphics {

typedef std::vector<std::pair<MaterialDescriptor, GL2::Program*> >::const_iterator ProgramIterator;

// for material-less line and point drawing
GL2::MultiProgram *vtxColorProg;
GL2::MultiProgram *flatColorProg;

namespace Targets {
	RenderTarget *scene;
}

namespace Filters {
	GL2::Program *show;
	GL2::Program *grayscale;
}

namespace Resources {
	TextureGL *scanline;
}

RendererGL2::RendererGL2(const Graphics::Settings &vs)
: RendererLegacy(vs)
, m_invLogZfarPlus1(0.f)
, m_activeRenderTarget(0)
{
	//the range is very large due to a "logarithmic z-buffer" trick used
	//http://outerra.blogspot.com/2009/08/logarithmic-z-buffer.html
	//http://www.gamedev.net/blog/73/entry-2006307-tip-of-the-day-logarithmic-zbuffer-artifacts-fix/
	m_minZNear = 0.0001f;
	m_maxZFar = 10000000.0f;

	MaterialDescriptor desc;
	flatColorProg = new GL2::MultiProgram(desc);
	m_programs.push_back(std::make_pair(desc, flatColorProg));
	desc.vertexColors = true;
	vtxColorProg = new GL2::MultiProgram(desc);
	m_programs.push_back(std::make_pair(desc, vtxColorProg));
}

RendererGL2::~RendererGL2()
{
	while (!m_programs.empty()) delete m_programs.back().second, m_programs.pop_back();

	delete Targets::scene;
	delete Filters::show;
	delete Filters::grayscale;
	delete Resources::scanline;
}

void RendererGL2::Init()
{
	RenderTargetDesc rtd(m_width, m_height, TEXTURE_RGB_888, TEXTURE_DEPTH, false);
	Targets::scene = CreateRenderTarget(rtd);

	Filters::show = new GL2::Program("post_show", "");
	Filters::grayscale = new GL2::Program("post_grayscale", "");

	Resources::scanline = static_cast<TextureGL*>(TextureBuilder("textures/scanline.png", LINEAR_REPEAT, false, false, false).CreateTexture(this));
}

bool RendererGL2::BeginFrame()
{
	SetDepthWrite(true);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetRenderTarget(Targets::scene);
	SetDepthWrite(true);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return true;
}

void fs_tri() {

}

bool RendererGL2::EndFrame()
{
	SetRenderTarget(0);

	//draw a textured full screen triangle
	static const float vts[] = {
		-3.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 3.f, 0.f,
	};

	static const float uvs[] = {
		-1.f, 0.f,
		1.f, 0.f,
		1.f, 2.f,
	};

	TextureGL* sceneTex = static_cast<TextureGL*>(Targets::scene->GetColorTexture());

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &vts[0]);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs[0]);

	//Filters::show->Use();
	Filters::grayscale->Use();
	glActiveTexture(GL_TEXTURE0);
	sceneTex->Bind();
	glActiveTexture(GL_TEXTURE1);
	Resources::scanline->Bind();
	Filters::grayscale->texture0.Set(0);
	Filters::grayscale->texture1.Set(1);
	glUniform2f(Filters::grayscale->GetUniformLocation("texelSize"), 1.f/m_width, 1.f/m_height);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glActiveTexture(GL_TEXTURE0);
	sceneTex->Unbind(); //must unbind for RTT to work again

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return true;
}

bool RendererGL2::SetRenderTarget(RenderTarget *rt)
{
	if (rt)
		static_cast<GL2::RenderTarget*>(rt)->Bind();
	else if (m_activeRenderTarget)
		m_activeRenderTarget->Unbind();

	m_activeRenderTarget = static_cast<GL2::RenderTarget*>(rt);

	return true;
}

bool RendererGL2::SetPerspectiveProjection(float fov, float aspect, float near, float far)
{
	// update values for log-z hack
	m_invLogZfarPlus1 = 1.0f / (log(far+1.0f)/log(2.0f));

	return RendererLegacy::SetPerspectiveProjection(fov, aspect, near, far);
}

bool RendererGL2::SetAmbientColor(const Color &c)
{
	m_ambient = c;
	return true;
}

bool RendererGL2::DrawLines(int count, const vector3f *v, const Color *c, LineType t)
{
	if (count < 2 || !v) return false;

	vtxColorProg->Use();
	vtxColorProg->invLogZfarPlus1.Set(m_invLogZfarPlus1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vector3f), v);
	glColorPointer(4, GL_FLOAT, sizeof(Color), c);
	glDrawArrays(t, 0, count);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	vtxColorProg->Unuse();

	return true;
}

bool RendererGL2::DrawLines(int count, const vector3f *v, const Color &c, LineType t)
{
	if (count < 2 || !v) return false;

	flatColorProg->Use();
	flatColorProg->diffuse.Set(c);
	flatColorProg->invLogZfarPlus1.Set(m_invLogZfarPlus1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vector3f), v);
	glDrawArrays(t, 0, count);
	glDisableClientState(GL_VERTEX_ARRAY);
	flatColorProg->Unuse();

	return true;
}

Material *RendererGL2::CreateMaterial(const MaterialDescriptor &d)
{
	MaterialDescriptor desc = d;

	GL2::Material *mat = 0;
	GL2::Program *p = 0;

	if (desc.lighting) {
		desc.dirLights = m_numDirLights;
	}

	// Create the material. It will be also used to create the shader,
	// like a tiny factory
	switch (desc.effect) {
	case EFFECT_PLANETRING:
		mat = new GL2::RingMaterial();
		break;
	case EFFECT_STARFIELD:
		mat = new GL2::StarfieldMaterial();
		break;
	case EFFECT_GEOSPHERE_TERRAIN:
	case EFFECT_GEOSPHERE_TERRAIN_WITH_LAVA:
	case EFFECT_GEOSPHERE_TERRAIN_WITH_WATER:
		mat = new GL2::GeoSphereSurfaceMaterial();
		break;
	case EFFECT_GEOSPHERE_SKY:
		mat = new GL2::GeoSphereSkyMaterial();
		break;
	default:
		if (desc.lighting)
			mat = new GL2::LitMultiMaterial();
		else
			mat = new GL2::MultiMaterial();
		mat->twoSided = desc.twoSided; //other mats don't care about this
	}

	mat->m_renderer = this;
	mat->m_descriptor = desc;

	try {
		p = GetOrCreateProgram(mat);
	} catch (GL2::ShaderException &) {
		// in release builds, the game does not quit instantly but attempts to revert
		// to a 'shaderless' state
		return RendererLegacy::CreateMaterial(desc);
	}

	mat->SetProgram(p);
	return mat;
}

RenderTarget *RendererGL2::CreateRenderTarget(const RenderTargetDesc &desc)
{
	GL2::RenderTarget* rt = new GL2::RenderTarget(desc);
	rt->Bind();
	if (desc.colorFormat != TEXTURE_NONE) {
		Graphics::TextureDescriptor cdesc(
			desc.colorFormat,
			vector2f(desc.width, desc.height),
			vector2f(desc.width, desc.height),
			LINEAR_CLAMP,
			false,
			false);
		TextureGL *colorTex = new TextureGL(cdesc, false);
		rt->SetColorTexture(colorTex);
	}
	if (desc.depthFormat != TEXTURE_NONE) {
		if (desc.allowDepthTexture) {
			Graphics::TextureDescriptor ddesc(
				TEXTURE_DEPTH,
				vector2f(desc.width, desc.height),
				vector2f(desc.width, desc.height),
				LINEAR_CLAMP,
				false,
				false);
			TextureGL *depthTex = new TextureGL(ddesc, false);
			rt->SetDepthTexture(depthTex);
		} else {
			rt->CreateDepthRenderbuffer();
		}
	}
	rt->CheckStatus();
	rt->Unbind();
	return rt;
}

bool RendererGL2::ReloadShaders()
{
	printf("Reloading " SIZET_FMT " programs...\n", m_programs.size());
	for (ProgramIterator it = m_programs.begin(); it != m_programs.end(); ++it) {
		it->second->Reload();
	}
	printf("Done.\n");

	return true;
}

GL2::Program* RendererGL2::GetOrCreateProgram(GL2::Material *mat)
{
	const MaterialDescriptor &desc = mat->GetDescriptor();
	GL2::Program *p = 0;

	// Find an existing program...
	for (ProgramIterator it = m_programs.begin(); it != m_programs.end(); ++it) {
		if ((*it).first == desc) {
			p = (*it).second;
			break;
		}
	}

	// ...or create a new one
	if (!p) {
		p = mat->CreateProgram(desc);
		m_programs.push_back(std::make_pair(desc, p));
	}

	return p;
}

}
