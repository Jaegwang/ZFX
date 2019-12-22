//----------------//
// ZVoxelizer.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZVoxelizer::ZVoxelizer()
{
	reset();
}

ZVoxelizer::ZVoxelizer( const ZGrid3D& grid )
{
	ZVoxelizer::set( grid );
}

ZVoxelizer::ZVoxelizer( float h, int maxSubdivision, const ZBoundingBox& bBox )
{
	ZVoxelizer::set( h, maxSubdivision, bBox );
}

void
ZVoxelizer::reset()
{
	ZGrid3D::reset();

	_initialState = true;
	_onCell = false;

	_iMax = _jMax = _kMax = 0;
	_eps  = 0.001f;
	_negRange = _posRange = 0.f;

	_posHeap.clear();
	_negHeap.clear();

	_lvs  = (ZScalarField3D*)NULL;
	_vel  = (ZVectorField3D*)NULL;
	_stt  = (ZMarkerField3D*)NULL;

	_vPos = (ZPointArray*)NULL;
	_vVel = (ZVectorArray*)NULL;
	_v012 = (ZInt3Array*)NULL;
}

void
ZVoxelizer::set( const ZGrid3D& grid )
{
	ZVoxelizer::reset();
	ZGrid3D::operator=( grid );
	_h = ZGrid3D::avgCellSize();
}

void
ZVoxelizer::set( float h, int maxSubdivision, const ZBoundingBox& bBox )
{
	ZVoxelizer::reset();
	ZGrid3D::set( h, maxSubdivision, bBox );
	_h = h;
}

void
ZVoxelizer::addMesh( ZScalarField3D& lvs, ZMarkerField3D& stt, ZTriMesh& mesh, float negRange, float posRange )
{
	if( !lvs.location()==ZFieldLocation::zNode || !lvs.location()==ZFieldLocation::zCell )
	{
		cout << "Error@ZVoxelizer::addMesh(): Invalid field location." << endl;
		return;
	}

	if( !lvs.directComputable(stt) )
	{
		cout << "Error@ZVoxelizer::addMesh(): Not computable fields." << endl;
		return;
	}

	if( mesh.numVertices() < 3 )
	{
		lvs.fill( Z_LARGE );
		return;
	}

	_onCell = (lvs.location()==ZFieldLocation::zCell) ? true : false;

	_iMax = lvs.iMax();
	_jMax = lvs.jMax();
	_kMax = lvs.kMax();

	_eps  = 0.001f; // epsilon in voxel space
	_negRange = ZAbs( negRange );
	_posRange = ZAbs( posRange );

	_lvs  = &lvs;
	_vel  = (ZVectorField3D*)NULL;
	_stt  = &stt;

	_vPos = &mesh.p;
	_vVel = (ZVectorArray*)NULL;
	_v012 = &mesh.v012;

	_tagInterfacialElements();
}

void
ZVoxelizer::addMesh( ZScalarField3D& lvs, ZVectorField3D& vel, ZMarkerField3D& stt, ZTriMesh& mesh, ZVectorArray& vVel, float negRange, float posRange )
{
	if( !lvs.location()==ZFieldLocation::zNode || !lvs.location()==ZFieldLocation::zCell )
	{
		cout << "Error@ZVoxelizer::addMesh(): Invalid field location." << endl;
		return;
	}

	if( !lvs.directComputable(stt) || !lvs.directComputable(vel) )
	{
		cout << "Error@ZVoxelizer::addMesh(): Not computable fields." << endl;
		return;
	}

	if( mesh.numVertices() != vVel.length() )
	{
		cout << "Error@ZVoxelizer::addMesh(): Invalid vertex velocities." << endl;
		return;
	}

	if( mesh.numVertices() < 3 )
	{
		lvs.fill( Z_LARGE );
		return;
	}

	_onCell = (lvs.location()==ZFieldLocation::zCell) ? true : false;

	_iMax = lvs.iMax();
	_jMax = lvs.jMax();
	_kMax = lvs.kMax();

	_eps  = 0.001f; // epsilon in voxel space
	_negRange = ZAbs( negRange );
	_posRange = ZAbs( posRange );

	_lvs  = &lvs;
	_vel  = &vel;
	_stt  = &stt;

	_vPos = &mesh.p;
	_vVel = &vVel;
	_v012 = &mesh.v012;

	_tagInterfacialElements();
}

void
ZVoxelizer::_tagInterfacialElements()
{
	ZScalarField3D& lvs = *_lvs;
	ZMarkerField3D& stt = *_stt;
	ZPointArray&   vPos = *_vPos;
	ZInt3Array&    v012 = *_v012;

	const bool hasVel = ( _vel && _vVel );
	ZVector* vel  = hasVel ? (ZVector*)(_vel->pointer())  : (ZVector*)NULL;
	ZVector* vVel = hasVel ? (ZVector*)(_vVel->pointer()) : (ZVector*)NULL;

	const int numVertices = vPos.length();
	const int numTrifaces = v012.length();
	if( !numVertices || !numTrifaces ) { return; }

	if( _initialState )
	{
		if( _lvs ) { lvs.fill( _iMax+_jMax+_kMax ); }
		if( _vel ) { _vel->zeroize();              }
		if( _stt ) { stt.fill( ZFMMState::zFar );   } 

		_initialState = false;
	}

	int     idx, sgn;
	int     i,i0,i1, j,j0,j1, k,k0,k1;
	float   lvsEst;
	double  x,y,z;
	ZFloat3 baryCoords;
	ZPoint  minP, maxP;
	ZPoint  p, p0,p1,p2;
	ZVector triNrm, velEst, v0,v1,v2;

	const ZPoint minPt = lvs.minPoint();
	const float dx  =lvs.dx(), dy  =lvs.dy(), dz  =lvs.dz();
	const float ddx =1/dx,     ddy =1/dy,     ddz =1/dz;
	const float dx_2=0.5f*dx,  dy_2=0.5f*dy,  dz_2=0.5f*dz;

	FOR( iTri, 0, numTrifaces )
	{
		const ZInt3& v = v012[iTri];
		p0=vPos[v[0]]; p1=vPos[v[1]]; p2=vPos[v[2]];
		if( hasVel ) { v0=vVel[v[0]]; v1=vVel[v[1]]; v2=vVel[v[2]]; }

		triNrm = Normal( p0,p1,p2 );

		ZMinMax( p0.x,p1.x,p2.x, minP.x,maxP.x );
		ZMinMax( p0.y,p1.y,p2.y, minP.y,maxP.y );
		ZMinMax( p0.z,p1.z,p2.z, minP.z,maxP.z );

		ZGrid3D::getCellIndex( minP, i0,j0,k0 );
		ZGrid3D::getCellIndex( maxP, i1,j1,k1 );

		i0 = ZClamp( i0, 0, _iMax );
		i1 = ZClamp( i1, 0, _iMax );
		j0 = ZClamp( j0, 0, _jMax );
		j1 = ZClamp( j1, 0, _jMax );
		k0 = ZClamp( k0, 0, _kMax );
		k1 = ZClamp( k1, 0, _kMax );

		if( ZAbs(triNrm.x) > _eps )
		{
			sgn = (triNrm.x>0) ? (+1) : (-1);

			for( k=k0; k<=k1; ++k )
			for( j=j0; j<=j1; ++j )
			{{
				p = lvs.position(0,j,k);

				if( BaryCoords( p, p0,p1,p2, 1, baryCoords ) )
				{
					x = ( baryCoords[0]*p0.x + baryCoords[1]*p1.x + baryCoords[2]*p2.x ) - minPt.x;
					if( _onCell ) { x -= dx_2; }

					if( hasVel ) { velEst = WeightedSum( v0,v1,v2, baryCoords ); }

					const int I = i = int(x*ddx);
					if( i >= 0 && i <= _iMax )
					{
						if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(i*dx-x)) )
						{
							lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
						}

						if( (i=I+1) <= _iMax )
						{
							if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(i*dx-x)) )
							{
								lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
							}

							if( (i=I+2) <= _iMax )
							{
								if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(i*dx-x)) )
								{
									lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
								}
							}
						}

						if( (i=(I-1)) >= 0 )
						{
							if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(i*dx-x)) )
							{
								lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
							}
						}
					}

				}
			}}
		}

		if( ZAbs(triNrm.y) > _eps )
		{
			sgn = (triNrm.y>0) ? (+1) : (-1);

			for( k=k0; k<=k1; ++k )
			for( i=i0; i<=i1; ++i )
			{{
				p = lvs.position(i,0,k);

				if( BaryCoords( p, p0,p1,p2, 2, baryCoords ) )
				{
					y = ( baryCoords[0]*p0.y + baryCoords[1]*p1.y + baryCoords[2]*p2.y ) - minPt.y;
					if( _onCell ) { y -= dy_2; }

					if( hasVel ) { velEst = WeightedSum( v0,v1,v2, baryCoords ); }

					const int J = j = int(y*ddy);
					if( j >= 0 && j <= _jMax )
					{
						if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(j*dy-y)) )
						{
							lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
						}

						if( (j=J+1) <= _jMax )
						{
							if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(j*dy-y)) )
							{
								lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
							}

							if( (j=J+2) <= _jMax )
							{
								if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(j*dy-y)) )
								{
									lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
								}
							}
						}

						if( (j=(J-1)) >= 0 )
						{
							if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(j*dy-y)) )
							{
								lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
							}
						}
					}
				}
			}}
		}

		if( ZAbs(triNrm.z) > _eps )
		{
			sgn = (triNrm.z>0) ? (+1) : (-1);

			for( j=j0; j<=j1; ++j )
			for( i=i0; i<=i1; ++i )
			{{
				p = lvs.position(i,j,0);

				if( BaryCoords( p, p0,p1,p2, 0, baryCoords ) )
				{
					z = ( baryCoords[0]*p0.z + baryCoords[1]*p1.z + baryCoords[2]*p2.z ) - minPt.z;
					if( _onCell ) { z -= dz_2; }

					if( hasVel ) { velEst = WeightedSum( v0,v1,v2, baryCoords ); }

					const int K = k = int(z*ddz);
					if( k >= 0 && k <= _kMax )
					{
						if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(k*dz-z)) )
						{
							lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
						}

						if( (k=K+1) <= _kMax )
						{
							if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(k*dz-z)) )
							{
								lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
							}

							if( (k=K+2) <= _kMax )
							{
								if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(k*dz-z)) )
								{
									lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
								}
							}
						}

						if( (k=(K-1)) >= 0 )
						{
							if( ZAbs(lvs[idx=lvs.index(i,j,k)]) > ZAbs(lvsEst=sgn*(k*dz-z)) )
							{
								lvs[idx]=lvsEst; stt[idx]=ZFMMState::zInterface; if(hasVel){vel[idx]=velEst;}
							}
						}
					}
				}
			}}
		}
	}
}

void
ZVoxelizer::finalize()
{
	if( !_lvs || !_stt ) { return; }

	ZScalarField3D& lvs = *_lvs;
	ZMarkerField3D& stt = *_stt;

	int idx0, idx1;

	// initialize two heaps (both the positive heap and the negative heap)
	PER_EACH_ELEMENT_3D( lvs )

		idx0 = lvs.index(i,j,k);
		if( stt[idx0] != ZFMMState::zInterface ) { continue; }

		if( i != 0 )
		{
			idx1 = lvs.i0(idx0);

			if( stt[idx1] == ZFMMState::zFar )
			{
				stt[idx1] = ZFMMState::zTrial;

				if( lvs[idx0] > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i-1,j,k), lvs[idx0]+1 ) ); }
				else                { _negHeap.push( ZHEAPNODE( ZInt3(i-1,j,k), lvs[idx0]-1 ) ); }
			}
		}

		if( i != _iMax )
		{
			idx1 = lvs.i1(idx0);

			if( stt[idx1] == ZFMMState::zFar )
			{
				stt[idx1] = ZFMMState::zTrial;

				if( lvs[idx0] > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i+1,j,k), lvs[idx0]+1 ) ); }
				else                { _negHeap.push( ZHEAPNODE( ZInt3(i+1,j,k), lvs[idx0]-1 ) ); }
			}
		}

		if( j != 0 )
		{
			idx1 = lvs.j0(idx0);

			if( stt[idx1] == ZFMMState::zFar )
			{
				stt[idx1] = ZFMMState::zTrial;

				if( lvs[idx0] > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i,j-1,k), lvs[idx0]+1 ) ); }
				else                { _negHeap.push( ZHEAPNODE( ZInt3(i,j-1,k), lvs[idx0]-1 ) ); }
			}
		}

		if( j != _jMax )
		{
			idx1 = lvs.j1(idx0);

			if( stt[idx1] == ZFMMState::zFar )
			{
				stt[idx1] = ZFMMState::zTrial;

				if( lvs[idx0] > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i,j+1,k), lvs[idx0]+1 ) ); }
				else                { _negHeap.push( ZHEAPNODE( ZInt3(i,j+1,k), lvs[idx0]-1 ) ); }
			}
		}

		if( k != 0 )
		{
			idx1 = lvs.k0(idx0);

			if( stt[idx1] == ZFMMState::zFar )
			{
				stt[idx1] = ZFMMState::zTrial;

				if( lvs[idx0] > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i,j,k-1), lvs[idx0]+1 ) ); }
				else                { _negHeap.push( ZHEAPNODE( ZInt3(i,j,k-1), lvs[idx0]-1 ) ); }
			}
		}

		if( k != _kMax )
		{
			idx1 = lvs.k1(idx0);

			if( stt[idx1] == ZFMMState::zFar )
			{
				stt[idx1] = ZFMMState::zTrial;

				if( lvs[idx0] > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i,j,k+1), lvs[idx0]+1 ) ); }
				else                { _negHeap.push( ZHEAPNODE( ZInt3(i,j,k+1), lvs[idx0]-1 ) ); }
			}
		}

	END_PER_EACH_3D

	// update & marching
	ZInt3 ijk;
	while( !_negHeap.empty() ) { ijk=_negHeap.top().data; _negHeap.pop(); _update(ijk,-1); }
	while( !_posHeap.empty() ) { ijk=_posHeap.top().data; _posHeap.pop(); _update(ijk,+1); }

	float& minValue = lvs.minValue;
	float& maxValue = lvs.maxValue;

	// scale & set min/max.
	const int nElems = lvs.numElements();
	FOR( i, 0, nElems )
	{
		float& v = lvs[i];
		v *= _h;

		minValue = ZMin( minValue, v );
		maxValue = ZMax( maxValue, v );
	}

	reset();
}

void
ZVoxelizer::_update( const ZInt3& ijk, int sign )
{
	ZScalarField3D& lvs = *_lvs;
	ZVectorField3D& vel = *_vel;
	ZMarkerField3D& stt = *_stt;

	const int &i=ijk[0], &j=ijk[1], &k=ijk[2];
	const int idx0 = lvs.index( i, j, k );

	bool infoExist = false;
	float infoX=Z_LARGE, infoY=Z_LARGE, infoZ=Z_LARGE;

	int idx1, cnt=0;
	ZVector sum;
	if( i !=0     ) { idx1=lvs.i0(idx0); if(ZHasPhi(stt[idx1])){ infoExist=true; infoX=ZMin(infoX,ZAbs(lvs[idx1])); if(_vel){sum+=vel[idx1];++cnt;} } }
	if( i !=_iMax ) { idx1=lvs.i1(idx0); if(ZHasPhi(stt[idx1])){ infoExist=true; infoX=ZMin(infoX,ZAbs(lvs[idx1])); if(_vel){sum+=vel[idx1];++cnt;} } }
	if( j !=0     ) { idx1=lvs.j0(idx0); if(ZHasPhi(stt[idx1])){ infoExist=true; infoY=ZMin(infoY,ZAbs(lvs[idx1])); if(_vel){sum+=vel[idx1];++cnt;} } }
	if( j !=_jMax ) { idx1=lvs.j1(idx0); if(ZHasPhi(stt[idx1])){ infoExist=true; infoY=ZMin(infoY,ZAbs(lvs[idx1])); if(_vel){sum+=vel[idx1];++cnt;} } }
	if( k !=0     ) { idx1=lvs.k0(idx0); if(ZHasPhi(stt[idx1])){ infoExist=true; infoZ=ZMin(infoZ,ZAbs(lvs[idx1])); if(_vel){sum+=vel[idx1];++cnt;} } }
	if( k !=_kMax ) { idx1=lvs.k1(idx0); if(ZHasPhi(stt[idx1])){ infoExist=true; infoZ=ZMin(infoZ,ZAbs(lvs[idx1])); if(_vel){sum+=vel[idx1];++cnt;} } }

	if( !infoExist ) { return; }

	// update
	stt[idx0] = ZFMMState::zUpdated;
	const float lvsEst = ZSolvePhi( infoX, infoY, infoZ );
	ZUpdatePhi( lvs[idx0], sign*lvsEst );
	if( cnt > 0 ) { vel[idx0] = sum * (1/(float)cnt); }

	// narrow band checking
	float& l = lvs[idx0];
	if( lvs[idx0] > 0 ) { if( l >  _posRange ) { return; } }
	else                { if( l < -_negRange ) { return; } }

	// marching
	if( i != 0 )
	{
		idx1 = lvs.i0(idx0);

		if( stt[idx1] == ZFMMState::zFar )
		{
			stt[idx1] = ZFMMState::zTrial;

			if( sign > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i-1,j,k), lvs[idx0]+1 ) ); }
			else           { _negHeap.push( ZHEAPNODE( ZInt3(i-1,j,k), lvs[idx0]-1 ) ); }
		}
	}

	if( i!=_iMax )
	{
		idx1 = lvs.i1(idx0);

		if( stt[idx1] == ZFMMState::zFar )
		{
			stt[idx1] = ZFMMState::zTrial;

			if( sign > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i+1,j,k), lvs[idx0]+1 ) ); }
			else           { _negHeap.push( ZHEAPNODE( ZInt3(i+1,j,k), lvs[idx0]-1 ) ); }
		}
	}

	if( j != 0 )
	{
		idx1 = lvs.j0(idx0);

		if( stt[idx1] == ZFMMState::zFar )
		{
			stt[idx1] = ZFMMState::zTrial;

			if( sign > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i,j-1,k), lvs[idx0]+1 ) ); }
			else           { _negHeap.push( ZHEAPNODE( ZInt3(i,j-1,k), lvs[idx0]-1 ) ); }
		}
	}

	if( j!=_jMax )
	{
		idx1 = lvs.j1(idx0);

		if( stt[idx1] == ZFMMState::zFar )
		{
			stt[idx1] = ZFMMState::zTrial;

			if( sign > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i,j+1,k), lvs[idx0]+1 ) ); }
			else           { _negHeap.push( ZHEAPNODE( ZInt3(i,j+1,k), lvs[idx0]-1 ) ); }
		}
	}

	if( k != 0 )
	{
		idx1 = lvs.k0(idx0);

		if( stt[idx1] == ZFMMState::zFar )
		{
			stt[idx1] = ZFMMState::zTrial;

			if( sign > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i,j,k-1), lvs[idx0]+1 ) ); }
			else           { _negHeap.push( ZHEAPNODE( ZInt3(i,j,k-1), lvs[idx0]-1 ) ); }
		}
	}

	if( k!=_kMax )
	{
		idx1 = lvs.k1(idx0);

		if( stt[idx1] == ZFMMState::zFar )
		{
			stt[idx1] = ZFMMState::zTrial;

			if( sign > 0 ) { _posHeap.push( ZHEAPNODE( ZInt3(i,j,k+1), lvs[idx0]+1 ) ); }
			else           { _negHeap.push( ZHEAPNODE( ZInt3(i,j,k+1), lvs[idx0]-1 ) ); }
		}
	}
}

ostream&
operator<<( ostream& os, const ZVoxelizer& object )
{
	os << "<ZVoxelizer>" << endl;
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

