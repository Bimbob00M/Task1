#include <windows.h>

#include <gdiplus.h>

Gdiplus::Bitmap* LoadGdiBitmap( HMODULE hInst, LPCTSTR pName, LPCTSTR pType )
{
    HGLOBAL hBuffer;
    Gdiplus::Bitmap* pBitmap = nullptr;

    HRSRC hResource = FindResource( hInst, pName, pType );
    if( !hResource )
        return nullptr;

    DWORD imageSize = SizeofResource( hInst, hResource );
    if( !imageSize )
        return nullptr;

    HGLOBAL resource = LoadResource( hInst, hResource );
    if( !resource )
        return nullptr;

    const void* pResourceData = LockResource( resource );
    if( !pResourceData )
        return nullptr;

    hBuffer = GlobalAlloc( GMEM_MOVEABLE, imageSize );
    if( hBuffer )
    {
        void* pBuffer = GlobalLock( hBuffer );
        if( pBuffer )
        {
            CopyMemory( pBuffer, pResourceData, imageSize );

            IStream* pStream = NULL;
            if( CreateStreamOnHGlobal( hBuffer, FALSE, &pStream ) == S_OK )
            {
                pBitmap = Gdiplus::Bitmap::FromStream( pStream );
                pStream->Release();
                if( pBitmap )
                {
                    if( pBitmap->GetLastStatus() == Gdiplus::Ok )
                        return pBitmap;

                    delete pBitmap;
                    pBitmap = nullptr;
                }
            }
            GlobalUnlock( hBuffer );
        }
        GlobalFree( hBuffer );
        hBuffer = NULL;
    }
    return nullptr;
}