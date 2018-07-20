//---------------------------------------------------------------------------
#ifndef StorageOraH
#define StorageOraH

#include <vector>
#include <map>
#include "Ora.hpp"
#include "DBAccess.hpp"
#include "MemDS.hpp"
#include "Storage.h"


// Структура для хранения параметров полей в Oracle (процедуры/таблицы)
class TOraField : public TStorageField {
};

class TStorageTableOra : public TStorageTable
{
public:
    String Table;       // Имя таблицы (используется если нужно предварительно очистить)
    String Server;
    String Username;
    String Password;
};


// Структура для хранения параметров
class TOraProcTable : public TStorageTableOra
{
public:
    String Procedure;   // Имя процедуры
};

// Структура для хранения параметров
class TOraSqlTable : public TStorageTableOra
{
public:
    String Sql;     // Имя файла с Sql-запросом
};


//---------------------------------------------------------------------------
// class TStorageOra
// base class for Oracle source/destination
//---------------------------------------------------------------------------

class TStorageOra: public TStorage
{
public:
    TStorageOra();

protected:
    void closeTable();
    virtual ~TStorageOra();

    void openConnection(AnsiString Server, AnsiString Username, AnsiString Password);
    TOraSession* dbSession;
    TOraQuery* dbQuery;
    void truncateTable(TStorageTableOra* Table);
    void executeProc(const String& procName);      // Добавлена для initProc и finalProc
    void prepareQuery();



};

//---------------------------------------------------------------------------
// class TStorageOraProc: TStorage
// Класс объекта приемника данных
// Используется процедура в БД Oracle
//---------------------------------------------------------------------------
class TStorageOraProc: public TStorageOra
{
public:
    void openTable(bool ReadOnly = true);
    void setFieldValue(Variant Value);
    void post();
    void commit();
    void nextField();
    virtual TOraField* addField();
    void addTable(const TOraProcTable& Table);
    AnsiString getTable();
private:
    std::vector<TOraProcTable> Tables;    // Список таблиц (процедур)
};



//---------------------------------------------------------------------------
// class TStorageOraSql: TStorage
// Класс объекта источника/приемника данных
// Используется таблица в БД Oracle
//---------------------------------------------------------------------------
class TStorageOraSql: public TStorageOra
{
public:
    //TStorageOraSql() {};
    void openTable(bool ReadOnly = true);
    //Variant Get(AnsiString Field);      // Возвращает значение по имени поля
    Variant getFieldValue(TStorageField* Field);
    //void Set(Variant Value);      // Устанавливает значение активного поля
    void setFieldValue(Variant Value);      // Устанавливает значение активного поля
    void append();
    void commit();
    void post();
    bool eor();     // End Of Records
    void nextRecord();
    virtual TOraField* addField();
    void addTable(const TOraSqlTable &Table);
    AnsiString getTable();

private:
    std::vector<TOraSqlTable> Tables;    // Список полей для экспрта в файл DBF
};







//---------------------------------------------------------------------------
#endif
