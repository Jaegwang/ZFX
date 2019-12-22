//------------------//
// ZMaya_Register.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.02.17                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Register_h_
#define _ZMaya_Register_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

// data
#define RegisterData( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerData( class::typeName, class::id, class::creator );\
	if( !s ) { s.perror( MString("Failed to register: ") + class::typeName ); return s; }\
}

#define RegisterGeomData( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerData( class::typeName, class::id, class::creator, MPxData::kGeometryData );\
	if( !s ) { s.perror( MString("Failed to register: ") + class::typeName ); return s; }\
}

#define DeregisterData( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.deregisterData( class::id );\
	if( !s ) { s.perror( MString("Failed to deregister: ") + class::typeName ); return s; }\
}

// command
#define RegisterCommand( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerCommand( class::name, class::creator, class::newSyntax );\
	if( !s ) { s.perror( MString("Failed to register: ") + class::name ); return s; }\
}

#define DeregisterCommand( class )\
{\
	MStatus s;\
	s = pluginFn.deregisterCommand( class::name );\
	if( !s ) { s.perror( MString("Failed to deregister: ") + class::name ); return s; }\
}

// DG node
#define RegisterNode( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerNode( class::name, class::id, class::creator, class::initialize, MPxNode::kDependNode );\
	if( !s ) { s.perror( MString("Failed to register: ") + class::name ); return s; }\
}

// locator node
#define RegisterLocatorNode( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerNode( class::name, class::id, class::creator, class::initialize, MPxNode::kLocatorNode );\
	if( !s ) { s.perror( MString("Failed to register: ") + class::name ); return s; }\
}

// deformer node
#define RegisterDeformerNode( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerNode( class::name, class::id, class::creator, class::initialize, MPxNode::kDeformerNode );\
	if( !s ) { s.perror( MString("Failed to register: ") + class::name ); return s; }\
}

// transform node
#define RegisterTransformNode( xForm, matrix )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerTransform( xForm::name, xForm::id, xForm::creator, xForm::initialize, matrix::creator, matrix::id );\
	if( !s ) { s.perror( MString("Failed to register: ") + xForm::name ); return s; }\
}

// manipulator
#define RegisterManipulator( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerNode( class::name, class::id, class::creator, class::initialize, MPxNode::kManipulatorNode );\
	if( !s ) { s.perror( MString("Failed to register: " ) + class::name ); return s; }\
}

// create node command
#define RegisterCreateNodeCommand( class )\
{\
	MStatus s = MS::kSuccess;\
	static class _cmd;\
	s = _cmd.registerCommand( obj );\
	if( !s ) { s.perror( MString("Failed to register: MTemplateCreateNodeCommand") ); return s; }\
}

#define DeregisterCreateNodeCommand( class )\
{\
	MStatus s = MS::kSuccess;\
	static class _cmd;\
	s = _cmd.deregisterCommand( obj );\
	if( !s ) { s.perror( MString("Failed to deregister: MTemplateCreateNodeCommand") ); return s; }\
}

// context
#define RegisterContext( ctx, ctxToolCmd, ctxCmd )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerContextCommand( ctx::name, ctxCmd::creator, ctxToolCmd::name, ctxToolCmd::creator, ctxToolCmd::newSyntax );\
}

#define DeregisterContext( ctx, ctxToolCmd )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.deregisterContextCommand( ctx::name, ctxToolCmd::name );\
}

// field node
#define RegisterFieldNode( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerNode( class::name, class::id, class::creator, class::initialize, MPxNode::kFieldNode );\
	if( !s ) { s.perror( MString("Failed to register: ") + class::name ); return s; }\
} 

// emitter node
#define RegisterEmitterNode( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerNode( class::name, class::id, class::creator, class::initialize, MPxNode::kEmitterNode );\
	if( !s ) { s.perror( MString("Failed to register: ") + class::name ); return s; }\
}

// shape node
#define RegisterShapeNode( shapeClass, shapeUIClass )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerShape( shapeClass::name, shapeClass::id, &shapeClass::creator, &shapeClass::initialize, &shapeUIClass::creator );\
	if( !s ) { s.perror( MString("Failed to register: ") + shapeClass::name ); return s; }\
}

#define DeregisterNode( class )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.deregisterNode( class::id );\
	if( !s ) { s.perror( MString("Failed to deregister: ") + class::name ); return s; }\
}

//////////////////////
// for viewport 2.0 //

#define RegisterLocatorNodeWithDrawOverride( node, drawOverride )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerNode( node::name, node::id, node::creator, node::initialize, MPxNode::kLocatorNode, &node::drawDbClassification );\
	if( !s ) { s.perror( MString("Failed to register: ") + node::name ); return s; }\
	s = MHWRender::MDrawRegistry::registerDrawOverrideCreator( node::drawDbClassification, node::drawRegistrantId, drawOverride::Creator );\
	if( !s ) { s.perror( MString("Failed to register: ") + node::drawRegistrantId ); return s; }\
}

#define RegisterShapeWithDrawOverride( node, drawOverride )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerShape( node::name, node::id, node::creator, node::initialize, NULL, &node::drawDbClassification );\
	if( !s ) { s.perror( MString("Failed to register: ") + node::name ); return s; }\
	s = MHWRender::MDrawRegistry::registerDrawOverrideCreator( node::drawDbClassification, node::drawRegistrantId, drawOverride::Creator );\
	if( !s ) { s.perror( MString("Failed to register: ") + node::drawRegistrantId ); return s; }\
}

#define DeregisterShapeWithDrawOverride( node )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.deregisterNode( node::id );\
	if( !s ) { s.perror( MString("Failed to deregister: ") + node::name ); return s; }\
	s = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator( node::drawDbClassification, node::drawRegistrantId );\
	if( !s ) { s.perror( MString("Failed to deregister: ") + node::name ); return s; }\
}

#define DeregisterDrawOverride( node )\
{\
	MStatus s = MS::kSuccess;\
	s = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator( node::drawDbClassification, node::drawRegistrantId );\
	if( !s ) { s.perror( MString("Failed to deregister: ") + node::name ); return s; }\
}	

#define RegisterLocatorNodeWithGeomOverride( node, drawOverride )\
{\
	MStatus s = MS::kSuccess;\
	s = pluginFn.registerNode( node::name, node::id, node::creator, node::initialize, MPxNode::kLocatorNode, &node::drawDbClassification );\
	if( !s ) { s.perror( MString("Failed to register: ") + node::name ); return s; }\
	s = MHWRender::MDrawRegistry::registerGeometryOverrideCreator( node::drawDbClassification, node::drawRegistrantId, drawOverride::Creator );\
	if( !s ) { s.perror( MString("Failed to register: ") + node::drawRegistrantId ); return s; }\
}

#define DeregisterGeomOverride( node )\
{\
	MStatus s = MS::kSuccess;\
	s = MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator( node::drawDbClassification, node::drawRegistrantId ); \
	if( !s ) { s.perror( MString("Failed to deregister: ") + node::name ); return s; }\
}	

ZELOS_NAMESPACE_END

#endif

