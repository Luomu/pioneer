#pragma once

#include "berkelium/Berkelium.hpp"
#include "berkelium/Window.hpp"

class Berkele {
public:
	Berkele() { }
	Berkele(const std::string &userdir);
	void LoadURL(const std::string &);
	// call each frame
	void Update();
	// show the texture
	void Render(int x, int y, int w, int h);
	~Berkele();
private:
	Berkelium::Window *m_window;
};