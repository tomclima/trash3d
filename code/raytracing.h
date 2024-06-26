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
    MATRIX3D orientation;
};
EYE set_eye(float pos[3], VECTOR3D ori[3])
{
    EYE eye;

    for(int i = 0; i < 3; i++)
    {
        eye.position.values[i] = pos[i];
        eye.orientation.lines[i] = ori[i];
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
    float specularity;
    float reflexivity;
};

SURFACEINFO set_surfaceinfo(float color[3], float specularity)
{
    SURFACEINFO surfaceinfo;
    
    surfaceinfo.color = set_vector(color);
    surfaceinfo.specularity = specularity;
    
    return surfaceinfo;
};


struct SPHERE
{
    VECTOR3D center;
    SURFACEINFO surface_info;
    float radius;
};

SPHERE set_sphere(float center[3], float color[3], float radius, float specularity, float reflexivity)
{
    SPHERE sphere;

    sphere.center = set_vector(center);
    sphere.radius = radius;
    sphere.surface_info.color = set_vector(color);
    sphere.surface_info.specularity = specularity;
    sphere.surface_info.reflexivity = reflexivity;
    
    return sphere;
};



struct LIGHT
{
    VECTOR3D position;
    VECTOR3D orientation;
    float intensity;
    char type; // 'a' = ambient, 'p' = point, 'd' = directional
};


LIGHT set_point_light(float pos[3], float intensity)
{
    LIGHT point_light;

    point_light.position = set_vector(pos);
    point_light.intensity = intensity;
    point_light.type = 'p';

    return point_light;
};
LIGHT set_ambient_light(float intensity)
{
    LIGHT ambient_light;
    float null_pos[3] = {0, 0, 0}; //position is irrelevant for ambient
    
    ambient_light.position = set_vector(null_pos);
    ambient_light.intensity = intensity;
    ambient_light.type = 'a';

    return ambient_light;
};
LIGHT set_directional_light(float orientation[3], float intensity)
{
    LIGHT directional_light;

    directional_light.orientation = set_vector(orientation);
    directional_light.intensity = intensity;

    return directional_light;
}


struct SCENE 
{
	SPHERE spheres[4];
    LIGHT lights[5];
};

SCENE set_scene(SPHERE spheres[4], LIGHT lights[4])
{
    SCENE scene;

    for(int i = 0; i <4; i ++)
    {
        scene.spheres[i] = spheres[i];
    };
    
    for(int i = 0; i< 5; i++)
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
    YX_canvas.yx[1] = screen_x- (canvas->width/2); 

	return YX_canvas;
};


VECTOR3D canvas_to_viewport_conversion(CANVAS *canvas, VIEWPORT *viewport, EYE* eye, int canvas_x, int canvas_y)
{
	VECTOR3D XYZviewport;

	XYZviewport.values[0] = canvas_x*((float)(viewport->width)/(canvas->width)) + eye->position.values[0];
	XYZviewport.values[1] = canvas_y*((float)(viewport->height)/(canvas->height)) + eye->position.values[1];
    XYZviewport.values[2] = viewport->distance_to_eye + eye->position.values[2];
	
    return XYZviewport;
};



float ray_intersection_sphere(VECTOR3D, VECTOR3D, SPHERE);
HITINFO first_intersection_sphere(SPHERE spheres[3], VECTOR3D ray_vector, VECTOR3D origin, float min_intersection, float max_intersection)
{

    HITINFO ray_hit;

    float smallest_interseciton = max_intersection;
    int smallest_iter = -1;
    float current_intersection = smallest_interseciton;

    for(int i = 0; i < 4; i ++)
    {
        current_intersection = ray_intersection_sphere(ray_vector, origin, spheres[i]);
        if((current_intersection < smallest_interseciton)&(current_intersection > min_intersection))
        {
            smallest_interseciton = current_intersection;
            smallest_iter = i;

        };
    };


    if (smallest_iter == -1)
    {
        ray_hit.hit_happened = 0; //hitinfo nulo
    }
    else
    {
        VECTOR3D hit_position = vec_sum(origin, vec_MultbyScalar(ray_vector, smallest_interseciton));
        ray_hit.position = hit_position;
        ray_hit.normal_vec = vec_sum(hit_position, vec_MultbyScalar(spheres[smallest_iter].center, -1));
        ray_hit.surface_info = spheres[smallest_iter].surface_info;
        ray_hit.hit_happened = 1;
    };

    return ray_hit;
};


float ray_intersection_sphere(VECTOR3D ray_vector, VECTOR3D origin_position, SPHERE sphere)
{

    float t_result;

    // quadratic form at^2 + bt + c = 0
    float a = 0;
    float b = 0;
    float c = 0;

    for(int i = 0; i < 3; i++)
    {
        a += vec_DotProduct(ray_vector, ray_vector);
        b += 2*vec_DotProduct(ray_vector, origin_position) - 2*vec_DotProduct(ray_vector, sphere.center);
        c += vec_DotProduct(origin_position, origin_position) + vec_DotProduct(sphere.center, sphere.center) - 2*vec_DotProduct(sphere.center, origin_position);
    };

    c -= sphere.radius*sphere.radius;

    
    float results[2] = { 0, 0 };

    // delta verification
    if (c < 0.0001) { c = 0;};
    
    float delta = b*b - 4*a*c;

    
    if(delta > 0)
    {
        results[0] = (-b + sqrtf(delta))/(2*a);
        results[1] = (-b - sqrtf(delta))/(2*a);   
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

    t_result = sorted_results[0];

    if(t_result <= 0)
    {
        t_result = sorted_results[1];
    };

    
    return t_result; 
}; 


float compute_non_ambient_light(LIGHT light, HITINFO ray_hit, VECTOR3D eye_position, SPHERE spheres[4]);
float calculate_total_light(SCENE* scene, VECTOR3D eye_position, HITINFO ray_hit)
{
    float light_intensity = 0;

    for(int i = 0; i < 2; i++)
    {
        switch (scene->lights[i].type)
        {
        case 'a':
            light_intensity += scene->lights[i].intensity;
            break;
        
        default:
            light_intensity += compute_non_ambient_light(scene->lights[i], ray_hit, eye_position, scene->spheres);
            break;
        };
    };

    if(light_intensity > 1)
    {
        light_intensity = 1;
    };
    return light_intensity;
};

float compute_non_ambient_light(LIGHT light, HITINFO ray_hit, VECTOR3D eye_position, SPHERE spheres[4])
{
    float diffuse_light_intensity = 0;
    float specular_light_intensity = 0;
    float combined_light_intensity;
    float max_blocked;

    VECTOR3D surface_to_light;
    VECTOR3D reflection_vector;
    VECTOR3D vision_ray_vector;

    switch (light.type)
    {
    case 'p': // if light is point light
        {
            surface_to_light = vec_sum(light.position, vec_MultbyScalar(ray_hit.position, -1));
            max_blocked = 1;
        }break;
    
    default: // if light is directional light
        {
            surface_to_light = light.orientation;
            max_blocked = INFINITY;
        }
    };

    // calculate shadows
    bool unblocked = !first_intersection_sphere(spheres, surface_to_light, ray_hit.position, 0.0001, max_blocked).hit_happened;




    // diffuse light
    if(unblocked)
    {
        float diffuse_cosine = vec_cosine(ray_hit.normal_vec, surface_to_light);
        if(diffuse_cosine < 0) {diffuse_cosine = 0;};
        diffuse_light_intensity = (light.intensity)*diffuse_cosine;
    }

    if((ray_hit.surface_info.specularity > 0)&(unblocked)) //specular light
    {
        reflection_vector = reflect(surface_to_light, ray_hit.normal_vec);
        vision_ray_vector = vec_sum(vec_MultbyScalar(ray_hit.position, -1), eye_position);

        float specular_cosine = vec_cosine(vision_ray_vector, reflection_vector);
        if(specular_cosine < 0) 
        {
            specular_cosine = 0;
        };
        specular_light_intensity = (light.intensity)*pow(specular_cosine, ray_hit.surface_info.specularity);
    };

    combined_light_intensity = diffuse_light_intensity + specular_light_intensity;
    return combined_light_intensity;
};


VECTOR3D trace_ray(VECTOR3D pixel_ray_vector, VECTOR3D eye_position, VECTOR3D ray_origin, SCENE scene, uint8 max_reflections, float min_intersection)
{
    VECTOR3D color;
    float black_rgb[3] = {0,0,0};
    color = set_vector(black_rgb);

    HITINFO intersection_info = first_intersection_sphere(scene.spheres, pixel_ray_vector, ray_origin, min_intersection, INFINITY);

    if(intersection_info.hit_happened)
    {
        float light_intensity = calculate_total_light(&scene, eye_position, intersection_info);
        color = vec_MultbyScalar(intersection_info.surface_info.color, light_intensity*(1 - intersection_info.surface_info.reflexivity));
        if(intersection_info.surface_info.reflexivity > 0)
        {
            VECTOR3D reflection = trace_ray(reflect(vec_MultbyScalar(pixel_ray_vector,-1), intersection_info.normal_vec), eye_position, intersection_info.position,scene, max_reflections - 1, 0.00001);
            color = vec_sum(color, vec_MultbyScalar(reflection, intersection_info.surface_info.reflexivity));
        }
    }

    return color;
};

VECTOR3D simulate_pixel_color(SCENE scene, EYE eye, CANVAS canvas, VIEWPORT viewport, int pixelX, int pixelY, uint8 max_reflections)
{
    VECTOR3D pixelcolor;

    YXpixelCoord YX_canvas = screen_to_canvas(&canvas, pixelY, pixelX);
    int canvasY = YX_canvas.yx[0];
    int canvasX = YX_canvas.yx[1];

    VECTOR3D XYZviewport = canvas_to_viewport_conversion(&canvas, &viewport, &eye, canvasX, canvasY);
    XYZviewport = vec_linear_transform(eye.orientation, XYZviewport);

    VECTOR3D pixel_ray_vector = vec_sum(XYZviewport, vec_MultbyScalar(eye.position, -1));
    
    pixelcolor = trace_ray(pixel_ray_vector, eye.position, eye.position, scene, max_reflections, 1);


    return pixelcolor;
};