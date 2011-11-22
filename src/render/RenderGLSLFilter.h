#ifndef _RENDERGLSLFILTER_H
#define _RENDERGLSLFILTER_H

#include "RenderFilter.h"
#include "RenderTexture.h"

/* GLSL program filter */
namespace Render {

	class RenderTarget;
	class GLSLFilterProgram;

	/*
	 * Basic filter that operates on single texture and
	 * does not require special parameters
	 */
	class GLSLFilter : public Filter {
	public:
		GLSLFilter(const std::string &progName, RenderTarget *target);
		virtual ~GLSLFilter();
		virtual void Execute();
	protected:
		virtual void SetProgramParameters();
		GLSLFilterProgram *m_program;
	};

	class ColorLUTFilter : public GLSLFilter {
	public:
		ColorLUTFilter(const std::string &progName, RenderTarget *t);

	protected:
		virtual void SetProgramParameters();

	private:
		Texture m_lut;
	};

}

#endif
