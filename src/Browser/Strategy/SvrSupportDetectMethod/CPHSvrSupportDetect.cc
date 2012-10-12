/*
 * CPHSvrSupportDetect.cc
 *
 *  Created on: 2012-5-22
 *      Author: Wang Qian
 */

#include "CPHSvrSupportDetect.h"

CPHSvrSupportDetect::CPHSvrSupportDetect() {
    // TODO Auto-generated constructor stub

}

CPHSvrSupportDetect::~CPHSvrSupportDetect()
{
    // TODO Auto-generated destructor stub
    svrSupportMap.clear();
}

void CPHSvrSupportDetect::initSvrSupportForSock(PipeSockData *sockdata)
{
    SvrSupportMap::iterator iter = svrSupportMap.find(sockdata->svrName);
    if ( iter != svrSupportMap.end())
    {
        sockdata->svrSupport = iter->second;
    }
    else
    {
        sockdata->svrSupport = e_Unknown;
        svrSupportMap.insert(make_pair(sockdata->svrName, e_Unknown));
    }
}

HttpContentType CPHSvrSupportDetect::setSvrSupportForSock(PipeSockData *sockdata,  cPacket *msg)
{
    //record the response resource type for the follow-up use
    HttpContentType contentType;

    HttpReplyMessage *appmsg = check_and_cast<HttpReplyMessage*>(msg);
    if (appmsg==NULL)
//                std::runtime_error("Message (%s)%s is not a valid reply message", msg->getClassName(), msg->getName());
    contentType = (HttpContentType)appmsg->contentType();

    SvrSupportMap::iterator iter = svrSupportMap.find(sockdata->svrName);
    if ( iter != svrSupportMap.end())
    {
        //not detect whether the server support pipelining
        if (iter->second == e_Unknown)
        {
            if (appmsg->result()!=200 || contentType==CT_UNKNOWN)
            {
                EV << "[In CPHSvrSupportDetect] Result for " << appmsg->getName() << " was other than OK. Code: " << appmsg->result() << endl;
            }

            /*
             * if the http protocol version is 1.1 and the connection is keepalive, the server is treated as it supports pipelining
             * otherwise, the server doesn't support pipelining
             */
            if (11==appmsg->protocol() && true==appmsg->keepAlive())
            {
                iter->second = e_Support;
            }
            else
            {
                iter->second = e_NotSupport;
            }
        }
        sockdata->svrSupport = iter->second;
    }
    else
    {
        EV << "[In CPHSvrSupportDetect] Cannot find the host's server support state for HTTP  pipelining. " << endl;
    }

    return contentType;
}
