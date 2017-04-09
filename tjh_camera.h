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
// - Actually calculate the right vector
// - remove the dependancy on GLM
// 		- maybe have switch to turn on/off GLM
//		- then have normal float x, y, z versions that always work
// - Fix dependancies info! should be GLM not GLEW!
// - Orthogrphic mode
// - clamp vertical rotation at directly vertical, possibly requires spherical coords
// - add helpers for camera interactivity
//  - forward, back
//  - forwardHorizontal, backHorizontal <- for moving locked on the horizontal plane. Needs better names...

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
    TJH_CAMERA_TYPENAME( float x, float y, float z, float look_at_x, float look_at_y, float look_at_z );
	~TJH_CAMERA_TYPENAME() {}
	
    // Movement helpers
    void moveUp( float distance )       { pos_ += up_ * distance; }
    void moveDown( float distance )     { moveUp( -distance ); }
    void moveLeft( float distance )     { pos_ -= right_ * distance; }
    void moveRight( float distance )    { moveLeft( -distance ); }
    void moveForward( float distance )  { pos_ += dir_ * distance; }
    void moveBack( float distance )     { moveForward( -distance ); }
    void rotateLeft( float radians );
    void rotateRight( float radians )   { rotateLeft( -radians ); }
    void rotateUp( float radians );
    void rotateDown( float radians )    { rotateUp( -radians ); }
    
	// Setters
	void setUp( glm::vec3 up );
	void setPosition( glm::vec3 pos )             { pos_ = pos; }
	void setDirection( glm::vec3 dir );
	void setLookAt( glm::vec3 look_at );
	void setVerticalFOV( float fov )              { vertical_fov_ = glm::radians(fov); }
	void setNearFarPlane( float near, float far ) { near_plane_ = near; far_plane_ = far; }

	// Getters
	glm::vec3 position()    const { return pos_; }
	glm::vec3 direction()   const { return dir_; }
	glm::vec3 up()          const { return up_; }
	glm::vec3 right()       const { return right_; }
	glm::mat4 view()        const { return glm::lookAt( pos_, pos_ + dir_, up_ ); }
	glm::mat4 projection( int width, int height ) const;

private:

	glm::vec3 pos_          = { 0.0f, 1.0f, 5.0f };
	glm::vec3 dir_          = { 0.0f, 0.0f, -1.0f };
	glm::vec3 up_           = { 0.0f, 1.0f, 0.0f };
	glm::vec3 right_        = { 1.0f, 0.0f, 0.0f };

	float vertical_fov_     = glm::radians(45.0f);
	float near_plane_       = 0.1f;
	float far_plane_        = 100.0f;
};

#endif // END TJH_CAMERA_H

////// IMPLEMENTATION //////////////////////////////////////////////////////////

#ifdef TJH_CAMERA_IMPLEMENTATION

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

TJH_CAMERA_TYPENAME::TJH_CAMERA_TYPENAME() {}
TJH_CAMERA_TYPENAME::TJH_CAMERA_TYPENAME( float x, float y, float z, float look_at_x, float look_at_y, float look_at_z )
{
    pos_ = { x, y, z };
    setLookAt( { look_at_x, look_at_y, look_at_z } );
}

void TJH_CAMERA_TYPENAME::rotateLeft( float radians )
{
    dir_ = glm::rotate( dir_, radians, up_ );
    dir_ = glm::normalize( dir_ );
    right_ = glm::normalize( glm::cross( dir_, up_ ) );
}

// NOTE: this method will not clamp to exactly vertical when rotating up or down
// TODO: find a way of making it clamp to vertical
//  - One solution would be to switch to using spherical coords?
void TJH_CAMERA_TYPENAME::rotateUp( float radians )
{
    glm::vec3 new_dir = glm::rotate( dir_, radians, right_ );
    glm::vec3 new_right = glm::cross( new_dir, up_ );

    // Only apply rotation if we aren't going to 'flip' upside down
    // Check for flipping by comparing
    if( new_right.x * right_.x >= 0.0f && new_right.y * right_.y >= 0.0f && new_right.z * right_.z >= 0.0f )
    {
        dir_ = glm::normalize( new_dir );
        right_ = glm::normalize( new_right );
    }
}

void TJH_CAMERA_TYPENAME::setUp( glm::vec3 up )
{
	up_ = glm::normalize( up );
    right_ = glm::normalize( glm::cross( dir_, up_ ) );
}

void TJH_CAMERA_TYPENAME::setDirection( glm::vec3 dir )
{
	dir_ = glm::normalize( dir );
    right_ = glm::normalize( glm::cross( dir_, up_ ) );
}

void TJH_CAMERA_TYPENAME::setLookAt( glm::vec3 look_at )
{
    dir_ = glm::normalize( (look_at - pos_) );
    right_ = glm::normalize( glm::cross( dir_, up_ ) );
}

glm::mat4 TJH_CAMERA_TYPENAME::projection( int width, int height ) const
{
    return glm::perspective( vertical_fov_, width / static_cast<float>(height), near_plane_, far_plane_ );
}

// Prevent the implementation from leaking into subsequent includes
#undef TJH_DRAW_IMPLEMENTATION
#endif // END TJH_CAMERA_IMPLEMENTATION
