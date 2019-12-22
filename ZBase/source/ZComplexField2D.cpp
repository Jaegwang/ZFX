//---------------------//
// ZComplexField2D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZComplexField2D::ZComplexField2D()
: ZField2DBase(), ZComplexArray()
{
}

ZComplexField2D::ZComplexField2D( const ZComplexField2D& f )
: ZField2DBase(f), ZComplexArray(f)
{
}

ZComplexField2D::ZComplexField2D( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
: ZField2DBase(), ZComplexArray()
{
	set( grid, loc );
}

ZComplexField2D::ZComplexField2D( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
: ZField2DBase(), ZComplexArray()
{
	set( Nx, Nz, loc );
}

ZComplexField2D::ZComplexField2D( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Nz, Lx, Lz, loc );
}

ZComplexField2D::ZComplexField2D( const char* filePathName )
: ZField2DBase(), ZComplexArray()
{
	load( filePathName );
}

void
ZComplexField2D::set( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( grid, loc );
	ZComplexArray::setLength( ZField2DBase::numElements() );
}

void
ZComplexField2D::set( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( Nx, Nz, loc );
	ZComplexArray::setLength( ZField2DBase::numElements() );
}

void
ZComplexField2D::set( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	ZField2DBase::set( Nx, Nz, Lx, Lz, loc );
	ZComplexArray::setLength( ZField2DBase::numElements() );
}

void
ZComplexField2D::reset()
{
	ZField2DBase::reset();
	ZComplexArray::reset();
}

ZComplexField2D&
ZComplexField2D::operator=( const ZComplexField2D& f )
{
	ZGrid2D::operator=( f );
	ZComplexArray::operator=( f );
	return (*this);
}

bool
ZComplexField2D::exchange( const ZComplexField2D& f )
{
	if( ZGrid2D::operator!=(f) ) { return false; }
	ZComplexArray::exchange( *(ZComplexArray*)(&f) );
	return true;
}

const ZString
ZComplexField2D::dataType() const
{
	return ZString( "ZComplexField2D" );
}

bool
ZComplexField2D::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZComplexField2D::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	ZComplexField2D::dataType().write( fout, true );
	ZField2DBase::write( fout );
	ZComplexArray::write( fout, true );
	fout.close();
	return true;
}

bool
ZComplexField2D::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		reset();
		cout << "Error@ZComplexField2D::save(): Failed to save file " << filePathName << endl;
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZComplexField2D::load(): Data type mismatch." << endl;
		reset();
		return false;
	}
	ZField2DBase::read( fin );
	ZComplexArray::read( fin, true );
	fin.close();
	return true;
}

ostream& operator<<( ostream& os, const ZComplexField2D& object )
{
	os << "<ZComplexField2D>" << endl;
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

