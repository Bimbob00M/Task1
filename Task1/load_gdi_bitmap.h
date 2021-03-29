#pragma once

#include <windows.h>

#include <gdiplus.h>

Gdiplus::Bitmap* LoadGdiBitmap( HMODULE hInst, LPCTSTR pName, LPCTSTR pType );