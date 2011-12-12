#ifndef _RENDERRESOURCEMANAGER_H
#define _RENDERRESOURCEMANAGER_H

#include "RenderPostProgram.h"
#include <map>
#include <string>

namespace Render {

	/*
	 * Rendertarget & GLSL program "manager"
	 * Users can request programs by vtx+fragment shader name
	 * Rendertargets by dimensions/etc.
	 * Requests are not supposed to be made each frame
	 */
	class ResourceManager {
	public:
		ResourceManager() {};
		RefCountedPtr<Post::Program> RequestProgram(const std::string &vtxName,
			const std::string &fragName);
	private:
		std::map<std::pair<std::string, std::string>, RefCountedPtr<Post::Program> > m_programs;
	};

}

#endif
