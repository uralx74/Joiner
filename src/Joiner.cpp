//---------------------------------------------------------------------------

#ifndef _DEBUG 
#define DEBUG_MESSAGE
#endif  
#ifdef _DEBUG  
#define DEBUG_MESSAGE(msg) OutputDebugString(msg) 
#endif


#include "pch.h"
#pragma hdrstop

#include "CommandLine.h"
#include "Messages.h"
#include "Logger.h"
#include "..\util\appver.h"
#include "TransferModule.h"

USEFORM("Unit1.cpp", Form1);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
   //TCommandLine commandline;
    TCommandLine* cl = &TCommandLine::getInstance();
    TLogger* Logger = &TLogger::getInstance();

    // Устанавливаем Лог-файл, если задан в параметрах
    AnsiString sLogFileName = cl->GetValue("-log","-l");
    if (sLogFileName != "") {
        try {
            sLogFileName = ExpandFileName(sLogFileName);
            Logger->AddConsole(sLogFileName, cl->GetFlag("-logrewrite","-lr")); // Если GetFlag, то лог-файл перезаписывается
        } catch (...) {
        }
    }


    Logger->WriteLog("---------------------------------------------------------------------------" );
    Logger->WriteLog("Joiner " + AppVer::FullVersion);
    Logger->WriteLog("AppFileName = " + Application->ExeName);
    Logger->WriteLog("LogFileName = " + Logger->GetLogFilename(sLogFileName));

    Logger->WriteLog("The program is started." );
    //Logger->WriteLog("Полный путь " + Application->ExeName + " .");
    // Выводить путь
    // имя компьютера
    // имя пользователя
    // Используемые ключи командной строки
    // путь к Лог-файлу



    // Выводим справку о программе, если требуется
    if (cl->GetFlag("-help","-h"))
    {
        MessageBoxInf(
            "Using comand-line:"
            "\nJoiner [-c=file_path] [-du=my_user1 -dp=my_password1] [-su=my_user2 -sp=my_password2] "
            "[-h]"

            "\n"
            "\nOptions:"
            "\n-h | -help\tShow this help"
            "\n-i | -info\tShow info about author"
            "\n"
            "\n-c | -config = <file_path>\tSpecify path to the xml config file"
            "\n"
            "\n-su | -srcuser\tSource Database Username in open text"
            "\n-sp | -srcpassword\tSource Database Password in open text"
            "\n-du | -dstuser\tDestination Database Username in open text"
            "\n-dp | -dstpassword\tDestination Database Password in open text"
            "\n-gp | -getpassword\tShow encoded value of password"
            "\n"
            "\n-ac | -accesscode\tОShow encoded value of username and password"
            "\n-acp | -accesscodepersonal\tShow encoded value of username and password linked to OS user  (not realised)"
            "\n-acw | -accesscodeworkstation\tShow encoded value of username and password linked to personal workstation (not realised)"
            "\n"
            "\n-s | -silent\tSilent mode"
            "\n-a | -auto\tAuto start execute (not needs with -s/-silent option)"
            "\n-ae | -autoexit\tAuto exit after execute (not needs with -s/-silent option)"
            "\n-l | -log = <file_path>\tSpecify path to the log file"
            "\n-lr | -logrewrite \tRewrite log file"
        );
        return 0;
    }

    // Выводим информацию об авторе, если требуется
    if (cl->GetFlag("-info","-i"))
    {
        MessageBoxInf(
            "Program for merging files\nJoiner v." + AppVer::Version + " (" + AppVer::Build + ")"
            "\n"
            "\nCopyright © 2014-2018"
            "\n"
            "\nWelcome to GitHub repository of this project: https://github.com/uralx74/Joiner"
            "\n"
            "\nAuthor:"
            "\nVictor Ovchinnikov"
            "\n"
            "\n e-mail: x74@list.ru"
            "\n skype: uralx74"
            "\n icq: 217-056-065"
        );
        return 0;
    }

    // Выполняем в тихом режиме, если задан ключ
    if (cl->GetFlag("-silent","-s"))
    {
        CoInitialize(NULL);     // COM libruary initialize. Надо для работы Word и Excel
        TTransferModule TransferModule;
        TransferModule.Start();
        CoUninitialize();
        return 0;
    }

    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TForm1), &Form1);
         Application->Run();
        Logger->WriteLog("The program is closed.");
    }
    catch (Exception &exception)
    {
        Logger->WriteLog("The program is aborted with error." + exception.Message);
        Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
