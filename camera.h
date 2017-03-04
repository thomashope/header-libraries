#ifndef TJH_CAMERA_H
#define TJH_CAMERA_H

// TODO: remove set right
//	- instead calculate right when set dir or set up
// TODO: is it worth trying to remove the dependancy on GLM?

////// README //////////////////////////////////////////////////////////////////
//
// TODO: write the readme...

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