#pragma once

#include "berkelium/Berkelium.hpp"
#include "berkelium/Window.hpp"
#include "berkelium/WindowDelegate.hpp"

class BerkProgram;

class Berkele : public Berkelium::WindowDelegate {
public:
	Berkele() { }
	Berkele(const std::string &userdir);
	void LoadURL(const std::string &);
	// call each frame
	void Update();
	// show the texture
	void Render(int x, int y, int w, int h);
	~Berkele();
	virtual void onPaint(Berkelium::Window *wini,
			const unsigned char *bitmap_in, const Berkelium::Rect &bitmap_rect,
			size_t num_copy_rects, const Berkelium::Rect *copy_rects,
			int dx, int dy, const Berkelium::Rect &scroll_rect);
	void Mouse(int x, int y);
	void MouseButtonEvent(int button, bool state);
private:
	BerkProgram *m_prog;
	Berkelium::Window *m_window;
	char *m_scrollBuffer;
	unsigned int m_tex;
	bool m_fullRefresh;
};