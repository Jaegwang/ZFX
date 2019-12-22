//-----------//
// ZMesh.cpp //
//-------------------------------------------------------//
// author: Taeyong Kim @ nVidia                          //
//         Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZMesh::ZMesh()
{
	// nothing to do
}

ZMesh::ZMesh( const ZMesh& other )
{
	*this = other;
}

ZMesh::ZMesh( const char* filePathName )
{
	load( filePathName );
}

void
ZMesh::reset()
{
	_points   .reset();
	_uvs      .reset();
	_elements .reset();
}

ZMesh&
ZMesh::operator=( const ZMesh& other )
{
	_points   = other._points;
	_uvs      = other._uvs;
	_elements = other._elements;

	return (*this);
}

bool
ZMesh::create( const ZPointArray& vertexPositions, const ZIntArray& polyCounts, const ZIntArray& polyConnections, ZMeshElementType::MeshElementType type )
{
	reset();

	const int nElems = polyCounts.length();

	if( nElems != polyCounts.length() ) // = (# of elements)
	{
		cout << "Error@ZMesh::create(): Invalid input data." << endl;
		return false;
	}

	_points = vertexPositions;
	_elements.reserve( nElems );

	int idx = 0;
	FOR( i, 0, nElems )
	{
		const int n = polyCounts[i];
		_elements.push_back( ZMeshElement( type, n ) );
		ZMeshElement& justAdded = *_elements.rbegin();
		FOR( j, 0, n ) { justAdded[j] = polyConnections[idx++]; }
	}

	return true;
}

bool
ZMesh::assignUVs( const ZPointArray& uvs, const ZIntArray& uvIndices )
{
	_uvs.clear();

	const int nElems  = numElements();
	const int uvsLen  = uvs.length();
	const int uviLen  = uvIndices.length();

	if( (nElems*uvsLen*uviLen) == 0 ) { return true; }

	FOR( i, 0, uviLen )
	{
		if( uvIndices[i] >= uvsLen )
		{
			cout << "Error@ZMesh::assignUVs(): There are invalid uv indices." << endl;
			return false;
		}
	}

	int idx = 0;
	FOR( i, 0, nElems )
	{
		FOR( j, 0, _elements[i].count() )
		{
			++idx;
		}
	}

	if( idx != uviLen )
	{
		cout << "Error@ZMesh::assignUVs(): Invalid length of uvIndices." << endl;
		return false;
	}

	_uvs = uvs;

	idx = 0;
	FOR( i, 0, nElems )
	{
		ZMeshElement& e = _elements[i];
		const int n = e.count();
		FOR( j, 0, n ) { e(j) = uvIndices[idx++]; }
	}

	return true;
}

int
ZMesh::addPoint( const ZPoint& point )
{
	_points.push_back( point );
	return ( _points.length() - 1 );
}

int 
ZMesh::addPoints( const ZPointArray& points )
{
	_points.append( points );
	return (_points.length() - 1);
}

int
ZMesh::addUV( const ZPoint& uv )
{
	this->_uvs.push_back( uv );
	return ( _uvs.size() - 1 );
}

int
ZMesh::addUVs( const ZPointArray& uvs )
{
	this->_uvs.append( uvs );
	return ( _uvs.size() - 1 );
}

ZMeshElement&
ZMesh::addElement( const ZMeshElement& element )
{
	_elements.push_back( element );
	ZMeshElement& justAdded = *_elements.rbegin();
	return justAdded;
}

ZMeshElement&
ZMesh::addLine( int v0, int v1, int id )
{
	ZMeshElement e( ZMeshElementType::zLine, 2 );
    
	e[0] = v0;
	e[1] = v1;

	e.id = id;

	return addElement(e);
}

ZMeshElement&
ZMesh::addLine( const ZPoint& p0, const ZPoint& p1, int id )
{
	int v0 = addPoint(p0);
	int v1 = addPoint(p1);

	return addLine( v0,v1, id );
}

ZMeshElement&
ZMesh::addLine( int v0, int v1, int uv0, int uv1, int id )
{
	ZMeshElement& e = addLine( v0,v1, id );

	e(0) = uv0;
	e(1) = uv1;

	return e;
}

ZMeshElement&
ZMesh::addTri( int v0, int v1, int v2, int id )
{
	ZMeshElement e( ZMeshElementType::zFace, 3 );
    
	e[0] = v0;
	e[1] = v1;
	e[2] = v2;

	e.id = id;

	return addElement(e);
}

ZMeshElement&
ZMesh::addTri( const ZPoint& p0, const ZPoint& p1, const ZPoint& p2, int id )
{
	int v0 = addPoint(p0);
	int v1 = addPoint(p1);
	int v2 = addPoint(p2);

	return addTri( v0,v1,v2, id );
}

ZMeshElement&
ZMesh::addTri( int v0, int v1, int v2, int uv0, int uv1, int uv2, int id )
{
	ZMeshElement& e = addTri( v0,v1,v2, id );

	e(0) = uv0;
	e(1) = uv1;
	e(2) = uv2;

	return e;
}

ZMeshElement&
ZMesh::addTet( int v0, int v1, int v2, int v3, int id )
{
	ZMeshElement e( ZMeshElementType::zTet, 4 );
    
	e[0] = v0;
	e[1] = v1;
	e[2] = v2;
	e[3] = v3;

	e.id = id;

	return addElement(e);
}

ZMeshElement&
ZMesh::addTet( const ZPoint& p0, const ZPoint& p1, const ZPoint& p2, const ZPoint& p3, int id )
{
	int v0 = addPoint(p0);
	int v1 = addPoint(p1);
	int v2 = addPoint(p2);
	int v3 = addPoint(p3);

	return addTet( v0,v1,v2,v3, id );
}

ZMeshElement&
ZMesh::addTet( int v0, int v1, int v2, int v3, int uv0, int uv1, int uv2, int uv3, int id )
{
	ZMeshElement& e = addTet( v0,v1,v2,v3, id );

	e(0) = uv0;
	e(1) = uv1;
	e(2) = uv2;
	e(3) = uv3;

	return e;
}

ZMeshElement&
ZMesh::addQuad( int v00, int v10, int v11, int v01, int id )
{
	ZMeshElement e( ZMeshElementType::zFace, 4 );

	e[0] = v00;
	e[1] = v10;
	e[2] = v11;
	e[3] = v01;

	e.id = id;

	return addElement(e);
}

ZMeshElement&
ZMesh::addQuad( const ZPoint& p00, const ZPoint& p10, const ZPoint& p11, const ZPoint& p01, int id )
{
	int v00 = addPoint(p00);
	int v10 = addPoint(p10);
	int v11 = addPoint(p11);
	int v01 = addPoint(p01);
    
	return addQuad( v00,v10,v11,v01, id );
}

ZMeshElement&
ZMesh::addQuad( int v00, int v10, int v11, int v01, int uv00, int uv10, int uv11, int uv01, int id )
{
	ZMeshElement& e = addQuad( v00,v10,v11,v01, id );

	e(0) = uv00;
	e(1) = uv10;
	e(2) = uv11;
	e(3) = uv01;

	return e;
}

ZMeshElement&
ZMesh::addCube( int v000, int v100, int v101, int v001, int v010, int v110, int v111, int v011, int id )
{
	ZMeshElement e( ZMeshElementType::zCube, 8 );

	e[0] = v000;
	e[1] = v100;
	e[2] = v101;
	e[3] = v001;
	e[4] = v010;
	e[5] = v110;
	e[6] = v111;
	e[7] = v011;

	e.id = id;

	return addElement(e);
}

ZMeshElement&
ZMesh::addCube( const ZPoint& p000, const ZPoint& p100, const ZPoint& p101, const ZPoint& p001, const ZPoint& p010, const ZPoint& p110, const ZPoint& p111, const ZPoint& p011, int id )
{
	int v000 = addPoint(p000);
	int v100 = addPoint(p100);
	int v101 = addPoint(p101);
	int v001 = addPoint(p001);
	int v010 = addPoint(p010);
	int v110 = addPoint(p110);
	int v111 = addPoint(p111);
	int v011 = addPoint(p011);

	return addCube( v000, v100, v101, v001, v010, v110, v111, v011, id );
}

ZMeshElement&
ZMesh::addCube( int v000, int v100, int v101, int v001, int v010, int v110, int v111, int v011, int uv000, int uv100, int uv101, int uv001, int uv010, int uv110, int uv111, int uv011, int id )
{
	ZMeshElement& e = addCube( v000, v100, v101, v001, v010, v110, v111, v011, id );

	e(0) = uv000;
	e(1) = uv100;
	e(2) = uv101;
	e(3) = uv001;
	e(4) = uv010;
	e(5) = uv110;
	e(6) = uv111;
	e(7) = uv011;

	return e;
}

void
ZMesh::append( const ZMesh& mesh )
{
	// append verts
	const int nVerts = numVertices();
	_points.reserve( nVerts + mesh.numVertices() );
	_points.insert( _points.end(), mesh._points.begin(), mesh._points.end() );

	// append uvs
	const int nUVs = numUVs();
	_uvs.reserve( nUVs + mesh.numUVs() );
	_uvs.insert( _uvs.end(), mesh._uvs.begin(), mesh._uvs.end() );

	// append elems
	_elements.reserve( numElements() + mesh.numElements()) ;

	const ZMeshElementArray& elems = mesh._elements;
	ZMeshElementArray::const_iterator e = elems.begin();
	for( ; e != elems.end(); ++e )
	{
		_elements.push_back( *e );
		ZMeshElement& f = *_elements.rbegin();

		// shift vert index
		FOR( i, 0, f.count() )
		{
			f[i] += nVerts;
			f(i) += nUVs;
		}
	}
}

bool
ZMesh::deleteElement( int i )
{
	const int last = (int)_elements.size()-1;
	if( last<0 || (int)i > last ) { return false; }

	_elements[i] = _elements[last];
	_elements.pop_back();

	return true;
}

int
ZMesh::deleteElements( const ZIntArray& toDel )
{
	const int nElems = (int)_elements.size();

	ZIntArray mask( nElems );
	mask.setMask( toDel, true );

	ZMeshElementArray alive;
	alive.reserve( ZMax( 10, nElems - toDel.length() ) );

	// copy only remaining (not deleted) elements
	FOR( i, 0, nElems )
	{
		if( mask[i] ) { continue; }
		alive.push_back( _elements[i] );
	}

	swap( _elements, alive );

	deleteUnusedPointsAndUVs();

	return (int)_elements.size();
}

void
ZMesh::deleteUnusedPointsAndUVs()
{
	ZIntArray unusedPoints;
	_findUnusedPoints( unusedPoints );
	_deletePoints( unusedPoints );

	ZIntArray unusedUVs;
	_findUnusedUVs( unusedUVs );
	_deleteUVs( unusedUVs );
}

int
ZMesh::_findUnusedPoints( ZIntArray& unusedPoints )
{
	const int nVerts = numVertices();
	const int nElems = numElements();

	ZIntArray mask( nVerts );
	unusedPoints.reserve( nVerts );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];
		const int n = e.count();
		FOR( j, 0, n )
		{
			mask[e[j]] = true;
		}
	}

	FOR( i, 0, nVerts )
	{
		if( mask[i] ) { continue; }
		unusedPoints.push_back( i );
	}

	return unusedPoints.length();
}

int
ZMesh::_findUnusedUVs( ZIntArray& unusedUVs )
{
	const int nUVs   = numUVs();
	const int nElems = numElements();

	if( nUVs <= 0 ) { return 0; }

	ZIntArray mask( nUVs );
	unusedUVs.reserve( nUVs );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];
		const int n = e.count();
		FOR( j, 0, n )
		{
			mask[e(j)] = true;
		}
	}

	FOR( i, 0, nUVs )
	{
		if( mask[i] ) { continue; }
		unusedUVs.push_back( i );
	}

	return unusedUVs.length();
}

void
ZMesh::_deletePoints( const ZIntArray& toDel )
{
	const int nVerts = numVertices();

	ZIntArray mask( nVerts );
	mask.setMask( toDel, true );

	ZPointArray tmp;
	tmp.reserve( ZMax( 10, nVerts - toDel.length() ) );

	ZIntArray newVertexTable( nVerts, -1 );

	// copy only remaining (not deleted) points
	FOR( i, 0, nVerts ) // per each vertex
	{
		if( mask[i] ) { continue; }
		newVertexTable[i] = tmp.length();
		tmp.push_back( _points[i] );
    }

	// update face vert id
	_updateElementVertices( newVertexTable );

	// copy to verts
	_points = tmp;
}

void
ZMesh::_deleteUVs( const ZIntArray& toDel )
{
	const int nUVs = numUVs();
	if( nUVs <= 0 ) { return; }

	ZIntArray mask( nUVs );
	mask.setMask( toDel, true );

	ZPointArray tmp;
	tmp.reserve( ZMax( 10, nUVs - toDel.length() ) );

	ZIntArray newUVTable( nUVs, -1 );

	// copy only remaining (not deleted) points
	FOR( i, 0, nUVs ) // per each vertex
	{
		if( mask[i] ) { continue; }
		newUVTable[i] = tmp.length();
		tmp.push_back( _uvs[i] );
    }

	// update face vert id
	_updateElementUVs( newUVTable );

	// copy to verts
	_uvs = tmp;
}

void
ZMesh::_updateElementVertices( ZIntArray& newVertexTable )
{
	const int nElems = numElements();

	FOR( i, 0, nElems ) // per each element
	{
        ZMeshElement& e = _elements[i];

		const int n = e.count();
		FOR( j, 0, n ) // per each vertex of this element
		{
			int& eVert = e[j];

			// no need to change
			if( newVertexTable[eVert] < 0 ) { continue; }

			eVert = newVertexTable[eVert];
        }
    }
}

void
ZMesh::_updateElementUVs( ZIntArray& newUVTable )
{
	const int nElems = numElements();

	FOR( i, 0, nElems ) // per each element
	{
        ZMeshElement& e = _elements[i];

		FOR( j, 0, e.count() ) // per each vertex of this element
		{
			int& eUV = e(j);

			// no need to change
			if( newUVTable[eUV] < 0 ) { continue; }

			eUV = newUVTable[eUV];
        }
    }
}

ZBoundingBox
ZMesh::boundingBox() const
{
	return _points.boundingBox();
}

void
ZMesh::getMeshInfo( ZIntArray& polyCounts, ZIntArray& polyConnections ) const
{
	polyCounts.clear();
	polyConnections.clear();

	const int nElems = numElements();

	polyCounts.reserve( nElems );
	polyConnections.reserve( nElems*3 );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];

		int cnt = 0;
		polyCounts.push_back( cnt = e.count() );

		FOR( j, 0, cnt )
		{
			polyConnections.push_back( e[j] );
		}
	}
}

void
ZMesh::getConnections( ZIntArray& polyConnections ) const
{
	polyConnections.clear();

	const int nElems = numElements();

	polyConnections.reserve( nElems*3 );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];

		const int cnt = e.count();

		FOR( j, 0, cnt )
		{
			polyConnections.push_back( e[j] );
		}
	}
}

void
ZMesh::getVertexNormals( ZVectorArray& normals ) const
{
	const int nVerts = numVertices();
	const int nElems = numElements();

	normals.setLength( nVerts );

	ZVector nrm;
	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];

		const int n = e.count();

		if( n >= 3 ) {

			const int& v0 = e[0];
			const int& v1 = e[1];
			const int& v2 = e[2];

			const ZPoint& p0 = _points[v0];
			const ZPoint& p1 = _points[v1];
			const ZPoint& p2 = _points[v2];

			nrm = Normal( p0, p1, p2 );
			nrm *= Area( p0, p1, p2 );

		} else if( n == 2 ) {

			const int& v0 = e[0];
			const int& v1 = e[1];

			const ZPoint& p0 = _points[v0];
			const ZPoint& p1 = _points[v1];

			nrm = p1 - p0;

		} else {

			nrm.set(0,1,0);

		}

		FOR( j, 0, n )
		{
			normals[ e[j] ] += nrm;
		}
	}

	FOR( i, 0, nVerts )
	{
		normals[i].normalize();
	}
}

void
ZMesh::getElementNormals( ZVectorArray& normals ) const
{
	const int nElems = numElements();

	normals.setLength( nElems );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];

		ZVector& nrm = normals[i];

		const int n = e.count();

		if( n >= 3 ) {

			const int& v0 = e[0];
			const int& v1 = e[1];
			const int& v2 = e[2];

			const ZPoint& p0 = _points[v0];
			const ZPoint& p1 = _points[v1];
			const ZPoint& p2 = _points[v2];

			nrm = Normal( p0, p1, p2 );

		} else if( n == 2 ) {

			const int& v0 = e[0];
			const int& v1 = e[1];

			const ZPoint& p0 = _points[v0];
			const ZPoint& p1 = _points[v1];

			nrm = p1 - p0;
			nrm.normalize();

		} else {

			nrm.set(0,1,0);

		}
	}
}

ZMesh&
ZMesh::triangulate()
{
	if( isTriangulated() ) { return (*this); }

	const int nElems = numElements();

	ZMeshElementArray newElements;
	newElements.reserve( nElems*2 );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];
		if( e.type != ZMeshElementType::zFace )
		{
			newElements.push_back( e );
			continue;
		}

		FOR( j, 0, e.count()-2 )
		{
			ZMeshElement new_e;

			new_e.type = ZMeshElementType::zFace;
			new_e.setCount( 3 );
			new_e.id = e.id;

			new_e[0] = e[0];
			new_e[1] = e[j+1];
			new_e[2] = e[j+2];

			new_e(0) = e(0);
			new_e(1) = e(j+1);
			new_e(2) = e(j+2);

			newElements.push_back( new_e );
		}
	}

	swap( _elements, newElements );

	return (*this);
}

bool
ZMesh::isTriangulated() const
{
	const int nElems = numElements();

	bool alreadyTriangulated = true;

	FOR( i, 0, nElems )
	{
		if( _elements[i].count() != 3 )
		{
			alreadyTriangulated = false;
			break;
		}
	}

	return alreadyTriangulated;
}

void
ZMesh::getTriangleIndices( ZInt3Array& triConnections ) const
{
	triConnections.clear();
	const int nElems = numElements();
	triConnections.reserve( nElems );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];
		if( e.type != ZMeshElementType::zFace ) { continue; }

		FOR( j, 0, e.count()-2 )
		{
			ZInt3 idx;
			idx[0] = e[0];
			idx[1] = e[j+1];
			idx[2] = e[j+2];

			triConnections.push_back( idx );
		}
	}
}

void
ZMesh::getTriangleCenters( ZPointArray& centers, int computingMethod, bool useOpenMP ) const
{
	ZInt3Array triangles;
	getTriangleIndices( triangles );

	const int numTriangles = triangles.length();
	centers.setLength( numTriangles );

	const ZPointArray& vPos = _points.length();

	#pragma omp parallel for if( useOpenMP && numTriangles > 10000 )
	FOR( i, 0, numTriangles )
	{
		ZPoint& tc = centers[i];
		tc.zeroize();

		const ZInt3& tIdx = triangles[i];

		tc += vPos[ tIdx[0] ];
		tc += vPos[ tIdx[1] ];
		tc += vPos[ tIdx[2] ];

		tc *= ( 1/3.f );
	}
}

void
ZMesh::getQuadrilateralIndices( ZInt4Array& quadConnections ) const
{
	quadConnections.clear();
	const int nElems = numElements();
	quadConnections.reserve( nElems );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];
		if( e.type != ZMeshElementType::zFace ) { continue; }

		FOR( j, 0, e.count()-3 )
		{
			ZInt4 idx;
			idx[0] = e[0];
			idx[1] = e[j+1];
			idx[2] = e[j+2];
			idx[3] = e[j+3];

			quadConnections.push_back( idx );
		}
	}
}

void
ZMesh::getTetrahedronIndices( ZInt4Array& tetConnections ) const
{
	tetConnections.clear();
	const int nElems = numElements();
	tetConnections.reserve( 4*nElems );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];
		if( e.type != ZMeshElementType::zTet ) { continue; }

		const int numVerts = e.count();
		if( numVerts!=4 ) { continue; }

		tetConnections.push_back( e[0] );
		tetConnections.push_back( e[1] );
		tetConnections.push_back( e[2] );
		tetConnections.push_back( e[3] );
	}
}

void
ZMesh::weld( float epsilon )
{
//	if( _points.empty() ) { return; }
//
//	const int nVerts = numVertices();
//	const int nUVs   = numUVs();
//	const int nElems = numElements();
//
//	///////////////////////
//	// for vertex points //
//
//	ZIntArray closePts;
//	ZIntArray tagPts( nVerts, -1 );
//	ZBoundingBox bBox;
//	bBox = _points.boundingBox();
//	ZHashGrid3D hashGrdPts( bBox.diagonalLength()/powf(_points.length(),1/3.f) );
//
//	// mark close vertices for deletion
//	FOR( i, 0, nVerts )
//	{
//		if( tagPts[i] >= 0 ) { continue; } // already marked for deletion
//
//		hashGrdPts.getCandidates( closePts, _points[i], epsilon );
//
//		const int n = closePts.length();
//		FOR( j, 0, n )
//       	{
//			int toDel = closePts[j];
//			if( toDel == i ) { continue; } // same vertex
//			if( tagPts[toDel] >= 0 ) { continue; } // already marked
//			tagPts[toDel] = i;
//   	    }
//    }
//
//	// update face vertex id for deleted vertices
//	FOR( i, 0, nElems )
//	{
//		ZMeshElement& e = _elements[i];
//
//		const int n = e.count();
//		FOR( j, 0, n )
//		{
//			int v = e[j];
//			if( tagPts[v] < 0 ) { continue; } // not deleted
//			e[j] = tagPts[v];
//		}
//	}
//
//	/////////////
//	// for UVs //
//
//	ZIntArray closeUVs;
//	ZIntArray tagUVs( nUVs, -1 );
//	bBox = _uvs.boundingBox();
//	ZHashGrid3D hashGrdUVs( 1/powf(_uvs.length(),1/3.f) );
//
//	// mark close UVs for deletion
//	FOR( i, 0, nUVs )
//	{
//		if( tagUVs[i] >= 0 ) { continue; } // already marked for deletion
//
//		hashGrdUVs.getCandidates( closeUVs, _uvs[i], epsilon );
//
//		const int n = closeUVs.length();
//		FOR( j, 0, n )
//       	{
//			int toDel = closeUVs[j];
//			if( toDel == i ) { continue; } // same UV
//			if( tagUVs[toDel] >= 0 ) { continue; } // already marked
//			tagUVs[toDel] = i;
//   	    }
//    }
//
//	// update face UV id for deleted UVs
//	FOR( i, 0, nElems )
//	{
//		ZMeshElement& e = _elements[i];
//
//		const int n = e.count();
//		FOR( j, 0, n )
//		{
//			int v = e(j);
//			if( tagUVs[v] < 0 ) { continue; } // not deleted
//			e(j) = tagUVs[v];
//		}
//	}
//
//	deleteUnusedPointsAndUVs();
}

void
ZMesh::reverse()
{
	const int nElems = numElements();

	FOR( i, 0, nElems )
	{
		ZMeshElement& e = _elements[i];

		const int nVerts = e.count();
		FOR( j, 0, (int)(0.5*nVerts) )
		{
			ZSwap( e[j], e[nVerts-1-j] );
		}
	}
}

void
ZMesh::exchange( ZMesh& mesh )
{
	_points.exchange   ( mesh._points   );
	_uvs.exchange      ( mesh._uvs      );
	_elements.exchange ( mesh._elements );
}

void
ZMesh::transform( const ZMatrix& matrix, bool useOpenMP )
{
	const int nVerts = numVertices();

	#pragma omp parallel for if( useOpenMP && nVerts>100000 )
	FOR( i, 0, nVerts )
	{
		_points[i] = matrix.transform( _points[i], false );
	}
}

const ZString
ZMesh::dataType() const
{
	return ZString( "ZMesh" );
}

void
ZMesh::write( ofstream& fout ) const
{
	const int nVerts = numVertices();
	const int nUVs   = numUVs();
	const int nElems = numElements();

	fout.write( (char*)&nVerts, sizeof(int) );				// number of vertices
	fout.write( (char*)&nUVs,   sizeof(int) );				// number of UVs
	fout.write( (char*)&nElems, sizeof(int) );				// number of elements

	_points.write( fout, false );							// vertex positions
	_uvs.write( fout, false );								// texture coordinates

	ZIntArray counts( nElems );
	ZIntArray elemTypes( nElems );
	ZIntArray elemIds( nElems );
	ZIntArray connections;   connections.reserve( nElems*8 );
	ZIntArray uvIndices;     uvIndices.reserve( nElems*8 );

	FOR( i, 0, nElems )
	{
		const ZMeshElement& e = _elements[i];

		counts[i]    = e.count();
		elemTypes[i] = e.type;
		elemIds[i]   = e.id;

		const int n = e.count();
		FOR( j, 0, n )
		{
			connections.push_back( e[j] );
			uvIndices.push_back( e(j) );
		}
	}

	counts.write( fout, false );
	elemTypes.write( fout, false );
	elemIds.write( fout, false );
	connections.write( fout, true );
	uvIndices.write( fout, true );
}

void
ZMesh::read( ifstream& fin )
{
	reset();

	int nVerts=0, nUVs=0, nElems=0;

	fin.read( (char*)&nVerts, sizeof(int) );				// number of vertices
	fin.read( (char*)&nUVs,   sizeof(int) );				// number of UVs
	fin.read( (char*)&nElems, sizeof(int) );				// number of elements

	_points.setLength( nVerts );    _points.read( fin, false );
	_uvs.setLength( nUVs );         _uvs.read( fin, false );

	ZIntArray counts( nElems );		counts.read( fin, false );
	ZIntArray elemTypes( nElems );	elemTypes.read( fin, false );
	ZIntArray elemIds( nElems );	elemIds.read( fin, false );

	ZIntArray connections;          connections.read( fin, true );
	ZIntArray uvIndices;			uvIndices.read( fin, true );

	_elements.reserve( nElems );

	int idx = 0;
	FOR( i, 0, nElems )
	{
		const int n = counts[i];
		_elements.push_back( ZMeshElement( (ZMeshElementType::MeshElementType)elemTypes[i], n ) );
		ZMeshElement& justAdded = *_elements.rbegin();
		FOR( j, 0, n ) { justAdded[j] = connections[idx++]; }
	}

	idx = 0;
	FOR( i, 0, nElems )
	{
		ZMeshElement& e = _elements[i];
		const int n = e.count();
		FOR( j, 0, n ) { e(j) = uvIndices[idx++]; }
	}
}

bool
ZMesh::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZMesh::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	dataType().write( fout, true );
	write( fout );
	fout.close();
	return true;
}

bool
ZMesh::load( const char* filePathName )
{
	reset();

	ifstream fin( filePathName, ios::out|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZMesh::load(): Failed to load file." << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZMesh::load(): Data type mismatch." << endl;
		reset();
		return false;
	}
	read( fin );
	fin.close();
	return true;
}

void
ZMesh::draw( ZMeshDisplayMode::MeshDisplayMode mode, const ZColor& lineColor, const ZColor& surfaceColor, float opacity ) const
{
	const int numVerts = numVertices();
	const int numElems = numElements();
	if( !( numVerts && numElems ) ) { return; }

	if( mode == ZMeshDisplayMode::zPoints ) // as points cloud
	{
		glColor( lineColor, opacity );
		glBegin( GL_POINTS );
			FOR( i, 0, numVerts ) { glVertex( _points[i] ); }
		glEnd();

		return;
	}

	if( ( mode == ZMeshDisplayMode::zSurface ) || ( mode == ZMeshDisplayMode::zWireSurface ) ) // as solid surface
	{
		glColor( surfaceColor, opacity );
		FOR( i, 0, numElems )
		{
			glBegin( GL_POLYGON );
				const int n = _elements[i].count();
				FOR( j, 0, n ) { glVertex( _points[ _elements[i][j] ] ); }
			glEnd();
		}
	}

	if( ( mode == ZMeshDisplayMode::zWireframe ) || ( mode == ZMeshDisplayMode::zWireSurface ) ) // as wireframe
	{
		glEnable( GL_POLYGON_OFFSET_LINE );
		glPolygonOffset( -1, -1 );

		glColor( lineColor, opacity );
		FOR( i, 0, numElems )
		{
			switch( _elements[i].type )
			{
				case ZMeshElementType::zFace:
				{
					glBegin( GL_LINE_LOOP );
						const int n = _elements[i].count();
						FOR( j, 0, n ) { glVertex( _points[ _elements[i][j] ] ); }
					glEnd();

					break;
				}

				case ZMeshElementType::zTet:
				{
					glBegin( GL_LINES );
						glVertex( _points[ _elements[i][0] ] ); glVertex( _points[ _elements[i][1] ] );
						glVertex( _points[ _elements[i][0] ] ); glVertex( _points[ _elements[i][2] ] );
						glVertex( _points[ _elements[i][0] ] ); glVertex( _points[ _elements[i][3] ] );
						glVertex( _points[ _elements[i][1] ] ); glVertex( _points[ _elements[i][2] ] );
						glVertex( _points[ _elements[i][1] ] ); glVertex( _points[ _elements[i][3] ] );
						glVertex( _points[ _elements[i][2] ] ); glVertex( _points[ _elements[i][3] ] );
					glEnd();

					break;
				}
			}
		}
	}
}

void
ZMesh::drawVertexNormals( const ZVectorArray& normals, const ZColor& lineColor, float scale ) const
{
	const int nVerts = numVertices();
	if( !nVerts ) { return; }

	if( normals.length() != nVerts )
	{
		cout << "Error@ZMesh::drawNormals(): Invalid length of input normals" << endl;
		return;
	}

	glColor( lineColor );

	if( ZAlmostSame(scale,1.f) ) {

		glBegin( GL_LINES );
			FOR( i, 0, nVerts ) { glLine( _points[i], normals[i] ); }
		glEnd();

	} else {

		glBegin( GL_LINES );
			FOR( i, 0, nVerts ) { glLine( _points[i], scale*normals[i] ); }
		glEnd();

	}
}

void
ZMesh::drawUVs( const ZColor& lineColor ) const
{
	if( !numUVs() ) { return; }

	const int numElems = numElements();

	glColor( lineColor );

	FOR( i, 0, numElems )
	{
		glBegin( GL_LINE_LOOP );
			const int n = _elements[i].count();
			FOR( j, 0, n ) { glVertex( _uvs[ _elements[i](j) ] ); }
		glEnd();
	}
}

void
ZMesh::drawVertices() const
{
	_points.drawPoints();
}

void
ZMesh::drawWireframe() const
{
	const int numVerts = numVertices();
	const int numElems = numElements();
	if( !( numVerts && numElems ) ) { return; }

	FOR( i, 0, numElems )
	{
		const ZMeshElement& e = _elements[i];
		const int n = e.count();

		glBegin( GL_LINE_LOOP );
		{
			FOR( j, 0, n )
			{
				glVertex( _points[ e[j] ] );
			}
		}
		glEnd();

		// additional two lines for a tetrahedron
		if( e.type == ZMeshElementType::zTet )
		{
			glBegin( GL_LINES );
			{
				glVertex( _points[ e[0] ] );
				glVertex( _points[ e[2] ] );

				glVertex( _points[ e[1] ] );
				glVertex( _points[ e[3] ] );
			}
			glEnd();
		}
	}
}

void
ZMesh::drawSurface( ZVectorArray* vNrmPtr ) const
{
	const int numVerts = numVertices();
	const int numElems = numElements();
	if( !( numVerts && numElems ) ) { return; }

	if( vNrmPtr ) { // with vertex normals

		FOR( i, 0, numElems )
		{
			const ZMeshElement& e = _elements[i];
			const int n = e.count();

			glBegin( GL_POLYGON );
			{
				FOR( j, 0, n )
				{
					glNormal( (*vNrmPtr)[ e[j] ] );
					glVertex( _points[ e[j] ] );
				}
			}
			glEnd();
		}

	} else { // without vertex normals

		FOR( i, 0, numElems )
		{
			const ZMeshElement& e = _elements[i];
			const int n = e.count();

			glBegin( GL_POLYGON );
			{
				FOR( j, 0, n )
				{
					glVertex( _points[ e[j] ] );
				}
			}
			glEnd();
		}

	}
}

void
ZMesh::drawWireSurface( const ZColor& wireColor, ZVectorArray* vNrmPtr ) const
{
	const int numVerts = numVertices();
	const int numElems = numElements();
	if( !( numVerts && numElems ) ) { return; }

	const bool lightingWasOn  = glIsEnabled( GL_LIGHTING ) ? true : false;
	const bool lightingWasOff = !lightingWasOn;

	if( lightingWasOff && vNrmPtr ) { glEnable( GL_LIGHTING ); }

	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.f, 1.f );
	drawSurface( vNrmPtr );
	glDisable( GL_POLYGON_OFFSET_FILL );

	if( lightingWasOn ) { glDisable( GL_LIGHTING ); }

	glColor( wireColor );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	drawSurface( vNrmPtr );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	if( lightingWasOn ) { glEnable( GL_LIGHTING ); }
}

ostream&
operator<<( ostream& os, const ZMesh& object )
{
	os << "<ZMesh>" << endl;
	os << " # of vertices: " << ZString::commify(object.numVertices()) << endl;
	os << " # of elements: " << ZString::commify(object.numElements()) << endl;
	os << " # of UVs     : " << ZString::commify(object.numUVs()) << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

