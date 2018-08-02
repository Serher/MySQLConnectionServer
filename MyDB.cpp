//---------------------------------------------------------------------------


#pragma hdrstop

#include "MyDB.h"
#include "MyUtils.h"
#include "Unit1.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)



/* Функция для чтения инфы с сервера и корректной записи в StringGrid
Пример использования:
    AnsiString sMess = Edit1->Text;
    IdTCPClient1->Write(sMess); // Отправляем MySQL-комманду
    RecieveReplyToGrid(Form1);  // Ждем ответа



void RecieveReplyToGrid(TForm1 *Form1, int nPart = 0)
{
    AnsiString str = Form1->IdTCPClient1->CurrentReadBuffer();

    if(str == "Сервер занят")
    {
        Form1->Memo1->Lines->Add(str);
        return;
    }
    nPart++;

    if(nPart == 1)//Принимаем Y-размер таблицы
    {
        Form1->SG->RowCount = StrToInt(str);
        Form1->IdTCPClient1->Write("+");
        RecieveReplyToGrid(Form1, nPart);
    }
    else if(nPart == 2)//Принимаем X-размер таблицы
    {
        Form1->SG->ColCount = StrToInt(str);
        Form1->IdTCPClient1->Write("+");
        RecieveReplyToGrid(Form1, nPart);
    }
    else //Принимаем основную инфу для таблицы
    {
        int nY = (nPart-3) / Form1->SG->ColCount;
        int nX = (nPart-3) % Form1->SG->ColCount;
        Form1->SG->Cells[nX][nY] = str;


        if( ((nY+1) < Form1->SG->RowCount)   ||   ((nX+1) < Form1->SG->ColCount) )
        {
            Form1->IdTCPClient1->Write("+");
            RecieveReplyToGrid(Form1, nPart);
        }
        else // вся таблица заполнена
        {
            //nPart = 0;
            Form1->Memo1->Lines->Add("Finish");
        }
    }
}
*/






// QueryManager
//------------------------------------------------------------------------------
AnsiString QueryManager::GetName(int nX)
{
    return Utf8ToAnsi(Que->Fields->Fields[nX]->DisplayName);
}
//------------------------------------------------------------------------------
AnsiString QueryManager::GetFieldValue(int nY, int nX)
{
    if(nX>=Que->FieldCount)
        nX = Que->FieldCount - 1;
    if(nX < 0)
        nX = 0;

    if(nY > Que->RecordCount)
        nY = Que->RecordCount;
    if(nY==0)
        return GetName(nX); // возвращаем имя колонки
    if(nY<0)
        nY = 1;

    Que->RecNo = nY;
return Utf8ToAnsi(Que->Fields->Fields[nX]->AsString);
}
//------------------------------------------------------------------------------





// MyDBResult
//------------------------------------------------------------------------------
MyDBResult::MyDBResult(TADOQuery *iQue, AnsiString isError, AnsiString isNewBase)
{
    QMan.Que = iQue;
    sError = isError;
    //sNewBase = isNewBase;
}
//------------------------------------------------------------------------------







// MyDB
//------------------------------------------------------------------------------
MyDB::MyDB(TADOConnection *iConnection, TADOQuery *iQue)
{
    Connection = iConnection;
    Que = iQue;
    if(Connection && Que)
        Que->Connection = Connection;

    Memo = 0;
}
//------------------------------------------------------------------------------
/*void MyDB::SetConnectionFile(AnsiString sPath)
{
    sPath = "FILE NAME=" + sPath;
    WideString WS;
    WS.SetLength(sPath.Length());

    for(int i = 1; i <= sPath.Length(); i++)
    {
        WS[i] = sPath[i];
    }
    Connection->ConnectionString = WS;
}
//------------------------------------------------------------------------------
void MyDB::Connect()
{
    Connection->Connected = true;
    //Log("Connection OK");
}*/
//------------------------------------------------------------------------------
/*void MyDB::CheckFieldWidth(AnsiString sValue, int nX)
{
    int nWidth = 8 * sValue.Length() + 12;
    if(SG->ColWidths[nX]<nWidth)
        SG->ColWidths[nX] = nWidth;
}
//------------------------------------------------------------------------------
void MyDB::ShowOnGrid()
{
    AnsiString sValue;
    int nColumsCount = Que->FieldCount+1;
    int nRowsCount = Que->RecordCount+1;

    SG->ColCount = (nColumsCount>1) ? nColumsCount : 2;
    SG->RowCount = (nRowsCount>1) ? nRowsCount : 2;
    SG->Rows[1]->Clear();

    for(int nY = 1; nY<=Que->RecordCount; nY++)
    {
        Que->RecNo = nY;
        for(int nX = 1; nX<=Que->FieldCount; nX++)
        {
            if(nY==1)
            {
                SG->ColWidths[nX] = 0;
                sValue = Utf8ToAnsi(Que->Fields->Fields[nX-1]->DisplayName);
                SG->Cells[nX][0] = sValue;
                CheckFieldWidth(sValue, nX);
            }
            sValue = Utf8ToAnsi(Que->Fields->Fields[nX-1]->AsString); //Utf8ToAnsi(Que->Fields->Fields[nX-1]->Text);
            SG->Cells[nX][nY] = sValue;
            CheckFieldWidth(sValue, nX);
        }
    }
}
//------------------------------------------------------------------------------
void MyDB::ClearGrid()
{
    for(int i=0; i<SG->RowCount; i++)
    {
        SG->Rows[0]->Clear();
        SG->Rows[1]->Clear();
        SG->ColCount = 2;
        SG->RowCount = 2;
        SG->ColWidths[1] = 64;
    }
}
//------------------------------------------------------------------------------
bool MyDB::NotNeedInLog(AnsiString sCommand)
{
    if(sCommand.AnsiPos("select off from info"))
        return true;
    if(sCommand.AnsiPos("select date, news from news order by id desc"))
        return true;
return false;
}*/
//------------------------------------------------------------------------------
void MyDB::Reconnect()
{
    Connection->Close();
    Connection->Open();
    ExecuteCommand("Use " + LastBase);
    //ExecuteCommand("set names utf8");
}
//------------------------------------------------------------------------------
MyDBResult MyDB::ExecuteCommand(AnsiString sCommand)
{
    //Log("[MyDB] " + sCommand);
    AnsiString sResult;
    LastCommand = sCommand;

    Que->Close();
    Que->SQL->Clear();
    Que->SQL->Text = AnsiToUtf8(sCommand);

    try
    {
        if(GetIsNoReplyCommand(sCommand))
        {
            Que->ExecSQL();
            CheckForNewBase();
        }
        else
            Que->Open();
    }
    catch(Exception &exception)
    {
        if(exception.Message.AnsiPos("You have an error in your SQL syntax"))
            sResult = "Неверный систаксис";
        else if(exception.Message.AnsiPos("Table") && exception.Message.AnsiPos("doesn't exist"))
            sResult = "В выбранной базе нет указанной таблицы!";
        else if(exception.Message.AnsiPos("MySQL server has gone away"))
        {
            Log("MySQL сервер разорвал соединение. Переподключение");
            Reconnect();
            return ExecuteCommand(sCommand);
        }
        else
            sResult = exception.Message;
    }
    catch(...)
    {
        sResult = "Какая-то куета";
    }
return MyDBResult(Que, sResult);
}
//------------------------------------------------------------------------------
MyDBResult MyDB::ExecuteCommandInBase(AnsiString sCommand, AnsiString sBase)
{
    MyDBResult dbRes;
    if(sBase != "" && LastBase != sBase && !GetIsNoReplyCommand(sCommand))
        dbRes = ExecuteCommand("use " + sBase);
    if(dbRes.sError == "")
        dbRes = ExecuteCommand(sCommand);
return dbRes;
}
//------------------------------------------------------------------------------
/*AnsiString MyDB::GetFieldValue(int nY, int nX)
{
    if(nY > Que->RecordCount)
        nY = Que->RecordCount;
    else if(nY<=0)
        nY = 1;

    if(nX>=Que->FieldCount)
        nX = Que->FieldCount - 1;
    if(nX < 0)
        nX = 0;
    
    Que->RecNo = nY;

    //Log("GetFieldValue: nY = " + IntToStr(nY) + ", nX = " + IntToStr(nX));
    //Log("GetFieldValue Result: " + Utf8ToAnsi(Que->Fields->Fields[nX]->Text));
return Utf8ToAnsi(Que->Fields->Fields[nX]->AsString);
}*/
//------------------------------------------------------------------------------
/*AnsiString MyDB::GetAutoIncrement(AnsiString sTable)
{
    ExecuteCommand("show table status like '" + sTable + "'");
    return GetFieldValue(1, 10);
}*/
//------------------------------------------------------------------------------
bool MyDB::GetIsUseBaseCommand(AnsiString sCommand)
{
    if(sCommand.LowerCase().SubString(1, 4) == "use ")
        return true;
return false;
}
//------------------------------------------------------------------------------
void MyDB::CheckForNewBase()
{
    if(GetIsUseBaseCommand(LastCommand))
    {
        StringsContainer strCont;
        strCont.BreakString(LastCommand, " ");
        if(strCont.Vector.size() == 2)
            LastBase = strCont.Vector[1];
    }
}
//------------------------------------------------------------------------------
bool MyDB::GetIsNoReplyCommand(AnsiString sCommand)
{
    AnsiString sSub = sCommand.LowerCase().SubString(1, 4);
    if(sSub == "use " || sSub == "set ")
        return true;

    sSub = sCommand.LowerCase().SubString(1, 7);
    if(sSub == "insert " || sSub == "delete " || sSub == "create " || sSub == "update ")
        return true;

    if(sCommand.LowerCase().SubString(1, 6) == "alter ")
        return true;
    if(sCommand.LowerCase().SubString(1, 5) == "drop ")
        return true;

return false;
}
