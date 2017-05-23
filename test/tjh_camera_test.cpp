#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#define TJH_CAMERA_IMPLEMENTATION
#include "../tjh_camera.h"

TEST_CASE( "can instansiate camera", "[camera]" )
{
    Camera camera;
    glm::vec3 pos = camera.position();
    pos += glm::vec3( 0.0f, 1.0f, 0.0f );

    camera.moveUp( 1.0f );

    REQUIRE( camera.position() == pos );
}
