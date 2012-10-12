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

#include "PipeHttpServer.h"

Define_Module(PipeHttpServer);

PipeHttpServer::PipeHttpServer()
{
    replyInfoPerSocket.clear();
}
PipeHttpServer::~PipeHttpServer()
{

}

std::string PipeHttpServer::generateBody()
{
    EV_DEBUG << "Pipe enter generateBody"<< endl;

    int numResources = (int)rdNumResources->draw();
    int numImages = (int)(numResources*rdTextImageResourceRatio->draw());
    int numText = numResources - numImages;

    std::string result;

    char resource[128];
    char tempBuf[128];
    for (int i=0; i<numImages; i++)
    {
        sprintf(resource, "%s%.4d.%s", "IMG", i, "jpg");
        resources[resource] = (unsigned int)(rdImageResourceSize->draw());
        EV_DEBUG << "resource "<< resource <<" bytelength " << resources[resource] << endl;
        sprintf(tempBuf, "%s\n", resource);
        result.append(tempBuf);

    }
    for (int i=0; i<numText; i++)
    {
        sprintf(resource, "%s%.4d.%s", "TEXT", i, "txt");
        resources[resource] = (unsigned int)(rdTextResourceSize->draw());
        EV_DEBUG << "resource "<< resource <<" bytelength " << resources[resource] << endl;
        sprintf(tempBuf, "%s\n", resource);
        result.append(tempBuf);
    }

    return result;
}

void PipeHttpServer::socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent)
{
    if (yourPtr==NULL)
    {
        EV_ERROR << "Socket establish failure. Null pointer" << endl;
        return;
    }
    TCPSocket *socket = (TCPSocket*)yourPtr;

    // Should be a HttpRequestMessage
    EV_DEBUG << "Socket data arrived on connection " << connId << ". Message=" << msg->getName() << ", kind=" << msg->getKind() << endl;

    //--modified by wangqian, 2012-05-24
    // call the message handler to process the message.
    cMessage *reply = handleReceivedMessage(msg);
    if (reply!=NULL)
    {
        /*
         * add delay time to send reply, in order to make HOL possible
         */
//        socket->send(reply); // Send to socket if the reply is non-zero.
        double replyDelay;
        replyDelay = (double)(rdReplyDelay->draw());
        //--added by wangqian, 2012-05-17
        EV_DEBUG << "Need to send message on socket " << socket << ". Message=" << reply->getName() << ", kind=" << reply->getKind() <<", sendDelay = " << replyDelay << endl;
        //--added end
        if (replyDelay == 0)
        {
            // if there is no other msg wait to send, send it directly
            if (replyInfoPerSocket.find(socket) == replyInfoPerSocket.end() || replyInfoPerSocket[socket].empty())
            {
                socket->send(reply); // Send to socket if the reply is non-zero.
            }
            else
            {
                /*
                 * check if there's earlier reply not send
                 */
                bool readyToSend = true;
                for (HttpReplyInfoQueue::iterator iter = replyInfoPerSocket[socket].begin(); iter != replyInfoPerSocket[socket].end(); iter++)
                {
                    readyToSend &= iter->readyToSend;
                }
                if (readyToSend)
                {
                    socket->send(msg); // Send to socket if the reply is non-zero.
                }
                // there is earlier reply not send, record this reply
                else
                {
                    HttpReplyMessage *replyMsg = dynamic_cast<HttpReplyMessage*>(reply);
                    ReplyInfo replyInfo = {replyMsg, true};
                    replyInfoPerSocket[socket].push_back(replyInfo);
                }
            }
        }
        else
        {
            HttpReplyMessage *replyMsg = dynamic_cast<HttpReplyMessage*>(reply);
            ReplyInfo replyInfo = {replyMsg, false};
            if (replyInfoPerSocket.find(socket) == replyInfoPerSocket.end())
            {
                HttpReplyInfoQueue tempQueue;
                tempQueue.clear();
                tempQueue.push_back(replyInfo);
                replyInfoPerSocket[socket] = tempQueue;
            }
            else
            {
                replyInfoPerSocket[socket].push_back(replyInfo);
            }

            reply->setKind(HTTPT_DELAYED_RESPONSE_MESSAGE);
            scheduleAt(simTime()+replyDelay, reply);
        }
    }
    delete msg; // Delete the received message here. Must not be deleted in the handler!
}

void PipeHttpServer::handleSelfMessages(cMessage *msg)
{
    switch (msg->getKind())
    {
        case HTTPT_DELAYED_RESPONSE_MESSAGE:
            handleSelfDelayedReplyMessage(msg);
            break;
    }
}

void PipeHttpServer::handleSelfDelayedReplyMessage(cMessage *msg)
{
    EV_DEBUG << "Need to send delayed message " << msg->getName() << " @ T=" << simTime() << endl;
    HttpReplyMessage* reply = dynamic_cast<HttpReplyMessage*>(msg);
    reply->setKind(HTTPT_RESPONSE_MESSAGE);

    TCPSocket *socket = NULL;
    HttpReplyInfoQueue::iterator thisIter;
    if (!replyInfoPerSocket.empty())
    {
        for (TCPSocket_ReplyInfo_Map::iterator sockIter = replyInfoPerSocket.begin(); (socket == NULL && sockIter != replyInfoPerSocket.end()); sockIter++)
        {
            for (HttpReplyInfoQueue::iterator iter = sockIter->second.begin(); iter != sockIter->second.end(); iter++)
            {
                EV_DEBUG << "######iter->reply" << iter->reply << " reply" << reply << endl;
                if (iter->reply == reply)
                {
                    socket = sockIter->first;
                    thisIter = iter;
                    break;
                }
            }
        }
        if (socket != NULL)
        {
            /*
             * check if there's earlier reply not send
             */
            bool readyToSend = true;
            unsigned int earlierCount = 0;
        //    for (HttpReplyInfoQueue::iterator iter = replyInfoPerSocket[socket].begin(), earlierCount = 0; iter != thisIter; iter++, earlierCount++)
            for (HttpReplyInfoQueue::iterator iter = replyInfoPerSocket[socket].begin(); iter != thisIter; iter++)
            {
                readyToSend &= iter->readyToSend;
                earlierCount++;
            }
            EV_DEBUG << "Sending " << earlierCount << " earlier reply messages " << " @ T=" << simTime() << endl;
            if (readyToSend)
            {
                //send the earlier replies
                for (unsigned int i = 1; i <= earlierCount; i++)
                {
                    msg = dynamic_cast<cMessage*>(replyInfoPerSocket[socket].front().reply);
                    socket->send(msg); // Send to socket if the reply is non-zero.
                    EV_INFO << "Send earlier message # " << i << ". Message=" << msg->getName() << ", kind=" << msg->getKind()<< endl;
                    replyInfoPerSocket[socket].pop_front();
                }

                msg = dynamic_cast<cMessage*>(reply);
                socket->send(msg); // Send to socket if the reply is non-zero.
                EV_INFO << "Send message =" << msg->getName() << ", kind=" << msg->getKind()<< endl;
                replyInfoPerSocket[socket].pop_front();

                while (!replyInfoPerSocket[socket].empty())
                {
                    if (replyInfoPerSocket[socket].front().readyToSend)
                    {
                        msg = dynamic_cast<cMessage*>(replyInfoPerSocket[socket].front().reply);
                        socket->send(msg); // Send to socket if the reply is non-zero.
                        EV_INFO << "Send message =" << msg->getName() << ", kind=" << msg->getKind()<< endl;
                        replyInfoPerSocket[socket].pop_front();
                    }
                    else
                    {
                        return;
                    }
                }
            }
            // there is earlier reply not send, record this reply
            else
            {
                thisIter->readyToSend = true;
            }
        }
        else
        {
            EV_DEBUG << "Cannot find socket"<< endl;
        }
    }
    else
    {
        EV_DEBUG << "!!!!!!replyInfoPerSocket is empty"<< endl;
    }
}

void PipeHttpServer::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        handleSelfMessages(msg);
    }
    else
    {
        EV_INFO << "Handle inbound message " << msg->getName() << " of kind " << msg->getKind() << endl;
        TCPSocket *socket = sockCollection.findSocketFor(msg);
        if (!socket)
        {
            EV_DEBUG << "No socket found for the message. Create a new one" << endl;
            // new connection -- create new socket object and server process
            socket = new TCPSocket(msg);
            socket->setOutputGate(gate("tcpOut"));
            socket->setDataTransferMode(TCP_TRANSFER_OBJECT);
            socket->setCallbackObject(this, socket);
            sockCollection.addSocket(socket);
        }
        EV_DEBUG << "Process the message " << msg->getName() << endl;
        socket->processMessage(msg);
    }
    updateDisplay();
}
