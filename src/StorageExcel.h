//---------------------------------------------------------------------------
#ifndef StorageExcelH
#define StorageExcelH

#include <vector>
#include "Storage.h"

#include "ExcelUtil.h"


class TStorageExcel;


//---------------------------------------------------------------------------
// Структура для хранения полей в MS Excel
class TExcelField : public TStorageField {    // Для описания структуры dbf-файла
public:
    String format;      // Формат ячейки в MS Excel
    //TExcelField(AnsiString Name, int Index);
private:
    friend class TStorageExcel;
    int index;
};

// Структура для хранения параметров
class TExcelTable : public TStorageTable
{
public:
    AnsiString File;
    int WorksheetIndex;
    int TitleRowIndex;
    int LastDataRowIndex;
    int FirstDataRowIndex;
};

//---------------------------------------------------------------------------
// class TStorageExcel: TStorage
// Класс объекта источника/приемника данных
// Используется таблица, хранимая в файле MS Excel
//---------------------------------------------------------------------------

class TStorageExcel: TStorage
{
protected:
    //friend class TExcelField;
    void closeTable();


public:
    TStorageExcel(){};
    ~TStorageExcel();
    void openTable(bool ReadOnly = true);
    Variant getFieldValue(TStorageField* Field);
    void setFieldValue(Variant Value);
    void commit();
    void append();

    //bool Eot();     // End Of Tables
    bool eor();     // End Of Records
    //bool Eof();     // End Of Fields

    //void NextTable();
    //void nextRecord();
    //void NextField();


    virtual TExcelField* addField();
    void addTable(const TExcelTable& Table);

    AnsiString getTable();

    copyFieldsFrom(TStorage* storage);

private:
    copyFieldsToExcel(TStorage* storage);
    std::vector<TExcelTable> Tables;    // Список полей для экспорта
    MSExcelWorks* msexcel;
    Variant Worksheet;
    Variant Workbook;
};

//---------------------------------------------------------------------------
#endif
