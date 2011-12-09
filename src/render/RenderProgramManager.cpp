#include "RenderProgramManager.h"

namespace Render {

RefCountedPtr<Post::Program> ProgramManager::Request(const std::string &vtxName,
	const std::string &fragName)
{
	using std::string;
	using std::pair;
	pair<string, string> p = std::make_pair(vtxName, fragName);
	std::map<pair<string, string>, RefCountedPtr<Post::Program> >::iterator i = m_programs.find(p);
	if (i != m_programs.end())
		return (*i).second;
	RefCountedPtr<Post::Program> prog(new Post::Program(vtxName, fragName));
	return prog;
}

}
