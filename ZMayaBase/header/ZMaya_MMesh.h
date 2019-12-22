//---------------//
// ZMaya_MMesh.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_MMesh_h_
#define _ZMaya_MMesh_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

/// bending angles per eash edge (0~2Pi)
void GetEdgeAngles( MObject& meshObj, MDoubleArray& edgeAngles );

// curvature per each vertex (by Taubin95 algorithm)
// which=0: Gaussian curvature
// which=1: mean curvature
// which=2: curvature intergral
void CalculateCurvature_Taubin95( MObject& meshObj, MDoubleArray& curvature, const uint which=0 );

MString TriangulateIfNot( const MString& nodeName );
void UndoTriangulate( const MString& nodeName );
MString TessellateIfNot( const MString& nodeName );
MVector CurrentNormal( const MPointArray& meshVertPositions, const MIntArray& connectedVerIds );
void GetOneRingVertIds( MItMeshVertex& meshVertItr, const int iVert, MIntArray& jVerts );
void GetOneRingVertIds( MItMeshVertex& meshVertItr, vector<MIntArray>& jVerts );
void GetOneAndTwoRingVertIds( MItMeshVertex& meshVertItr, const int iVert, MIntArray& jVerts );

// for 'calculateCurvature_Taubin95'
inline void GetTij( const MVector& N, const MPoint& Vi, const MPoint& Vj, MVector& Tij )
{
	double x = Vi.x - Vj.x;
	double y = Vi.y - Vj.y;
	double z = Vi.z - Vj.z;

	Tij.x = ( 1 - N.x*N.x ) * x  +  (   - N.x*N.y ) * y  +  (   - N.x*N.z ) * z;
	Tij.y = (   - N.y*N.x ) * x  +  ( 1 - N.y*N.y ) * y  +  (   - N.y*N.z ) * z;
	Tij.z = (   - N.z*N.x ) * x  +  (   - N.z*N.y ) * y  +  ( 1 - N.z*N.z ) * z;

	Tij.normalize();
}

// for 'calculateCurvature_Taubin95'
inline void GetKij( const MVector& N, const MPoint& Vi, const MPoint& Vj, double& Kij )
{
	double x = Vi.x - Vj.x;
	double y = Vi.y - Vj.y;
	double z = Vi.z - Vj.z;

	double length2 = x*x + y*y + z*z;

	Kij = 2 * ( N.x * x  +  N.y * y  +  N.z * z ) / length2;
}

ZELOS_NAMESPACE_END

#endif

