//-------------------------//
// ZWStitchCurvesControl.h //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.08.17                               //
//-------------------------------------------------------//

#ifndef _ZWStitchCurvesControl_h_
#define _ZWStitchCurvesControl_h_

#include <ZelosBaseForMaya.h>

class ZWStitchCurvesControl : public MPxNode
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
		static MObject rotateFactorObj;
		static MObject rotateVarianceObj;
		static MObject scaleFactorObj;
		static MObject scaleVarianceObj;
		static MObject spaceObj;
		static MObject numStitchsObj;
		static MObject numScaleObj;
        static MObject outWebDataObj;
		
	public:

		ZWStitchCurvesControl();
		~ZWStitchCurvesControl();

		static void* creator();
		virtual void postConstructor();
		static MStatus initialize();
    	virtual MStatus compute( const MPlug&, MDataBlock& );		
		virtual MStatus connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc );		
};

#endif

