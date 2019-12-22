//---------------//
// ZSkeleton.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
//         Jungmin Lee @ Dexter Studios                  //
// last update: 2017.12.13                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZSkeleton::ZSkeleton()
{
    int dispListId = -1;

    upSphereObj   = gluNewQuadric();
    cylinderObj   = gluNewQuadric();
    downSphereObj = gluNewQuadric();
}	


ZSkeleton::~ZSkeleton()
{
    glDeleteLists( dispListId, 1 );

    gluDeleteQuadric( upSphereObj   );
    gluDeleteQuadric( cylinderObj   );
    gluDeleteQuadric( downSphereObj );

}

void
ZSkeleton::reset()
{
	if( _joints.size() > 0 ) { _joints.clear(); }

	Joint* temp = new Joint;
	_root    = temp;
	_current = nullptr;
	_parent  = nullptr;

	_theMode = NONE;
	_vertIndex = _channelsNum = _channelTotal = _channelIndex = 0;
	_tempAttr.clear();
	_tempTrans.clear();
	//_tempRotates.clear();
	_tempMotion.setToIdentity();

	_boneIndex = _childCount = _subParentCount = _lastParentCount = 0;

	_frameTime = _frameUnit = _frameNum = _endTime = 0.0;
	_keyframes.clear();
	_mdataCount = 0;
 
	cout << "@zskeleton::reset(): Reset complete. " << endl;

}

int
ZSkeleton::numJoints() const
{
    return (int)_joints.size();
}

int
ZSkeleton::numFrames() const
{
	return (int)_frameNum;
}

void
ZSkeleton::setCurrentFrame( float frame )
{
	_currentFrame = frame;
}

void
ZSkeleton::setDrawingMode( int drawingMode )
{
	_drawingMode = drawingMode;
}

void
ZSkeleton::setDrawingRadius( float drawingRadius )
{
	_drawingRadius = drawingRadius;
}

int
ZSkeleton::jointIndex( const char* jointName ) const
{	
	if( _joints.size() > 0 )
	{
		FOR(i, 0, _joints.size())
		{
			if( _joints[i]->name == jointName )
			{ return i; }
		}
	}
	
    return -1;
}

ZSkeleton::Joint*
ZSkeleton::getRoot() const
{
	return _root;
}

ZString
ZSkeleton::jointName( Joint* joint)
{
	return joint->name;
}

ZString 
ZSkeleton::jointName( const int index )
{
	if( _joints.size() > 0 )
	{
		return _joints[index]->name;
	}
}

int
ZSkeleton::childNum( const int index ) const
{
	if( _joints.size() > 0 )
	{
		return _joints[index]->child.size();
	}
}

int
ZSkeleton::childNum( Joint* joint ) const
{
	if( _joints.size() > 0 )
	{
		return joint->child.size();
	}
}

ZSkeleton::Joint*
ZSkeleton::getParent( Joint* joint ) const
{
	return joint->parent;
}

ZSkeleton::Joint*
ZSkeleton::childAt( Joint* joint, int childIndex ) const
{
	return joint->child[childIndex];
}

vector<ZString>
ZSkeleton::getChannels( Joint* joint ) const
{
	return joint->channels;
}

vector<float>
ZSkeleton::getOffsets( Joint* joint ) const
{
	return joint->offsets;
}

vector<ZVector> 
ZSkeleton::translation( int jointIndex ) const
{
	int i; vector<ZVector> trans;
	FOR( i, 0, _frameNum )
	{
		ZVector t(_joints[jointIndex]->Xposition[i],_joints[jointIndex]->Yposition[i], _joints[jointIndex]->Zposition[i] );
		trans.push_back(t);
	}
	return trans;
}

// 1frame
ZVector
ZSkeleton::translation( int jointIndex, int frame ) const
{
	if(_joints[jointIndex]->Xposition.size()>0 && _joints[jointIndex]->Yposition.size()>0 && _joints[jointIndex]->Zposition.size()>0 )
	{
		ZVector trans(_joints[jointIndex]->Xposition[frame],_joints[jointIndex]->Yposition[frame], _joints[jointIndex]->Zposition[frame] );
	    return trans;
	}
}

// 1frame
ZVector
ZSkeleton::translation( ZSkeleton::Joint* joint, int frame ) const
{
	if(joint->Xposition.size()>0 && joint->Yposition.size()>0 && joint->Zposition.size()>0 )
	{
		ZVector trans(joint->Xposition[frame], joint->Yposition[frame], joint->Zposition[frame] );
		return trans;
	}
}

vector<ZVector>
ZSkeleton::orientation( int jointIndex ) const
{
	int i; vector<ZVector> rotates;
	FOR( i, 0, _frameNum )
	{
		ZVector r(_joints[jointIndex]->Xrotation[i],_joints[jointIndex]->Yrotation[i], _joints[jointIndex]->Zrotation[i] );
		rotates.push_back(r);
	}
	return rotates;
}

// 1frame
ZVector
ZSkeleton::orientation( int jointIndex, int frame ) const
{
	if(_joints[jointIndex]->Xrotation.size()>0 && _joints[jointIndex]->Yrotation.size()>0 && _joints[jointIndex]->Zrotation.size()>0 )
	{
		ZVector rotate(_joints[jointIndex]->Xrotation[frame],_joints[jointIndex]->Yrotation[frame], _joints[jointIndex]->Zrotation[frame] );
		return rotate;
	}
}

// 1frame
ZVector
ZSkeleton::orientation( ZSkeleton::Joint* joint, int frame ) const
{
	if(joint->Xrotation.size()>0 && joint->Yrotation.size()>0 && joint->Zrotation.size()>0 )
	{
		ZVector rotate(joint->Xrotation[frame], joint->Yrotation[frame], joint->Zrotation[frame] );
		return rotate;
	}
}

void
ZSkeleton::_drawCapsule( float radius = 0.4, float length = 5 ) const
{
    const int numSlices = 20;
    const int numStacks = 20;

	// LIGHT
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	
	glEnable(GL_COLOR_MATERIAL);

	glPushMatrix();	
    {
		glTranslatef( 0, +length, 0 );
    	gluSphere   ( upSphereObj, radius/4, numSlices, numStacks );
	}
	glPopMatrix();
		
	glPushMatrix();
	{
		glRotatef(-90,1,0,0);
		gluCylinder( cylinderObj, radius, radius/4, length, numSlices, numStacks );
	}
	glPopMatrix();
	
    gluSphere( downSphereObj, radius, numSlices, numStacks );

	glDisable(GL_LIGHTING);
}

void
ZSkeleton::_drawSphere( float radius = 0.4 ) const
{
	const int numSlices = 20;
    const int numStacks = 20;

	// LIGHT
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	
	glEnable(GL_COLOR_MATERIAL);

	gluSphere( upSphereObj, radius, numSlices, numStacks );

	glDisable(GL_LIGHTING);
}

void
ZSkeleton::draw()
{
	
	if(_joints.size()>0)
	{		
		recursDraw( _root, _drawingMode );
	}
	cout << endl << endl;
	
}
void
ZSkeleton::recursDraw( ZSkeleton::Joint* joint=NULL, int drawMode=2 )
{	
	int frame = _currentFrame / _frameUnit;
	if( frame > _frameNum - 1 ) { frame = _frameNum - 1; }
	else if( frame < 0        ) { frame = 0; }

	// CALCULATE MATRIX
	ZMatrix m1, m2;
	m1.setToIdentity();
	m2.setToIdentity();

	//cout << joint->name << endl;
	// offset
	if( joint->offsets.size() > 0 )
	{
		m1.setTranslation( joint->offsets[0],joint->offsets[1], joint->offsets[2] );
	}

	// position
	if( joint->Xposition.size() > 0 )
	{	
		m1.setTranslation( joint->Xposition[frame], joint->Yposition[frame], joint->Zposition[frame] );
	}

	//cout << m1.translation() << endl;
	// first rotation key
	if( joint->Xrotation.size() > 0 )
	{ 
		m2.setRotation(joint->Xrotation[frame], joint->Yrotation[frame], joint->Zrotation[frame], false ); 
		//cout << joint->Xrotation[0] << " " << joint->Yrotation[0] << " " << joint->Zrotation[0] << endl;
	}
	// multiply
	m1 = m1 * m2;
	GLfloat glmatrix[16] = { m1._00, m1._10, m1._20, m1._30,
                             m1._01, m1._11, m1._21, m1._31,
                             m1._02, m1._12, m1._22, m1._32,
                             m1._03, m1._13, m1._23, m1._33 };

	glPushMatrix();
	{
		glMultMatrixf( glmatrix );

		////////////////			
		// DRAW LINES //
		if( drawMode == 1 )
		{
			if( joint->child.size() > 0 )
			{
				for( int i=0; i<joint->child.size(); i++ )
				{
					glLineWidth( _drawingRadius );
					glColor3f( 0.8f, 0.1f, 0.1f ); // red
					glPushMatrix();
					{
						_drawSphere( _drawingRadius );
						glBegin(GL_LINES);
						{
							glVertex3f( 0, 0, 0 );
							if( joint->child[i]->Xposition.size() > 0 )
							{
								glVertex3f( joint->child[i]->Xposition[frame],
											joint->child[i]->Yposition[frame],
											joint->child[i]->Zposition[frame]);
							}
							else
							{
							glVertex3f( joint->child[i]->offsets[0],joint->child[i]->offsets[1],joint->child[i]->offsets[2] );
							}
						}
						glEnd();
					}
					glPopMatrix();
				}
			}
		}
						
		////////////////////			
		// DRAW CYLINDERS //
		if( drawMode == 2 )
		{
			if( joint->child.size() > 0 )
			{

				dispListId = glGenLists( joint->child.size() );
				for( int i=0; i<joint->child.size(); i++ )
				{
					float tempx, tempy, tempz;
					float yaw, padj, pitch;
					if( joint->child[i]->Xposition.size() > 0 )
					{
					tempx = joint->child[i]->Xposition[frame];
					tempy = joint->child[i]->Yposition[frame]; 
					tempz = joint->child[i]->Zposition[frame];
					}
					else
					{
					//z=0
					tempx = joint->child[i]->offsets[0];
					tempy = joint->child[i]->offsets[1]; 
					tempz = joint->child[i]->offsets[2];
					}

					yaw = ZRadToDeg( atan2( tempx, tempz ) );
					padj = sqrt(pow(tempx,2)+pow(tempz,2));
					pitch = ZRadToDeg( atan2(padj, tempy) );

					glPushMatrix();
					{
						glRotatef(yaw,0,1,0);
						glRotatef(pitch,1,0,0);
						glPushMatrix();
						{
							ZVector cylength = { tempx, tempy, tempz };
							glNewList( dispListId + i, GL_COMPILE );
							{
								if( joint->child[i]->child.size() > 0 )
								{ 
									glColor3f( 0.5f, 0.5f, 0.5f ); 
									_drawCapsule( _drawingRadius, cylength.length() ); 
								}
								else
								{
									glColor3f( 0.8f, 0.1f, 0.1f );
									_drawSphere( _drawingRadius );
								}

							}
							glEndList();
							glCallList( dispListId + i );
						}
						glPopMatrix();
					}
					glPopMatrix();

				}
			}
		}

		for( int i=0; i<joint->child.size(); i++ )
		{		
			recursDraw( joint->child[i], drawMode );
		}
	}
	glPopMatrix();
}

bool
ZSkeleton::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "error@zskeleton::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

    const ZString ext = ZFileExtension( filePathName );

    if( ext == "anim" ) { return _save_ANIM( fout ); }
    if( ext == "bvh"  ) { return _save_BVH ( fout ); }
    if( ext == "json" ) { return _save_JSON( fout ); }

	fout.close();
	
    return true;
}

bool
ZSkeleton::load( const char* filePathName )
{
	ZSkeleton::reset();

	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZSkeleton::load(): Failed to load file." << endl;
		return false;
	}

    const ZString ext = ZFileExtension( filePathName );
	
    //if( ext == "anim" ) { return _load_ANIM( fin ); }
	if( ext == "anim" ) { return false; }
    if( ext == "bvh"  ) { return _load_BVH ( fin ); }
    if( ext == "json" ) { return _load_JSON( fin ); }

	fin.close();

    return true;
}

bool
ZSkeleton::_save_ANIM( ofstream& fout ) const
{
	fout << "animVersion 1.1;" << endl;
	fout << "mayaVersion 2017;" << endl;
	fout << "timeUnit film;" << endl;
	fout << "linearUnit cm;" << endl;
	fout << "angularUnit deg;" << endl;
	fout << "startTime " << 0 << ";" << endl;
	fout << "endTime " << _endTime << ";" << endl;

	for ( int i=0; i < _joints.size(); i++)
	{
		// END SITE EXCEPTION
		int childNum;
		if( _joints[i]->child.size() > 1 )
		{
			childNum = _joints[i]->child.size();
		}
		else if( _joints[i]->child.size() == 1 )
		{
			if( _joints[i]->child[0]->child.size() > 0 ) { childNum = 1; }
			else                                         { childNum = 0; }
		}
		else if( _joints[i]->child.size() == 0 )	
		{ 
			continue; 
		}
		
		if (_joints[i]->channels.size() > 0 )
		{
			for ( int k=0; k < _joints[i]->channels.size(); k++)
			{
				// JOINT NAME :: anim translate.translateX translateX C_Skin_root_JNT 0 1 0;
				ZString chn = _joints[i]->channels[k]; 
				fout << "anim " << chn.substr( 0, chn.size()-1 ) << "." << chn << " " << chn << " ";
				fout << _joints[i]->name << " ";
				fout << "0 " << childNum << " " << k << ";" << endl;

				// key data start
				fout << "animData {\n  input time;\n  ";
				// rotate or translate
				if (chn.substr( 0, chn.size()-1 ) == "rotate")
				{ fout << "output angular"; }
				else
				{ fout << "output linear"; }
				// key info
				fout << ";\n  weighted 0;\n  preInfinity constant;\n  postInfinity constant;\n  keys {\n";

				// key per channels
				for ( int j=0; j<_frameNum; j++ )
				{
					if ( chn.substr( 0, chn.size()-1 ) == "translate" && _joints[i]->Xposition.size() > 0 )
					{
						fout << "    " << _frameUnit * j << " "; //frame
						if ( chn == "translateX" )
						{ fout << _joints[i]->Xposition[j] << " "; } // key value
						else if ( chn == "translateY" )
						{ fout << _joints[i]->Yposition[j] << " "; }
						else if ( chn == "translateZ" )
						{ fout << _joints[i]->Zposition[j] << " "; }
						fout << "spline spline 1 0 0;" << endl; //default
					}
					if ( chn.substr( 0, chn.size()-1 ) == "rotate" && _joints[i]->Xrotation.size() > 0 )
					{
						fout << "    " << _frameUnit * j << " "; //frame
						if ( chn == "rotateX" )
						{ fout << _joints[i]->Xrotation[j] << " "; } // key value
						else if ( chn == "rotateY" )
						{ fout << _joints[i]->Yrotation[j] << " ";  }
						else if ( chn == "rotateZ" )
						{ fout << _joints[i]->Zrotation[j] << " ";  }
						fout << "spline spline 1 0 0;" << endl; //default
					}
				}
				
				fout << "  }" << endl << "}" << endl;
			}	
		}
		else
		{
			// no channels
			fout << "anim " << _joints[i]->name << " "; //anim BVH:L_Skin_BackLegToeTip_JNT 0 1 0;
			fout << "0 " << _joints[i]->child.size() << " " << 0 << ";" << endl;
		}
	}
	cout << "# ANIM saved successfully." << endl;
    return true;
}

bool
ZSkeleton::_save_BVH( ofstream& fout ) const
{
	// bone structure
	fout << "HIERARCHY" << endl;
	int childCount,parentCount = 0;	
	vector<int> parentStack;
	
	ZString space = "";
	for( int i=0; i<_joints.size(); i++ )
	{
		Joint* b = _joints[i];
		if (i == 0)
		{
			fout << "ROOT " << b->name << endl;
		}
		else
		{
			fout << space << "JOINT " << b->name << endl;
			childCount++;
		}
		
		if( ( i != 0 ) && ( b->child.size() > 1 ) )
		{ parentStack.push_back( b->child.size() ); childCount = 0; }
		
		fout << space << "{" << endl;
		space+= "  ";
		
		// OFFSET
		fout << space << "OFFSET ";
		if( b->Xposition.size() > 0 && b->Yposition.size() > 0 && b->Zposition.size() > 0 )
		{
			fout << b->Xposition[0] << " " << b->Yposition[0] << " " << b->Zposition[0] << endl;
		}
		else if( b->Xposition.size() == 0 && b->Yposition.size() == 0 && b->Zposition.size() == 0 )
		{
			cout << "Error@ZSkeleton::_save_BVH(): BVH save failed, translation key from anim file needed." << endl;
			return true;
			//fout << 0.0 << " " << 0.0 << " " << 0.0 << endl;
		}

		// CHANNEL
		if( b->channels.size() > 3 )
		{
			fout << space << "CHANNELS " << b->channels.size() << " ";

			fout << "Xposition" << " " << "Yposition" << " " << "Zposition" << " ";
			fout << "Zrotation" << " " << "Xrotation" << " " << "Yrotation" << endl;

		}
		
		if( b->child.size() == 0 )
		{
		    fout << space << "End Site" << endl;
		    fout << space << "{" << endl;
		    fout << space << "  OFFSET 0 0 0" << endl;
		    fout << space << "}" << endl;
		    parentCount++;
		
			// close '}'
			if( parentStack.size()>0)
			{
		    	if(parentCount == parentStack[parentStack.size()-1] )
				{ parentStack.pop_back(); childCount += parentCount + 1; parentCount = 0; }
			}		    

			for( int c=0; c<childCount; c++ )
			{
				fout << space.substr( 0,space.size()-2 ) << "}" << endl;
				space = space.substr( 0,space.size()-2 );
			}
			childCount = 0;
		}
	}

	fout << "}" << endl;

	// FRAMES
	fout << "MOTION"       << endl;
	fout << "Frames: "     << _keyframes.size() << endl;
	fout << "Frame Time: " << _frameUnit / 24 << endl;

	// MOTION
	// for _keyframes
	for( int f=0; f<_keyframes.size(); f++)
	{
		for( int k=0; k<_joints.size(); k++)
		{
			Joint* b = _joints[k];

			// KEYS
			if( b->channels.size() > 3 )
			{
				// TRANS
				fout << b->Xposition[f] << " ";
				fout << b->Yposition[f] << " ";
				fout << b->Zposition[f] << " ";
				
				// ROTATES
				float tempX, tempY, tempZ;
				tempX = b->Xrotation[f];
				tempY = b->Yrotation[f];
				tempZ = b->Zrotation[f];
		
				// convert to angle	
				vector<float> output = _getAngle( tempX, tempY, tempZ );
				fout << output[2] << " " << output[0] << " " << output[1] << " ";
			}
		}
		fout << endl;
	}
	cout << "# BVH saved successfully." << endl;
	return true;
}

bool
ZSkeleton::_save_JSON( ofstream& fout ) const
{
	ZJSON json;
	json.append("key1","data1");
	json.append("key2","data2");
	cout << json << endl;

	ZJSON json1;
	json1.append("key3","data3");

	json["key3"] = "data3";
    return true;
}

bool
ZSkeleton::_load_ANIM( ifstream& fin )
{
	for (ZString line; getline(fin, line);)
	{
		_read_ANIM(line.c_str());
		_process_ANIM(line.c_str());
	}
	_readRecursive(_root);

	_eulerRecursive(_root);

	// STORE OFFSET
	for( int i=0; i<_joints.size(); i++ )
	{
		if( _joints[i]->Xposition.size() > 0 )
		{
			_joints[i]->offsets[0] = _joints[i]->Xposition[0];
			_joints[i]->offsets[1] = _joints[i]->Yposition[0];
			_joints[i]->offsets[2] = _joints[i]->Zposition[0];
		}
	}

	float ftotal;
	for( int m=1; m<_keyframes.size(); m++ )
	{ ftotal += ( _keyframes[m] - _keyframes[m-1] ); } // average frame rate
	_frameUnit = roundf(ftotal / _keyframes.size() * 10 ) / 10;
	_frameNum  = _keyframes.size();

	cout << "# ANIM loaded successfully." << endl;
	cout << "# Bone size: " << _joints.size() << endl;
	cout << "# Frame size: " << _keyframes.size()-1 << endl;
    return true;
}

bool
ZSkeleton::_load_BVH( ifstream& fin )
{
	istream_iterator<ZString> bvhIter(fin);
	istream_iterator<ZString> sentinel;
	vector<ZString> lines(bvhIter, sentinel);
	int i;
	for( i=0; i<lines.size(); i++ )
	{
		_read_BVH( lines[i].c_str() );
	}
	_eulerRecursive(_root);	

	cout << "# BVH loaded successfully." << endl;
	cout << "# Bone size: " << _joints.size() << endl;
	cout << "# Frame size: " << _frameNum << endl; 
    return true;
}

bool
ZSkeleton::_load_JSON( ifstream& fin )
{
    return true;
}

bool
ZSkeleton::_read_BVH( const char* line )
{
	ZString l = line;
	if ( l == "ROOT")
	{
		_root->parent = NULL;
		_current = _root;
		_theMode = ROOT;
	}
	else if( l == "{" )
	{
		_theMode = NONE;
	}
	else if( l == "}" )
	{
		if ( _current != _root )
		{
			_current = _current->parent;
			_theMode = NONE;
		}

	}
	else if( l == "OFFSET")
	{
		_vertIndex = 0;
		_theMode = OFFSET;
	}
	else if( l == "CHANNELS" ) 
	{
		_theMode = CHANNELS;
	}
	else if( l == "JOINT")
	{				
		Joint* temp = new Joint;
		temp->parent = _current;
		_current->child.push_back(temp);
		_current = temp;
		_theMode = JOINT;
	}
	else if( l == "Site")
	{
		Joint* temp = new Joint;
		temp->name = _current->name + "_End";
		temp->parent = _current; // temp->parent = bone1
		_current->child.push_back(temp); // bone1.child = temp 
		_current = temp;
		_theMode = NONE;
	}
	else if( l == "Frames:")
	{
		_theMode = Frames;
	}
	else if( l == "Time:")
	{
		_theMode = Time;
	}
	else
	{
		_process_BVH(line);
	}

	return true;
}

bool
ZSkeleton::_read_ANIM( const char* line )
{
	ZString l = line;
	istringstream buf(l);
	istream_iterator<ZString> beg(buf), end;
	vector<ZString> words(beg, end);

	// read & close
	int i;
	for (i=0; i<words.size(); i++)
	{
		if( words[i] == "anim" )
		{
			ZString tempName = "";
			int tempChildNum;
			
			// default : no channels
			tempName     = words[1];
			tempChildNum = atoi(words[3].c_str());
			if( words.size() > 5 )
			{
				_tempAttr = words[2];
				if (_tempAttr.substr(0, _tempAttr.size()-1 ) != "translate" && _tempAttr.substr(0, _tempAttr.size()-1 ) != "rotate"  ) 
				{ _theMode = NONE; break; }
				tempName = words[3];
				tempChildNum = atoi(words[5].c_str());
			}
		
			// _root 
			if( _boneIndex == 0 )
			{
				_root->parent = NULL;
				_current      = _root;
				_root->name   = tempName;
				_boneIndex++;
			}
			
			// add child
			else
			{
				if( _current->name != tempName )
				{
					if ( _parent != NULL ) 
					{
						_current = _parent;
						_parent  = NULL;
					}

					Joint *temp  = new Joint;
					temp->parent = _current;
					temp->name   = tempName;
					_current->child.push_back(temp);
					_current = temp;
					_boneIndex++;
					_childCount++;

					// find parent
					if( tempChildNum == 0 && _parent == NULL ) // end of hierarchy
					{

						_subParentCount--;
						if( _subParentCount == 0 )
						{ _childCount = _childCount + _lastParentCount; }

						_parent = _current;
						for( int i=0; i<_childCount; i++ )
						{
							_parent = _parent->parent;
						}
						_childCount = 0;

					}
			
					if( tempChildNum > 1 )
					{ _lastParentCount = _childCount; _childCount = 0; _subParentCount=tempChildNum; }

					// when end of hierarchy and no attr	
					if( _tempAttr == "" && _parent != NULL )
					{
						_current = _parent;
						_parent  = NULL;
					}

				}
			}

			if( _tempAttr != "" )
			{
				_current->channels.push_back(_tempAttr);	
			}
			_theMode = NONE;

		}
		else if( words[i] == "keys")
		{
			if ( _tempAttr.substr(0, _tempAttr.size()-1 ) == "translate" || _tempAttr.substr(0, _tempAttr.size()-1 ) == "rotate" )	
			{ _theMode = KEYS; }
		}
		else if( words[i] == "}")
		{
			_theMode = NONE;
		}
	}
	return true;
}


bool
ZSkeleton::_process_BVH( const char* line )
{
	ZString l = line;
	switch(_theMode)
	{
		case(ROOT):
			_root->name = line;
			_theMode = NONE;
			break;

		case(JOINT):
			_current->name = line;
			_theMode = NONE;
			break;
			
		case(OFFSET):
			_current->offsets[_vertIndex] = atof(line);
			_vertIndex++;
			if (_vertIndex >2) {
				_vertIndex = 0;
				_theMode = NONE;
			}
			break;

		case(CHANNELS):
			if (_channelsNum == 0){
				_channelsNum = atoi(line);
			} else if (l == "Xposition") {	
				_current->channels.push_back("translateX");	
			} else if (l == "Yposition") {					
				_current->channels.push_back("translateY");	
			} else if (l == "Zposition") {				
				_current->channels.push_back("translateZ");	
			} else if (l == "Zrotation") {
				_current->channels.push_back("rotateZ");				
			} else if (l == "Yrotation") {
				_current->channels.push_back("rotateY");				
			} else if (l == "Xrotation") {
				_current->channels.push_back("rotateX");				
			}				
			// initialize
			if (_current->channels.size() == _channelsNum)
			{
				_channelTotal += _channelsNum;
				_channelsNum = 0;
			}
			break;

		case(Frames):
			_frameNum = atoi(line);
			_theMode = NONE;
			break;

		case(Time):
			_frameTime = atof(line);
			_frameUnit = 24 / ( roundf( 1/_frameTime) );
			_endTime   = (_frameNum-1)*_frameUnit;
			cout << "frame unit: " << _frameUnit << endl;				
			cout << "endTime " << _endTime << endl;
			_mdataCount = 0;
			_theMode = MOTIONDATA;
			_readRecursive(_root);
			break;
		
		case(MOTIONDATA):
			_mdataCount++;
			if ( (_boneIndex < _joints.size()-1) && ( _joints[_boneIndex]->channels.size() == 0 ))
			{ _boneIndex++; }
			else if ((_boneIndex == _joints.size()-1)&&( _joints[_boneIndex]->channels.size() == 0 )) //joint end
			{ _boneIndex = 0; }
			
			// store temp data
			if (_channelIndex < _joints[_boneIndex]->channels.size())
			{
				if (_joints[_boneIndex]->channels[_channelIndex] == "translateX"){
					_tempTrans.push_back(atof(line));
				} else if (_joints[_boneIndex]->channels[_channelIndex] == "translateY"){
					_tempTrans.push_back(atof(line));
				} else if (_joints[_boneIndex]->channels[_channelIndex] == "translateZ"){
					_tempTrans.push_back(atof(line));
				} else if (_joints[_boneIndex]->channels[_channelIndex] == "rotateX"){
					_tempRotates.x = atof(line);
				} else if (_joints[_boneIndex]->channels[_channelIndex] == "rotateY"){
					_tempRotates.y = atof(line);
				} else if (_joints[_boneIndex]->channels[_channelIndex] == "rotateZ"){
					_tempRotates.z = atof(line);
				} 
				_channelIndex ++;
			}
			// calculate euler rotation and store to "rotations"
			if (_channelIndex >= _joints[_boneIndex]->channels.size())
			{
				_tempMotion.setToIdentity();

				ZMatrix rotateX; rotateX.setToIdentity();
				rotateX.setRotation( _tempRotates.x, 0 ,0 );
				ZMatrix rotateY; rotateY.setToIdentity();
				rotateY.setRotation( 0, _tempRotates.y, 0 );
				ZMatrix rotateZ; rotateZ.setToIdentity();
				rotateZ.setRotation( 0, 0, _tempRotates.z );

				_tempMotion = rotateZ * rotateX * rotateY;
				_tempMotion.getRotation( _tempRotates.x, _tempRotates.y, _tempRotates.z, true );

				// check rotate , trans exist
				int t; bool trans, rotate; trans = rotate = false;
				for(t=0;t<_joints[_boneIndex]->channels.size();t++)				
				{
					string chname = _joints[_boneIndex]->channels[t].substr(0, _joints[_boneIndex]->channels[t].size()-1);
					if( chname == "translate" )
					{ trans = true; }
					else if( chname == "rotate" )
					{ rotate = true; }
				}

				if ( trans == true )
				{
					_joints[_boneIndex]->Xposition.push_back(_tempTrans[0]); //tx
					_joints[_boneIndex]->Yposition.push_back(_tempTrans[1]); //ty
					_joints[_boneIndex]->Zposition.push_back(_tempTrans[2]); //tz
				}

				if ( rotate == true )
				{
					_joints[_boneIndex]->Xrotation.push_back(_tempRotates.x); 
					_joints[_boneIndex]->Yrotation.push_back(_tempRotates.y); 
					_joints[_boneIndex]->Zrotation.push_back(_tempRotates.z); 
				}

				_tempTrans.clear();
				_tempRotates.zeroize();
				_channelIndex = 0; // initialize channel index
				_boneIndex ++; // next bone
			}

			if (_boneIndex >= _joints.size())
			{
				_boneIndex = 0; // end
				break;
			}	
	
			break;	
			
	}
	return true;
}

bool
ZSkeleton::_process_ANIM( const char* line )
{

	istringstream buf(line);
	istream_iterator<ZString> beg(buf), end;
	vector<ZString> words(beg, end);

	switch(_theMode)
	{
		case(KEYS):
			int i; bool key=false;
			float frame = atof(words[0].c_str());
			ZString currentAttr = _tempAttr;

			for (i=0; i < words.size(); i++)	
			{
				if( words[i] == "keys" || words[i] == "{" ){
					key=false; break; 
				} else if ( _tempAttr.substr( 0, _tempAttr.size()-1 ) == "translate" || _tempAttr.substr( 0, _tempAttr.size()-1 ) == "rotate"  ) {
					key=true; break;
				}
			}
			if( key == true )
			{
				if ( _keyframes.size() == 0 )
				{
					_keyframes.push_back(frame);
				}
				else 
				{
					if ( frame > _keyframes[_keyframes.size()-1] + 0.01 )
					{
						_keyframes.push_back(frame);
					}
				}
				if(currentAttr == "translateX"){
					_current->Xposition.push_back(atof(words[1].c_str()));
				} else if(currentAttr == "translateY"){
					_current->Yposition.push_back(atof(words[1].c_str()));
				} else if(currentAttr == "translateZ"){
					_current->Zposition.push_back(atof(words[1].c_str()));
				} else if(currentAttr == "rotateX"){
					_current->Xrotation.push_back(atof(words[1].c_str()));
				} else if(currentAttr == "rotateY"){
					_current->Yrotation.push_back(atof(words[1].c_str()));
				} else if(currentAttr == "rotateZ"){
					_current->Zrotation.push_back(atof(words[1].c_str()));
				}
			}
			currentAttr = "";	
			break;

	}
	return true;
}

bool
ZSkeleton::_readRecursive( Joint* joint )
{
	_joints.push_back(joint);
	for ( int i=0; i<joint->child.size(); i++ )	
	{ 
		_readRecursive(joint->child[i]);
	}
}

bool
ZSkeleton::_eulerRecursive( Joint* joint )
{
	_eulerFilter(joint);
	int i;
	for (i=0; i<joint->child.size(); i++)	
	{ 
		_eulerRecursive(joint->child[i]);		
	}
}

bool
ZSkeleton::_eulerFilter( Joint* joint )
{
	int i, j = 0;
	for( j=1; j < joint->Xrotation.size(); j++ ) // key frame number
	{
		int beform = (j-1);
		int currm = j;
	
		// rx
		float before_rx  = joint->Xrotation[ beform  ];
		float current_rx = joint->Xrotation[ currm  ];
		current_rx = _naive_flip_diff(before_rx, current_rx);

		// ry
		float before_ry  = joint->Yrotation[ beform ];
		float current_ry = joint->Yrotation[ currm  ];
		current_ry = _naive_flip_diff(before_ry, current_ry);

		// rz
		float before_rz  = joint->Zrotation[ beform ];
		float current_rz = joint->Zrotation[ currm  ];
		current_rz = _naive_flip_diff(before_rz, current_rz);

		float fcurrent_rx = current_rx + 180;
		float fcurrent_ry = current_ry + 180;
		fcurrent_ry *= -1;
		float fcurrent_rz = current_rz + 180;

		fcurrent_rx = _naive_flip_diff(before_rx, fcurrent_rx);
		fcurrent_ry = _naive_flip_diff(before_ry, fcurrent_ry);
		fcurrent_rz = _naive_flip_diff(before_rz, fcurrent_rz);

		float distance  = fabs(before_rz - current_rz) + fabs(before_rx - current_rx) + fabs(before_ry - current_ry);
		float fdistance = fabs(before_rz - fcurrent_rz) + fabs(before_rx - fcurrent_rx) + fabs(before_ry - fcurrent_ry);

		if ( fdistance < distance )
		{
		    current_rx = fcurrent_rx;
		    current_ry = fcurrent_ry;
		    current_rz = fcurrent_rz;
        }

		// store
		joint->Xrotation[ currm  ] = current_rx; //z
		joint->Yrotation[ currm  ] = current_ry; //x
		joint->Zrotation[ currm  ] = current_rz; //y
	}
}

float 
ZSkeleton::_naive_flip_diff(float pre, float cur)
{
	while (fabs( pre - cur ) > 180 )
	{
		if (pre < cur)
		{ cur -= 2 * 180; }
		else
		{ cur += 2 * 180; }
	}

	return cur;
}

vector<float>
ZSkeleton::_getAngle(float x, float y, float z) const
{
	x = ZDegToRad(x);
	y = ZDegToRad(y);
	z = ZDegToRad(z);

	if ( x <= 0.0 && x > -0.000000000001 ) { x = 0.0; }
	if ( y <= 0.0 && y > -0.000000000001 ) { y = 0.0; }
	if ( z <= 0.0 && z > -0.000000000001 ) { z = 0.0; }

	float A = cos(x);
	float B = sin(x);
	float C = cos(y);
	float D = sin(y);
	float E = cos(z);
	float F = sin(z);
	
	x = ZRadToDeg( asin( B*C ));
	y = ZRadToDeg(atan2( D, A*C ));
	z = ZRadToDeg(atan2( -1*B*D*E + A*F, B*D*F + A*E ));

	x = _noScience(x);
	y = _noScience(y);
	z = _noScience(z);

	vector<float> output = {x,y,z};
	return output;
}

float
ZSkeleton::_noScience( float num ) const
{
	if ( num > -0.1 && num < -1.0 )
		num = num/100000;
	else 
		num = num*100000;

	float tnum = round(num);
	if ( ( num - tnum ) >= 0.5 )
		num = ceil(num);
	else
		num = floor(num);

	num = num/100000;
	return num;
}

ostream&
operator<<( ostream& os, const ZSkeleton& object )
{
	os << "<ZSkeleton>" << endl;
	os << " # of joint: " << object.numJoints() << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

