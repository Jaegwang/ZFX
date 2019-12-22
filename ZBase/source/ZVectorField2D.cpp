//--------------------//
// ZVectorField2D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZVectorField2D::ZVectorField2D()
: ZField2DBase(), ZVectorArray()
{
}

ZVectorField2D::ZVectorField2D( const ZVectorField2D& f )
: ZField2DBase(f), ZVectorArray(f)
{
}

ZVectorField2D::ZVectorField2D( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
: ZField2DBase(), ZVectorArray()
{
	set( grid, loc );
}

ZVectorField2D::ZVectorField2D( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
: ZField2DBase(), ZVectorArray()
{
	set( Nx, Nz, loc );
}

ZVectorField2D::ZVectorField2D( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Nz, Lx, Lz, loc );
}

ZVectorField2D::ZVectorField2D( const char* filePathName )
: ZField2DBase(), ZVectorArray()
{
	load( filePathName );
}

void
ZVectorField2D::set( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( grid, loc );
	ZVectorArray::setLength( ZField2DBase::numElements() );
}

void
ZVectorField2D::set( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( Nx, Nz, loc );
	ZVectorArray::setLength( ZField2DBase::numElements() );
}

void
ZVectorField2D::set( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( Nx, Nz, Lx, Lz, loc );
	ZVectorArray::setLength( ZField2DBase::numElements() );
}

void
ZVectorField2D::reset()
{
	ZField2DBase::reset();
	ZVectorArray::reset();
}

ZVectorField2D&
ZVectorField2D::operator=( const ZVectorField2D& f )
{
	ZGrid2D::operator=( f );
	ZVectorArray::operator=( f );
	return (*this);
}

bool
ZVectorField2D::exchange( const ZVectorField2D& f )
{
	if( ZGrid2D::operator!=(f) ) { return false; }
	ZVectorArray::exchange( *(ZVectorArray*)(&f) );
	return true;
}

const ZString
ZVectorField2D::dataType() const
{
	return ZString( "ZVectorField2D" );
}

bool
ZVectorField2D::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZVectorField2D::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	ZVectorField2D::dataType().write( fout, true );
	ZField2DBase::write( fout );
	ZVectorArray::write( fout, true );
	fout.close();
	return true;
}

bool
ZVectorField2D::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		reset();
		cout << "Error@ZVectorField2D::save(): Failed to save file " << filePathName << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZVectorField2D::load(): Data type mismatch." << endl;
		reset();
		return false;
	}
	ZField2DBase::read( fin );
	ZVectorArray::read( fin, true );
	fin.close();
	return true;
}

ostream& operator<<( ostream& os, const ZVectorField2D& object )
{
	os << "<ZVectorField2D>" << endl;
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

