// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Thrusters.h"
#include "graphics/Renderer.h"
#include "graphics/Material.h"
#include "graphics/TextureBuilder.h"
#include "scenegraph/SceneGraph.h"

static bool g_initted = false;

static Color baseColor(0.7f, 0.6f, 1.f, 1.f);

static RefCountedPtr<Graphics::Material> matWhite;

void Thrusters::Init(Graphics::Renderer *renderer)
{
	assert(!g_initted);

	Graphics::MaterialDescriptor desc;
	desc.textures = 1;

	//todo: use one texture to store both glow and flame for each colour
	matWhite.Reset(renderer->CreateMaterial(desc));
	matWhite->texture0 = Graphics::TextureBuilder::Billboard("textures/halo.png").CreateTexture(renderer);

	g_initted = true;
}

void Thrusters::Uninit()
{
	assert(g_initted);

	delete matWhite->texture0;
	matWhite.Reset();

	g_initted = false;
}

Thrusters::Thrusters(SceneGraph::Model *model)
: m_linThrust(0.0)
, m_angThrust(0.0)
{
	using SceneGraph::Node;
	using SceneGraph::MatrixTransform;

	Graphics::Renderer *renderer = model->GetRenderer();

	//Find thruster positions from the model
	//transforms are pre-accumulated to make positions and directions relative
	//to model center
	SceneGraph::FindNodeVisitor finder(SceneGraph::FindNodeVisitor::MATCH_NAME_STARTSWITH, "thruster_");
	model->GetRoot()->Accept(finder);
	const std::vector<Node*> &results = finder.GetResults();

	for (unsigned int i = 0; i < results.size(); ++i) {
		MatrixTransform *mt = dynamic_cast<MatrixTransform*>(results.at(i));
		assert(mt);
		const bool linear = starts_with(mt->GetName(), "thruster_linear");

		matrix4x4f transform = mt->GetTransform();
		vector3f pos = transform.GetTranslate();
		transform.ClearToRotOnly();

		m_ports.push_back(Port());
		Port &port = m_ports.back();
		port.linearOnly = linear;
		port.dir = (transform * vector3f(0.f, 0.f, 1.f)).Normalized();

		port.graphic = new SceneGraph::Thruster(renderer);
		mt->AddChild(port.graphic);
		mt->SetNodeMask(SceneGraph::NODE_TRANSPARENT);
	}
}

Thrusters::~Thrusters()
{
}

void Thrusters::Save(Serializer::Writer &wr)
{
	wr.Vector3d(m_linThrust);
	wr.Vector3d(m_angThrust);
}

void Thrusters::Load(Serializer::Reader &rd)
{
	m_linThrust = rd.Vector3d();
	m_angThrust = rd.Vector3d();
}

void Thrusters::Update(float)
{
	std::vector<Port>::iterator it;
	for (it = m_ports.begin(); it != m_ports.end(); ++it) {
		float power = 0.f;
		power = -it->dir.Dot(vector3f(m_linThrust));

		if (!it->linearOnly) {
			// pitch X
			// yaw   Y
			// roll  Z
			const vector3f at = vector3f(m_angThrust);
			const vector3f angdir = it->pos.Cross(it->dir);

			const float xp = angdir.x * at.x;
			const float yp = angdir.y * at.y;
			const float zp = angdir.z * at.z;

			if (xp+yp+zp > 0) {
				if (xp > yp && xp > zp && fabs(at.x) > power) power = fabs(at.x);
				else if (yp > xp && yp > zp && fabs(at.y) > power) power = fabs(at.y);
				else if (zp > xp && zp > yp && fabs(at.z) > power) power = fabs(at.z);
			}
		}

		it->graphic->SetIntensity(power);
	}
}

void Thrusters::SetLinear(unsigned int axis, double level)
{
	m_linThrust[axis] = Clamp(level, -1.0, 1.0);
}

void Thrusters::SetLinear(const vector3d &v)
{
	m_linThrust.x = Clamp(v.x, -1.0, 1.0);
	m_linThrust.y = Clamp(v.y, -1.0, 1.0);
	m_linThrust.z = Clamp(v.z, -1.0, 1.0);
}

void Thrusters::SetAngular(unsigned int axis, double level)
{
	m_angThrust[axis] = Clamp(level, -1.0, 1.0);
}

void Thrusters::SetAngular(const vector3d &v)
{
	m_angThrust.x = Clamp(v.x, -1.0, 1.0);
	m_angThrust.y = Clamp(v.y, -1.0, 1.0);
	m_angThrust.z = Clamp(v.z, -1.0, 1.0);
}
