#include "Object2.h"
#include "ClipboardUtil.h"
#include <sstream>
#include <string>
#include <map>

LRESULT CALLBACK Object2_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        std::vector<PointI>* pts = (std::vector<PointI>*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (pts && !pts->empty()) {
            int y = 10;
            for (size_t i = 0; i < pts->size(); ++i) {
                std::wstringstream ss;
                ss << i+1 << L": (" << (*pts)[i].x << L"," << (*pts)[i].y << L")";
                std::wstring s = ss.str();
                TextOutW(hdc, 10, y, s.c_str(), (int)s.size());
                y += 16;
                if (y > 260) break;
            }
        }
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
    {
        std::vector<PointI>* pts = (std::vector<PointI>*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (pts) {
            delete pts;
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
        }
        break;
    }
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

HWND Object2::Create(HINSTANCE hInstance, int x, int y)
{
    const wchar_t *cls = L"Object2Class";
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Object2_WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = cls;
    RegisterClassExW(&wcex);
    HWND h = CreateWindowW(cls, L"Object2 - Data", WS_OVERLAPPEDWINDOW, x, y, 400, 300, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(h, SW_SHOW);
    UpdateWindow(h);
    return h;
}

void Object2::ShowPoints(HWND hWnd, const std::vector<PointI>& pts)
{
    // Process: sort by x, aggregate duplicates by averaging y
    if (pts.empty()) {
        // clear existing
        std::vector<PointI>* old = (std::vector<PointI>*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (old) { delete old; SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0); }
        InvalidateRect(hWnd, nullptr, TRUE);
        UpdateWindow(hWnd);
        return;
    }

    // aggregate
    std::map<int, std::pair<long long,int>> agg; // x -> (sumY,count)
    for (auto &p : pts) {
        auto it = agg.find(p.x);
        if (it == agg.end()) agg[p.x] = std::make_pair((long long)p.y, 1);
        else { it->second.first += p.y; it->second.second += 1; }
    }
    std::vector<PointI> proc;
    proc.reserve(agg.size());
    for (auto &kv : agg) {
        int x = kv.first;
        long long sum = kv.second.first;
        int cnt = kv.second.second;
        int y = (int)(sum / cnt);
        proc.push_back({x,y});
    }

    // free previous stored data if any
    std::vector<PointI>* old = (std::vector<PointI>*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    if (old) { delete old; }
    // set new data
    SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)new std::vector<PointI>(proc));

    // write textual representation to clipboard: one pair per line
    std::wstringstream ss;
    for (auto &p : proc) ss << p.x << L"\t" << p.y << L"\r\n";
    PutTextToClipboardW(hWnd, ss.str());
    // force redraw so WM_PAINT will show the new list
    InvalidateRect(hWnd, nullptr, TRUE);
    UpdateWindow(hWnd);
}
