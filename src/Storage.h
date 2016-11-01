//---------------------------------------------------------------------------
#ifndef StorageH
#define StorageH

#include <vector>
#include "Logger.h"


class TStorage;  // ����������� ����������

//---------------------------------------------------------------------------
// ����
// TStorageField
//---------------------------------------------------------------------------
class TStorageField {
public:
    TStorageField();
    virtual ~TStorageField() {};

    bool active;        // ������� ������������� ����������� ���� (���� false, ���� ���������, �� �� �����������)
    bool enable;        // ������� ����, ���������� �� ������ ��������� ��� ���� (���� false, ���� �� ���������)
    String name;    // ��� ���� (�� 10 ��������).
    String name_src;    // ��� ���� �� ���������� ��

//private:
    bool required;      // ������� �������������� ������� ��������������� ���� � ���������
    bool linked;

};



//---------------------------------------------------------------------------
// �������
// TStorageTable
//---------------------------------------------------------------------------
class TStorageTable {
public:
    TStorageTable();
    virtual ~TStorageTable() {};
    bool Truncate;
    int retry_count;
    int retry_interval;
};


//---------------------------------------------------------------------------
// ���������
// class TStorage
//---------------------------------------------------------------------------

class TStorage
{
public:
    friend TStorageField;

    TStorage();
    virtual ~TStorage() {};

    bool linkSource(TStorage* Storage);
    TStorageField* findField(AnsiString fieldName);

    virtual void openTable(bool ReadOnly = true) {};
    virtual Variant getFieldValue(TStorageField* Field) {};
    virtual void setFieldValue(Variant Value) {};
    virtual void commit() {};
    virtual void append();   // ��������� ������ ������ ����� (������������ � DBase)
    virtual void post() {};     // ��������� ������ (������������ � OraProc)
    virtual void rollback();     // ��������� ������ (������������ � OraProc)

    virtual void closeTable();

    virtual bool eot();     // End Of Tables
    virtual bool eor() {};     // End Of Records
    virtual bool eof();     // End Of Fields

    virtual void nextTable();
    virtual void nextRecord();
    virtual void nextField();

    virtual TStorageField* getField();   // ���������� ��� ��������������� ���� � ���������
    bool isActiveField();
    bool isLinkedField();
    bool isActiveTable() { return Active; };
    bool isModified() { return Modified; };
    virtual int getRecordCount(){ return RecordCount; };
    virtual int getRecordIndex(){ return RecordIndex; };

    // �������������� �������
    virtual AnsiString getTable() = 0;     // ���������� ������������ ��������� ��������� ������
    virtual AnsiString getTableStage();     // ���������� ������� ���� ��������� ������
    virtual AnsiString getRecordStage();     // ���������� ������� ���� ��������� ������

    virtual TStorageField* addField(TStorageField* Field) {return NULL;};
    virtual void loadFieldDefs() {};    // ��������� �������� �����

    // ������ ����������� �����
    // ������������ ������� ���������������
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
    bool Active;    // ������� ����, ��� ��������/�������� ������ � ����� � ����������/������ ������
    bool ReadOnly;
    bool Modified;

    std::vector<TStorageField*> Fields;

    TStorage* templateStorage;    // ������ (������� ������ ��������� �������)
    bool delTemplateStorage;

    TLogger* Logger; // ��������� 2016-10-25 ��� ������ � ��� ������� ����������. �� ���������.


};


//---------------------------------------------------------------------------
#endif
