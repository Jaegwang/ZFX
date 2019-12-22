//----------------//
// ZWExportComm.h //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.09.01                               //
//-------------------------------------------------------//

#ifndef _ZWExportComm_h_
#define _ZWExportComm_h_

#include <ZelosBaseForMaya.h>

class ZWExportComm : public MPxCommand
{
    public:

        static const MString name;

        ZWExportComm() {};
        virtual ~ZWExportComm() {};

        virtual MStatus doIt( const MArgList& args );
		virtual bool isUndoable() const { return false; }

        MStatus undoIt() { return MS::kSuccess; }     

        static void* creator() { return new ZWExportComm(); }
        static MSyntax newSyntax() { MSyntax syntax; return syntax; }

};

#endif

