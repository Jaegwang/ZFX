//------------//
// String.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

MString
MakePadding( int number, int paddingCount )
{
	MString result;

	MString str;
	str = number;

	const int strLen = str.length();

	if( paddingCount < strLen )
	{
		result;
	}

	FOR( i, 0, paddingCount-strLen )
	{
		result += "0";
	}

	result += number;

	return result;
}

void
RemoveSpace( MString& str )
{
	MStringArray tokens;
	str.split( ' ', tokens );
	str.clear();
	const int len = tokens.length();
	FOR( i, 0, len )
	{
		str += tokens[i];
	}
}

bool
GetList( const MString& string, MIntArray& list )
{
	MString str( string );
	RemoveSpace( str );

	MStringArray tokens0;
	str.split( ',', tokens0 );

	const int len0 = tokens0.length();
	FOR( i, 0, len0 )
	{
		MStringArray tokens;
		tokens0[i].split( '-', tokens );

		bool failed = false;

		const int len = tokens.length();
		if( len == 1 ) {
			const char* c = tokens[0].asChar();
			if( !ZNumericChar(c[0]) ) { failed = true; }
			list.append( tokens[0].asInt() );
		} else if( len == 2 ) {
			const char* c0 = tokens[0].asChar();
			const char* c1 = tokens[1].asChar();
			if( !( ZNumericChar(c0[0]) && ZNumericChar(c1[0]) ) ) { failed = true; }
			const int i0 = tokens[0].asInt();
			const int i1 = tokens[1].asInt();
			int min, max;   ZMinMax( i0, i1, min, max );
			FOR(j,min,max+1) { list.append(j); }
		} else {
			failed = true;
		}

		if( failed ) { list.clear(); return false; }
	}

	return true;
}

ZELOS_NAMESPACE_END

