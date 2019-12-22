//----------//
// File.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool
ZSave( const MDoubleArray& a, const char* filePathName )
{
	ofstream fout( filePathName, ios::out|ios::binary );
	if( fout.fail() )
	{
		mPrintError( MString("Failed to save file: ")+MString(filePathName) );
		return false;
	}

	const unsigned int n = a.length();
	fout.write( (char*)&n, sizeof(unsigned int));
	const double& v = a[0];
	fout.write( (char*)&v, n*sizeof(double) );

	fout.close();
	return true;
}

bool
ZSave( const MPointArray& a, const char* filePathName )
{
	ofstream fout( filePathName, ios::out|ios::binary );
	if( fout.fail() )
	{
		mPrintError( MString("Failed to save file: ")+MString(filePathName) );
		return false;
	}

	const unsigned int n = a.length();
	fout.write( (char*)&n, sizeof(unsigned int));
	fout.write( (char*)&a[0].x, n*sizeof(MPoint) );

	fout.close();
	return true;
}

bool
ZLoad( MDoubleArray& a, const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );
	if( fin.fail() )
	{
		mPrintError( MString("Failed to load file: ")+MString(filePathName) );
		return false;
	}

	unsigned int n = 0;
	fin.read( (char*)&n, sizeof(unsigned int));
	if( !n ) { a.clear(); return true; }

	a.setLength( n );
	fin.read( (char*)&a[0], n*sizeof(double) );

	fin.close();
	return true;
}

bool
ZLoad( MPointArray& a, const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );
	if( fin.fail() )
	{
		mPrintError( MString("Failed to load file: ")+MString(filePathName) );
		return false;
	}

	unsigned int n = 0;
	fin.read( (char*)&n, sizeof(unsigned int));
	if( !n ) { a.clear(); return true; }

	a.setLength( n );
	fin.read( (char*)&a[0].x, n*sizeof(MPoint) );

	fin.close();
	return true;
}

ZELOS_NAMESPACE_END

