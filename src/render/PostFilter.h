#pragma once

#include <string>

namespace Render {

	class RenderTarget;

namespace Post {

typedef RenderTarget* FilterSource;
typedef RenderTarget* FilterTarget;

class Shader;
class Blursh;

/*
 * Performs a filter operation on Source, renders to Target
 */
class Filter {
public:
	Filter(FilterSource source, FilterTarget target);
	virtual void Execute() = 0;
	virtual void Reload() { } //for reloading shaders - might not be final
protected:
	virtual void ScreenAlignedQuad();
	FilterSource m_source;
	FilterTarget m_target;
};

class ShaderFilter : public Filter {
public:
	ShaderFilter(FilterSource source, FilterTarget target,
		const std::string &vert, const std::string &frag);
	~ShaderFilter();
	virtual void Execute();
	void Reload();
protected:
	virtual void SetUniforms();
	Shader *m_shader;
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
