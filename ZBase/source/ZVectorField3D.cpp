//--------------------//
// ZVectorField3D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZVectorField3D::ZVectorField3D()
: ZField3DBase(), ZVectorArray()
{
}

ZVectorField3D::ZVectorField3D( const ZVectorField3D& f )
: ZField3DBase(f), ZVectorArray(f)
{
}

ZVectorField3D::ZVectorField3D( const ZGrid3D& grid, ZFieldLocation::FieldLocation loc )
: ZField3DBase(), ZVectorArray()
{
	set( grid, loc );
}

ZVectorField3D::ZVectorField3D( int Nx, int Ny, int Nz, ZFieldLocation::FieldLocation loc )
: ZField3DBase(), ZVectorArray()
{
	set( Nx, Ny, Nz, loc );
}

ZVectorField3D::ZVectorField3D( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Ny, Nz, Lx, Ly, Lz, loc );
}

ZVectorField3D::ZVectorField3D( const char* filePathName )
: ZField3DBase(), ZVectorArray()
{
	load( filePathName );
}

void
ZVectorField3D::set( const ZGrid3D& grid, ZFieldLocation::FieldLocation loc )
{
	ZField3DBase::set( grid, loc );
	ZVectorArray::setLength( ZField3DBase::numElements() );
}

void
ZVectorField3D::set( int Nx, int Ny, int Nz, ZFieldLocation::FieldLocation loc )
{
	ZField3DBase::set( Nx, Ny, Nz, loc );
	ZVectorArray::setLength( ZField3DBase::numElements() );
}

void
ZVectorField3D::set( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz, ZFieldLocation::FieldLocation loc )
{
	ZField3DBase::set( Nx, Ny, Nz, Lx, Ly, Lz, loc );
	ZVectorArray::setLength( ZField3DBase::numElements() );
}

void
ZVectorField3D::reset()
{
	ZField3DBase::reset();
	ZVectorArray::reset();
}

ZVectorField3D&
ZVectorField3D::operator=( const ZVectorField3D& f )
{
	ZField3DBase::operator=( f );
	ZVectorArray::operator=( f );
	return (*this);
}

bool
ZVectorField3D::exchange( const ZVectorField3D& f )
{
	if( ZGrid3D::operator!=(f) ) { return false; }
	ZVectorArray::exchange( *(ZVectorArray*)(&f) );
	return true;
}

const ZString
ZVectorField3D::dataType() const
{
	return ZString( "ZVectorField3D" );
}

bool
ZVectorField3D::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZVectorField3D::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	ZVectorField3D::dataType().write( fout, true );
	ZField3DBase::write( fout );
	ZVectorArray::write( fout, true );
	fout.close();
	return true;
}

bool
ZVectorField3D::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		reset();
		cout << "Error@ZVectorField3D::save(): Failed to save file " << filePathName << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZVectorField3D::load(): Data type mismatch." << endl;
		reset();
		return false;
	}
	ZField3DBase::read( fin );
	ZVectorArray::read( fin, true );
	fin.close();
	return true;
}

ostream& operator<<( ostream& os, const ZVectorField3D& object )
{
	os << "<ZVectorField3D>" << endl;
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

