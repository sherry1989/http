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

#ifndef __HTTP_PIPEHTTPSERVER_H_
#define __HTTP_PIPEHTTPSERVER_H_

#include "HttpServer.h"

#include <omnetpp.h>
#include <map>
#include <deque>

/**
 * TODO - Generated class
 */
class PipeHttpServer : public HttpServer
{
    public:
        PipeHttpServer();
        virtual ~PipeHttpServer();

        /** Create a random body according to the site content random distributions. */
        virtual std::string generateBody();

        /**
         * Handler for socket data arrived events.
         * Dispatches the received message to the message handler in the base class and
         * finishes by deleting the received message.
         */
        virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);

        /** Handle incoming messages */
        virtual void handleMessage(cMessage *msg);

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
         * A list of http requests for each socket
         */
        typedef std::map<TCPSocket *, HttpReplyInfoQueue> TCPSocket_ReplyInfo_Map;

        TCPSocket_ReplyInfo_Map replyInfoPerSocket;

        void handleSelfDelayedReplyMessage(cMessage *msg);

        virtual void handleSelfMessages(cMessage *msg);
};

#endif
