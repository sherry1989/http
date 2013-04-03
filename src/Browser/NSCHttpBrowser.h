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

#ifndef __HTTP_NSCHTTPBROWSER_H_
#define __HTTP_NSCHTTPBROWSER_H_

#include "TypeDef.h"
#include "CPipeReqBase.h"
#include "CSvrSupportDetectBase.h"
#include "ProtocolTypeDef.h"
#include "CMessageController.h"

#include <list>

#include <omnetpp.h>

/** redefine PipeSockData as NSCSockData */
struct NSCSockData :public PipeSockData
{
      Socket_ID_Type sockID;
};

/**
 * TODO - Generated class
 */

class NSCHttpBrowser : public HttpBrowser
{
    public:
        NSCHttpBrowser();
        virtual ~NSCHttpBrowser();

    public:
        /** @name do statistics */
        //@{
        virtual void incRequestHarGenerated();
        virtual void recordCompressStatistic(size_t framelen, size_t nvbuflen, size_t bodyLength);
        //@}

    protected:
        /** @name cSimpleModule redefinitions */
        //@{
        /** Initialization of the component and startup of browse event scheduling */
        virtual void initialize(int stage);

        /** Report final statistics */
        virtual void finish();
        //@}

    protected:
        /** @name TCPSocket::CallbackInterface callback methods */
        //@{
        /**
         * Handler for socket established event.
         * Called by the socket->processMessage(msg) handler call in handleMessage.
         * The pending messages for the socket are transmitted in the order queued. The socket remains
         * open after the handler has completed. A counter for pending messages is incremented for each
         * request sent.
         */
        virtual void socketEstablished(int connId, void *yourPtr);

        /**
         * Handler for socket data arrival.
         * Called by the socket->processMessage(msg) handler call in handleMessage.
         * virtual method of the parent class. The counter for pending replies is decremented for each one handled.
         * Close is called on the socket once the counter reaches zero.
         */
        virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);

        /**
         * Handler for the socket closed by peer event.
         * Called by the socket->processMessage(msg) handler call in handleMessage.
         */
        virtual void socketPeerClosed(int connId, void *yourPtr);

        /**
         * Socket closed handler.
         * Called by the socket->processMessage(msg) handler call in handleMessage.
         */
        virtual void socketClosed(int connId, void *yourPtr);

        /**
         * Socket failure handler.
         * This method does nothing but reporting and statistics collection at this time.
         * @todo Implement reconnect if necessary. See the INET demos.
         */
        virtual void socketFailure(int connId, void *yourPtr, int code);
        //@}

    protected:
        /** @name Socket establishment and data submission */
        //@{
        /**
         * Establishes a socket and assigns a queue of messages to be transmitted.
         * Same as the overloaded version, except a number of messages are queued for transmission. The same socket
         * instance is used for all the queued messages.
         */
        virtual void submitToSocket(const char* moduleName, int connectPort, HttpRequestQueue &queue);
        //@}

    protected:
        /**
         * send HTTP reply message though TCPSocket depends on the TCP DataTransferMode
         */
        virtual void sendMessage(NSCSockData *sockdata, HttpRequestMessage *reply);

        /**
         * parse a parsed response message, check if this response message is complete
         *      if it's a complete message, handle it and try to deal with the rest bytes may remained unparsed
         *      else, wait for the coming messages contain the else message body, do nothing
         */
        void parseResponseMessage(int connId, NSCSockData *sockdata, cPacket *parsedMsg);

        /** try to deal with the rest bytes */
        void dealWithRestBytes(int connId, NSCSockData *sockdata);

    private:
        /** select pipelining related strategies, pointer set to the configured derived classes */
        void selectPipeStrategy();

        /** set message factory depending on the message header encode type and compression type */
        void setMessageFactory();


    protected:
        /**
         * record the http limit
         */
        unsigned int maxConnections;
        unsigned int maxConnectionsPerHost;
        unsigned int maxPipelinedReqs;

        /**
         * record the configure informations
         */
        Pipelining_Mode_Type pipeliningMode;                            //Pipelining Request Distribution Algorithm
        SvrSupportDetect_Method_Type SvrSupportDetect;                  //Pipelining Server Support Detection Algorithm
        Header_Encode_Type headerEncodeType;                            //Message header encode type
        Header_Compress_Type headerCompressType;                        //Message header compress type
        Message_Info_Src messageInfoSrc;                                //Message information source

        /**
         * A list of active sockets for each server
         */
        typedef std::map<std::string, SocketSet> TCPSocketServerMap;
        TCPSocketServerMap sockCollectionMap;

        /**
         * Basic statistics
         */
        long responseMessageReceived;
        long responseParsed;
        long headerBytesBeforeDeflate;
        long headerBytesAfterDeflate;
        long totalBytesSent;
        long requestSent;
        long requestHarGenerated;

        /**
         * Output Vectors
         */
        cOutVector recvResTimeVec;
        cOutVector sendReqTimeVec;
        cOutVector headerDeflateRatioVec;
        cOutVector totalDeflateRatioVec;

    private:
        /**
         * Pointers to pipelining related strategies Base Classes
         */
        CPipeReqBase *pPipeReq;
        CSvrSupportDetectBase *pSvrSupportDetect;

        /**
         * Pointers to message factory
         */
        CMessageController *pMessageController;

        /**
         * A list of request URI for each socket:
         *      when send request message, push the request URI to the tail of the socket's list
         *      when receive response message, pop the request URI from the head of the socket's list
         */
        typedef std::list<std::string> ReqUriList;
        typedef std::map<TCPSocket *, ReqUriList> TCPSocket_ReqUriList_Map;
        TCPSocket_ReqUriList_Map reqListPerSocket;
};

#endif
