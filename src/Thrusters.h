// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _THRUSTERS_H
#define _THRUSTERS_H
/*
 * Spaceship thrusters
 */
#include "libs.h"
#include "Serializer.h"

namespace Graphics { class Renderer; }
namespace SceneGraph { class Model; class Billboard; class Thruster; }

class Thrusters /* : public Subsystem */ {
public:
	struct Port {
		vector3f pos;
		vector3f dir;
		float scale;
		bool linearOnly;
		SceneGraph::Thruster *graphic;
	};

	Thrusters(SceneGraph::Model*);
	virtual ~Thrusters();
	virtual void Save(Serializer::Writer &wr);
	virtual void Load(Serializer::Reader &rd);

	virtual void Update(float time);

	void SetLinear(unsigned int axis, double level);
	void SetLinear(const vector3d&);
	void SetAngular(unsigned int axis, double level);
	void SetAngular(const vector3d&);
	const vector3d &GetLinear() const { return m_linThrust; }
	const vector3d &GetAngular() const { return m_angThrust; }

	static void Init(Graphics::Renderer*);
	static void Uninit();

protected:
	std::vector<Port> m_ports;

	vector3d m_linThrust;
	vector3d m_angThrust;
};

#endif
