/*
 * CPCSvrSupportDetect.cc
 *
 *  Created on: 2012-5-22
 *      Author: Wang Qian
 */

#include "CPCSvrSupportDetect.h"

CPCSvrSupportDetect::CPCSvrSupportDetect()
{
    // TODO Auto-generated constructor stub

}

CPCSvrSupportDetect::~CPCSvrSupportDetect()
{
    // TODO Auto-generated destructor stub
}

void CPCSvrSupportDetect::initSvrSupportForSock(PipeSockData *sockdata)
{
    sockdata->svrSupport = e_Unknown;
}

HttpContentType CPCSvrSupportDetect::setSvrSupportForSock(PipeSockData *sockdata,  cPacket *msg)
{
    //record the response resource type for the follow-up use
    HttpContentType contentType;

    //not detect whether the server support pipelining
    if (sockdata->svrSupport == e_Unknown)
    {
        HttpReplyMessage *appmsg = check_and_cast<HttpReplyMessage*>(msg);
        if (appmsg==NULL)
//                runtime_error("Message (%s)%s is not a valid reply message", msg->getClassName(), msg->getName());
        contentType = (HttpContentType)appmsg->contentType();
        if (appmsg->result()!=200 || contentType==CT_UNKNOWN)
        {
            EV << "[In CPCSvrSupportDetect] Result for " << appmsg->getName() << " was other than OK. Code: " << appmsg->result() << endl;
        }

        /*
         * if the http protocol version is 1.1 and the connection is keepalive, the server is treated as it supports pipelining
         * otherwise, the server doesn't support pipelining
         */
        if (11==appmsg->protocol() && true==appmsg->keepAlive())
        {
            sockdata->svrSupport = e_Support;
        }
        else
        {
            sockdata->svrSupport = e_NotSupport;
        }
    }

    return contentType;
}

