#ifndef _SCANNERELEMENT_H
#define _SCANNERELEMENT_H

#include "ui/UIManager.h"
#include <vector>

/*
 * Classic scanner, rocket version
 * Default projection angle is 30
 * 0 is from side and 90 is top down
 * <scanner angle=90 />
 */
class ScannerElement : public Rocket::Core::Element {
public:
	ScannerElement(const Rocket::Core::String &tag);
	virtual ~ScannerElement();

	virtual bool GetIntrinsicDimensions(Rocket::Core::Vector2f &dimensions);
	virtual void OnRender();
	virtual void OnAttributeChange(const Rocket::Core::AttributeNameList &changedAttributes);
private:
	Rocket::Core::Vector2f m_rad;
	Rocket::Core::Vector2f m_center;
	void AddCircle(const Rocket::Core::Vector2f &center,
				   const Rocket::Core::Vector2f &size);
	void AddBlobs(const Rocket::Core::Vector2f &center,
				  const Rocket::Core::Vector2f &size);
	void AddSpokes();
	void AddBlob(const vector3d &pos);
	std::vector<float> m_circle;
	std::vector<float> m_spokes;
	float m_angle;
};

#endif
