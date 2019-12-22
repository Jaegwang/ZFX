//---------------//
// ZQuadMesh.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.11.04                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZQuadMesh::ZQuadMesh()
{
	// nothing to do
}

ZQuadMesh::ZQuadMesh( const ZQuadMesh& other )
{
	*this = other;
}

ZQuadMesh::ZQuadMesh( const char* filePathName )
{
	load( filePathName );
}

void
ZQuadMesh::reset()
{
	p     .reset();
	v0123 .reset();
	uv    .reset();
}

ZQuadMesh&
ZQuadMesh::operator=( const ZQuadMesh& other )
{
	p     = other.p;
	v0123 = other.v0123;
	uv    = other.uv;

	return (*this);
}

void
ZQuadMesh::transform( const ZMatrix& matrix, bool useOpenMP )
{
	const int numV = numVertices();

	#pragma omp parallel for if( useOpenMP && numV>100000 )
	FOR( i, 0, numV )
	{
		ZPoint& pt = p[i];
		pt = matrix.transform( pt, false );
	}
}

ZBoundingBox
ZQuadMesh::boundingBox() const
{
	return p.boundingBox();
}

void
ZQuadMesh::reverse()
{
	const int numQ = numQuads();

	FOR( i, 0, numQ )
	{
		ZInt4& t = v0123[i];
		ZSwap( t[1], t[3] );
	}
}

void
ZQuadMesh::getVertexNormals( ZVectorArray& normals ) const
{
	const int nVertices = numVertices();
	const int nQuads    = numQuads();

	normals.setLength( nVertices );

	ZVector nrm;
	FOR( i, 0, nQuads )
	{
		const ZInt4& q = v0123[i];
		const int& v0=q[0];   const ZPoint& p0=p[v0];
		const int& v1=q[1];   const ZPoint& p1=p[v1];
		const int& v2=q[2];   const ZPoint& p2=p[v2];

		nrm = Area(p0,p1,p2) * Normal(p0,p1,p2);

		normals[v0] += nrm;
		normals[v1] += nrm;
		normals[v2] += nrm;
	}

	FOR( i, 0, nVertices )
	{
		normals[i].normalize();
	}
}

void
ZQuadMesh::combine( const ZQuadMesh& mesh )
{
	const int oldNumVerts = numVertices();
	const int oldNumQuads = numQuads();

	p.append( mesh.p );
	v0123.append( mesh.v0123 );
	uv.append( mesh.uv ); // Is it meaningful?

	const int newNumVerts = numVertices();
	const int newNumQuads = numQuads();

	FOR( i, oldNumQuads, newNumQuads )
	{
		v0123[i][0] += oldNumVerts;
		v0123[i][1] += oldNumVerts;
		v0123[i][2] += oldNumVerts;
		v0123[i][3] += oldNumVerts;
	}
}

void
ZQuadMesh::getTriangleIndices( ZInt3Array& triangles ) const
{
	triangles.clear();
	const int nQ = numQuads();

	triangles.reserve( 2*nQ );

	ZInt3 idx;

	FOR( i, 0, nQ )
	{
		const ZInt4& quad = v0123[i];

		FOR( j, 0, 2 )
		{
			idx[0] = quad[0];
			idx[1] = quad[j+1];
			idx[2] = quad[j+2];

			triangles.push_back( idx );
		}
	}
}

double
ZQuadMesh::usedMemorySize( ZDataUnit::DataUnit dataUnit ) const
{
	double ret = 0;

	ret += p.usedMemorySize( dataUnit );
	ret += v0123.usedMemorySize( dataUnit );
	ret += uv.usedMemorySize( dataUnit );

	return ret;
}

const ZString
ZQuadMesh::dataType() const
{
	return ZString( "ZQuadMesh" );
}

void
ZQuadMesh::write( ofstream& fout ) const
{
	p.write( fout, true );
	v0123.write( fout, true );
	uv.write( fout, true );
}

void
ZQuadMesh::read( ifstream& fin )
{
	p.read( fin, true );
	v0123.read( fin, true );
	uv.read( fin, true );
}

bool
ZQuadMesh::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZQuadMesh::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	dataType().write( fout, true );

	write( fout );

	fout.close();

	return true;
}

bool
ZQuadMesh::load( const char* filePathName )
{
	reset();

	ifstream fin( filePathName, ios::out|ios::binary );
	if( fin.fail() )
	{
		cout << "Error@ZQuadMesh::load(): Failed to load file." << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZQuadMesh::load(): Data type mismatch." << endl;
		reset();
		return false;
	}

	read( fin );

	fin.close();

	return true;
}

void
ZQuadMesh::drawVertices() const
{
	p.drawPoints( false );
}

void
ZQuadMesh::drawWireframe() const
{
	const int numV = numVertices();
	const int numQ = numQuads();
	if( !( numV && numQ ) ) { return; }

	glBegin( GL_LINES );
	{
		FOR( i, 0, numQ )
		{
			const ZInt4& q = v0123[i];
			glVertex( p[ q[0] ] );   glVertex( p[ q[1] ] );
			glVertex( p[ q[1] ] );   glVertex( p[ q[2] ] );
			glVertex( p[ q[2] ] );   glVertex( p[ q[3] ] );
			glVertex( p[ q[3] ] );   glVertex( p[ q[0] ] );
		}
	}
	glEnd();
}

void
ZQuadMesh::drawSurface( bool withNormal ) const
{
	const int numV = numVertices();
	const int numQ = numQuads();
	if( !( numV && numQ ) ) { return; }

	if( withNormal ) {

		glBegin( GL_QUADS );
		{
			FOR( i, 0, numQ )
			{
				const ZInt4&  q = v0123[i];

				const ZPoint& p0 = p[ q[0] ];
				const ZPoint& p1 = p[ q[1] ];
				const ZPoint& p2 = p[ q[2] ];
				const ZPoint& p3 = p[ q[3] ];

				const ZVector n( Normal(p0,p1,p2) );

				glNormal( n ); glVertex( p0 );
				glNormal( n ); glVertex( p1 );
				glNormal( n ); glVertex( p2 );
				glNormal( n ); glVertex( p3 );
			}
		}
		glEnd();

	} else {

		glBegin( GL_QUADS );
		{
			FOR( i, 0, numQ )
			{
				const ZInt4&  q = v0123[i];

				const ZPoint& p0 = p[ q[0] ];
				const ZPoint& p1 = p[ q[1] ];
				const ZPoint& p2 = p[ q[2] ];
				const ZPoint& p3 = p[ q[3] ];

				const ZVector n( Normal(p0,p1,p2) );

				glVertex( p0 );
				glVertex( p1 );
				glVertex( p2 );
				glVertex( p3 );
			}
		}
		glEnd();

	}
}

void
ZQuadMesh::draw( ZMeshDisplayMode::MeshDisplayMode mode, const ZColor& lineColor, const ZColor& surfaceColor, float opacity ) const
{
	const int numV = numVertices();
	const int numQ = numQuads();
	if( !( numV && numQ ) ) { return; }

	if( mode == ZMeshDisplayMode::zPoints ) // as p cloud
	{
		glColor( lineColor, opacity );
		glBegin( GL_POINTS );
			FOR( i, 0, numV ) { glVertex( p[i] ); }
		glEnd();

		return;
	}

	if( ( mode == ZMeshDisplayMode::zSurface ) || ( mode == ZMeshDisplayMode::zWireSurface ) ) // as solid surface
	{
		glColor( surfaceColor, opacity );
		FOR( i, 0, numQ )
		{
			const ZInt4& q = v0123[i];
			glBegin( GL_QUADS );
				glVertex( p[ q[0] ] );
				glVertex( p[ q[1] ] );
				glVertex( p[ q[2] ] );
				glVertex( p[ q[3] ] );
			glEnd();
		}
	}

	if( ( mode == ZMeshDisplayMode::zWireframe ) || ( mode == ZMeshDisplayMode::zWireSurface ) ) // as solid surface
	{
		glEnable( GL_POLYGON_OFFSET_LINE );
		glPolygonOffset( -1, -1 );

		glColor( lineColor, opacity );
		FOR( i, 0, numQ )
		{
			const ZInt4& q = v0123[i];
			glBegin( GL_LINE_LOOP );
				glVertex( p[ q[0] ] );
				glVertex( p[ q[1] ] );
				glVertex( p[ q[2] ] );
			glEnd();
		}
	}
}

void
ZQuadMesh::drawVertexNormals( const ZVectorArray& vNrm, float scale ) const
{
	const int numV = numVertices();

	if( vNrm.length() != numV )
	{
		cout << "Error@ZQuadMesh::draw(): Invalid array length." << endl;
		return;
	}

	FOR( i, 0, numV )
	{
		glBegin( GL_LINES );
			glVertex(p[i]);
			glVertex(vNrm[i]*scale);
		glEnd();
	}
}

void
ZQuadMesh::drawUVs() const
{
	const int numT  = numQuads();
	const int numUV = numUVs();
	if( !( numT && numUV ) ) { return; }

	FOR( i, 0, numT )
	{
		int idx = 4*i;
		glBegin( GL_LINE_LOOP );
			glVertex( uv[  idx] );
			glVertex( uv[++idx] );
			glVertex( uv[++idx] );
			glVertex( uv[++idx] );
		glEnd();
	}
}

ostream&
operator<<( ostream& os, const ZQuadMesh& object )
{
	os << "<ZQuadMesh>" << endl;
	os << " # of vertices : " << ZString::commify( object.numVertices() ) << endl;
	os << " # of quadfaces: " << ZString::commify( object.numQuads() ) << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

