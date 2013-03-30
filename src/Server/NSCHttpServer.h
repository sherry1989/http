//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __HTTP_NSCHTTPSERVER_H_
#define __HTTP_NSCHTTPSERVER_H_

#include "HttpServer.h"
#include "ProtocolTypeDef.h"
#include "CMessageController.h"

#include <omnetpp.h>
#include <map>
#include <deque>

/**
 * TODO - Generated class
 */
class NSCHttpServer : public HttpServer
{
    public:
        NSCHttpServer();
        virtual ~NSCHttpServer();

    public:
        /** @name do statistics */
        //@{
        virtual void incImgResourcesServed();
        virtual void incTextResourcesServed();
        virtual void incMediaResourcesServed();
        virtual void incOtherResourcesServed();
        virtual void decBadRequests();
        virtual void recordCompressStatistic(size_t framelen, size_t nvbuflen, size_t bodyLength);
        //@}

    protected:
        /** @name cSimpleModule redefinitions */
        //@{
        /** Initialization of the component and startup of browse event scheduling */
        virtual void initialize();

        /** Report final statistics */
        virtual void finish();

        /** Handle incoming messages */
        virtual void handleMessage(cMessage *msg);
        //@}

    protected:
        /** @name TCPSocket::CallbackInterface callback methods */
        //@{
        /**
         * Handler for socket established events.
         * Only used to update statistics.
         */
        virtual void socketEstablished(int connId, void *yourPtr);

        /**
         * Handler for socket closed event.
         * Cleanup the resources for the closed socket.
         */
        virtual void socketClosed(int connId, void *yourPtr);

        /**
         * Handler for socket failure event.
         * Very basic handling -- displays warning and cleans up resources.
         */
        virtual void socketFailure(int connId, void *yourPtr, int code);

        /**
         * Handler for socket data arrived events.
         * Dispatches the received message to the message handler in the base class and
         * finishes by deleting the received message.
         */
        virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);
        //@}

    protected:
        /** @name HttpServerBase redefinitions */
        //@{
        /** Create a random body according to the site content random distributions. */
        virtual std::string generateBody();
        /** Handle a received data message, e.g. check if the content requested exists. */
        virtual HttpReplyMessage *handleReceivedMessage(cMessage *msg);
        //@}

    protected:
        /** @name NSCHttpServer Methods */
        //@{
        /** send HTTP reply message though TCPSocket depends on the TCP DataTransferMode */
        virtual void sendMessage(TCPSocket *socket, HttpReplyMessage *reply);

        /** handle self delayed message to support server processing delay */
        void handleSelfDelayedReplyMessage(cMessage *msg);

        /** handle self messages */
        virtual void handleSelfMessages(cMessage *msg);

        /** when socket is closed or failed, release the information related to the socket */
        void removeIDInfo(TCPSocket *sock);
        //@}

    private:
        /** set message factory depending on the message header encode type and compression type */
        void setMessageFactory();

    protected:
        /**
         * A list of HTTP replies to send.
         */
        struct ReplyInfo
        {
            HttpReplyMessage* reply;
            bool readyToSend;
        };
        typedef std::deque<ReplyInfo> HttpReplyInfoQueue;

        /**
         * A list of http responses for each socket
         */
        typedef std::map<TCPSocket *, HttpReplyInfoQueue> TCPSocket_ReplyInfo_Map;

        TCPSocket_ReplyInfo_Map replyInfoPerSocket;
//
//        Protocol_Type protocolType;
//
//        RealHttpReplyMessage *changeReplyToReal(HttpReplyMessage *httpResponse);

        /**
         * A list of deflater or inflater for each socket
         */
        typedef std::map<TCPSocket *, Socket_ID_Type> TCPSocket_ID_Map;

        TCPSocket_ID_Map idPerSocket;

        /**
         * record the configure informations
         */
        Header_Encode_Type headerEncodeType;                            //Message header encode type
        Header_Compress_Type headerCompressType;                        //Message header compress type
        Message_Info_Src messageInfoSrc;                                //Message information source

        /**
         * Basic statistics
         */
        long headerBytesBeforeDeflate;
        long headerBytesAfterDeflate;
        long totalBytesSent;
        long mediaResourcesServed;
        long otherResourcesServed;

        /**
         * Output Vectors
         */
        cOutVector recvReqTimeVec;
        cOutVector sendResTimeVec;
        cOutVector headerDeflateRatioVec;
        cOutVector totalDeflateRatioVec;

    private:
        /**
         * Pointers to message factory
         */
        CMessageController *pMessageController;

        bool getWaitTimeFromHar;
};

#endif
