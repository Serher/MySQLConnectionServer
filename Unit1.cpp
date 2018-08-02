//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    MyDB1 = MyDB(ADOConnection1);
    MyDB1.Memo = Memo1;

    //MyDB1.Connection = ADOConnection1;
    //ExecuteCommand("set names utf8");



    //LoadConfig(Form1);
    //MyDB1.SaveLogToFile("");

    //StringsContainer strCont;
    //strCont.BreakString(Label4->Caption, ",");
    //MyDB1.SaveLogToFile("Server started!" + strCont.Vector[2]);
    Log("Server started!");


    if(LE2->Text == "")
        LE2->Text = IntToStr(IdTCPServer1->DefaultPort);

     



    /*
    if(ChB1->Checked)//Autoconnect
    {
        if(LE1->Text != "")
        {
            MyDB1.SetConnectionFile(LE1->Text);
            MyDB1.Connect();
            MyDB1.ExecuteCommand("set names utf8");

            if(LB1->Caption != "None")
                MyDB1.ExecuteCommand("use " + LB1->Caption);
        }
    }
    ColNames = false;
    DumpTimer->Enabled = true;
    */
}
//------------------------------------------------------------------------------


// GLOBAL FUNCS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------






// IDGen
//------------------------------------------------------------------------------
AnsiString IDGen::GenarateID()
{
    AnsiString sResult;
    if((++Postfix)>1000)
        Postfix = 0;

    sResult = Now().TimeString()+IntToStr(Postfix);
    Form1->Log("Присвоен ID: " + sResult);
    Form1->LB2->Items->Add(sResult);
    return sResult;
}
//------------------------------------------------------------------------------






// ClientRequest
//------------------------------------------------------------------------------
ClientRequest::ClientRequest(AnsiString isID)
{
    ID = isID;
    //Form1->Log("Create ClientRequest: " + ID);
    QMan.Que = new TADOQuery(Form1);
}
//------------------------------------------------------------------------------
AnsiString ClientRequest::GetFieldValue(int nY, int nX)
{
    return QMan.GetFieldValue(nY, nX);
}
//------------------------------------------------------------------------------






// RequestManager
//------------------------------------------------------------------------------
AnsiString RequestManager::HandleFirstRequest(AnsiString sCommand, TIdPeerThread *AThread)
{
    AnsiString sBase;
    StringsContainer strCont;
    // Проверяем запрашиваемую базу
    strCont.BreakString(sCommand, DIVIDER_2);
    sCommand = strCont.Vector[0];
    if(strCont.Vector.size()>1)
        sBase = strCont.Vector[1];
    else if(sCommand.LowerCase() != "show databases")
        return ErrorMess().Message(REQ_FORMAT_FAILED);

    // Создать запрос
    ClientRequest crNew;
    Form1->MyDB1.Que = crNew.QMan.Que;
    Form1->MyDB1.Que->Connection = Form1->MyDB1.Connection;
    // Выполняет команду запроса
    Form1->Log("<" + AThread->Connection->Binding->PeerIP + "> Command: " + sCommand);
    Form1->Log("Base: " + sBase);
    MyDBResult dbRes = Form1->MyDB1.ExecuteCommandInBase(sCommand, sBase);
    if(dbRes.sError == "")
        return AddNewRequest(crNew, AThread);

    // Какая-то ошибка
    delete crNew.QMan.Que;
    return ErrorMess().Message(CUSTOM_MESSAGE, dbRes.sError);
}
//------------------------------------------------------------------------------
AnsiString RequestManager::AddNewRequest(ClientRequest req, TIdPeerThread *AThread)
{
    if(!req.QMan.Que->IsEmpty())
    {
        int nY = req.QMan.Que->RecordCount;
        int nX = req.QMan.Que->FieldCount;                                      
        if(nY>0 && nX>0)
        {
            // генерим номер
            req.ID = Form1->IDGen1.GenarateID();
            // добавляем запрос в вектор
            Reqs.push_back(req);
            // Возвращаем размеры таблицы и ID
            return IntToStr(nY) + DIVIDER_1 + IntToStr(nX) + DIVIDER_1 + req.ID;
        }
    }

    // если таблица нулевая
    //req.~ClientRequest();
    delete req.QMan.Que;
    return ErrorMess().Message(MYSQL_COMMAND_ACCEPTED);
}
//------------------------------------------------------------------------------
AnsiString RequestManager::HandleSecondaryRequest(AnsiString sID, int nY, int nX)
{
    if(nY == -1 && nX == -1)// Передача завершена, удаляем запрос
    {
        DeleteRequest(sID);
        return ErrorMess().Message(FINISH_ACCEPTED);
    }

    // Передаем следующую часть
    ClientRequest *req = GetReqWithID(sID);
    if(req)
    {
        AnsiString sResult = req->GetFieldValue(nY, nX);
        if(sResult == "")
            sResult = " ";
        return sResult;
    }
return ""; // Какая-то ошибка
}
//------------------------------------------------------------------------------
ClientRequest *RequestManager::GetReqWithID(AnsiString sID)
{
    ClientRequest *crRes = 0;
    for(unsigned i=0; i<Reqs.size(); i++)
    {
        if(Reqs[i].ID == sID)
            crRes = &Reqs[i];
    }
return crRes;
}
//------------------------------------------------------------------------------
void RequestManager::DeleteRequest(AnsiString sID)
{
    int nID;
    for(unsigned i=0; i<Reqs.size(); i++)
    {
        if(Reqs[i].ID == sID)
        {
            Form1->LB2->Items->Delete(Form1->LB2->Items->IndexOf(sID));
            Form1->Log("Delete Request: " + sID);
            delete Reqs[i].QMan.Que;
            Reqs.erase(Reqs.begin()+i);
            return;
        }
    }
}
//------------------------------------------------------------------------------







// AccountChecker
//------------------------------------------------------------------------------
bool AccountChecker::Check(AnsiString str)
{
    if(Accs.IndexOf(str)>=0)
        return true;

    StringsContainer strCont;
    strCont.BreakString(str, DIVIDER_2);
    if(strCont.Vector.size() != 2)
        return false;


    bool bRes = false; // Нужна для возможности выполнить delete
    ::CoInitialize(NULL);
    TADOQuery* Que = new TADOQuery(Form1);
    Que->Connection = Form1->MyDB1.Connection;
    Form1->MyDB1.Que = Que;
    AnsiString sCommand = "select * from main where acc like '" + strCont.Vector[0] + "' && pass like '" + strCont.Vector[1] + "'";
    MyDBResult dbRes = Form1->MyDB1.ExecuteCommandInBase(sCommand, "accs");

    if(dbRes.QMan.Que->RecordCount)
    {
        Accs.Vector.push_back(str);
        Form1->LB1->Items->Add(strCont.Vector[0]);
        bRes = true;
    }
    delete Que;

    //Secretary().Log("bRes: " + IntToStr(bRes));
    ::CoUninitialize();
return bRes;
}
//------------------------------------------------------------------------------






// ServerControl
//------------------------------------------------------------------------------
AnsiString ServerControl::GetCommand(AnsiString sCommand)
{
    if(sCommand.Length()<9)
        return "";

    // Должно прийти что-то вроде "Control>>HideForm|-2|"
    StringsContainer strCont;
    strCont.BreakString(sCommand, DIVIDER_2); // убираем разделитель в конце
    sCommand = strCont.Vector[0];

return sCommand.Delete(1, 9); // Удаляем "Control>>"
}
//------------------------------------------------------------------------------
AnsiString ServerControl::ExecuteCommand(AnsiString sCommand)
{
    if(sCommand == "HideForm")
        Form1->Visible = false;
    else if(sCommand == "ShowForm")
        Form1->Visible = true;
    else
        return "Неизвестная команда: " + sCommand;
return "";
}
//------------------------------------------------------------------------------
AnsiString ServerControl::Execute(AnsiString str)
{
    AnsiString sResult = "Некорректный формат!";
    str = GetCommand(str);
    if(str != "")
        sResult = ExecuteCommand(str);
return sResult;
}
//------------------------------------------------------------------------------






// Secretary
//------------------------------------------------------------------------------
bool Secretary::GetIsControlCommand(AnsiString sCommand)
{
    if(!sCommand.AnsiPos("Control>>"))
        return false;
return true;
}
//------------------------------------------------------------------------------
void Secretary::SendCode(TIdPeerThread *AThread)
{
    Send(AThread, Form1->Ed1->Text, false);
}
//------------------------------------------------------------------------------
void Secretary::Send(TIdPeerThread *AThread, AnsiString str, bool bNeedCode)
{
    // Тут при необходимости можно вставить логирование
    if(bNeedCode)
        str = CodeString(str, StrToInt(Form1->Ed1->Text));
    AThread->Connection->Write(str);
}
//------------------------------------------------------------------------------
void Secretary::AnalizeRequest(TIdPeerThread *AThread, AnsiString sRequest)
{
    //Log("Recieved 1: " + sRequest);
    if(sRequest == "15974398791235875") // запрос на получание кода
    {
        Send(AThread, "84579" + Form1->Ed1->Text + "34952", false); // Клиент отбросит 5 первых и 5 последних цифр
        return;
    }

    AnsiString sWorkResult; // Сделал на всякий, пока наверное для логов
    sRequest = CodeString(sRequest, StrToInt(Form1->Ed1->Text));
    //Log("Recieved: " + sRequest);
    StringsContainer strCont;
    strCont.BreakString(sRequest, DIVIDER_1);
    if(strCont.Vector.size()<2 || strCont.Vector.size()>3) // пришел какой-то мусор
        sWorkResult = ErrorMess().Message(REQ_FORMAT_FAILED);
    else if(strCont.Vector.size() == 2) // Команда или Первичный запрос? // Лого||пасс + Команда
    {
        // Проверить логин и пасс
        if(!Form1->acc1.Check(strCont.Vector[0]))
            sWorkResult = ErrorMess().Message(PASS_FAILED);
        else
        {   // Проверяем команду
            if(GetIsControlCommand(strCont.Vector[1]))
            {
                sWorkResult = ServerControl().Execute(strCont.Vector[1]);
                if(sWorkResult != "")
                {
                    Log(sWorkResult);
                    sWorkResult = ErrorMess().Message(UNKNOWN_COMMAND);
                }
                else
                    sWorkResult = ErrorMess().Message(COMMAND_ACCEPTED);
            }
            else // Request Manager обрабатывает первичный запрос и возвращает то что надо послать клиенту
                sWorkResult = Form1->rqMan1.HandleFirstRequest(strCont.Vector[1], AThread);
        }
    }
    else if(strCont.Vector.size() == 3) // Вторичный запрос? // ID-||-nY-||-nX
    {
        int nY, nX;
        try
        {
            nY = StrToInt(strCont.Vector[1]);
            nX = StrToInt(strCont.Vector[2]);
        }
        catch(...)
        {
            Send(AThread, ErrorMess().Message(REQ_FORMAT_FAILED));
            return;
        }
        // Обрабатываем вторичный запрос // Если все ок, sWorkResult будет значением поля таблицы
        sWorkResult = Form1->rqMan1.HandleSecondaryRequest(strCont.Vector[0], nY, nX);
        if(sWorkResult == "")
            sWorkResult = ErrorMess().Message(INVALID_REQUEST_ID);
        /*else if(sWorkResult != ErrorMess().Message(FINISH_ACCEPTED))
            sWorkResult = strCont.Vector[1] + DIVIDER_1 + strCont.Vector[2] + DIVIDER_1 + sWorkResult;*/
    }
    //Log("sWorkResult: " + sWorkResult);
    Send(AThread, sWorkResult);
}
//------------------------------------------------------------------------------
void Secretary::Log(AnsiString sMess)
{
    Form1->Log(sMess);
}
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------






// Form1
//------------------------------------------------------------------------------
void TForm1::Log(AnsiString sMessage)
{
    sMessage = "[" + Now().DateTimeString() + "] " + sMessage;
    sMessage += "\n";
    Memo1->Lines->Add(sMessage);
    
    AnsiString sFileName = FileFuncs().GetCurDir()+ "\\Logs\\Log.txt";
    std::ofstream F;
    F.open(sFileName.c_str(), std::ios_base::app);
    if(F)
    {
        F<<sMessage.c_str();
        F.close();
    }
}
//---------------------------------------------------------------------------
void TForm1::SaveDump()
{
    AnsiString sFileName, sDate = Now().DateString();
    StringsContainer strCont;
    strCont.BreakString(sDate, ".");

    if(strCont.Vector.size() == 3)
        sFileName = strCont.Vector[2] + "_" + strCont.Vector[1] + "_" + strCont.Vector[0] + ".sql"; // 2017_07_24.sql

    AnsiString sPath = "C:\\Program Files\\MySQL\\MySQL Server 5.7\\bin\\mysqldump.exe";
    AnsiString sParams = "-uroot -pkjubytrc -rD:\\C++\\DUMPS\\" + sFileName + " tvtask";
    ShellExecute(0, 0, sPath.c_str(), sParams.c_str(), 0, SW_NORMAL);

    Log("Сохранение дампа. Имя файла: " + sFileName);
}
//------------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
    if(OD1->Execute())
    {
        LE1->Text = OD1->FileName;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
    if(LE1->Text == "")
    {
        SpeedButton1Click(Form1);
        return;
    }
    /*
    //Задать файл подключения
    MyDB1.SetConnectionFile(LE1->Text);
    MyDB1.Connect();
    MyDB1.ExecuteCommand("set names utf8");
    Configger Conf(Memo2);
    Conf.SetValue("[LAST_UDL_FILE]", LE1->Text);

    if(LB1->Caption != "None")
        MyDB1.ExecuteCommand("use " + LB1->Caption);
    */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ChB1Click(TObject *Sender)
{
    //Configger Conf(Memo2);
    //Conf.SetValue("[Autoconnect]", IntToStr(ChB1->Checked));
}
//---------------------------------------------------------------------------
void __fastcall TForm1::IdTCPServer1Execute(TIdPeerThread *AThread)
{
    Secretary sec;
    sec.AnalizeRequest(AThread, AThread->Connection->CurrentReadBuffer());
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DumpTimerTimer(TObject *Sender)
{
    AnsiString sTime = Now().TimeString();
    if(sTime.AnsiPos("9:00:"))//Будет писать в 9:00 и в 19:00
        SaveDump();
}
//---------------------------------------------------------------------------

