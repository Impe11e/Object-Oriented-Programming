#pragma once
#include <fstream>
#include <string>
#include <vector>

void ImbueUtf8(std::wifstream &f);
void ImbueUtf8(std::wofstream &f);

std::vector<wchar_t> MakeNullTerminated(const std::wstring &s);
