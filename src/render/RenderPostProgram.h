#ifndef _RENDERPOSTPROGRAM_H
#define _RENDERPOSTPROGRAM_H

#include "libs.h"
#include "RefCounted.h"

namespace Render {
	namespace Post {
		/*
		 * GLSL program
		 *
		 * Slightly different from RenderShader: vtx shader and fragment shader
		 * can be specified separately, since vtx shader is often the same
		 * (full screen quad)
		 */
		class Program : public RefCounted {
		public:
			Program();
			Program(const std::string &vtxName, const std::string &fragName);
			virtual ~Program();
			//file name will be formatted to PIONEER_DATA_DIR + /shaders/ + filename
			void Load(const std::string &vtxName, const std::string &fragName);
			void Use();
			void Unuse();

			//Uniform stuff
			//these are not for performance but speedy testing
			inline GLuint GetLocation(const char *name);
			void SetUniform1i(const char *name, int value);
			void SetUniform1f(const char *name, float value);
			void SetUniform2f(const char *name, float, float);

		protected:
			GLuint m_program;
		};

		class Shader {
		public:
			Shader(const std::string &fileName, GLenum type);
			~Shader();
			GLuint GetShader() const { return m_shader; }

		private:
			GLuint m_shader;
			void SetSource(const std::string &sourceString);
			std::string LoadSource(const std::string &fileName);
		};
	}
}

#endif