#include "ScannerElement.h"
#include "Player.h"
#include "Sensor.h"
#include "Pi.h"

ScannerElement::ScannerElement(const Rocket::Core::String &tag) :
	Rocket::Core::Element(tag),
	m_angle(30.f)
{
	// create circle model
	const float circle = float(2 * M_PI);
	const float step   = float(M_PI * 0.02);  // 1/100th or 3.6 degrees
	const Rocket::Core::Vector2f c(0.f, 0.f); // center at 0,0
	const Rocket::Core::Vector2f s(1.f, 1.f); // w/h 1.0

	for (float a = 0; a < circle; a += step) {
		vector3d foo(c.x + sin(a) * s.x/2.f, 0.f, c.y + cos(a) * s.y/2.f);
		m_circle.push_back(foo.x);
		m_circle.push_back(foo.y);
		m_circle.push_back(foo.z);
	}

	//spokes from inner circle (0.1) to outer circle (1.0)
	for (float a = 0; a < circle; a += float(M_PI * 0.25)) {
		//AddSpoke(a);
		float rad = 0.1f;
		m_spokes.push_back(sin(a) * rad);
		m_spokes.push_back(0.f);
		m_spokes.push_back(cos(a) * rad);

		rad = 1.f;
		m_spokes.push_back(sin(a) * rad);
		m_spokes.push_back(0.f);
		m_spokes.push_back(cos(a) * rad);
	}
}

ScannerElement::~ScannerElement()
{
}

bool ScannerElement::GetIntrinsicDimensions(Rocket::Core::Vector2f &dimensions)
{
	dimensions.x = 100.f;
	dimensions.y = 100.f;
	return true;
}

void ScannerElement::AddCircle(const Rocket::Core::Vector2f &center,
							   const Rocket::Core::Vector2f &size)
{
	glPushMatrix();

	glTranslatef(center.x, center.y, 0.f);
	glScalef(size.x, size.y, 1.f);
	glRotatef(m_angle, 1.f, 0.f, 0.f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &m_circle[0]);
	glDrawArrays(GL_LINE_LOOP, 0, m_circle.size()/3);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void ScannerElement::AddBlob(const vector3d &pos)
{
	//immediate mode bleargh
	glLineWidth(2.f);
	glPointSize(6.f);

	glBegin(GL_LINES);
	glVertex3f(pos.x, 0.f, pos.z); //tail
	glVertex3f(pos.x, pos.y, pos.z);
	glEnd();
	glBegin(GL_POINTS);
	glVertex3f(pos.x, pos.y, pos.z);
	glEnd();

	glLineWidth(1.f);
	glPointSize(1.f);
}

void ScannerElement::AddBlobs(const Rocket::Core::Vector2f &center,
							  const Rocket::Core::Vector2f &size)
{
	glPushMatrix();
	glTranslatef(center.x, center.y, 0.f);
	glScalef(size.x/2.f, -size.y/2.f, -1.f/2.f);
	glRotatef(m_angle, 1.f, 0.f, 0.f);

	const double maxrange = 10000.0;
	const double scale = 1.0/maxrange;
	if (Pi::player) {
		matrix4x4d rot;
		Pi::player->GetRotMatrix(rot);
		Sensor::ContactList &clist = Pi::player->GetSensor()->GetContacts();
		for (Sensor::ContactIterator i = clist.begin(); i != clist.end(); ++i) {
			vector3d pos = i->GetBody()->GetPositionRelTo(Pi::player);
			if (pos.Length() > maxrange) continue;
			AddBlob(scale * rot.InverseOf() * pos);
		}
	}

	glPopMatrix();
}

void ScannerElement::AddSpokes()
{
	glColor3f(1.f, 1.f, 1.f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &m_spokes[0]);
	glDrawArrays(GL_LINES, 0, m_spokes.size()/3);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ScannerElement::OnRender()
{
	const Rocket::Core::Vector2f top(GetAbsoluteLeft(), GetAbsoluteTop());
	const Rocket::Core::Vector2f size(GetClientWidth(),
									  GetClientHeight());
	const Rocket::Core::Vector2f center(top.x + GetClientWidth()/2.f,
										top.y + GetClientHeight()/2.f);
	{
		//glDisable(GL_DEPTH_TEST);
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, 1024, 768, 0, 0, 10000); //bad
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.f, 0.f, -10.f);
		glColor3f(0.f, 1.f, 0.f);
		glLineWidth(1.f);
		AddCircle(center, size);
		AddCircle(center, size * 0.75f);
		AddCircle(center, size * 0.5f);
		AddCircle(center, size * 0.1f);
		AddBlobs(center, size);
	}

	glPushMatrix();
	glTranslatef(center.x, center.y, 0.f);
	glScalef(size.x/2.f, size.y/2.f, 1.f/2.f);
	glRotatef(m_angle, 1.f, 0.f, 0.f);

	AddSpokes();

	glPopMatrix();
}

void ScannerElement::OnAttributeChange(const Rocket::Core::AttributeNameList &changedAttributes)
{
	m_angle = GetAttribute<float>("angle", 0.f);
}