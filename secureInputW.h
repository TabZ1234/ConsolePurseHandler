#pragma once
#include"stdafx.h"

#define _PURSE_HANDLER

using namespace std;

template<typename type, typename functor>
inline void secureInputW(wistream& stream, type& var, functor f);
template<typename functor>
inline void secureInputW(wistream& stream, int& var, functor f);
template<typename functor>
inline void secureInputW(wistream& stream, wstring& var, functor f);
template<typename type>
inline void secureInputW(wistream& stream, type& var);
template<>
inline void secureInputW<wstring>(wistream& stream, wstring& var);
template<>
inline void secureInputW<int>(wistream& stream, int& var);


template<typename type, typename functor>
void secureInputW(wistream& stream, type& var, functor f)
{
    type temp;
    do
    {
        stream.seekg(ios::end);
        stream >> temp;

    } while ([&]()->bool {
        if (temp == static_cast<type>(NULL))
        {
            stream.clear();
            return true;
        }
        else
        {
            if (f(temp))return true;

            stream.clear();
            var = temp;
            return false;
        }
        }());
}
template<typename functor>
void secureInputW(wistream& stream, int& var, functor f)
{
    wstring buff;

bad_input:
    stream.seekg(ios::end);
    getline(stream, buff);
    try
    {
#ifdef _PURSE_HANDLER
        if (buff == L"q")throw - 1;
#endif
        var = stoi(buff);
        if (f(var))goto bad_input;
    }
    catch (const exception&)
    {
        goto bad_input;
    }
    stream.clear();
}
template<typename functor>
void secureInputW(wistream& stream, wstring& var, functor f)
{
    do
    {
        stream.seekg(ios::end);
        getline(stream, var);

    } while ([&]()->bool {
        for (const auto& elem : var)
            if (!f(elem))return true;
        return false;
        }());
    stream.clear();
}

//WITHOUT FUNCTOR
template<typename type>
void secureInputW(wistream& stream, type& var)
{
    type temp;
    do
    {
        stream.seekg(ios::end);
        stream >> temp;
    } while ([&]()->bool {
        if (temp == static_cast<type>(NULL))
        {
            stream.clear();
            return true;
        }
        else
        {
            stream.clear();
            var = temp;
            return false;
        }
        }());
}
template<>
void secureInputW<wstring>(wistream& stream, wstring& var)
{
    stream.clear();
    stream.seekg(ios::end);
    getline(stream,var);
    if (var == L"q")throw - 1;
}

template<>
void secureInputW<int>(wistream& stream, int& var)
{
    wstring buff;

bad_input:
    stream.seekg(ios::end);
    getline(stream, buff);
    try
    {
#ifdef _PURSE_HANDLER
        if (buff == L"q")throw - 1;
        var = stoi(buff);

        try
        {
            if (buff[0] == L'+' || buff[0] == L'-')throw stoi(buff);
        }
        catch (const invalid_argument&)
        {
            goto bad_input;
        }
#endif
        var = stoi(buff);
    }
    catch (const exception&)
    {
        goto bad_input;
    }
    stream.clear();
}
//WITHOU
