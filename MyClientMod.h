

//---------------------------------------------------------------------------
#ifndef MyClientModH
#define MyClientModH


#include "MyUtils.h"


#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <Grids.hpp>

//---------------------------------------------------------------------------

//using namespace MyUtils;



const int MYSQL_SYNT_FAILED = 1;
const int TIME_OUT          = 2;
const int TRY_LATER         = 3;
const int REQ_FORMAT_FAILED = 4;
const int COMMAND_ACCEPTED  = 5;
const int UNKNOWN_COMMAND   = 6;
const int CUSTOM_MESSAGE    = 7;
const int TRANSMIT_SUCCESS  = 8;
const int MYSQL_COMMAND_ACCEPTED = 9;
const int INVALID_REQUEST_ID = 10;
const int PASS_FAILED       = 11;
const int FINISH_ACCEPTED   = 12;



const AnsiString DIVIDER_1 = "|-1|";
const AnsiString DIVIDER_2 = "|-2|";






// GLOBAL FUNCS
AnsiString CodeString(AnsiString str, int nCode);





//------------------------------------------------------------------------------
class ErrorMess
{
    public:
    AnsiString IdToStr(int nID);
    AnsiString Message(int nID, AnsiString sMess = "");
};
//------------------------------------------------------------------------------
class MyClientMod
{
    public:
    TIdTCPClient *Cli;
    TStringGrid *SG;
    TMemo *Memo; // Для логов

    AnsiString sIP;  // Это сервер к которому коннектимся
    AnsiString Loggin;
    AnsiString Pass;
    int nCode;

    // Constructors
    MyClientMod(){Cli = 0; SG = 0; Memo = 0; nCode = 0;}
    MyClientMod(TIdTCPClient *iCli, TStringGrid *iSG, TMemo *iMemo);


    //Funcs
    private:
    void ClearSG();
    void Log(AnsiString sMess);
    bool GetIsConnectionOK();
    bool TakeCodeFromServ();
    int GetErrors(AnsiString sMessage);
    void CheckFieldWidth(AnsiString sValue, int nX);
    void Send(AnsiString sCommand, bool bNeedCode = true);
    AnsiString Recieve(bool bNeedCode = true);
    AnsiString SendAndRecieve(AnsiString sMessage, bool bNeedCode = true);


    public:
    void SendCommand(AnsiString sCommand, AnsiString sBase);
    void RecieveTable(AnsiString sRequestID);
};

#endif
