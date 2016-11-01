//---------------------------------------------------------------------------
#ifndef StorageOraH
#define StorageOraH

#include <vector>
#include <map>
#include "Ora.hpp"
#include "DBAccess.hpp"
#include "MemDS.hpp"
#include "Storage.h"


// ��������� ��� �������� ���������� ����� � Oracle (���������/�������)
class TOraField : public TStorageField {
};

class TStorageTableOra : public TStorageTable
{
public:
    AnsiString Table;       // ��� ������� (������������ ���� ����� �������������� ��������)
    AnsiString Server;
    AnsiString Username;
    AnsiString Password;
};


// ��������� ��� �������� ����������
class TOraProcTable : public TStorageTableOra
{
public:
    AnsiString Procedure;   // ��� ���������
};

// ��������� ��� �������� ����������
class TOraSqlTable : public TStorageTableOra
{
public:
    AnsiString Sql;     // ��� ����� � Sql-��������
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
    void prepareQuery();



};

//---------------------------------------------------------------------------
// class TStorageOraProc: TStorage
// ����� ������� ��������� ������
// ������������ ��������� � �� Oracle
//---------------------------------------------------------------------------
class TStorageOraProc: public TStorageOra
{
public:
    void openTable(bool ReadOnly = true);
    void setFieldValue(Variant Value);
    void post();
    void nextField();
    TOraField* addField();
    void addTable(const TOraProcTable& Table);
    AnsiString getTable();
private:
    std::vector<TOraProcTable> Tables;    // ������ ������ (��������)
};



//---------------------------------------------------------------------------
// class TStorageOraSql: TStorage
// ����� ������� ���������/��������� ������
// ������������ ������� � �� Oracle
//---------------------------------------------------------------------------
class TStorageOraSql: public TStorageOra
{
public:
    //TStorageOraSql() {};
    void openTable(bool ReadOnly = true);
    //Variant Get(AnsiString Field);      // ���������� �������� �� ����� ����
    Variant getFieldValue(TStorageField* Field);
    //void Set(Variant Value);      // ������������� �������� ��������� ����
    void setFieldValue(Variant Value);      // ������������� �������� ��������� ����
    void append();
    void commit();
    void post();
    bool eor();     // End Of Records
    void nextRecord();
    TOraField* addField();
    void addTable(const TOraSqlTable &Table);
    AnsiString getTable();

private:
    std::vector<TOraSqlTable> Tables;    // ������ ����� ��� ������� � ���� DBF
};







//---------------------------------------------------------------------------
#endif
