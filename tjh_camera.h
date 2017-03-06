#ifndef TJH_CAMERA_H
#define TJH_CAMERA_H

////// UNLICENSE ///////////////////////////////////////////////////////////////
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>

////// DEPENDENCIES ////////////////////////////////////////////////////////////
//
// This single header library relies on the following:
//
// GLEW v2.0.0 which is Copyright (C) Nate Robins - 1997, Michael Wimmer - 1999
// Milan Ikits - 2002-2008, Nigel Stewart - 2008-2013 and distributed under the
// Modified BSD License, Mesa 3-D Licsense (MIT) and the Khronos License (MIT).
// See here for more http://glew.sourceforge.net/

////// TODO ////////////////////////////////////////////////////////////////////
//
// - write the how to use info in documentation
// - write short tutorial in documentation
// - remove set right
//		- instead calculate right when set dir or set up
// - is it worth trying to remove the dependancy on GLM?

////// DOCUMENTATION ///////////////////////////////////////////////////////////
//

////// LIBRARY OPTIONS /////////////////////////////////////////////////////////

// Change this to customise the class name
#define TJH_CAMERA_TYPENAME Camera

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

////// HEADER //////////////////////////////////////////////////////////////////

class TJH_CAMERA_TYPENAME
{
public:
	TJH_CAMERA_TYPENAME();
	~TJH_CAMERA_TYPENAME() {}
	
	// Setters
	void setUp( glm::vec3 up );
	void setRight( glm::vec3 right ) { right_ = right; }
	void setPosition( glm::vec3 pos ) { pos_ = pos; }
	void setDirection( glm::vec3 dir );
	void setLookAt( glm::vec3 look_at ) { dir_ = glm::normalize( (look_at - pos_) ); }
	void setVerticalFOV( float fov ) { vertical_fov_ = glm::radians(fov); }
	void setNearFarPlane( float near, float far ) { near_plane_ = near; far_plane_ = far; }

	// Getters
	glm::vec3 position() const { return pos_; }
	glm::vec3 direction() const { return dir_; }
	glm::vec3 up() const { return up_; }
	glm::vec3 right() const { return right_; }
	glm::mat4 view() const { return glm::lookAt( pos_, pos_ + dir_, up_ ); }
	glm::mat4 projection( int width, int height ) const;

private:

	glm::vec3 pos_; // Position
	glm::vec3 dir_; // Direction the camera is facing
	glm::vec3 up_;
	glm::vec3 right_; // TODO: CALCULATE ME

	float vertical_fov_;
	float near_plane_;
	float far_plane_;
};

#endif // END TJH_CAMERA_H

////// IMPLEMENTATION //////////////////////////////////////////////////////////

#ifdef TJH_CAMERA_IMPLEMENTATION

TJH_CAMERA_TYPENAME::TJH_CAMERA_TYPENAME() :
pos_( 3.0f, 2.0f, 3.0f ),
dir_( 1.0f, 0.0f, 0.0f ),
up_( 0.0f, 1.0f, 0.0f ),
right_( 1.0f, 0.0f, 0.0f ),
vertical_fov_( glm::radians(45.0f) ),
near_plane_( 0.1f ),
far_plane_( 200.0f )
{
	setLookAt( glm::vec3(0, 0, 0) );
}

void TJH_CAMERA_TYPENAME::setUp( glm::vec3 up )
{
	up_ = up;
}

void TJH_CAMERA_TYPENAME::setDirection( glm::vec3 dir )
{
	dir_ = dir;
}

glm::mat4 TJH_CAMERA_TYPENAME::projection( int width, int height ) const
{
	return glm::perspective( vertical_fov_, width / (float)height, near_plane_, far_plane_ );
}

#endif // END TJH_CAMERA_IMPLEMENTATION