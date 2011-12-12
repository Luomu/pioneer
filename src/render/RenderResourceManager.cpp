#include "RenderResourceManager.h"
#include "Render.h"

namespace Render {

RefCountedPtr<Post::Program> ResourceManager::RequestProgram(const std::string &vtxName,
	const std::string &fragName)
{
	using std::string;
	using std::pair;
	pair<string, string> p = std::make_pair(vtxName, fragName);

	// check for existing
	std::map<pair<string, string>, RefCountedPtr<Post::Program> >::iterator i = m_programs.find(p);
	if (i != m_programs.end())
		return (*i).second;

	// create new
	try {
		RefCountedPtr<Post::Program> prog(new Post::Program(vtxName, fragName));
		m_programs.insert(std::make_pair(p, prog));
		return prog;
	} catch (Render::Exception &ex) {
		// notify and try to load fallback shader
		// XXX printf is enough for now
		printf("%s\n", ex.what());
		RefCountedPtr<Post::Program> prog(new Post::Program("debug.vert", "debug.frag"));
		return prog;
	}
}

}
