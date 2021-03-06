#include"PurseHandlerData.h"

#define F_DELIM Balance<int,int,wstring>::field_delimiter
#define B_DELIM Balance<int,int,wstring>::block_delimiter

#define ERR error(convert_cstr_to_wstring(err.what()));

#define u16_to_u8 wstring_convert<codecvt_utf8_utf16<wchar_t>>
#define u8_to_u16 wstring_convert<codecvt_utf8<wchar_t>>

wstring PurseHandlerData::getPath(const wstring& fname)
{
    wchar_t dir[MAX_PATH];
    _wgetcwd(dir, MAX_PATH);

    if (errno == ERANGE) {
        wcout << L"ОШИБКА : путь  к .exe слишком длинный,перенесите программу \"повыше\"";
        wait();
        exit(EXIT_FAILURE);
    }

    wcscat_s(dir, MAX_PATH, L"\\PurseHandlerData\\");
    wcscat_s(dir, MAX_PATH, fname.c_str());


    return static_cast<wstring>(dir);
}
bool PurseHandlerData::checkFnameSafety(const path& path)
{

    constexpr int max_fname = 50;
    constexpr int windows_forbidden_path_symbols_size = 15;

    array<wchar_t, windows_forbidden_path_symbols_size> windows_forbidden_path_symbols{
     L'\\',L'/',L':',L'*',L'?',L'\"',L'>',L'<',L'.',L'|',L'+',L'%',L'!',L'@',L'\0'
    };

    if (path.fname.size() == 0)
    {
        error(L"ОШИБКА : имя файла не может быть пустым");
        return false;
    }

    for (const auto& elem : windows_forbidden_path_symbols)
        if (path.fname.find_first_of(elem) != string::npos)
        {
            wstring err_massage = L"ОШИБКА : имя файла не может содержать символ : ";
            err_massage += elem;
            error(err_massage);
            return false;
        }

    if (path.fname[path.fname.size() - 1] == L' ')
    {
        error(L"ОШИБКА : имя файла не может заканчиваться пробелом");
        return false;
    }
    if (path.fname.size() > max_fname)
    {
        error(L"ОШИБКА : имя файла сликом длинное");
        return false;
    }
    return true;
}

void PurseHandlerData::setFname(path& dir)
{
    size_t start = dir.fullpath.find_last_of(L'\\');
    size_t end = dir.fullpath.find_first_of(L'.');

    start++;
    dir.fname.clear();

    for (size_t i = start; i < end; i++)
    {
        dir.fname += dir.fullpath[i];
    }
}
void PurseHandlerData::deleteLastBalance()
{
    fstream file;

    string buff;
    string result;

    wchar_t choice;

    cls();
    wcout << L"Вы уверены,что хотите удалить последнюю запись ?(q - нет) :";
    secureInputW(wcin, choice);
    if (choice == L'q')return;

    try
    {
        if (vb.empty())throw - 2;

        file.open(main_file.fullpath, ios::binary | ios::in);
        if (!file)throw - 1;

        while (!file.eof())
        {
            getline(file, buff);
            result += buff + u8'\n';
        }
        file.close();

        result.pop_back();
        result.pop_back();
        result.pop_back();
        result.shrink_to_fit();

        for (size_t i = result.size() - 1; result.at(i) != B_DELIM && i>0; i--)
        {
            result.pop_back();
        }
        result += u8'\n';

        result.shrink_to_fit();

        file.open(main_file.fullpath, ios::binary | ios::out);
        file.write(result.c_str(), result.size());
        file.close();

        vb.pop_back();
        vb.shrink_to_fit();
        obj_count--;

        success(L"Последняя запись успешно удалена");
        wait();
    }
    catch (const ios_base::failure& err)
    {
        ERR
    }
    catch (int err)
    {
        if (err == -1)
            error(static_cast<wstring>(
                L"ОШИБКА : deleteLastBalance() не может открыть файл \""
                + main_file.fullpath + L"\""));
        else if (err == -2)error(L"ОШИБКА : файл и так пустой");
        else throw FATAL_ERROR;
    }
}
void PurseHandlerData::sort_vb()
{
    sort(vb.begin(), vb.end(), [](const Balance<int, int, wstring>& obj1,
        const Balance<int, int, wstring>& obj2)->bool {
            return obj1 < obj2;
        });
}
PurseHandlerData::PurseHandlerData()
{
    obj_count = 0;
    CrDir();
    set_path_defaults();
    readCustomFileName();
    writeCustomFileName();
    prepareConsole();
}
PurseHandlerData::~PurseHandlerData()
{
    vb.~vector();
}
void PurseHandlerData::init_data()
{
    alloc_vectors();
    info_vect = parseData();
    initObjects();
    destroy_info_vect();
    sort_vb();
}
void PurseHandlerData::set_path_defaults()
{
    default_fname.fname = L"PurseHandlerData.txt";
    default_fname.fullpath = getPath(default_fname.fname);

    store_main_fname.fname = L"NameOfMainFile.txt";
    store_main_fname.fullpath = getPath(store_main_fname.fname);

    main_file.fname = default_fname.fname;
    main_file.fullpath = getPath(main_file.fname);
}
void PurseHandlerData::alloc_vectors()
{
    readObjectsCount();
    vb.resize(obj_count);
    info_vect.resize(obj_count * field_count);
}
void PurseHandlerData::destroy_info_vect()
{
    info_vect.~vector();
}
void PurseHandlerData::CrDir()
{
    CreateDirectory(L"PurseHandlerData", NULL);
}
void PurseHandlerData::initObjects()
{
    int j{};
    for (auto& elem : vb)
    {
        elem.set_date(info_vect[j]);
        elem.set_time(info_vect[++j]);
        elem.set_balance(info_vect[++j]);
        elem.set_debt(info_vect[++j]);
        elem.set_description(info_vect[++j]);
        j++;
    }
}
void PurseHandlerData::readObjectsCount()
{
    fstream file;
    int obj_count{};
    int delim_count{};

    try
    {
        file.open(main_file.fullpath, ios::binary | ios::in);
        if (!file)throw - 1;

        string buff;
        string result;

        while (!file.eof())
        {
            getline(file, buff);
            result += buff;
        }
        for (const auto& elem : result)
        {
            if (elem == F_DELIM)delim_count++;
            else if (elem == B_DELIM)obj_count++;
        }

        if ((delim_count % 2) || (delim_count / 2 != obj_count * field_count))throw - 2;

        PurseHandlerData::obj_count = obj_count;

        file.close();
    }
    catch (const ios_base::failure& err)
    {
        ERR
            throw FATAL_ERROR;
    }
    catch (int err)
    {
        if (err == -1)
        {
            error(static_cast<wstring>(
                L"ОШИБКА : считыватель количества блоков не смог открыть файл \""
                + main_file.fullpath + L'\"'));

            fstream file(main_file.fullpath, ios::out);
            if (!file)throw FATAL_ERROR;
            file.close();
        }
        else if (err == -2)
        {
            error(L"ОШИБКА : данные повреждены");
            throw FATAL_ERROR;
        }
    }
}
vector<wstring> PurseHandlerData::parseData()
{
    fstream file;
    bool check{ true };

    auto fail_ret = []() {
        return vector<wstring>(0);
    };

    try
    {
        file.open(main_file.fullpath, ios::binary | ios::in);
        if (!file)throw - 1;

        vector<string> fieldbuff(field_count * obj_count);


        string u8buff;
        char ch{};

        vector<string>::iterator vect_iterator;
        int i{};

        for (i = 0, vect_iterator = fieldbuff.begin(); i < obj_count; i++)
        {
            getline(file, u8buff, static_cast<char>(B_DELIM));
            u8buff += B_DELIM;

            for (int i = 0; i < u8buff.size(); i++)
            {
                check = true;
                if (u8buff[i] == F_DELIM)
                {
                    i++;
                    if (u8buff[i] == F_DELIM)
                    {
                        u8buff[i] = u8'\0';
                        *vect_iterator++ += u8buff[i];
                        check = false;
                    }
                    while (u8buff[i] != F_DELIM && check)
                    {

                        *vect_iterator += u8buff[i++];
                        if (u8buff[i] == F_DELIM)vect_iterator++;
                    }
                }
            }
            u8buff.clear();
        }
        file.close();

        vector<wstring> u16fieldbuff(field_count * obj_count);

        u8_to_u16 convert;
        for (int i = 0; i < fieldbuff.size(); i++)
            u16fieldbuff[i] = convert.from_bytes(fieldbuff[i]);

        return u16fieldbuff;
    }
    catch (const ios_base::failure& err)
    {
        ERR
            throw FATAL_ERROR;
    }
    catch (int)
    {
        error(static_cast<wstring>
            (L"ОШИБКА : парсер не может открыть файл \"" 
            + main_file.fullpath + L"\""));
        return fail_ret();;
    }
    return fail_ret();
}
void PurseHandlerData::writeCustomFileName()
{
    fstream file_name_store;
    try
    {
        file_name_store.open(store_main_fname.fullpath, ios::binary | ios::out);

        u8_to_u16 converter;

        string u8_store_main_fname = converter.to_bytes(main_file.fullpath);


        file_name_store.write(u8_store_main_fname.c_str(), u8_store_main_fname.size());

        file_name_store.close();
    }
    catch (const ios_base::failure& err)
    {
        ERR
            throw FATAL_ERROR;
    }
}
void PurseHandlerData::readCustomFileName()
{
    fstream file_name_store;
    try
    {
        file_name_store.open(store_main_fname.fullpath, ios::binary | ios::in);
        if (!file_name_store)throw - 1;

        string u8_buff;
        string result;
        while (!file_name_store.eof())
        {
            getline(file_name_store, u8_buff);
            result += u8_buff;
        }

        char dir[MAX_PATH];
        _getcwd(dir, MAX_PATH);

        string result_copy(result);
        if ([&]()->bool {
            result_copy.erase(result_copy.find_last_of(u8'\\'));
                if(result_copy == static_cast<string>(dir))return false;
                else return true;
            }())
        {
            result_copy = result;
            result_copy = static_cast<string>(dir);
            result_copy += u8"\\PurseHandlerData";
            result_copy += result.substr(result.find_last_of(u8'\\'));
            result = result_copy;
        }

        u8_to_u16 converter;
        main_file.fullpath = converter.from_bytes(result);
        setFname(main_file);

        file_name_store.close();
    }
    catch (const ios_base::failure& err)
    {
        ERR
            throw FATAL_ERROR;
    }
    catch (int)
    {
        writeCustomFileName();
    }
}

void PurseHandlerData::findBalance()
{
    cls();

    int mday{}, month{}, year{};

    wcout << L"Введите месяц :";
    try
    {
        secureInputW(wcin, month, [&](const auto&)->bool {
            return (month <= 0 || month > 12);
            });

        wcout << L"Введите день :";
        secureInputW(wcin, mday, [&](const auto&)->bool {
            return (mday <= 0 || mday > 31);
            });

        wcout << L"Введите год :";
        secureInputW(wcin, year, [&](const auto&)->bool {
            return (year < 10 || year > 99);
            });
    }
    catch (int)
    {
        return;
    }
    cls();


    tm temp;
    int obj_count{};

    for (const auto& elem : vb)
    {
        temp = elem.get_time();
        if (temp.tm_mday == mday &&
            temp.tm_mon == month &&
            temp.tm_year == year + 100)
        {
            showList(elem);
            obj_count++;
        }
    }

    if (obj_count == 0)error(L"ОШИБКА : записи с такой датой нет");
    else
    {
        wstring buff;
        buff += L'\n';
        buff += L'\n';
        buff.append(77, L'\u2666');
        buff += L'\n';

        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
            buff.c_str(), buff.size(), NULL, NULL);

        success(static_cast<wstring>(
            L"Найдено "
            + to_wstring(obj_count)
            + L" записи c такой датой"));
    }
}
bool PurseHandlerData::changeFile()
{
    path main_new;
    bool path_safety{ false };

    do
    {
        cls();
        wcout << L"Введите название нового файла для записи :";

        try
        {
            secureInputW(wcin, main_new.fname);
        }
        catch (int)
        {
            return false;
        }
        path_safety = checkFnameSafety(main_new);

        if (path_safety)
        {
            main_new.fullpath = getPath(main_new.fname);
            main_new.fullpath += L".txt";
        }
    } while (!path_safety);

    main_file.fname = main_new.fname;
    main_file.fullpath = getPath(main_new.fname);
    main_file.fullpath += L".txt";


    writeCustomFileName();

    success(L"Файл для записи успешно изменён");

    vb.clear();
    return true;
}
bool PurseHandlerData::clearFile()
{
    wchar_t choice{};

    cls();
    wcout << L"Вы уверены,что хотите полностью очистить файл ?(q - нет) :";

    secureInputW(wcin, choice);
    if (choice == L'q')return false;

    eraseFile(main_file);
    success(L"Файл успешно очищен ");

    vb.clear();
    return true;
}
bool PurseHandlerData::makeCopy()
{
    path file_copy;
    bool path_safety{ false };
    do
    {
        cls();
        wcout << L"Введите имя файла-копии файла \""
              <<main_file.fname << L"\" :";

        try
        {
            secureInputW(wcin, file_copy.fname);
        }
        catch (int)
        {
            return true;
        }
    } while ([&]()->bool {
        path_safety = checkFnameSafety(file_copy);

        if (path_safety)
        {
            file_copy.fullpath = getPath(file_copy.fname);
            file_copy.fullpath += L".txt";
        }
        return !path_safety;
        }());

    if (file_copy.fname == L"q")return false;

    if (CopyFile(main_file.fullpath.c_str(),
        file_copy.fullpath.c_str(), FALSE)) success(static_cast<wstring>(
            L"Файл успешно скопирован в \""
            + file_copy.fullpath + L'\"'
        ));
    else throw FATAL_ERROR;

    return true;
}
void PurseHandlerData::emergencyBackup()
{
    tm time = getLocalTime64();
    wostringstream ss;
    ss << L"EMERGENCY COPY    "
        << time.tm_hour << L'.' << time.tm_min << L'.' << time.tm_sec << L"    "
        << time.tm_mday << L'.' << time.tm_mon << L'.' << time.tm_year - 100 
        << L".txt";
    wstring fname_to = ss.str();

    if (CopyFile(main_file.fullpath.c_str(), getPath(fname_to).c_str(), FALSE))
    success(L"Произошла критическая ошибка,аварийная копия успешно создана !!!");
    else 
        error(L"Произошла критическая ошибка,аварийная копия НЕ создана !!!");
}
void PurseHandlerData::createObject()
{
    wstring descr;
    constexpr int undefined = -777;
    int bal{ undefined }, db{ undefined };
    int i{};
    int j{};

    auto bal_lambda = [&]() {
        wcout << L"Введите баланс :";
        secureInputW(wcin, bal);
    };

    auto db_lambda = [&]() {
        wcout << L"Введите долг :";
        secureInputW(wcin, db);
    };
    auto descr_lambda = [&]() {
        wcout << L"Введите комментарий :";
        secureInputW(wcin, descr, [](wchar_t elem) {
            return (elem != B_DELIM && elem != F_DELIM);
            });
    };

    cls();
    try
    {
        bal_lambda();
        db_lambda();
        descr_lambda();
    }
    catch (int val)
    {
        if (val == -1)return;
        else {
            if (db == undefined)
            {
                if (vb.empty()) bal = val;
                else bal = vb.back().get_balance() + val;

                try
                {
                    db_lambda();
                    descr_lambda();
                }
                catch (int val)
                {
                    if (val == -1)return;
                    else {
                        if (vb.empty()) db = val;
                        else db = vb.back().get_debt() + val;
                        descr_lambda();
                    }
                }
            }
            else
            {
                db = vb.back().get_debt() + val;
                descr_lambda();
            }
        }
    }

    Balance<int,int,wstring> newobj(bal, db, descr, main_file.fullpath);
    newobj.print();

    vb.push_back(newobj);
    obj_count++;

    success(static_cast<wstring>(
        L"Запись в файл \"" 
        + main_file.fname 
        + L"\" успешно создана !!!"));
}
bool PurseHandlerData::showList()
{
    if (vb.empty())return false;

    for (const auto& obj : vb)
    {
        showList(obj);
    }
    
    wstring buff;
    buff += L'\n';
    buff += L'\n';
    buff.append(77, L'\u2666');
    buff += L'\n';
    buff += L'\n';
    buff += L"Количество записей :" + to_wstring(vb.size());
    buff += L'\n';

    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
        buff.c_str(), buff.size(), NULL, NULL);

    return true;
}
bool PurseHandlerData::showList(const Balance<int, int, wstring>& obj)
{
    if (vb.empty())return false;

    wstring buff;

    auto border_lambda = [&](wchar_t ch)->void {
        buff.append(75, ch);
    };
    [&]()->void {
        buff += L'\u2554';
        border_lambda(L'\u2550');
        buff += L'\u2557';
    }();
    [&]()->void {
        wstring str1, str2, str3, str4, str5, barrier_str;

        barrier_str += L'\u255f';
        barrier_str.append(75, L'\u2500');
        barrier_str += L'\u2562';

        str1 = L"\u2551Дата         : " + obj.get_date_str();
        str1.append(76 - str1.size(), L' ');
        str1 += L'\u2551';

        str2 = L"\u2551Время        : " + obj.get_time_str();
        str2.append(76 - str2.size(), L' ');
        str2 += L'\u2551';

        str3 = L"\u2551Баланс       : " + to_wstring(obj.get_balance()) + VALUTE;
        str3.append(76 - str3.size(), L' ');
        str3 += L'\u2551';

        str4 = L"\u2551Долг         : " + to_wstring(obj.get_debt()) + VALUTE;
        str4.append(76 - str4.size(), L' ');
        str4 += L'\u2551';

        str5 = L"\u2551Комментарий  : \u201e " + obj.get_description() + L" \u201d";
        const int line_count = str5.size() / 77;

        if (line_count > 0)
        {
            for (int i = 1, t = 0; i <= line_count; i++, t++)
            {
                str5[76 * i + t] = L'\u2551';
                str5[77 * i] = L'\u2551';
            }
            str5.append((76 * (line_count + 1) + line_count) - str5.size(), L' ');
            str5 += L'\u2551';
        }
        else
        {
            str5.append(76 - str5.size(), L' ');
            str5 += L'\u2551';
        }

        buff += str1 + barrier_str
            + str2 + barrier_str
            + str3 + barrier_str
            + str4 + barrier_str
            + str5;
    }();

    [&]()->void {
        buff += L'\u255a';
        border_lambda(L'\u2550');
        buff += L'\u255d';
    }();

    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
        buff.c_str(), buff.size(), NULL, NULL);

    return true;
}
void PurseHandlerData::prepareConsole(int fsize)
{

    HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND hwndhconsole = GetConsoleWindow();
    DWORD dwBytesWritten{};
    CONSOLE_FONT_INFOEX info{};

    info.cbSize = sizeof(info);
    info.dwFontSize.Y = fsize;
    info.FontWeight = FW_NORMAL;
    wcscpy_s(info.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(hconsole, NULL, &info);

    SetConsoleTitle(L"PurseHandler build 7.4.20");
    SetWindowPos(hwndhconsole, HWND_TOP, 0, 0, 1200, 900, NULL);
    SetConsoleScreenBufferSize(hconsole, { 77,9999 });
    setProgrammColors();
}