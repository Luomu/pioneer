#pragma once
#include "RenderShader.h"

namespace Render {

	class RenderTarget;

namespace Post {

typedef RenderTarget* FilterSource;
typedef RenderTarget* FilterTarget;

/*
 * Performs a filter operation on Source, renders to Target
 */
class Filter {
public:
	Filter(FilterSource source, FilterTarget target);
	virtual void Execute() = 0;
protected:
	void ScreenAlignedQuad();
	FilterSource m_source;
	FilterTarget m_target;
};

/*
 * Example filter:
 * Gets the texture from source and draws it to screen
 */
class Present : public Filter {
public:
	Present(FilterSource);
	void Execute();
};

/*
 * Colourize the source texture to a surprise colour
 */
class Tint : public Filter {
public:
	Tint(FilterSource, FilterTarget);
	void Execute();
};

SHADER_CLASS_BEGIN(Blursh)
	SHADER_UNIFORM_SAMPLER(tex)
SHADER_CLASS_END()

/*
 * Blurs the source texture somewhat
 */
class Blur : public Filter {
public:
	Blur(FilterSource, FilterTarget);
	void Execute();
private:
	Blursh *m_shader;
};

}
}
