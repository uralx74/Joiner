#include "pch.h"
#pragma hdrstop

#include "StorageExcel.h"


//---------------------------------------------------------------------------
//  TStorageExcel
//---------------------------------------------------------------------------
TStorageExcel::~TStorageExcel()
{
    closeTable();
}

//---------------------------------------------------------------------------
// Добавляет таблицу в список (имя файла, может быть задано маской)
void TStorageExcel::addTable(const TExcelTable& Table)
{
    TSearchRec SearchRec;
    FindFirst(Table.File, faAnyFile, SearchRec);

    if (SearchRec.Name != "")
    {     // Если файл найден, то формируем новый элемент-Таблицу
        AnsiString FilePath = ExtractFilePath(Table.File);
        do
        {
            TExcelTable NewTable(Table);
            NewTable.File = FilePath + SearchRec.Name;
            Tables.push_back(NewTable);
            TableCount++;
        } while ( FindNext(SearchRec) == 0);
    }
    else
    {    // Если файл не найден, добавляем таблицу как есть
        Tables.push_back(Table);
        TableCount++;
    }
    FindClose(SearchRec);
}

//---------------------------------------------------------------------------
// Открытие таблицы
void TStorageExcel::openTable(bool ReadOnly)
{
    this->ReadOnly = ReadOnly;
    if (Tables[TableIndex].TitleRowIndex == 0)
    {
        Tables[TableIndex].TitleRowIndex = 1;
    }

    // Продумать случаи открытия - создание пустого файла!!!!
    msexcel = new MSExcelWorks();
    if (ReadOnly)
    {
        if (FileExists(Tables[TableIndex].File))
        {
            try
            {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument(Tables[TableIndex].File);
                Worksheet = msexcel->GetSheet(Workbook, Tables[TableIndex].WorksheetIndex);
            }
            catch (Exception &e)
            {
                throw Exception("Can't to open file  \"" + Tables[TableIndex].File + "\".\n" + e.Message);
            }
        }
        else
        {
            throw Exception("File not found \"" + Tables[TableIndex].File + "\".");
        }
    } else
    {  // Если открываем как приемник, то в любом случае создаем новый файл
        if ( FileExists(Tables[TableIndex].File) )
        {
            try
            {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument(Tables[TableIndex].File);
            }
            catch (Exception &e)
            {
                closeTable();
                throw Exception("Can't to open file  \"" + Tables[TableIndex].File + "\".\n" + e.Message);
            }
            if ( msexcel->IsReadOnly(Workbook) )
            {
                closeTable();
                throw Exception("Can't to open file  \"" + Tables[TableIndex].File + "\" for writing.");
            }
            Worksheet = msexcel->GetSheet(Workbook, 1);

            for(int i = 0; i < Fields.size(); i++)
            {
                AnsiString sCellValue = msexcel->ReadCell(Worksheet, Tables[TableIndex].TitleRowIndex, i+1);
                if (Fields[i]->name != sCellValue)
                {
                    closeTable();
                    throw Exception("Field names \"" + Fields[i]->name + "\" and \"" + sCellValue +"\" do not match.");
                }
            }
            if (Tables[TableIndex].Truncate)
            {
                //msexcel->SetVisibleExcel();
                msexcel->ClearWorksheet(Worksheet);
                // Создаем "структуру" таблицы (шапку)
                for(int i = 0; i < Fields.size(); i++)
                {
                    msexcel->WriteToCell(Worksheet, Fields[i]->name, Tables[TableIndex].TitleRowIndex, i+1, "@");
                }
            }
        }
        else
        {
            try
            {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument();
                Worksheet = msexcel->GetSheet(Workbook, 1);

                // Создаем "структуру" таблицы (шапку)
                for(int i = 0; i < Fields.size(); i++)
                {
                    msexcel->WriteToCell(Worksheet, Fields[i]->name, Tables[TableIndex].TitleRowIndex, i+1, "@");
                }
                msexcel->SaveDocument(Workbook, Tables[TableIndex].File);
                //Modified = true;
            }
            catch (Exception &e)
            {
                throw Exception("Can't to create file  \"" + Tables[TableIndex].File + "\".\n" + e.Message);
            }
        }
    }

    // Подсчет столбцов
    if (ReadOnly)
    {
        int j = 1;
        String sCellValue;
        while ( (sCellValue = msexcel->ReadCell(Worksheet, Tables[TableIndex].TitleRowIndex, j)) != "" )
        {
            TExcelField* field = this->addField();
            if ( field != NULL )
            {
                field->name = LowerCase(sCellValue);
                field->index = j;
                j++;
            }
            //field->active = true;
            //field->enable = true;
            //Fields.push_back(field);
            //FieldsList[LowerCase(sCellValue)] = j;
            //j++;
        }
    }

    FieldCount = Fields.size();

    // Подсчет строк
    Tables[TableIndex].FirstDataRowIndex = Tables[TableIndex].TitleRowIndex + 1;
    if (Tables[TableIndex].LastDataRowIndex <= 0)
    {
        RecordCount = 0;
        int FirstDataRowIndex = Tables[TableIndex].TitleRowIndex+1;
        int i = FirstDataRowIndex;
        String sCellValue;
        while ( (sCellValue = msexcel->ReadCell(Worksheet, i, 1)) != "" )
        {
                RecordCount++;
                i++;
        }
        Tables[TableIndex].LastDataRowIndex = Tables[TableIndex].FirstDataRowIndex + RecordCount-1;
    }
    else
    {
        RecordCount = Tables[TableIndex].LastDataRowIndex-Tables[TableIndex].TitleRowIndex;
    }

    Active = true;
}

//TExcelField::TExcelField(AnsiString Name, int Index)
//{
 //   TExcelField* field = new TExcelField();
//}

//---------------------------------------------------------------------------
// Закрывает таблицу
void TStorageExcel::closeTable()
{
    if (msexcel != NULL)
    {
        Active = false;
        msexcel->CloseApplication();
        delete msexcel;
        msexcel = NULL;

        // В последующем обернуть эти переменные
        Workbook = Unassigned;   // эквивалентно VarClear(WorkBooks)
        Worksheet = Unassigned;   // эквивалентно VarClear(WorkBooks)
    }
    TStorage::closeTable();
}

//---------------------------------------------------------------------------
//
bool TStorageExcel::eor()
{
    return RecordIndex >= RecordCount;
}

//---------------------------------------------------------------------------
// Возвращает наименование активного источника/приемника данных
AnsiString TStorageExcel::getTable()
{
    if ( !eot() )
    {
        return Tables[TableIndex].File;
    }
}

//---------------------------------------------------------------------------
// Возвращает значение поля
Variant TStorageExcel::getFieldValue(TStorageField* Field)
{
    AnsiString fieldName = LowerCase(Field->name_src);

    //TStorageField* field = FindField(Field->name_src);

    TExcelField* field = static_cast<TExcelField*>(findField(Field->name_src));

    if (field != NULL)
    {
        return msexcel->ReadCell(Worksheet, RecordIndex + Tables[TableIndex].FirstDataRowIndex, field->index);
    }
    else
    {
        throw Exception("Field not found " + fieldName + ".");
    }

    /*if (FieldsList.find(fieldName) != FieldsList.end() ) {
        return msexcel->ReadCell(Worksheet, RecordIndex+2, FieldsList[fieldName]);
    } else {
        throw Exception("Field not found " + fieldName + ".");
    }*/
}

//---------------------------------------------------------------------------
// Добавляет новую запись в таблицу
void TStorageExcel::append()
{
    RecordIndex = RecordCount;
    TStorage::append();
}

//---------------------------------------------------------------------------
// Устанавливает значение активного поля
void TStorageExcel::setFieldValue(Variant Value)
{
    if (Fields[FieldIndex]->active && Fields[FieldIndex]->enable)
    {
        msexcel->WriteToCell(Worksheet, Value, RecordIndex+Tables[TableIndex].FirstDataRowIndex, FieldIndex+1, ((TExcelField*)Fields[FieldIndex])->format);
    }
}

//---------------------------------------------------------------------------
// Добавляет новое поле в список полей
TExcelField* TStorageExcel::addField()
{
    TExcelField* Field = new TExcelField();
    if (Field != NULL)
    {
        Fields.push_back(dynamic_cast<TStorageField*>(Field));
        FieldCount++;
    }
    return Field;
}

//---------------------------------------------------------------------------
// Фиксирует изменения (сохраняет файл)
void TStorageExcel::commit()
{
    //Здесь сделать 
    if ( ReadOnly )
    {
        throw Exception("Can't commit the storage because it is read-only.");
        //return;
    }

    msexcel->SaveDocument(Workbook);
    Modified = true;
}

//---------------------------------------------------------------------------
// Копирование полей из источника
// Используется двойная диспетчеризация
TStorageExcel::copyFieldsFrom(TStorage* storage)
{
    storage->copyFieldsToExcel(this);
}

//---------------------------------------------------------------------------
// Полное копирование полей из Excel в Excel
TStorageExcel::copyFieldsToExcel(TStorage* storage)
{
    TStorage::fullCopyFields(this, storage);
}
