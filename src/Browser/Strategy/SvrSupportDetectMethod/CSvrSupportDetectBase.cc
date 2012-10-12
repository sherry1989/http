/*
 * CSvrSupportDetectBase.cc
 *
 *  Created on: 2012-5-22
 *      Author: Wang Qian
 */

#include "CSvrSupportDetectBase.h"

CSvrSupportDetectBase::CSvrSupportDetectBase() {
    // TODO Auto-generated constructor stub

}

CSvrSupportDetectBase::~CSvrSupportDetectBase() {
    // TODO Auto-generated destructor stub
}

void CSvrSupportDetectBase::initSvrSupportForSock(PipeSockData *sockdata)
{
    sockdata->svrSupport = e_Unknown;
}

HttpContentType CSvrSupportDetectBase::setSvrSupportForSock(PipeSockData *sockdata,  cPacket *msg)
{
    HttpReplyMessage *appmsg = check_and_cast<HttpReplyMessage*>(msg);

    return (HttpContentType)appmsg->contentType();
}
