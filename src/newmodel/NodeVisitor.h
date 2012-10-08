#ifndef _NODEVISITOR_H
#define _NODEVISITOR_H
/*
 * Node visitor.
 * Start traversal with node->Accept(visitor)!
 */
namespace Newmodel {

class Billboard;
class Group;
class Label3D;
class LOD;
class MatrixTransform;
class Node;
class StaticGeometry;
class Thruster;

class NodeVisitor
{
public:
	virtual void ApplyNode(Node&);
	virtual void ApplyGroup(Group&);
	virtual void ApplyStaticGeometry(StaticGeometry&);
	virtual void ApplyLabel(Label3D&);
	virtual void ApplyMatrixTransform(MatrixTransform&);
	virtual void ApplyBillboard(Billboard&);
	virtual void ApplyThruster(Thruster&);
	virtual void ApplyLOD(LOD&);
};

}
#endif