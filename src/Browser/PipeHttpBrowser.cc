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

#include "PipeHttpBrowser.h"
#include "CFFPipeReq.h"
#include "CDFPipeReq.h"
#include "CPCSvrSupportDetect.h"
#include "CPHSvrSupportDetect.h"
#include <stdexcept>

Define_Module(PipeHttpBrowser);

PipeHttpBrowser::PipeHttpBrowser()
{
    maxConnections = 0;
    maxConnectionsPerHost = 0;
    maxPipelinedReqs = 0;
    pipeliningMode = 0;
    SvrSupportDetect = 0;

    pPipeReq = NULL;
    pSvrSupportDetect = NULL;
}

PipeHttpBrowser::~PipeHttpBrowser()
{
    if (NULL != pPipeReq)
    {
        delete pPipeReq;
        pPipeReq = NULL;
    }

    if (NULL != pSvrSupportDetect)
    {
        delete pSvrSupportDetect;
        pSvrSupportDetect = NULL;
    }
}

void PipeHttpBrowser::initialize(int stage)
{
    HttpBrowser::initialize(stage);

    //--added by wangqian, 2012-05-15
    if (stage==0)
    {
        maxConnections = par("maxConnections");
        maxConnectionsPerHost = par("maxConnectionsPerHost");
        maxPipelinedReqs = par("maxPipelinedReqs");
        pipeliningMode = par("pipeliningMode");
        //--added by wangqian, 2012-05-16
        SvrSupportDetect = par("SvrSupportDetect");
        //--added end

        //--added by wangqian, 2012-05-15
        sockCollectionMap.clear();    ///< List of active sockets for each server

        chooseStrategy(pipeliningMode, SvrSupportDetect);

    }
    //--added end
}

void PipeHttpBrowser::socketEstablished(int connId, void *yourPtr)
{
    EV_DEBUG << "Socket with id " << connId << " established" << endl;

    socketsOpened++;

    if (yourPtr==NULL)
    {
        EV_ERROR << "SocketEstablished failure. Null pointer" << endl;
        return;
    }

    // Get the socket and associated data structure.
    PipeSockData *sockdata = (PipeSockData*)yourPtr;
    TCPSocket *socket = sockdata->socket;
    //--modified by wangqian, 2012-05-15
    std::string svrName(sockdata->svrName);
    EV_DEBUG << "Socket with server " << svrName.c_str() << endl;
//
//    if (reqListPerSvr.empty())
//    {
//        EV_ERROR << "No req" << endl;
//        return;
//    }

    if (pPipeReq->isEmpty(svrName, socket))
    {
        EV_INFO << "No data to send on socket with connection id " << connId << ". Closing" << endl;
        socket->close();
        return;
    }

    // Send pending messages on the established socket.
    cMessage *msg;
    EV_DEBUG << "Proceeding to send messages on socket " << connId << endl;

    //--modified by wangqian, 2012-05-16
    /*
     * This is the first time to send http request in the TCP connection
     * Need to detect if the server support pipelining, so just send a single request
     */
    if (!pPipeReq->isEmpty(svrName, socket))
    {
        msg = pPipeReq->getMsg(svrName, socket);
        cPacket *pckt = check_and_cast<cPacket *>(msg);
        EV_INFO << "Submitting request when established " << msg->getName() << " to socket " << connId << ". size is " << pckt->getByteLength() << " bytes" << endl;
        socket->send(msg);
        //--add by wangqian, 2012-07-04
        // add msg log when send it
        //static_cast<const HttpRequestMessage *>(msg)
//        const HttpRequestMessage* httpRequest;
//        logRequest(httpRequest);
        //--add end
        sockdata->pending++;
    }
    //--modified end

    //--modified end
}

void PipeHttpBrowser::socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent)
{
    EV_DEBUG << "Socket data arrived on connection " << connId << ": " << msg->getName() << endl;
    if (yourPtr==NULL)
    {
        EV_ERROR << "socketDataArrivedfailure. Null pointer" << endl;
        return;
    }

    PipeSockData *sockdata = (PipeSockData*)yourPtr;
    TCPSocket *socket = sockdata->socket;

    HttpContentType contentType = pSvrSupportDetect->setSvrSupportForSock(sockdata, msg);

    handleDataMessage(msg);

    //--added by wangqian, 2012-05-15
    --sockdata->pending;

    std::string svrName(sockdata->svrName);
    cMessage *sendMsg;

    //--modified by wangqian, 2012-05-16
    /*
     * if received the response for html, send a single request directly
     */
    if (contentType == CT_HTML)
    {
        if (!pPipeReq->isEmpty(svrName, socket))
        {
            sendMsg = pPipeReq->getMsg(svrName, socket);
            cPacket *pckt = check_and_cast<cPacket *>(sendMsg);
            EV_INFO << "Submitting request first use html connection " << sendMsg->getName() << " to socket " << connId << ". size is " << pckt->getByteLength() << " bytes" << endl;
            socket->send(sendMsg);
            //--add by wangqian, 2012-07-04
            // add msg log when send it
//            logRequest(check_and_cast<HttpRequestMessage *>(sendMsg));
            //--add end
            sockdata->pending++;
        }
    }
    /*
     * otherwise send requests depends on the server supporting
     */
    else
    {
        /*
         * if the server supports pipelining do it, else send single request
         */
        if (sockdata->svrSupport == e_Support)
        {
            while (!pPipeReq->isEmpty(svrName, socket) && sockdata->pending <= maxPipelinedReqs)
            {
                sendMsg = pPipeReq->getMsg(svrName, socket);
                cPacket *pckt = check_and_cast<cPacket *>(sendMsg);
                EV_INFO << "Submitting request pipelined " << sendMsg->getName() << " to socket " << connId << ". size is " << pckt->getByteLength() << " bytes" << endl;
                socket->send(sendMsg);
                //--add by wangqian, 2012-07-04
                // add msg log when send it
//                logRequest(check_and_cast<HttpRequestMessage *>(sendMsg));
                //--add end
                sockdata->pending++;
            }
        }
        else if (sockdata->svrSupport == e_NotSupport)
        {
            if (!pPipeReq->isEmpty(svrName, socket))
            {
                sendMsg = pPipeReq->getMsg(svrName, socket);
                cPacket *pckt = check_and_cast<cPacket *>(sendMsg);
                EV_INFO << "Submitting request not pipelined " << sendMsg->getName() << " to socket " << connId << ". size is " << pckt->getByteLength() << " bytes" << endl;
                socket->send(sendMsg);
                //--add by wangqian, 2012-07-04
                // add msg log when send it
//                logRequest(check_and_cast<HttpRequestMessage *>(sendMsg));
                //--add end
                sockdata->pending++;
            }
        }
        else
        {
            EV_DEBUG << "Server supporting not recognised." << endl;
        }
    }
    //--modified end

    //--added end

    //--modified by wangqian, 2012-05-15
//    if (--sockdata->pending==0)
    if (sockdata->pending == 0 && pPipeReq->isEmpty(svrName, socket))
    {
        EV_DEBUG << "Received last expected reply on this socket　and there is no resource need to request. Issing a close" << endl;
        socket->close();
    }
    //--modified end

    // Message deleted in handler - do not delete here!
}

void PipeHttpBrowser::socketPeerClosed(int connId, void *yourPtr)
{
    EV_DEBUG << "Socket " << connId << " closed by peer" << endl;
    if (yourPtr==NULL)
    {
        EV_ERROR << "socketPeerClosed failure. Null pointer" << endl;
        return;
    }

    PipeSockData *sockdata = (PipeSockData*)yourPtr;
    TCPSocket *socket = sockdata->socket;

    // close the connection (if not already closed)
    if (socket->getState()==TCPSocket::PEER_CLOSED)
    {
        EV_INFO << "remote TCP closed, closing here as well. Connection id is " << connId << endl;
        socket->close();
    }
}

void PipeHttpBrowser::socketClosed(int connId, void *yourPtr)
{
    EV_INFO << "Socket " << connId << " closed" << endl;

    if (yourPtr==NULL)
    {
        EV_ERROR << "socketClosed failure. Null pointer" << endl;
        return;
    }

    PipeSockData *sockdata = (PipeSockData*)yourPtr;
    TCPSocket *socket = sockdata->socket;
    sockCollection.removeSocket(socket);
    //--added by wangqian, 2012-05-16
    std::string svrName(sockdata->svrName);
    sockCollectionMap[svrName].erase(socket);
    //--added end
    delete socket;
    delete sockdata;
}

void PipeHttpBrowser::socketFailure(int connId, void *yourPtr, int code)
{
    EV_WARNING << "connection broken. Connection id " << connId << endl;
    numBroken++;

    if (yourPtr==NULL)
    {
        EV_ERROR << "socketFailure failure. Null pointer" << endl;
        return;
    }

    if (code==TCP_I_CONNECTION_RESET)
        EV_WARNING << "Connection reset!\n";
    else if (code==TCP_I_CONNECTION_REFUSED)
        EV_WARNING << "Connection refused!\n";

    PipeSockData *sockdata = (PipeSockData*)yourPtr;
    TCPSocket *socket = sockdata->socket;
    sockCollection.removeSocket(socket);
    //--added by wangqian, 2012-05-16
    std::string svrName(sockdata->svrName);
    sockCollectionMap[svrName].erase(socket);
    //--added end
    delete socket;
    delete sockdata;
}


void PipeHttpBrowser::submitToSocket(const char* moduleName, int connectPort, HttpRequestMessage *msg)
{
    // Create a queue and push the single message
    HttpRequestQueue queue;
    queue.push_back(msg);
    // Call the overloaded version with the queue as parameter
    submitToSocket(moduleName, connectPort, queue);
}

void PipeHttpBrowser::submitToSocket(const char* moduleName, int connectPort, HttpRequestQueue &queue)
{
    // Dont do anything if the queue is empty.s
    if (queue.empty())
    {
        EV_INFO << "Submitting to socket. No data to send to " << moduleName << ". Skipping connection." << endl;
        return;
    }

    EV_DEBUG << "Pipe Submitting to socket. Module: " << moduleName << ", port: " << connectPort << ". Total messages: " << queue.size() << endl;

    //--modified by wangqian, 2012-05-15

    std::string svrName(moduleName);

    EV_DEBUG << "Submitting server name " << svrName.c_str() << ". " << endl;

    /*
     * Create and initialize sockets if there's not enough socket to use
     * Issue connect to the created sockets for the specified module and port.
     */
    if (maxConnectionsPerHost > sockCollectionMap[svrName].size())
    {
        unsigned int lackConnection = std::min((maxConnectionsPerHost - sockCollectionMap[svrName].size()), queue.size());
        EV_DEBUG << "Need to create " << lackConnection << " more TCP connections。"<< endl;
        for (unsigned int i = 0; i < lackConnection; i++)
        {
            // Create and initialize the socket
            TCPSocket *socket = new TCPSocket();
            socket->setDataTransferMode(TCP_TRANSFER_OBJECT);
            socket->setOutputGate(gate("tcpOut"));
            sockCollection.addSocket(socket);
            sockCollectionMap[svrName].insert(socket);

            // Initialize the associated data structure
            PipeSockData *sockdata = new PipeSockData;
    //        sockdata->messageQueue = HttpRequestQueue(queue);
            sockdata->svrName.assign(moduleName);
            sockdata->socket = socket;
            sockdata->pending = 0;
            pSvrSupportDetect->initSvrSupportForSock(sockdata);
            socket->setCallbackObject(this, sockdata);

            // Issue a connect to the socket for the specified module and port.
            socket->connect(IPvXAddressResolver().resolve(moduleName), connectPort);
        }
    }
    else
    {
        EV_DEBUG << "Don't need to create more TCP connections。"<< endl;
    }
    //--modified end

    HttpRequestMessage *msg;
    while (!queue.empty())
    {
        msg = queue.back();
        pPipeReq->addMsg(msg, svrName, sockCollectionMap[svrName]);
        queue.pop_back();
//            EV_DEBUG << "Submitting request pipelined " << msg->getName() << " to socket " << connId << ". size is " << pckt->getByteLength() << " bytes" << endl;
    }
}

/*************************************************************************
 * Function：    chooseStrategy
 * Author:      Wang Qian
 * Description:
 *      策略选择，确定派生类
 * Args：
 *      Pipelining_Mode_Type pipeliningMode:          流水线调度模式
 *      SvrSupportDetect_Method_Type SvrSupportDetect:     服务器探测模式
 * Return Values:
 *          无
 *************************************************************************/
void PipeHttpBrowser::chooseStrategy(Pipelining_Mode_Type pipeliningMode, SvrSupportDetect_Method_Type SvrSupportDetect)
{
    switch(pipeliningMode)
    {
        case FILL_FIRST:
        {
            pPipeReq = new CFFPipeReq();
            break;
        }
        case DISTRIBUTE_FIRST:
        {
            pPipeReq = new CDFPipeReq();
            break;
        }
        default:
        {
            throw std::runtime_error("[In PipeHttpBrowser] Browser get unrecognizable pipeling mode, can not work!");
        }
    }

    switch(SvrSupportDetect)
    {
        case PER_CONNECTION:
        {
            pSvrSupportDetect = new CPCSvrSupportDetect();
            break;
        }
        case PER_HOST:
        {
            pSvrSupportDetect = new CPHSvrSupportDetect();
            break;
        }
        default:
        {
            throw std::runtime_error("[In PipeHttpBrowser] Browser get unrecognizable server support detect method, can not work!");
        }
    }
}

