//------------------//
// ZWebCurvesData.h //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.08.23                               //
//-------------------------------------------------------//

#ifndef _ZWebCurvesData_h_
#define _ZWebCurvesData_h_

#include <ZelosBaseForMaya.h>

class ZWebCurvesData : public MPxData
{
	public:

		static MTypeId id;
		static MString typeName;

	public:

        int   numStrands=0;
		int   numScale=0.f;
        float segScale=1.f;
        float radius=1.f;
        float swirl=0.f;
        float noiseScale=0.f;
        float noiseFrequency=1.f;
        float noiseOffset=0.f;
		int   noiseOctaves=1;
		float scaleVariance=0.f;
		float scale=0.f;
		float rotateVariance=0.f;
		float rotate=0.f;
		float translate=0.f;
		float space=1.f;
        
	public:

		ZWebCurvesData();

		static  void* creator();
		virtual MTypeId typeId() const;
		virtual MString name() const;
		virtual void copy( const MPxData& );
};

#endif

