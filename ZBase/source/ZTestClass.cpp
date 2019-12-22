//----------------//
// ZTestClass.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.02.16                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZTestClass::ZTestClass()
{
}

ZTestClass::ZTestClass( const ZTestClass& obj )
{
}

std::vector<double>
ZTestClass::return_double_tuple()
{
	std::vector<double> ret( 4 );
	ret[0] = 1.0;
	ret[1] = 2.0;
	ret[2] = 3.0;
	ret[3] = 4.0;
	return ret;
}

std::vector<float>
ZTestClass::return_float_tuple()
{
	std::vector<float> ret( 4 );
	ret[0] = 1.f;
	ret[1] = 2.f;
	ret[2] = 3.f;
	ret[3] = 4.f;
	return ret;
}

std::vector<double>
ZTestClass::times2( std::vector<double> a )
{
	for( int i=0; i<(int)a.size(); ++i )
	{
		a[i] *= 2.0;
	}

	return a;
}

ostream&
operator<<( ostream& os, const ZTestClass& obj )
{
	return os;
}

ZELOS_NAMESPACE_END

