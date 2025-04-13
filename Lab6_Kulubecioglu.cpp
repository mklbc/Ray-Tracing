

#include "framework.h"
#include "resource.h"
#include "raytrace.h"
#include "scene.h"

#define MAX_LOADSTRING 100


HINSTANCE hInst;                              
WCHAR szTitle[MAX_LOADSTRING];               
WCHAR szWindowClass[MAX_LOADSTRING];          

void DrawStudyExample(HWND hWnd);
void DrawStudyExample2(HWND hWnd);
void DrawStudyExample3(HWND hWnd);
void Rendering(HWND hWnd);

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB6RAYS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB6RAYS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB6RAYS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB6RAYS);


    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
     
            switch (wmId)
            {
            case ID_RENDERING_PYRAMIDREFLECTION:
                DrawStudyExample(hWnd);
                break;
            case ID_RENDERING_SPHEREONTHETABLE:
                DrawStudyExample2(hWnd);
                break;
            case ID_RENDERING_TRANSPARENCY:
                DrawStudyExample3(hWnd);
                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
           
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void DrawStudyExample(HWND hWnd) //burasý piramitli olanýn yani 1. ekranýn
{

    if (OpenArrays_SCE(100, 3000, 12000, 10) != 1) return;

    SetCameraPosition_SCE(250, 500, 200, 12, 80, 400);
    AddLightSource_SCE(1, 1, 1, -150, 250, 350);       


    SetColor_SCE(0.7, 0.9, 0.7);  // pastel yeþil
    // zemin
    SetMaterial_SCE(0.5, 0.45, 0.05, 0, 20, 0);     
    AddQuad_SCE(-700, 300, 0, 700, 300, 0, 700, -500, 0, -700, -500, 0);

 
    SetColor_SCE(0.6, 0.6, 0.9);  // lavanta tonu
    // ayna
    SetMaterial_SCE(0.03, 0.01, 0.9, 0.01, 20, 0);      
    AddQuad_SCE(-300, -300, 0, 300, -300, 0, 300, -300, 300, -300, -300, 300);
    AddQuad_SCE(-300, -300, 0, -300, -300, 300, -500, 0, 300, -500, 0, 0);
    AddQuad_SCE(300, -300, 0, 300, -300, 300, 500, 0, 300, 500, 0, 0);

  
    SetColor_SCE(0.95, 0.6, 0.4);  // somon/turuncu
    // piramit
    SetMaterial_SCE(0, 1, 0, 0.3, 500, 0);
    AddPyramid_SCE(0, 0, 200, 80, 80, 40, -80, 80, 40, -80, -80, 40, 80, -80, 40);

 
    Rendering(hWnd);


    CloseArrays_SCE();
}

void DrawStudyExample2(HWND hWnd)
{

    if (OpenArrays_SCE(100, 3000, 12000, 10) != 1) return;
    SetCameraPosition_SCE(350, 600, 320, 26, 75, 400); 
    AddLightSource_SCE(1, 1, 1, -150, 400, 600);      


    SetColor_SCE(0.6, 1.0, 0.9);
    SetMaterial_SCE(0.5, 0.5, 0, 0, 20, 0); 
    AddCylinder_SCE(-200, 200, 0, 15, 180, 18);
    CopyObjectAndShift_SCE(400, 0, 0);
    CopyObjectAndShift_SCE(0, -400, 0);
    CopyObjectAndShift_SCE(400, -400, 0);
    SetMaterial_SCE(0.3, 0.5, 0.2, 0, 20, 0);   
    AddPrisma4_SCE(-250, 250, 250, 250, 250, -250, -250, -250, 180, 200);

    SetColor_SCE(1.0, 0.95, 0.85);

    SetMaterial_SCE(0.0, 0.5, 0.1, 0.5, 200, 0);      

    AddSphere_SCE(0, 0, 320, 120, 6, 6);

    SetColor_SCE(1.0, 0.95, 0.85);

    SetMaterial_SCE(0.3, 0.4, 0.3, 0, 20, 0);  
    AddQuad_SCE(0, 0, 0, 400, 0, 0, 400, 400, 0, 0, 400, 0);
    CopyObjectAndShift_SCE(-400, -400, 0);
    CopyObjectAndShift_SCE(0, -800, 0);
    CopyObjectAndShift_SCE(-400, -1200, 0);
    CopyObjectAndShift_SCE(400, -400, 0);
    CopyObjectAndShift_SCE(400, -1200, 0);
    CopyObjectAndShift_SCE(-800, 0, 0);
    CopyObjectAndShift_SCE(-800, -800, 0);
    SetColor_SCE(0.8, 0.9, 1.0);

    AddQuad_SCE(0, -400, 0, 400, -400, 0, 400, 0, 0, 0, 0, 0);
    CopyObjectAndShift_SCE(-400, 400, 0);
    CopyObjectAndShift_SCE(0, -800, 0);
    CopyObjectAndShift_SCE(-400, -400, 0);
    CopyObjectAndShift_SCE(400, 400, 0);
    CopyObjectAndShift_SCE(400, -400, 0);
    CopyObjectAndShift_SCE(-800, 0, 0);
    CopyObjectAndShift_SCE(-800, -800, 0);

    Rendering(hWnd);

    CloseArrays_SCE();
}

void DrawStudyExample3(HWND hWnd)
{
    if (OpenArrays_SCE(100, 3000, 12000, 10) != 1) return;

    SetCameraPosition_SCE(78, 1200, 500, 0, 77, 700); 
    AddLightSource_SCE(1, 0.5, 0, -300, 200, 600);
    AddLightSource_SCE(0, 0.5, 1, 300, 200, 600);

    SetColor_SCE(0.8, 0.95, 1.0);

    SetMaterial_SCE(0, 0, 0.5, 0.5, 100, 0.5);
    AddSphere_SCE(0, 0, 350, 200, 10, 10);

    SetColor_SCE(1.0, 0.85, 0.75);

    SetMaterial_SCE(0.4, 0.4, 0.2, 0, 20, 0);
    AddQuad_SCE(0, 0, 0, 400, 0, 0, 400, 400, 0, 0, 400, 0);
    CopyObjectAndShift_SCE(-400, -400, 0);
    CopyObjectAndShift_SCE(0, -800, 0);
    CopyObjectAndShift_SCE(-400, -1200, 0);
    CopyObjectAndShift_SCE(400, -400, 0);
    CopyObjectAndShift_SCE(400, -1200, 0);
    CopyObjectAndShift_SCE(-800, 0, 0);
    CopyObjectAndShift_SCE(-800, -800, 0);
    SetColor_SCE(0.85, 0.95, 1.0);

    AddQuad_SCE(0, -400, 0, 400, -400, 0, 400, 0, 0, 0, 0, 0);
    CopyObjectAndShift_SCE(-400, 400, 0);
    CopyObjectAndShift_SCE(0, -800, 0);
    CopyObjectAndShift_SCE(-400, -400, 0);
    CopyObjectAndShift_SCE(400, 400, 0);
    CopyObjectAndShift_SCE(400, -400, 0);
    CopyObjectAndShift_SCE(-800, 0, 0);
    CopyObjectAndShift_SCE(-800, -800, 0);

    Rendering(hWnd);

    CloseArrays_SCE();
}

void Rendering(HWND hWnd) 
{
    HDC hdc = GetDC(hWnd);       
    RECT rc;
    GetClientRect(hWnd, &rc);     
    Rendering_RAYTR(hdc, rc.right, rc.bottom);
    ReleaseDC(hWnd, hdc);         
}