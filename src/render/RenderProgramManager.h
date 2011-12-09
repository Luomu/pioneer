#ifndef _RENDERPROGRAMMANAGER_H
#define _RENDERPROGRAMMANAGER_H

#include "RenderPostProgram.h"
#include <map>
#include <string>

namespace Render {

	/*
	 * GLSL program "manager"
	 * Users can request programs by vtx+fragment shader name
	 * Requests are not supposed to be made each frame
	 */
	class ProgramManager {
	public:
		ProgramManager() {};
		RefCountedPtr<Post::Program> Request(const std::string &vtxName,
			const std::string &fragName);
	private:
		std::map<std::pair<std::string, std::string>, RefCountedPtr<Post::Program> > m_programs;
	};

}

#endif
