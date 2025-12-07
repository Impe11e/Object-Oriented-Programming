#include "MyTable.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <windowsx.h>
#include <codecvt>

MyTableClass::MyTableClass(HWND hParentWnd, HINSTANCE hInst)
    : m_hParentWnd(hParentWnd)
    , m_hInst(hInst)
    , m_hListView(nullptr)
    , m_hEditWnd(nullptr)
    , m_editRow(-1)
    , m_editCol(-1)
    , m_isEditing(false)
    , m_modified(false)
    , m_oldListViewProc(nullptr)
{
    CreateListView();
}

MyTableClass::~MyTableClass()
{
    if (m_hEditWnd)
    {
        DestroyWindow(m_hEditWnd);
    }
    
    if (m_hListView && m_oldListViewProc)
    {
        SetWindowLongPtr(m_hListView, GWLP_WNDPROC, (LONG_PTR)m_oldListViewProc);
    }
    
    if (m_hListView)
    {
        DestroyWindow(m_hListView);
    }
}

void MyTableClass::CreateListView()
{
    m_hListView = CreateWindowExW(
        0,
        WC_LISTVIEW,
        L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | 
        WS_BORDER | LVS_SINGLESEL | WS_VSCROLL | WS_HSCROLL,
        0, 0, 0, 0,
        m_hParentWnd,
        (HMENU)1001,
        m_hInst,
        nullptr
    );
    
    if (m_hListView)
    {
        ListView_SetExtendedListViewStyle(m_hListView,
            LVS_EX_FULLROWSELECT |   
            LVS_EX_GRIDLINES |        
            LVS_EX_DOUBLEBUFFER);     
        
        m_oldListViewProc = (WNDPROC)SetWindowLongPtr(m_hListView, 
            GWLP_WNDPROC, (LONG_PTR)ListViewSubclassProc);
        SetWindowLongPtr(m_hListView, GWLP_USERDATA, (LONG_PTR)this);
    }
}

void MyTableClass::InitializeColumns()
{
    if (!m_hListView || m_data.empty())
        return;
    
    int colCount = Header_GetItemCount(ListView_GetHeader(m_hListView));
    for (int i = colCount - 1; i >= 0; i--)
    {
        ListView_DeleteColumn(m_hListView, i);
    }
    
    LVCOLUMNW lvc = { 0 };
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 50;
    lvc.pszText = (LPWSTR)L"¹";
    ListView_InsertColumn(m_hListView, 0, &lvc);
    
    for (size_t i = 0; i < m_data[0].size(); i++)
    {
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
        lvc.fmt = LVCFMT_LEFT;
        lvc.cx = 120;
        lvc.pszText = (LPWSTR)m_data[0][i].c_str();
        ListView_InsertColumn(m_hListView, (int)i + 1, &lvc);
    }
}

void MyTableClass::FillListView()
{
    if (!m_hListView || m_data.empty())
        return;
    
    ListView_DeleteAllItems(m_hListView);
    
    InitializeColumns();
    
    for (size_t row = 1; row < m_data.size(); row++)
    {
        LVITEMW lvi = { 0 };
        lvi.mask = LVIF_TEXT;
        lvi.iItem = (int)row - 1;
        lvi.iSubItem = 0;
        
        wchar_t rowNum[16];
        swprintf_s(rowNum, L"%d", (int)row);
        lvi.pszText = rowNum;
        int itemIndex = ListView_InsertItem(m_hListView, &lvi);
        
        for (size_t col = 0; col < m_data[row].size(); col++)
        {
            ListView_SetItemText(m_hListView, itemIndex, (int)col + 1, 
                                (LPWSTR)m_data[row][col].c_str());
        }
    }
}

bool MyTableClass::LoadFromFile(const wchar_t* filename)
{
    std::wifstream file(filename);
    if (!file.is_open())
        return false;

    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>));

    m_data.clear();
    std::wstring line;
    
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
            
        std::vector<std::wstring> row;
        std::wstring cell;
        
        for (size_t i = 0; i < line.length(); ++i)
        {
            wchar_t ch = line[i];
            
            if (ch == L'\t')
            {
                row.push_back(cell);
                cell.clear();
            }
            else if (ch != L'\r' && ch != L'\n')
            {
                cell += ch;
            }
        }
        
        row.push_back(cell);
        
        bool hasContent = false;
        for (const auto& c : row)
        {
            if (!c.empty())
            {
                hasContent = true;
                break;
            }
        }
        
        if (hasContent)
            m_data.push_back(row);
    }
    
    file.close();
    
    if (m_data.empty())
        return false;
    
    m_currentFileName = filename;
    m_modified = false;
    m_undoStack.clear();
    
    FillListView();
    
    return true;
}

bool MyTableClass::SaveToFile(const wchar_t* filename)
{
    std::wofstream file(filename);
    if (!file.is_open())
        return false;
    
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>));
    
    for (size_t i = 0; i < m_data.size(); ++i)
    {
        for (size_t j = 0; j < m_data[i].size(); ++j)
        {
            if (j > 0)
                file << L'\t';
            file << m_data[i][j];
        }
        file << L'\n';
    }
    
    file.close();
    m_currentFileName = filename;
    m_modified = false;
    
    return true;
}

bool MyTableClass::SaveToFileAs(const wchar_t* filename)
{
    return SaveToFile(filename);
}

void MyTableClass::OnSize(int width, int height)
{
    if (m_hListView)
    {
        MoveWindow(m_hListView, 0, 0, width, height, TRUE);
    }
}

void MyTableClass::GetSubItemRect(int row, int col, RECT* rect)
{
    ListView_GetSubItemRect(m_hListView, row, col, LVIR_BOUNDS, rect);
}

void MyTableClass::BeginEdit(int row, int col)
{
    if (m_isEditing)
    {
        EndEdit(false);
    }
    
    if (row < 0 || col <= 0)
        return;
    
    int dataRow = row + 1;
    int dataCol = col - 1; 
    
    if (dataRow >= (int)m_data.size() || dataCol >= (int)m_data[dataRow].size())
        return;
    
    m_editRow = dataRow;
    m_editCol = dataCol;
    m_isEditing = true;
    
    RECT rect;
    GetSubItemRect(row, col, &rect);
    CreateEditControl(rect, m_data[dataRow][dataCol]);
}

void MyTableClass::CreateEditControl(const RECT& cellRect, const std::wstring& text)
{
    m_hEditWnd = CreateWindowExW(
        0,
        L"EDIT",
        text.c_str(),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        cellRect.left, cellRect.top,
        cellRect.right - cellRect.left, cellRect.bottom - cellRect.top,
        m_hListView,
        nullptr,
        m_hInst,
        nullptr
    );
    
    if (m_hEditWnd)
    {
        HFONT hFont = (HFONT)SendMessage(m_hListView, WM_GETFONT, 0, 0);
        SendMessage(m_hEditWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
        SetFocus(m_hEditWnd);
        SendMessage(m_hEditWnd, EM_SETSEL, 0, -1);
        
        SetWindowSubclass(m_hEditWnd, EditSubclassProc, 0, (DWORD_PTR)this);
    }
}

void MyTableClass::DestroyEditControl()
{
    if (m_hEditWnd)
    {
        RemoveWindowSubclass(m_hEditWnd, EditSubclassProc, 0);
        DestroyWindow(m_hEditWnd);
        m_hEditWnd = nullptr;
    }
}

void MyTableClass::EndEdit(bool save)
{
    if (!m_isEditing || !m_hEditWnd)
        return;
    
    if (save)
    {
        int len = GetWindowTextLengthW(m_hEditWnd);
        std::wstring newText(len + 1, L'\0');
        GetWindowTextW(m_hEditWnd, &newText[0], len + 1);
        newText.resize(len);
        
        if (m_editRow >= 0 && m_editRow < (int)m_data.size() &&
            m_editCol >= 0 && m_editCol < (int)m_data[m_editRow].size())
        {
            if (m_data[m_editRow][m_editCol] != newText)
            {
                SaveEditState();
                m_data[m_editRow][m_editCol] = newText;
                m_modified = true;
                
                int listRow = m_editRow - 1; 
                int listCol = m_editCol + 1; 
                ListView_SetItemText(m_hListView, listRow, listCol, (LPWSTR)newText.c_str());
            }
        }
    }
    
    DestroyEditControl();
    m_isEditing = false;
    m_editRow = -1;
    m_editCol = -1;
    
    SetFocus(m_hListView);
}

void MyTableClass::SaveEditState()
{
    UndoState state;
    state.row = m_editRow;
    state.col = m_editCol;
    state.oldValue = m_data[m_editRow][m_editCol];
    
    int len = GetWindowTextLengthW(m_hEditWnd);
    std::wstring newText(len + 1, L'\0');
    GetWindowTextW(m_hEditWnd, &newText[0], len + 1);
    newText.resize(len);
    state.newValue = newText;
    
    m_undoStack.push_back(state);
    
    if (m_undoStack.size() > 50)
    {
        m_undoStack.erase(m_undoStack.begin());
    }
}

void MyTableClass::Undo()
{
    if (m_undoStack.empty())
        return;
    
    if (m_isEditing)
    {
        EndEdit(false);
    }
    
    UndoState state = m_undoStack.back();
    m_undoStack.pop_back();
    
    if (state.row >= 0 && state.row < (int)m_data.size() &&
        state.col >= 0 && state.col < (int)m_data[state.row].size())
    {
        m_data[state.row][state.col] = state.oldValue;
        m_modified = true;
        
        int listRow = state.row - 1;
        int listCol = state.col + 1;
        ListView_SetItemText(m_hListView, listRow, listCol, (LPWSTR)state.oldValue.c_str());
    }
}

void MyTableClass::OnNotify(LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;
    
    if (pnmh->hwndFrom == m_hListView)
    {
        if (pnmh->code == NM_DBLCLK)
        {
            LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
            if (lpnmitem->iItem >= 0 && lpnmitem->iSubItem > 0)
            {
                BeginEdit(lpnmitem->iItem, lpnmitem->iSubItem);
            }
        }
    }
}

void MyTableClass::OnKeyDown(WPARAM wParam)
{
    if (wParam == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000))
    {
        Undo();
    }
}

LRESULT CALLBACK MyTableClass::EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                                  LPARAM lParam, UINT_PTR uIdSubclass,
                                                  DWORD_PTR dwRefData)
{
    MyTableClass* pTable = (MyTableClass*)dwRefData;
    
    switch (msg)
    {
    case WM_KEYDOWN:
        if (wParam == VK_RETURN)
        {
            pTable->EndEdit(true);
            return 0;
        }
        else if (wParam == VK_ESCAPE)
        {
            pTable->EndEdit(false);
            return 0;
        }
        break;
        
    case WM_KILLFOCUS:
        pTable->EndEdit(true);
        break;
    }
    
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK MyTableClass::ListViewSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                                      LPARAM lParam, UINT_PTR uIdSubclass,
                                                      DWORD_PTR dwRefData)
{
    MyTableClass* pTable = (MyTableClass*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (msg)
    {
    case WM_KEYDOWN:
        if (pTable && wParam == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000))
        {
            pTable->Undo();
            return 0;
        }
        break;
    }
    
    return CallWindowProc(pTable->m_oldListViewProc, hwnd, msg, wParam, lParam);
}