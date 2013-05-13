/*
 * CHeaderEncoderBase.h
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#ifndef CHEADERENCODERBASE_H_
#define CHEADERENCODERBASE_H_

#include "ProtocolTypeDef.h"
#include "TypeDef.h"

class CHeaderEncoderBase
{
    public:
        CHeaderEncoderBase();
        virtual ~CHeaderEncoderBase();

    public:
        /** Generate an encoded request message header */
        virtual std::string generateReqMsgHeader(const HeaderFrame requestFrame, const string requestURI);

        /** Generate an encoded response message header */
        virtual std::string generateResMsgHeader(const HeaderFrame responseFrame);

        /** Parse a received byte message to a RealHttpRequestMessage */
        virtual RealHttpRequestMessage* parseReqMsg(const char *data, size_t len, cPacket *msg, Socket_ID_Type sockID);

        /** Parse a received byte message to a RealHttpReplyMessage */
        virtual RealHttpReplyMessage* parseResMsg(const char *data, size_t len, cPacket *msg, Socket_ID_Type sockID);

        /** try to deal with the rest bytes */
        virtual RealHttpReplyMessage *dealWithRestResBytes(Socket_ID_Type sockID);

        /** try to deal with the rest bytes in request message */
        virtual RealHttpRequestMessage *dealWithRestReqBytes(Socket_ID_Type sockID);

        /** initialize parsers related to sockID */
        virtual void initParsers(Socket_ID_Type sockID);

        /** Release parsers assigned to the sockID */
        virtual void releaseParsers(Socket_ID_Type sockID);
};

#endif /* CHEADERENCODERBASE_H_ */
