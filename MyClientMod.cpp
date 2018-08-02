//---------------------------------------------------------------------------


#pragma hdrstop

#include "MyClientMod.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)



// GLOBAL FUNCS
//------------------------------------------------------------------------------
AnsiString CodeString(AnsiString str, int nCode)
{
    int k, nLen = str.Length();
    for(k=0; k<nLen; k++)
        str.c_str()[k] = str.c_str()[k]^nCode;
return str;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------





// ErrorMess
//------------------------------------------------------------------------------
AnsiString ErrorMess::IdToStr(int nID)
{
    AnsiString sMess;
    switch(nID)
    {
        case PASS_FAILED:           sMess = "Неверный пароль!"; break;
        case MYSQL_SYNT_FAILED:     sMess = "Неверный синтаксис команды!"; break;
        case TIME_OUT:              sMess = "Время обработки запроса истекло!"; break;
        case TRY_LATER:             sMess = "Сервер занят, попробуете позже..."; break;
        case REQ_FORMAT_FAILED:     sMess = "Неверный формат запроса!"; break;
        case COMMAND_ACCEPTED:      sMess = "Команда принята!"; break;
        case UNKNOWN_COMMAND:       sMess = "Неизвестная команда!"; break;
        case TRANSMIT_SUCCESS:      sMess = "Передача успешно завершена!"; break;
        case MYSQL_COMMAND_ACCEPTED:sMess = "MySQL команда принята!"; break;
        case INVALID_REQUEST_ID:    sMess = "Неверный номер запроса!"; break;
        case FINISH_ACCEPTED:       sMess = "Завершение передачи подтверждено!"; break;


        default:                sMess = IntToStr(nID); break;
        // еще вариант 7, означает что дальше следует сообщение
    }
return sMess;
}
//------------------------------------------------------------------------------
AnsiString ErrorMess::Message(int nID, AnsiString sMess)
{
    if(sMess != "")
        return "m" + DIVIDER_1 + IntToStr(CUSTOM_MESSAGE) + DIVIDER_1 + sMess;
    else
        return "m" + DIVIDER_1 + IntToStr(nID);
}
//------------------------------------------------------------------------------






// MyClientMod
//------------------------------------------------------------------------------
MyClientMod::MyClientMod(TIdTCPClient *iCli, TStringGrid *iSG, TMemo *iMemo)
{
    Cli = iCli;
    SG = iSG;
    Memo = iMemo;

    sIP = Cli->Host;
    nCode = 0;
}
//------------------------------------------------------------------------------
// Private funcs
//------------------------------------------------------------------------------
void MyClientMod::ClearSG()
{
    for(int i = 0; i < SG->RowCount; i++)
        SG->Rows[i]->Clear();
    SG->RowCount = 2;
    SG->ColCount = 2;
    SG->ColWidths[1] = 20;// Ширина первой колонки. Нужна здесь на случай если управляющий поток не пойдет в RecieveTable()
}
//------------------------------------------------------------------------------
void MyClientMod::Log(AnsiString sMess)
{
    if(Memo)
        Memo->Lines->Add(sMess);
}
//------------------------------------------------------------------------------
bool MyClientMod::GetIsConnectionOK()
{
    if(sIP != "")
        Cli->Host = sIP;

    // Проверяем подключение
    try
    {
        Cli->Connect();
    }
    catch(Exception &exception)
    {
        Log(exception.Message);
    }
return Cli->Connected();
}
//------------------------------------------------------------------------------
bool MyClientMod::TakeCodeFromServ()
{
    AnsiString sReply = SendAndRecieve("15974398791235875", false);
    sReply = GetStringLeft(sReply, sReply.Length()-5);
    sReply = GetStringRight(sReply, sReply.Length()-5);

    try
    {
        nCode = StrToInt(sReply);
    }
    catch(...)
    {
        Log(sReply + " is not int!");
        nCode = 0;
    }
return nCode;
}
//------------------------------------------------------------------------------
int MyClientMod::GetErrors(AnsiString sMessage)
{
    StringsContainer strCont;
    strCont.BreakString(sMessage, DIVIDER_1);
    if(strCont.Vector[0] != "m")
        return 0;


    // Произошла какая-то ошибка, либо пришло сообщение после которого уже не нужно ничего делать
    Cli->Disconnect();
    Log("Disconnect.");


    int nErrNumber;
    try
    {
        nErrNumber = StrToInt(strCont.Vector[1]);
    }
    catch(...)
    {
        Log("Некорректный номер ошибки: " + strCont.Vector[1]);
        return CUSTOM_MESSAGE;
    }

    if(nErrNumber == CUSTOM_MESSAGE)
    {
        if(strCont.Vector.size() == 3)
            Log(strCont.Vector[2]);
        else
            Log("Текст ошибки отсутствует!");
    }
    else
        Log(ErrorMess().IdToStr(nErrNumber));
return nErrNumber;
}
//------------------------------------------------------------------------------
// Public funcs
//------------------------------------------------------------------------------
void MyClientMod::Send(AnsiString sCommand, bool bNeedCode)
{
    //Log("Send: " + sCommand);
    if(bNeedCode)
        sCommand = CodeString(sCommand, nCode);
    Cli->Write(sCommand);
}
//------------------------------------------------------------------------------
AnsiString MyClientMod::Recieve(bool bNeedCode)
{
    AnsiString sReply = Cli->CurrentReadBuffer();
    if(bNeedCode)
        sReply = CodeString(sReply, nCode);
    //Log("Recieve: " + sReply);
return sReply;
}
//------------------------------------------------------------------------------
AnsiString MyClientMod::SendAndRecieve(AnsiString sMessage, bool bNeedCode)
{
    Send(sMessage, bNeedCode);
    return Recieve(bNeedCode);
}
//------------------------------------------------------------------------------
void MyClientMod::SendCommand(AnsiString sCommand, AnsiString sBase)
{
    ClearSG();
    if(Loggin == "" || Pass == "")
    {
        Log("Нет логина или пароля!");
        return;
    }

    if(GetIsConnectionOK())
    {
        Log("Connection... Success!");
        if(nCode || TakeCodeFromServ())
        {
            sCommand = Loggin + DIVIDER_2 + Pass + DIVIDER_1 + sCommand + DIVIDER_2 + sBase;
            // Ждем первый ответ, должен содержать размеры таблицы + ID запроса
            AnsiString sReply = SendAndRecieve(sCommand);
            if(GetErrors(sReply))
                return;

            StringsContainer strCont;
            strCont.BreakString(sReply, DIVIDER_1);
            if(strCont.Vector.size()<3)
            {
                Log("Некорректный ответ сервера: " + sReply);
                return;
            }


            int nRowCount, nColumnsCount;
            try
            {
                nRowCount = StrToInt(strCont.Vector[0]);
                nColumnsCount = StrToInt(strCont.Vector[1]);
            }
            catch(...)
            {
                Log("Некорректные индексы таблицы: " + sReply);
                return;
            }
            if(strCont.Vector[2] == "")
            {
                Log("Номер запроса пуст: " + sReply);
                return;
            }

            //Строк и столбцов должнобыть на 1 больше чем ответов.
            nRowCount++;
            nColumnsCount++;
            if(nRowCount<2) nRowCount = 2;
            if(nColumnsCount<2) nColumnsCount = 2;
            SG->RowCount = nRowCount;
            SG->ColCount = nColumnsCount;
            
            RecieveTable(strCont.Vector[2]);
        }
    }
    else
    {
        Log("Connection... Failed.");
        exit(0);
    }
}
//------------------------------------------------------------------------------
void MyClientMod::RecieveTable(AnsiString sRequestID)
{
    int nY = 0;
    int nX = 0;
    AnsiString sRequest, sReply;

    Log("Загрузка данных с сервера...");
    while(true)
    {
        sRequest = sRequestID + DIVIDER_1 + IntToStr(nY) + DIVIDER_1 + IntToStr(nX);
        sReply = SendAndRecieve(sRequest);

        // Проверяем на ошибки и тут же пишем лог
        if(GetErrors(sReply))
            return;

        // Типа все ок, пишем инфу в таблицу
        nX++;
        if(nY == 0) // При первом проходе обнуляем ширину колонки
            SG->ColWidths[nX] = 0;

        SG->Cells[nX][nY] = sReply;
        CheckFieldWidth(sReply, nX);

        if(nX >= SG->ColCount-1)
        {
            nX = 0;
            nY++;
            if(nY > SG->RowCount-1) //Значит получена уже вся информация
            {
                // sRequestID + DIVIDER_1 + "-1" + DIVIDER_1 + "-1"; // Формат чтобы сервер удалил запрос из своей базы
                nY = -1;
                nX = -1;
            }
        }
    }
}
//------------------------------------------------------------------------------
void MyClientMod::CheckFieldWidth(AnsiString sValue, int nX)
{
    int nWidth = 8 * sValue.Length() + 12;
    if(SG->ColWidths[nX]<nWidth)
        SG->ColWidths[nX] = nWidth;
}
//------------------------------------------------------------------------------
