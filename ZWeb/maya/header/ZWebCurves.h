//--------------//
// ZWebCurves.h //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.09.06                               //
//-------------------------------------------------------//

#ifndef _ZWebCurves_h_
#define _ZWebCurves_h_

#include <ZelosBaseForMaya.h>

class ZWebCurves : public MPxNode
{
	private:

		MObject           nodeObj;
		MString           nodeName;
		MFnDependencyNode nodeFn;
		MFnDagNode        dagNodeFn;
		MDataBlock*       blockPtr;
		bool              isThe1stTime=true;

		/* Buffers */
		MVectorArray guideCurvePoints;
		MVectorArray guideCurveBinormals;
		MVectorArray guideCurveNormals;
		std::vector< MPointArray > cvPoints;

	public:

		static bool enable;
		static std::map<std::string, ZWebCurves*> instances;

	public:

		static MTypeId id;
		static MString name;
		
		static MObject inWebDataObj;
		static MObject inGuideCurveObj;
		static MObject seedObj;
        static MObject outWebCurvesObj;		
		static MObject rampSwirlObj;
		static MObject rampNoiseObj;		
		static MObject guidePointsObj;
		static MObject guideBinormalsObj;
		static MObject guideNormalsObj;

		// Noises
		ZSimplexNoise radiusNoise;
		
	public:

		ZWebCurves();
		~ZWebCurves();

		static void* creator();
		virtual void postConstructor();
		static MStatus initialize();
		static void destructor( MObject& node, void* data );
    	virtual MStatus compute( const MPlug&, MDataBlock& );		
		virtual MStatus connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc );
		virtual MStatus connectionBroken( const MPlug& plug, const MPlug& otherPlug, bool asSrc );
};

#endif

