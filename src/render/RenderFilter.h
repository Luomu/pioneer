#ifndef _RENDERFILTER_H
#define _RENDERFILTER_H

#include <vector>

namespace Render {

	class Texture;

	/* Source - sink terminology is from here:
	 * http://http.developer.nvidia.com/GPUGems/gpugems_ch27.html
	 */

	/* Source operation, e.g. acquire colour texture
	 * In fact, always operates on textures, what else
	 */
	class Source {
	public:
		Source() { }
		virtual ~Source() { }
		void AddTexture(Texture *t) {
			m_textures.push_back(t);
		}

	protected:
		std::vector<Texture*> m_textures;
	};

	/* Sink operation, e.g. render to screen */
	class Sink {
	public:
		Sink() : m_source(0) { }
		virtual ~Sink() {}

		virtual void SetSource(Source *s) { m_source = s; }
		virtual void Execute() = 0;

	protected:
		Source *m_source;
	};

	/* Filter operator */
	class Filter : public Sink, public Source {
	public:
		Filter() { }
		virtual ~Filter() { }
	};
}

#endif