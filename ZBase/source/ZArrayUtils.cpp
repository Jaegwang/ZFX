//-----------------//
// ZArrayUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.02.27                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

void
Copy( ZFloatArray& to, const ZDoubleArray& from )
{
	const int n = from.length();

	to.setLength( n );

	FOR( i, 0, n )
	{
		to[i] = (float)from[i];
	}
}

void
Copy( ZDoubleArray& to, const ZFloatArray& from )
{
	const int n = from.length();

	to.setLength( n );

	FOR( i, 0, n )
	{
		to[i] = (double)from[i];
	}
}

void
Copy( ZIntArray& to, const ZIntSetArray& from )
{
	const int nTotalElements = from.numTotalElements();

	to.setLength( nTotalElements );

	const int n = from.numSets();

	int idx = 0;

	FOR( i, 0, n )
	{
		const int m = from.count( i );

		FOR( j, 0, m )
		{
			to[idx++] = from(i,j);
		}
	}
}

void
AssignGroups( ZIntArray& groupIds, const ZFloatArray& likelihoods, int randomSeed )
{
	// the number of points
	const int n = groupIds.length();
	if( !n ) { return; }

	// the number of groups (sources)
	const int m = likelihoods.length();
	if( !m ) { groupIds.fill( -1 ); return; }

	ZFloatArray ratio( likelihoods );
	ratio.normalize();

	ZIntArray count;
	{
		count.setLength( m );

		int totalCount = 0;
		double dn = 0.0;

		FOR( j, 0, m )
		{
			double a = n * ratio[j];
			int b = (int)a;

			dn += a - b;

			if( dn > 1.0 )
			{
				dn -= 1.0;
				++b;
			}

			count[j] = b;

			totalCount += b;
		}

		if( totalCount != n )
		{
			count.last() += n - totalCount;
		}
	}

	int i0=0, i1=0;
	FOR( j, 0, m )
	{
		i1 = i0 + count[j];

		for( int i=i0; i<i1; ++i )
		{
			groupIds[i] = j;
		}

		i0 = i1;
	}

	groupIds.shuffle( randomSeed );
}

void
ReverseConnections( const ZIntArray& vCounts, ZIntArray& vConnects )
{
	if( !vCounts.length() ) { return; }
	if( !vConnects.length() ) { return; }

	const int nFaces = vCounts.length();

	// Let's skip the test for efficiency
	//if( vConnects.length() != vCounts.sum() ) { ERROR! }

	int vIdx = 0;

	FOR( i, 0, nFaces )
	{
		const int count = vCounts[i];

		FOR( j, 0, count/2 )
		{
			ZSwap( vConnects[vIdx+j], vConnects[vIdx+count-1-j] );
		}

		vIdx += count;
	}
}

void
ComputeVertexNormals( const ZPointArray& vertexPositions, const ZIntArray& vCounts, const ZIntArray& vConnects, ZVectorArray& vertexNormals )
{
	const int nVerts = vertexPositions.length();
	const int nPolys = vCounts.length();

	vertexNormals.setLength( nVerts );

	ZVector nrm;

	int startIndex = 0;
	FOR( i, 0, nPolys )
	{
		const int& count = vCounts[i];

		if( count >= 3 )
		{
			const int& i0 = startIndex;
			const int  i1 = i0+1;
			const int  i2 = i1+1;

			const int& v0 = vConnects[i0];
			const int& v1 = vConnects[i1];
			const int& v2 = vConnects[i2];

			const ZPoint& p0 = vertexPositions[v0];
			const ZPoint& p1 = vertexPositions[v1];
			const ZPoint& p2 = vertexPositions[v2];

			nrm = Normal( p0, p1, p2 );
			nrm *= Area( p0, p1, p2 );

			FOR( j, 0, count )
			{
				const int& v = vConnects[startIndex+j];
				vertexNormals[v] += nrm;
			}
		}

		startIndex += count;
	}

	FOR( i, 0, nVerts )
	{
		vertexNormals[i].normalize();
	}
}


ZELOS_NAMESPACE_END

