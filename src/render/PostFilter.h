#pragma once

namespace Render {

	class RenderTarget;

namespace Post {

typedef RenderTarget* FilterSource;

/*
 * Performs a filter operation on Source, renders to Target
 */
class Filter {
public:
	Filter(FilterSource source);
	virtual void Execute() = 0;
protected:
	FilterSource m_source;
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

}
}
