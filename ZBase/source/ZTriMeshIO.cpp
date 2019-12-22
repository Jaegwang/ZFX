//----------------//
// ZTriMeshIO.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

bool
Load_from_obj( ZTriMesh& mesh, const char* filePathName )
{
	mesh.reset();

	ifstream fin( filePathName, ios::in );
	if( fin.fail() )
	{
		cout << "Failed to open file: " << filePathName << endl;
		return false;
	}

	ZPointArray& p    = mesh.p;
	ZInt3Array&  v012 = mesh.v012;
	ZPointArray& uv   = mesh.uv;

	string line, attr, token;
	ZPoint pt;

	while( getline( fin, line ) )
	{
		attr = line.substr(0,2);

		if( attr == "v " ) { // vertex position

			istringstream s( line.substr(2) );

			s >> pt.x >> pt.y >> pt.z;
			p.push_back( pt );

		} else if( attr == "f " ) { // poly-connections

			istringstream s( line.substr(2) );

			int count = 0;
			ZIntArray tmp;
			while( getline(s,token,' ') )
			{
				tmp.push_back( atoi(token.c_str()) );
				++count;
			}

			const int numVerts = tmp.length();
			FOR( j, 0, numVerts-2 )
			{
				v012.push_back( ZInt3( tmp[0]-1, tmp[j+1]-1, tmp[j+2]-1 ) );
			}

		} else if( attr == "vt" ) { // texture coordinates

		} else if( attr == "vn" ) { // vertex normals

		} else if( attr == "vp" ) { // parameter space vertex positions

		} else if( line[0] == '#' ) {}

		else {}
	}

	fin.close();

	return true;
}

ZELOS_NAMESPACE_END

