#include <stdint.h>
#include <math.h>
#include "vectors.h"
#include <Windows.h>

#define internal static
//#include "geometry.h"

typedef uint8_t 	uint8;
typedef uint16_t 	uint16;
typedef uint32_t 	uint32;
typedef uint64_t 	uint64;

typedef int8_t 		int8;
typedef int16_t 	int16;
typedef int32_t 	int32;
typedef int64_t 	int64;


struct YXpixelCoord
{
	int yx[2];
};


struct EYE
{
	VECTOR3D position;
    VECTOR3D orientation;
};
EYE set_eye(float pos[3], float ori[3])
{
    EYE eye;

    for(int i = 0; i < 3; i++)
    {
        eye.position.values[i] = pos[i];
        eye.orientation.values[i] = ori[i];
    }
    return eye;
};


struct VIEWPORT
{
	int distance_to_eye;
	float width;
	float height;
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


struct SURFACEINFO
{
    VECTOR3D color;
    bool is_especular;
};

SURFACEINFO set_surfaceinfo(float color[3], bool is_especular)
{
    SURFACEINFO surfaceinfo;
    
    surfaceinfo.color = set_vector(color);
    surfaceinfo.is_especular = is_especular;
    
    return surfaceinfo;
};


struct SPHERE
{
    VECTOR3D center;
    SURFACEINFO surface_info;
    float radius;
};

SPHERE set_sphere(float center[3], float color[3], float radius, bool is_especular)
{
    SPHERE sphere;

    sphere.center = set_vector(center);
    sphere.radius = radius;
    sphere.surface_info.color = set_vector(color);
    sphere.surface_info.is_especular = is_especular;
    
    return sphere;
};



struct LIGHT
{
    VECTOR3D position;
    float intensity;
    bool is_ambient;
};


LIGHT set_point_light(float pos[3], float intensity)
{
    LIGHT point_light;

    point_light.position = set_vector(pos);
    point_light.intensity = intensity;
    point_light.is_ambient = 0;

    return point_light;
};
LIGHT set_ambient_light(float intensity)
{
    LIGHT ambient_light;
    float null_pos[3] = {0, 0, 0}; //position is irrelevant for ambient
    
    ambient_light.position = set_vector(null_pos);
    ambient_light.intensity = intensity;
    ambient_light.is_ambient = 1;

    return ambient_light;
};


struct SCENE 
{
	SPHERE spheres[4];
    LIGHT lights[2];
};

SCENE set_scene(SPHERE spheres[4], LIGHT lights[2])
{
    SCENE scene;

    for(int i = 0; i <4; i ++)
    {
        scene.spheres[i] = spheres[i];
    };
    
    for(int i = 0; i< 2; i++)
    {
        scene.lights[i] = lights[i];
    };

    return scene;
};


struct HITINFO 
{
    VECTOR3D position;
    VECTOR3D normal_vec;
    SURFACEINFO surface_info;
    bool hit_happened;
};


YXpixelCoord screen_to_canvas(CANVAS *canvas, int screen_y, int screen_x)
{
    YXpixelCoord YX_canvas;

    YX_canvas.yx[0] = (canvas->height/2) - screen_y;
    YX_canvas.yx[1] = screen_x - (canvas->width/2); 

	return YX_canvas;
};

VECTOR3D canvas_to_viewport_conversion(CANVAS *canvas, VIEWPORT *viewport, int canvas_x, int canvas_y)
{
	VECTOR3D XYZviewport;

	XYZviewport.values[0] = canvas_x*((float)(viewport->width)/(canvas->width));
	XYZviewport.values[1] = canvas_y*((float)(viewport->height)/(canvas->height));
    XYZviewport.values[2] = viewport->distance_to_eye;
	
    // TODO (tom): generalizar para comportar multiplas posições diferentes, não só a em relação ao olho no (0,0,0)
    return XYZviewport;
};

float ray_intersection_sphere(VECTOR3D ray_vector, VECTOR3D eye_position, SPHERE sphere)
{

    // quadratic form at^2 + bt + c = 0
    float a = 0;
    float b = 0;
    float c = 0;

    for(int i = 0; i < 3; i++)
    {
        a += vec_DotProduct(ray_vector, ray_vector);
        b += 2*vec_DotProduct(ray_vector, eye_position) - 2*vec_DotProduct(ray_vector, sphere.center);
        c += vec_DotProduct(eye_position, eye_position) + vec_DotProduct(sphere.center, sphere.center) - 2*vec_DotProduct(sphere.center, eye_position);
    };

    c -= sphere.radius*sphere.radius;

    
    float results[2] = { 0, 0 };

    // delta verification
    float delta = b*b - 4*a*c;

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

float compute_point_light(LIGHT point_light, HITINFO ray_hit)
{
    float light_intensity;
    VECTOR3D surface_to_light = vec_sum(point_light.position, vec_MultbyScalar(ray_hit.position, -1));

    light_intensity = (point_light.intensity)*vec_cosine(surface_to_light, ray_hit.normal_vec);    

    if(light_intensity < 0)
    {
        light_intensity = 0;
    }

    return light_intensity;
};

float calculate_total_light(SCENE scene, HITINFO ray_hit)
{
    float light_intensity = 0;

    for(int i = 0; i < 2; i++)
    {
        if(scene.lights[i].is_ambient)
        {
            light_intensity += scene.lights[i].intensity;
        }
        
        else
        {
            light_intensity += compute_point_light(scene.lights[i], ray_hit);
        };
    };

    if(light_intensity > 1)
    {
        light_intensity = 1;
    };
    return light_intensity;
};

HITINFO first_intersection_sphere(SCENE *scene, VECTOR3D ray_vector, VECTOR3D eye_position)
{

    HITINFO ray_hit;

    float smallest_interseciton = INFINITY;
    int smallest_iter = -1;
    float current_intersection = smallest_interseciton;

    for(int i = 0; i < 4; i ++)
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
            ray_hit.hit_happened = 0; //hit info nulo
        };
    }
    else
    {
        VECTOR3D hit_position = vec_sum(eye_position, vec_MultbyScalar(ray_vector, smallest_interseciton));
        ray_hit.normal_vec = vec_sum(scene->spheres[smallest_iter].center, vec_MultbyScalar(hit_position, -1));
        ray_hit.surface_info = scene->spheres[smallest_iter].surface_info;
        ray_hit.hit_happened = 1;
    };

    return ray_hit;
};

VECTOR3D SimulatePixelColor(SCENE scene, EYE eye, CANVAS canvas, VIEWPORT viewport, int pixelX, int pixelY)
{
    VECTOR3D pixelcolor;

    YXpixelCoord YX_canvas = screen_to_canvas(&canvas, pixelY, pixelX);

    
    int canvasY = YX_canvas.yx[0];
    int canvasX = YX_canvas.yx[1];

    VECTOR3D XYZviewport = canvas_to_viewport_conversion(&canvas, &viewport, canvasX, canvasY);
    
    VECTOR3D ray_vector = vec_sum(XYZviewport, vec_MultbyScalar(eye.position, -1));

    HITINFO intersection_info = first_intersection_sphere(&scene, ray_vector, eye.position);


    
    if(intersection_info.hit_happened)
    {
        float light_intensity = calculate_total_light(scene, intersection_info);
        pixelcolor = vec_MultbyScalar(intersection_info.surface_info.color, light_intensity);
    }
    else
    {
        float black_rgb[3] = {0,0,0};
        pixelcolor = set_vector(black_rgb);
        
    };

    return pixelcolor;
};

