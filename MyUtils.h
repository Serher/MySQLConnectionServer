// Модифицирован 17.01.2018


//---------------------------------------------------------------------------

#ifndef MyUtilsH
#define MyUtilsH
//---------------------------------------------------------------------------
#include <StdCtrls.hpp> // Для Memo
#include <vector>
#include <fstream>
#include <ComCtrls.hpp> //для RE
//#include <stdio.h>
#include <sys/stat.h>





// ДЛЯ ИНФОРМАЦИИ

// Переместить файл
// MoveFile(Текущее мостоположение.c_str(), Новое местоположение.c_str());

// Переименование. Все понятно - указывается старое имя(полный путь) и новое. Возможно требует #include <stdio.h>
// rename(AsString().c_str(), (sPath + sNewName + sExt).c_str());

// Создание директории. 
// CreateDirectory(sVideoDirectory.c_str(), NULL);

// Пример парсинга директории, взят отсюда http://www.cyberforum.ru/cpp-builder/thread241445.html
/*
    TSearchRec sr;
    int done = FindFirst(sPath + "*", faAnyFile, sr);  // Опция: AnsiString Path = Dir+"*."+Mask; (таким образом можно задать интересующее расширение)
    while(!done)
    {
        Memo2->Lines->Add(sr.Name);
        done = FindNext(sr);
    }
    FindClose(sr);
*/





//FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
void DBM(TMemo *Memo, AnsiString str1, AnsiString str2 = "");
void DBM(TMemo *Memo, AnsiString str1 = "", int n = 0);

void NormalizeCB(TComboBox *ComboBox, bool bOne = false, bool bNoNew = true); //bOne - подставляет значение если оно одно в списке; bNoNew - не позволяет вводить то чего нет в списке
void ReplaceLine(TMemo *Memo, int nLine, AnsiString str);

void LeftDigitsOnly(AnsiString &str);

void CopyTextToClipboard(AnsiString str);

WideString AnsiToWide(AnsiString str);
AnsiString WideToAnsi(WideString WS);


AnsiString GetStringRight(AnsiString str, int nCount);
AnsiString GetStringLeft(AnsiString str, int nCount);





//CLASSES
////////////////////////////////////////////////////////////////////////////////

class FileFuncs
{
    public:
    // File
    bool GetIsFileExist(AnsiString sPath);
    AnsiString GetFileLastUpdateTime(AnsiString sPath, AnsiString sComposer = ".");
    long GetFileSize(AnsiString sPath);

    // Directories
    bool GetIsDirectoryExist(AnsiString sPath);
    AnsiString GetCurDir();
    AnsiString GetDesktopDir();

    // Shortcuts
    void CreateShortCut(const AnsiString &sFilePath, const AnsiString &sFileDir, const AnsiString &sShortcutDir, AnsiString sDescription = "");
    void CreateShortcutOnDesktop(const AnsiString &sFilePath, const AnsiString &sFileDir, AnsiString sDescription = "");
};
//---------------------------------------------------------------------------
class StringsContainer
{
    public:
    std::vector<AnsiString> Vector;

    StringsContainer(){Vector.clear();}
    unsigned int BreakString(AnsiString str, AnsiString sDivider);
    int IndexOf(AnsiString sValue);
};
//---------------------------------------------------------------------------


//For RE
//---------------------------------------------------------------------------
class htmlTag
{    
    public:

    int nFormatType;
    int nStart;
    int nLength;
    unsigned int nParam;



    htmlTag()
    {
        nFormatType = 0;
        nStart = 0;
        nLength = 0;
        nParam = -1;
    }

    void ApplyTagToRE(TRichEdit *RE);
    void FindParam(AnsiString sTag);
    void FindLength(AnsiString sTag);
    void htmlTagFromString(AnsiString sTag);
};
//---------------------------------------------------------------------------
class TaggedString
{
    public:
    //main components
    AnsiString sStrWithTags;
    AnsiString sStrWithoutTags;
    std::vector<htmlTag> TagList; //Массив тэгов
    bool bFormated;
    //---
    void ExtractTagAndSaveItToVector(AnsiString &str, int nStart);


    TaggedString()
    {
        sStrWithTags = "";
        sStrWithoutTags = "";
        TagList.clear();
        bFormated = false;
    }

    TaggedString operator=(AnsiString str);

    //Акскссорные
    //void SetStringWithTags(AnsiString str){sStrWithTags = str;}
    
    //Для внешнего использования
    void ExtractAllTags();
    void DoFormating(TRichEdit *RE);
    void ConvertToFormated(TRichEdit *RE);
    void RemoveFormat(TRichEdit *RE);
    AnsiString ToString(){return sStrWithTags;}
    //bool GetREIsFormated(TRichEdit *RE){return RE->Text == sStrWithoutTags/* && sStrWithTags!=""*/;}
};
//---------------------------------------------------------------------------

class Configger
{
    TMemo *Memo;
    public:

    Configger(){Memo = 0;}
    Configger(TMemo *M){Memo = M; Memo->Clear();}

    void OpenConfig();
    void SaveConfig();
    int GetValueLine(AnsiString sConfig);
    AnsiString GetValue(AnsiString sConfig);
    void SetValue(AnsiString sConfig, AnsiString sValue);
};

#endif




