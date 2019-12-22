//----------//
// ZImage.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMImage_h_
#define _ZMImage_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

class ZMImage
{
	private:

		bool toReleaseCPixel;
		bool toReleaseFPixel;

		uint m_width;
		uint m_height;
		uint m_depth;
		uint m_size;

		unsigned char *m_cPixel;
		float         *m_fPixel;

	public:

		ZMImage();
		ZMImage( const char* filePathName );

		virtual ~ZMImage();

		void reset();

		void getRGB( uint i, uint j, unsigned char& r, unsigned char& g, unsigned char& b );
		void getRGB( uint i, uint j, float& r, float& g, float& b );

		void getRGBA( uint i, uint j, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a );
		void getRGBA( uint i, uint j, float& r, float& g, float& b, float& a );

		void getColor( float x, float y, float& r, float& g , float& b );

		bool readFromFile( const char* filePathName );

	private:

		void init();
};

inline void
ZMImage::getRGB( uint i, uint j, unsigned char& r, unsigned char& g, unsigned char& b )
{
	uint index = m_depth * ( i + j * m_width );

	r = m_cPixel[  index];
	g = m_cPixel[++index];
	b = m_cPixel[++index];
}

inline void
ZMImage::getRGB( uint i, uint j, float& r, float& g, float& b )
{
	uint index = m_depth * ( i + j * m_width );

	r = m_fPixel[  index];
	g = m_fPixel[++index];
	b = m_fPixel[++index];
}

inline void
ZMImage::getRGBA( uint i, uint j, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a )
{
	uint index = m_depth * ( i + j * m_width );

	r = m_cPixel[  index];
	g = m_cPixel[++index];
	b = m_cPixel[++index];
	a = m_cPixel[++index];
}

inline void
ZMImage::getRGBA( uint i, uint j, float& r, float& g, float& b, float& a )
{
	uint index = m_depth * ( i + j * m_width );

	r = m_fPixel[  index];
	g = m_fPixel[++index];
	b = m_fPixel[++index];
	a = m_fPixel[++index];
}

inline void ZMImage::getColor( float x, float y, float& r, float& g , float& b )
{
	uint i = (uint)( x * m_width );
	uint j = (uint)( y * m_height );

	uint index = m_depth * ( i + j * m_width );

	r = m_fPixel[  index];
	g = m_fPixel[++index];
	b = m_fPixel[++index];
}

ZELOS_NAMESPACE_END

#endif

