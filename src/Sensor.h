#ifndef _SENSOR_H
#define _SENSOR_H

#include <vector>

/*
 * Radar logic & contact handling
 */

class Body;

class Contact {
public:
	Contact(Body *b);
	virtual ~Contact();

	Body *GetBody() const { return m_body; }

private:
	Body *m_body;
};

class Sensor {
public:
	Sensor();
	virtual ~Sensor();

	std::vector<Contact> &GetContacts() { return m_contacts; }

	void Update(float timeStep);

	typedef std::vector<Contact>::iterator ContactIterator;

private:
	void CollectContacts();
	std::vector<Contact> m_contacts;
	float m_range;
};

#endif
