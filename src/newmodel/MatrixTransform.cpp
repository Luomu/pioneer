#include "MatrixTransform.h"
#include "graphics/Renderer.h"
namespace Newmodel {

MatrixTransform::MatrixTransform(const matrix4x4f &m)
: Group()
, m_transform(m)
{
}

void MatrixTransform::Render(Graphics::Renderer *renderer, const matrix4x4f &trans, RenderData *rd)
{
	const matrix4x4f t = trans * m_transform;
	//renderer->SetTransform(t);
	RenderChildren(renderer, t, rd);
}

}