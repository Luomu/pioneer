#pragma once

#include <string>
#include <vector>

namespace Render {
	class Uniform;
namespace Post {

// Recompilable shader
// Overlap with Render::Shader but it uses memset to initialize
// and I like strings
class Shader {
public:
	Shader(const std::string &vertname, const std::string &fragname);
	~Shader();
	void Bind();
	void Unbind();
	void Recompile();
	unsigned int GetProgram() const { return m_program; }
	Uniform* AddUniform(const std::string&);
protected:
	virtual void InvalidateLocations();
	unsigned int m_program;
private:
	void Compile();
	std::string m_vertName;
	std::string m_fragName;
	std::vector<Uniform*> m_uniforms;
};

} //namespace Post

class Uniform {
public:
	Uniform(const std::string&);
	void Set(float);
	void Set(int);
	// call this if shader is recompiled to make sure uniform location stays correct
	void InvalidateLocation();
private:
	Post::Shader *m_parent;
	unsigned int  m_location;
	std::string   m_name;
	friend class  Post::Shader;
	unsigned int  GetLocation();
};

} //namespace Render
