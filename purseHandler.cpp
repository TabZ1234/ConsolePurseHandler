//PurseHandler.cpp
#ifdef NDEBUG
#pragma warning (disable : 26451)//arithmetic overflow 
#pragma warning (disable : 6031)//return value ignored
#endif

#include "stdafx.h"
#include"resource.h"
#include"PurseHandlerData.h"

wchar_t menu(wstring text, const int options_count);

int wmain()
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    _wsetlocale(LC_ALL, NULL);



    PurseHandlerData Data;
    try
    {
        Data.init_data();

        const wstring menu_text = L"                             \u2736 Главное меню \u2736\n"
                                  L"1.Показать все записи\n"
                                  L"2.Сделать новую запись\n"
                                  L"3.Получить записи по дате\n"
                                  L"4.Манипуляции с файловой системой\n";

        constexpr int main_menu_options_count = 4;

        while (true)
        {
            Data.readCustomFileName();
            switch (menu(menu_text, main_menu_options_count))
            {
            case L'1':
            {
                cls();
                if (Data.showList())
                {
                    _getwch();
                    break;
                }
                else
                {
                    error(L"Список пуст");
                    break;
                }
            }
            case L'2':
            {
                Data.createObject();
                break;
            }
            case L'3':
            {
                Data.findBalance();
                break;
            }
            case L'4':
            {

                const  wstring menu_text = L"                            \u21a8 Файловое меню \u21a8\n"
                                           L"1.Изменить основной файл записи\n"
                                           L"2.Скопировать информацию в новый файл\n"
                                           L"3.Очистить файл\n"
                                           L"4.Удалить последнюю запись\n";

                constexpr int file_menu_options_count = 4;
                bool  exit_fmenu{ false };
                do
                {
                    switch (menu(menu_text, file_menu_options_count))
                    {
                    case L'1':
                    {
                        Data.changeFile();
                        break;
                    }
                    case L'2':
                    {
                        while (!Data.makeCopy());
                        break;
                    }
                    case L'3':
                    {
                        Data.clearFile();
                        break;
                    }
                    case L'4':
                    {
                        Data.deleteLastBalance();
                        break;
                    }
                    case L'q':
                    {
                        exit_fmenu = true;
                        break;
                    }
                    }
                } while (!exit_fmenu);
                break;
            }
            case L'q':
            {
                wait();
                exit(EXIT_SUCCESS);
            }
            }
        }
    }
    catch (...)
    {
#ifdef NDEBUG
        Data.emergencyBackup();
#endif
        wait();
        exit(EXIT_FAILURE);
    }
}
