//---------------------------------------------------------------------------
#ifndef StorageH
#define StorageH

#include <vector>


//#include <typeinfo>

class TStorage;  // опережающее объявление

//---------------------------------------------------------------------------
// Поле
// TStorageField
//---------------------------------------------------------------------------
class TStorageField {
public:
    bool active;        // Признак необходимости заполненеия поля (если false, поле создается, но не заполняется)
    bool enable;        // Признак того, необходимо ли вообще учитывать это поле (если false, поле не создается)
    String name;    // Имя поля (до 10 символов).
    String name_src;    // Имя поля из копируемой БД

//private:
    bool required;      // Признак обязательности наличия сопоставленного поля в источнике
    bool linked;

protected:
    //int FieldType;
    //virtual Copy(TStorageField *Field);
    //GetName();
    //GetLength();
    //GetDecimals();
};

//---------------------------------------------------------------------------
// Таблица
// TStorageTable
//---------------------------------------------------------------------------
class TStorageTable {
public:
    TStorageTable();
    bool Truncate;
};


//---------------------------------------------------------------------------
// Хранилище
// class TStorage
//---------------------------------------------------------------------------

class TStorage
{
public:
    friend TStorageField;

    TStorage();

    virtual ~TStorage() {};

    bool linkSource(TStorage* Storage);
    //bool FindField(AnsiString fieldName);
    TStorageField* findField(AnsiString fieldName);

    virtual void openTable(bool ReadOnly = true) {};
    virtual Variant getFieldValue(TStorageField* Field) {};
    virtual void setFieldValue(Variant Value) {};
    virtual void commit() {};
    virtual void append();   // Добавляет пустую запись перед (используется в DBase)
    virtual void post() {};     // Фиксирует запись (используется в OraProc)
    virtual void rollback();     // Фиксирует запись (используется в OraProc)

    virtual void closeTable();

    virtual bool eot();     // End Of Tables
    virtual bool eor() {};     // End Of Records
    virtual bool eof();     // End Of Fields

    virtual void nextTable();
    virtual void nextRecord();
    virtual void nextField();

    virtual TStorageField* getField();   // Возвращает имя сопоставленного поля в источнике
    bool isActiveField();
    bool isLinkedField();
    bool isActiveTable() { return Active; };
    bool isModified() { return Modified; };
    //virtual void SetReadOnly(bool ReadOnlyFlag){ this->ReadOnly = ReadOnlyFlag; };
    virtual int getRecordCount(){ return RecordCount; };
    virtual int getRecordIndex(){ return RecordIndex; };

    // Информационные функции
    virtual AnsiString getTable() = 0;     // Возвращает наименование активного хранилища данных
    virtual AnsiString getTableStage();     // Возвращает текущий этап обработки данных
    virtual AnsiString getRecordStage();     // Возвращает текущий этап обработки данных

    virtual TStorageField* addField(TStorageField* Field) {return NULL;};
    virtual void loadFieldDefs() {};    // Загружает описание полей

    // Методы копирования полей
    // Используется двойная диспетчеризация
    virtual copyFieldsFrom(TStorage* storage);
    virtual copyFieldsToDbf(TStorage* storage);
    virtual copyFieldsToExcel(TStorage* storage);
    //virtual CopyFieldsToOraProc(TStorage* storage);

    virtual setTemplate(TStorage* storage, bool deleteAfterUse = true);

protected:
    void fullCopyFields(TStorage* src, TStorage* dst);
    int TableIndex;
    int TableCount;
    int FieldCount;
    int FieldIndex;
    int RecordCount;
    int RecordIndex;
    bool Active;    // Признак того, что источник/приемник открыт и готов к считыванию/записи данных
    bool ReadOnly;
    bool Modified;

    std::vector<TStorageField*> Fields;

    TStorage* templateStorage;    // Шаблон (берется первая доступная таблица)
    bool delTemplateStorage;

};


//---------------------------------------------------------------------------
#endif
