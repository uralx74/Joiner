#include "pch.h"
#pragma hdrstop

#include "Storage.h"
#include <assert.h>

#define NDEBUG



TStorageField::TStorageField():
    active(true)
{
}

//---------------------------------------------------------------------------
// Закрытие хранилища
void TStorage::closeTable()
{
    std::vector<String>::size_type n = Fields.size();
    for (int i = 0; i < n; i++) {   // Удаление полей
        delete Fields[i];
    }
    Fields.clear();
    FieldIndex = 0;
    FieldCount = 0;
    RecordIndex = 0;
    RecordCount = 0;
    Active = false;
    //TableIndex = 0;
    //TableCount = 0;
}

//---------------------------------------------------------------------------
// Добавление пустой записи
void TStorage::append()
{
    RecordCount++;
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// Откат
void TStorage::rollback()
{
    RecordCount--;
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// Проверка является ли поле ассоциированными с полем в источнике
bool TStorage::isLinkedField()
{
    return Fields[FieldIndex]->linked;
}

//---------------------------------------------------------------------------
// Ассоциирует поля с полями в источнике
// Если поле найдено, устанавливает флаг linked = true
bool TStorage::linkSource(TStorage* Storage)
{
    // Storage is Source storage
    int n = Fields.size();
    for (int i = 0; i < n; i++) {
        //String sss = this->Fields[i]->name_src;
        //bool k = Storage->FindField(this->Fields[i]->name_src);
        this->Fields[i]->linked = Storage->findField(Fields[i]->name_src) != NULL;
    }
}

//---------------------------------------------------------------------------
// Поиск поля по имени
TStorageField* TStorage::findField(AnsiString fieldName)
{
    int n = Fields.size();
    for (int i = 0; i < n; i++) {
        if (UpperCase(Fields[i]->name) == UpperCase(fieldName)) {
            return Fields[i];
        }
    }
    return NULL;
}

/*//---------------------------------------------------------------------------
//
bool TStorage::FindField(AnsiString fieldName)
{
//int tt = FieldCount;
    int n = Fields.size();
    for (int i = 0; i < n; i++) {
        if (Fields[i]->name == fieldName) {
            return true;
        }
    }
    return false;
} */


//---------------------------------------------------------------------------
// Возвращает соответствующее активному полю - имя поля в таблице-источнике
TStorageField* TStorage::getField()
{
    return Fields[FieldIndex];
    //return Fields[FieldIndex]->name_src;
}

//---------------------------------------------------------------------------
// Возвращает true - если поле поле подлежит записи
bool TStorage::isActiveField()
{
    return Fields[FieldIndex]->active && Fields[FieldIndex]->enable;
}


//---------------------------------------------------------------------------
TStorageTable::TStorageTable() :
    Truncate(false),
    retry_count(1),
    retry_interval(10)
{

}

//---------------------------------------------------------------------------
// Задает templateStorage в качестве источника структуры
// Используется когда Хранилище представляет приемник
TStorage::setTemplate(TStorage* storage, bool deleteAfterUse)
{
    templateStorage = storage;
    delTemplateStorage = deleteAfterUse;
}


//---------------------------------------------------------------------------
// Тестирование 2
TStorage::copyFieldsFrom(TStorage* storage)
{
    // Конвертация
}

//
TStorage::copyFieldsToDbf(TStorage* storage)
{
    // Конвертация
    Modified = true;
}

//
TStorage::copyFieldsToExcel(TStorage* storage)
{
    // Конвертация
    bool Modified = true;
}



//---------------------------------------------------------------------------
// Полное копирование полей
// Вызывается только для источников одинакового типа!
void TStorage::fullCopyFields(TStorage* src, TStorage* dst)
{
    // Копируем из src в dst
    int n = src->Fields.size();
    for (int i = 0; i < n; i++) {
        dst->addField(src->Fields[i]);
    }
}




//---------------------------------------------------------------------------
// TStorage
// The interface class
//---------------------------------------------------------------------------

TStorage::TStorage() :
    TableIndex(0),
    TableCount(0),
    FieldCount(0),
    FieldIndex(0),
    RecordCount(0),
    RecordIndex(0),
    Active(false),
    ReadOnly(true),
    Modified(false),
    templateStorage(NULL),
    delTemplateStorage(false)

{
    Logger = &TLogger::getInstance();
}

//---------------------------------------------------------------------------
// Возвращает true если достигнут конец списка таблиц
bool TStorage::eot()
{
    return TableIndex >= TableCount;
}

//---------------------------------------------------------------------------
// Возвращает true если достигнут конец списка полей
bool TStorage::eof()
{
    return FieldIndex  >= FieldCount;
}

//---------------------------------------------------------------------------
// Переходит к следующей таблице
void TStorage::nextTable()
{
    ////////////////////// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if (Active) {
        this->closeTable();
    }

    if (!eot()) {
        TableIndex ++;
        FieldIndex = 0;
        RecordIndex = 0;    // 2016-08-08
    } else {
        throw Exception("");
    }

/*    if (!eot()) {
        this->openTable();
    }     */
}

//---------------------------------------------------------------------------
// Переходит к следующей записи таблицы
void TStorage::nextRecord()
{
    if (!eor()) {
        FieldIndex = 0;
        RecordIndex++;
    }
}

//---------------------------------------------------------------------------
// Переходит к следующему полю таблицы
void TStorage::nextField()
{
    if (!eof()) {
        FieldIndex++;
    }
}

//---------------------------------------------------------------------------
//
AnsiString TStorage::getTableStage()
{
    return IntToStr(TableIndex+1) + "/" + IntToStr(TableCount);
}

//---------------------------------------------------------------------------
//
AnsiString TStorage::getRecordStage()
{
    return IntToStr(RecordIndex+1) + "/" + IntToStr(RecordCount);
}





