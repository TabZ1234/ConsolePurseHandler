#pragma once
#include"Balance.h"

class PurseHandlerData
{
private:
    static constexpr int field_count{ 5 };
    int obj_count;

    path default_fname;
    path store_main_fname;
    path main_file;

    mutable vector<Balance<int, int, wstring>> vb;
    vector<wstring> info_vect;


    void CrDir();
    void prepareConsole(int fsize = 32);
    void readObjectsCount();
    void initObjects();
    vector<wstring> parseData();


    void setFname(path& dir);
    wstring getPath(const wstring& fname);
    bool checkFnameSafety(const path& path);
    void set_path_defaults();
    void alloc_vectors();
    void destroy_info_vect();


public:
    PurseHandlerData();
    ~PurseHandlerData();
    void sort_vb();
    void init_data();

    void readCustomFileName();
    void writeCustomFileName();

    void deleteLastBalance();
    void findBalance();
    void emergencyBackup();
    bool changeFile();
    bool clearFile();
    bool makeCopy();
    bool showList();
    bool showList(const Balance<int, int, wstring>& obj);
    void createObject();
};
