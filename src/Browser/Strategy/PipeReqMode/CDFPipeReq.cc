/*
 * CDFPipeReq.cc
 *
 *  Created on: 2012-5-22
 *      Author: Administrator
 */

#include "CDFPipeReq.h"

CDFPipeReq::CDFPipeReq()
{
    // TODO Auto-generated constructor stub
    reqListPerSvr.clear();
    lastReqSockPerSvr.clear();
}

CDFPipeReq::~CDFPipeReq() {
    // TODO Auto-generated destructor stub
}

void CDFPipeReq::addMsg(HttpRequestMessage *msg, const std::string &svrName, SocketSet socketSet)
{
    //if it's the first time to request for this server
    SocketSet::iterator iter;
    if (lastReqSockPerSvr.find(svrName) == lastReqSockPerSvr.end())
    {
        iter = socketSet.begin();
        reqListPerSvr[svrName][*iter].push_front(msg);
        lastReqSockPerSvr[svrName] = *iter;
    }
    else
    {
        iter = socketSet.find(lastReqSockPerSvr[svrName]);
        iter++;
        if (iter != socketSet.end())
        {
            reqListPerSvr[svrName][*iter].push_front(msg);
            lastReqSockPerSvr[svrName] = *iter;
        }
        else
        {
            iter = socketSet.begin();
            reqListPerSvr[svrName][*iter].push_front(msg);
            lastReqSockPerSvr[svrName] = *iter;
        }
    }
}

cMessage *CDFPipeReq::getMsg(const std::string &svrName, TCPSocket *socket)
{
    cMessage *msg = reqListPerSvr[svrName][socket].back();
    reqListPerSvr[svrName][socket].pop_back();
    return msg;
}

bool CDFPipeReq::isEmpty(const std::string &svrName, TCPSocket *socket)
{
    return reqListPerSvr[svrName][socket].empty();
}
