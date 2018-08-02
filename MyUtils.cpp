#define NO_WIN32_LEAN_AND_MEAN
#include <shlobj.h>
//---------------------------------------------------------------------------


#pragma hdrstop

#include "MyUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)



//------------------------------------------------------------------------------
void DBM(TMemo *Memo, AnsiString str1, AnsiString str2)
{
    Memo->Lines->LoadFromFile("D:\\C++\\CLog.txt");
    if(str2 != "")
        str1+= " || "+str2;
    Memo->Lines->Add(str1);
    Memo->Lines->SaveToFile("D:\\C++\\CLog.txt");
}
//------------------------------------------------------------------------------
void DBM(TMemo *Memo, AnsiString str1, int n)
{
    Memo->Lines->LoadFromFile("D:\\C++\\CLog.txt");
    if(str1 != "")
        str1+= " || "+IntToStr(n);
    else
        str1 = IntToStr(n);
    Memo->Lines->Add(str1);
    Memo->Lines->SaveToFile("D:\\C++\\CLog.txt");
}
//------------------------------------------------------------------------------
void NormalizeCB(TComboBox *ComboBox, bool bOne, bool bNoNew)
{
    int nIndex = ComboBox->ItemIndex;
    if(nIndex>-1 && ComboBox->Text!="")
    {
        ComboBox->ItemIndex = -1;
        ComboBox->ItemIndex = nIndex;
    }
    else if(bNoNew)
        ComboBox->Text = "";

    if(bOne)
    {
        if(ComboBox->Items->Count == 1)
            ComboBox->ItemIndex = 0;
    }
}
//------------------------------------------------------------------------------
void ReplaceLine(TMemo *Memo, int nLine, AnsiString str)
{
    Memo->Lines->Delete(nLine);
    Memo->Lines->Insert(nLine, str);
}
//------------------------------------------------------------------------------
void LeftDigitsOnly(AnsiString &str)
{
    AnsiString sTemp, sSymb;
    int nLen = str.Length();
    for(int i=1; i<=nLen; i++)
    {

        sSymb = str.SubString(i, 1);
        if(sSymb == "0" || sSymb == "1" || sSymb == "2" || sSymb == "3" || sSymb == "4" || sSymb == "5" || sSymb == "6" || sSymb == "7" || sSymb == "8" || sSymb == "9")
            sTemp += sSymb;
    }
    str = sTemp;
}
//------------------------------------------------------------------------------
bool GetIsFileExist(AnsiString sPath)
{
    std::fstream F;
    F.open(sPath.c_str());
    if(F)
    {
        F.close();
        return true;
    }
return false;
}
//------------------------------------------------------------------------------
bool GetIsDirectoryExist(AnsiString sPath)
{
    DWORD dwAttrib = ::GetFileAttributes(sPath.c_str());
    return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}
//------------------------------------------------------------------------------
void CopyTextToClipboard(AnsiString str)
{
    if(OpenClipboard(NULL))
    {
        HGLOBAL hgBuffer;
        char* chBuffer;
        EmptyClipboard();
        hgBuffer= GlobalAlloc(GMEM_DDESHARE, str.Length()+1);//???????? ??????
        chBuffer= (char*)GlobalLock(hgBuffer);
        strcpy(chBuffer, str.c_str());
        GlobalUnlock(hgBuffer);//???????????? ??????
        SetClipboardData(CF_TEXT, hgBuffer);//???????? ????? ? ????? ??????
        CloseClipboard();
    }
}
//------------------------------------------------------------------------------








//------------------------------------------------------------------------------
WideString AnsiToWide(AnsiString str)
{
    WideString WS;
    WS.SetLength(str.Length());
    for(int i = 1; i <= str.Length(); i++)
    {
        WS[i] = str[i];
    }
return WS;
}
//------------------------------------------------------------------------------
AnsiString WideToAnsi(WideString WS)
{
    AnsiString str;
    str.SetLength(WS.Length());
    for(int i = 1; i <= WS.Length(); i++)
    {
        str[i] = WS[i];
    }
return str;
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
AnsiString GetStringRight(AnsiString str, int nCount)
{
    return str.SubString(str.Length()-nCount+1,  nCount);
}
//------------------------------------------------------------------------------
AnsiString GetStringLeft(AnsiString str, int nCount)
{
    return str.SubString(1, nCount);
}
//------------------------------------------------------------------------------



















// FileFuncs

// Files
//------------------------------------------------------------------------------
bool FileFuncs::GetIsFileExist(AnsiString sPath)
{
    std::fstream F;
    F.open(sPath.c_str());
    if(F)
    {
        F.close();
        return true;
    }
return false;
}
//------------------------------------------------------------------------------
AnsiString FileFuncs::GetFileLastUpdateTime(AnsiString sPath, AnsiString sComposer)
{
    AnsiString sResult, sYear, sMonth, sDay;
    HANDLE fH;
    FILETIME updateTime;
    SYSTEMTIME sysTime;
    fH = CreateFile(sPath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(fH != INVALID_HANDLE_VALUE)
    {
        GetFileTime(fH, 0, 0, &updateTime);
        FileTimeToSystemTime(&updateTime, &sysTime);
        sYear = sysTime.wYear;
        sMonth = sysTime.wMonth;
        if(sMonth.Length()<2)
            sMonth = "0" + sMonth;
        sDay = sysTime.wDay;
        if(sDay.Length()<2)
            sDay = "0" + sDay;

        sResult = sYear + sComposer + sMonth + sComposer + sDay;
        CloseHandle(fH);
    }
return sResult;
}
//------------------------------------------------------------------------------
long FileFuncs::GetFileSize(AnsiString sPath)
{
    struct stat fi;
    stat(sPath.c_str(), &fi);
    return fi.st_size;
}
//------------------------------------------------------------------------------

// Directories
//------------------------------------------------------------------------------
bool FileFuncs::GetIsDirectoryExist(AnsiString sPath)
{
    DWORD dwAttrib = ::GetFileAttributes(sPath.c_str());
    return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}
//------------------------------------------------------------------------------
AnsiString FileFuncs::GetCurDir()
{
    /*
    TCHAR thBuffer[MAX_PATH];
    GetCurrentDirectory(sizeof(thBuffer),thBuffer);
    return AnsiString(thBuffer) + "\\";
    */

    TCHAR thBuffer[MAX_PATH];
    GetModuleFileName(NULL, thBuffer, MAX_PATH);
    AnsiString str = AnsiString(thBuffer);

    StringsContainer strCont;
    strCont.BreakString(str, "\\");
    str = "";
    for(unsigned i=0; i<strCont.Vector.size()-1; i++)
        str += strCont.Vector[i] + "\\";

return str;
}
//------------------------------------------------------------------------------
AnsiString FileFuncs::GetDesktopDir()
{
    AnsiString sDesktopDir;
    char stol[MAX_PATH];
    LPITEMIDLIST pidl;
    SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
    SHGetPathFromIDList(pidl, stol);
    sDesktopDir = stol;
return sDesktopDir;
}
//------------------------------------------------------------------------------

// Shortcuts
//------------------------------------------------------------------------------
void FileFuncs::CreateShortCut(const AnsiString &sFilePath, const AnsiString &sFileDir, const AnsiString &sShortcutDir, AnsiString sDescription)
{
    // IShellLink позволит нам создать ярлык.   
    // IPersistFile сохранит ярлык на жесткий диск С. 
    IShellLink* pLink;
    IPersistFile* pPersistFile;
 
    // Первым делом инициализируем COM библиотеку
    if(SUCCEEDED(CoInitialize(NULL)))
    {
        // Если инициализация прошла успешно, то получим
        // объект IShellLink вызвав CoCreateInstance.
        if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&pLink)))
        {
            // если все прошло успешно - заполним атрибуты ярлыка
            pLink->SetPath(sFilePath.c_str());
            pLink->SetWorkingDirectory(sFileDir.c_str());
            pLink->SetDescription(sDescription.c_str());
            pLink->SetShowCmd(SW_SHOW);

            // Теперь необходимо сохранить ярлык диске. Объект
            // IShellLink также содержит интерфейс IPersistFile.
            // Получим интерфейс IPersistFile вызвав QueryInterface.
            if(SUCCEEDED(pLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile)))
            {
                // Если все успешно, вызовем метод Save
                // объекта IPersistFile для сохранения/записи ярлыка.
                WideString strShortCutLocation(sShortcutDir + "\\ConnectionInfo.lnk");
                pPersistFile->Save(strShortCutLocation.c_bstr(), TRUE);
                pPersistFile->Release();
            }
            pLink->Release();
        } 
        // После вызова CoInitialize необходимо вызвать CoUninitialize 
        CoUninitialize();
    }
}

//http://www.cyberforum.ru/cpp-builder/thread638376.html
/*
void __fastcall TMain::Button17Click(TObject *Sender)
{
    AnsiString Dir;
    char stol[MAX_PATH];
    LPITEMIDLIST pidl;
    SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
    SHGetPathFromIDList(pidl, stol);
    Dir= stol;
    CreateLink(Edit1->Text ,"%WINDIR%" ,Dir+"\\Съемный диск "+Edit1->Text.SubString(1,1)+".lnk","Съемный диск","",SW_SHOWNORMAL);
}
*/

//------------------------------------------------------------------------------
void FileFuncs::CreateShortcutOnDesktop(const AnsiString &sFilePath, const AnsiString &sFileDir, AnsiString sDescription)
{
    CreateShortCut(sFilePath, sFileDir, GetDesktopDir(), sDescription);
}

//------------------------------------------------------------------------------











//StringsContainer
//------------------------------------------------------------------------------
unsigned int StringsContainer::BreakString(AnsiString str, AnsiString sDivider)
{
    Vector.clear();

    AnsiString sTemp;
    int nDivLen = sDivider.Length();
    int nPos = str.AnsiPos(sDivider);
    while(nPos)
    {
        sTemp = str.SubString(0, nPos-1);
        Vector.push_back(sTemp);
        str.Delete(1, nPos+nDivLen-1);
        
        nPos = str.AnsiPos(sDivider);
    }
    if(str != "")
        Vector.push_back(str);//Запихиваем в вектор последнюю часть(то что осталось, если что-то осталось)

return Vector.size();
}
//------------------------------------------------------------------------------
int StringsContainer::IndexOf(AnsiString sValue)
{
    int nResult = -1;
    for(unsigned int i = 0; i<Vector.size(); i++)
    {
        if(Vector[i] == sValue)
        {
            nResult = i;
            return nResult;
        }
    }
return nResult;
}





//For RE
//htmlTag
void htmlTag::ApplyTagToRE(TRichEdit *RE)
{
    RE->SelStart = nStart;
    RE->SelLength = nLength;

    if(nFormatType == 1)//size
        RE->SelAttributes->Size = nParam;
    else if(nFormatType == 2)//color
        RE->SelAttributes->Color = nParam;
    else if(nFormatType == 3)//bold
        RE->SelAttributes->Style = RE->SelAttributes->Style << fsBold;
    else if(nFormatType == 4)//Italic
        RE->SelAttributes->Style = RE->SelAttributes->Style << fsItalic;
    else if(nFormatType == 5)//Underline
        RE->SelAttributes->Style = RE->SelAttributes->Style << fsUnderline;
    else if(nFormatType == 6)//Strike Out
        RE->SelAttributes->Style = RE->SelAttributes->Style << fsStrikeOut;
}

//------------------------------------------------------------------------------
void htmlTag::FindParam(AnsiString sTag)
{
    int nTemp = AnsiPos("=", sTag);
    int nTemp2 = AnsiPos(", ", sTag);
    nParam = StrToInt(sTag.SubString(nTemp+1, nTemp2-nTemp-1));
}
//------------------------------------------------------------------------------
void htmlTag::FindLength(AnsiString sTag)
{
    int nTemp = AnsiPos("l=", sTag);
    nLength = StrToInt(sTag.SubString(nTemp+2, sTag.Length()-nTemp-1));
}
//------------------------------------------------------------------------------
void htmlTag::htmlTagFromString(AnsiString sTag)
{
    int nLength, nParam, nTemp, nTemp2;

    if(AnsiPos("size", sTag))
    {
        nFormatType = 1;
        FindParam(sTag);  //(Font size)
    }
    else if(AnsiPos("color", sTag))
    {
        nFormatType = 2;
        FindParam(sTag);  //(Color number)
    }
    else if(AnsiPos("/b", sTag))//Bold
    {
        nFormatType = 3;
    }
    else if(AnsiPos("/c", sTag))//Курсив
    {
        nFormatType = 4;
    }
    else if(AnsiPos("/u", sTag))//Underlined
    {
        nFormatType = 5;
    }
    else if(AnsiPos("/s", sTag))//Underlined
    {
        nFormatType = 6;
    }

    //Find Length
    FindLength(sTag);
}
//------------------------------------------------------------------------------


//TaggedString
//---------------------------------------------------------------------------
void TaggedString::ExtractTagAndSaveItToVector(AnsiString &str, int nStart)
{
    htmlTag hTag;
    AnsiString sTag;

    int nFinish = AnsiPos("/>", str);
    sTag = str.SubString(nStart+1, nFinish - nStart-1);   //Тут проверить эти минусы и плюсы
    hTag.htmlTagFromString(sTag); //Main hTag formation
    hTag.nStart = nStart-1;   //Почему -1 ???

    str.Delete(nStart, nFinish-nStart+2);

    TagList.push_back(hTag);
}
//---------------------------------------------------------------------------
void TaggedString::ExtractAllTags()
{
    TagList.clear();

    sStrWithoutTags = sStrWithTags; //Саму оригинальную строку не трогаем
    int nStart = AnsiPos("</", sStrWithoutTags);
    while(nStart)
    {
        ExtractTagAndSaveItToVector(sStrWithoutTags, nStart);
        nStart = AnsiPos("</", sStrWithoutTags);
    }
}
//------------------------------------------------------------------------------
void TaggedString::DoFormating(TRichEdit *RE)
{
    RE->Text = sStrWithoutTags;
    for(unsigned int i = 0; i<TagList.size(); i++)
        TagList[i].ApplyTagToRE(RE);
    bFormated = true;
}
//---------------------------------------------------------------------------
void TaggedString::ConvertToFormated(TRichEdit *RE)
{
    sStrWithTags = RE->Text;

    ExtractAllTags();
    DoFormating(RE);
}
//---------------------------------------------------------------------------
void TaggedString::RemoveFormat(TRichEdit *RE)
{
    if(bFormated)
    {
        RE->Text = sStrWithTags;
        TagList.clear();
        bFormated = false;
    }
}
//---------------------------------------------------------------------------
TaggedString TaggedString::operator=(AnsiString str)
{
    sStrWithTags = str;
    return *this;
}
//---------------------------------------------------------------------------






//Configger
//---------------------------------------------------------------------------
void Configger::OpenConfig()
{
    FileFuncs ff;
    AnsiString sPath = ff.GetCurDir() + "Config.ini";
    if(ff.GetIsFileExist(sPath))
        Memo->Lines->LoadFromFile(sPath);
}
//---------------------------------------------------------------------------
void Configger::SaveConfig()
{
    FileFuncs ff;
    AnsiString sPath = ff.GetCurDir() + "Config.ini";
    Memo->Lines->SaveToFile(sPath);
}
//---------------------------------------------------------------------------
int Configger::GetValueLine(AnsiString sConfig)
{
    if(!Memo->Lines->Count)
        OpenConfig();

    int nLine = Memo->Lines->IndexOf(sConfig);
    if(nLine)
        nLine++;

return nLine;
}
//---------------------------------------------------------------------------
AnsiString Configger::GetValue(AnsiString sConfig)
{
    AnsiString sResult;
    int nConfigValueLine = GetValueLine(sConfig);
    if(nConfigValueLine)
        sResult = Memo->Lines->Strings[nConfigValueLine];        
return sResult;
}
//---------------------------------------------------------------------------
void Configger::SetValue(AnsiString sConfig, AnsiString sValue)
{
    int nLine = GetValueLine(sConfig);
    if(!nLine)
    {
        Memo->Lines->Add("");
        Memo->Lines->Add(sConfig);
        Memo->Lines->Add(sValue);
    }
    else
        ReplaceLine(Memo, nLine, sValue);

   SaveConfig();
}
//---------------------------------------------------------------------------

