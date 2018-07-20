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
    String Table;       // ��� ������� (������������ ���� ����� �������������� ��������)
    String Server;
    String Username;
    String Password;
};


// ��������� ��� �������� ����������
class TOraProcTable : public TStorageTableOra
{
public:
    String Procedure;   // ��� ���������
};

// ��������� ��� �������� ����������
class TOraSqlTable : public TStorageTableOra
{
public:
    String Sql;     // ��� ����� � Sql-��������
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
    void executeProc(const String& procName);      // ��������� ��� initProc � finalProc
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
    void commit();
    void nextField();
    virtual TOraField* addField();
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
    virtual TOraField* addField();
    void addTable(const TOraSqlTable &Table);
    AnsiString getTable();

private:
    std::vector<TOraSqlTable> Tables;    // ������ ����� ��� ������� � ���� DBF
};







//---------------------------------------------------------------------------
#endif
