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

#ifndef __HTTP_PipeHttpBrowser_H_
#define __HTTP_PipeHttpBrowser_H_

#include "TypeDef.h"
#include "CPipeReqBase.h"
#include "CSvrSupportDetectBase.h"

#include <omnetpp.h>

/**
 * TODO - Generated class
 */

class PipeHttpBrowser : public HttpBrowser
{
    public:
        PipeHttpBrowser();
        virtual ~PipeHttpBrowser();

    //--added by wangqian, 2012-05-15
    protected:
        /*
         * record the http limit
         */
        unsigned int maxConnections;
        unsigned int maxConnectionsPerHost;
        unsigned int maxPipelinedReqs;
        Pipelining_Mode_Type pipeliningMode;
        //--added by wangqian, 2012-05-16
        SvrSupportDetect_Method_Type SvrSupportDetect;
        //--added end
    //--added end

    protected:

        //--added by wangqian, 2012-05-15
        /**
         * A list of active sockets for each server
         */
        typedef std::map<std::string, SocketSet> TCPSocketServerMap;
        /**
         * A list of http requests left for each server
         */
//        typedef std::map<std::string, HttpRequestQueue> ReqListPerSvr;
        //--added end


    //--added by wangqian, 2012-05-15
    protected:
        TCPSocketServerMap sockCollectionMap;    ///< List of active sockets for each server
//        ReqListPerSvr reqListPerSvr;    ///< List of http requests left for each server
    //--added end

    protected:
        /** @name cSimpleModule redefinitions */
        //@{
        /** Initialization of the component and startup of browse event scheduling */
        virtual void initialize(int stage);

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

    protected:
        /** @name Socket establishment and data submission */
        //@{
        /**
         * Establishes a socket and queues a single message for transmission.
         * A new socket is created and added to the collection. The message is assigned to a data structure
         * stored as a myPtr with the socket. The message is transmitted once the socket is established, signaled
         * by a call to socketEstablished.
         */
        virtual void submitToSocket(const char* moduleName, int connectPort, HttpRequestMessage *msg);

        /**
         * Establishes a socket and assigns a queue of messages to be transmitted.
         * Same as the overloaded version, except a number of messages are queued for transmission. The same socket
         * instance is used for all the queued messages.
         */
        virtual void submitToSocket(const char* moduleName, int connectPort, HttpRequestQueue &queue);
        //@}

        //--added by wangqian, 2012-05-22
        /*
         * 指向各类策略基类的指针，用于调用不同的策略
         */
    private:
        CPipeReqBase *pPipeReq;
        CSvrSupportDetectBase *pSvrSupportDetect;

        //策略选择，确定派生类
        void chooseStrategy(Pipelining_Mode_Type pipeliningMode, SvrSupportDetect_Method_Type SvrSupportDetect);
        //--added end


};

#endif
