/*
 * CPHSvrSupportDetect.h
 *
 *  Created on: 2012-5-22
 *      Author: Wang Qian
 */

#ifndef CPHSVRSUPPORTDETECT_H_
#define CPHSVRSUPPORTDETECT_H_

#include "CSvrSupportDetectBase.h"

class CPHSvrSupportDetect : public  CSvrSupportDetectBase
{
    /**
     * A list of active sockets for each server
     */
    typedef std::map<std::string, SvrSupport_Type> SvrSupportMap;

    public:
        CPHSvrSupportDetect();
        virtual ~CPHSvrSupportDetect();

        virtual void initSvrSupportForSock(PipeSockData *sockdata);
        virtual HttpContentType setSvrSupportForSock(PipeSockData *sockdata,  cPacket *msg);

    private:
        //--record server support state for each host
        SvrSupportMap svrSupportMap;
};

#endif /* CPHSVRSUPPORTDETECT_H_ */
