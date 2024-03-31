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

global BITMAPINFO  BitmapInfo; // RGB information for each pixel
global void *BitmapMemory; // pointer to bottom of section where the bitmap is stored 
global int BitmapWidth;
global int BitmapHeight;
global int BytesPerPixel;

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
	//defining message loop
	bool running = true;
	while(running)
	{
		BOOL evaluation = GetMessage(&Msg, hMainWindow, 0, 0);
		if (evaluation > 0)
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else
		{
			running = false;
		}
	}
}


// Pre declaration
internal void Win32ResizeDIBSection(int width, int height); 

internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int topleftX, int topleftY, int width, int height);

internal void render_weird_gradient()
{
	int Pitch = BytesPerPixel*BitmapWidth;
	uint8 *Row = (uint8 *)BitmapMemory;
	for(int Y = 0; Y < BitmapHeight; Y++)
	{
		uint32 *Pixel = (uint32 *)Row;
		for(int X = 0; X < BitmapWidth; X++)
		{
			uint8 *PixelColor = (uint8 *)Pixel;
			*PixelColor = (uint8)Y; //Blue
			++PixelColor;

			*PixelColor = (uint8)X; // Green
			++PixelColor;

			*PixelColor = 0; // red
			++PixelColor;

			*PixelColor = 0; //pading
			++PixelColor;
			++Pixel;
		}
		Row += Pitch;
	}	
}
// End pre declaration

LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_ACTIVATEAPP: {
		OutputDebugStringA("muerte");
	}break;
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

	render_weird_gradient();
}


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
