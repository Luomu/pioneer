// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _GRAPHIC_H
#define _GRAPHIC_H

#include "libs.h"
#include "graphics/Renderer.h"

// A Graphic is a combination of a transform and something renderable
// (vertex array + material, StaticMesh)
// A graphic is not shared, but the renderables may be
//
// Goal: use the generic Graphic types when you can,
// implement custom ones when you have to

class Graphic {
public:
	virtual ~Graphic() { }

	Graphics::Renderer *GetRenderer() const { return m_renderer; }

	void SetTransform(const matrix4x4f &transform) { m_transform = transform; }
	const matrix4x4f &GetTransform() const { return m_transform; }

	virtual void Draw() = 0;

protected:
	Graphic(Graphics::Renderer *r) : m_renderer(r), m_transform(matrix4x4f::Identity()) {}

	Graphics::Renderer *m_renderer;
	matrix4x4f m_transform;
};

// uses either a StaticMesh or a Surface
class RenderableGraphic : public Graphic {
public:
	RenderableGraphic(Graphics::Renderer*, Graphics::Renderable*);
	virtual void Draw();

private:
	Graphics::Renderable *m_renderable;
};

//takes ownership of vertex array
class TriangleGraphic : public Graphic {
public:
	TriangleGraphic(Graphics::Renderer*, Graphics::VertexArray*,
		RefCountedPtr<Graphics::Material>, Graphics::PrimitiveType = Graphics::TRIANGLES);

	RefCountedPtr<Graphics::Material> &GetMaterial() { return m_material; }
	void SetMaterial(RefCountedPtr<Graphics::Material> m) { m_material = m; }

	Graphics::VertexArray *GetVertexArray() const;
	void SetVertexArray(Graphics::VertexArray*);

	virtual void Draw();

private:
	ScopedPtr<Graphics::VertexArray> m_vertexArray;
	RefCountedPtr<Graphics::Material> m_material;
	Graphics::PrimitiveType m_primitiveType;
};

class ShieldGraphic : public Graphic {
public:
	ShieldGraphic(Graphics::Renderer *r);
	virtual void Draw();

	void SetStrength(float s) { m_strength = s; }

private:
	float m_strength;
};

class LaserGraphic : public Graphic {
public:
	LaserGraphic(Graphics::Renderer *r);

	void SetColor(const Color &c) { m_color = c; }
	void SetSideIntensity(float i) { m_sideIntensity = i; }
	void SetGlowIntensity(float i) { m_glowIntensity = i; }

	virtual void Draw();

private:
	Color m_color;
	float m_sideIntensity;
	float m_glowIntensity;
};

#endif
