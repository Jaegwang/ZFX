//--------------//
// ZXORDraw.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
//         Nayoung Kim @ Dexter Studios                  //
// last update: 2016.12.01                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

ZXORDraw::ZXORDraw()
{
	_pView = (M3dView*)NULL;

	_depthTest    = false;
	_lightingTest = false;
	_blend        = false;
	_pointSmooth  = false;
	_lineSmooth   = false;
	_lineStipple  = false;
	_colorLogicOp = false;

	_pointSize    = 1.f;
	_lineWidth    = 1.f;

	useCustomDraw = true;
	pointSize     = 1.f;
	lineWidth     = 1.f;
	color         = MColor(1,1,1);
}

ZXORDraw::ZXORDraw( const M3dView& pView )
{
	_pView = const_cast<M3dView*>(&pView);

	_depthTest    = false;
	_lightingTest = false;
	_blend        = false;
	_pointSmooth  = false;
	_lineSmooth   = false;
	_lineStipple  = false;
	_colorLogicOp = false;

	_pointSize    = 1.f;
	_lineWidth    = 1.f;

	useCustomDraw = true;
	pointSize     = 1.f;
	lineWidth     = 1.f;
	color         = MColor(1,1,1);
}

void
ZXORDraw::set( const M3dView& pView )
{
	_pView = const_cast<M3dView*>(&pView);
}

void
ZXORDraw::beginXorDrawing()
{
	if( !_pView ) { return; }

	// Save the current states for restoring them later.
	glGetBooleanv( GL_DEPTH_TEST,     &_depthTest    );
	glGetBooleanv( GL_LIGHTING,       &_lightingTest );
	glGetBooleanv( GL_BLEND,          &_blend        );
	glGetBooleanv( GL_POINT_SMOOTH,   &_pointSmooth  );
	glGetBooleanv( GL_LINE_SMOOTH,    &_lineSmooth   );
	glGetBooleanv( GL_LINE_STIPPLE,   &_lineStipple  );
	glGetBooleanv( GL_COLOR_LOGIC_OP, &_colorLogicOp );

	glGetFloatv( GL_POINT_SIZE, &_pointSize );
	glGetFloatv( GL_LINE_WIDTH, &_lineWidth );

	if( useCustomDraw ) {

		glPointSize( pointSize );
		glLineWidth( lineWidth );
		glColor( color.r, color.g, color.b );

		glDrawBuffer( GL_FRONT );

		// Save the state of the matrix on stack
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();

		// Setup the Orthographic projection Matrix.
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluOrtho2D( 0.0, (GLdouble)_pView->portWidth(), 0.0, (GLdouble)_pView->portHeight() );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glTranslatef( 0.375, 0.375, 0.0 );

		glDisable( GL_DEPTH_TEST );

		// Enable XOR mode.
		glEnable( GL_COLOR_LOGIC_OP );
		glLogicOp( GL_XOR );

	} else {

		_pView->beginXorDrawing( true, true, lineWidth, M3dView::kStippleNone, color );

	}
}

void
ZXORDraw::endXorDrawing()
{
	if( !_pView ) { return; }

	if( useCustomDraw ) {

		glFlush();
		glDrawBuffer( GL_BACK );

		// Restore the state of the matrix from stack
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();

	} else {

		_pView->endXorDrawing();

	}

	if( _depthTest    ) { glEnable ( GL_DEPTH_TEST     ); }
	else                { glDisable( GL_DEPTH_TEST     ); }

	if( _lightingTest ) { glEnable ( GL_LIGHTING       ); }
	else                { glDisable( GL_LIGHTING       ); }

	if( _blend        ) { glEnable ( GL_BLEND          ); }
	else                { glDisable( GL_BLEND          ); }

	if( _pointSmooth  ) { glEnable ( GL_POINT_SMOOTH   ); }
	else                { glDisable( GL_POINT_SMOOTH   ); }

	if( _lineSmooth   ) { glEnable ( GL_LINE_SMOOTH    ); }
	else                { glDisable( GL_LINE_SMOOTH    ); }

	if( _lineStipple  ) { glEnable ( GL_LINE_STIPPLE   ); }
	else                { glDisable( GL_LINE_STIPPLE   ); }

	if( _colorLogicOp ) { glEnable ( GL_COLOR_LOGIC_OP ); }
	else                { glDisable( GL_COLOR_LOGIC_OP ); }

	glPointSize( _pointSize );
	glLineWidth( _lineWidth );
}

ZELOS_NAMESPACE_END

