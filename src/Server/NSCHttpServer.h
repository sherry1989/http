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

#include <omnetpp.h>
#include <map>
#include <deque>

typedef unsigned int Pipelining_Mode_Type;
const Pipelining_Mode_Type FILL_FIRST = 1;
const Pipelining_Mode_Type DISTRIBUTE_FIRST = 2;

/**
 * TODO - Generated class
 */
class NSCHttpServer : public HttpServer
{
    public:
        NSCHttpServer();
        virtual ~NSCHttpServer();

        /** Initialization of the component and startup of browse event scheduling */
        virtual void initialize();

        /** Report final statistics */
        virtual void finish();

        /** Handle incoming messages */
        virtual void handleMessage(cMessage *msg);


    protected:

        /** Create a random body according to the site content random distributions. */
        virtual std::string generateBody();

        /**
         * Handler for socket data arrived events.
         * Dispatches the received message to the message handler in the base class and
         * finishes by deleting the received message.
         */
        virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);

        /** send HTTP reply message though TCPSocket depends on the TCP DataTransferMode*/
        virtual void sendMessage(TCPSocket *socket, HttpReplyMessage *reply);

        /** Handle a received data message, e.g. check if the content requested exists. */
        virtual HttpReplyMessage *handleReceivedMessage(cMessage *msg);

        void handleSelfDelayedReplyMessage(cMessage *msg);

        virtual void handleSelfMessages(cMessage *msg);

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

        /*
         * Format an application TCP_TRANSFER_BYTESTREAM response message which can be sent though NSC TCP depence on the application layer protocol
         * the protocol type can be HTTP \ SPDY \ HTTPS+M \ HTTPNF
         */
        virtual std::string formatByteResponseMessage(TCPSocket *socket, HttpReplyMessage *httpResponse);

        /** Format a response message to HTTP Response Message Header */
        virtual std::string formatHttpResponseMessageHeader(RealHttpReplyMessage *httpResponse);

        /** Deflate a HTTP Response message header using the Name-Value zlib dictionary */
        virtual std::string formatSpdyZlibHttpResponseMessageHeader(TCPSocket *socket, RealHttpReplyMessage *httpResponse);

        /** Format a response message header to zlib-deflated SPDY header block */
        virtual std::string formatSpdyZlibHeaderBlockResponseMessageHeader(RealHttpReplyMessage *httpResponse);

        /** Format a response message to HTTPNF Response Message Header */
        virtual std::string formatHttpNFResponseMessageHeader(RealHttpReplyMessage *httpResponse);

        bool initSockZlibInfo(TCPSocket *sock);

        void removeSockZlibInfo(TCPSocket *sock);


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

        Protocol_Type protocolType;

        RealHttpReplyMessage *changeReplyToReal(HttpReplyMessage *httpResponse);

        /**
         * A list of deflater or inflater for each socket
         */
        typedef std::map<TCPSocket *, ZlibInfo> TCPSocket_Zlib_Map;

        TCPSocket_Zlib_Map zlibPerSocket;

        // Basic statistics
        long headerBytesBeforeDeflate;
        long headerBytesAfterDeflate;
        long totalBytesSent;

        // Output Vectors
        cOutVector recvReqTimeVec;
        cOutVector sendResTimeVec;
        cOutVector headerDeflateRatioVec;
        cOutVector totalDeflateRatioVec;
};

#endif
