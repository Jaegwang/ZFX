//-------------//
// ZCurves.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2019.03.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZCurves::ZCurves()
{
}

ZCurves::ZCurves( const ZCurves& curve )
{
	*this = curve;
}

ZCurves::ZCurves( const ZIntArray& nCVs )
{
	set( nCVs );
}

ZCurves::ZCurves( const char* filePathName )
{
	load( filePathName );
}

void
ZCurves::reset()
{
	_numCVs   .clear();
	_startIdx .clear();
	_cv       .clear();
}

ZCurves&
ZCurves::operator=( const ZCurves& other )
{
	_numCVs   = other._numCVs;
	_startIdx = other._startIdx;
	_cv       = other._cv;

	return (*this);
}

void
ZCurves::set( const ZIntArray& nCVs )
{
	_numCVs = nCVs;

	_allocate();
}

void
ZCurves::zeroize()
{
	_cv.zeroize();
}

void
ZCurves::from( const vector<ZCurves>& curve, bool memAlloc )
{
	const int nGroups = (int)curve.size();
	if( !nGroups ) { reset(); return; }

	// memory allocation
	if( memAlloc )
	{
		int nTotalCurves = 0;
		FOR( i, 0, nGroups )
		{
			nTotalCurves += curve[i].numCurves();
		}

		ZIntArray nCVs( nTotalCurves );

		int idx = 0;

		FOR( i, 0, nGroups )
		{
			const ZCurves& curves = curve[i];
			const int nCurves = curves.numCurves();

			FOR( j, 0, nCurves )
			{
				nCVs[idx++] = curves.numCVs(j);
			}
		}

		set( nCVs );
	}

	// data copy
	{
		int idx = 0;

		FOR( i, 0, nGroups )
		{
			const ZCurves& curves = curve[i];
			const int nCurves = curves.numCurves();

			FOR( j, 0, nCurves )
			{
				const int nCVs = curves.numCVs(j);

				FOR( k, 0, nCVs )
				{
					_cv[idx++] = curves.cv(j,k);
				}
			}
		}
	}
}

void
ZCurves::from( const ZCurves& other, const ZCharArray& mask )
{
	ZCurves::reset();

	const int nCurves = other.numCurves();

	if( nCurves != mask.length() )
	{
		cout << "Error@ZCurves::from(): Invalid input data." << endl;
		return;
	}

	int count = 0;
	{
		FOR( i, 0, nCurves )
		{
			if( mask[i] )
			{
				++count;
			}
		}
	}

	_numCVs.setLength( count );
	_startIdx.setLength( count );

	int idx = 0;
	int nTotalCVs = 0;
	FOR( i, 0, nCurves )
	{
		if( mask[i] )
		{
			_startIdx[idx] = nTotalCVs;
			nTotalCVs += ( _numCVs[idx] = other._numCVs[i] );

			++idx;
		}
	}

	_cv.reserve( nTotalCVs );

	FOR( i, 0, nCurves )
	{
		if( mask[i] )
		{
			const int& nCVs = other.numCVs(i);

			FOR( j, 0, nCVs )
			{
				_cv.push_back( other.cv(i,j) );
			}
		}
	}
}

void
ZCurves::to( ZPointSetArray& cvs ) const
{
	cvs.set( _numCVs );
	const int nTotalCVs = _cv.length();
	memcpy( (char*)&cvs.start(0).x, (char*)&_cv[0].x, nTotalCVs*sizeof(ZPoint) );
}

void
ZCurves::append( const ZCurves& other )
{
	const int nCurvesOld = _numCVs.length();
	const int nTotalCVs = ZCurves::numTotalCVs();

	_numCVs.append( other._numCVs );
	_startIdx.append( other._startIdx );
	_cv.append( other._cv );

	const int nCurvesNew = _numCVs.length();

	FOR( i, nCurvesOld, nCurvesNew )
	{
		_startIdx[i] += nTotalCVs;
	}
}

// _numCVs must be set before!
void
ZCurves::_allocate()
{
	const int nCurves = _numCVs.length();
	_startIdx.setLength( nCurves );

	int nTotalCVs = 0;
	FOR( i, 0, nCurves )
	{
		_startIdx[i] = nTotalCVs;
		nTotalCVs += _numCVs[i];
	}

	_cv.setLength( nTotalCVs );
}

ZPoint
ZCurves::position( int i, float t ) const
{
	int idx[4];
	_whereIsIt( i, t, idx );

	const ZPoint& P0 = cv( i, idx[0] );
	const ZPoint& P1 = cv( i, idx[1] );
	const ZPoint& P2 = cv( i, idx[2] );
	const ZPoint& P3 = cv( i, idx[3] );

	return _zeroDerivative( t, P0, P1, P2, P3 );
}

ZVector
ZCurves::tangent( int i, float t ) const
{
	int idx[4];
	_whereIsIt( i, t, idx );

	const ZPoint& P0 = cv( i, idx[0] );
	const ZPoint& P1 = cv( i, idx[1] );
	const ZPoint& P2 = cv( i, idx[2] );
	const ZPoint& P3 = cv( i, idx[3] );

	return _firstDerivative( t, P0, P1, P2, P3 ).normalize();
}

ZVector
ZCurves::normal( int i, float t ) const
{
	return ( biNormal(i,t) ^ tangent(i,t) ).normalize();
}

// old implementation
//{
//	int idx[4];
//	_whereIsIt( i, t, idx );
//
//	const ZPoint& P0 = cv( i, idx[0] );
//	const ZPoint& P1 = cv( i, idx[1] );
//	const ZPoint& P2 = cv( i, idx[2] );
//	const ZPoint& P3 = cv( i, idx[3] );
//
//	ZVector vel( _firstDerivative ( t, P0, P1, P2, P3 ) );
//	ZVector acc( _secondDerivative( t, P0, P1, P2, P3 ) );
//
//	const float vv = vel * vel;
//	const float va = vel * acc;
//
//	ZVector nrm( WeightedSum( vel, acc, -va, vv ) );
//
//	const float lenSQ = nrm.squaredLength();
//
//	if( ZAlmostZero(lenSQ) ) {
//
//		nrm.zeroize();
//
//		const ZVector a( P3 - P0 );
//
//		if( ZAbs(a.x ) < ZAbs(a.y) ) {
//			if( ZAbs(a.z ) < ZAbs(a.y) ) { nrm.z = ZSgn(a.y); }
//			else                         { nrm.x = ZSgn(a.z); }
//		} else {
//			if( ZAbs(a.z ) < ZAbs(a.x) ) { nrm.y = ZSgn(a.x); }
//			else                         { nrm.x = ZSgn(a.z); }
//		}
//
//	} else {
//
//		nrm *= 1 / sqrtf(lenSQ);
//
//	}
//
//	return nrm;
//}

ZVector
ZCurves::biNormal( int i, float t ) const
{
	int idx[4];
	_whereIsIt( i, t, idx );

	const ZPoint& P0 = cv( i, idx[0] );
	const ZPoint& P1 = cv( i, idx[1] );
	const ZPoint& P2 = cv( i, idx[2] );
	const ZPoint& P3 = cv( i, idx[3] );

	ZVector vel( _firstDerivative ( t, P0, P1, P2, P3 ) );
	ZVector acc( _secondDerivative( t, P0, P1, P2, P3 ) );

	return ( vel ^ acc ).normalize();
}

// old implementation
//{
//	int idx[4];
//	_whereIsIt( i, t, idx );
//
//	const ZPoint& P0 = cv( i, idx[0] );
//	const ZPoint& P1 = cv( i, idx[1] );
//	const ZPoint& P2 = cv( i, idx[2] );
//	const ZPoint& P3 = cv( i, idx[3] );
//
//	ZVector vel( _firstDerivative ( t, P0, P1, P2, P3 ) );
//	ZVector acc( _secondDerivative( t, P0, P1, P2, P3 ) );
//
//	const float vv = vel * vel;
//	const float va = vel * acc;
//
//	ZVector nrm( WeightedSum( vel, acc, -va, vv ) );
//
//	const float lenSQ = nrm.squaredLength();
//
//	if( ZAlmostZero(lenSQ) ) {
//
//		nrm.zeroize();
//
//		const ZVector a( P3 - P0 );
//
//		if( ZAbs(a.x ) < ZAbs(a.y) ) {
//			if( ZAbs(a.z ) < ZAbs(a.y) ) { nrm.z = ZSgn(a.y); }
//			else                         { nrm.x = ZSgn(a.z); }
//		} else {
//			if( ZAbs(a.z ) < ZAbs(a.x) ) { nrm.y = ZSgn(a.x); }
//			else                         { nrm.x = ZSgn(a.z); }
//		}
//
//	} else {
//
//		nrm *= 1 / sqrtf(lenSQ);
//
//	}
//
//	return (vel^nrm).normalize();
//}

float
ZCurves::speed( int i, float t ) const
{
	int idx[4];
	_whereIsIt( i, t, idx );

	const ZPoint& P0 = cv( i, idx[0] );
	const ZPoint& P1 = cv( i, idx[1] );
	const ZPoint& P2 = cv( i, idx[2] );
	const ZPoint& P3 = cv( i, idx[3] );

	return _firstDerivative( t, P0, P1, P2, P3 ).length();
}

void
ZCurves::getPositionAndTangent( int i, float t, ZPoint& P, ZVector& T ) const
{
	int idx[4];
	_whereIsIt( i, t, idx );

	const ZPoint& P0 = cv( i, idx[0] );
	const ZPoint& P1 = cv( i, idx[1] );
	const ZPoint& P2 = cv( i, idx[2] );
	const ZPoint& P3 = cv( i, idx[3] );

	P = _zeroDerivative( t, P0, P1, P2, P3 );
	T = _firstDerivative( t, P0, P1, P2, P3 ).normalize();
}

// ref) Parallel Poisson Disk Sampling with Spectrum Analysis on Surfaces: eqn. (1)
float
ZCurves::curveLength( int i ) const
{
	const int& nCVs   = _numCVs[i];
	const int  nCVs_1 = nCVs-1;

	const float dt = 1 / (float)nCVs_1;

	ZVectorArray nrm( nCVs );
	for( int l=0; l<nCVs; ++l ) { nrm[l] = normal( i, l*dt ); }

	float totalLength = 0;

	int     i0, i1;
	float   de, c0, c1;
	ZVector dir, n0, n1;

	for( int i0=0; i0<nCVs_1; ++i0 )
	{
		i1 = i0+1;

		const ZPoint& p0 = cv( i,i0);
		const ZPoint& p1 = cv( i,i1);

		dir = p1 - p0;
		de = dir.length();
		if( ZAlmostZero(de) ) { continue; }
		dir *= 1 / de;

		c0 = ZClamp( nrm[i0]*dir, -1.f, 1.f );
		c1 = ZClamp( nrm[i1]*dir, -1.f, 1.f );

		if( ZAlmostSame( c0, c1 ) ) {
			totalLength += de;
		} else {
			totalLength += de * ( asinf(c1) - asinf(c0) ) / ( c1 - c0 );
		}
	}

	return totalLength;
}

float
ZCurves::curveLength( int i, float t0, float t1 ) const
{
	t0 = ZClamp( t0, 0.f, 1.f );
	t1 = ZClamp( t1, 0.f, 1.f );

	if( t0 > t1 ) { ZSwap(t0,t1); }

	const int& nCVs   = _numCVs[i];
	const int  nCVs_1 = nCVs-1;

	float dt = 1 / (float)nCVs_1;

	int idx0 = (t0==0) ? 0      : ZCeil ( t0 / dt );
	int idx1 = (t1==1) ? nCVs_1 : ZFloor( t1 / dt );
	int diff = idx1 - idx0;

	ZVectorArray nrm( diff+1 );
	for( int l=0; l<=diff; ++l ) { nrm[l] = normal( i, (l+idx0)*dt ); }

	float totalLength = 0;

	ZPoint  p0, p1;
	ZVector dir;
	float   de, c0, c1;

	// for the first segment
	p0 = position(i,t0);
	p1 = cv(i,idx0);

	dir = p1 - p0;
	de = dir.length();

	if( !ZAlmostZero(de) )
	{
		dir *= 1 / de;

		c0 = ZClamp( normal(i,t0)*dir, -1.f, 1.f );
		c1 = ZClamp( nrm[0]      *dir, -1.f, 1.f );

		if( ZAlmostSame( c0, c1 ) ) {
			totalLength += de;
		} else {
			totalLength += de * ( asinf(c1) - asinf(c0) ) / ( c1 - c0 );
		}
	}

	// for the last segment
	p0 = cv(i,idx1);
	p1 = position(i,t1);

	dir = p1 - p0;
	de = dir.length();

	if( !ZAlmostZero(de) )
	{
		dir *= 1 / de;

		c0 = ZClamp( nrm[diff]   *dir, -1.f, 1.f );
		c1 = ZClamp( normal(i,t1)*dir, -1.f, 1.f );

		if( ZAlmostSame( c0, c1 ) ) {
			totalLength += de;
		} else {
			totalLength += de * ( asinf(c1) - asinf(c0) ) / ( c1 - c0 );
		}
	}

	// for the other segments
	int i1;
	for( int i0=idx0, idx=idx0; i0<idx1; ++i0, ++idx )
	{
		i1 = i0+1;

		const ZPoint& p0 = cv( i,i0);
		const ZPoint& p1 = cv( i,i1);

		dir = p1 - p0;
		de = dir.length();
		if( ZAlmostZero(de) ) { continue; }
		dir *= 1 / de;

		c0 = ZClamp( nrm[idx  ]*dir, -1.f, 1.f );
		c1 = ZClamp( nrm[idx+1]*dir, -1.f, 1.f );

		if( ZAlmostSame( c0, c1 ) ) {
			totalLength += de;
		} else {
			totalLength += de * ( asinf(c1) - asinf(c0) ) / ( c1 - c0 );
		}
	}

	return totalLength;
}

void
ZCurves::getCurveLengths( ZFloatArray& curveLengths, bool useOpenMP ) const
{
	const int nCurves = numCurves();
	curveLengths.setLength( nCurves );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nCurves )
	{
		curveLengths[i] = curveLength( i );
	}
}

float
ZCurves::lineLength( int i ) const
{
	const int& nCVs   = _numCVs[i];
	const int  nCVs_1 = nCVs-1;

	float totalLength = 0;

	int i0, i1;
	for( int i0=0; i0<nCVs_1; ++i0 )
	{
		i1 = i0+1;

		const ZPoint& p0 = cv(i,i0);
		const ZPoint& p1 = cv(i,i1);

		totalLength += p0.distanceTo( p1 );
	}

	return totalLength;
}

void
ZCurves::getLineLengths( ZFloatArray& curveLengths, bool useOpenMP ) const
{
	const int nCurves = numCurves();
	curveLengths.setLength( nCurves );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nCurves )
	{
		curveLengths[i] = lineLength( i );
	}
}

ZBoundingBox
ZCurves::boundingBox( int i ) const
{
    ZBoundingBox box;

    const int& i0 = _startIdx[i];
    const int& i1 = _startIdx[i] + _numCVs[i];

    FOR( i, i0, i1 )
    {
        box.expand( _cv(i) );
    }

    return box;
}

ZBoundingBox
ZCurves::boundingBox( bool onlyEndPoints, bool useOpenMP ) const
{
	if( onlyEndPoints ) {

		const int nCurves = numCurves();

		ZPointArray ends( nCurves );

		FOR( i, 0, nCurves )
		{
			ends[i] = tip(i);
		}

		return ends.boundingBox( useOpenMP );

	} else {

		return _cv.boundingBox( useOpenMP );

	}
}

void
ZCurves::getRootPoints( ZPointArray& rootPoints ) const
{
	const int nCurves = numCurves();

	rootPoints.setLength( nCurves );

	FOR( i, 0, nCurves )
	{
		rootPoints[i] = _cv[ _startIdx[i] ];
	}
}

int
ZCurves::deformByCCDIK( int i, const ZPoint& target, float rigidity, float maxAngleInDegrees, int maxIters, float tol )
{
	const int nCVs = _numCVs[i];
	const int nCVs_1 = nCVs-1;

	float maxAngle = ZDegToRad( maxAngleInDegrees );
	maxIters = ZMin( maxIters, nCVs_1 );

	const ZPoint& r = root(i);
	const ZPoint& t = tip(i);

	int itr = 0;
	float ab, rotAngle;
	ZVector a, b, rotAxis;

	if( rigidity > Z_EPS )
	{
		a = t - r;
		b = target - r;

		rotAxis = (a^b).normalize();
		rotAngle = rigidity * Angle( a, b );

		if( ZRadToDeg(rotAngle) > 0.001f )
		{
			FOR( j, 1, nCVs )
			{
				ZPoint& p = cv(i,j);
				p = Rotate( p, rotAxis, rotAngle, r );
			}
		}
	}

	while( itr < maxIters && t.squaredDistanceTo(target) > tol )
	{
		for( int j=nCVs_1; j>0; --j )
		{
			const ZVector& rotCenter = cv(i,j-1);

			a = t      - rotCenter; // current direction
			b = target - rotCenter; // target  direction

			a.normalize();
			b.normalize();

			if( (a*b) < 0.99999f )
			{
				rotAxis = (a^b).normalize();
				rotAngle = Angle( a, b );

				if( ZRadToDeg(rotAngle) > 0.001f )
				{
					FOR( k, j+1, nCVs )
					{
						ZPoint& p = cv(i,k);
						p = Rotate( p, rotAxis, rotAngle, rotCenter );
					}
				}
			}
		}

		++itr;
	}

	return itr;
}

void
ZCurves::write( ofstream& fout ) const
{
	// other data don't need to be saved.
	_numCVs.write( fout, true );
	_cv.write( fout, true );
}

void
ZCurves::read( ifstream& fin )
{
	_numCVs.read( fin, true );
	_allocate();
	_cv.read( fin, true );
}

bool
ZCurves::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZCurves::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	write( fout );

	fout.close();

	return true;
}

bool
ZCurves::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZCurves::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	read( fin );

	fin.close();

	return true;
}

void
ZCurves::drawLine( int i, float dt ) const
{
	const int& nCVs = _numCVs[i];

	if( dt <= 0.f ) {

		glBegin( GL_LINE_STRIP );
			FOR( j, 0, nCVs )
			{
				glVertex( cv(i,j) );
			}
		glEnd();

	} else {

		glBegin( GL_LINE_STRIP );
			for( float t=0.f; t<1.f; t+=dt )
			{
				glVertex( position(i,t) );
			}
			glVertex( tip(i) );
		glEnd();

	}
}

void
ZCurves::drawCVs( int i ) const
{
	const int& nCVs = _numCVs[i];

	glBegin( GL_POINTS );
		FOR( j, 0, nCVs )
		{
			glVertex( cv(i,j) );
		}
	glEnd();
}

double
ZCurves::usedMemorySize( ZDataUnit::DataUnit dataUnit ) const
{
	return ( _numCVs.usedMemorySize(dataUnit) + _startIdx.usedMemorySize(dataUnit) + _cv.usedMemorySize(dataUnit) );
}

void
ZCurves::drawLines( float dt ) const
{
	const int nCurves = _numCVs.length();

	if( dt <= 0 ) {

		FOR( i, 0, nCurves )
		{
			const int& nCVs = _numCVs[i];

			glBegin( GL_LINE_STRIP );
				FOR( j, 0, nCVs ) { glVertex( cv(i,j) ); }
			glEnd();
		}

	} else {

		FOR( i, 0, nCurves )
		{
			glBegin( GL_LINE_STRIP );
				for( float t=0.f; t<1.f; t+=dt ) { glVertex( position(i,t) ); }
				glVertex( tip(i) );
			glEnd();
		}

	}
}

void
ZCurves::drawLines( float dt, ZColor color, float width, float cRatio, float wRatio ) const
{
	const int nCurves = _numCVs.length();

	const ZColor rc( color * cRatio );
	const ZColor tc( color          );
	const float  rw( width          );
	const float  tw( width * wRatio );

	const bool smoothColor = (cRatio<1.f) ? true : false;
	const bool smoothWidth = (wRatio<1.f) ? true : false;

	if( !smoothWidth ) { glLineWidth( width ); }
	if( !smoothColor ) { glColor( color ); }

	float t=0.f, s=0.f;

	if( dt <= 0 ) {

		FOR( i, 0, nCurves )
		{
			const int& nCVs = _numCVs[i];
			const int nCVs_1 = nCVs-1;

			t = 0.f;
			dt = 1 / (float)(nCVs_1);

			FOR( j, 0, nCVs_1 )
			{
				if( smoothWidth )
				{
					s = 1.f - t;
					glLineWidth( s*rw+t*tw );
				}

				glBegin( GL_LINES );
				{
					if( smoothColor )
					{
						glColor( s*rc.r+t*tc.r, s*rc.g+t*tc.g, s*rc.b+t*tc.b );
					}
					glVertex( cv(i,j) );

					t += dt;

					if( smoothColor )
					{
						s = 1.f - t;
						glColor( s*rc.r+t*tc.r, s*rc.g+t*tc.g, s*rc.b+t*tc.b );
					}
					glVertex( cv(i,j+1) );
				}
				glEnd();
			}
		}

	} else {

		dt = ZClamp( dt, 0.f, 1.f );

		FOR( i, 0, nCurves )
		{
			for( float t=0.f; t<1.f; t+=dt )
			{
				if( smoothWidth )
				{
					s = 1.f - t;
					glLineWidth( s*rw+t*tw );
				}

				glBegin( GL_LINES );
				{
					if( smoothColor )
					{
						glColor( s*rc.r+t*tc.r, s*rc.g+t*tc.g, s*rc.b+t*tc.b );
					}
					glVertex( position(i,t) );

					t += dt;

					if( smoothColor )
					{
						if( t > 1.f ) { t = 1.f; }
						s = 1.f - t;
						glColor( s*rc.r+t*tc.r, s*rc.g+t*tc.g, s*rc.b+t*tc.b );
					}
					glVertex( position(i,t) );

					t -= dt;
				}
				glEnd();
			}
		}

	}
}

void
ZCurves::drawLines( float dt, ZCharArray& display, ZColorArray& color )
{
	const int nCurves = _numCVs.length();

	if( ( nCurves != display.length() ) || ( nCurves != color.length() ) )
	{
		cout << "ZCurves::drawLines(): Invalid input data." << endl;
		return;
	}

	if( dt <= 0 ) {

		FOR( i, 0, nCurves )
		{
			if( !display[i] ) { continue; }
			glColor( color[i] );

			const int& nCVs = _numCVs[i];

			glBegin( GL_LINE_STRIP );
				FOR( j, 0, nCVs ) { glVertex( cv(i,j) ); }
			glEnd();
		}

	} else {

		FOR( i, 0, nCurves )
		{
			if( !display[i] ) { continue; }
			glColor( color[i] );

			glBegin( GL_LINE_STRIP );
				for( float t=0; t<1; t+=dt ) { glVertex( position(i,t) ); }
				glVertex( tip(i) );
			glEnd();
		}

	}
}

void
ZCurves::drawLines( float dt, ZCharArray& display, ZColorArray& color, float width, float cRatio, float wRatio ) const
{
	const int nCurves = _numCVs.length();

	if( ( nCurves != display.length() ) || ( nCurves != color.length() ) )
	{
		cout << "ZCurves::drawLines(): Invalid input data." << endl;
		return;
	}

	const float rw( width          );
	const float tw( width * wRatio );

	const bool smoothColor = (cRatio<1.f) ? true : false;
	const bool smoothWidth = (wRatio<1.f) ? true : false;

	if( !smoothWidth ) { glLineWidth( width ); }

	float t=0.f, s=0.f;

	if( dt <= 0 ) {

		FOR( i, 0, nCurves )
		{
			if( !display[i] ) { continue; }

			const int& nCVs = _numCVs[i];
			const int nCVs_1 = nCVs-1;

			t = 0.f;
			dt = 1 / (float)(nCVs_1);

			const ZColor& tc = color[i];
			const ZColor rc( tc * cRatio );

			if( !smoothColor ) { glColor( tc ); }

			FOR( j, 0, nCVs_1 )
			{
				if( smoothWidth )
				{
					s = 1.f - t;
					glLineWidth( s*rw+t*tw );
				}

				glBegin( GL_LINES );
				{
					if( smoothColor )
					{
						glColor( s*rc.r+t*tc.r, s*rc.g+t*tc.g, s*rc.b+t*tc.b );
					}
					glVertex( cv(i,j) );

					t += dt;

					if( smoothColor )
					{
						s = 1.f - t;
						glColor( s*rc.r+t*tc.r, s*rc.g+t*tc.g, s*rc.b+t*tc.b );
					}
					glVertex( cv(i,j+1) );
				}
				glEnd();
			}
		}

	} else {

		FOR( i, 0, nCurves )
		{
			if( !display[i] ) { continue; }
			glColor( color[i] );

			glBegin( GL_LINE_STRIP );
				for( float t=0; t<1; t+=dt ) { glVertex( position(i,t) ); }
				glVertex( tip(i) );
			glEnd();
		}

	}
}

void
ZCurves::drawCVs() const
{
	_cv.drawPoints();
}

void
ZCurves::drawCVs( ZCharArray& display, ZColorArray& color )
{
	const int nCurves = _numCVs.length();

	FOR( i, 0, nCurves )
	{
		if( !display[i] ) { continue; }
		glColor( color[i] );

		const int& nCVs = _numCVs[i];

		glBegin( GL_POINTS );
			FOR( j, 0, nCVs ) { glVertex( cv(i,j) ); }
		glEnd();
	}
}

ostream&
operator<<( ostream& os, const ZCurves& object )
{
	os << "<ZCurves>" << endl;
	os << " # of curves   : " << object.numCurves() << endl;
	os << " # of total CVs: " << object.numTotalCVs() << endl;
	return os;
}

ZELOS_NAMESPACE_END

