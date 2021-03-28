#include <ctime>
#include <memory>

#include "framework.h"
#include "task1.h"

#include <gdiplus.h> 
#pragma comment(lib, "GdiPlus.lib")

#define MAX_LOADSTRING 100

#define UM_CHANGE_BKGND WM_APP + 1

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];           

WNDPROC g_oldProc;
HWND g_btn;
const int ID_CHANGE_WNDPROC_BTN = 12345;
std::unique_ptr<HBRUSH> g_brush;
std::unique_ptr<HBITMAP> g_bkgnd_bitmap; 
std::unique_ptr<Gdiplus::Bitmap> g_bkgnd;
ULONG_PTR g_gdiplusToken;

ATOM                MyRegisterClass( HINSTANCE hInstance );
BOOL                InitInstance( HINSTANCE, int );
LRESULT CALLBACK    MainWndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    BtnDblClickWndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK    About( HWND, UINT, WPARAM, LPARAM );

void SwitchWndProc( HWND, WNDPROC ) noexcept;

int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPWSTR    lpCmdLine,
                       _In_ int       nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine ); 

    srand( time( 0 ) );

    LoadStringW( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
    LoadStringW( hInstance, IDC_TASK1, szWindowClass, MAX_LOADSTRING );
    MyRegisterClass( hInstance );

    if( !InitInstance( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_TASK1 ) );

    MSG msg;
    while( GetMessage( &msg, nullptr, 0, 0 ) )
    {
        if( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass( HINSTANCE hInstance )
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof( WNDCLASSEX );

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_TASK1 ) );
    wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = MAKEINTRESOURCEW( IDC_TASK1 );
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon( wcex.hInstance, MAKEINTRESOURCE( IDI_SMALL ) );

    return RegisterClassExW( &wcex );
}

BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
    hInst = hInstance; 

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup( &g_gdiplusToken, &gdiplusStartupInput, NULL );

    int screenWidth = GetSystemMetrics( SM_CXSCREEN );
    int screenHeight = GetSystemMetrics( SM_CYSCREEN );
    int wndWidth = screenWidth / 2;
    int wndHeight = screenHeight / 2;

    HWND hWnd = CreateWindowW( szWindowClass,
                               szTitle,
                               WS_OVERLAPPEDWINDOW,
                               ( ( screenWidth - wndWidth ) / 2 ),
                               ( ( screenHeight - wndHeight ) / 2 ),
                               wndWidth,
                               wndHeight,
                               nullptr,
                               nullptr,
                               hInstance,
                               nullptr );

    if( !hWnd )
    {
        return FALSE;
    }

    ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );

    return TRUE;
}

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
        case WM_CREATE:
        {
            RECT rc;
            GetClientRect( hWnd, &rc );
            int width = 120;
            int height = 30;
            int xPos = ( ( rc.right - rc.left ) - width ) / 2;
            int yPos = ( ( rc.bottom - rc.top ) - height ) / 2;

            g_btn = CreateWindow( L"button",
                                  L"Double click me",
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY,
                                  xPos,
                                  yPos,
                                  width,
                                  height,
                                  hWnd,
                                  reinterpret_cast<HMENU>( ID_CHANGE_WNDPROC_BTN ),
                                  hInst,
                                  nullptr );

            g_brush = std::make_unique<HBRUSH>( CreateSolidBrush( RGB( rand() % 256, rand() % 256, rand() % 256 ) ) );
            
            Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile( L"space.jpeg", false );
            HBITMAP bmp;
            bitmap->GetHBITMAP( {}, &bmp );
            g_bkgnd_bitmap = std::make_unique<HBITMAP>( bmp );
            bmp = nullptr;

            SetClassLongPtr( hWnd, GCL_HBRBACKGROUND, reinterpret_cast<LONG>( *g_brush ) );
            InvalidateRect( hWnd, nullptr, true );            
            break;
        }
        case UM_CHANGE_BKGND:
        {
            SetClassLongPtr( hWnd, GCL_HBRBACKGROUND, reinterpret_cast<LONG>( *g_brush ) );
            InvalidateRect( hWnd, nullptr, true );
            break;
        }
        case WM_COMMAND:
        {
            switch( LOWORD( wParam ) )
            {                
                case ID_CHANGE_WNDPROC_BTN:
                {
                    if( HIWORD( wParam ) == BN_DBLCLK )
                    {
                        SwitchWndProc( hWnd, BtnDblClickWndProc );
                        SendMessage( hWnd, UM_CHANGE_BKGND, NULL, NULL );
                    }
                    break;
                }
                case IDM_ABOUT:
                    DialogBox( hInst, MAKEINTRESOURCE( IDD_ABOUTBOX ), hWnd, About );
                    break;
                case IDM_EXIT:
                    DestroyWindow( hWnd );
                    break;
                default:
                    return DefWindowProc( hWnd, message, wParam, lParam );
            }
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint( hWnd, &ps );

            EndPaint( hWnd, &ps );
            break;
        }
        case WM_DESTROY:
        {
            Gdiplus::GdiplusShutdown( g_gdiplusToken );
            PostQuitMessage( 0 );
            break;
        }
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }
    return 0L;
}

LRESULT CALLBACK BtnDblClickWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
        case UM_CHANGE_BKGND:
        {

            auto bkgnd = CreatePatternBrush( *g_bkgnd_bitmap );
            SetClassLongPtr( hWnd,
                             GCL_HBRBACKGROUND,
                             reinterpret_cast<LONG>( bkgnd ) );
            InvalidateRect( hWnd, nullptr, true );
            break;
        }
        case WM_COMMAND:
        {
            if( LOWORD( wParam ) == ID_CHANGE_WNDPROC_BTN && 
                HIWORD( wParam ) == BN_DBLCLK )
            {
                SwitchWndProc( hWnd, MainWndProc );
                SendMessage( hWnd, UM_CHANGE_BKGND, NULL, NULL );
            }
            else
            {
                return CallWindowProc( g_oldProc, hWnd, message, wParam, lParam );
            }
            break;
        }
        default:
            return CallWindowProc( g_oldProc, hWnd, message, wParam, lParam );
    }

    return 0L;
}

void SwitchWndProc( HWND hWnd, WNDPROC newWndProc ) noexcept
{
    g_oldProc = reinterpret_cast<WNDPROC>( SetWindowLongPtr( hWnd,
                                                             GWL_WNDPROC,
                                                             reinterpret_cast<LONG_PTR>( newWndProc ) ) );
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER( lParam );
    switch( message )
    {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;

        case WM_COMMAND:
        {
            if( LOWORD( wParam ) == IDOK || 
                LOWORD( wParam ) == IDCANCEL )
            {
                EndDialog( hDlg, LOWORD( wParam ) );
                return (INT_PTR)TRUE;
            }
        }
        break;
    }
    return (INT_PTR)FALSE;
}
