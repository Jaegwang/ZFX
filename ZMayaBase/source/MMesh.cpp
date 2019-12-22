//-----------//
// MMesh.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

void
GetEdgeAngles( MObject& meshObj, MDoubleArray& edgeAngles )
{
	edgeAngles.clear();

	MItMeshVertex  vItr( meshObj );
	MItMeshEdge    eItr( meshObj );
	MItMeshPolygon fItr( meshObj );

	MPointArray vPos;
	for( vItr.reset(); !vItr.isDone(); vItr.next() )
	{
		vPos.append( vItr.position( MSpace::kWorld ) );
	}

	int iEdge = 0;
	for( eItr.reset(); !eItr.isDone(); eItr.next(), ++iEdge ) // per each edge
	{
		if( eItr.onBoundary() )
		{
			edgeAngles.append( Z_PI );
			continue;
		}

		MIntArray faceList;
		eItr.getConnectedFaces( faceList );

		int e0 = eItr.index(0);
		int e1 = eItr.index(1);

		MPoint cPos = eItr.center();

		int faceCount = (int)faceList.length();
		if( faceCount != 2 )
		{
			MGlobal::displayWarning( MString( "Not manifold" ) );
			edgeAngles.append( Z_PI );
			continue;
		}

		int f0 = faceList[0];
		int f1 = faceList[1];

		int v0 = -1, v1 = -1;
		int dummy = 0;
		MVector n0, n1;
		MIntArray vertices;

		fItr.setIndex( f0, dummy );
		fItr.getVertices( vertices );

		fItr.getNormal( n0, MSpace::kWorld );

		for( int j=0; j<(int)vertices.length(); ++j )
		{
			if( ( vertices[j] != e0 ) && ( vertices[j] != e1 ) ) { v0 = vertices[j]; }
		}

		vertices.clear();

		fItr.setIndex( f1, dummy );
		fItr.getVertices( vertices );

		fItr.getNormal( n1, MSpace::kWorld );

		for( int j=0; j<(int)vertices.length(); ++j )
		{
			if( ( vertices[j] != e0 ) && ( vertices[j] != e1 ) ) { v1 = vertices[j]; }
		}

		MVector N = n0 + n1;
		N.normalize();

		MVector V0 = vPos[v0] - cPos;
		MVector V1 = vPos[v1] - cPos;

		V0.normalize();
		V1.normalize();

		double angle0 = acos( V0 * N );
		double angle1 = acos( V1 * N );

		#ifdef OS_WINDOWS
		 if( std::_isnan(angle0) ) { angle0 = 0; }
		 if( std::_isnan(angle1) ) { angle1 = 0; }
		#endif

		#ifdef OS_LINUX
		 if( std::isnan(angle0) ) { angle0 = 0; }
		 if( std::isnan(angle1) ) { angle1 = 0; }
		#endif

		double angle = angle0 + angle1;

		edgeAngles.append( angle );
	}
}

void
CalculateCurvature_Taubin95( MObject& meshObj, MDoubleArray& curvature, const uint whichCurvature )
{
	unsigned int i, j, k;

	MFnMesh			mesh    ( meshObj );
	MItMeshVertex	vertItr ( meshObj );
	MItMeshEdge		edgeItr ( meshObj );
	MItMeshPolygon	faceItr ( meshObj );

	curvature.setLength( mesh.numVertices() );

	MPoint Vi, Vj;
	MVector Ni;

	unsigned numOneRingNeighbor;

	MIntArray		e2n;
	MIntArray		f2e;

	MDoubleArray	area;
	MDoubleArray	wij;
	MVectorArray	Tij;
	MDoubleArray	kij;

	int dummy;
	double areaTemp;
	double totalArea;
	MVector normalTemp;

	double m11, m22;
	double k1, k2;

	int vIdx = 0;
	for( i=0, vertItr.reset(); !vertItr.isDone(); vertItr.next(), ++i ) // per each vertex
	{
		vertItr.getNormal( Ni, MSpace::kObject );	// the normal vector of the vertex (object space)

		Vi = vertItr.position( MSpace::kObject );	// the position of the vertex (object space)

		vertItr.getConnectedEdges( e2n );			// indices of incident edges

		numOneRingNeighbor = e2n.length();			// the # of incident edges

		area.setLength ( numOneRingNeighbor );
		wij.setLength  ( numOneRingNeighbor );
		Tij.setLength  ( numOneRingNeighbor );
		kij.setLength  ( numOneRingNeighbor );

		for( j=0; j<numOneRingNeighbor; ++j ) 		// per each incident edge into this vertex
		{
			edgeItr.setIndex( e2n[j], dummy );		// set 'edgeItr' as the edge

			edgeItr.getConnectedFaces( f2e );		// find the IDs of incident faces into the edge

			area[j] = 0;							// initialize as zero

			for( k=0; k<f2e.length(); ++k ) 		// per each incident face into the edge
			{
				faceItr.setIndex( f2e[k], dummy );	// set 'faceItr' as the face

				faceItr.getArea( areaTemp );		// get the area of the face

				area[j] += areaTemp;				// get total sum of areas
			}

			// get the position of other vertex on the edge
			if( i == edgeItr.index(0) ) { Vj = edgeItr.point( 1, MSpace::kObject ); }
			else                        { Vj = edgeItr.point( 0, MSpace::kObject ); }

			GetTij( Ni, Vi, Vj, Tij[j] );
			GetKij( Ni, Vi, Vj, kij[j] );

			f2e.clear();
		}

		totalArea = 0;
		for( j=0; j<numOneRingNeighbor; ++j ) { totalArea += area[j]; }
		for( j=0; j<numOneRingNeighbor; ++j ) { wij[j] = area[j] / totalArea; }

		/////////////////////////
		// matrix construction //

		ZMat3x3d M;
		double coeff;
		for( j=0; j<numOneRingNeighbor; ++j )
		{
			coeff = wij[j] * kij[j];

			M(0,0) += coeff * Tij[j].x * Tij[j].x;		M(0,1) += coeff * Tij[j].x * Tij[j].y;		M(0,2) += coeff * Tij[j].x * Tij[j].z;
			M(1,0) += coeff * Tij[j].y * Tij[j].x;		M(1,1) += coeff * Tij[j].y * Tij[j].y;		M(1,2) += coeff * Tij[j].y * Tij[j].z;
			M(2,0) += coeff * Tij[j].z * Tij[j].x;		M(2,1) += coeff * Tij[j].z * Tij[j].y;		M(2,2) += coeff * Tij[j].z * Tij[j].z;
		}
		
		ZVec3d   eigenvalues;
		ZMat3x3d eigenvectors;
		M.eigen( eigenvalues, eigenvectors );

		if( ZAlmostZero( eigenvalues[0] ) ) {

			m11 = eigenvalues[1];
			m22 = eigenvalues[2];

		} else if( ZAlmostZero( eigenvalues[1] ) ) {

			m11 = eigenvalues[2];
			m22 = eigenvalues[0];

		} else {

			m11 = eigenvalues[0];
			m22 = eigenvalues[1];

		}

		k1 = 3*m11 - m22;
		k2 = 3*m22 - m11;

		//MGlobal::displayInfo( MString("") + k1 + " " + k2 );

		switch( whichCurvature )
		{
			case ZCurvatureType::zGaussian:

				curvature[vIdx++] = k1 * k2;
				break;

			case ZCurvatureType::zMean:

				curvature[vIdx++] = .5*(k1+k2);
				break;

			case ZCurvatureType::zIntegral:

				curvature[vIdx++] = Z_PI*(k1+k2);
				break;

			default:
				break;
		}

		e2n.clear();
		area.clear();
		wij.clear();
		Tij.clear();
		kij.clear();
	}
}

MString
TriangulateIfNot( const MString& nodeName )
{
	MStringArray tempStrings;
	MString polyTriangulateNode;
	tempStrings = GetConnectedNodeNames( nodeName.asChar(), "inMesh", "polyTriangulate" );

	if( tempStrings.length() != 0 )
	{
		MString nodeType;
		MGlobal::executeCommand( MString( "nodeType tempStrings[0]" ), nodeType );
		if( nodeType != "polyTriangulate" )
		{
			return polyTriangulateNode;
		}
	}

	MGlobal::executeCommand( MString( "polyTriangulate " ) + nodeName );

	polyTriangulateNode = GetConnectedNodeNames( nodeName.asChar(), "inMesh", "polyTriangulate" )[0];
	MGlobal::executeCommand( MString( "select -cl" ) );

	return polyTriangulateNode;
}

void
UndoTriangulate( const MString& nodeName )
{
	MStringArray tempStrings;
	MString polyTriangulateNode;
	tempStrings = GetConnectedNodeNames( nodeName.asChar(), "inMesh", "polyTriangulate" );

	if( tempStrings.length() != 0 )
	{
		MGlobal::executeCommand( MString( "delete " ) + tempStrings[0] );
	}

	MGlobal::executeCommand( MString( "select -cl" ) );
}

MString
TessellateIfNot( const MString& nodeName )
{
	MStringArray tempStrings;
	MString tessellateNode;
	tempStrings = GetConnectedNodeNames( nodeName.asChar(), "worldSpace[0]", "nurbsTessellate" );

	if( tempStrings.length() == 0 )
	{
		MGlobal::executeCommand( MString( "nurbsToPoly -mnd 1  -ch 1 -f 1 -pt 0 -pc 200 -chr 0.9 -ft 0.01 -mel 0.001 -d 0.1 -ut 1 -un 3 -vt 1 -vn 3 -uch 0 -ucr 0 -cht 1 -es 0 -ntr 0 -mrt 0 -uss 1 \" " ) + nodeName + "\"" );
	}

	tessellateNode = GetConnectedNodeNames( nodeName.asChar(), "worldSpace[0]", "nurbsTessellate" )[0];
	MGlobal::executeCommand( MString( "select -cl" ) );

	return tessellateNode;
}
 
MVector
CurrentNormal( const MPointArray& meshVertPositions, const MIntArray& connectedVertIds )
{
	const int numConnectedVerts = (int)connectedVertIds.length();

	MVector a, b, cross, cross_sum(0,0,0), avgPt(0,0,0);

	for( int i=0; i<numConnectedVerts; ++i )
	{
		avgPt += meshVertPositions[connectedVertIds[i]];
	}

	avgPt /= (double)numConnectedVerts;

	// next, we need to sum the cross products between adjacent vertices and the averaged center
	for( int i=0; i<numConnectedVerts; ++i )
	{
		Copy( a, meshVertPositions[connectedVertIds[(i+1)%numConnectedVerts]] );
		Copy( b, meshVertPositions[connectedVertIds[i]] );

		cross = a^b;
		cross_sum += cross;
	}

	cross_sum.normalize();

	return cross_sum;
}	

void
GetOneRingVertIds( MItMeshVertex& vItr, const int iVert, MIntArray& jVerts )
{
	vItr.getConnectedVertices( jVerts );
}

void
GetOneRingVertIds( MItMeshVertex& vItr, vector<MIntArray>& jVerts )
{
	jVerts.clear();

	const int numVerts = vItr.count();

	int iVert = 0;
	for( vItr.reset(); !vItr.isDone(); vItr.next(), ++iVert )
	{
		MIntArray oneRingList;
		GetOneRingVertIds( vItr, iVert, oneRingList );

		jVerts.push_back( oneRingList );
	}
}

void
GetOneAndTwoRingVertIds( MItMeshVertex& vItr, const int iVert, MIntArray& jVerts )
{
	vItr.getConnectedVertices( jVerts ); // add one-ring neighbor vertices

	const int numOneRingVerts = (int)jVerts.length();

	int prevIndex; // a dummy variable
	MIntArray jjVerts; // temporary storage for two-ring neighbor vertices of the j-th neighbor vertex with duplication

	// add two-ring neighbor vertices without duplication
	// for each connected vertex, find the connected vertices and add them to the jVerts array
	for( int j=0; j<numOneRingVerts; ++j )
	{
		vItr.setIndex( jVerts[j], prevIndex );
		vItr.getConnectedVertices( jjVerts );

		const int n = (int)jjVerts.length();
		for( int jj=0; jj<n; ++jj )
		{
			bool alreadyExists = false;
			for( int k=0; k<jVerts.length(); ++k )
			{
				if( jVerts[k] == jjVerts[jj] )
				{
					alreadyExists = true;
					break;
				}
			}

			if( ( jjVerts[jj] != iVert ) && !alreadyExists )
			{
				jVerts.append( jjVerts[jj] );
			}
		}
	}
}

ZELOS_NAMESPACE_END

