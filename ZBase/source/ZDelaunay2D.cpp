//-----------------//
// ZDelaunay2D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// modified from Magic Software library                  //
// last update: 2017.02.14                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZDelaunay2D::ZDelaunay2D( ZFloat2Array& vertices, int& numTrifaces, ZIntArray& connectionInfo, ZIntArray& neighborInfo )
{
	int numVertices = vertices.length();

	m_kVertex.resize( numVertices );

	// output values
	numTrifaces = 0;
	connectionInfo.clear();
	neighborInfo.clear();

	// transform the points to [-1,1]^2 for numerical preconditioning
	float fMin = vertices[0][0], fMax = fMin;

	for( int i=0; i<numVertices; ++i )
	{
		if( vertices[i][0] < fMin )
			fMin = vertices[i][0];
		else if( vertices[i][0] > fMax )
			fMax = vertices[i][0];

		if( vertices[i][1] < fMin )
			fMin = vertices[i][1];
		else if( vertices[i][1] > fMax )
			fMax = vertices[i][1];
	}

	float fHalfRange = 0.5f * ( fMax - fMin );
	float fInvHalfRange = 1 / fHalfRange;

	// sort by x-component to prepare to remove duplicate vertices
	for( int i=0; i<numVertices; ++i )
	{
		m_kVertex[i].m_kV[0] = -1.f + fInvHalfRange * ( vertices[i][0] - fMin );
		m_kVertex[i].m_kV[1] = -1.f + fInvHalfRange * ( vertices[i][1] - fMin );
		m_kVertex[i].m_iIndex = i;
	}

	sort( m_kVertex.begin(),m_kVertex.end() );

	// remove duplicate points
	vector<ZDelaunay2D::ZD_SortedVertex>::iterator pkEnd = unique( m_kVertex.begin(),m_kVertex.end() );
	m_kVertex.erase( pkEnd, m_kVertex.end() );

	// construct supertriangle and add to triangle mesh
	numVertices = (int)m_kVertex.size();
	m_aiSuperV[0] = numVertices;
	m_aiSuperV[1] = numVertices+1;
	m_aiSuperV[2] = numVertices+2;
	m_kVertex.push_back( ZDelaunay2D::ZD_SortedVertex( ZFloat2(-2,-2), m_aiSuperV[0] ) );
	m_kVertex.push_back( ZDelaunay2D::ZD_SortedVertex( ZFloat2( 5,-2), m_aiSuperV[1] ) );
	m_kVertex.push_back( ZDelaunay2D::ZD_SortedVertex( ZFloat2(-2, 5), m_aiSuperV[2] ) );

	ZDelaunay2D::ZD_Triangle* pkTri = new ZDelaunay2D::ZD_Triangle( m_aiSuperV[0], m_aiSuperV[1], m_aiSuperV[2], NULL, NULL, NULL );
	m_kTriangle.insert( pkTri );
	m_apkSuperT[0] = pkTri;
	m_apkSuperT[1] = pkTri;
	m_apkSuperT[2] = pkTri;

	// incrementally update the triangulation
	for( int i=0; i<numVertices; ++i )
	{
		// construct the insertion polygon
        set<ZD_Triangle*> kPolyTri;
		GetInsertionPolygon( m_kVertex[i].m_kV, kPolyTri );

		vector<ZD_Edge> kPoly;
		GetInsertionPolygonEdges( kPolyTri, kPoly );

		// add new triangles formed by the point and insertion polygon edges
		AddTriangles( i, kPoly );

		// remove insertion polygon
		RemoveInsertionPolygon( kPolyTri );
	}

	// remove triangles sharing a vertex of the supertriangle
	RemoveTriangles();

	// assign integer values to the triangles for use by the caller
	map<ZDelaunay2D::ZD_Triangle*,int> kPermute;
	set<ZD_Triangle*>::iterator pkTIter = m_kTriangle.begin();
	for( int i=0; pkTIter!=m_kTriangle.end(); pkTIter++ )
	{
		pkTri = *pkTIter;
		kPermute[pkTri] = i++;
	}
	kPermute[NULL] = -1;

	// put Delaunay triangles into an array (vertices and adjacency info)
	numTrifaces = (int)m_kTriangle.size();
	if( numTrifaces > 0 )
	{
		connectionInfo.setLength( 3*numTrifaces );
		neighborInfo.setLength( 3*numTrifaces );
		int i = 0;
		pkTIter = m_kTriangle.begin();
		for( ; pkTIter!=m_kTriangle.end(); pkTIter++ )
		{
			pkTri = *pkTIter;
			connectionInfo[i] = m_kVertex[pkTri->m_aiV[0]].m_iIndex;
			neighborInfo[i++] = kPermute[pkTri->m_apkAdj[0]];
			connectionInfo[i] = m_kVertex[pkTri->m_aiV[1]].m_iIndex;
			neighborInfo[i++] = kPermute[pkTri->m_apkAdj[1]];
			connectionInfo[i] = m_kVertex[pkTri->m_aiV[2]].m_iIndex;
			neighborInfo[i++] = kPermute[pkTri->m_apkAdj[2]];
		}
	}
}

ZDelaunay2D::~ZDelaunay2D()
{
	set<ZD_Triangle*>::iterator pkTIter = m_kTriangle.begin();
	for( ; pkTIter!=m_kTriangle.end(); pkTIter++ )
	{
		delete *pkTIter;
	}
}

ZDelaunay2D::ZD_Triangle*
ZDelaunay2D::getContaining( const ZFloat2& rkP ) const
{
	// start with supertriangle <S0,S1,V>
	ZDelaunay2D::ZD_Triangle* pkTri = m_apkSuperT[0];
	assert( pkTri->m_aiV[0] == m_aiSuperV[0] &&  pkTri->m_aiV[1] == m_aiSuperV[1] );

	const ZFloat2& rkS1 = m_kVertex[m_aiSuperV[1]].m_kV;
	ZFloat2 kDiff = rkP - rkS1;
	int iSIndex = 1;
	int iVIndex = 2;  // local index following that of S1

	// The search should never iterate over more than all the triangles. But, just to be safe...
	for( int i=0; i<(int)m_kTriangle.size(); ++i )
	{
		// test if P is inside the triangle
		ZFloat2 kVmS1 = m_kVertex[pkTri->m_aiV[iVIndex]].m_kV - rkS1;
		float fKross = kVmS1[0]*kDiff[1] - kVmS1[1]*kDiff[0];
		if( fKross >= 0 ) { return pkTri; }

		pkTri = pkTri->m_apkAdj[iSIndex];
		//assert( pkTri );

		if( pkTri->m_aiV[0] == m_aiSuperV[1] ) {

			iSIndex = 0;
			iVIndex = 1;

		} else if( pkTri->m_aiV[1] == m_aiSuperV[1] ) {

			iSIndex = 1;
			iVIndex = 2;

		} else if( pkTri->m_aiV[2] == m_aiSuperV[1] ) {

			iSIndex = 2;
			iVIndex = 0;

		} else {

			cout << "Error@ZDelaunay2D::getContaining(): Failed." << endl;
			return NULL;
		}
	}

	// by construction, point must be in some triangle in the mesh
	return pkTri;
}

bool
ZDelaunay2D::isInsertionComponent( const ZFloat2& rkV, ZDelaunay2D::ZD_Triangle* pkTri ) const
{
	// determine the number of vertices in common with the supertriangle
	int iCommon = 0, j = -1;
	for( int i=0; i<3; ++i )
	{
		int iV = pkTri->m_aiV[i];
		if( iV == m_aiSuperV[0] )
		{
			iCommon++;
			j = i;
		}
		if( iV == m_aiSuperV[1] )
		{
			iCommon++;
			j = i;
		}
		if ( iV == m_aiSuperV[2] )
		{
			iCommon++;
			j = i;
		}
	}

	if( iCommon == 0 )
	{
		return pkTri->pointInCircle( rkV,m_kVertex );
	}

	if( iCommon == 1 )
	{
		return pkTri->pointLeftOfEdge( rkV, m_kVertex, (j+1)%3, (j+2)%3 );
	}

    return pkTri->pointInTriangle( rkV, m_kVertex );
}

void
ZDelaunay2D::GetInsertionPolygon( const ZFloat2& rkV, set<ZD_Triangle*>& rkPolyTri ) const
{
	// locate triangle containing input point, add to insertion polygon
	ZDelaunay2D::ZD_Triangle* pkTri = getContaining( rkV );
	//assert( isInsertionComponent(rkV,pkTri) );
	rkPolyTri.insert( pkTri );

	// breadth-first search for other triangles in the insertion polygon
	set<ZD_Triangle*> kInterior, kBoundary;
	kInterior.insert( pkTri );
	ZDelaunay2D::ZD_Triangle* pkAdj;

	for( int i=0; i<3; ++i )
	{
		pkAdj = pkTri->m_apkAdj[i];
		if( pkAdj ) { kBoundary.insert( pkAdj ); }
	}

	while( kBoundary.size() > 0 )
	{
		set<ZD_Triangle*> kExterior;

		// process boundary triangles
		set<ZD_Triangle*>::iterator pkIter;

		for( pkIter = kBoundary.begin(); pkIter != kBoundary.end(); pkIter++ )
		{
			// current boundary triangle to process
			pkTri = *pkIter;
			if( isInsertionComponent( rkV, pkTri ) )
			{
				// triangle is an insertion component
				rkPolyTri.insert(pkTri);

				// locate adjacent, exterior triangles for later processing
				for( int i=0; i<3; ++i )
				{
					pkAdj = pkTri->m_apkAdj[i];
					if( pkAdj && kInterior.find(pkAdj) == kInterior.end() && kBoundary.find(pkAdj) == kBoundary.end() )
					{
						kExterior.insert( pkAdj );
					}
				}
			}
		}

		// boundary triangles processed, move them to interior
		for( pkIter = kBoundary.begin(); pkIter != kBoundary.end(); pkIter++ )
		{
			kInterior.insert( *pkIter );
		}

		// exterior triangles are next in line to be processed
		kBoundary = kExterior;
	}

	// If the input point is outside the current convex hull, triangles
	// sharing a supertriangle edge have to be added to the insertion polygon
	// if the non-supertriangle vertex is shared by two edges that are visible
	// to the input point.
	for( int i=0; i<3; ++i )
	{
		pkTri = m_apkSuperT[i];
		if( rkPolyTri.find(pkTri->m_apkAdj[1]) != rkPolyTri.end()
		 && rkPolyTri.find(pkTri->m_apkAdj[2]) != rkPolyTri.end() )
		{
			rkPolyTri.insert( pkTri );
		}
	}
}

void
ZDelaunay2D::GetInsertionPolygonEdges( set<ZD_Triangle*>& rkPolyTri, vector<ZD_Edge>& rkPoly ) const
{
	// locate edges of the insertion polygon
	map<int,ZD_Edge> kIPolygon;
	int iV0, iV1;
	ZDelaunay2D::ZD_Triangle* pkTri;
	ZDelaunay2D::ZD_Triangle* pkAdj;
	set<ZD_Triangle*>::iterator pkTIter;

	for( pkTIter = rkPolyTri.begin(); pkTIter != rkPolyTri.end(); pkTIter++ )
	{
		// get an insertion polygon triangle
		pkTri = *pkTIter;

		// If an adjacent triangle is not an insertion polygon triangle, then
		// the shared edge is a boundary edge of the insertion polygon.  Use
		// this edge to create a new Delaunay triangle from the input vertex
		// and the shared edge.
		for( int j=0; j<3; ++j )
		{
			pkAdj = pkTri->m_apkAdj[j];

			if( pkAdj ) {
				if( rkPolyTri.find(pkAdj) == rkPolyTri.end() )
				{
					// adjacent triangle is not part of insertion polygon
					iV0 = pkTri->m_aiV[j];
					iV1 = pkTri->m_aiV[(j+1)%3];
					kIPolygon[iV0] = ZD_Edge(iV0,iV1,pkTri,pkAdj);
				}

			} else {

				// no adjacent triangle, edge is part of insertion polygon
				iV0 = pkTri->m_aiV[j];
				iV1 = pkTri->m_aiV[(j+1)%3];
				kIPolygon[iV0] = ZD_Edge(iV0,iV1,pkTri,pkAdj);
			}
		}
	}

	// insertion polygon should be at least a triangle
	int iSize = (int)kIPolygon.size();
	//assert( iSize >= 3l  );

	// sort the edges
	map<int,ZD_Edge>::iterator pkE = kIPolygon.begin();
	iV0 = pkE->second.m_iV0;
	for( int i=0; i<iSize; ++i )
	{
		iV1 = pkE->second.m_iV1;
		rkPoly.push_back(pkE->second);
		pkE = kIPolygon.find(iV1);
		//assert( pkE != kIPolygon.end() );
	}
	//assert( iV1 == iV0 );
}

void
ZDelaunay2D::AddTriangles( int iV2, const vector<ZD_Edge>& rkPoly )
{
	// create new triangles
	int iSize = (int)rkPoly.size();
	vector<ZD_Triangle*> kTriangle( iSize );
	ZDelaunay2D::ZD_Triangle* pkTri;
	ZDelaunay2D::ZD_Triangle* pkAdj;
	for( int i=0; i<iSize; ++i )
	{
		const ZD_Edge& rkE = rkPoly[i];

		// construct new triangle
		pkTri = new ZDelaunay2D::ZD_Triangle( rkE.m_iV0, rkE.m_iV1, iV2,rkE.m_pkA, NULL, NULL );
		kTriangle[i] = pkTri;
		if( rkE.m_iV0 == m_aiSuperV[0] && rkE.m_iV1 == m_aiSuperV[1] )
			m_apkSuperT[0] = pkTri;
		else if( rkE.m_iV0 == m_aiSuperV[1] && rkE.m_iV1 == m_aiSuperV[2] )
			m_apkSuperT[1] = pkTri;
		else if ( rkE.m_iV0 == m_aiSuperV[2] && rkE.m_iV1 == m_aiSuperV[0] )
			m_apkSuperT[2] = pkTri;

		// update information of triangle adjacent to insertion polygon
		pkAdj = rkE.m_pkA;
		if( pkAdj )
		{
			for( int j=0; j<3; ++j )
			{
				if( pkAdj->m_apkAdj[j] == rkE.m_pkT )
				{
					pkAdj->m_apkAdj[j] = pkTri;
					break;
				}
			}
		}
	}

	// Insertion polygon is a star polygon with center vertex V2.  Finalize
	// the triangles by setting the adjacency information.
	for( int i=0; i<iSize; ++i )
	{
		// current triangle
		pkTri = kTriangle[i];

		// connect to next new triangle
		int iNext = i+1;
		if( iNext == iSize ) { iNext = 0; }
		pkTri->m_apkAdj[1] = kTriangle[iNext];

		// connect to previous new triangle
		int iPrev = i-1;
		if( iPrev == -1 ) { iPrev = iSize-1; }
		pkTri->m_apkAdj[2] = kTriangle[iPrev];
	}

	// add the triangles to the mesh
	for( int i=0; i<iSize; ++i )
		m_kTriangle.insert( kTriangle[i] );
}

void
ZDelaunay2D::RemoveInsertionPolygon( set<ZD_Triangle*>& rkPolyTri )
{
	set<ZD_Triangle*>::iterator pkTIter;
	for( pkTIter=rkPolyTri.begin(); pkTIter!=rkPolyTri.end(); pkTIter++ )
	{
		ZDelaunay2D::ZD_Triangle* pkTri = *pkTIter;
		m_kTriangle.erase( pkTri );
		delete pkTri;
	}
}

void
ZDelaunay2D::RemoveTriangles()
{
	// identify those triangles sharing a vertex of the supertriangle
	set<ZD_Triangle*> kRemoveTri;
	ZDelaunay2D::ZD_Triangle* pkTri;
	set<ZD_Triangle*>::iterator pkTIter = m_kTriangle.begin();
	for( ; pkTIter!=m_kTriangle.end(); pkTIter++ )
	{
		pkTri = *pkTIter;
		for( int j=0; j<3; ++j )
		{
			int iV = pkTri->m_aiV[j];
			if( iV == m_aiSuperV[0] || iV == m_aiSuperV[1] || iV == m_aiSuperV[2] )
			{
            	kRemoveTri.insert( pkTri );
				break;
			}
		}
	}

	// remove the triangles from the mesh
	pkTIter = kRemoveTri.begin();
	for( ; pkTIter!=kRemoveTri.end(); pkTIter++ )
	{
		pkTri = *pkTIter;
		m_kTriangle.erase(pkTri);
		delete pkTri;
	}
}

ZELOS_NAMESPACE_END

