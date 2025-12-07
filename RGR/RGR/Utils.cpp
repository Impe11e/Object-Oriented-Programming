#include "Utils.h"
#include <locale>
#include <codecvt>

void ImbueUtf8(std::wifstream &f)
{
    f.imbue(std::locale(f.getloc(), new std::codecvt_utf8<wchar_t>()));
}

void ImbueUtf8(std::wofstream &f)
{
    f.imbue(std::locale(f.getloc(), new std::codecvt_utf8<wchar_t>()));
}

std::vector<wchar_t> MakeNullTerminated(const std::wstring &s)
{
    std::vector<wchar_t> buf(s.begin(), s.end());
    buf.push_back(0);
    return buf;
}
