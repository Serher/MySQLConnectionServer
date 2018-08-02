//---------------------------------------------------------------------------


#pragma hdrstop

#include "Queue.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)



// Request
//------------------------------------------------------------------------------
Request::Request(TIdPeerThread *iAThread, AnsiString isMess)
{
    AThread = iAThread;
    sMess = isMess;
}
//------------------------------------------------------------------------------





// QUEUE
//------------------------------------------------------------------------------
void Queue::AddRequest(Request Req)
{
    Reqs.push_back(Req);
}
//------------------------------------------------------------------------------
void Queue::DeleteRequest() // Удаляет первый в списке запрос
{
    if(Reqs.size())
        Reqs.erase(Reqs.begin());
}
//------------------------------------------------------------------------------
Request Queue::GetNextRequest()
{
    Request rqResult;
    if(Reqs.size())
    {
        rqResult = Reqs[0];
        DeleteRequest();

        if(!rqResult.AThread || rqResult.sMess == "")
            rqResult = GetNextRequest();
    }
return rqResult;
}
//------------------------------------------------------------------------------
