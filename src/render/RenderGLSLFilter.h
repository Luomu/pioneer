#ifndef _RENDERGLSLFILTER_H
#define _RENDERGLSLFILTER_H

#include "RenderFilter.h"
#include "RenderTexture.h"
#include "RenderPostProgram.h"

/* GLSL program filter */
namespace Render {

	class RenderTarget;

	/*
	 * Basic filter that operates on single texture and
	 * does not require special parameters
	 */
	class GLSLFilter : public Filter {
	public:
		GLSLFilter(const std::string &vertname, const std::string &fragname, RenderTarget *target);
		virtual ~GLSLFilter();
		virtual void Execute();
	protected:
		virtual void SetProgramParameters();
		Post::Program *m_program;
	};

	class ColorLUTFilter : public GLSLFilter {
	public:
		ColorLUTFilter(RenderTarget *t);

	protected:
		virtual void SetProgramParameters();

	private:
		Texture m_lut;
	};

}

#endif
