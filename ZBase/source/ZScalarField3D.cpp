//--------------------//
// ZScalarField3D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZScalarField3D::ZScalarField3D()
: ZField3DBase(), ZFloatArray()
{
	minValue = maxValue = 0.f;
}

ZScalarField3D::ZScalarField3D( const ZScalarField3D& f )
: ZField3DBase(f), ZFloatArray(f)
{
	minValue = maxValue = 0.f;
}

ZScalarField3D::ZScalarField3D( const ZGrid3D& grid, ZFieldLocation::FieldLocation loc )
: ZField3DBase(), ZFloatArray()
{
	set( grid, loc );
}

ZScalarField3D::ZScalarField3D( int Nx, int Ny, int Nz, ZFieldLocation::FieldLocation loc )
: ZField3DBase(), ZFloatArray()
{
	set( Nx, Ny, Nz, loc );
}

ZScalarField3D::ZScalarField3D( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Ny, Nz, Lx, Ly, Lz, loc );
}

ZScalarField3D::ZScalarField3D( const char* filePathName )
: ZField3DBase(), ZFloatArray()
{
	load( filePathName );
}

void
ZScalarField3D::set( const ZGrid3D& grid, ZFieldLocation::FieldLocation loc )
{
	ZField3DBase::set( grid, loc );
	ZFloatArray::setLength( ZField3DBase::numElements() );
	minValue = maxValue = 0.f;
}

void
ZScalarField3D::set( int Nx, int Ny, int Nz, ZFieldLocation::FieldLocation loc )
{
	ZField3DBase::set( Nx, Ny, Nz, loc );
	ZFloatArray::setLength( ZField3DBase::numElements() );
	minValue = maxValue = 0.f;
}

void
ZScalarField3D::set( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz, ZFieldLocation::FieldLocation loc )
{
	ZField3DBase::set( Nx, Ny, Nz, Lx, Ly, Lz, loc );
	ZFloatArray::setLength( ZField3DBase::numElements() );
	minValue = maxValue = 0.f;
}

void
ZScalarField3D::reset()
{
	ZField3DBase::reset();
	ZFloatArray::reset();
	minValue = maxValue = 0.f;
}

ZScalarField3D&
ZScalarField3D::operator=( const ZScalarField3D& f )
{
	ZField3DBase::operator=( f );
	ZFloatArray::operator=( f );
	minValue = f.minValue;
	maxValue = f.maxValue;
	return (*this);
}

void
ZScalarField3D::fill( float v )
{
	ZFloatArray::fill( v );
}

bool
ZScalarField3D::exchange( const ZScalarField3D& f )
{
	if( ZGrid3D::operator!=(f) ) { return false; }
	ZFloatArray::exchange( *(ZFloatArray*)(&f) );
	return true;
}

float
ZScalarField3D::min( bool useOpenMP ) const
{
	return ZFloatArray::min( useOpenMP );
}

float
ZScalarField3D::max( bool useOpenMP ) const
{
	return ZFloatArray::max( useOpenMP );
}

void
ZScalarField3D::setMinMax( bool useOpenMP )
{
	ZFloatArray::getMinMax( minValue, maxValue, useOpenMP );
}

float
ZScalarField3D::absMax( bool useOpenMP ) const
{
	return ZFloatArray::absMax( useOpenMP );
}

const ZString
ZScalarField3D::dataType() const
{
	return ZString( "ZScalarField3D" );
}

bool
ZScalarField3D::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZScalarField3D::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	ZScalarField3D::dataType().write( fout, true );
	ZField3DBase::write( fout );
	ZFloatArray::write( fout, true );
	fout.write( (char*)&minValue, sizeof(float) );
	fout.write( (char*)&maxValue, sizeof(float) );
	fout.close();
	return true;
}

bool
ZScalarField3D::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		reset();
		cout << "Error@ZScalarField3D::save(): Failed to save file " << filePathName << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZScalarField3D::load(): Data type mismatch." << endl;
		reset();
		return false;
	}
	ZField3DBase::read( fin );
	ZFloatArray::read( fin, true );
	fin.read( (char*)&minValue, sizeof(float) );
	fin.read( (char*)&maxValue, sizeof(float) );
	fin.close();
	return true;
}

void
ZScalarField3D::drawSlice( const ZInt3& whichSlice, const ZFloat3& sliceRatio,
                           bool smoothPosArea, const ZColor& farPos, const ZColor& nearPos,
                           bool smoothNegArea, const ZColor& farNeg, const ZColor& nearNeg,
                           float elementSize ) const
{
	if( whichSlice[0] )
	{
		_drawXSlice( ZClamp( _iMax*sliceRatio[0], 0, _iMax ),
                     smoothPosArea, farPos, nearPos,
                     smoothNegArea, farNeg, nearNeg,
                     elementSize );
	}

	if( whichSlice[1] )
	{
		_drawYSlice( ZClamp( _jMax*sliceRatio[1], 0, _jMax ),
                     smoothPosArea, farPos, nearPos,
                     smoothNegArea, farNeg, nearNeg,
                     elementSize );
	}

	if( whichSlice[2] )
	{
		_drawZSlice( ZClamp( _kMax*sliceRatio[2], 0, _kMax ),
                     smoothPosArea, farPos, nearPos,
                     smoothNegArea, farNeg, nearNeg,
                     elementSize );
	}
}

void
ZScalarField3D::_drawXSlice( int i,
                             bool smoothPosArea, const ZColor& farPos, const ZColor& nearPos,
                             bool smoothNegArea, const ZColor& farNeg, const ZColor& nearNeg,
                             float elementSize ) const
{
	const int numElems = numElements();
	if( numElems<1 ) { return; }

	const float h = ZClamp( elementSize, 0.f, 1.f );

	const float hDx = 0.5f*h*_dx;
	const float hDy = 0.5f*h*_dy;
	const float hDz = 0.5f*h*_dz;

	ZPoint p;

	glBegin( GL_QUADS );

		FOR( k, 0, _kMax+1 )
		FOR( j, 0, _jMax+1 )
		{{
			const float& v = this->operator()(i,j,k);

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

			p = ZField3DBase::position(i,j,k);

			glVertex( p.x, p.y-hDy, p.z-hDz );
			glVertex( p.x, p.y+hDy, p.z-hDz );
			glVertex( p.x, p.y+hDy, p.z+hDz );
			glVertex( p.x, p.y-hDy, p.z+hDz );
		}}

	glEnd();
}

void
ZScalarField3D::_drawYSlice( int j,
                             bool smoothPosArea, const ZColor& farPos, const ZColor& nearPos,
                             bool smoothNegArea, const ZColor& farNeg, const ZColor& nearNeg,
                             float elementSize ) const
{
	const int numElems = numElements();
	if( numElems<1 ) { return; }

	const float h = ZClamp( elementSize, 0.f, 1.f );

	const float hDx = 0.5f*h*_dx;
	const float hDy = 0.5f*h*_dy;
	const float hDz = 0.5f*h*_dz;

	ZPoint p;

	glBegin( GL_QUADS );

		FOR( k, 0, _kMax+1 )
		FOR( i, 0, _iMax+1 )
		{{
			const float& v = this->operator()(i,j,k);

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

			p = this->position(i,j,k);

			glVertex( p.x-hDx, p.y, p.z-hDz );
			glVertex( p.x+hDx, p.y, p.z-hDz );
			glVertex( p.x+hDx, p.y, p.z+hDz );
			glVertex( p.x-hDx, p.y, p.z+hDz );
		}}

	glEnd();
}

void
ZScalarField3D::_drawZSlice( int k,
                             bool smoothPosArea, const ZColor& farPos, const ZColor& nearPos,
                             bool smoothNegArea, const ZColor& farNeg, const ZColor& nearNeg,
                             float elementSize ) const
{
	const int numElems = numElements();
	if( numElems<1 ) { return; }

	const float h = ZClamp( elementSize, 0.f, 1.f );

	const float hDx = 0.5f*h*_dx;
	const float hDy = 0.5f*h*_dy;
	const float hDz = 0.5f*h*_dz;

	ZPoint p;

	glBegin( GL_QUADS );

		FOR( j, 0, _jMax+1 )
		FOR( i, 0, _iMax+1 )
		{{
			const float& v = this->operator()(i,j,k);

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

			p = this->position(i,j,k);

			glVertex( p.x-hDx, p.y-hDy, p.z );
			glVertex( p.x+hDx, p.y-hDy, p.z );
			glVertex( p.x+hDx, p.y+hDy, p.z );
			glVertex( p.x-hDx, p.y+hDy, p.z );
		}}

	glEnd();
}

void
ZScalarField3D::drawSliceRangeOnly( const ZInt3& whichSlice, const ZFloat3& sliceRatio,
                         			bool smoothPosArea, const ZColor& farPos, const ZColor& nearPos,
		                            bool smoothNegArea, const ZColor& farNeg, const ZColor& nearNeg,
         			                float elementSize,
									float dispMin, float dispMax ) const
{
	if( whichSlice[0] )
	{
		_drawXSliceRangeOnly( ZClamp( _iMax*sliceRatio[0], 0, _iMax ),
                  			  smoothPosArea, farPos, nearPos,
		                      smoothNegArea, farNeg, nearNeg,
         		              elementSize, dispMin, dispMax );
	}

	if( whichSlice[1] )
	{
		_drawYSliceRangeOnly( ZClamp( _jMax*sliceRatio[1], 0, _jMax ),
                  			  smoothPosArea, farPos, nearPos,
		                      smoothNegArea, farNeg, nearNeg,
        		              elementSize, dispMin, dispMax );
	}

	if( whichSlice[2] )
	{
		_drawZSliceRangeOnly( ZClamp( _kMax*sliceRatio[2], 0, _kMax ),
               			      smoothPosArea, farPos, nearPos,
                     		  smoothNegArea, farNeg, nearNeg,
		                      elementSize, dispMin, dispMax );
	}
}

void
ZScalarField3D::_drawXSliceRangeOnly( int i,
                             		  bool smoothPosArea, const ZColor& farPos, const ZColor& nearPos,
		                              bool smoothNegArea, const ZColor& farNeg, const ZColor& nearNeg,
        		                      float elementSize,
									  float dispMin, float dispMax ) const
{
	const int numElems = numElements();
	if( numElems<1 ) { return; }

	const float h = ZClamp( elementSize, 0.f, 1.f );

	const float hDx = 0.5f*h*_dx;
	const float hDy = 0.5f*h*_dy;
	const float hDz = 0.5f*h*_dz;

	ZPoint p;

	glBegin( GL_QUADS );

		FOR( k, 0, _kMax+1 )
		FOR( j, 0, _jMax+1 )
		{{
			const float& v = this->operator()(i,j,k);
			if( v < dispMin || v > dispMax ) { continue; }

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

			p = ZField3DBase::position(i,j,k);

			glVertex( p.x, p.y-hDy, p.z-hDz );
			glVertex( p.x, p.y+hDy, p.z-hDz );
			glVertex( p.x, p.y+hDy, p.z+hDz );
			glVertex( p.x, p.y-hDy, p.z+hDz );
		}}

	glEnd();
}

void
ZScalarField3D::_drawYSliceRangeOnly( int j,
                             		  bool smoothPosArea, const ZColor& farPos, const ZColor& nearPos,
		                              bool smoothNegArea, const ZColor& farNeg, const ZColor& nearNeg,
         		                      float elementSize,
						 		  	  float dispMin, float dispMax ) const
{
	const int numElems = numElements();
	if( numElems<1 ) { return; }

	const float h = ZClamp( elementSize, 0.f, 1.f );

	const float hDx = 0.5f*h*_dx;
	const float hDy = 0.5f*h*_dy;
	const float hDz = 0.5f*h*_dz;

	ZPoint p;

	glBegin( GL_QUADS );

		FOR( k, 0, _kMax+1 )
		FOR( i, 0, _iMax+1 )
		{{
			const float& v = this->operator()(i,j,k);
			if( v < dispMin || v > dispMax ) { continue; }

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

			p = this->position(i,j,k);

			glVertex( p.x-hDx, p.y, p.z-hDz );
			glVertex( p.x+hDx, p.y, p.z-hDz );
			glVertex( p.x+hDx, p.y, p.z+hDz );
			glVertex( p.x-hDx, p.y, p.z+hDz );
		}}

	glEnd();
}

void
ZScalarField3D::_drawZSliceRangeOnly( int k,
	    	                          bool smoothPosArea, const ZColor& farPos, const ZColor& nearPos,
    	    	                      bool smoothNegArea, const ZColor& farNeg, const ZColor& nearNeg,
                	    	          float elementSize,
									  float dispMin, float dispMax ) const
{
	const int numElems = numElements();
	if( numElems<1 ) { return; }

	const float h = ZClamp( elementSize, 0.f, 1.f );

	const float hDx = 0.5f*h*_dx;
	const float hDy = 0.5f*h*_dy;
	const float hDz = 0.5f*h*_dz;

	ZPoint p;

	glBegin( GL_QUADS );

		FOR( j, 0, _jMax+1 )
		FOR( i, 0, _iMax+1 )
		{{
			const float& v = this->operator()(i,j,k);
			if( v < dispMin || v > dispMax ) { continue; }

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

			p = this->position(i,j,k);

			glVertex( p.x-hDx, p.y-hDy, p.z );
			glVertex( p.x+hDx, p.y-hDy, p.z );
			glVertex( p.x+hDx, p.y+hDy, p.z );
			glVertex( p.x-hDx, p.y+hDy, p.z );
		}}

	glEnd();
}



ostream& operator<<( ostream& os, const ZScalarField3D& object )
{
	os << "<ZScalarField3D>" << endl;
	os << " resolution : " << object.nx() << " x " << object.ny() << " x " << object.nz() << endl;
	os << " dimension  : " << object.lx() << " x " << object.ly() << " x " << object.lz() << endl;
	os << " cell size  : " << object.dx() << " x " << object.dy() << " x " << object.dz() << endl;
	os << " domain     : " << object.minPoint() << " ~ " << object.maxPoint() << endl;
	os << " location   : " << ZFieldLocation::name(object.location()) << endl;
	os << " memory size: " << object.usedMemorySize(ZDataUnit::zMegabytes) << " mb." << endl;
	os << " range      : " << object.minValue << " ~ " << object.maxValue << endl; 
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

