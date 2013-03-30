/*
 * CMessageController.h
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#ifndef CMESSAGECONTROLLER_H_
#define CMESSAGECONTROLLER_H_

#include "MsgInfoSrcBase.h"
#include "CHeaderEncoderBase.h"
#include "CHeaderCompressorBase.h"
#include "HttpNodeBase.h"

class CMessageController
{
    public:
        CMessageController(CHeaderEncoderBase *vEncoder, CHeaderCompressorBase *vCompressor, MsgInfoSrcBase *vMsgInfoSrc, HttpNodeBase *vHttpNode);
        ~CMessageController();

    public:
        /**
         * Format an application TCP_TRANSFER_BYTESTREAM Request message which can be sent though NSC TCP depending on the application layer protocol
         */
        std::string generateRequestMessage(HttpRequestMessage *httpRequest, Socket_ID_Type sockID);

        /**
         * Format an application TCP_TRANStypedef unsigned int FER_BYTESTREAM response message which can be sent though NSC TCP depending on the application layer protocol
         */
        std::string generateResponseMessage(HttpReplyMessage *httpResponse, Socket_ID_Type sockID);

        /** Parse a received byte message to a RealHttpRequestMessage */
        RealHttpRequestMessage* parseRequestMessage(cPacket *msg, Socket_ID_Type sockID);

        /** Parse a received byte message to a RealHttpReplyMessage */
        RealHttpReplyMessage* praseResponseMessage(cPacket *msg, Socket_ID_Type sockID);

        /** assign a new sockID and assign related things to the sockID */
        Socket_ID_Type getNewSockID();

        /** Release related thing assigned to the sockID */
        void releaseSockID(Socket_ID_Type sockID);

        HeaderFrame getTimingFromHar(string requestURI);

        RealHttpRequestMessage *changeRequestToReal(HttpRequestMessage *httpRequest);
        RealHttpReplyMessage *changeReplyToReal(HttpReplyMessage *httpResponse);

    private:
        /** Message header encode type */
        CHeaderEncoderBase *pEncoder;

        /** Message header compress type */
        CHeaderCompressorBase *pCompressor;

        /** Message information source */
        MsgInfoSrcBase *pMsgInfoSrc;

        /** Pointer to the server or browser */
        HttpNodeBase *pHttpNode;

        /** socket ID to assign this time */
        Socket_ID_Type assignSockID;
};

#endif /* CMESSAGECONTROLLER_H_ */
