//---------------------------------------------------------------------------

#ifndef MyDBH
#define MyDBH
//---------------------------------------------------------------------------
#include <ADODB.hpp>
#include <DBGrids.hpp>
#include <IdTCPServer.hpp>



/*
struct DBResStruct
{
    TADOQuery *Que;
    AnsiString sError;
};*/




class QueryManager
{
    public:
    TADOQuery *Que;

    // Constr
    QueryManager(){Que=0;}
    QueryManager(TADOQuery *iQue){Que = iQue;}

    // Funcs
    AnsiString GetName(int nX);
    AnsiString GetFieldValue(int nY = 1, int nX = 0);
};
//------------------------------------------------------------------------------
class MyDBResult
{
    public:
    QueryManager QMan;
    AnsiString sError;
    //AnsiString sNewBase;

    MyDBResult(){QueryManager();}
    MyDBResult(TADOQuery *iQue, AnsiString isError = "", AnsiString isNewBase = "");
};
//------------------------------------------------------------------------------
class MyDB
{
    // Priv Funcs
    bool GetIsUseBaseCommand(AnsiString sCommand);

    public:
    // Components
    TADOConnection *Connection;
    TADOQuery *Que;
    AnsiString LastCommand;  //Я не помню нахрен это мне было нужно, но пока оставлю
    AnsiString LastBase; //Для переподключения

    //Constructor
    MyDB(TADOConnection *Connection = 0, TADOQuery *iQue = 0);


    // Temp for debug
    TMemo *Memo;
    void Log(const AnsiString &sMess){Memo->Lines->Add(sMess);}





    // Funcs
    //void SetConnectionFile(AnsiString sPath);
    //void Connect();
    void Reconnect();
    bool GetIsNoReplyCommand(AnsiString sCommand);
    MyDBResult ExecuteCommand(AnsiString sCommand);
    MyDBResult ExecuteCommandInBase(AnsiString sCommand, AnsiString sBase = "");
    //AnsiString GetFieldValue(int nY=1, int nX=0);
    //AnsiString GetAutoIncrement(AnsiString sTable);
    //void SaveLogToFile(AnsiString sMess, bool AddTime = true);
    void CheckForNewBase();


    //Temp
    //void InsertIntoStates(AnsiString sState, int nPop);
    //void Log(AnsiString sMess, bool AddTime = true);
};
//-----------------------------------------------------------------------------


#endif
