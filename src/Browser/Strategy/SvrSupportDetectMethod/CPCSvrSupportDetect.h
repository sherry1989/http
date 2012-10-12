/*
 * CPCSvrSupportDetect.h
 *
 *  Created on: 2012-5-22
 *      Author: Wang Qian
 */

#ifndef CPCSVRSUPPORTDETECT_H_
#define CPCSVRSUPPORTDETECT_H_

#include "CSvrSupportDetectBase.h"


class CPCSvrSupportDetect  : public  CSvrSupportDetectBase
{
    public:
        CPCSvrSupportDetect();
        virtual ~CPCSvrSupportDetect();

        virtual void initSvrSupportForSock(PipeSockData *sockdata);
        virtual HttpContentType setSvrSupportForSock(PipeSockData *sockdata,  cPacket *msg);
};

#endif /* CPCSVRSUPPORTDETECT_H_ */
