//-----------//
// ZMesh.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.01.27                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool
Convert( ZMesh& mesh, MObject& meshObj, bool vPosOnly, const char* uvSetName )
{
std::cout<<3333<<std::endl;

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
	MPointArray p( numVertices );	// vertex positions
	MFloatArray u, v;				// uv coordinates

	//////////////////////
	// vertex positions //

	int         vIdx = 0;
	MPoint      localPos;
	MMatrix     localToWorld;

	if( GetWorldMatrix( meshObj, localToWorld ) ) {
		for( vItr.reset(); !vItr.isDone(); vItr.next(), ++vIdx )
		{
			localPos = vItr.position( MSpace::kObject );
			ApplyXForm( localToWorld, localPos, p[vIdx] );
		}
	} else {
		for( vItr.reset(); !vItr.isDone(); vItr.next(), ++vIdx )
		{
			p[vIdx] = vItr.position( MSpace::kWorld );
		}
	}

	if( vPosOnly ) { return true; }
	
	/////////////////
	// UV-set name //

std::cout<<2222<<std::endl;

	bool convertUVs = true;
	MString uvSetNameStr( uvSetName );

	if( meshFn.numUVSets()==0 && meshFn.numUVs()==0 ) {

		convertUVs = false;

	} else if( MString(uvSetName) == MString("currentUVSet") ) {

		uvSetNameStr = meshFn.currentUVSetName();
		if( !meshFn.getUVs( u, v, &uvSetNameStr ) ) { convertUVs=false; }

	} else {

		if( !meshFn.getUVs( u, v, &uvSetNameStr ) ) { convertUVs = false; }

	}

std::cout<<1111<<std::endl;

	ZPoint vp;
	FOR( i, 0, numVertices )
	{
		Copy( vp, p[i] );
		mesh.addPoint( vp );
	}

	if( convertUVs )
	{
		const int numUVs = (int)u.length(); // = v.length();
		FOR( i, 0, numUVs )
		{
			mesh.addUV( ZPoint( u[i], v[i], 0 ) );
		}
	}

	for( fItr.reset(); !fItr.isDone(); fItr.next() )
	{
		MIntArray pList;
		fItr.getVertices( pList );

		const int n = (int)pList.length();

		if( convertUVs ) {

			int uvIndex[4];

			FOR( i, 0, n )
			{
				fItr.getUVIndex( i, uvIndex[i], &uvSetNameStr );
			}

			switch( n )
			{
				case 3:
				{
					mesh.addTri( pList[0], pList[1], pList[2],
								 uvIndex[0], uvIndex[1], uvIndex[2] );
					break;
				}

				case 4:
				{
					mesh.addQuad( pList[0], pList[1], pList[2], pList[3],
								  uvIndex[0], uvIndex[1], uvIndex[2], uvIndex[3] );
					break;
				}

				default:
				{
					mPrintError( "Invalid # of vertices" );
				}
			}

		} else {

			switch( n )
			{
				case 3:
				{
					mesh.addTri( pList[0], pList[1], pList[2] );
					break;
				}

				case 4:
				{
					mesh.addQuad( pList[0], pList[1], pList[2], pList[3] );
					break;
				}

				default:
				{
					mPrintError( "Invalid # of vertices" );
				}
			}

		}

	}

	return true;
}

bool
Convert( ZMesh& mesh, MObjectArray& meshObjs, const char* uvSetName )
{
	mesh.reset();

	MStatus stat = MS::kSuccess;

	const int numMeshes = (int)meshObjs.length();

	int lastPIdx = 0;
	int lastUVIdx = 0;

	FOR( iMesh, 0, numMeshes )
	{
		MFnMesh meshFn( meshObjs[iMesh], &stat );
		if( !stat )
			{ mPrintError( "Error@Convert(): Failed to get MFnMesh." ); return false; }

		MItMeshVertex vItr( meshObjs[iMesh], &stat );
		if( !stat )
			{ mPrintError( "Error@Convert(): Failed to get MItMeshVertex." ); return false; }

		MItMeshPolygon fItr( meshObjs[iMesh], &stat );
		if( !stat )
			{ mPrintError( "Error@Convert(): Failed to get MItMeshPolygon." ); return false; }

		const int numVertices = meshFn.numVertices();

		MPointArray p( numVertices );	// vertex positions
		MFloatArray u, v;				// uv coordinates

		MMatrix localToWorld;
		bool getWorldMat = GetWorldMatrix( meshObjs[iMesh], localToWorld );

		int vIdx = 0;
		MPoint localPos;

		if( getWorldMat ) {

			for( vItr.reset(); !vItr.isDone(); vItr.next(), ++vIdx )
			{
				localPos = vItr.position( MSpace::kObject );
				ApplyXForm( localToWorld, localPos, p[vIdx] );
			}

		} else {

			for( vItr.reset(); !vItr.isDone(); vItr.next(), ++vIdx )
			{
				p[vIdx] = vItr.position( MSpace::kWorld );
			}

		}

		bool convertUVs = true;
		MString uvSetNameStr( uvSetName );

		if( meshFn.numUVSets()==0 || meshFn.numUVs()==0 ) {

			convertUVs = false;

		} else if( MString(uvSetName) == MString("currentUVSet") ) {

			uvSetNameStr = meshFn.currentUVSetName();
			if( !meshFn.getUVs( u, v, &uvSetNameStr ) ) { convertUVs=false; }

		} else {

			if( !meshFn.getUVs( u, v, &uvSetNameStr ) ) { convertUVs = false; }

		}

		const int numUVs = (int)u.length(); // = v.length();

		ZPoint vp;
		FOR( i, 0, numVertices )
		{
			Copy( vp, p[i] );
			mesh.addPoint( vp );
		}

		if( convertUVs )
		{
			FOR( i, 0, numUVs )
			{
				mesh.addUV( ZPoint( u[i], v[i], 0 ) );
			}
		}

		for( fItr.reset(); !fItr.isDone(); fItr.next() )
		{
			MIntArray pList;
			fItr.getVertices( pList );

			const int n = (int)pList.length();

			if( convertUVs ) {

				int uvIndex[4];

				FOR( i, 0, n )
				{
					fItr.getUVIndex( i, uvIndex[i], &uvSetNameStr );
				}

				switch( n )
				{
					case 3:
					{
						mesh.addTri( pList[0]+lastPIdx, pList[1]+lastPIdx, pList[2]+lastPIdx,
										uvIndex[0]+lastUVIdx, uvIndex[1]+lastUVIdx, uvIndex[2]+lastUVIdx );
						break;
					}

					case 4:
					{
						mesh.addQuad( pList[0]+lastPIdx, pList[1]+lastPIdx, pList[2]+lastPIdx, pList[3]+lastPIdx,
										uvIndex[0]+lastUVIdx, uvIndex[1]+lastUVIdx, uvIndex[2]+lastUVIdx, uvIndex[3]+lastUVIdx );
						break;
					}

					default:
					{
						mPrintError( "Invalid # of vertices" );
					}
				}

			} else {

				switch( n )
				{
					case 3:
					{
						mesh.addTri( pList[0]+lastPIdx, pList[1]+lastPIdx, pList[2]+lastPIdx );
						break;
					}

					case 4:
					{
						mesh.addQuad( pList[0]+lastPIdx, pList[1]+lastPIdx, pList[2]+lastPIdx, pList[3]+lastPIdx );
						break;
					}

					default:
					{
						mPrintError( "Invalid # of vertices" );
					}
				}

			}
		}

		lastPIdx += numVertices;
		lastUVIdx += numUVs;
	}

	return true;
}

ZELOS_NAMESPACE_END

