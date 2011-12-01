#ifndef _RENDERPOSTPASS_H
#define _RENDERPOSTPASS_H

namespace Render {

	class Texture;
	class RenderTarget;

	namespace Post {

		class Program;

		/*
		 * Texture input for a post processing pass
		 */
		class Sampler {
		private:
			Texture *m_texture;
		};

		/*
		 * Post processing pass
		 * Runs a glsl filter on a source texture (or several)
		 * Renders to a RenderTarget, or to screen in case of the
		 * final pass
		 */
		class Pass {
		public:
			Pass(Program *p);
			virtual ~Pass();

			virtual void Execute();
			void SetProgram(Program *p);
			Program* GetProgram() { return m_program; }
			void SetTarget(RenderTarget *t) { m_target = t; }
			RenderTarget* GetTarget() const { return m_target; }

			bool renderToScreen;
		protected:
			// called after program.Use()
			virtual void SetProgramParameters();
			// clean up after rendering, if needed
			virtual void CleanUpProgramParameters();
			Program *m_program;
			RenderTarget *m_target;
			//inputs

			void DoQuad(float x, float y, float w, float h);
		};
	}
}

#endif
