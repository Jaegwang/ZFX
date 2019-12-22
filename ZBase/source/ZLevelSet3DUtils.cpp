//----------------------//
// ZLevelSet3DUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

//Given four signed distance values (square corners), determine what fraction of the square is "inside"
float
ZFractionInside( float phi0, float phi1, float phi2, float phi3 )
{
	const int insideCount = (phi0<0?1:0) + (phi2<0?1:0) + (phi1<0?1:0) + (phi3<0?1:0);
	ZFloat4 list( phi0, phi1, phi3, phi2 );

	switch( insideCount )
	{
		// fully inside
		default:
		case 0:
		{
			return 0;
		}

		case 1:
		{
			// Rotate until the negative value is in the first position.
			while(list[0]>=0) { list.cycle(); }
			// Work out the area of the interior triangle, and subtract from 1.
			const float side0 = ZFractionInside( list[0], list[3] );
			const float side1 = ZFractionInside( list[0], list[1] );
			return (0.5f*side0*side1);
		}

		case 2:
		{
			// Rotate until a negative value is in the first position, and the next negative is in either slot 1 or 2.
			while(list[0]>=0||!(list[1]<0||list[2]<0)) { list.cycle(); }

			if( list[1] < 0 ) { //the matching signs are adjacent
				const float side0 = ZFractionInside( list[0], list[3] );
				const float side1 = ZFractionInside( list[1], list[2] );
				return (0.5f*(side0+side1));
			} else { // matching signs are diagonally opposite
				// determine the centre point's sign to disambiguate this case
				const float cPhi = 0.25f*(list[0]+list[1]+list[2]+list[3]);
				if( cPhi < 0 ) {
					float area = 0;
					// first triangle (top left)
					const float side1 = 1-ZFractionInside( list[0], list[3] );
					const float side3 = 1-ZFractionInside( list[2], list[3] );
					area += 0.5f*side1*side3;
					// second triangle (top right)
					const float side2 = 1-ZFractionInside( list[2], list[1] );
					const float side0 = 1-ZFractionInside( list[0], list[1] );
					area += 0.5f*side0*side2;
					return (1-area);
				} else {
					float area = 0;
					// first triangle (bottom left)
					const float side0 = ZFractionInside(list[0], list[1]);
					const float side1 = ZFractionInside(list[0], list[3]);
					area += 0.5f*side0*side1;
					// second triangle (top right)
					const float side2 = ZFractionInside(list[2], list[1]);
					const float side3 = ZFractionInside(list[2], list[3]);
					area += 0.5f*side2*side3;
					return area;
				}
			}
		}

		case 3:
		{
			// Rotate until the positive value is in the first position.
			while(list[0]<0) { list.cycle(); }
			// Work out the area of the exterior triangle.
			const float side0 = 1-ZFractionInside( list[0], list[3] );
			const float side1 = 1-ZFractionInside( list[0], list[1] );
			return (1-0.5f*side0*side1);
		}

		// fully outside
		case 4:
		{
			return 1;
		}
     }
}

// Given eight signed distance values (square corners), determine what fraction of the square is "inside".
float
ZFractionInside( float phi0, float phi1, float phi2, float phi3, float phi4, float phi5, float phi6, float phi7, bool approximation )
{
	float V = 0;

	if( approximation ) {

		if( phi0 < 0 ) { V += 0.125f; }
		if( phi1 < 0 ) { V += 0.125f; }
		if( phi2 < 0 ) { V += 0.125f; }
		if( phi3 < 0 ) { V += 0.125f; }
		if( phi4 < 0 ) { V += 0.125f; }
		if( phi5 < 0 ) { V += 0.125f; }
		if( phi6 < 0 ) { V += 0.125f; }
		if( phi7 < 0 ) { V += 0.125f; }

	} else {

		cout << "Error@ZFractionInside(): Not yet implemented." << endl;

	}

	return V;
}

////
////		if( phi0 < 0 ) { V += (phi0<-0.5f)?(0.125f):(0.125f*(phi0/-0.5f)); }
////		if( phi1 < 0 ) { V += (phi1<-0.5f)?(0.125f):(0.125f*(phi1/-0.5f)); }
////		if( phi2 < 0 ) { V += (phi2<-0.5f)?(0.125f):(0.125f*(phi2/-0.5f)); }
////		if( phi3 < 0 ) { V += (phi3<-0.5f)?(0.125f):(0.125f*(phi3/-0.5f)); }
////		if( phi4 < 0 ) { V += (phi4<-0.5f)?(0.125f):(0.125f*(phi4/-0.5f)); }
////		if( phi5 < 0 ) { V += (phi5<-0.5f)?(0.125f):(0.125f*(phi5/-0.5f)); }
////		if( phi6 < 0 ) { V += (phi6<-0.5f)?(0.125f):(0.125f*(phi6/-0.5f)); }
////		if( phi7 < 0 ) { V += (phi7<-0.5f)?(0.125f):(0.125f*(phi7/-0.5f)); }
////
//
////
////{
////	float V = 0;
////
////	if( phi0 < -0.5f ) { V += 1; }
////	else if( phi0 < 0.0f ) { V -= phi0*2; }
////	else if( phi0 < 0.5f ) { V += 1-(phi0*2); }
////
////	if( phi1 < -0.5f ) { V += 1; }
////	else if( phi1 < 0.0f ) { V -= phi1*2; }
////	else if( phi1 < 0.5f ) { V += 1-(phi1*2); }
////
////	if( phi2 < -0.5f ) { V += 1; }
////	else if( phi2 < 0.0f ) { V -= phi2*2; }
////	else if( phi2 < 0.5f ) { V += 1-(phi2*2); }
////
////	if( phi3 < -0.5f ) { V += 1; }
////	else if( phi3 < 0.0f ) { V -= phi3*2; }
////	else if( phi3 < 0.5f ) { V += 1-(phi3*2); }
////
////	if( phi4 < -0.5f ) { V += 1; }
////	else if( phi4 < 0.0f ) { V -= phi4*2; }
////	else if( phi4 < 0.5f ) { V += 1-(phi4*2); }
////
////	if( phi5 < -0.5f ) { V += 1; }
////	else if( phi5 < 0.0f ) { V -= phi5*2; }
////	else if( phi5 < 0.5f ) { V += 1-(phi5*2); }
////
////	if( phi6 < -0.5f ) { V += 1; }
////	else if( phi6 < 0.0f ) { V -= phi6*2; }
////	else if( phi6 < 0.5f ) { V += 1-(phi6*2); }
////
////	if( phi7 < -0.5f ) { V += 1; }
////	else if( phi7 < 0.0f ) { V -= phi7*2; }
////	else if( phi7 < 0.5f ) { V += 1-(phi7*2); }
////
////	return (V*0.125f);
////}

ZELOS_NAMESPACE_END

