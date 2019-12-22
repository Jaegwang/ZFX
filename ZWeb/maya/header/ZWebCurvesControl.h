//---------------------//
// ZWebCurvesControl.h //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.08.17                               //
//-------------------------------------------------------//

#ifndef _ZWebCurvesControl_h_
#define _ZWebCurvesControl_h_

#include <ZelosBaseForMaya.h>

class ZWebCurvesControl : public MPxNode
{
	private:

		MObject           nodeObj;
		MString           nodeName;
		MFnDependencyNode nodeFn;
		MFnDagNode        dagNodeFn;
		MDataBlock*       blockPtr;
		bool              isThe1stTime=true;

	public:

		static MTypeId id;
		static MString name;
		
		static MObject inLinkObj;
		static MObject numStrandsObj;
		static MObject segmentScaleObj;
		static MObject webRadiusObj;
		static MObject webSwirlObj;
		static MObject outWebDataObj;

		// Noise parameters
		static MObject raNoiseFreqObj;
		static MObject raNoiseOffsetObj;
		static MObject raNoiseScaleObj;
		static MObject raNoiseOctavesObj;

		// Noises
		ZSimplexNoise radiusNoise;
		
	public:

		ZWebCurvesControl();
		~ZWebCurvesControl();

		static void* creator();
		virtual void postConstructor();
		static MStatus initialize();
    	virtual MStatus compute( const MPlug&, MDataBlock& );		
		virtual MStatus connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc );
        
};

#endif

