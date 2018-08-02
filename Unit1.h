//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ADODB.hpp>
#include <DB.hpp>
#include <Dialogs.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPServer.hpp>


//#include "MyUtils.h"
#include "MyDB.h"
#include "MyClientMod.h"
#include <ComCtrls.hpp>






//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------





class IDGen
{
    int Postfix;

    public:
    IDGen(){Postfix = 0;}
    AnsiString GenarateID();
};
//------------------------------------------------------------------------------
class ClientRequest
{
    public:
    AnsiString ID;
    QueryManager QMan;

    // Const/Dest -ructor
    ClientRequest(AnsiString isID = "");

    AnsiString GetFieldValue(int nY, int nX);
};
//------------------------------------------------------------------------------
class RequestManager
{
    public:
    std::vector<ClientRequest> Reqs;

    // First request
    AnsiString HandleFirstRequest(AnsiString sCommand, TIdPeerThread *AThread);
    AnsiString AddNewRequest(ClientRequest req, TIdPeerThread *AThread);
    // Secondary request
    AnsiString HandleSecondaryRequest(AnsiString sID, int nY, int nX);
    ClientRequest *GetReqWithID(AnsiString sID);
    void DeleteRequest(AnsiString sID);
};
//------------------------------------------------------------------------------
class AccountChecker
{
    public:
    StringsContainer Accs;

    bool Check(AnsiString str);
};
//------------------------------------------------------------------------------
class ServerControl
{
    TForm *Form; 

    // priv funcs
    AnsiString GetCommand(AnsiString sCommand);
    AnsiString ExecuteCommand(AnsiString sCommand);
    public:

    ServerControl(){Form = 0;}
    ServerControl(TForm *iForm){Form = iForm;}
    
    // Funcs
    AnsiString Execute(AnsiString str);
};
//------------------------------------------------------------------------------
class Secretary
{
    public:
    bool GetIsControlCommand(AnsiString sCommand);
    void SendCode(TIdPeerThread *AThread);
    void Send(TIdPeerThread *AThread, AnsiString str, bool bNeedCode = true);
    void AnalizeRequest(TIdPeerThread *AThread, AnsiString sRequest);
    void Log(AnsiString sMess);
};
//------------------------------------------------------------------------------









//------------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TSpeedButton *SpeedButton1;
    TLabeledEdit *LE1;
    TButton *Button2;
    TLabel *Label2;
    TCheckBox *ChB1;
    TADOConnection *ADOConnection1;
    TADOQuery *ADOQuery1;
    TOpenDialog *OD1;
    TIdTCPServer *IdTCPServer1;
    TMemo *Memo1;
    TLabel *Label1;
    TLabeledEdit *LabeledEdit1;
    TLabeledEdit *LE2;
    TEdit *Ed1;
    TLabel *Label3;
    TLabel *Label4;
    TListBox *LB1;
    TListBox *LB2;
    TTimer *DumpTimer;
    void __fastcall SpeedButton1Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall ChB1Click(TObject *Sender);
    void __fastcall IdTCPServer1Execute(TIdPeerThread *AThread);
    void __fastcall DumpTimerTimer(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);

    // Funcs
    void Log(AnsiString sMessage);
    void SaveDump();

    // Components
    IDGen IDGen1;
    AccountChecker acc1;
    MyDB MyDB1;
    RequestManager rqMan1;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
 