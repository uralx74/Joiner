#include "pch.h"
#pragma hdrstop
//---------------------------------------------------------------------------

#include "XmlParamsLoader.h"

//---------------------------------------------------------------------------
//
TXmlLoader::TXmlLoader()
{
    Logger = &TLogger::getInstance();
    CommandLine = &TCommandLine::getInstance();

    SrcStor = NULL;
    DstStor = NULL;

}

//---------------------------------------------------------------------------
//
TXmlLoader::~TXmlLoader()
{
    if (SrcStor != NULL) {
        delete SrcStor;
    }
    if (DstStor != NULL)
    {
        delete DstStor;
    }
}

AnsiString TXmlLoader::ExpandFileNameCustom(AnsiString FileName, AnsiString FilePath)
{
    AnsiString result = ExpandFileName(FileName);
    if (UpperCase(result) != UpperCase(Trim(FileName))) {
        result = ExtractFilePath(FilePath) + Trim(FileName);
    } else {
        result = FileName;
    }
    return result;

}

//---------------------------------------------------------------------------
// Загружает параметры из командной строки
// и файла конфигурации
bool __fastcall TXmlLoader::LoadParameters()
{

// !!!!!!!!!!!!
// Возможно вместо ExpandFilename лучше использовать путь от xml-файла конфигурации.


    // Параметры командной строки
    AnsiString clConfig;    // Путь к файлу конфигурации
    AnsiString clConfigPath;    // Путь к файлу конфигурации

    AnsiString clSrcUsername;  // Имя пользователя базы данных
    AnsiString clSrcPassword;  // Пароль к базе данных
    AnsiString clDstUsername;  // Имя пользователя базы данных
    AnsiString clDstPassword;  // Пароль к базе данных

    clConfig = CommandLine->GetValue("-config","-c");
    clDstUsername = CommandLine->GetValue("-dstuser","-du");
    clDstPassword = CommandLine->GetValue("-dstpassword","-dp");
    clSrcUsername = CommandLine->GetValue("-srcuser","-su");
    clSrcPassword = CommandLine->GetValue("-srcpassword","-sp");
    //clLogfile = CommandLine->GetValue("-logfile","-l");
    //clSilent = CommandLine->GetValue("-silent","-s");

    // Преобразовываем относительный путь к файлу в абсолютный путь
    clConfig = ExpandFileName(clConfig);
    clConfigPath = ExtractFilePath(clConfig);

    int LogLine = Logger->WriteLog("Загрузка файла конфигурации > " + clConfig);

    if (!FileExists(clConfig)) {
        Logger->WriteLog("Ошибка. Файл конфигурации не найден " + clConfig);
        return false;
    }

    CoInitialize(NULL);
    OleXml msxml;
    msxml.LoadXMLFile(clConfig);

    if (msxml.GetParseError() != "") {
        Logger->WriteLog("Ошибка: " + msxml.GetParseError());
        return false;
    }

    Variant RootNode = msxml.GetRootNode();
    Variant node = msxml.GetFirstNode(RootNode);

    while (!node.IsEmpty())
    {
        String sNodeName = msxml.GetNodeName(node);
        if (sNodeName == "import")  {
            Variant subnode = msxml.GetFirstNode(node);
            AnsiString sImportType = msxml.GetNodeName(subnode);

            if (sImportType == "dbase4") {
                TStorageDbase* StorDbase = new TStorageDbase();
                SrcStor = (TStorage*) StorDbase;
                while (!subnode.IsEmpty()) {
                    if (msxml.GetNodeName(subnode) == "dbase4") {
                        TDbaseTable Table;
                        Table.File = ExpandFileNameCustom(msxml.GetAttributeValue(subnode, "file"), clConfigPath);  // Имя файла может быть задано маской
                        StorDbase->addTable(Table);
                    }
                    subnode = msxml.GetNextNode(subnode);
                }
            } else if (sImportType == "orasql" || sImportType == "oratable") {
                TStorageOraSql* StorOraSql = new TStorageOraSql();
                SrcStor = (TStorage*) StorOraSql;

                while (!subnode.IsEmpty()) {
                    if (msxml.GetNodeName(subnode) == "orasql" || sImportType == "oratable") {
                        TOraSqlTable Table;
                        Table.Server = msxml.GetAttributeValue(subnode, "server");
                        Table.retry_count = msxml.GetAttributeValue(subnode, "retry_count", 1);
                        Table.retry_interval = msxml.GetAttributeValue(subnode, "retry_interval", 10);

                        // В дальнейшем переделать. 1. Командная строка приоритетней; 2. Шифрование-дешифрование
                        // Тоже самое в OraProc и в приемник

                        // Если задано имя пользователя и пароль в командной строке
                        if (clSrcUsername != "" || clSrcPassword != "") {
                            Table.Username = clSrcUsername;
                            Table.Password = clSrcPassword;
                        } else {
                            AnsiString code = msxml.GetAttributeValue(subnode, "code");
                            if (code != "") {
                                TEncoder encoder;
                                try {
                                    encoder.Decode(code, Table.Username, Table.Password );
                                } catch (...) {
                                    Logger->WriteLog("Ошибка. Не удалось расшифровать пароль к источнику \"" + Table.Server + "\"");
                                    return false;
                                }
                            } else {    // Если задано не зашифрованное значение
                                Table.Username = msxml.GetAttributeValue(subnode, "username");
                                Table.Password = msxml.GetAttributeValue(subnode, "password", clSrcPassword);
                            }
                        }


                        if (sImportType == "orasql") {
                            //Table.Sql = ExpandFileName(msxml.GetAttributeValue(subnode, "sql"));
                            Table.Sql = ExpandFileNameCustom(msxml.GetAttributeValue(subnode, "sql"), clConfigPath);

                        }
                        Table.Table = msxml.GetAttributeValue(subnode, "table");
                        StorOraSql->addTable(Table);
                    }
                    subnode = msxml.GetNextNode(subnode);
                }
            } else if (sImportType == "excel") {
                TStorageExcel* StorageExcel = new TStorageExcel();
                SrcStor = (TStorage*) StorageExcel;

                while (!subnode.IsEmpty()) {
                    if (msxml.GetNodeName(subnode) == "excel") {
                        TExcelTable Table;
                        Table.File = ExpandFileNameCustom(msxml.GetAttributeValue(subnode, "file"), clConfigPath);  // Имя файла может быть задано маской
                        StorageExcel->addTable(Table);
                    }
                    subnode = msxml.GetNextNode(subnode);
                }
            }
        } else if (sNodeName == "export") {

            Variant subnode = msxml.GetFirstNode(node);
            AnsiString sExportType = msxml.GetNodeName(subnode);
            if (sExportType == "dbase4") {

                TStorageDbase* StorDbase = new TStorageDbase();
                DstStor = (TStorage*) StorDbase;
                //DstStor->SetReadOnly(false);

                TDbaseTable Table;
                Table.File = ExpandFileNameCustom(msxml.GetAttributeValue(subnode, "file"), clConfigPath);
                Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                Table.retry_count = msxml.GetAttributeValue(subnode, "retry_count", 1);
                Table.retry_interval = msxml.GetAttributeValue(subnode, "retry_interval", 10);
                StorDbase->addTable(Table);

                String xmlTemplate = msxml.GetAttributeValue(subnode, "template", "");
                bool xmlSourceAsTemplate = msxml.GetAttributeValue(subnode, "source_as_template", false);

                //StorDbase->AddTemplate(xmlTemplate);

                if (xmlTemplate != "") {    // Если задан путь к шаблону
                    try {
                        StorDbase->setTemplate(new TStorageDbase(ExpandFileNameCustom(xmlTemplate, clConfigPath)), true);
                    } catch (Exception &e) {
                        Logger->WriteLog("Ошибка. Не удалось загрузить шаблон из файла " + xmlTemplate + ".");
                        throw Exception("");
                    }
                } else if (xmlSourceAsTemplate) {
                    // нужно ли закрывать после использования?
                    // как получить список полей не меняя положение указателя на запись, таблицу, источник?
                    StorDbase->setTemplate(SrcStor);

                } else {

                    // Здесь возможно тоже создавать TStorage и добавлять в него поля ?
                    Variant node_fields = msxml.GetFirstNode(subnode);

                    while (!node_fields.IsEmpty()) {
                        if (msxml.GetNodeName(node_fields) == "field") {
                            // Возможно переделать на абстрактную фабрику или др.
                            // а также позаботиться об удалении обьекта
                            TDbaseField* dbaseField = StorDbase->addField();
                            if (dbaseField != NULL) {
                                // Создать функцию для установки значений
                                dbaseField->type = LowerCase(msxml.GetAttributeValue(node_fields, "type", "C"))[1];
                                dbaseField->name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                                dbaseField->length = msxml.GetAttributeValue(node_fields, "length", 0);
                                dbaseField->decimals = msxml.GetAttributeValue(node_fields, "decimals", 0);
                                dbaseField->active = msxml.GetAttributeValue(node_fields, "active", true);
                                dbaseField->enable = msxml.GetAttributeValue(node_fields, "enable", true);
                                dbaseField->name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", dbaseField->name));
                            }
                            //if (dbaseField->name_src == "" )
                            //    dbaseField->name_src = dbaseField->name;
                        }
                        node_fields = msxml.GetNextNode(node_fields);
                    }
                }

           } else if (sExportType == "oraproc") {   //
                TStorageOraProc* StorOraProc = new TStorageOraProc();
                DstStor = (TStorage*) StorOraProc;

                TOraProcTable Table;
                Table.Server = msxml.GetAttributeValue(subnode, "server");

                //!!! Фрагмент дублируется - необходим рефакторинг!
                // Если задано имя пользователя и пароль в командной строке
                if (clSrcUsername != "" || clSrcPassword != "") {
                    Table.Username = clSrcUsername;
                    Table.Password = clSrcPassword;
                } else {
                    AnsiString code = msxml.GetAttributeValue(subnode, "code");
                    if (code != "") {
                        TEncoder encoder;
                        try {
                            encoder.Decode(code, Table.Username, Table.Password );
                        } catch (...) {
                            Logger->WriteLog("Ошибка. Не удалось расшифровать пароль к источнику \"" + Table.Server + "\"");
                            return false;
                        }
                    } else {    // Если задано не зашифрованное значение
                        Table.Username = msxml.GetAttributeValue(subnode, "username");
                        Table.Password = msxml.GetAttributeValue(subnode, "password", clSrcPassword);
                    }
                }

                Table.Procedure = msxml.GetAttributeValue(subnode, "procedure");
                Table.Table = msxml.GetAttributeValue(subnode, "table");
                Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                Table.retry_count = msxml.GetAttributeValue(subnode, "retry_count", 1);
                Table.retry_interval = msxml.GetAttributeValue(subnode, "retry_interval", 10);
                StorOraProc->addTable(Table);


                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TOraField* field = StorOraProc->addField();   // AddField

                        if (field != NULL) {
                            field->name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                            field->active = msxml.GetAttributeValue(node_fields, "active", true);
                            field->enable = msxml.GetAttributeValue(node_fields, "enable", true);
                            field->name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field->name));
                            //field->name_src = field->name_src == "" ? "" : field->name;
                            //field->name_src = field->name;
                        }
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }

            } else if (sExportType == "oratable") {   // Приемник - Таблица в БД Oracle

                TStorageOraSql* StorOraTable = new TStorageOraSql();
                DstStor = (TStorage*) StorOraTable;
                //DstStor->SetReadOnly(false);

                TOraSqlTable Table;
                Table.Server = msxml.GetAttributeValue(subnode, "server");


                // Если задано имя пользователя и пароль в командной строке
                if (clSrcUsername != "" || clSrcPassword != "") {
                    Table.Username = clSrcUsername;
                    Table.Password = clSrcPassword;
                } else {
                    AnsiString code = msxml.GetAttributeValue(subnode, "code");
                    if (code != "") {
                        TEncoder encoder;
                        try {
                            encoder.Decode(code, Table.Username, Table.Password );
                        } catch (...) {
                            Logger->WriteLog("Ошибка. Не удалось расшифровать пароль к источнику \"" + Table.Server + "\"");
                            return false;
                        }
                    } else {    // Если задано не зашифрованное значение
                        Table.Username = msxml.GetAttributeValue(subnode, "username");
                        Table.Password = msxml.GetAttributeValue(subnode, "password", clSrcPassword);
                    }
                }

                Table.Sql = ""; // Важно!
                Table.Table = msxml.GetAttributeValue(subnode, "table");
                Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                Table.retry_count = msxml.GetAttributeValue(subnode, "retry_count", 1);
                Table.retry_interval = msxml.GetAttributeValue(subnode, "retry_interval", 10);
                StorOraTable->addTable(Table);

                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TOraField* field = StorOraTable->addField();      // Возможно заменить на
                        if (field != NULL) {
                            field->name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                            field->active = msxml.GetAttributeValue(node_fields, "active", true);
                            field->enable = msxml.GetAttributeValue(node_fields, "enable", true);
                            field->name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field->name));
                        }
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }
            } else if (sExportType == "excel") {

                TStorageExcel* StorageExcel = new TStorageExcel();
                DstStor = (TStorage*) StorageExcel;
                //DstStor->SetReadOnly(false);

                TExcelTable Table;
                Table.File = ExpandFileNameCustom(msxml.GetAttributeValue(subnode, "file"), clConfigPath);
                Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                Table.retry_count = msxml.GetAttributeValue(subnode, "retry_count", 1);
                Table.retry_interval = msxml.GetAttributeValue(subnode, "retry_interval", 10);
                StorageExcel->addTable(Table);

                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TExcelField* field = StorageExcel->addField();
                        if (field) {
                            field->name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                            field->active = msxml.GetAttributeValue(node_fields, "active", true);
                            field->enable = msxml.GetAttributeValue(node_fields, "enable", true);
                            field->format = msxml.GetAttributeValue(node_fields, "format", "");
                            field->name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field->name));
                            if (field->name_src == "" )
                                field->name_src = field->name;
                            }
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }
            } else if (sExportType == "sqltext") {

                TStorageSqlText* StorageSqlText = new TStorageSqlText();
                DstStor = (TStorage*) StorageSqlText;
                //DstStor->SetReadOnly(false);

                TSqlTextTable Table;
                Table.File = ExpandFileNameCustom(msxml.GetAttributeValue(subnode, "file"), clConfigPath);
                Table.Template = ExpandFileNameCustom(msxml.GetAttributeValue(subnode, "template"), clConfigPath);
                Table.retry_count = msxml.GetAttributeValue(subnode, "retry_count", 1);
                Table.retry_interval = msxml.GetAttributeValue(subnode, "retry_interval", 10);
                //Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                StorageSqlText->addTable(Table);

                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TSqlTextField* field = StorageSqlText->addField();
                        if (field) {
                            field->name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                            field->active = msxml.GetAttributeValue(node_fields, "active", true);
                            field->enable = msxml.GetAttributeValue(node_fields, "enable", true);
                            //field->format = msxml.GetAttributeValue(node_fields, "format", "");
                            field->name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field->name));
                            if (field->name_src == "" ) {
                                field->name_src = field->name;
                            }
                        }
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }
            }
        }
        node = msxml.GetNextNode(node);
    }

    Logger->WriteLog("Файл конфигурации загружен", LogLine);
    return true;

}            


