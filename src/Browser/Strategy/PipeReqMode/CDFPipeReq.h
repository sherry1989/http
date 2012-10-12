/*
 * CDFPipeReq.h
 *
 *  Created on: 2012-5-22
 *      Author: Administrator
 */

#ifndef CDFPIPEREQ_H_
#define CDFPIPEREQ_H_

#include "CPipeReqBase.h"

class CDFPipeReq   : public CPipeReqBase
{
    public:
        CDFPipeReq();
        virtual ~CDFPipeReq();

        virtual void addMsg(HttpRequestMessage *msg, const std::string &svrName, SocketSet socketSet);

        virtual cMessage *getMsg(const std::string &svrName, TCPSocket *socket);

        virtual bool isEmpty(const std::string &svrName, TCPSocket *socket);

        /**
         * A list of http requests for each socket
         */
        typedef std::map<TCPSocket *, HttpReqQueue> TCPSocket_ReqQueue_Map;

        /**
         * A list of http requests left for each server
         */
        typedef std::map<std::string, TCPSocket_ReqQueue_Map> DFReqListPerSvr;

        /**
         * A list of last request socket for each server
         */
        typedef std::map<std::string, TCPSocket *> LastReqSocketPerSvr;

    protected:
        DFReqListPerSvr reqListPerSvr;    ///< List of http requests left for each server
        LastReqSocketPerSvr lastReqSockPerSvr;          ///< List of last request socket for each server
};

#endif /* CDFPIPEREQ_H_ */
