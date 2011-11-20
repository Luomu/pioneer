#ifndef _RENDERGLSLFILTER_H
#define _RENDERGLSLFILTER_H

#include "RenderFilter.h"

/* GLSL program filter */
namespace Render {

	class RenderTarget;
	class GLSLFilterProgram;

	class GLSLFilter : public Filter {
	public:
		GLSLFilter(const std::string &progName, RenderTarget *target);
		virtual ~GLSLFilter();
		virtual void Execute();
	private:
		GLSLFilterProgram *m_program;
	};
}

#endif
