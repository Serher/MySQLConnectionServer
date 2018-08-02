//---------------------------------------------------------------------------

#ifndef QueueH
#define QueueH
//---------------------------------------------------------------------------
#include <IdTCPServer.hpp>
#include <vector>




//------------------------------------------------------------------------------
class Request
{
    public:
    TIdPeerThread *AThread;
    AnsiString sMess;

    Request(){AThread = 0; sMess = "";}
    Request(TIdPeerThread *iAThread, AnsiString isMess);
};
//------------------------------------------------------------------------------
class Queue
{
    public:
    std::vector<Request> Reqs;

    //Constructors
    Queue(){Reqs.clear();}
    
    //Funcs
    void AddRequest(Request Req);
    void DeleteRequest(); // Удаляет первый в списке запрос
    Request GetNextRequest();
};
//------------------------------------------------------------------------------





#endif
