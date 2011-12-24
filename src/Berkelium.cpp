#include "Berkelium.h"
#include "berkelium/Context.hpp"
#include "berkelium/WindowDelegate.hpp"

struct BDelegate : public Berkelium::WindowDelegate {
	virtual void onPaint(Berkelium::Window *wini,
        const unsigned char *bitmap_in, const Berkelium::Rect &bitmap_rect,
        size_t num_copy_rects, const Berkelium::Rect *copy_rects,
        int dx, int dy, const Berkelium::Rect &scroll_rect)
	{

	}
};

Berkele::Berkele(const std::string &)
{
	if (!Berkelium::init(Berkelium::FileString::empty()))
		throw "lo, disaster";

	Berkelium::Context *ctx = Berkelium::Context::create();
	m_window = Berkelium::Window::create(ctx);

	m_window->setDelegate(new BDelegate());
	m_window->resize(1024, 768);
	LoadURL("http://www.google.com");
	delete ctx;
}

Berkele::~Berkele()
{
	Berkelium::destroy();
}

void Berkele::LoadURL(const std::string &url)
{
	m_window->navigateTo(url.data(), url.length());
}

void Berkele::Update()
{
	Berkelium::update();
}

void Berkele::Render(int x, int y, int w, int h)
{

}