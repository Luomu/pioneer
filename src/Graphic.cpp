// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Graphic.h"
#include "Sfx.h"
#include "Projectile.h"
#include "graphics/Drawables.h"
#include "graphics/Renderer.h"
#include "graphics/Material.h"
#include "graphics/VertexArray.h"

using Graphics::Renderer;
using Graphics::Renderable;
using Graphics::VertexArray;
using Graphics::Material;

RenderableGraphic::RenderableGraphic(Renderer *renderer, Renderable *renderable)
: Graphic(renderer)
, m_renderable(renderable)
{
}

void RenderableGraphic::Draw()
{
	m_renderer->SetTransform(m_transform);
	m_renderable->Draw(m_renderer);
}

TriangleGraphic::TriangleGraphic(Renderer *renderer, VertexArray *v, RefCountedPtr<Graphics::Material> m)
: Graphic(renderer)
, m_vertexArray(v)
, m_material(m)
{
}

VertexArray *TriangleGraphic::GetVertexArray() const
{
	return m_vertexArray.Get();
}

void TriangleGraphic::SetVertexArray(VertexArray *v)
{
	m_vertexArray.Reset(v);
}

void TriangleGraphic::Draw()
{
	m_renderer->SetTransform(m_transform);
	m_renderer->DrawTriangles(m_vertexArray.Get(), m_material.Get());
}

ShieldGraphic::ShieldGraphic(Renderer *r)
: Graphic(r)
, m_strength(0.f)
{
}

void ShieldGraphic::Draw()
{
	GetRenderer()->SetTransform(m_transform);
	Sfx::shieldEffect->GetMaterial()->diffuse =
		Color((1.0f - m_strength), m_strength, 0.0, 0.33f * (1.0f - m_strength));
	Sfx::shieldEffect->Draw(GetRenderer());
}

LaserGraphic::LaserGraphic(Renderer *r)
: Graphic(r)
, m_color(Color::WHITE)
, m_sideIntensity(0.f)
, m_glowIntensity(0.f)
{
}

void LaserGraphic::Draw()
{
	//Using the shared resources in Projectile
	GetRenderer()->SetTransform(m_transform);
	if (m_sideIntensity > 0.01f) {
		Projectile::s_sideMat->diffuse = m_color * m_sideIntensity;
		GetRenderer()->DrawTriangles(Projectile::s_sideVerts.Get(), Projectile::s_sideMat.Get());
	}
	if (m_glowIntensity > 0.01f) {
		Projectile::s_glowMat->diffuse = m_color * m_glowIntensity;
		GetRenderer()->DrawTriangles(Projectile::s_glowVerts.Get(), Projectile::s_glowMat.Get());
	}
}
