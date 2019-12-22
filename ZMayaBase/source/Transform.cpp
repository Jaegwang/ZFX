//---------------//
// Transform.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.01.27                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

// dag node object -> world matrix
bool
GetWorldMatrix( const MObject& dagNodeObj, MMatrix& worldMat )
{
	MStatus status = MS::kSuccess;

	MFnDagNode dagFn( dagNodeObj, &status );
	if( status != MS::kSuccess ) { return false; } // Don't print any error messages here!

	MDagPath dagPath;
	status = dagFn.getPath( dagPath );
	if( status != MS::kSuccess ) { return false; } // Don't print any error messages here!

	worldMat = dagPath.inclusiveMatrix();

	return true;
}

// p -> p (p=M*p)
void
ApplyXForm( MPoint& p, const MMatrix& M )
{
	const double (*matrix)[4] = M.matrix;
	const double x=p.x, y=p.y, z=p.z;

	p.x = matrix[0][0]*x + matrix[1][0]*y + matrix[2][0]*z + matrix[3][0];
	p.y = matrix[0][1]*x + matrix[1][1]*y + matrix[2][1]*z + matrix[3][1];
	p.z = matrix[0][2]*x + matrix[1][2]*y + matrix[2][2]*z + matrix[3][2];
}

// p -> q (q=M*p)
void
ApplyXForm( const MMatrix& M, const MPoint& p, MPoint& q )
{
	const double (*matrix)[4] = M.matrix;
	const double &x=p.x, &y=p.y, &z=p.z;

	q.x = matrix[0][0]*x + matrix[1][0]*y + matrix[2][0]*z + matrix[3][0];
	q.y = matrix[0][1]*x + matrix[1][1]*y + matrix[2][1]*z + matrix[3][1];
	q.z = matrix[0][2]*x + matrix[1][2]*y + matrix[2][2]*z + matrix[3][2];
}

ZELOS_NAMESPACE_END

