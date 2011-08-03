#pragma once

#include <string>

namespace Render {
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
protected:
	virtual void InvalidateLocations();
	unsigned int m_program;
private:
	void Compile();
	std::string m_vertName;
	std::string m_fragName;
};

}
}