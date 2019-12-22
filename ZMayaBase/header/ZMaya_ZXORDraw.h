//------------------//
// ZMaya_ZXORDraw.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_ZXORDraw_h_
#define _ZMaya_ZXORDraw_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

class ZXORDraw
{
	private:

		M3dView* _pView;

		GLboolean _depthTest;
		GLboolean _lightingTest;
		GLboolean _blend;
		GLboolean _pointSmooth;
		GLboolean _lineSmooth;
		GLboolean _lineStipple;
		GLboolean _colorLogicOp;

		GLfloat   _pointSize;
		GLfloat   _lineWidth;

	public:

		bool    useCustomDraw;

		GLfloat pointSize;
		GLfloat lineWidth;

		MColor  color;

	public:

		ZXORDraw();
		ZXORDraw( const M3dView& view );

		void set( const M3dView& view );

		void beginXorDrawing();
		void endXorDrawing();
};

ZELOS_NAMESPACE_END

#endif

