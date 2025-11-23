#include "Object3.h"
#include "ClipboardUtil.h"
#include <algorithm>
#include <sstream>

static std::vector<PointI> s_points;

LRESULT CALLBACK Object3_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_APP + 1:
    {
        // parse clipboard into vector of pairs
        s_points.clear();
        std::wstring data = GetTextFromClipboardW(hWnd);
        if (!data.empty()) {
            std::wistringstream iss(data);
            int x,y;
            while (iss >> x >> y) {
                s_points.push_back({x,y});
            }
            // sort by x
            std::sort(s_points.begin(), s_points.end(), [](const PointI&a,const PointI&b){ return a.x < b.x; });
            // remove duplicates by x, keep first occurrence
            std::vector<PointI> unique;
            unique.reserve(s_points.size());
            int lastX = INT_MIN;
            bool first = true;
            for (auto &p : s_points) {
                if (first || p.x != lastX) {
                    unique.push_back(p);
                    lastX = p.x;
                    first = false;
                }
            }
            s_points.swap(unique);
        }
        InvalidateRect(hWnd, nullptr, TRUE);
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
        int margin = 40;
        int left = rc.left + margin;
        int right = rc.right - margin;
        int top = rc.top + margin;
        int bottom = rc.bottom - margin;
        MoveToEx(hdc, left, bottom, NULL);
        LineTo(hdc, right, bottom);
        MoveToEx(hdc, left, bottom, NULL);
        LineTo(hdc, left, top);
        if (!s_points.empty()) {
            int xmin = s_points.front().x;
            int xmax = s_points.front().x;
            int ymin = s_points.front().y;
            int ymax = s_points.front().y;
            for (auto &p : s_points) {
                xmin = min(xmin, p.x);
                xmax = max(xmax, p.x);
                ymin = min(ymin, p.y);
                ymax = max(ymax, p.y);
            }
            if (xmin == xmax) xmax = xmin + 1;
            if (ymin == ymax) ymax = ymin + 1;
            std::wstringstream ssx; ssx << xmin; std::wstring sxmin = ssx.str();
            std::wstringstream ssx2; ssx2 << xmax; std::wstring sxmax = ssx2.str();
            TextOutW(hdc, left, bottom + 2, sxmin.c_str(), (int)sxmin.size());
            TextOutW(hdc, right - 30, bottom + 2, sxmax.c_str(), (int)sxmax.size());
            std::wstringstream ssy; ssy << ymin; std::wstring symin = ssy.str();
            std::wstringstream ssy2; ssy2 << ymax; std::wstring symax = ssy2.str();
            TextOutW(hdc, left - 30, bottom - 12, symin.c_str(), (int)symin.size());
            TextOutW(hdc, left - 30, top, symax.c_str(), (int)symax.size());
            auto mapX = [&](int vx)->int {
                double t = double(vx - xmin) / double(xmax - xmin);
                return left + (int)(t * (right - left));
            };
            auto mapY = [&](int vy)->int {
                double t = double(vy - ymin) / double(ymax - ymin);
                return bottom - (int)(t * (bottom - top));
            };
            std::vector<POINT> poly;
            poly.reserve(s_points.size());
            for (size_t i = 0; i < s_points.size(); ++i) {
                POINT pt;
                pt.x = mapX(s_points[i].x);
                pt.y = mapY(s_points[i].y);
                poly.push_back(pt);
            }
            if (!poly.empty()) Polyline(hdc, poly.data(), (int)poly.size());
        }
        EndPaint(hWnd, &ps);
    }
    break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

HWND Object3::Create(HINSTANCE hInstance, int x, int y)
{
    const wchar_t *cls = L"Object3Class";
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Object3_WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = cls;
    RegisterClassExW(&wcex);
    HWND h = CreateWindowW(cls, L"Object3 - Graph", WS_OVERLAPPEDWINDOW, x, y, 600, 400, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(h, SW_SHOW);
    UpdateWindow(h);
    return h;
}

void Object3::RequestReadClipboard(HWND hWnd)
{
    PostMessage(hWnd, WM_APP + 1, 0, 0);
}
