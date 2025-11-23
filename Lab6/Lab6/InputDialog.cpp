#include "InputDialog.h"
#include "PointGenerator.h"
#include "Object2.h"
#include "Object3.h"
#include <string>
#include <sstream>

struct InputDialog::Impl {
    HWND hDlg;
    HWND hEdN, hEdXmin, hEdXmax, hEdYmin, hEdYmax, hBtnExec;
    HINSTANCE hInst;
    HWND hOwner;
    Impl(HINSTANCE hi, HWND owner): hDlg(NULL), hEdN(NULL), hEdXmin(NULL), hEdXmax(NULL), hEdYmin(NULL), hEdYmax(NULL), hBtnExec(NULL), hInst(hi), hOwner(owner) {}
};

static LRESULT CALLBACK Input_StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

InputDialog::InputDialog(HINSTANCE hInst_, HWND owner)
{
    pImpl = new Impl(hInst_, owner);
}

InputDialog::~InputDialog()
{
    if (pImpl) {
        // ensure window destroyed
        if (pImpl->hDlg) DestroyWindow(pImpl->hDlg);
        delete pImpl;
        pImpl = nullptr;
    }
}

HWND InputDialog::Create()
{
    const wchar_t *cls = L"InputDialogClass";
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Input_StaticWndProc;
    wcex.hInstance = pImpl->hInst;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcex.lpszClassName = cls;
    RegisterClassExW(&wcex);
    pImpl->hDlg = CreateWindowW(cls, L"Input parameters", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, 100, 100, 320, 220, pImpl->hOwner, nullptr, pImpl->hInst, pImpl);
    ShowWindow(pImpl->hDlg, SW_SHOW);
    UpdateWindow(pImpl->hDlg);
    return pImpl->hDlg;
}

static void Input_OnCreate(InputDialog::Impl* self, HWND hWnd)
{
    self->hEdN = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 100, 10, 80, 20, hWnd, NULL, self->hInst, NULL);
    CreateWindowW(L"STATIC", L"nPoints:", WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, hWnd, NULL, self->hInst, NULL);
    self->hEdXmin = CreateWindowW(L"EDIT", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 100, 40, 80, 20, hWnd, NULL, self->hInst, NULL);
    CreateWindowW(L"STATIC", L"xMin:", WS_CHILD | WS_VISIBLE, 10, 40, 80, 20, hWnd, NULL, self->hInst, NULL);
    self->hEdXmax = CreateWindowW(L"EDIT", L"100", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 100, 70, 80, 20, hWnd, NULL, self->hInst, NULL);
    CreateWindowW(L"STATIC", L"xMax:", WS_CHILD | WS_VISIBLE, 10, 70, 80, 20, hWnd, NULL, self->hInst, NULL);
    self->hEdYmin = CreateWindowW(L"EDIT", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 100, 100, 80, 20, hWnd, NULL, self->hInst, NULL);
    CreateWindowW(L"STATIC", L"yMin:", WS_CHILD | WS_VISIBLE, 10, 100, 80, 20, hWnd, NULL, self->hInst, NULL);
    self->hEdYmax = CreateWindowW(L"EDIT", L"100", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 100, 130, 80, 20, hWnd, NULL, self->hInst, NULL);
    CreateWindowW(L"STATIC", L"yMax:", WS_CHILD | WS_VISIBLE, 10, 130, 80, 20, hWnd, NULL, self->hInst, NULL);
    self->hBtnExec = CreateWindowW(L"BUTTON", L"Execute", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 160, 80, 24, hWnd, (HMENU)201, self->hInst, NULL);
}

static int GetIntFromEdit(HWND hEd, int defVal)
{
    wchar_t buf[64] = {0};
    GetWindowTextW(hEd, buf, 63);
    if (buf[0] == 0) return defVal;
    return _wtoi(buf);
}

static void Input_OnExecute(InputDialog::Impl* self)
{
    int n = GetIntFromEdit(self->hEdN, 10);
    int xmin = GetIntFromEdit(self->hEdXmin, 0);
    int xmax = GetIntFromEdit(self->hEdXmax, 100);
    int ymin = GetIntFromEdit(self->hEdYmin, 0);
    int ymax = GetIntFromEdit(self->hEdYmax, 100);

    if (xmin > xmax || ymin > ymax) {
        MessageBoxW(self->hDlg, L"Invalid ranges: ensure xMin<=xMax and yMin<=yMax.", L"Input error", MB_OK | MB_ICONERROR);
        return;
    }

    int nx = xmax - xmin + 1;
    int ny = ymax - ymin + 1;
    if (nx <= 0 || ny <= 0) {
        MessageBoxW(self->hDlg, L"Empty ranges result in no points.", L"Input error", MB_OK | MB_ICONERROR);
        return;
    }

    std::vector<PointI> pts;
    int maxDistinct = (nx < ny ? nx : ny);
    if (n > maxDistinct) {
        std::wstringstream ss;
        ss << L"Requested n is " << n << L" but there are only " << maxDistinct << L" distinct x and y values.\n\n";
        ss << L"Will cap n to " << maxDistinct << L" to ensure unique x and y values for graph.";
        MessageBoxW(self->hDlg, ss.str().c_str(), L"Capping n to distinct values", MB_OK | MB_ICONINFORMATION);
        n = maxDistinct;
    }
    // generate points with unique x and unique y values
    pts = GeneratePointsBijective(n, xmin, xmax, ymin, ymax);

    // create or find object windows
    HWND hObj2 = FindWindowW(L"Object2Class", NULL);
    if (!hObj2) hObj2 = Object2::Create(self->hInst, 50, 50);
    HWND hObj3 = FindWindowW(L"Object3Class", NULL);
    if (!hObj3) hObj3 = Object3::Create(self->hInst, 500, 50);

    Object2::ShowPoints(hObj2, pts);
    Object3::RequestReadClipboard(hObj3);
}

static LRESULT CALLBACK Input_StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    InputDialog::Impl* pThis = (InputDialog::Impl*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    if (message == WM_CREATE) {
        CREATESTRUCTW *pcs = (CREATESTRUCTW*)lParam;
        pThis = (InputDialog::Impl*)pcs->lpCreateParams;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
        Input_OnCreate(pThis, hWnd);
        return 0;
    }
    if (!pThis) return DefWindowProcW(hWnd, message, wParam, lParam);

    switch (message)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == 201) {
            Input_OnExecute(pThis);
        }
        break;
    case WM_DESTROY:
        // Don't quit the whole app; just cleanup
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
        pThis->hDlg = NULL;
        break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}
