//--------------------//
// ZMarkerField2D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZMarkerField2D::ZMarkerField2D()
: ZField2DBase(), ZIntArray()
{
}

ZMarkerField2D::ZMarkerField2D( const ZMarkerField2D& f )
: ZField2DBase(f), ZIntArray(f)
{
}

ZMarkerField2D::ZMarkerField2D( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
: ZField2DBase(), ZIntArray()
{
	set( grid, loc );
}

ZMarkerField2D::ZMarkerField2D( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
: ZField2DBase(), ZIntArray()
{
	set( Nx, Nz, loc );
}

ZMarkerField2D::ZMarkerField2D( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Nz, Lx, Lz, loc );
}

ZMarkerField2D::ZMarkerField2D( const char* filePathName )
: ZField2DBase(), ZIntArray()
{
	load( filePathName );
}

void
ZMarkerField2D::set( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( grid, loc );
	ZIntArray::setLength( ZField2DBase::numElements() );
}

void
ZMarkerField2D::set( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( Nx, Nz, loc );
	ZIntArray::setLength( ZField2DBase::numElements() );
}

void
ZMarkerField2D::set( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( Nx, Nz, Lx, Lz, loc );
	ZIntArray::setLength( ZField2DBase::numElements() );
}

void
ZMarkerField2D::reset()
{
	ZField2DBase::reset();
	ZIntArray::reset();
}

ZMarkerField2D&
ZMarkerField2D::operator=( const ZMarkerField2D& f )
{
	ZGrid2D::operator=( f );
	ZIntArray::operator=( f );
	return (*this);
}

bool
ZMarkerField2D::exchange( const ZMarkerField2D& f )
{
	if( ZGrid2D::operator!=(f) ) { return false; }
	ZIntArray::exchange( *(ZIntArray*)(&f) );
	return true;
}

const ZString
ZMarkerField2D::dataType() const
{
	return ZString( "ZMarkerField2D" );
}

bool
ZMarkerField2D::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZMarkerField2D::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	ZMarkerField2D::dataType().write( fout, true );
	ZField2DBase::write( fout );
	ZIntArray::write( fout, true );
	fout.close();
	return true;
}

bool
ZMarkerField2D::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		reset();
		cout << "Error@ZMarkerField2D::save(): Failed to save file " << filePathName << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZMarkerField2D::load(): Data type mismatch." << endl;
		reset();
		return false;
	}
	ZField2DBase::read( fin );
	ZIntArray::read( fin, true );
	fin.close();
	return true;
}

ostream& operator<<( ostream& os, const ZMarkerField2D& object )
{
	os << "<ZMarkerField2D>" << endl;
	os << " resolution : " << object.nx() << " x " << object.nz() << endl;
	os << " dimension  : " << object.lx() << " x " << object.lz() << endl;
	os << " cell size  : " << object.dx() << " x " << object.dz() << endl;
	os << " domain     : " << object.minPoint() << " ~ " << object.maxPoint() << endl;
	os << " location   : " << ZFieldLocation::name(object.location()) << endl;
	os << " memory size: " << object.usedMemorySize(ZDataUnit::zMegabytes) << " mb." << endl;
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

