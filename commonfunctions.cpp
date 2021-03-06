//commonfunctions.cpp
#pragma warning (disable : 6031)//return value ignored

#include "stdafx.h"
#include "commonfunctions.h"

#define Balance Balance<int,int,wstring>
#define Balance_ref Balance&

#define F_DELIM Balance::field_delimiter
#define B_DELIM Balance::block_delimiter

#define ERR error(convert_cstr_to_wstring(err.what()));

#define u16_to_u8 wstring_convert<codecvt_utf8_utf16<wchar_t>>
#define u8_to_u16 wstring_convert<codecvt_utf8<wchar_t>>



void eraseFile(const path& main_file)
{
    fstream file;

    try
    {
        file.open(main_file.fullpath, ios::trunc | ios::out);
        file.close();
    }
    catch (const ios_base::failure& err)
    {
        ERR
            throw FATAL_ERROR;
    }
}
tm getLocalTime64()
{
    tm time;
    __time64_t long_time;
    _time64(&long_time);
    _localtime64_s(&time, &long_time);

    time.tm_mon++;//1-12
    return time;
}
wstring convert_cstr_to_wstring(const char* c_str)
{
    string u8_str(c_str);
    u8_to_u16 converter;
    return converter.from_bytes(u8_str);
}

void cls()
{
    system("CLS");
}

void success(const wstring &text)
{
    setSuccessTextColor();
    wcout << endl << text << endl;
    setNormalTextColor();
    wait();
}
void error(const wstring &err)
{
    setErrorTextColor();
    wcerr << endl << err << endl;
    setNormalTextColor();
    wait();
}
wchar_t menu(wstring text, const int options_count)
{
    cls();
    wchar_t choice{};

    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
        text.c_str(), text.size(), NULL, NULL);

bad_input:
    try
    {

        secureInputW(wcin, choice, [&](const auto&)->bool {
            return (choice < 0 && choice >= options_count || choice == L'q');
            });
    }
    catch (const invalid_argument& err)
    {
        ERR
            goto bad_input;
    }
    return choice;
}
void wait(const int dot_c, const int speed_ms)
{
    const int dot_count{ dot_c };
    wstring massage, buff;
    wcout << L'\n';
    do
    {
        buff += L'\r';
        for (int i = 0; i < massage.size(); i++)
            buff += L' ';

        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
            buff.c_str(), buff.size(), NULL, NULL);

        massage = L"\rНажмите любую клавишу для продолжения";

        for (int i = 0; i < dot_count && !_kbhit(); i++)
        {
            WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
                massage.c_str(), massage.size(), NULL, NULL);

            massage += L'.';
            for (int i = 0; i < 32; i++)
            {
                Sleep(speed_ms / 32);
                if (_kbhit())break;
            }
        }
    } while (!_kbhit());
    wcin.ignore();
}
void setProgrammColors()
{
    system("color F0");
}
void setDefaultColors()
{
    system("color 0F");
}

auto set_color = [](const int color)->bool {
    if (color < 1 || color >256)return false;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    return true;
};

bool setErrorTextColor(const int color)
{
    return set_color(color);
}
bool setNormalTextColor(const int color)
{
    return set_color(color);
}
bool setSuccessTextColor(const int color)
{
    return set_color(color);
}