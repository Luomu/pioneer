#ifndef _RENDERPOSTPASS_H
#define _RENDERPOSTPASS_H

#include "libs.h"
#include "RefCounted.h"

namespace Render {

	class Texture;
	class RenderTarget;

	namespace Post {

		class Program;
		class Control;

		/*
		 * Numeric input
		 */
		class Uniform {
		public:
			void Set();
			std::string m_name;
			RefCountedPtr<Program> m_program;
			float m_value;
			GLuint m_location;
		};

		/*
		 * Texture input for a post processing pass
		 */
		class Sampler {
		public:
			void Set(int texunit);
			void Unset(int texunit);
			Texture *m_texture;
			std::string m_name;
			GLuint m_location;
			RefCountedPtr<Program> m_program;
		};

		/*
		 * Post processing pass
		 * Runs a glsl filter on a source texture (or several)
		 * Renders to a RenderTarget, or to screen in case of the
		 * final pass
		 */
		class Pass {
		public:
			Pass(Control *c, RefCountedPtr<Program> p);
			virtual ~Pass();

			virtual void Execute();
			void SetProgram(RefCountedPtr<Program> p);
			RefCountedPtr<Program> GetProgram() const { return m_program; }
			void SetTarget(RefCountedPtr<RenderTarget> t) { m_target = t; }
			RefCountedPtr<RenderTarget> GetTarget() const { return m_target; }

			// AddSampler can be used to add named samplers. However,
			// it is better to define fixed samplers in derived Pass classes
			void AddSampler(const std::string &name, Texture *tex);
			// get target texture of another pass
			void AddSampler(const std::string &name, Pass *pass);
			void AddUniform(const std::string &name, float value);

			bool renderToScreen;
		protected:
			// called after program.Use()
			virtual void SetProgramParameters();
			// clean up after rendering, if needed (mostly samplers)
			virtual void CleanUpProgramParameters();
			RefCountedPtr<Program> m_program;
			RefCountedPtr<RenderTarget> m_target;
			// texture inputs
			std::vector<Sampler> m_samplers;
			std::vector<Uniform> m_uniforms;
			Control *m_control;

			void DoQuad(float x, float y, float w, float h);
		};
	}
}

#endif
