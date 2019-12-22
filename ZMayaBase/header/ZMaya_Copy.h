//--------------//
// ZMaya_Copy.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.04.05                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Copy_h_
#define _ZMaya_Copy_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

// to <- from

// ZVector
inline void Copy( ZVector& to, const MPoint&       from ) { to.x=(float)from.x; to.y=(float)from.y; to.z=(float)from.z; }
inline void Copy( ZVector& to, const MVector&      from ) { to.x=(float)from.x; to.y=(float)from.y; to.z=(float)from.z; }
inline void Copy( ZVector& to, const MFloatPoint&  from ) { to.x=from.x; to.y=from.y; to.z=from.z; }
inline void Copy( ZVector& to, const MFloatVector& from ) { to.x=from.x; to.y=from.y; to.z=from.z; }

inline void Copy( MPoint&       to, const ZVector& from ) { to.x=from.x; to.y=from.y; to.z=from.z; }
inline void Copy( MVector&      to, const ZVector& from ) { to.x=from.x; to.y=from.y; to.z=from.z; }
inline void Copy( MFloatPoint&  to, const ZVector& from ) { to.x=from.x; to.y=from.y; to.z=from.z; }
inline void Copy( MFloatVector& to, const ZVector& from ) { to.x=from.x; to.y=from.y; to.z=from.z; }

inline void Copy( MFloatVector& to, const MFloatPoint&  from ) { to.x=from.x; to.y=from.y; to.z=from.z; }
inline void Copy( MFloatPoint&  to, const MFloatVector& from ) { to.x=from.x; to.y=from.y; to.z=from.z; }

inline void Copy( MFloatVector& to, const MPoint&  from ) { to.x=(float)from.x; to.y=(float)from.y; to.z=(float)from.z; }
inline void Copy( MFloatVector& to, const MVector& from ) { to.x=(float)from.x; to.y=(float)from.y; to.z=(float)from.z; }

inline void Copy( MPoint&  to, const MFloatVector& from ) { to.x=from.x; to.y=from.y; to.z=from.z; }
inline void Copy( MVector& to, const MFloatVector& from ) { to.x=from.x; to.y=from.y; to.z=from.z; }

inline void Copy( MColor& to, const ZColor& from ) { to.r=from.r; to.g=from.g; to.b=from.b; }
inline void Copy( ZColor& to, const MColor& from ) { to.r=from.r; to.g=from.g; to.b=from.b; }

// matrix
void Copy( ZMatrix& to, const MMatrix& from, bool transposed=true );
void Copy( MMatrix& to, const ZMatrix& from, bool transposed=true );

// ZVectorArray
void Copy( ZVectorArray& to, const MPointArray&        from );
void Copy( ZVectorArray& to, const MVectorArray&       from );
void Copy( ZVectorArray& to, const MFloatPointArray&   from );
void Copy( ZVectorArray& to, const MFloatVectorArray&  from );
void Copy( ZVectorArray& to, const MFnPointArrayData&  from );
void Copy( ZVectorArray& to, const MFnVectorArrayData& from );
void Copy( MVectorArray& to, const MFnVectorArrayData& from );

void Copy( MPointArray&       to, const ZVectorArray& from );
void Copy( MVectorArray&      to, const ZVectorArray& from );
void Copy( MFloatPointArray&  to, const ZVectorArray& from );
void Copy( MFloatVectorArray& to, const ZVectorArray& from );

void Copy( MFloatArray&  to, const ZVectorArray& from );
void Copy( MDoubleArray& to, const ZVectorArray& from );

void Copy( MFloatArray&  to, const ZFloat2Array& from );
void Copy( MDoubleArray& to, const ZFloat2Array& from );

void Copy( MFloatArray&  to, const ZFloat3Array& from );
void Copy( MDoubleArray& to, const ZFloat3Array& from );

// ZIntArray
void Copy( MIntArray& to, const ZIntArray&       from );
void Copy( MIntArray& to, const MFnIntArrayData& from );
void Copy( ZIntArray& to, const MIntArray&       from );
void Copy( ZIntArray& to, const MFnIntArrayData& from );

// ZFloatArray
void Copy( ZFloatArray& to, const MFloatArray&        from );
void Copy( ZFloatArray& to, const MDoubleArray&       from );
void Copy( ZFloatArray& to, const MFnFloatArrayData&  from );
void Copy( ZFloatArray& to, const MFnDoubleArrayData& from );

void Copy( MFloatArray&  to, const ZFloatArray&  from );
void Copy( MDoubleArray& to, const ZFloatArray&  from );

// ZDoubleArray
void Copy( ZDoubleArray& to, const MFloatArray&        from );
void Copy( ZDoubleArray& to, const MDoubleArray&       from );
void Copy( ZDoubleArray& to, const MFnFloatArrayData&  from );
void Copy( ZDoubleArray& to, const MFnDoubleArrayData& from );

void Copy( MFloatArray&  to, const ZDoubleArray&  from );
void Copy( MDoubleArray& to, const ZDoubleArray&  from );
void Copy( MDoubleArray& to, const MFnDoubleArrayData& from );

// ZInt3Array
void Copy( ZInt3Array& to, const MIntArray&       from );
void Copy( MIntArray&  to, const ZInt3Array&      from );
void Copy( ZInt3Array& to, const MFnIntArrayData& from );

// etc.
void Copy( ZFloat2Array& uv, const MFloatArray& u, const MFloatArray& v );

// MBoundingBox
void Copy( MBoundingBox& to, const ZBoundingBox& from );
// ZBoundingBox
void Copy( ZBoundingBox& to, const MBoundingBox& from );

ZELOS_NAMESPACE_END

#endif

