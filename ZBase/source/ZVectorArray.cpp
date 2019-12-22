//------------------//
// ZVectorArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2019.03.26                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZVectorArray::ZVectorArray()
{}

ZVectorArray::ZVectorArray( const ZVectorArray& a )
: ZArray<ZVector>::ZArray( a )
{}

ZVectorArray::ZVectorArray( int l )
: ZArray<ZVector>::ZArray( l )
{}

ZVectorArray::ZVectorArray( int l, const ZVector& v )
: ZArray<ZVector>::ZArray( l, v )
{}

ZPoint
ZPointArray::center() const
{
	ZPoint c;

	const int n = (int)parent::size();
	if( !n ) { return c; }
	const float _n = 1 / (float)n;

	const ZPoint* p = (ZPoint*)this->pointer();

	c = p[0] * _n;

	FOR( i, 1, n )
	{
		ZOP3( c, +=, _n, *, p[i] );
	}

	return c;
}

ZBoundingBox
ZPointArray::boundingBox( bool useOpenMP ) const
{
	ZBoundingBox bBox;

	const size_t N = (size_t)parent::size();
	const ZPoint* p = (ZPoint*)this->pointer();

	if( useOpenMP && N>10000 ) {

		const size_t numCPUs = (size_t)omp_get_num_procs();
		const size_t numThreads = (size_t)(2*numCPUs);

		vector<ZBoundingBox> localBBox( numThreads );

		#pragma omp parallel for
        for( size_t threadId=0; threadId<numThreads; ++threadId )
		{
			const size_t startIdx = (size_t)( (threadId*N)/numThreads );
			const size_t endIdx   = ZMin( startIdx+((N+1)/numThreads ), N );

            for( size_t i=startIdx; i<endIdx; ++i )
            {
			    localBBox[threadId].expand( p[i] );
            }
		}

        for( size_t i=0; i<numThreads; ++i )
		{
			bBox.expand( localBBox[i] );
		}

	} else {

		FOR( i, 0, N )
		{
			bBox.expand( p[i] );
		}

	}

	bBox.expand( Z_EPS );

	return bBox;
}

void
ZPointArray::addPoints( int numPoints, const ZPoint& minPt, const ZPoint& maxPt, int seed )
{
	if( numPoints <= 0 ) { return; }

	const int oldLength = length();
	const int newLength = oldLength + numPoints;

	setLength( newLength, false );

	const float xMin = minPt.x;
	const float yMin = minPt.y;
	const float zMin = minPt.z;

	const float xMax = maxPt.x;
	const float yMax = maxPt.y;
	const float zMax = maxPt.z;

	ZPoint* p = (ZPoint*)this->pointer();

	FOR( i, oldLength, newLength )
	{
		ZPoint& pP = p[i];

		pP.x = ZRand( seed++, xMin, xMax );
		pP.y = ZRand( seed++, yMin, yMax );
		pP.z = ZRand( seed++, zMin, zMax );
	}
}

void
ZVectorArray::getCovarianceMatrix( ZMatrix& cm ) const
{
	const int n = (int)parent::size();

	if( n == 0 )
	{
		cm.zeroize();
		return;
	}

	const ZPoint* p = (ZPoint*)this->pointer();

	ZVector diff;
	const ZPoint c( this->center() );

	double varX  = 0.0;
	double varY  = 0.0;
	double varZ  = 0.0;
	double covXY = 0.0;
	double covXZ = 0.0;
	double covYZ = 0.0;

	FOR( i, 0, n )
	{
		diff = p[i] - c;

		varX  += diff.x * diff.x;
		varY  += diff.y * diff.y;
		varZ  += diff.z * diff.z;

		covXY += diff.x * diff.y;
		covXZ += diff.x * diff.z;
		covYZ += diff.y * diff.z;
	}

	// normalize: divide all of the (co)variances by (n-1).
	// (skipping the division if n=2, since that would be division by 1.0
	if( n > 2 )
	{
		const double denom = 1 / (double)(n-1);

		varX  *= denom;
		varY  *= denom;
		varZ  *= denom;

		covXY *= denom;
		covXZ *= denom;
		covYZ *= denom;
	}

	// pack values into the covariance matrix, which is symmetric
	cm(0,0) = (float)varX;
	cm(1,1) = (float)varY;
	cm(2,2) = (float)varZ;
	cm(1,0) = cm(0,1) = (float)covXY;
	cm(2,0) = cm(0,2) = (float)covXZ;
}

void
ZVectorArray::drawPoints( bool useRandomColor ) const
{
	const int numPtc = parent::length();
	if( !numPtc ) { return; }

	if( useRandomColor ) {

		int seed = 0;

		glBegin( GL_POINTS );

			FOR( i, 0, numPtc )
			{
				glColor( ZRand(i), ZRand(i+13), ZRand(i+31) ); 
				glVertex( parent::at(i) );
			}

		glEnd();

	} else {

		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, &parent::at(0) );
		glDrawArrays( GL_POINTS, 0, numPtc );
		glDisableClientState( GL_VERTEX_ARRAY );

	}
}

void
ZVectorArray::drawVectors( const ZVectorArray& p, float scale, bool useRandomColor ) const
{
	const int numPtc = parent::length();
	if( !numPtc ) { return; }
	if( p.length() != numPtc )
	{
		cout << "Error@ZVectorArray::drawVectors(): Invalid input data." << endl;
		return;
	}

	int seed = 0;

	glBegin( GL_LINES );

		FOR( i, 0, numPtc )
		{
			glColor( ZRand(seed++), ZRand(seed++), ZRand(seed++) ); 
			glLine( p[i], parent::at(i)*scale );
		}

	glEnd();
}

float
ZVectorArray::maxMagnitude() const
{
	const int N = (int)parent::size();
	if(N<=0) {return -1;}
	const ZVector* a = (const ZVector*)parent::pointer();
	float ret = a[0].squaredLength();
	FOR( i, 1, N ) { ret = ZMax( ret, a[i].squaredLength() ); }
	return sqrtf(ret);
}

int
ZVectorArray::minMagnitudeIndex() const
{
	const int N = (int)parent::size();
	if(N<=0) {return -1;}
	const ZVector* a = (const ZVector*)parent::pointer();
    int idx=-1; float min=Z_LARGE;
    FOR(i,0,N) { float l=a[i].length(); if(l<min){min=l; idx=i;} }
    return idx;
}

int
ZVectorArray::maxMagnitudeIndex() const
{
	const int N = (int)parent::size();
	if( N <= 0 ) { return -1; }
	const ZVector* a = (const ZVector*)parent::pointer();
    int idx=-1; float max=-Z_LARGE;
    FOR(i,0,N) { float l=a[i].length(); if(l>max){max=l; idx=i;} }
    return idx;
}

void
ZVectorArray::scale( float v, bool useOpenMP )
{
	const int N = (int)parent::size();
	ZVector* data = pointer();

	#pragma omp parallel for if( useOpenMP && N>10000 )
	FOR( i, 0, N )
	{
		data[i] *= v;
	}
}

void
ZVectorArray::applyTransform( const ZMatrix& matrix, bool asVector, bool useOpenMP )
{
	const int N = (int)parent::size();
	ZVector* data = pointer();

	#pragma omp parallel for if( useOpenMP && N>10000 )
	FOR( i, 0, N )
	{
		ZVector& v = data[i];
		matrix.transform( v, asVector );
	}
}

ostream&
operator<<( ostream& os, const ZVectorArray& object )
{
	os << "<ZVectorArray>" << endl;
	const int size     = object.size();
	const int capacity = object.capacity();

	double bytes = size * sizeof(ZVector);
	os << " size    : " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	bytes = capacity * sizeof(ZVector);
	os << " capacity: " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	os << endl;
	return os;
}

void
PrintLocator( const ZPointArray& ptc )
{
	FOR( i, 0, ptc.size() )
	{
		PrintLocator( ptc[i] );
	}
}

ZELOS_NAMESPACE_END

