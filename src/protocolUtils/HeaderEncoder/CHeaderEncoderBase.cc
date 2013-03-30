/*
 * CHeaderEncoderBase.cc
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#include "CHeaderEncoderBase.h"

CHeaderEncoderBase::CHeaderEncoderBase()
{
    // TODO Auto-generated constructor stub
    
}

CHeaderEncoderBase::~CHeaderEncoderBase()
{
    // TODO Auto-generated destructor stub
}

std::string CHeaderEncoderBase::generateReqMsgHeader(const HeaderFrame requestFrame, const string requestURI)
{
    std::ostringstream str;

    return str.str();
}

std::string CHeaderEncoderBase::generateResMsgHeader(const HeaderFrame responseFrame)
{
    std::ostringstream str;

    return str.str();
}

/** Parse a received byte message to a RealHttpRequestMessage */
RealHttpRequestMessage* CHeaderEncoderBase::parseReqMsg(const char *data, size_t len, cPacket *msg, Socket_ID_Type sockID)
{
    RealHttpRequestMessage* httpRequest = new RealHttpRequestMessage();
    return httpRequest;
}

/** Parse a received byte message to a RealHttpReplyMessage */
RealHttpReplyMessage* CHeaderEncoderBase::parseResMsg(const char *data, size_t len, cPacket *msg, Socket_ID_Type sockID)
{
    RealHttpReplyMessage *httpResponse = new RealHttpReplyMessage();
    return httpResponse;
}

/** initialize parsers related to sockID */
void CHeaderEncoderBase::initParsers(Socket_ID_Type sockID)
{

}

/** Release parsers assigned to the sockID */
void CHeaderEncoderBase::releaseParsers(Socket_ID_Type sockID)
{

}
