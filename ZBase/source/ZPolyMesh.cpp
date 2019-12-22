//---------------//
// ZPolyMesh.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.03.03                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZPolyMesh::ZPolyMesh()
{
	// nothing to do
}

ZPolyMesh::ZPolyMesh( const ZPolyMesh& other )
{
	*this = other;
}

ZPolyMesh::ZPolyMesh( const char* filePathName )
{
	load( filePathName );
}

void
ZPolyMesh::reset()
{
	vertexPositions .reset();
	polygonCounts   .reset();
	polygonConnects .reset();
	boundingBox     .reset();
	vertexNormals   .reset();
	u               .reset();
	v               .reset();
	uvIndices       .reset();
}

ZPolyMesh&
ZPolyMesh::operator=( const ZPolyMesh& other )
{
	vertexPositions = other.vertexPositions;
	polygonCounts   = other.polygonCounts;
	polygonConnects = other.polygonConnects;
	boundingBox     = other.boundingBox;
	vertexNormals   = other.vertexNormals;
	u               = other.u;
	v               = other.v;
	uvIndices       = other.uvIndices;

	return (*this);
}

void
ZPolyMesh::transform( const ZMatrix& matrix, bool useOpenMP )
{
	const int numV = numVertices();

	#pragma omp parallel for if( useOpenMP && numV>100000 )
	FOR( i, 0, numV )
	{
		ZPoint& pt = vertexPositions[i];
		pt = matrix.transform( pt, false );
	}
}

void
ZPolyMesh::append( const ZPolyMesh& other, bool uv )
{
	const int old_vCount = vertexPositions.length();
	const int startIndex = polygonConnects.length();

	vertexPositions.append( other.vertexPositions );
	polygonCounts  .append( other.polygonCounts   );
	polygonConnects.append( other.polygonConnects );
	boundingBox    .expand( other.boundingBox     );

	FOR( i, startIndex, polygonConnects.length() )
	{
		polygonConnects[i]+=old_vCount;
	}

	if( uv )
	{
		const int old_uCount = u.length();

		u        .append( other.u         );
		v        .append( other.v         );
		uvIndices.append( other.uvIndices );

		uvIndices.add( old_vCount );
	}
}

void
ZPolyMesh::computeVertexNormals()
{
	ComputeVertexNormals( vertexPositions, polygonCounts, polygonConnects, vertexNormals );
}

void
ZPolyMesh::computeBoundingBox()
{
	boundingBox = vertexPositions.boundingBox();
}

void
ZPolyMesh::reverse()
{
	ReverseConnections( polygonCounts, polygonConnects );
}

void
ZPolyMesh::convertTo( ZTriMesh& mesh )
{
	const int nVerts = vertexPositions.length();
	const int nPolys = polygonCounts.length();

	mesh.reset();

	ZPointArray& p    = mesh.p;
	ZInt3Array&  v012 = mesh.v012;

	p = vertexPositions;

	v012.reserve( nPolys * 2 );

	int idx = 0;

	FOR( i, 0, nPolys )
	{
		const int vCount_2 = polygonCounts[i] - 2;

		FOR( j, 0, vCount_2 )
		{
			const int& v0 = polygonConnects[idx    ];
			const int& v1 = polygonConnects[idx+j+1];
			const int& v2 = polygonConnects[idx+j+2];

			v012.push_back( ZInt3( v0, v1, v2 ) );
		}

		idx += polygonCounts[i];
	}

	// To Do List
	// mesh.uv
}

double
ZPolyMesh::usedMemorySize( ZDataUnit::DataUnit dataUnit ) const
{
	double ret = 0;

	ret += vertexPositions .usedMemorySize( dataUnit );
	ret += polygonCounts   .usedMemorySize( dataUnit );
	ret += polygonConnects .usedMemorySize( dataUnit );
	ret += vertexNormals   .usedMemorySize( dataUnit );
	ret += u               .usedMemorySize( dataUnit );
	ret += v               .usedMemorySize( dataUnit );
	ret += uvIndices       .usedMemorySize( dataUnit );

	return ret;
}

const ZString
ZPolyMesh::dataType() const
{
	return ZString( "ZPolyMesh" );
}

void
ZPolyMesh::write( ofstream& fout ) const
{
	vertexPositions .write( fout, true );
	polygonCounts   .write( fout, true );
	polygonConnects .write( fout, true );
	boundingBox     .write( fout       );
	vertexNormals   .write( fout, true );
	u               .write( fout, true );
	v               .write( fout, true );
	uvIndices       .write( fout, true );
}

void
ZPolyMesh::read( ifstream& fin )
{
	vertexPositions .read( fin, true );
	polygonCounts   .read( fin, true );
	polygonConnects .read( fin, true );
	boundingBox     .read( fin       );
	vertexNormals   .read( fin, true );
}

bool
ZPolyMesh::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZPolyMesh::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	dataType().write( fout, true );

	write( fout );

	fout.close();

	return true;
}

bool
ZPolyMesh::load( const char* filePathName )
{
	reset();

	ifstream fin( filePathName, ios::out|ios::binary );
	if( fin.fail() )
	{
		cout << "Error@ZPolyMesh::load(): Failed to load file." << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZPolyMesh::load(): Data type mismatch." << endl;
		reset();
		return false;
	}

	read( fin );

	fin.close();

	return true;
}

void
ZPolyMesh::drawVertices() const
{
	vertexPositions.drawPoints( false );
}

void
ZPolyMesh::drawWireframe() const
{
	const int nVerts = vertexPositions.length();
	const int nPolys = polygonCounts.length();
	if( !( nVerts && nPolys ) ) { return; }

	int startIndex = 0;
	FOR( i, 0, nPolys )
	{
		const int& count = polygonCounts[i];

		glBegin( GL_LINE_LOOP );
		{
			FOR( j, 0, count )
			{
				const int& v = polygonConnects[startIndex+j];

				glVertex( vertexPositions[v] );
			}
		}
		glEnd();

		startIndex += count;
	}
}

void
ZPolyMesh::drawSurface( bool withNormal ) const
{
	const int nVerts = vertexPositions.length();
	const int nPolys = polygonCounts.length();
	if( !( nVerts && nPolys ) ) { return; }

	if( withNormal ) {

		int startIndex = 0;

		FOR( i, 0, nPolys )
		{
			const int& count = polygonCounts[i];

			glBegin( GL_POLYGON );
			{
				FOR( j, 0, count )
				{
					const int& v = polygonConnects[startIndex+j];

					glNormal( vertexNormals[v]   );
					glVertex( vertexPositions[v] );
				}
			}
			glEnd();

			startIndex += count;
		}

	} else {

		int startIndex = 0;

		FOR( i, 0, nPolys )
		{
			const int& count = polygonCounts[i];

			glBegin( GL_POLYGON );
			{
				FOR( j, 0, count )
				{
					const int& v = polygonConnects[startIndex+j];

					glVertex( vertexPositions[v] );
				}
			}
			glEnd();

			startIndex += count;
		}

	}

//	glEnableClientState( GL_VERTEX_ARRAY );
//	glVertexPointer( 3, GL_FLOAT, 0, &vertexPositions[0].x );
//
//	glEnableClientState( GL_NORMAL_ARRAY );
//	glNormalPointer( GL_FLOAT, 0, &vertexNormals[0].x );
//
//	glDrawElemnts( GL_POLYGON, 
//
//	glDisableClientState( GL_NORMAL_ARRAY );
//	glDisableClientState( GL_VERTEX_ARRAY );
//
}

ostream&
operator<<( ostream& os, const ZPolyMesh& object )
{
	os << "<ZPolyMesh>" << endl;
	os << " # of vertices: " << ZString::commify(object.numVertices()) << endl;
	os << " # of polygons: " << ZString::commify(object.numPolygons()) << endl;
	os << " # of UVs     : " << ZString::commify(object.numUVs())      << endl;
	os << " has normal   : " << ( object.hasVertexNormals() ? "Yes" : "No" ) << endl;
	os << " has AABB     : " << ( object.hasBoundingBox() ? "Yes" : "No" ) << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END


