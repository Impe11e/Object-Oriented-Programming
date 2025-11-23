#pragma once

#include <string>
#include <Windows.h>

static bool PutTextToClipboardW(HWND hWnd, const std::wstring &src)
{
    if (src.empty()) return false;
    size_t sizeBytes = (src.size() + 1) * sizeof(wchar_t);
    HGLOBAL hglbCopy = GlobalAlloc(GHND, sizeBytes);
    if (!hglbCopy) return false;
    void *pTmp = GlobalLock(hglbCopy);
    if (!pTmp) { GlobalFree(hglbCopy); return false; }
    memcpy(pTmp, src.c_str(), sizeBytes);
    GlobalUnlock(hglbCopy);
    if (!OpenClipboard(hWnd)) { GlobalFree(hglbCopy); return false; }
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, hglbCopy);
    CloseClipboard();
    return true;
}

static std::wstring GetTextFromClipboardW(HWND hWnd)
{
    std::wstring res;
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) return res;
    if (!OpenClipboard(hWnd)) return res;
    HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
    if (hglb) {
        wchar_t *p = (wchar_t*)GlobalLock(hglb);
        if (p) {
            res = p;
            GlobalUnlock(hglb);
        }
    }
    CloseClipboard();
    return res;
}
