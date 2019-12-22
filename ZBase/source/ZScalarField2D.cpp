//--------------------//
// ZScalarField2D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZScalarField2D::ZScalarField2D()
: ZField2DBase(), ZFloatArray()
{
	minValue = maxValue = 0.f;
}

ZScalarField2D::ZScalarField2D( const ZScalarField2D& f )
: ZField2DBase(f), ZFloatArray(f)
{
	minValue = maxValue = 0.f;
}

ZScalarField2D::ZScalarField2D( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
: ZField2DBase(), ZFloatArray()
{
	set( grid, loc );
}

ZScalarField2D::ZScalarField2D( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
: ZField2DBase(), ZFloatArray()
{
	set( Nx, Nz, loc );
}

ZScalarField2D::ZScalarField2D( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Nz, Lx, Lz, loc );
}

ZScalarField2D::ZScalarField2D( const char* filePathName )
: ZField2DBase(), ZFloatArray()
{
	load( filePathName );
}

void
ZScalarField2D::set( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( grid, loc );
	ZFloatArray::setLength( ZField2DBase::numElements() );
	minValue = maxValue = 0.f;
}

void
ZScalarField2D::set( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( Nx, Nz, loc );
	ZFloatArray::setLength( ZField2DBase::numElements() );
	minValue = maxValue = 0.f;
}

void
ZScalarField2D::set( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( Nx, Nz, Lx, Lz, loc );
	ZFloatArray::setLength( ZField2DBase::numElements() );
	minValue = maxValue = 0.f;
}

void
ZScalarField2D::reset()
{
	ZField2DBase::reset();
	ZFloatArray::reset();
	minValue = maxValue = 0.f;
}

ZScalarField2D&
ZScalarField2D::operator=( const ZScalarField2D& f )
{
	ZGrid2D::operator=( f );
	ZFloatArray::operator=( f );
	minValue = f.minValue;
	maxValue = f.maxValue;
	return (*this);
}

void
ZScalarField2D::fill( float v )
{
	ZFloatArray::fill( v );
}

bool
ZScalarField2D::exchange( const ZScalarField2D& f )
{
	if( ZGrid2D::operator!=(f) ) { return false; }
	ZFloatArray::exchange( *(ZFloatArray*)(&f) );
	return true;
}

float
ZScalarField2D::min( bool useOpenMP ) const
{
	return ZFloatArray::min( useOpenMP );
}

float
ZScalarField2D::max( bool useOpenMP ) const
{
	return ZFloatArray::max( useOpenMP );
}

float
ZScalarField2D::absMax( bool useOpenMP ) const
{
	return ZFloatArray::absMax( useOpenMP );
}

void
ZScalarField2D::setMinMax( bool useOpenMP )
{
	ZFloatArray::getMinMax( minValue, maxValue, useOpenMP );
}

const ZString
ZScalarField2D::dataType() const
{
	return ZString( "ZScalarField2D" );
}

bool
ZScalarField2D::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZScalarField2D::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	ZField2DBase::write( fout );
	ZFloatArray::write( fout, true );

	fout.write( (char*)&minValue, sizeof(float) );
	fout.write( (char*)&maxValue, sizeof(float) );

	fout.close();

	return true;
}

bool
ZScalarField2D::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		reset();
		cout << "Error@ZScalarField2D::load(): Failed to load file " << filePathName << endl;
		return false;
	}

	ZField2DBase::read( fin );
	ZFloatArray::read( fin, true );

	fin.read( (char*)&minValue, sizeof(float) );
	fin.read( (char*)&maxValue, sizeof(float) );

	fin.close();

	return true;
}

void
ZScalarField2D::setNoise( const ZSimplexNoise& noise, float time, bool useOpenMP )
{
	float* _data = (float*)ZFloatArray::pointer();

	#pragma omp parallel for if( useOpenMP )
	for( int k=0; k<=_kMax; ++k )
	for( int i=0; i<=_iMax; ++i )
	{{
		const int idx = ZField2DBase::index(i,k);
		const ZPoint p( this->position(i,k) );
		_data[idx] = noise.value( p.x, p.y, p.z, time );
	}}
}

void
ZScalarField2D::drawHeightMesh( ZMeshDisplayMode::MeshDisplayMode mode, const ZColor& lineColor, const ZColor& surfaceColor ) const
{
	const int numElems = numElements();
	if( numElems<1 ) { return; }

	int idx;
	ZPoint p;
	const ZScalarField2D& h = *this;

	if( mode == ZMeshDisplayMode::zPoints ) // as p cloud
	{
		glColor( lineColor );
		glBegin( GL_POINTS );
			for( int k=0; k<=_kMax; ++k )
			for( int i=0; i<=_iMax; ++i )
			{{
				p = this->position(i,k);   glVertex( p.x, h(i,k), p.z );
			}}
		glEnd();

		return;
	}

	if( ( mode == ZMeshDisplayMode::zSurface ) || ( mode == ZMeshDisplayMode::zWireSurface ) ) // as solid surface
	{
		glColor( surfaceColor );
		for( int k=0; k<_kMax; ++k )
		for( int i=0; i<_iMax; ++i )
		{{
			glBegin( GL_QUADS );
				p = this->position(i  ,k  );   glVertex( p.x, h(i  ,k  ), p.z );
				p = this->position(i+1,k  );   glVertex( p.x, h(i+1,k  ), p.z );
				p = this->position(i+1,k+1);   glVertex( p.x, h(i+1,k+1), p.z );
				p = this->position(i  ,k+1);   glVertex( p.x, h(i  ,k+1), p.z );
			glEnd();
		}}
	}

	if( ( mode == ZMeshDisplayMode::zWireframe ) || ( mode == ZMeshDisplayMode::zWireSurface ) ) // as wireframe
	{
		glEnable( GL_POLYGON_OFFSET_LINE );
		glPolygonOffset( -1, -1 );

		glColor( lineColor );
		for( int k=0; k<_kMax; ++k )
		for( int i=0; i<_iMax; ++i )
		{{
			glBegin( GL_LINE_LOOP );
				p = this->position(i  ,k  );   glVertex( p.x, h(i  ,k  ), p.z );
				p = this->position(i+1,k  );   glVertex( p.x, h(i+1,k  ), p.z );
				p = this->position(i+1,k+1);   glVertex( p.x, h(i+1,k+1), p.z );
				p = this->position(i  ,k+1);   glVertex( p.x, h(i  ,k+1), p.z );
			glEnd();
		}}
	}
}

void
ZScalarField2D::draw( bool smoothPosArea, const ZColor& farPos, const ZColor& nearPos,
                      bool smoothNegArea, const ZColor& farNeg, const ZColor& nearNeg,
                      float elementSize ) const
{
	const int numElems = numElements();
	if( numElems<1 ) { return; }

	const float h = ZClamp( elementSize, 0.f, 1.f );

	const float hDx = 0.5f*h*_dx;
	const float hDz = 0.5f*h*_dz;

	ZPoint p;

	glBegin( GL_QUADS );

	for( int k=0; k<_kMax; ++k )
	for( int i=0; i<_iMax; ++i )
	{{
		const float& v = this->operator()(i,k);

		const float  a = v / ( minValue + Z_EPS );
		const float _a = 1 - a;
		const float  b = v / ( maxValue + Z_EPS );
		const float _b = 1 - b;

		if( v<0 ) {
			if( smoothNegArea ) { glColor( a*farNeg + _a*nearNeg ); }
			else                { glColor( farNeg ); }
		} else {
			if( smoothPosArea ) { glColor( b*farPos + _b*nearPos ); }
			else                { glColor( farPos ); }
		}

		p = ZField2DBase::position(i,k);

		glVertex( p.x-hDx, _y, p.z-hDz );
		glVertex( p.x+hDx, _y, p.z-hDz );
		glVertex( p.x+hDx, _y, p.z+hDz );
		glVertex( p.x-hDx, _y, p.z+hDz );
	}}

	glEnd();
}

ostream& operator<<( ostream& os, const ZScalarField2D& object )
{
	os << "<ZScalarField2D>" << endl;
	os << " resolution : " << object.nx() << " x " << object.nz() << endl;
	os << " dimension  : " << object.lx() << " x " << object.lz() << endl;
	os << " cell size  : " << object.dx() << " x " << object.dz() << endl;
	os << " domain     : " << object.minPoint() << " ~ " << object.maxPoint() << endl;
	os << " location   : " << ZFieldLocation::name(object.location()) << endl;
	os << " memory size: " << object.usedMemorySize(ZDataUnit::zMegabytes) << " mb." << endl;
	os << " range      : " << object.minValue << " ~ " << object.maxValue << endl; 
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

