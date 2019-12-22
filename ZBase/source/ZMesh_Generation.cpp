//----------------------//
// ZMesh_Generation.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.03.30                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

bool
MakePlane( ZMesh& mesh, float width, float height, int subdWidth, int subdHeight, ZDirection::Direction axis, bool createUVs )
{
	mesh.reset();

	if( ( width < Z_EPS ) || ( height < Z_EPS ) )
	{
		cout << "Error@MakePlane(): Invalid dimension." << endl;
		return false;
	}

	if( ( subdWidth < 1 ) || ( subdHeight < 1 ) )
	{
		cout << "Error@MakePlane(): Invalid subdivision size." << endl;
		return false;
	}

	int idx = 0;

	ZPointArray positions( (subdWidth+1)*(subdHeight+1) );

	switch( axis )
	{
		case ZDirection::xPositive:
		{
			FOR( j, 0, subdHeight+1 ) { float y = height * ( -0.5f + j/(float)subdHeight );
			FOR( i, 0, subdWidth+1  ) {	float x = width  * ( -0.5f + i/(float)subdWidth  );
				positions[idx++].set( 0, x, y );
			}}
			break;
		}

		case ZDirection::yPositive:
		{
			FOR( j, 0, subdHeight+1 ) { float y = height * ( -0.5f + j/(float)subdHeight );
			FOR( i, 0, subdWidth+1  ) {	float x = width  * ( -0.5f + i/(float)subdWidth  );
				positions[idx++].set( y, 0, x );
			}}
			break;
		}

		case ZDirection::zPositive:
		{
			FOR( j, 0, subdHeight+1 ) { float y = height * ( -0.5f + j/(float)subdHeight );
			FOR( i, 0, subdWidth+1  ) {	float x = width  * ( -0.5f + i/(float)subdWidth  );
				positions[idx++].set( x, y, 0 );
			}}
			break;
		}

		default:
		{
			cout << "Error@MakePlane(): Invalid axis type." << endl;
			return false;
		}
	}

	ZIntArray polyCounts( subdWidth*subdHeight );
	ZIntArray polyConnections( 4*subdWidth*subdHeight );

	idx = 0;
	FOR( j, 0, subdHeight ) {
	FOR( i, 0, subdWidth  ) {

		polyCounts[idx] = 4;

		polyConnections[4*idx  ] = (i  )+(subdWidth+1)*(j  );
		polyConnections[4*idx+1] = (i+1)+(subdWidth+1)*(j  );
		polyConnections[4*idx+2] = (i+1)+(subdWidth+1)*(j+1);
		polyConnections[4*idx+3] = (i  )+(subdWidth+1)*(j+1);

		++idx;

	}}

	if( !mesh.create( positions, polyCounts, polyConnections, ZMeshElementType::zFace ) )
	{
		cout << "Error@MakePlane(): Failed to create plane." << endl;
		return false;
	}

	if( !createUVs ) { return true; }

	idx = 0;
	ZPointArray uvs( (subdWidth+1)*(subdHeight+1) );
	FOR( j, 0, subdHeight+1 ) { float y = j / (float)subdHeight;
	FOR( i, 0, subdWidth+1  ) {	float x = i / (float)subdWidth;
		uvs[idx++].set( x, y, 0 );
	}}

	mesh.assignUVs( uvs, polyConnections );

	return true;
}

bool
MakeSphere( ZMesh& mesh, float radius, int subdAxis, int subdHeight, ZDirection::Direction axis, bool createUVs )
{
	mesh.reset();

	if( radius < Z_EPS )
	{
		cout << "Error@MakeSphere(): Invalid radius." << endl;
		return false;
	}

	if( ( subdAxis < 4 ) || ( subdHeight < 4 ) )
	{
		cout << "Error@MakeSpehre(): Invalid subdivision size." << endl;
		return false;
	}

	ZPointArray positions;
	positions.reserve( subdAxis*(subdHeight-1)+2 );

	switch( axis )
	{
		case ZDirection::xPositive:
		{
			FOR( j, 1, subdHeight )
			{
				float phi = ( Z_PI / (float)subdHeight ) * j;
				float r = radius * sinf(phi);
				float x = radius * -cosf(phi);

				FOR( i, 0, subdAxis )
				{
					float theta = ( Z_PIx2 / (float)subdAxis ) * i;

					float z = r * cosf( theta );
					float y = r * sinf( theta );

					positions.push_back( ZPoint(x,y,z) );
				}
			}

			positions.push_back( ZPoint( -radius, 0, 0 ) );
			positions.push_back( ZPoint(  radius, 0, 0 ) );

			break;
		}

		case ZDirection::yPositive:
		{
			FOR( j, 1, subdHeight )
			{
				float phi = ( Z_PI / (float)subdHeight ) * j;
				float r = radius * sinf(phi);
				float y = radius * -cosf(phi);

				FOR( i, 0, subdAxis )
				{
					float theta = ( Z_PIx2 / (float)subdAxis ) * i;

					float x = r * cosf( theta );
					float z = r * sinf( theta );

					positions.push_back( ZPoint(x,y,z) );
				}
			}

			positions.push_back( ZPoint( 0, -radius, 0 ) );
			positions.push_back( ZPoint( 0,  radius, 0 ) );

			break;
		}

		case ZDirection::zPositive:
		{
			FOR( j, 1, subdHeight )
			{
				float phi = ( Z_PI / (float)subdHeight ) * j;
				float r = radius * sinf(phi);
				float z = radius * -cosf(phi);

				FOR( i, 0, subdAxis )
				{
					float theta = ( Z_PIx2 / (float)subdAxis ) * i;

					float y = r * cosf( theta );
					float x = r * sinf( theta );

					positions.push_back( ZPoint(x,y,z) );
				}
			}

			positions.push_back( ZPoint( 0, 0, -radius ) );
			positions.push_back( ZPoint( 0, 0,  radius ) );

			break;
		}

		default:
		{
			cout << "Error@MakeSphere(): Invalid axis type." << endl;
			return false;
		}
	}

	ZIntArray polyCounts;
	polyCounts.reserve( subdAxis*(subdHeight-2)+(2*subdAxis) );

	ZIntArray polyConnections;
	polyConnections.reserve( 4*(subdAxis*(subdHeight-2))+3*(2*subdAxis) );

	FOR( j, 0, subdHeight-2 )
	{
		FOR( i, 0, subdAxis )
		{
			polyCounts.push_back( 4 );

			int i_next = (i==subdAxis-1) ? 0 : (i+1);

			polyConnections.push_back( i     +subdAxis*(j  ) );
			polyConnections.push_back( i     +subdAxis*(j+1) );
			polyConnections.push_back( i_next+subdAxis*(j+1) );
			polyConnections.push_back( i_next+subdAxis*(j  ) );
		}
	}

	FOR( i, 0, subdAxis )
	{
		polyCounts.push_back( 3 );

		int i_next = (i==subdAxis-1) ? 0 : (i+1);

		polyConnections.push_back( i                    );
		polyConnections.push_back( i_next               );
		polyConnections.push_back( positions.length()-2 );
	}

	FOR( i, 0, subdAxis )
	{
		polyCounts.push_back( 3 );

		int i_next = (i==subdAxis-1) ? 0 : (i+1);

		polyConnections.push_back( i_next+subdAxis*(subdHeight-2) );
		polyConnections.push_back( i     +subdAxis*(subdHeight-2) );
		polyConnections.push_back( positions.length()-1           );
	}

	if( !mesh.create( positions, polyCounts, polyConnections, ZMeshElementType::zFace ) )
	{
		cout << "Error@MakePlane(): Failed to create plane." << endl;
		return false;
	}

	if( !createUVs ) { return true; }

	ZPointArray uvs;
	uvs.reserve( (subdAxis+1)*(subdHeight-1)+2*subdAxis );

	FOR( j, 1, subdHeight )
	{
		FOR( i, 0, subdAxis+1 )
		{
			float u = i/(float)subdAxis;
			float v = j/(float)subdHeight;

			uvs.push_back( ZPoint(u,v,0) );
		}
	}

	FOR( i, 0, subdAxis )
	{
		float u = (i+0.5f)/(float)subdAxis;
		float v = 0;

		uvs.push_back( ZPoint(u,v,0) );
	}

	FOR( i, 0, subdAxis )
	{
		float u = (i+0.5f)/(float)subdAxis;
		float v = 1;

		uvs.push_back( ZPoint(u,v,0) );
	}

	ZIntArray uvIndices;
	uvIndices.reserve( 4*(subdAxis*(subdHeight-2))+3*(2*subdAxis) );

	FOR( j, 0, subdHeight-2 )
	{
		FOR( i, 0, subdAxis )
		{
			uvIndices.push_back( (i  )+(j  )*(subdAxis+1) );
			uvIndices.push_back( (i  )+(j+1)*(subdAxis+1) );
			uvIndices.push_back( (i+1)+(j+1)*(subdAxis+1) );
			uvIndices.push_back( (i+1)+(j  )*(subdAxis+1) );
		}
	}

	FOR( i, 0, subdAxis )
	{
		uvIndices.push_back( i                             );
		uvIndices.push_back( i+1                           );
		uvIndices.push_back( i+(subdAxis+1)*(subdHeight-1) );
	}

	FOR( i, 0, subdAxis )
	{
		uvIndices.push_back( (i+1)+(subdAxis+1)*(subdHeight-2)      );
		uvIndices.push_back( (i  )+(subdAxis+1)*(subdHeight-2)      );
		uvIndices.push_back( i+(subdAxis+1)*(subdHeight-1)+subdAxis );
	}

	mesh.assignUVs( uvs, uvIndices );

	return true;
}

ZELOS_NAMESPACE_END

