//-----------------//
// ZWStitchCurves.h //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.09.06                               //
//-------------------------------------------------------//

#ifndef _ZWStitchCurves_h_
#define _ZWStitchCurves_h_

#include <ZelosBaseForMaya.h>

class ZWStitchCurves : public MPxNode
{
	private:

		MObject           nodeObj;
		MString           nodeName;
		MFnDependencyNode nodeFn;
		MFnDagNode        dagNodeFn;
		MDataBlock*       blockPtr;
		bool              isThe1stTime=true;

		/* Buffers */
		//MVectorArray guideCurveBinormals;
		//MVectorArray guideCurveNormals;

	public:

		static bool enable;
		static std::map<std::string, ZWStitchCurves*> instances;		

	public:

		static MTypeId id;
		static MString name;

		static MObject inWebDataObj;
        static MObject inSourceCurveObj;
		static MObject inGuideCurveObj;

		static MObject seedObj;
        static MObject txObj;
		static MObject tyObj;
		static MObject tzObj;
        static MObject rxObj;
		static MObject ryObj;
		static MObject rzObj;
        static MObject sxObj;
		static MObject syObj;
		static MObject szObj;
		static MObject pxObj;
		static MObject pyObj;
		static MObject pzObj;		
        static MObject outScalesObj;
        static MObject outRotatesObj;
        static MObject outTranslatesObj;
		static MObject outPivotObj;
		static MObject guideBinormalsObj;
		static MObject guideNormalsObj;
		
	public:

		ZWStitchCurves();
		~ZWStitchCurves();

		static void* creator();
		virtual void postConstructor();
		static MStatus initialize();
		static void destructor( MObject& node, void* data );
    	virtual MStatus compute( const MPlug&, MDataBlock& );		
		virtual MStatus connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc );		
		virtual MStatus connectionBroken( const MPlug& plug, const MPlug& otherPlug, bool asSrc );
};

#endif

