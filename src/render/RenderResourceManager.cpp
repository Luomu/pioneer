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

RefCountedPtr<RenderTarget> ResourceManager::RequestRenderTarget(int width, int height,
	const TextureFormat &tf, bool mipmaps)
{
	// TODO: of course this idea did not work as intended. A texture might be required
	// a few steps down the chain so it's not good to overwrite it!
	// check for existing
	/*std::vector<RefCountedPtr<RenderTarget> >::iterator i;
	for(i = m_renderTargets.begin(); i != m_renderTargets.end(); ++i) {
		bool match = true;
		RenderTarget *t = i->Get();
		if (t->Width()  != width) match  = false;
		if (t->Height() != height) match = false;

		if (match)
			return *i;
	}*/
	RefCountedPtr<RenderTarget> targ(new RenderTarget(width, height, tf.format, tf.internalFormat, tf.type));
	//m_renderTargets.push_back(targ);
	return targ;
}

RefCountedPtr<RenderTarget> ResourceManager::RequestRenderTarget(int width, int height)
{
	return RequestRenderTarget(width, height, TextureFormat(GL_RGB, GL_RGB8, GL_UNSIGNED_BYTE));
}

}
