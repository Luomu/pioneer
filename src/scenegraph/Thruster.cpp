// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Thruster.h"
#include "NodeVisitor.h"
#include "graphics/Renderer.h"
#include "graphics/VertexArray.h"
#include "graphics/Material.h"
#include "graphics/TextureBuilder.h"

namespace SceneGraph {

static const std::string thrusterTextureFilename("textures/thruster.png");
static const std::string thrusterGlowTextureFilename("textures/halo.png");
static Color baseColor(0.7f, 0.6f, 1.f, 1.f);

Thruster::Thruster(Graphics::Renderer *r)
: Node(r, NODE_TRANSPARENT)
, m_intensity(0.f)
{
	m_tVerts.Reset(CreateGeometry());

	//set up materials
	Graphics::MaterialDescriptor desc;
	desc.textures = 1;
	desc.twoSided = true;
	m_tMat.Reset(r->CreateMaterial(desc));
	m_tMat->texture0 = Graphics::TextureBuilder::Billboard(thrusterTextureFilename).GetOrCreateTexture(r, "model");
	m_tMat->diffuse = baseColor;
}

Thruster::Thruster(const Thruster &thruster, NodeCopyCache *cache)
: Node(thruster, cache)
, m_tMat(thruster.m_tMat)
{
	m_tVerts.Reset(CreateGeometry());
}

Node* Thruster::Clone(NodeCopyCache *cache)
{
	return this; //thrusters are shared
}

void Thruster::Accept(NodeVisitor &nv)
{
	nv.ApplyThruster(*this);
}

void Thruster::Render(const matrix4x4f &trans, RenderData *rd)
{
	if (m_intensity < 0.001f) return;

	Graphics::Renderer *r = GetRenderer();
	r->SetBlendMode(Graphics::BLEND_ADDITIVE);
	r->SetDepthWrite(false);
	r->SetTransform(trans);
	m_tMat->diffuse = baseColor * m_intensity;
	r->DrawTriangles(m_tVerts.Get(), m_tMat.Get());
	r->SetBlendMode(Graphics::BLEND_SOLID);
	r->SetDepthWrite(true);
}

void Thruster::SetIntensity(float i)
{
	m_intensity = Clamp(i, 0.f, 1.f);
}

Graphics::VertexArray *Thruster::CreateGeometry()
{
	Graphics::VertexArray *verts =
		new Graphics::VertexArray(Graphics::ATTRIB_POSITION | Graphics::ATTRIB_UV0);

	//zero at thruster center
	//+x down
	//+y right
	//+z backwards (or thrust direction)
	const float w = 0.5f;

	vector3f one(0.f, -w, 0.f); //top left
	vector3f two(0.f,  w, 0.f); //top right
	vector3f three(0.f,  w, 1.f); //bottom right
	vector3f four(0.f, -w, 1.f); //bottom left

	//uv coords
	const vector2f topLeft(0.f, 1.f);
	const vector2f topRight(1.f, 1.f);
	const vector2f botLeft(0.f, 0.f);
	const vector2f botRight(1.f, 0.f);

	//add four intersecting planes to create a volumetric effect
	for (int i=0; i < 4; i++) {
		verts->Add(one, topLeft);
		verts->Add(two, topRight);
		verts->Add(three, botRight);

		verts->Add(three, botRight);
		verts->Add(four, botLeft);
		verts->Add(one, topLeft);

		one.ArbRotate(vector3f(0.f, 0.f, 1.f), DEG2RAD(45.f));
		two.ArbRotate(vector3f(0.f, 0.f, 1.f), DEG2RAD(45.f));
		three.ArbRotate(vector3f(0.f, 0.f, 1.f), DEG2RAD(45.f));
		four.ArbRotate(vector3f(0.f, 0.f, 1.f), DEG2RAD(45.f));
	}

	return verts;
}

}
