/*
 * CHttpAsciiEncoder.h
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#ifndef CHTTPASCIIENCODER_H_
#define CHTTPASCIIENCODER_H_

#include "CHeaderEncoderBase.h"
#include "CHttpParser.h"

typedef std::map<Socket_ID_Type, CHttpParser *> Parsers;

class CHttpAsciiEncoder: public CHeaderEncoderBase
{
    public:
        CHttpAsciiEncoder();
        virtual ~CHttpAsciiEncoder();

    public:
        /** Generate an encoded request message header */
        virtual std::string generateReqMsgHeader(const HeaderFrame requestFrame, const string requestURI);

        /** Generate an encoded response message header */
        virtual std::string generateResMsgHeader(const HeaderFrame responseFrame);

        /** Parse a received byte message to a RealHttpRequestMessage */
        virtual RealHttpRequestMessage* parseReqMsg(const char *data, size_t len, cPacket *msg, Socket_ID_Type sockID);

        /** Parse a received byte message to a RealHttpReplyMessage */
        virtual RealHttpReplyMessage* parseResMsg(const char *data, size_t len, cPacket *msg, Socket_ID_Type sockID);

        /** initialize parsers related to sockID */
        virtual void initParsers(Socket_ID_Type sockID);

        /** Release parsers assigned to the sockID */
        virtual void releaseParsers(Socket_ID_Type sockID);

        bool ifNewMsg(Socket_ID_Type sockID);

    private:
        Parsers httpParsers;
};

#endif /* CHTTPASCIIENCODER_H_ */
