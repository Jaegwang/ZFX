//---------------//
// ZGlCamera.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlCamera::ZGlCamera()
: _eyeDirection(0,0,-1), _center(0,0,0)
{
	_init();
	_updateViewMatrix();
}

ZGlCamera::ZGlCamera( const ZPoint& eye, const ZPoint& center )
{
	_eyeDirection = center - eye;
	_center       = center;

	_init();
	_updateViewMatrix();
}

void ZGlCamera::_init()
{
	_mode      = IDLE;
	_zoomSpeed = 10.0f;
	_winWidth  = 200;
	_winHeight = 200;
}

void ZGlCamera::setCenter( const ZPoint& center )
{
	const ZPoint eye( _center - _eyeDirection );
	_center = center;
	_eyeDirection = _center - eye;
	_updateViewMatrix();
}

void ZGlCamera::setEye( const ZPoint& eye )
{
	_eyeDirection = _center - eye;
	_updateViewMatrix();
}

void ZGlCamera::setCurrentMousePos( float x, float y )
{
	_trackBall.setCurrentPosition( (int)x, (int)y );
	y = _winHeight - y;

	_mousePosLast = _mousePosCurrent;
	_mousePosCurrent.set( x, y );

	if( _mode != IDLE ) {

		const ZVector diff( _mousePosCurrent - _mousePosLast );
        
		switch( _mode )
		{
			case PANNING:
			{
				pan( diff.x, diff.y );
				break;
			}
                
			case ZOOMING:
			{
				zoom( diff.y );
				default:
				break;
			}
		}

	} else {

		_updateViewMatrix();

	}
}

void ZGlCamera::setWindowSize( float w, float h )
{
	_trackBall.setWindowSize( w, h );
	_winWidth  = w;
	_winHeight = h;
}

void ZGlCamera::beginRotate()
{
	_trackBall.start();
}

void ZGlCamera::endRotate()
{
	_trackBall.finish();
}

void ZGlCamera::beginPan()
{
	_mode = PANNING;
}

void ZGlCamera::endPan()
{
	_mode = IDLE;
}

void ZGlCamera::beginZoom()
{
	_mode = ZOOMING;
}

void ZGlCamera::endZoom()
{
	_mode = IDLE;
}

ZPoint ZGlCamera::getEye() const
{
	return _center - _eyeDirection;
}

void ZGlCamera::pan( float x, float y )
{
	x = x / _winWidth;
	y = y / _winHeight;

	const float length = 2 * _eyeDirection.length() * tanf( ZDegToRad(30.f) );

	ZVector dX( _viewMatrix.column(0) ); // current right
	dX *= (-x)*(length*(_winWidth/_winHeight));

	// _viewMatrix.column(1)
	ZVector dY( _viewMatrix.column(1) ); // current up
	dY *= (-y)*length;

	_center += dX + dY;

	_updateViewMatrix();
}

void ZGlCamera::zoom( float deltaZ )
{
	_eyeDirection = _eyeDirection * ( deltaZ * _zoomSpeed / _winHeight + 1 );
	_updateViewMatrix();
}

void ZGlCamera::getViewMatrix( float viewMatrix[16] ) const
{
	memcpy( (char*)viewMatrix, (char*)_viewMatrix.data, sizeof(float)*16 );
}

// Computes the view rotation for the look-at
// Gets the trackball rotation matrix
// Computes the following transformation:
//      translate points by -eye (move origin to eye)
//      rotate by lookatRot
//      translate into the view center (translation)
//      rotate with the trackball rotation
//      translate back out
void ZGlCamera::_updateViewMatrix()
{
	const ZPoint eye( _center - _eyeDirection );
	ZMatrix lookAtRotation( ZMatrix::gluLookAt( eye, _center, ZVector(0,1,0) ) );
	const ZMatrix lookAtTranslation( 1,0,0,-eye.x, 0,1,0,-eye.y, 0,0,1,-eye.z, 0,0,0,1 );
	const ZMatrix translation( 1,0,0,0, 0,1,0,0, 0,0,1,_eyeDirection.length(), 0,0,0,1 );
	const ZMatrix invTranslation( 1,0,0,0, 0,1,0,0, 0,0,1,-_eyeDirection.length(), 0,0,0,1 );
	const ZMatrix arcRot( _trackBall.rotationMatrix() );
	_viewMatrix =  ( invTranslation * arcRot.transposed() * translation  * lookAtRotation * lookAtTranslation ).transposed();
}

ZELOS_NAMESPACE_END

