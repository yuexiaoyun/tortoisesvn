// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2013 - TortoiseSVN

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "stdafx.h"
#include "BufferDC.h"

IMPLEMENT_DYNAMIC(CBufferDC, CPaintDC)

CBufferDC::CBufferDC(CWnd* pWnd) : CPaintDC(pWnd)
{
    if (pWnd != NULL && CPaintDC::m_hDC != NULL)
    {
        m_hOutputDC    = CPaintDC::m_hDC;
        m_hAttributeDC = CPaintDC::m_hAttribDC;

        pWnd->GetClientRect(&m_ClientRect);

        m_hMemoryDC = ::CreateCompatibleDC(m_hOutputDC);

        m_hPaintBitmap =
            ::CreateCompatibleBitmap(
                    m_hOutputDC,
                    m_ClientRect.right  - m_ClientRect.left,
                    m_ClientRect.bottom - m_ClientRect.top);

        m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemoryDC, m_hPaintBitmap);

        CPaintDC::m_hDC       = m_hMemoryDC;
        CPaintDC::m_hAttribDC = m_hMemoryDC;
    }

    m_bBoundsUpdated = FALSE;
}

CBufferDC::~CBufferDC(void)
{
    Flush();

    ::SelectObject(m_hMemoryDC, m_hOldBitmap);
    ::DeleteObject(m_hPaintBitmap);

    CPaintDC::m_hDC       = m_hOutputDC;
    CPaintDC::m_hAttribDC = m_hAttributeDC;

    ::DeleteDC(m_hMemoryDC);
}

void CBufferDC::Flush()
{
    ::BitBlt(
        m_hOutputDC,
        m_ClientRect.left, m_ClientRect.top,
        m_ClientRect.right  - m_ClientRect.left,
        m_ClientRect.bottom - m_ClientRect.top,
        m_hMemoryDC,
        0, 0,
        SRCCOPY);
}

UINT CBufferDC::SetBoundsRect( LPCRECT lpRectBounds, UINT flags )
{
    if (lpRectBounds != NULL)
    {
        if (m_ClientRect.right  - m_ClientRect.left > lpRectBounds->right  - lpRectBounds->left ||
            m_ClientRect.bottom - m_ClientRect.top  > lpRectBounds->bottom - lpRectBounds->top)
        {
            lpRectBounds = &m_ClientRect;
        }

        HBITMAP bmp =
            ::CreateCompatibleBitmap(
                    m_hOutputDC,
                    lpRectBounds->right - lpRectBounds->left,
                    lpRectBounds->bottom - lpRectBounds->top);

        HDC tmpDC  = ::CreateCompatibleDC(m_hOutputDC);

        HBITMAP oldBmp = (HBITMAP)::SelectObject(tmpDC, bmp);

        ::BitBlt(
            tmpDC,
            m_ClientRect.left, m_ClientRect.top,
            m_ClientRect.right  - m_ClientRect.left,
            m_ClientRect.bottom - m_ClientRect.top,
            m_hMemoryDC,
            0, 0,
            SRCCOPY);

        ::SelectObject(tmpDC, oldBmp);
        ::DeleteDC(tmpDC);

        HBITMAP old = (HBITMAP)::SelectObject(m_hMemoryDC, bmp);

        if (old != NULL && old != m_hPaintBitmap)
        {
            ::DeleteObject(old);
        }

        if (m_hPaintBitmap != NULL)
        {
            ::DeleteObject(m_hPaintBitmap);
        }

        m_hPaintBitmap = bmp;

        m_ClientRect = *lpRectBounds;
        m_bBoundsUpdated = TRUE;
    }

    return CPaintDC::SetBoundsRect(lpRectBounds, flags);
}

BOOL CBufferDC::RestoreDC( int nSavedDC )
{
    BOOL ret = CPaintDC::RestoreDC(nSavedDC);

    if (m_bBoundsUpdated)
    {
        SelectObject(m_hPaintBitmap);
    }

    return ret;
}