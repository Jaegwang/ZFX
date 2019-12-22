//--------------//
// ZTriMesh.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.11.14                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZTriMesh::ZTriMesh()
{
	// nothing to do
}

ZTriMesh::ZTriMesh( const ZTriMesh& other )
{
	*this = other;
}

ZTriMesh::ZTriMesh( const char* filePathName )
{
	load( filePathName );
}

void
ZTriMesh::reset()
{
	p    .reset();
	v012 .reset();
	uv   .reset();
}

ZTriMesh&
ZTriMesh::operator=( const ZTriMesh& other )
{
	p    = other.p;
	v012 = other.v012;
	uv   = other.uv;

	return (*this);
}

void
ZTriMesh::transform( const ZMatrix& matrix, bool useOpenMP )
{
	const int numV = ZTriMesh::numVertices();

	#pragma omp parallel for if( useOpenMP && numV>100000 )
	FOR( i, 0, numV )
	{
		ZPoint& pt = p[i];

		pt = matrix.transform( pt, false );
	}
}

void
ZTriMesh::transform( const ZMatrix& matrix, const ZPoint& pivot, bool useOpenMP )
{
	const int numV = ZTriMesh::numVertices();

	#pragma omp parallel for if( useOpenMP && numV>100000 )
	FOR( i, 0, numV )
	{
		ZPoint& pt = p[i];

		pt -= pivot;
		pt = matrix.transform( pt, false );
		pt += pivot;
	}
}

ZPoint
ZTriMesh::center( int triIdx ) const
{
	const ZPoint& p0 = p[ v012[triIdx][0] ];
	const ZPoint& p1 = p[ v012[triIdx][1] ];
	const ZPoint& p2 = p[ v012[triIdx][2] ];

	return Center( p0, p1, p2 );
}

ZBoundingBox
ZTriMesh::boundingBox( bool useOpenMP ) const
{
	return p.boundingBox( useOpenMP );
}

double
ZTriMesh::volume( bool useOpenMP ) const
{
	const int numT = ZTriMesh::numTriangles();
	if( !numT ) { return 0.0; }

	const ZPoint c = p.boundingBox( useOpenMP ).center();

	double V = 0.0;

	FOR( i, 0, numT )
	{
		const ZPoint& p0 = p[ v012[i][0] ];
		const ZPoint& p1 = p[ v012[i][1] ];
		const ZPoint& p2 = p[ v012[i][2] ];

		V += (double)Volume( c, p0, p1, p2 );
	}

	return V;
}

double
ZTriMesh::centerOfMass( ZPoint& cm, bool useOpenMP ) const
{
	const int numT = ZTriMesh::numTriangles();
	if( !numT ) { return 0.0; }

	const ZPoint c = p.boundingBox( useOpenMP ).center();

	double V = 0.0;

	FOR( i, 0, numT )
	{
		const ZPoint& p0 = p[ v012[i][0] ];
		const ZPoint& p1 = p[ v012[i][1] ];
		const ZPoint& p2 = p[ v012[i][2] ];

		const double v = (double)Volume( c, p0, p1, p2 );

		V += v;

		cm += ( Center(p0,p1,p2) *= v );
	}

	cm *= float( 1.0 / V );

	return V;
}

void
ZTriMesh::reverse()
{
	const int numT = ZTriMesh::numTriangles();
	if( !numT ) { return; }

	FOR( i, 0, numT )
	{
		ZInt3& t = v012[i];
		ZSwap( t[0], t[1] );
	}
}

void
ZTriMesh::getPositions( const ZIntArray& triIndices, const ZFloat3Array& baryCoords, ZPointArray& positions, bool useOpenMP ) const
{
	const int nPtc = triIndices.length();

	if( baryCoords.length() != nPtc )
	{
		cout << "Error@ZTriMesh::getPositions(): Invalid input data." << endl;
		positions.clear();
		return;
	}

	positions.setLength( nPtc );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nPtc )
	{
		const ZInt3& tIdx = v012[ triIndices[i] ];
		const int& v0 = tIdx[0];
		const int& v1 = tIdx[1];
		const int& v2 = tIdx[2];

		const ZPoint& p0 = p[v0];
		const ZPoint& p1 = p[v1];
		const ZPoint& p2 = p[v2];

		positions[i] = WeightedSum( p0, p1, p2, baryCoords[i] );
	}
}

void
ZTriMesh::getUVs( const ZIntArray& triIndices, const ZFloat3Array& baryCoords, ZPointArray& uvs, bool useOpenMP ) const
{
	const int nPtc = triIndices.length();

	if( baryCoords.length() != nPtc )
	{
		cout << "Error@ZTriMesh::getUVs(): Invalid input data." << endl;
		uvs.clear();
		return;
	}

	uvs.setLength( nPtc );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nPtc )
	{
		int idx = 3*triIndices[i];

		const ZPoint& uv0 = uv[  idx];
		const ZPoint& uv1 = uv[++idx];
		const ZPoint& uv2 = uv[++idx];

		uvs[i] = WeightedSum( uv0, uv1, uv2, baryCoords[i] );
	}
}

void
ZTriMesh::getNormals( const ZIntArray& triIndices, const ZFloat3Array& baryCoords, ZPointArray& normals, bool useOpenMP ) const
{
	const int nPtc = triIndices.length();

	if( baryCoords.length() != nPtc )
	{
		cout << "Error@ZTriMesh::getNormals(): Invalid input data." << endl;
		normals.clear();
		return;
	}

	normals.setLength( nPtc );

	ZVectorArray vNormals;
	ZTriMesh::getVertexNormals( vNormals, useOpenMP );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nPtc )
	{
		const ZInt3& tIdx = v012[ triIndices[i] ];
		const int& v0 = tIdx[0];
		const int& v1 = tIdx[1];
		const int& v2 = tIdx[2];

		const ZVector& n0 = vNormals[v0];
		const ZVector& n1 = vNormals[v1];
		const ZVector& n2 = vNormals[v2];

		normals[i] = WeightedSum( n0, n1, n2, baryCoords[i] );
	}
}

void
ZTriMesh::getTriangleCenters( ZPointArray& centers, bool useOpenMP ) const
{
	const int nTriangles = ZTriMesh::numTriangles();

	centers.setLength( nTriangles );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nTriangles )
	{
		const ZPoint& p0 = p[ v012[i][0] ];
		const ZPoint& p1 = p[ v012[i][1] ];
		const ZPoint& p2 = p[ v012[i][2] ];

		centers[i] = Center( p0, p1, p2 );
	}
}

void
ZTriMesh::getVertexNormals( ZVectorArray& normals, bool useOpenMP ) const
{
	const int nVertices  = ZTriMesh::numVertices();
	const int nTriangles = ZTriMesh::numTriangles();

	normals.setLength( nVertices );

	ZVector nrm;
	FOR( i, 0, nTriangles )
	{
		const ZInt3& t = v012[i];
		const int& v0=t[0];   const ZPoint& p0=p[v0];
		const int& v1=t[1];   const ZPoint& p1=p[v1];
		const int& v2=t[2];   const ZPoint& p2=p[v2];

		nrm = Area(p0,p1,p2) * Normal(p0,p1,p2);

		normals[v0] += nrm;
		normals[v1] += nrm;
		normals[v2] += nrm;
	}

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nVertices )
	{
		normals[i].normalize();
	}
}

void
ZTriMesh::getTriangleNormals( ZVectorArray& normals, bool useOpenMP ) const
{
	const int nTriangles = ZTriMesh::numTriangles();

	normals.setLength( nTriangles );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nTriangles )
	{
		const ZPoint& p0 = p[ v012[i][0] ];
		const ZPoint& p1 = p[ v012[i][1] ];
		const ZPoint& p2 = p[ v012[i][2] ];

		normals[i] = Normal( p0, p1, p2 );
	}
}

double
ZTriMesh::area( bool useOpenMP ) const
{
	const int nTriangles = ZTriMesh::numTriangles();

	double triAreaSum = 0.0;

	#pragma omp parallel for reduction( +: triAreaSum ) if( useOpenMP )
	FOR( i, 0, nTriangles )
	{
		const ZInt3& t = v012[i];
		const int& v0=t[0];   const ZPoint& p0=p[v0];
		const int& v1=t[1];   const ZPoint& p1=p[v1];
		const int& v2=t[2];   const ZPoint& p2=p[v2];

		triAreaSum += Area( p0, p1, p2 );
	}

	return triAreaSum;
}

void
ZTriMesh::getTriangleAreas( ZFloatArray& areas, bool useOpenMP ) const
{
	const int nTriangles = ZTriMesh::numTriangles();

	areas.setLength( nTriangles );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nTriangles )
	{
		const ZInt3& t = v012[i];
		const int& v0=t[0];   const ZPoint& p0=p[v0];
		const int& v1=t[1];   const ZPoint& p1=p[v1];
		const int& v2=t[2];   const ZPoint& p2=p[v2];

		areas[i] = Area( p0, p1, p2 );
	}
}

void
ZTriMesh::getMinMaxEdgeLength( float& min, float& max ) const
{
	min =  Z_LARGE;
	max = -Z_LARGE;

	const int nTri = v012.length();

	FOR( i, 0, nTri )
	{
		const int& v0 = v012[i][0];
		const int& v1 = v012[i][1];
		const int& v2 = v012[i][2];

		const ZPoint& p0 = p[v0];
		const ZPoint& p1 = p[v1];
		const ZPoint& p2 = p[v2];

		const float l0 = p0.squaredDistanceTo( p1 );
		const float l1 = p1.squaredDistanceTo( p2 );
		const float l2 = p2.squaredDistanceTo( p0 );

		min = ZMin( min, l0 );
		min = ZMin( min, l1 );
		min = ZMin( min, l2 );

		max = ZMax( max, l0 );
		max = ZMax( max, l1 );
		max = ZMax( max, l2 );
	}

	min = sqrtf( min );
	max = sqrtf( max );
}

void
ZTriMesh::getMinMaxUVEdgeLength( float& min, float& max ) const
{
	min =  Z_LARGE;
	max = -Z_LARGE;

	const int nTri = v012.length();

	FOR( i, 0, nTri )
	{
		const int v0 = i*3;
		const int v1 = v0+1;
		const int v2 = v0+2;

		const ZPoint& uv0 = uv[v0];
		const ZPoint& uv1 = uv[v1];
		const ZPoint& uv2 = uv[v2];

		const float l0 = uv0.squaredDistanceTo( uv1 );
		const float l1 = uv1.squaredDistanceTo( uv2 );
		const float l2 = uv2.squaredDistanceTo( uv0 );

		min = ZMin( min, l0 );
		min = ZMin( min, l1 );
		min = ZMin( min, l2 );

		max = ZMax( max, l0 );
		max = ZMax( max, l1 );
		max = ZMax( max, l2 );
	}

	min = sqrtf( min );
	max = sqrtf( max );
}

void
ZTriMesh::getTriangleCenterValue( const char* densityMapFilePathName, ZFloatArray& values, int channels, bool useOpenMP ) const
{
	const int nVertices  = ZTriMesh::numVertices();
	const int nTriangles = ZTriMesh::numTriangles();
	const int nUVs       = ZTriMesh::numUVs();

	if( !nVertices || !nTriangles || !nUVs )
	{
		values.clear();
		return;
	}

	values.setLength( nTriangles );

	ZImageMap densityMap;
	if( !densityMap.load( densityMapFilePathName ) )
	{
		values.fill( 1.f );
		return;
	}

	// image width / height
	const int w = densityMap.width();
	const int h = densityMap.height();

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nTriangles )
	{
		int idx = i*3;

		const ZPoint& uv0 = uv[  idx];
		const ZPoint& uv1 = uv[++idx];
		const ZPoint& uv2 = uv[++idx];

		// center of the triangle on UV-space
		const float uC = ( uv0.x + uv1.x + uv2.x ) / 3.f;
		const float vC = ( uv0.y + uv1.y + uv2.y ) / 3.f;

		if( channels < 0 ) {

			values[i] = densityMap.intensity( uC, vC );

		} else {

			values[i] = densityMap.fastValue( uC, vC, channels );

		}
	}
}

void
ZTriMesh::combine( const ZTriMesh& mesh )
{
	const int oldNumVerts = ZTriMesh::numVertices();
	const int oldNumTris  = ZTriMesh::numTriangles();

	p.append( mesh.p );
	v012.append( mesh.v012 );
	uv.append( mesh.uv ); // Is it meaningful?

	const int newNumVerts = ZTriMesh::numVertices();
	const int newNumTris  = ZTriMesh::numTriangles();

	FOR( i, oldNumTris, newNumTris )
	{
		v012[i][0] += oldNumVerts;
		v012[i][1] += oldNumVerts;
		v012[i][2] += oldNumVerts;
	}
}

void
ZTriMesh::deleteTriangles( const ZIntArray& indicesToBeDeleted )
{
	const int listSize = (int)indicesToBeDeleted.size();
	if( listSize < 1 ) { return; }

	const int numV = ZTriMesh::numVertices();
	const int numT = ZTriMesh::numTriangles();
	if( numV<3 || numT<1 ) { return; }

	ZCharArray toBeRemained( numT, (char)1 );

	int numToDel = 0;
	FOR( i, 0, listSize )
	{
		const int& iTri = indicesToBeDeleted[i];
		if( iTri<0 || iTri>=numT ) { continue; }
		toBeRemained[iTri] = (char)0;
		++numToDel;
	}

	ZPointArray newP;
	newP.reserve( toBeRemained.size()*3 );

	ZInt3Array newV012;
	newV012.reserve( toBeRemained.size() );

	int vCnt = 0;
	ZIntArray newVIdx ( numV, -1 );

	ZInt3 tmp;
	FOR( iTri, 0, numT )
	{
		if( !toBeRemained[iTri] ) { continue; }

		const ZInt3& tIdx = v012[iTri];
		int v0=tIdx[0], v1=tIdx[1], v2=tIdx[2];

		if( newVIdx[v0] < 0 ) { newP.push_back(p[v0]); v0=newVIdx[v0]=vCnt++; }
		else                  { v0=newVIdx[v0]; }

		if( newVIdx[v1] < 0 ) { newP.push_back(p[v1]); v1=newVIdx[v1]=vCnt++; }
		else                  { v1=newVIdx[v1]; }

		if( newVIdx[v2] < 0 ) { newP.push_back(p[v2]); v2=newVIdx[v2]=vCnt++; }
		else                  { v2=newVIdx[v2]; }

		tmp.set( v0, v1, v2 );
		newV012.push_back( tmp );
	}

	p.exchange( newP );
	v012.exchange( newV012 );

	if( numUVs() )
	{
		ZPointArray newUV;
		newUV.reserve( numT/2 );

		FOR( iTri, 0, numT )
		{
			if( !toBeRemained[iTri] ) { continue; }

			int uvIdx = 3*iTri;

			newUV.push_back( uv[  uvIdx] );
			newUV.push_back( uv[++uvIdx] );
			newUV.push_back( uv[++uvIdx] );
		}

		uv.exchange( newUV );
	}
}

double
ZTriMesh::usedMemorySize( ZDataUnit::DataUnit dataUnit ) const
{
	double ret = 0;

	ret += p.usedMemorySize( dataUnit );
	ret += v012.usedMemorySize( dataUnit );
	ret += uv.usedMemorySize( dataUnit );

	return ret;
}

const ZString
ZTriMesh::dataType() const
{
	return ZString( "ZTriMesh" );
}

void
ZTriMesh::write( ofstream& fout ) const
{
	p.write( fout, true );
	v012.write( fout, true );
	uv.write( fout, true );
}

void
ZTriMesh::read( ifstream& fin )
{
	p.read( fin, true );
	v012.read( fin, true );
	uv.read( fin, true );
}

bool
ZTriMesh::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZTriMesh::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	dataType().write( fout, true );
	write( fout );

	fout.close();

	return true;
}

bool
ZTriMesh::load( const char* filePathName )
{
	reset();

	ifstream fin( filePathName, ios::out|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZTriMesh::load(): Failed to load file." << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );

	if( type != dataType() )
	{
		cout << "Error@ZTriMesh::load(): Data type mismatch." << endl;
		reset();
		return false;
	}

	read( fin );

	fin.close();

	return true;
}

void
ZTriMesh::drawVertices() const
{
	p.drawPoints( false );
}

void
ZTriMesh::drawWireframe() const
{
	const int numV = ZTriMesh::numVertices();
	const int numT = ZTriMesh::numTriangles();
	if( !( numV && numT ) ) { return; }

	glBegin( GL_LINES );
	{
		FOR( i, 0, numT )
		{
			const ZInt3& t = v012[i];
			glVertex( p[ t[0] ] );   glVertex( p[ t[1] ] );
			glVertex( p[ t[1] ] );   glVertex( p[ t[2] ] );
			glVertex( p[ t[2] ] );   glVertex( p[ t[0] ] );
		}
	}
	glEnd();
}

void
ZTriMesh::drawSurface( bool withNormal ) const
{
	const int numV = ZTriMesh::numVertices();
	const int numT = ZTriMesh::numTriangles();
	if( !( numV && numT ) ) { return; }

	if( withNormal ) {

		glBegin( GL_TRIANGLES );
		{
			FOR( i, 0, numT )
			{
				const ZInt3& t = v012[i];

				const ZPoint& p0 = p[ t[0] ];
				const ZPoint& p1 = p[ t[1] ];
				const ZPoint& p2 = p[ t[2] ];

				const ZVector n( Normal( p0, p1, p2 ) );

				glNormal( n ); glVertex( p0 );
				glNormal( n ); glVertex( p1 );
				glNormal( n ); glVertex( p2 );
			}
		}
		glEnd();

	} else {

		glBegin( GL_TRIANGLES );
		{
			FOR( i, 0, numT )
			{
				const ZInt3& t = v012[i];

				const ZPoint& p0 = p[ t[0] ];
				const ZPoint& p1 = p[ t[1] ];
				const ZPoint& p2 = p[ t[2] ];

				const ZVector n( Normal( p0, p1, p2 ) );

				glVertex( p0 );
				glVertex( p1 );
				glVertex( p2 );
			}
		}
		glEnd();

	}
}

void
ZTriMesh::draw( ZMeshDisplayMode::MeshDisplayMode mode, const ZColor& lineColor, const ZColor& surfaceColor, float opacity ) const
{
	const int numV = ZTriMesh::numVertices();
	const int numT = ZTriMesh::numTriangles();
	if( !( numV && numT ) ) { return; }

	if( mode == ZMeshDisplayMode::zPoints ) // as p cloud
	{
		glColor( lineColor, opacity );
		drawVertices();
		return;
	}

	if( ( mode == ZMeshDisplayMode::zSurface ) || ( mode == ZMeshDisplayMode::zWireSurface ) )
	{
		glColor( surfaceColor, opacity );
		drawSurface();
	}

	if( ( mode == ZMeshDisplayMode::zWireframe ) || ( mode == ZMeshDisplayMode::zWireSurface ) )
	{
		glEnable( GL_POLYGON_OFFSET_LINE );
		glPolygonOffset( -1, -1 );

		glColor( lineColor, opacity );
		drawWireframe();
	}
}

void
ZTriMesh::draw( const ZVectorArray& vNrm ) const
{
	const int numV = ZTriMesh::numVertices();
	const int numT = ZTriMesh::numTriangles();
	if( !( numV && numT ) ) { return; }
	
	if( vNrm.length() != numV )
	{
		cout << "Error@ZTriMesh::draw(): Invalid array length." << endl;
		return;
	}

	glBegin( GL_TRIANGLES );
	FOR( i, 0, numT )
	{
		const ZInt3& t = v012[i];
		glNormal(vNrm[t[0]]); glVertex(p[t[0]]);
		glNormal(vNrm[t[1]]); glVertex(p[t[1]]);
		glNormal(vNrm[t[2]]); glVertex(p[t[2]]);
	}
	glEnd();
}

void
ZTriMesh::drawVertexNormals( const ZVectorArray& vNrm, float scale ) const
{
	const int numV = ZTriMesh::numVertices();
	if( !numV ) { return; }

	if( vNrm.length() != numV )
	{
		cout << "Error@ZTriMesh::draw(): Invalid array length." << endl;
		return;
	}

	glBegin( GL_LINES );
	FOR( i, 0, numV )
	{
			glVertex(p[i]);
			glVertex(vNrm[i]*scale);
	}
	glEnd();
}

void
ZTriMesh::drawUVs() const
{
	const int numT  = ZTriMesh::numTriangles();
	const int numUV = ZTriMesh::numUVs();
	if( !( numT && numUV ) ) { return; }

	FOR( i, 0, numT )
	{
		int idx = 3*i;
		glBegin( GL_LINE_LOOP );
			glVertex( uv[  idx] );
			glVertex( uv[++idx] );
			glVertex( uv[++idx] );
		glEnd();
	}
}

void
ZTriMesh::drawWithTexture( bool flipInY ) const
{
	const int numT  = ZTriMesh::numTriangles();
	const int numUV = ZTriMesh::numUVs();
	if( !( numT && numUV ) ) { return; }

	if( flipInY ) {

		glBegin( GL_TRIANGLES );
		{
			FOR( i, 0, numT )
			{
				int idx = 3*i;

				const ZPoint& uv0 = uv[  idx]; 
				const ZPoint& uv1 = uv[++idx]; 
				const ZPoint& uv2 = uv[++idx]; 

				const ZInt3& t = v012[i];

				glTexCoord2f( uv0.x, 1.f-uv0.y ); glVertex( p[t[0]] );
				glTexCoord2f( uv1.x, 1.f-uv1.y ); glVertex( p[t[1]] );
				glTexCoord2f( uv2.x, 1.f-uv2.y ); glVertex( p[t[2]] );
			}
		}
		glEnd();

	} else {

		glBegin( GL_TRIANGLES );
		{
			FOR( i, 0, numT )
			{
				int idx = 3*i;

				const ZPoint& uv0 = uv[  idx]; 
				const ZPoint& uv1 = uv[++idx]; 
				const ZPoint& uv2 = uv[++idx]; 

				const ZInt3& t = v012[i];

				glTexCoord2f( uv0.x, uv0.y ); glVertex( p[t[0]] );
				glTexCoord2f( uv1.x, uv1.y ); glVertex( p[t[1]] );
				glTexCoord2f( uv2.x, uv2.y ); glVertex( p[t[2]] );
			}
		}
		glEnd();

	}
}

ostream&
operator<<( ostream& os, const ZTriMesh& object )
{
	os << "<ZTriMesh>" << endl;
	os << " # of vertices: " << ZString::commify( object.numVertices() ) << endl;
	os << " # of trifaces: " << ZString::commify( object.numTriangles() ) << endl;
	os << " # of UVs     : " << ZString::commify( object.numUVs() ) << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

