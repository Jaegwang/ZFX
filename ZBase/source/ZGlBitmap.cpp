//---------------//
// ZGlBitmap.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlBitmap::ZGlBitmap()
: _width(0), _height(0), _pixel((unsigned char*)NULL), _windowTitle("ZGlBitmap OpenGL Window")
{}

ZGlBitmap::ZGlBitmap( int width, int height )
: _pixel((unsigned char*)NULL), _windowTitle("ZGlBitmap OpenGL Window")
{
	setSize( width, height );
}

ZGlBitmap::~ZGlBitmap()
{
	ZFree( _pixel );
}

bool
ZGlBitmap::setSize( int width, int height )
{
	if( ( width <= 0 ) || ( height <= 0 ) )
	{
		cout << "Error@ZGlBitmap::set(): Invalid image size." << endl;
		return false;
	}

	if( _width!=width || _height!=height )
	{
		_width = width;
		_height = height;

		ZFree( _pixel );
		ZAlloc( _pixel, size() );
	}

	memset( _pixel, 0, size()*sizeof(unsigned char) );

	return true;
}

void
ZGlBitmap::setWindowTitle( const char* title )
{
	_windowTitle = title;
}

int
ZGlBitmap::numPixels() const
{
	return ( _width * _height );
}

int
ZGlBitmap::size() const
{
	return ( _width * _height * 4 );
}

int
ZGlBitmap::width() const
{
	return _width;
}

int
ZGlBitmap::height() const
{
	return _height;
}

unsigned char*
ZGlBitmap::pointer() const
{
	return _pixel;
}

ZGlBitmap**
ZGlBitmap::bitmapPointer()
{
	static ZGlBitmap* gBitmap;
	return &gBitmap;
}

void
ZGlBitmap::display()
{
	ZGlBitmap** bitmap = bitmapPointer();
	*bitmap = this;

	int c = 1; // a bug in the Windows GLUT implementation prevents us from passing zero arguments to glutInit()
	glutInit( &c, NULL );
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
	glutInitWindowSize( _width, _height );
	glutCreateWindow( _windowTitle.asChar() );
	glutKeyboardFunc( keyboard );
	glutDisplayFunc( draw );
	glutMainLoop();
}

void
ZGlBitmap::keyboard( unsigned char key, int x, int y )
{
	switch( key )
	{
		case Z_KEY_ESC:
			exit(0);
	}
}

void
ZGlBitmap::draw()
{
	ZGlBitmap* bitmap = *(bitmapPointer());
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	glDrawPixels( bitmap->_width, bitmap->_height, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->_pixel );
	glFlush();
}

ostream&
operator<<( ostream& os, const ZGlBitmap& object )
{
	os << "<ZGlBitmap>" << endl;
	os << "width : " << object.width()  << endl;
	os << "height: " << object.height() << endl;

	return os;
}

ZELOS_NAMESPACE_END

