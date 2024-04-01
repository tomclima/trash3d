#include <stdint.h>
#include <math.h>
#include "vectors.h"
#include <Windows.h>


//#include "geometry.h"

typedef uint8_t 	uint8;
typedef uint16_t 	uint16;
typedef uint32_t 	uint32;
typedef uint64_t 	uint64;

typedef int8_t 		int8;
typedef int16_t 	int16;
typedef int32_t 	int32;
typedef int64_t 	int64;


struct EYE
{
	VECTOR3D position;
};

struct VIEWPORT
{
	int distance_to_eye;
	int width;
	int height;
};

struct CANVAS
{
	int width;
	int height;
};

CANVAS set_canvas(int width, int height)
{
    CANVAS canvas;
    canvas.width = width;
    canvas.height = height;
    return canvas;
}

struct YXpixelCoord
{
	int yx[2];
};

struct SPHERE
{
    VECTOR3D center;
    VECTOR3D color;
    float radius;
};

SPHERE set_sphere(float center[3], float color[3], float radius)
{
    SPHERE sphere;


    for(int i = 0; i < 3; i++)
    {
        sphere.color.values[i] = color[i];
        sphere.center.values[i] = center[i];
    }
    sphere.radius = radius;
    
    return sphere;
};

struct SCENE 
{
	SPHERE spheres[3];
};


SCENE set_scene(SPHERE spheres[3])
{
    SCENE scene;

    for(int i = 0; i < 3; i ++)
    {
        scene.spheres[i] = spheres[i];
    };
    return scene;
};


YXpixelCoord screen_to_canvas(CANVAS *canvas, int screen_y, int screen_x)
{
    YXpixelCoord YX_canvas;

	// YX_screen.Ycoord = (canvas->height/2) - canvas_y;
    YX_canvas.yx[0] = (canvas->height/2) - screen_y;
	//YX_screen.Xcoord = (canvas->width/2) + canvas_x;
    YX_canvas.yx[1] = screen_x - (canvas->width/2); 

	return YX_canvas;
};

VECTOR3D canvas_to_viewport_conversion(CANVAS *canvas, VIEWPORT *viewport, int canvas_x, int canvas_y)
{
	VECTOR3D XYZviewport;

	XYZviewport.values[0] = canvas_x*((float)(viewport->width)/(canvas->width));
	XYZviewport.values[1] = canvas_y*((float)(viewport->height)/(canvas->height));
    XYZviewport.values[2] = viewport->distance_to_eye;
	
    return XYZviewport;
};

float ray_intersection_sphere(VECTOR3D ray_vector, VECTOR3D eye_position, SPHERE sphere)
{

    bool ray_intersects = false;

    // quadratic form at^2 + bt + c = 0
    float a = 0;
    float b = 0;
    float c = 0;

    for(int i = 0; i < 3; i++)
    {
        a += pow(ray_vector.values[i], 2);
        b += 2*(eye_position.values[i]*ray_vector.values[i]) - 2*(ray_vector.values[i]*sphere.center.values[i]);
        c += pow(eye_position.values[i], 2) + pow(sphere.center.values[i], 2) - 2*(eye_position.values[i]*sphere.center.values[i]);
    };

    c -= pow(sphere.radius, 2);

    
    float results[2];

    // delta verification
    float delta = pow(b, 2) - 4*a*c;

    if(delta > 0)
    {
        results[0] = (float)(-b + sqrt(delta))/(2*a);
        results[1] = (float)(-b - sqrt(delta)/2*a);   
    };

    float sorted_results[2];

    if(results[0] > results[1])
    {
        sorted_results[0] = results[1];
        sorted_results[1] = results[0]; 
    }
    else
    {
        sorted_results[0] = results[0];
        sorted_results[1] = results[1];
    };

    float t_result = sorted_results[0];

    if(t_result <= 0)
    {
        t_result = sorted_results[1];
    };

    float t_valid = 0;

    if(t_result > 1)
    {
        t_valid = t_result;
    };

    return t_valid; //only t > 1 intersections are valid, all others default to 0 as an error
}; 

VECTOR3D first_intersection_rgb(SCENE *scene, VECTOR3D ray_vector, VECTOR3D eye_position)
{
    VECTOR3D rgb;
    float smallest_interseciton = INFINITY;
    int smallest_iter = -1;
    float current_intersection = smallest_interseciton;

    for(int i = 0; i <= 2; i ++)
    {
        current_intersection = ray_intersection_sphere(ray_vector, eye_position, scene->spheres[i]);
        if((current_intersection < smallest_interseciton)&(current_intersection > 1))
        {
            smallest_interseciton = current_intersection;
            smallest_iter = i;
        };
    };

    if (smallest_iter == -1)
    {
        for(int i = 0; i <= 2; i++)
        {
            rgb.values[i] = 255; //rgb do background escuro
        };
    }
    else
    {
        for(int i = 0; i <= 2; i++)
        {
            rgb.values[i] = scene->spheres[smallest_iter].color.values[i]; 
        };
    };
    return rgb;
};

VECTOR3D SimulatePixelColor(SCENE scene, EYE eye, CANVAS canvas, VIEWPORT viewport, int pixelX, int pixelY)
{
    VECTOR3D pixelcolor;

    YXpixelCoord YX_canvas = screen_to_canvas(&canvas, pixelY, pixelX);

    
    int canvasY = YX_canvas.yx[0];
    int canvasX = YX_canvas.yx[1];

    VECTOR3D XYZviewport = canvas_to_viewport_conversion(&canvas, &viewport, canvasX, canvasY);
    
    VECTOR3D ray_vector = vec_sum(XYZviewport, vec_MultbyScalar(eye.position, -1));

    pixelcolor = first_intersection_rgb(&scene, ray_vector, eye.position);
    
    return pixelcolor;
};

