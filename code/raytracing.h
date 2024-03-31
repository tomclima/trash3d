#include <stdint.h>
//#include "geometry.h"

typedef uint8_t 	uint8;
typedef uint16_t 	uint16;
typedef uint32_t 	uint32;
typedef uint64_t 	uint64;

typedef int8_t 		int8;
typedef int16_t 	int16;
typedef int32_t 	int32;
typedef int64_t 	int64;


struct Eye
{
	Vector3D position;
    Vector3D orientation;
};

struct Viewport
{
	int distance_to_eye;
	int width;
	int height;
};

struct Canvas
{
	int width;
	int height;
};


struct Vector3D
{
	int values[2];
};

struct YXCoord
{
	int Ycoord;
	int Xcoord;
};

struct Sphere
{
    Vector3D center;
    Vector3D Color;
    int radius;
};

struct Scene 
{
	Sphere spheres[3];
};

Vector3D vec_sum(Vector3D vec1, Vector3D vec2)
{
    Vector3D resulting_vec;
    int result;

    for(uint8 i = 0; i < 3; i++)
    {
        result = vec1.values[i] + vec2.values[i];
        resulting_vec.values[i] = result;
    };

    return resulting_vec;
};

Vector3D vec_MultbyScalar(Vector3D vec1, int scalar)
{
    Vector3D resulting_vec;
    int result;

    for(uint8 i = 0; i < 3; i++)
    {
        result = vec1.values[i] * scalar;
        resulting_vec.values[i] = result;
    };

    return resulting_vec;
};

int vec_DotProduct(Vector3D vec1, Vector3D vec2)
{
    int result = 0;

    for(uint8 i = 0; i < 3; i++)
    {
        result += vec1.values[i] * vec2.values[i];
    };

    return result;
};

YXCoord canvas_to_screen_conversion(Canvas *canvas, char canvas_x, int canvas_y)
{
    YXCoord YX_screen;

	YX_screen.Ycoord = (canvas->height/2) - canvas_y;
	YX_screen.Xcoord = (canvas->width/2) + canvas_x;

	return YX_screen;
};

Vector3D canvas_to_viewport_conversion(Canvas *canvas, Viewport *viewport, int canvas_x, int canvas_y)
{
	Vector3D XYZviewport;

	XYZviewport.values[0] = canvas_x*((viewport->width)/(canvas->width));
	XYZviewport.values[1] = canvas_y*((viewport->height)/(canvas->height));
    XYZviewport.values[2] = viewport->distance_to_eye;
	
    return XYZviewport;
};


bool Ray_intersection_sphere(Vector3D ray_vector, Sphere sphere)
{
    bool intersects = false;

    
    return intersects
}

Vector3D SimulatePixelColor(Scene scene, Eye eye, Canvas canvas, Viewport viewport, Sphere sphere, int pixelX, int pixelY)
{
    Vector3D XYZviewport = canvas_to_viewport_conversion(&canvas, &viewport, pixelX, pixelY);
    
    Vector3D ray_vector = vec_sum(XYZviewport, vec_MultbyScalar(eye.position, -1));

}

