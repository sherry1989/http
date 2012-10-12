/*
 * CFFPipeReq.cc
 *
 *  Created on: 2012-5-22
 *      Author: Administrator
 */

#include "CFFPipeReq.h"

CFFPipeReq::CFFPipeReq()
{
    // TODO Auto-generated constructor stub
    reqListPerSvr.clear();
}

CFFPipeReq::~CFFPipeReq() {
    // TODO Auto-generated destructor stub
}

void CFFPipeReq::addMsg(HttpRequestMessage *msg, const std::string &svrName, SocketSet socketSet)
{
    reqListPerSvr[svrName].push_front(msg);
}

cMessage *CFFPipeReq::getMsg(const std::string &svrName, TCPSocket *socket)
{
    cMessage *msg = reqListPerSvr[svrName].back();
    reqListPerSvr[svrName].pop_back();
    return msg;
}

bool CFFPipeReq::isEmpty(const std::string &svrName, TCPSocket *socket)
{
    return reqListPerSvr[svrName].empty();
}
