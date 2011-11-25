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
		GLSLFilter(Post::Program *prog, RenderTarget *target);
		virtual ~GLSLFilter();
		virtual void Execute();
		void SetProgram(Post::Program *prog) { m_program = prog; }
	protected:
		virtual void SetProgramParameters();
		Post::Program *m_program;
	};

	/*
	 * Color look-up table filter
	 */
	class ColorLUTFilter : public GLSLFilter {
	public:
		ColorLUTFilter(RenderTarget *t);

	protected:
		virtual void SetProgramParameters();

	private:
		Texture m_lut;
	};

	/*
	 * an example of a multi-pass filter
	 */
	class MultiBlurFilter : public Filter {
	public:
		MultiBlurFilter(RenderTarget *t);
		~MultiBlurFilter();
		virtual void Execute();
	private:
		GLSLFilter    *m_filter1;
		GLSLFilter    *m_filter2;
		Post::Program *m_program; //shared by filters
	};

}

#endif
