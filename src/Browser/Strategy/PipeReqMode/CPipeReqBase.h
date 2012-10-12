/*
 * CPipeReqBase.h
 *
 *  Created on: 2012-5-22
 *      Author: Administrator
 */

#ifndef CPIPEREQBASE_H_
#define CPIPEREQBASE_H_

#include "TypeDef.h"

class CPipeReqBase {
    public:
        CPipeReqBase();
        virtual ~CPipeReqBase();

        virtual void addMsg(HttpRequestMessage *msg, const std::string &svrName, SocketSet socketSet){};

        virtual cMessage *getMsg(const std::string &svrName, TCPSocket *socket){cMessage *msg = new cMessage; return msg;};

        virtual bool isEmpty(const std::string &svrName, TCPSocket *socket){return true;};
};

#endif /* CPIPEREQBASE_H_ */
