//-----------------//
// ZColorArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZColorArray::ZColorArray()
{
	_colorSpace = ZColorSpace::zRGB;
}

ZColorArray::ZColorArray( const ZColorArray& a )
: ZArray<ZColor>::ZArray( a )
{
	_colorSpace = ZColorSpace::zRGB;
}

ZColorArray::ZColorArray( int l )
: ZArray<ZColor>::ZArray( l )
{
	_colorSpace = ZColorSpace::zRGB;
}

ZColorArray::ZColorArray( int l, const ZColor& v )
: ZArray<ZColor>::ZArray( l, v )
{
	_colorSpace = ZColorSpace::zRGB;
}

ZColorSpace::ColorSpace
ZColorArray::colorSpace() const
{
	return _colorSpace;
}

void
ZColorArray::setColorSpace( ZColorSpace::ColorSpace colorSpace )
{
	if( colorSpace!=ZColorSpace::zRGB && colorSpace!=ZColorSpace::zHSV )
	{
		cout << "Error@ZColorArray::setColorSpace(): Invalid color colorSpace." << endl;
		return;
	}

	if( _colorSpace==ZColorSpace::zRGB && colorSpace==ZColorSpace::zHSV )
	{
		rgb2hsv();
		return;
	}

	if( _colorSpace==ZColorSpace::zHSV && colorSpace==ZColorSpace::zRGB )
	{
		hsv2rgb();
		return;
	}

	_colorSpace = colorSpace;
}

inline ZColorArray&
ZColorArray::operator*=( float s )
{
	const int n = parent::length();

	FOR( i, 0, n )
	{
		ZColor& c = parent::operator[](i);
		c *= s;
	}
	return (*this);
}

void
ZColorArray::rgb2hsv()
{
	if( _colorSpace == ZColorSpace::zHSV ) { return; }

	_colorSpace = ZColorSpace::zHSV;

	const int n = parent::length();

	FOR( i, 0, n )
	{
		ZColor& c = parent::operator[](i);

		c.rgb2hsv();
	}
}

void
ZColorArray::hsv2rgb()
{
	if( _colorSpace == ZColorSpace::zRGB ) { return; }

	_colorSpace = ZColorSpace::zRGB;

	const int n = parent::length();

	FOR( i, 0, n )
	{
		ZColor& c = parent::operator[](i);

		c.hsv2rgb();
	}
}

// not good
//void
//ZColorArray::setRandomColor( int seed, float S, float V )
//{
//	const int n = parent::length();
//
//	const float golden_ratio_conjugate = 0.618033988749895f;
//
//	#pragma omp parallel for
//	FOR( i, 0, n )
//	{
//		ZColor& c = parent::operator[](i);
//
//		c.h = 360.f * ( ZRand(i+seed) + golden_ratio_conjugate );
//		c.s = S;
//		c.v = V;
//
//		c.hsv2rgb();
//
//		c.a = 1.f;
//	}
//}

void
ZColorArray::setRandomColors( int seed, float min, float max )
{
	const int n = parent::length();

	FOR( i, 0, n )
	{
		ZColor& c = parent::operator[](i);

		c.r = ZRand( i+seed+13, min, max );
		c.g = ZRand( i+seed+31, min, max );
		c.b = ZRand( i+seed+53, min, max );
		c.a = 1.f;
	}
}

const ZString
ZColorArray::dataType() const
{
	ZString type( "ZArray_" );
	return ( type + typeid(ZColor).name() );
}

bool
ZColorArray::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZColorArray::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	ZColorArray::dataType().write( fout, true );
	fout.write( (char*)&_colorSpace, sizeof(int) );
	parent::write( fout );
	fout.close();
	return true;
}

bool
ZColorArray::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZColorArray::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	ZString type;
	type.read( fin, true );
	if( type != dataType() )
	{
		cout << "Error@ZColorArray::load(): Data type mismatch." << endl;
		reset();
		return false;
	}
	fin.read( (char*)&_colorSpace, sizeof(int) );
	parent::read( fin );
	fin.close();
	return true;
}

ostream&
operator<<( ostream& os, const ZColorArray& object )
{
	os << "<ZColorArray>" << endl;
	const int size     = object.size();
	const int capacity = object.capacity();

	double bytes = size * sizeof(ZColor);
	os << " size    : " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	bytes = capacity * sizeof(ZColor);
	os << " capacity: " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

