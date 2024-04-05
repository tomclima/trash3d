#include <windows.h>
#include <stdint.h>
#include "raytracing.h"

#define internal static
#define global static

typedef uint8_t 	uint8;
typedef uint16_t 	uint16;
typedef uint32_t 	uint32;
typedef uint64_t 	uint64;

typedef int8_t 		int8;
typedef int16_t 	int16;
typedef int32_t 	int32;
typedef int64_t 	int64;

global bool running;
global double pi = 3.14159265;

global BITMAPINFO  BitmapInfo; // RGB information for each pixel
global void *BitmapMemory; // pointer to bottom of section where the bitmap is stored 
global int BitmapWidth;
global int BitmapHeight;
global int BytesPerPixel;

global float center1[3] = {0, 1, 2};
global float center2[3] = {2, -1.5, 4};
global float center3[3] = {0.25,0,2};//{-2, 0.3, 2};
global float center4[3] = {0, 0, 3};

global float color1[3] = {255, 0, 0};
global float color2[3] = {0, 0, 255};
global float color3[3] = {0, 255, 0};
global float color4[3] = {255, 255 , 0};

global float radius1 = 0.5;
global float radius2 = 1;
global float radius3 = 0.25;
global float radius4 = 1;


internal void render_spheres()
{
	EYE eye;
	VIEWPORT viewport;
	CANVAS canvas = set_canvas(BitmapWidth, BitmapHeight);

	SPHERE sphere1 = set_sphere(center1, color1, radius1, 1000, 0);
	SPHERE sphere2 = set_sphere(center2, color2, radius2, 500, 0);
	SPHERE sphere3 = set_sphere(center3, color3, radius3, 500, 0);
	SPHERE sphere4 = set_sphere(center4, color4, radius4, 500, 0);	

	SPHERE spheres[] = {sphere1, sphere2, sphere3, sphere4};

	LIGHT ambient_light = set_ambient_light(0.1);
	float point_light_pos[3] = {1,0,1};
	LIGHT point_light = set_point_light(point_light_pos, 0.6);
	float direction_ligth_dir[3] = {0, 0, 2};
	LIGHT direction_ligth = set_directional_light(direction_ligth_dir, 0.5);

	LIGHT lights[] = {ambient_light, point_light, direction_ligth};
	SCENE scene = set_scene(spheres, lights);	

	float eye_position[3] = {0,0,-4};
	//float eye_position[3] = {-2.5, 0, -2};
	//float eye_position[3] = {2.5, 0, -2};
	
	eye.position = set_vector(eye_position);

	eye.orientation = get_identity_matrix();
	//eye.orientation = rotate_y(-pi/19);
	//eye.orientation = rotate_y(pi/13);
	
	canvas.height = BitmapHeight;
	canvas.width = BitmapWidth;
	viewport.height = (float)canvas.height/canvas.width;
	viewport.width = 1;//(float)canvas.width/canvas.height;
	viewport.distance_to_eye = 1;


	int Pitch = BytesPerPixel*BitmapWidth;
	uint8 *Row = (uint8 *)BitmapMemory;
	for(int Y = 0; Y < BitmapHeight; Y++)
	{
		uint32 *Pixel = (uint32 *)Row;
		for(int X = 0; X < BitmapWidth; X++)
		{

			VECTOR3D rgb = simulate_pixel_color(scene, eye, canvas, viewport, X, Y, 0);

			uint8 *PixelColor = (uint8 *)Pixel;
			*PixelColor = (uint8)rgb.values[2]; //Blue
			++PixelColor;

			*PixelColor = (uint8)rgb.values[1]; // Green
			++PixelColor;

			*PixelColor = (uint8)rgb.values[0]; // red
			++PixelColor;

			*PixelColor = 0; //pading
			++PixelColor;
			++Pixel;
		}
		Row += Pitch;
	}	
}


//pre declaration
LRESULT WINAPI WindowProc(HWND, UINT, WPARAM, LPARAM);
// end pre declaration

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX 	WindowClass;
	HWND 		hMainWindow;
	MSG 		Msg;

	// Defining the attributes of the Window factory
	WindowClass = {};
	WindowClass.cbSize			= sizeof(WNDCLASSEX),
	WindowClass.style 			= CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc 	= WindowProc;
	WindowClass.hInstance 		= hInstance;
	WindowClass.lpszClassName	= "trash3dWindowClass";
	
	//creating the window in which our program runs
	RegisterClassEx(&WindowClass);
	hMainWindow = CreateWindowEx(
		0,
		WindowClass.lpszClassName,
		"trash3d",
		WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,	
		CW_USEDEFAULT,
		0,
		0,
		hInstance,
		0
	);
	
	//main message loop
	running = true;
	while(running)
	{
		while(PeekMessage(&Msg, hMainWindow, 0, 0, PM_REMOVE))
		{
			if(Msg.message == WM_QUIT)
			{
				running = false;
			};
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		

		render_spheres();
		
		UpdateWindow(hMainWindow);
		
	}
}


// Pre declaration
internal void Win32ResizeDIBSection(int width, int height); 

internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int topleftX, int topleftY, int width, int height);

// End pre declaration

LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_SIZE: {
		RECT ClientRect;
		GetClientRect(hWnd, &ClientRect);
		int width = ClientRect.right - ClientRect.left;
		int height = ClientRect.bottom - ClientRect.top;
		Win32ResizeDIBSection(width, height);
	}break;
	case WM_PAINT:{
		PAINTSTRUCT Paint;
		HDC 		PaintDeviceContext;

		PaintDeviceContext = BeginPaint(hWnd, &Paint);
		int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int width = Paint.rcPaint.left - Paint.rcPaint.right;
		int height = Paint.rcPaint.top - Paint.rcPaint.bottom;
		Win32UpdateWindow(PaintDeviceContext, &Paint.rcPaint, X, Y, width, height);
		
		EndPaint(hWnd, &Paint);
	}break;
	case WM_CLOSE:
	{
		PostQuitMessage(0);
	}break;
	case WM_QUIT:
	{
		running = false;
	}break;
	default:
	{
		result = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}break;
	}
	return result;
}


internal void Win32ResizeDIBSection(int width, int height)
{	
	if(BitmapMemory)
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}

	BitmapWidth = width;
	BitmapHeight = height;

	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32; //24 bits for rgb 8R, 8G 8B + 8 padding bits for 4 byte alignment
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	// memory allocation
	BytesPerPixel = 4; 
	int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel; // how much we need to reserve for our bitmap
	BitmapMemory = VirtualAlloc(0,BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE); //alocando a memoria

	render_spheres();
};


internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int topleftX, int topleftY, int width, int height)
{
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top; 
	StretchDIBits(DeviceContext,
	0, 0, BitmapWidth, BitmapHeight,
	0, 0, WindowWidth, WindowHeight,
  	BitmapMemory,
  	&BitmapInfo,
  	DIB_RGB_COLORS,
	SRCCOPY
  );

};
