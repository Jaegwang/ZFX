//--------------------//
// ZMarkerField3D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZMarkerField3D::ZMarkerField3D()
: ZField3DBase(), ZIntArray()
{
}

ZMarkerField3D::ZMarkerField3D( const ZMarkerField3D& f )
: ZField3DBase(f), ZIntArray(f)
{
}

ZMarkerField3D::ZMarkerField3D( const ZGrid3D& grid, ZFieldLocation::FieldLocation loc )
: ZField3DBase(), ZIntArray()
{
	set( grid, loc );
}

ZMarkerField3D::ZMarkerField3D( int Nx, int Ny, int Nz, ZFieldLocation::FieldLocation loc )
: ZField3DBase(), ZIntArray()
{
	set( Nx, Ny, Nz, loc );
}

ZMarkerField3D::ZMarkerField3D( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Ny, Nz, Lx, Ly, Lz, loc );
}

ZMarkerField3D::ZMarkerField3D( const char* filePathName )
: ZField3DBase(), ZIntArray()
{
	load( filePathName );
}

void
ZMarkerField3D::set( const ZGrid3D& grid, ZFieldLocation::FieldLocation loc )
{
	ZField3DBase::set( grid, loc );
	ZIntArray::setLength( ZField3DBase::numElements() );
}

void
ZMarkerField3D::set( int Nx, int Ny, int Nz, ZFieldLocation::FieldLocation loc )
{
	ZField3DBase::set( Nx, Ny, Nz, loc );
	ZIntArray::setLength( ZField3DBase::numElements() );
}

void
ZMarkerField3D::set( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz, ZFieldLocation::FieldLocation loc )
{
	ZField3DBase::set( Nx, Ny, Nz, Lx, Ly, Lz, loc );
	ZIntArray::setLength( ZField3DBase::numElements() );
}

void
ZMarkerField3D::reset()
{
	ZField3DBase::reset();
	ZIntArray::reset();
}

ZMarkerField3D&
ZMarkerField3D::operator=( const ZMarkerField3D& f )
{
	ZGrid3D::operator=( f );
	ZIntArray::operator=( f );
	return (*this);
}

bool
ZMarkerField3D::exchange( const ZMarkerField3D& f )
{
	if( ZGrid3D::operator!=(f) ) { return false; }
	ZIntArray::exchange( *(ZIntArray*)(&f) );
	return true;
}

const ZString
ZMarkerField3D::dataType() const
{
	return ZString( "ZMarkerField3D" );
}

bool
ZMarkerField3D::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZMarkerField3D::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	ZMarkerField3D::dataType().write( fout, true );
	ZField3DBase::write( fout );
	ZIntArray::write( fout, true );
	fout.close();
	return true;
}

bool
ZMarkerField3D::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		reset();
		cout << "Error@ZMarkerField3D::save(): Failed to save file " << filePathName << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZMarkerField3D::load(): Data type mismatch." << endl;
		reset();
		return false;
	}
	ZField3DBase::read( fin );
	ZIntArray::read( fin, true );
	fin.close();
	return true;
}

ostream& operator<<( ostream& os, const ZMarkerField3D& object )
{
	os << "<ZMarkerField3D>" << endl;
	os << " resolution : " << object.nx() << " x " << object.ny() << " x " << object.nz() << endl;
	os << " dimension  : " << object.lx() << " x " << object.ly() << " x " << object.lz() << endl;
	os << " cell size  : " << object.dx() << " x " << object.dy() << " x " << object.dz() << endl;
	os << " domain     : " << object.minPoint() << " ~ " << object.maxPoint() << endl;
	os << " location   : " << ZFieldLocation::name(object.location()) << endl;
	os << " memory size: " << object.usedMemorySize(ZDataUnit::zMegabytes) << " mb." << endl;
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

