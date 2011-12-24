#include "Berkelium.h"
#include "berkelium/Context.hpp"
#include "render/RenderShader.h"

SHADER_CLASS_BEGIN(BerkProgram)
	SHADER_UNIFORM_SAMPLER(texture0)
SHADER_CLASS_END()

Berkele::Berkele(const std::string &)
{
	if (!Berkelium::init(Berkelium::FileString::empty()))
		throw "lo, disaster";

	Berkelium::Context *ctx = Berkelium::Context::create();
	m_window = Berkelium::Window::create(ctx);

	glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	int width = 1024;
	int height = 768;
	m_scrollBuffer = new char[width*(height+1)*4];

	m_window->setDelegate(this);
	m_window->resize(width, height);
	m_window->setTransparent(true);
	LoadURL("http://www.google.com");
	delete ctx;

	m_prog = new BerkProgram("berk");
}

Berkele::~Berkele()
{
	glDeleteTextures(1, &m_tex);
	delete m_window;
	delete m_scrollBuffer;
	delete m_prog;
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

void _DoQuad(const float x, const float y, const float w, const float h)
{
	//quad
	const GLfloat vertices[] = {
		x,     y,     0.f,
		x + w, y,     0.f,
		x + w, y + h, 0.f,
		x,     y + h, 0.f
	};
	const GLfloat texcoords[] = {
		0.f, 0.f,
		1.f, 0.f,
		1.f, 1.f,
		0.f, 1.f,
	};
	glColor3f(1.f, 0.f, 1.f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Berkele::Render(int, int, int, int)
{
	const float x = 0.f;
	const float y = 0.f;
	const float w = 1.f;
	const float h = 1.f;
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.f, w , 0.f, h, 0.1f, 2.f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.f, 0.f, -1.f);

	Render::State::UseProgram(m_prog);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	m_prog->set_texture0(0);
	_DoQuad(x, y, w, h);
	glBindTexture(GL_TEXTURE_2D, 0);
	Render::State::UseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

bool mapOnPaintToTexture(
    Berkelium::Window *wini,
    const unsigned char* bitmap_in, const Berkelium::Rect& bitmap_rect,
    size_t num_copy_rects, const Berkelium::Rect *copy_rects,
    int dx, int dy,
    const Berkelium::Rect& scroll_rect,
    unsigned int dest_texture,
    unsigned int dest_texture_width,
    unsigned int dest_texture_height,
    bool ignore_partial,
    char* scroll_buffer) {

    glBindTexture(GL_TEXTURE_2D, dest_texture);

    const int kBytesPerPixel = 4;

    // If we've reloaded the page and need a full update, ignore updates
    // until a full one comes in.  This handles out of date updates due to
    // delays in event processing.
    if (ignore_partial) {
        if (bitmap_rect.left() != 0 ||
            bitmap_rect.top() != 0 ||
            bitmap_rect.right() != dest_texture_width ||
            bitmap_rect.bottom() != dest_texture_height) {
            return false;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, kBytesPerPixel, dest_texture_width, dest_texture_height, 0,
            GL_BGRA, GL_UNSIGNED_BYTE, bitmap_in);
        ignore_partial = false;
        return true;
    }

    // Now, we first handle scrolling. We need to do this first since it
    // requires shifting existing data, some of which will be overwritten by
    // the regular dirty rect update.
    if (dx != 0 || dy != 0) {
        // scroll_rect contains the Rect we need to move
        // First we figure out where the the data is moved to by translating it
        Berkelium::Rect scrolled_rect = scroll_rect.translate(-dx, -dy);
        // Next we figure out where they intersect, giving the scrolled
        // region
        Berkelium::Rect scrolled_shared_rect = scroll_rect.intersect(scrolled_rect);
        // Only do scrolling if they have non-zero intersection
        if (scrolled_shared_rect.width() > 0 && scrolled_shared_rect.height() > 0) {
            // And the scroll is performed by moving shared_rect by (dx,dy)
            Berkelium::Rect shared_rect = scrolled_shared_rect.translate(dx, dy);

            int wid = scrolled_shared_rect.width();
            int hig = scrolled_shared_rect.height();

            int inc = 1;
            char *outputBuffer = scroll_buffer;
            // source data is offset by 1 line to prevent memcpy aliasing
            // In this case, it can happen if dy==0 and dx!=0.
            char *inputBuffer = scroll_buffer+(dest_texture_width*1*kBytesPerPixel);
            int jj = 0;
            if (dy > 0) {
                // Here, we need to shift the buffer around so that we start in the
                // extra row at the end, and then copy in reverse so that we
                // don't clobber source data before copying it.
                outputBuffer = scroll_buffer+(
                    (scrolled_shared_rect.top()+hig+1)*dest_texture_width
                    - hig*wid)*kBytesPerPixel;
                inputBuffer = scroll_buffer;
                inc = -1;
                jj = hig-1;
            }

            // Copy the data out of the texture
            glGetTexImage(
                GL_TEXTURE_2D, 0,
                GL_BGRA, GL_UNSIGNED_BYTE,
                inputBuffer
            );

            // Annoyingly, OpenGL doesn't provide convenient primitives, so
            // we manually copy out the region to the beginning of the
            // buffer
            for(; jj < hig && jj >= 0; jj+=inc) {
                memcpy(
                    outputBuffer + (jj*wid) * kBytesPerPixel,
//scroll_buffer + (jj*wid * kBytesPerPixel),
                    inputBuffer + (
                        (scrolled_shared_rect.top()+jj)*dest_texture_width
                        + scrolled_shared_rect.left()) * kBytesPerPixel,
                    wid*kBytesPerPixel
                );
            }

            // And finally, we push it back into the texture in the right
            // location
            glTexSubImage2D(GL_TEXTURE_2D, 0,
                shared_rect.left(), shared_rect.top(),
                shared_rect.width(), shared_rect.height(),
                GL_BGRA, GL_UNSIGNED_BYTE, outputBuffer
            );
        }
    }

    for (size_t i = 0; i < num_copy_rects; i++) {
        int wid = copy_rects[i].width();
        int hig = copy_rects[i].height();
        int top = copy_rects[i].top() - bitmap_rect.top();
        int left = copy_rects[i].left() - bitmap_rect.left();

        for(int jj = 0; jj < hig; jj++) {
            memcpy(
                scroll_buffer + jj*wid*kBytesPerPixel,
                bitmap_in + (left + (jj+top)*bitmap_rect.width())*kBytesPerPixel,
                wid*kBytesPerPixel
                );
        }

        // Finally, we perform the main update, just copying the rect that is
        // marked as dirty but not from scrolled data.
        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        copy_rects[i].left(), copy_rects[i].top(),
                        wid, hig,
                        GL_BGRA, GL_UNSIGNED_BYTE, scroll_buffer
            );
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void Berkele::onPaint(Berkelium::Window *wini,
        const unsigned char *bitmap_in, const Berkelium::Rect &bitmap_rect,
        size_t num_copy_rects, const Berkelium::Rect *copy_rects,
        int dx, int dy, const Berkelium::Rect &scroll_rect)
{
	int width = 1024;
	int height = 768;
	bool needs_full_refresh = false;

	bool updated = mapOnPaintToTexture(
		wini, bitmap_in, bitmap_rect, num_copy_rects, copy_rects,
		dx, dy, scroll_rect,
		m_tex, width, height, needs_full_refresh, m_scrollBuffer
	);
	printf("Updated: %d\n", updated);
}