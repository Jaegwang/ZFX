//--------------//
// ZFrustum.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

const int ZFrustum::triangleIndices[12][3] =
{
	 { 0, 1, 2 }, { 0, 2, 3 }, // near
	 { 6, 5, 4 }, { 6, 4, 7 }, // far
	 { 0, 3, 7 }, { 0, 7, 4 }, // left
	 { 1, 5, 6 }, { 1, 6, 2 }, // right
	 { 2, 6, 7 }, { 2, 7, 3 }, // top
	 { 1, 0, 4 }, { 1, 4, 5 }  // bottom
};

ZFrustum::ZFrustum()
{
	ZFrustum::reset();
}

ZFrustum::ZFrustum( const ZFrustum& frustum )
{
	*this = frustum;
}

void
ZFrustum::reset()
{
	_initialAim = ZDirection::zNegative; // Maya default

	_near          = 0.f;
	_far           = 0.f;
	_horizontalFOV = 0.f;
	_verticalFOV   = 0.f;
	_width         = 0.f;
	_height        = 0.f;
	_aspectRatio   = 0.f;

	_eyePosition.zeroize();

	_xform.setToIdentity();

	FOR( i, 0, 8 )
	{
		_localCorners[i].zeroize();
		_worldCorners[i].zeroize();
	}

	FOR( i, 0, 6 )
	{
		_planes[i].reset();
	}
}

ZFrustum&
ZFrustum::set( float near, float far, float horizontalFOV, float aspectRatio, ZDirection::Direction initialAim )
{
	// Don't reset!
	// Because it will touch the current transformation.
	//ZFrustum::reset();

	_initialAim = initialAim;

	if( ZAbs(near) > ZAbs(far) )
	{
		return (*this);
	}

	_near          = ZAbs( near );
	_far           = ZAbs( far );
	_horizontalFOV = ZAbs( horizontalFOV );
	_aspectRatio   = ZAbs( aspectRatio );

	_width         = 2.f * _near * tanf( 0.5f * _horizontalFOV );
	_height        = _width / _aspectRatio;
	_verticalFOV   = atan2f( 0.5f * _height, _near );

	// half width and height in the near and far plane, respectively
	const float wNear = 0.5f * _width;
	const float hNear = 0.5f * _height;
	const float wFar  = ( far / near ) * wNear;
	const float hFar  = ( far / near ) * hNear;

	if( _initialAim == ZDirection::zNegative ) // Maya default
	{
		_worldCorners[0] = _localCorners[0].set( -wNear, -hNear, -_near );
		_worldCorners[1] = _localCorners[1].set( +wNear, -hNear, -_near );
		_worldCorners[2] = _localCorners[2].set( +wNear, +hNear, -_near );
		_worldCorners[3] = _localCorners[3].set( -wNear, +hNear, -_near );
		_worldCorners[4] = _localCorners[4].set( -wFar,  -hFar,  -_far  );
		_worldCorners[5] = _localCorners[5].set( +wFar,  -hFar,  -_far  );
		_worldCorners[6] = _localCorners[6].set( +wFar,  +hFar,  -_far  );
		_worldCorners[7] = _localCorners[7].set( -wFar,  +hFar,  -_far  );
	}
	else
	{
		// TO DO: other cases
	}

	// to be outward unit normals
	_planes[0].set( _worldCorners[0], _worldCorners[1], _worldCorners[2] ); // near   plane
	_planes[1].set( _worldCorners[6], _worldCorners[5], _worldCorners[4] ); // far    plane
	_planes[2].set( _worldCorners[0], _worldCorners[3], _worldCorners[7] ); // left   plane
	_planes[3].set( _worldCorners[1], _worldCorners[5], _worldCorners[6] ); // right  plane
	_planes[4].set( _worldCorners[2], _worldCorners[6], _worldCorners[7] ); // top    plane
	_planes[5].set( _worldCorners[1], _worldCorners[0], _worldCorners[4] ); // bottom plane

	return (*this);
}
ZFrustum&
ZFrustum::set( float cs, float lsr, float ar, float hfa, float vfa, float ncp, float fcp, float fl, bool o, float ow, ZDirection::Direction initialAim)
{
    _initialAim = initialAim;
    if( ZAbs(ncp) > ZAbs(fcp) )
    {
        return (*this);
    }

    float hFov = hfa * 0.5 / (fl * 0.03937) * cs * lsr;
    float vFov = vfa * 0.5 / (fl * 0.03937);

    float nearRight = ncp * hFov;
    float nearTop   = ncp * vFov;
    float farRight  = fcp * hFov;
    float farTop    = fcp * vFov;

    if(o)
        nearRight = nearTop = farRight = farTop = ow / 2.0f;

    _near          = ZAbs( ncp );
    _far           = ZAbs( fcp );
    _horizontalFOV = ZAbs( hFov );
    _aspectRatio   = ZAbs( ar );

    _width         = 2.f * _near * tanf( 0.5f * _horizontalFOV );
    _height        = _width / _aspectRatio;
    _verticalFOV   = atan2f( 0.5f * _height, _near );


    if( _initialAim == ZDirection::zNegative ) // Maya default
    {
        _worldCorners[0] = _localCorners[0].set( -nearRight, -nearTop, -_near);
        _worldCorners[1] = _localCorners[1].set( +nearRight, -nearTop, -_near);
        _worldCorners[2] = _localCorners[2].set( +nearRight, +nearTop, -_near);
        _worldCorners[3] = _localCorners[3].set( -nearRight, +nearTop, -_near);
        _worldCorners[4] = _localCorners[4].set( -farRight,  -farTop,  -_far);
        _worldCorners[5] = _localCorners[5].set( +farRight,  -farTop,  -_far);
        _worldCorners[6] = _localCorners[6].set( +farRight,  +farTop,  -_far);
        _worldCorners[7] = _localCorners[7].set( -farRight,  +farTop,  -_far);
    }
    else
    {
        // TO DO: other cases
    }

    // to be outward unit normals
    _planes[0].set( _worldCorners[0], _worldCorners[1], _worldCorners[2] ); // near   plane
    _planes[1].set( _worldCorners[6], _worldCorners[5], _worldCorners[4] ); // far    plane
    _planes[2].set( _worldCorners[0], _worldCorners[3], _worldCorners[7] ); // left   plane
    _planes[3].set( _worldCorners[1], _worldCorners[5], _worldCorners[6] ); // right  plane
    _planes[4].set( _worldCorners[2], _worldCorners[6], _worldCorners[7] ); // top    plane
    _planes[5].set( _worldCorners[1], _worldCorners[0], _worldCorners[4] ); // bottom plane

    return (*this);

}


	ZFrustum&
ZFrustum::operator=( const ZFrustum& frustum )
{
	_initialAim    = frustum._initialAim;

	_near          = frustum._near;
	_far           = frustum._far;
	_horizontalFOV = frustum._horizontalFOV;
	_verticalFOV   = frustum._verticalFOV;
	_width         = frustum._width;
	_height        = frustum._height;
	_aspectRatio   = frustum._aspectRatio;

	_eyePosition   = frustum._eyePosition;
	_xform         = frustum._xform;

	FOR( i, 0, 8 )
	{
		_localCorners[i] = frustum._localCorners[i];
		_worldCorners[i] = frustum._worldCorners[i];
	}

	FOR( i, 0, 6 )
	{
		_planes[i] = frustum._planes[i];
	}

	return (*this);
}

void
ZFrustum::applyTransform( const ZMatrix& xform )
{
	xform.getTranslation( _eyePosition );

	_xform = xform;

	FOR( i, 0, 8 )
	{
		_worldCorners[i] = xform.transform( _localCorners[i], false );
	}

	// to be outward unit normals
	_planes[0].set( _worldCorners[0], _worldCorners[1], _worldCorners[2] ); // near   plane
	_planes[1].set( _worldCorners[6], _worldCorners[5], _worldCorners[4] ); // far    plane
	_planes[2].set( _worldCorners[0], _worldCorners[3], _worldCorners[7] ); // left   plane
	_planes[3].set( _worldCorners[1], _worldCorners[5], _worldCorners[6] ); // right  plane
	_planes[4].set( _worldCorners[2], _worldCorners[6], _worldCorners[7] ); // top    plane
	_planes[5].set( _worldCorners[1], _worldCorners[0], _worldCorners[4] ); // bottom plane
}

const ZPlane&
ZFrustum::nearPlane() const
{
	return _planes[0];
}

const ZPlane&
ZFrustum::farPlane() const
{
	return _planes[1];
}

const ZPlane&
ZFrustum::leftPlane() const
{
	return _planes[2];
}

const ZPlane&
ZFrustum::rightPlane() const
{
	return _planes[3];
}

const ZPlane&
ZFrustum::topPlane() const
{
	return _planes[4];
}

const ZPlane&
ZFrustum::bottomPlane() const
{
	return _planes[5];
}

bool
ZFrustum::contains( const ZPoint& point ) const
{
	FOR( i, 0, 6 ) // for the i-th plane of the frustum
	{
		const ZPlane& plane = _planes[i];

		if( plane.isOutside( point ) )
		{
			return false;
		}
	}

	return true;
}

bool
ZFrustum::contains( const ZSphere& sphere ) const
{
	const ZPoint& sphereCenter = sphere.center();
	const float& sphereRadius = sphere.radius();

	FOR( i, 0, 6 ) // for the i-th plane of the frustum
	{
		const ZPlane& plane = _planes[i];

		const float signedDist = plane.signedDistanceFrom( sphereCenter );

		if( signedDist > 0.f ) // The center of the sphere is outside of the plane.
		{
			return false;
		}
		else // The center of the sphere is inside of the plane.
		{
			if( ZAbs(signedDist) < sphereRadius ) // but the sphere is not inside perfectly
			{
				return false;
			}
		}
	}

	return true;
}

bool
ZFrustum::intersects( const ZSphere& sphere ) const
{
	const ZPoint& sphereCenter = sphere.center();
	const float& r2 = sphere.squaredRadius();

	// when the center of the sphere is inside of the frustum
	if( ZFrustum::contains( sphereCenter ) )
	{
		return true;
	}

	// when any corner point of the frustum is inside of the sphere
	FOR( i, 0, 8 ) // for the i-th corner point of the frustum
	{
		if( sphere.contains( _worldCorners[i] ) )
		{
			return true;
		}
	}

	ZFloat3 baryCoords;

	FOR( i, 0, 12 )
	{
		const ZPlane& plane = _planes[i];

		// three vertices of the i-th triangle
		const int& v0 = triangleIndices[i][0];
		const int& v1 = triangleIndices[i][1];
		const int& v2 = triangleIndices[i][2];

		// three points of the i-th triangle
		const ZPoint& p0 = _worldCorners[v0];
		const ZPoint& p1 = _worldCorners[v1];
		const ZPoint& p2 = _worldCorners[v2];

		const ZPoint q = ClosestPointOnTriangle( sphereCenter, p0, p1, p2, baryCoords );
		//q = WeightedSum( p0, p1, p2, baryCoords )

		if( q.squaredDistanceTo( sphereCenter ) < r2 )
		{
			return true;
		}
	}

	return false;
}

bool
ZFrustum::contains( const ZBoundingBox& aabb ) const
{
	ZPoint aabbCorners[8];
	aabb.getEightCorners( aabbCorners );

	FOR( i, 0, 6 ) // for the i-th plane of the frustum
	{
		const ZPlane& plane = _planes[i];

		FOR( j, 0, 8 ) // for the j-th corner point of the bounding box
		{
			if( plane.isOutside( aabbCorners[j] ) )
			{
				return false;
			}
		}
	}

	return true;
}

bool
ZFrustum::intersects( const ZBoundingBox& aabb ) const
{
	ZPoint aabbCorners[8];
	aabb.getEightCorners( aabbCorners );

	// when any corner point of the bounding box is inside of the frustum
	FOR( i, 0, 8 ) // for the i-th corner point of the bounding box
	{
		if( ZFrustum::contains( aabbCorners[i]  ) )
		{
			return true;
		}
	}

	// when any corner point of the frustum is inside of the bounding box
	FOR( i, 0, 8 ) // for the i-th corner point of the frustum
	{
		if( aabb.contains( _worldCorners[i] ) )
		{
			return true;
		}
	}

	if( aabb.intersectsWithLineSegment( _worldCorners[0], _worldCorners[4] ) ) { return true; }
	if( aabb.intersectsWithLineSegment( _worldCorners[1], _worldCorners[5] ) ) { return true; }
	if( aabb.intersectsWithLineSegment( _worldCorners[2], _worldCorners[6] ) ) { return true; }
	if( aabb.intersectsWithLineSegment( _worldCorners[3], _worldCorners[7] ) ) { return true; }

	return false;
}

void
ZFrustum::draw() const
{
	// near plane
	glBegin( GL_LINE_LOOP );
	{
		glVertex( _worldCorners[0] );
		glVertex( _worldCorners[1] );
		glVertex( _worldCorners[2] );
		glVertex( _worldCorners[3] );
	}
	glEnd();

	// far plane
	glBegin( GL_LINE_LOOP );
	{
		glVertex( _worldCorners[4] );
		glVertex( _worldCorners[5] );
		glVertex( _worldCorners[6] );
		glVertex( _worldCorners[7] );
	}
	glEnd();

	glBegin( GL_LINES );
	{
		glVertex( _eyePosition ); glVertex( _worldCorners[4] );
		glVertex( _eyePosition ); glVertex( _worldCorners[5] );
		glVertex( _eyePosition ); glVertex( _worldCorners[6] );
		glVertex( _eyePosition ); glVertex( _worldCorners[7] );
	}
	glEnd();
}

void
ZFrustum::write( ofstream& fout ) const
{
	fout.write( (char*)&_near,          sizeof(float) );
	fout.write( (char*)&_far,           sizeof(float) );
	fout.write( (char*)&_horizontalFOV, sizeof(float) );
	fout.write( (char*)&_aspectRatio,   sizeof(float) );

	_xform.write( fout );
}

void
ZFrustum::read( ifstream& fin )
{
	ZFrustum::reset();

	float near          = 0.f;
	float far           = 0.f;
	float horizontalFOV = 0.f;
	float aspectRatio   = 0.f;

	fin.read( (char*)&near,          sizeof(float) );
	fin.read( (char*)&far,           sizeof(float) );
	fin.read( (char*)&horizontalFOV, sizeof(float) );
	fin.read( (char*)&aspectRatio,   sizeof(float) );

	ZFrustum::set( near, far, horizontalFOV, aspectRatio );

	ZMatrix xform;
	xform.read( fin );

	ZFrustum::applyTransform( xform );
}

ostream&
operator<<( ostream& os, const ZFrustum& object )
{
	os << "<ZFrustum>" << endl;
	return os;
}

ZELOS_NAMESPACE_END

