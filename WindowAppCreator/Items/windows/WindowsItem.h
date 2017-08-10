#pragma once
#ifdef _WIN32
#include "../Item.h"
#include <Windows.h>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library

namespace Win32
{
	enum class WindowType
	{
		NORMAL,
		OPENGL
	};

	struct Win32_Item : public virtual Item
	{
		~Win32_Item()
		{
			if (hRC)											// Do We Have A Rendering Context?
			{
				if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
				{
					MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
				}

				if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
				{
					MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
				}
				hRC = NULL;										// Set RC To NULL
			}

			if (hDC && !ReleaseDC(hwnd, hDC))					// Are We Able To Release The DC
			{
				MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
				hDC = NULL;										// Set DC To NULL
			}

			if (hwnd && !DestroyWindow(hwnd))					// Are We Able To Destroy The Window?
			{
				MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
				hwnd = NULL;										// Set hWnd To NULL
			}

			if (!UnregisterClass(className.c_str(), instance))			// Are We Able To Unregister Class
			{
				MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
				instance = NULL;									// Set hInstance To NULL
			}
		}
		
		WindowType windowType = WindowType::NORMAL;

		HWND hwnd;

		HDC			hDC = NULL;		// Private GDI Device Context
		HGLRC		hRC = NULL;		// Permanent Rendering Context

		DWORD dwExStyle = WS_EX_CLIENTEDGE;
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		HWND hWndParent = NULL;
		HMENU hMenu = NULL;

		LPVOID lpParam = NULL;
		std::string className = "default class name";
		HINSTANCE instance;

		bool CreateItem()
		{
			RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
			WindowRect.left = (long) 0;			// Set Left Value To 0
			WindowRect.right = (long) rect.width;		// Set Right Value To Requested Width
			WindowRect.top = (long) 0;				// Set Top Value To 0
			WindowRect.bottom = (long) rect.height;		// Set Bottom Value To Requested Height

			AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

			hwnd = CreateWindowEx(dwExStyle, className.c_str(), title.c_str(), dwStyle,
				rect.x, rect.y, rect.width, rect.height, hWndParent, hMenu, instance, lpParam);

			if (hwnd == NULL)
			{
				MessageBox(NULL, (name + "::Error creatind windows item.").c_str(), "Error", MB_ICONEXCLAMATION);
				return false;
			}
			
			hDC = GetDC(hwnd);

			if (windowType == WindowType::OPENGL)
				SetOpenGLWindow();
			else
			{
				HFONT hNormalFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
				SendMessage(hwnd, WM_SETFONT, (WPARAM) hNormalFont, 0);
			}

			return true;
		}

		ItemMessage Update()
		{
			if (windowType == WindowType::OPENGL)
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
				glLoadIdentity();
			}// Reset The Current Modelview Matrix
			return{};
		}

		void SetOpenGLWindow()
		{
			unsigned int PixelFormat;			// Holds The Results After Searching For A Match

			static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
			{
				sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
				1,											// Version Number
				PFD_DRAW_TO_WINDOW |						// Format Must Support Window
				PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
				PFD_DOUBLEBUFFER,							// Must Support Double Buffering
				PFD_TYPE_RGBA,								// Request An RGBA Format
				24,										    // Select Our Color Depth
				0, 0, 0, 0, 0, 0,							// Color Bits Ignored
				0,											// No Alpha Buffer
				0,											// Shift Bit Ignored
				0,											// No Accumulation Buffer
				0, 0, 0, 0,									// Accumulation Bits Ignored
				16,											// 16Bit Z-Buffer (Depth Buffer)  
				0,											// No Stencil Buffer
				0,											// No Auxiliary Buffer
				PFD_MAIN_PLANE,								// Main Drawing Layer
				0,											// Reserved
				0, 0, 0										// Layer Masks Ignored
			};

			if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
			{
				MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);						// Return FALSE
			}

			if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
			{						
				MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);							// Return FALSE
			}

			if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
			{
				MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);								// Return FALSE
			}

			if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
			{
				MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);						// Return FALSE
			}

			ShowWindow(hwnd, SW_SHOW);

			glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
			glClearColor(0.0f, 1.0f, 0.0f, 1.0f);				// Black Background
			glClearDepth(1.0f);									// Depth Buffer Setup
			glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
			glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

			glViewport(0, 0, rect.width, rect.height);						// Reset The Current Viewport

			glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glLoadIdentity();									// Reset The Projection Matrix

																// Calculate The Aspect Ratio Of The Window
			//gluPerspective(45.0f, (GLfloat) rect.width / (GLfloat) rect.height, 0.1f, 100.0f);

			glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
			glLoadIdentity();
		}
	};
}
#endif

//virtual void Init() = 0;
//virtual ~Win32_Item() {}
//
//HWND hwnd;
//
//DWORD dwExStyle = WS_EX_CLIENTEDGE;
//LPCSTR lpClassName;
//LPCSTR lpWindowName = "";
//DWORD dwStyle = WS_OVERLAPPEDWINDOW;
//int X = CW_USEDEFAULT;
//int Y = CW_USEDEFAULT;
//int nWidth = 320;
//int nHeight = 240;
//HWND hWndParent = NULL;
//HMENU hMenu = NULL;
//HINSTANCE hInstance = NULL;
//LPVOID lpParam = NULL;
//
//bool CreateItem()
//{
//	hwnd = CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle,
//		X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
//
//	if (hwnd == NULL)
//	{
//		MessageBox(NULL, (lpWindowName + "::Error creatind windows item.").c_str(), "Error", MB_ICONEXCLAMATION);
//		return false;
//	}
//	return true;
//}