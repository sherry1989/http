/*
 * CSvrSupportDetectBase.h
 *
 *  Created on: 2012-5-22
 *      Author: Wang Qian
 */

#ifndef CSVRSUPPORTDETECTBASE_H_
#define CSVRSUPPORTDETECTBASE_H_

#include "TypeDef.h"

class CSvrSupportDetectBase
{
    public:
        CSvrSupportDetectBase();
        virtual ~CSvrSupportDetectBase();

        virtual void initSvrSupportForSock(PipeSockData *sockdata);
        virtual HttpContentType setSvrSupportForSock(PipeSockData *sockdata,  cPacket *msg);
};

#endif /* CSVRSUPPORTDETECTBASE_H_ */
