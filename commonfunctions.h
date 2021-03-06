//commonfunctions.h

#pragma once
#include "stdafx.h"
#include "secureInputW.h"

using namespace std;

constexpr auto FATAL_ERROR = -99;
constexpr auto VALUTE = L" грн";


struct path
{
    wstring fname;
    wstring fullpath;
};


wchar_t menu(wstring text, const int options_count);
void eraseFile(const path& main_file);
tm getLocalTime64();
void cls();
void error(const wstring&);
void success(const wstring&);
bool setNormalTextColor(const int color = 240);
bool setErrorTextColor(const int color = 244);
bool setSuccessTextColor(const int color = 242);
void setDefaultColors();
void setProgrammColors();
void wait(const int dot_c = 8,const int speed_ms = 200);
wstring convert_cstr_to_wstring(const char* c_err);


