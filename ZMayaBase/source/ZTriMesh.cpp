//--------------//
// ZTriMesh.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool
GetMesh( ZTriMesh& mesh, const char* meshShapeNodeName, bool vPosOnly, const char* uvSetName )
{
	MObject meshObj( NodeNameToMObject( meshShapeNodeName ) );
	return Convert( mesh, meshObj, vPosOnly, uvSetName );
}

bool
Convert( ZTriMesh& mesh, MObject& meshObj, bool vPosOnly, const char* uvSetName )
{
	MStatus status = MS::kSuccess;

	if( !vPosOnly ) { mesh.reset(); }

	MFnMesh meshFn( meshObj, &status );
	if( status != MS::kSuccess )
	{
		mPrintError( "Error@Convert(): Failed to get MFnMesh." );
		mesh.reset();
		return false;
	}

	MItMeshVertex vItr( meshObj, &status );
	if( status != MS::kSuccess )
	{
		mPrintError( "Error@Convert(): Failed to get MItMeshVertex." );
		mesh.reset();
		return false;
	}

	MItMeshPolygon fItr( meshObj, &status );
	if( status != MS::kSuccess )
	{
		mPrintError( "Error@Convert(): Failed to get MItMeshPolygon." );
		mesh.reset();
		return false;
	}

	const int numVertices = meshFn.numVertices();

	//////////////////////
	// vertex positions //

	int         vIdx = 0;
	MPoint      localPos;
	MMatrix     localToWorld;
	MPointArray vP( numVertices ); // vertex positions

	if( GetWorldMatrix( meshObj, localToWorld ) )
	{
		for( vItr.reset(); !vItr.isDone(); vItr.next(), ++vIdx )
		{
			localPos = vItr.position( MSpace::kObject );
			ApplyXForm( localToWorld, localPos, vP[vIdx] );
		}
	}
	else
	{
		for( vItr.reset(); !vItr.isDone(); vItr.next(), ++vIdx )
		{
			vP[vIdx] = vItr.position( MSpace::kWorld );
		}
	}

	Copy( mesh.p, vP ); // mesh.p <- vP

	if( vPosOnly ) { return true; }

	/////////////////
	// UV-set name //

	bool toConvertUV = true;
	MString uvSetNameStr;
	MFloatArray vU, vV;

	if( !meshFn.numUVSets() || !meshFn.numUVs() )
	{
		toConvertUV = false;
	}
	else
	{
		MString inUVSetName( uvSetName );

		if( inUVSetName.length() == 0 )
		{
			toConvertUV = false;
		}
		else if( inUVSetName == MString("currentUVSet") )
		{
			uvSetNameStr = meshFn.currentUVSetName();
		}
		else
		{
			MStringArray uvSetNames;
			meshFn.getUVSetNames( uvSetNames );
			const int numUVSets = (int)uvSetNames.length();

			FOR( i, 0, numUVSets )
			{
				if( inUVSetName == uvSetNames[i] )
				{
					uvSetNameStr = inUVSetName;
					break;
				}
			}
		}
	}

	if( toConvertUV )
	{
		if( !meshFn.getUVs( vU, vV, &uvSetNameStr ) ) { toConvertUV = false; }
	}

	//////////////////////
	// triangle indices //

	ZInt3Array& v012 = mesh.v012;
	v012.reserve( meshFn.numPolygons()*2 );

	ZPointArray& uv = mesh.uv;
	uv.reserve( meshFn.numPolygons()*3 );

	if( toConvertUV )
	{
		MIntArray vList, uvIndices;

		for( fItr.reset(); !fItr.isDone(); fItr.next() )
		{
			fItr.getVertices( vList );
			const int vCount = (int)vList.length();

			if( vCount < 3 ) // invalid case 
			{
                continue;
			}

			uvIndices.setLength( vCount );
			FOR( i, 0, vCount ) { fItr.getUVIndex( i, uvIndices[i], &uvSetNameStr ); }

			FOR( i, 0, vCount-2 )
			{
				v012.push_back( ZInt3( vList[0], vList[i+1], vList[i+2] ) );

				uv.push_back( ZPoint( vU[uvIndices[0  ]], vV[uvIndices[0  ]], 0.f ) );
				uv.push_back( ZPoint( vU[uvIndices[i+1]], vV[uvIndices[i+1]], 0.f ) );
				uv.push_back( ZPoint( vU[uvIndices[i+2]], vV[uvIndices[i+2]], 0.f ) );
			}
		}
	}
	else
	{
		MIntArray vList;

		for( fItr.reset(); !fItr.isDone(); fItr.next() )
		{
			fItr.getVertices( vList );
			const int vCount = (int)vList.length();

			if( vCount < 3 ) // invalid case
			{
                continue;
			}

			FOR( i, 0, vCount-2 )
			{
				v012.push_back( ZInt3( vList[0], vList[i+1], vList[i+2] ) );
			}
		}
	}

	// This code may generate per frame different connections when the mesh is deforming.
	//MIntArray triangleCounts, triangleVertices;
	//meshFn.getTriangles( triangleCounts, triangleVertices );
	//const int numTriangles = (int)(triangleVertices.length()/3);
	//mesh.v012.setLength( numTriangles );
	//int idx = 0;
	//FOR( i, 0, numTriangles )
	//{
	//	ZInt3& v012 = mesh.v012[i];
	//	v012[0] = triangleVertices[idx++];
	//	v012[1] = triangleVertices[idx++];
	//	v012[2] = triangleVertices[idx++];
	//}

	return true;
}

bool
Convert( ZTriMesh& mesh, MObjectArray& meshObjs )
{
	mesh.reset();

	ZTriMesh tmpMesh;
	FOR( i, 0, meshObjs.length() )
	{
		Convert( tmpMesh, meshObjs[i], false, NULL );
		mesh.combine( tmpMesh );
	}
}

bool 
MPolyPlaneToNormalMap( const char* meshShapeNodeName, ZImageMap& nrmMap, ZImageMap& hgtMap, int imgResolution, bool xFlip, bool zFlip, float strength )
{
	//--- 1.
	// MPolyPlane -> ZTriMesh 
	ZTriMesh triMesh;
	MObject meshObj( NodeNameToMObject( meshShapeNodeName ) );
	Convert( triMesh, meshObj );
	
	//--- 2.
	// ZTriMesh -> ZScalarField2D
	ZString planeName( meshShapeNodeName );
	ZString numOfPlaneStr = planeName.subString( 11, 12 );
	int numOfPlane = numOfPlaneStr.asInt();
	int subW=0, subH=0;
	MGlobal::executeCommand( MString( "getAttr polyPlane" ) + numOfPlane + ".subdivisionsWidth",  subW );
	MGlobal::executeCommand( MString( "getAttr polyPlane" ) + numOfPlane + ".subdivisionsHeight", subH );

	ZScalarField2D hgtField( subW, subH, ZFieldLocation::zNode );
	const ZPointArray& pArray = triMesh.p;

	const int iMax = hgtField.iMax();
	const int kMax = hgtField.kMax();
	const int iMaxPlus1 = hgtField.iMax()+1;
	const int kMaxPlus1 = hgtField.kMax()+1;

	#pragma omp parallel for
	FOR( k, 0, kMaxPlus1 )
	FOR( i, 0, iMaxPlus1 )
	{
		const int idx = i+iMaxPlus1*k;
		hgtField[idx] = pArray[idx].y;
	}

	if( xFlip ) // flip by maya x-axis
	{
		const int halfK = kMaxPlus1/2;
		#pragma omp parallel for
		FOR( i, 0, iMaxPlus1 )
		{
			FOR( k, 0, halfK )
			{	
				swap( hgtField(i,k), hgtField(i, kMax-k) );
			}
		}
	}

	if( zFlip ) // flip by maya z-axis
	{
		const int halfI = iMaxPlus1/2;
		#pragma omp parallel for
		FOR( k, 0, kMaxPlus1 )
		{
			FOR( i, 0, halfI )
			{
				swap( hgtField(i,k), hgtField(iMax-i,k) );
			}
		}
	}

	//--- 3.
	// ZScalarField2D -> ZImageMap(hgt)
	int imgResX = imgResolution*hgtField.nx();
	int imgResZ = imgResolution*hgtField.nz();
	float dxImg = (float)hgtField.nx()/(float)imgResX;
	float dzImg = (float)hgtField.nz()/(float)imgResZ;
	nrmMap.set( imgResX, imgResZ, 3 );
	hgtMap.set( imgResX, imgResZ, 3 );
	
	#pragma omp parallel for
	PER_EACH_PIXEL( hgtMap )
		float value =  hgtField.catrom( ZPoint( dxImg*i,0,dzImg*j ) );
		hgtMap(i,j,0) = value;
		hgtMap(i,j,1) = value;
		hgtMap(i,j,2) = value;
	END_PER_EACH_PIXEL
	
	//--- 4.
	// ZImageMap(hgt) -> ZImageMap(nrm)
	if( !CalculateNormalMap( nrmMap, hgtMap, strength, true ) )
	{
		cout << "Error@MPolyPlaneToNormalMap(): Failed to compute normal" << endl;
	}
}

bool 
GetHeightMap( ZImageMap& hgtMap, const ZTriMesh& triMesh, int subW, int subH, int imgResolution, bool xFlip, bool zFlip )
{
	// ZTriMesh -> ZScalarField2D
	ZScalarField2D hgtField( subW, subH, ZFieldLocation::zNode );
	const ZPointArray& pArray = triMesh.p;

	const int iMax = hgtField.iMax();
	const int kMax = hgtField.kMax();
	const int iMaxPlus1 = hgtField.iMax()+1;
	const int kMaxPlus1 = hgtField.kMax()+1;

	#pragma omp parallel for
	FOR( k, 0, kMaxPlus1 )
	FOR( i, 0, iMaxPlus1 )
	{
		const int idx = i+iMaxPlus1*k;
		hgtField[idx] = pArray[idx].y;
	}

	if( xFlip ) // flip by maya x-axis
	{
		const int halfK = kMaxPlus1/2;
		#pragma omp parallel for
		FOR( i, 0, iMaxPlus1 )
		{
			FOR( k, 0, halfK )
			{	
				swap( hgtField(i,k), hgtField(i, kMax-k) );
			}
		}
	}

	if( zFlip ) // flip by maya z-axis
	{
		const int halfI = iMaxPlus1/2;
		#pragma omp parallel for
		FOR( k, 0, kMaxPlus1 )
		{
			FOR( i, 0, halfI )
			{
				swap( hgtField(i,k), hgtField(iMax-i,k) );
			}
		}
	}

	// ZScalarField2D -> ZImageMap
	int imgResX = imgResolution*hgtField.nx();
	int imgResZ = imgResolution*hgtField.nz();
	float dxImg = (float)hgtField.nx()/(float)imgResX;
	float dzImg = (float)hgtField.nz()/(float)imgResZ;
	hgtMap.set( imgResX, imgResZ, 3 );
	
	#pragma omp parallel for
	PER_EACH_PIXEL( hgtMap )
		float value =  hgtField.catrom( ZPoint( dxImg*i,0,dzImg*j ) );
		hgtMap(i,j,0) = value;
		hgtMap(i,j,1) = value;
		hgtMap(i,j,2) = value;
	END_PER_EACH_PIXEL
}

bool 
GetNormalMap( ZImageMap& nrmMap, const ZTriMesh& triMesh, int subW, int subH, int imgResolution, bool xFlip, bool zFlip, float strength )
{
	ZScalarField2D hgtField( subW, subH, ZFieldLocation::zNode );
	const ZPointArray& pArray = triMesh.p;

	const int iMax = hgtField.iMax();
	const int kMax = hgtField.kMax();
	const int iMaxPlus1 = hgtField.iMax()+1;
	const int kMaxPlus1 = hgtField.kMax()+1;

	#pragma omp parallel for
	FOR( k, 0, kMaxPlus1 )
	FOR( i, 0, iMaxPlus1 )
	{
		const int idx = i+iMaxPlus1*k;
		hgtField[idx] = pArray[idx].y;
	}

	if( xFlip ) // flip by maya x-axis
	{
		const int halfK = kMaxPlus1/2;
		#pragma omp parallel for
		FOR( i, 0, iMaxPlus1 )
		{
			FOR( k, 0, halfK )
			{	
				swap( hgtField(i,k), hgtField(i, kMax-k) );
			}
		}
	}

	if( zFlip ) // flip by maya z-axis
	{
		const int halfI = iMaxPlus1/2;
		#pragma omp parallel for
		FOR( k, 0, kMaxPlus1 )
		{
			FOR( i, 0, halfI )
			{
				swap( hgtField(i,k), hgtField(iMax-i,k) );
			}
		}
	}

	// ZScalarField2D -> hgtMap
	int imgResX = imgResolution*hgtField.nx();
	int imgResZ = imgResolution*hgtField.nz();
	float dxImg = (float)hgtField.nx()/(float)imgResX;
	float dzImg = (float)hgtField.nz()/(float)imgResZ;
	nrmMap.set( imgResX, imgResZ, 3 );
	ZImageMap hgtMap;
	hgtMap.set( imgResX, imgResZ, 3 );

	#pragma omp parallel for
	PER_EACH_PIXEL( hgtMap )
		float value =  hgtField.catrom( ZPoint( dxImg*i,0,dzImg*j ) );
		hgtMap(i,j,0) = value;
		hgtMap(i,j,1) = value;
		hgtMap(i,j,2) = value;
	END_PER_EACH_PIXEL

	// hgtMap -> nrmMap
	if( !CalculateNormalMap( nrmMap, hgtMap, strength, true ) )
	{
		cout << "Error@MPolyPlaneToNormalMap(): Failed to compute normal" << endl;
	}
}

ZELOS_NAMESPACE_END

