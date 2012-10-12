/*
 * CFFPipeReq.h
 *
 *  Created on: 2012-5-22
 *      Author: Administrator
 */

#ifndef CFFPIPEREQ_H_
#define CFFPIPEREQ_H_

#include "CPipeReqBase.h"

class CFFPipeReq  : public CPipeReqBase
{
    public:
        CFFPipeReq();
        virtual ~CFFPipeReq();

        virtual void addMsg(HttpRequestMessage *msg, const std::string &svrName, SocketSet socketSet);

        virtual cMessage *getMsg(const std::string &svrName, TCPSocket *socket);

        virtual bool isEmpty(const std::string &svrName, TCPSocket *socket);


        /**
         * A list of http requests left for each server
         */
        typedef std::map<std::string, HttpReqQueue> FFReqListPerSvr;

    protected:
        FFReqListPerSvr reqListPerSvr;    ///< List of http requests left for each server
};

#endif /* CFFPIPEREQ_H_ */
