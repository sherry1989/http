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

#include "NSCHttpBrowser.h"
#include "CFFPipeReq.h"
#include "CDFPipeReq.h"
#include "CPCSvrSupportDetect.h"
#include "CPHSvrSupportDetect.h"
#include "ByteArrayMessage.h"

#include <stdexcept>
#include <iomanip>
#include <memory.h>

Define_Module(NSCHttpBrowser);

NSCHttpBrowser::NSCHttpBrowser()
{
    maxConnections = 0;
    maxConnectionsPerHost = 0;
    maxPipelinedReqs = 0;
    pipeliningMode = 0;
    SvrSupportDetect = 0;

    pPipeReq = NULL;
    pSvrSupportDetect = NULL;

}

NSCHttpBrowser::~NSCHttpBrowser()
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

void NSCHttpBrowser::initialize(int stage)
{
    HttpBrowser::initialize(stage);

    if (stage==0)
    {
        maxConnections = par("maxConnections");
        maxConnectionsPerHost = par("maxConnectionsPerHost");
        maxPipelinedReqs = par("maxPipelinedReqs");
        pipeliningMode = par("pipeliningMode");
        SvrSupportDetect = par("SvrSupportDetect");
        protocolType = par("protocolType");

        sockCollectionMap.clear();    ///< List of active sockets for each server

        chooseStrategy(pipeliningMode, SvrSupportDetect);

        // Initialize statistics
        responseMessageReceived = responseParsed = 0;
        totalBytesSent = 0;
        if (protocolType != HTTP)
        {
            headerBytesBeforeDeflate = headerBytesAfterDeflate = 0;
        }

        // Initialize watches
        WATCH(htmlRequested);
        WATCH(htmlReceived);
        WATCH(htmlErrorsReceived);
        WATCH(imgResourcesRequested);
        WATCH(imgResourcesReceived);
        WATCH(textResourcesRequested);
        WATCH(textResourcesReceived);
        WATCH(messagesInCurrentSession);
        WATCH(connectionsCount);
        WATCH(sessionCount);

        WATCH(reqInCurSession);
        WATCH(reqNoInCurSession);

        WATCH(numBroken);
        WATCH(socketsOpened);

        WATCH(responseMessageReceived);
        WATCH(responseParsed);

        WATCH(totalBytesSent);
        if (protocolType != HTTP)
        {
            WATCH(headerBytesBeforeDeflate);
            WATCH(headerBytesAfterDeflate);
        }

        // Initialize cOutputVectors
        recvResTimeVec.setName("Receive Response SimTime");
        sendReqTimeVec.setName("Send Request SimTime");
        headerDeflateRatioVec.setName("Header Deflate Ratio");
        totalDeflateRatioVec.setName("Total Deflate Ratio");
    }
}

void NSCHttpBrowser::finish()
{
    // Call the parent class finish. Takes care of most of the reporting.
    HttpBrowser::finish();

    // Report sockets related statistics.
    EV_SUMMARY << "Response Message Received: " << responseMessageReceived << endl;
    EV_SUMMARY << "Response Parsed: " << responseParsed << endl;
    EV_SUMMARY << "Total Bytes Sent: " << totalBytesSent << endl;
    if (protocolType != HTTP)
    {
        EV_SUMMARY << "Bytes Before Deflate: " << headerBytesBeforeDeflate << endl;
        EV_SUMMARY << "Bytes After Deflate: " << headerBytesAfterDeflate << endl;
    }

    // Record the sockets related statistics
    recordScalar("response.messageRec", responseMessageReceived);
    recordScalar("response.parsed", responseParsed);
    recordScalar("bytes.totalBytesSent", totalBytesSent);
    if (protocolType != HTTP)
    {
        recordScalar("bytes.beforeDeflate", headerBytesBeforeDeflate);
        recordScalar("bytes.AfterDeflate", headerBytesAfterDeflate);
    }
}

void NSCHttpBrowser::socketEstablished(int connId, void *yourPtr)
{
    EV_DEBUG << "Socket with id " << connId << " established" << endl;

    socketsOpened++;

    if (yourPtr==NULL)
    {
        EV_ERROR << "SocketEstablished failure. Null pointer" << endl;
        return;
    }

    // Get the socket and associated data structure.
    NSCSockData *sockdata = (NSCSockData*)yourPtr;
    TCPSocket *socket = sockdata->socket;

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

    /*
     * This is the first time to send http request in the TCP connection
     * Need to detect if the server support pipelining, so just send a single request
     */
    if (!pPipeReq->isEmpty(svrName, socket))
    {
        msg = pPipeReq->getMsg(svrName, socket);
        cPacket *pckt = check_and_cast<cPacket *>(msg);
        EV_INFO << "Submitting request when established " << msg->getName() << " to socket " << connId << ". size is " << pckt->getByteLength() << " bytes" << endl;

        // change message send method to adapt to the use of NSC
        //socket->send(msg);
        sendMessage(sockdata, check_and_cast<HttpRequestMessage *>(msg));

        sockdata->pending++;
    }
}

void NSCHttpBrowser::socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent)
{
    EV_DEBUG << "Socket data arrived on connection " << connId << ": " << msg->getName() << endl;
    if (yourPtr==NULL)
    {
        EV_ERROR << "socketDataArrivedfailure. Null pointer" << endl;
        return;
    }

    responseMessageReceived++;

    NSCSockData *sockdata = (NSCSockData*)yourPtr;
    TCPSocket *socket = sockdata->socket;

    if (sockdata->praser == NULL)
    {
        sockdata->praser = new HttpResponsePraser();
        EV_DEBUG << "New a HttpResponsePraser" << endl;
    }
    else
    {
        EV_DEBUG << "Use an old HttpResponsePraser" << endl;
    }

    cPacket *prasedMsg = sockdata->praser->praseHttpResponse(msg, protocolType, &(sockdata->zlib.inflater));

    /*
     * check if this response message is complete
     *      if it's a complete message, handle it
     *      else, wait for the coming messages contain the else message body, do nothing
     */
    if (prasedMsg != NULL)
    {
        responseParsed++;
        EV_DEBUG << "response to parse is No. " << responseParsed << endl;

        HttpContentType contentType = CT_UNKNOWN;
        contentType = pSvrSupportDetect->setSvrSupportForSock(sockdata, prasedMsg);

        handleDataMessage(prasedMsg);
        recvResTimeVec.record(simTime());

        delete sockdata->praser;
        sockdata->praser = NULL;

        --sockdata->pending;

        std::string svrName(sockdata->svrName);
        cMessage *sendMsg;

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

                // change message send method to adapt to the use of NSC
                //socket->send(sendMsg);
                sendMessage(sockdata, check_and_cast<HttpRequestMessage *>(sendMsg));

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
                while (!pPipeReq->isEmpty(svrName, socket) && sockdata->pending < maxPipelinedReqs)
                {
                    sendMsg = pPipeReq->getMsg(svrName, socket);
                    cPacket *pckt = check_and_cast<cPacket *>(sendMsg);
                    EV_INFO << "Submitting request pipelined " << sendMsg->getName() << " to socket " << connId << ". size is " << pckt->getByteLength() << " bytes" << endl;

                    // change message send method to adapt to the use of NSC
                    //socket->send(sendMsg);
                    sendMessage(sockdata, check_and_cast<HttpRequestMessage *>(sendMsg));

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

                    // change message send method to adapt to the use of NSC
                    //socket->send(sendMsg);
                    sendMessage(sockdata, check_and_cast<HttpRequestMessage *>(sendMsg));

                    sockdata->pending++;
                }
            }
            else
            {
                EV_ERROR << "Server supporting not recognised." << endl;
            }
        }

    //    if (--sockdata->pending==0)
        if (sockdata->pending == 0 && pPipeReq->isEmpty(svrName, socket))
        {
            EV_DEBUG << "Received last expected reply on this socket　and there is no resource need to request. Issing a close" << endl;
            socket->close();
        }
        else
        {
            EV_DEBUG << "Cannot close a socket. sockdata pending now is " << sockdata->pending << endl;
        }

        // Message deleted in handler - do not delete here!
    }
    else
    {
        EV_DEBUG << "Need to wait for the following msgs" << endl;
    }
}

void NSCHttpBrowser::socketPeerClosed(int connId, void *yourPtr)
{
    EV_DEBUG << "Socket " << connId << " closed by peer" << endl;
    if (yourPtr==NULL)
    {
        EV_ERROR << "socketPeerClosed failure. Null pointer" << endl;
        return;
    }

    NSCSockData *sockdata = (NSCSockData*)yourPtr;
    TCPSocket *socket = sockdata->socket;

    // close the connection (if not already closed)
    if (socket->getState()==TCPSocket::PEER_CLOSED)
    {
        EV_INFO << "remote TCP closed, closing here as well. Connection id is " << connId << endl;
        socket->close();
    }
}

void NSCHttpBrowser::socketClosed(int connId, void *yourPtr)
{
    EV_INFO << "Socket " << connId << " closed" << endl;

    if (yourPtr==NULL)
    {
        EV_ERROR << "socketClosed failure. Null pointer" << endl;
        return;
    }

    NSCSockData *sockdata = (NSCSockData*)yourPtr;

    spdylay_zlib_deflate_free(&(sockdata->zlib.deflater));
    spdylay_zlib_inflate_free(&(sockdata->zlib.inflater));

    TCPSocket *socket = sockdata->socket;
    sockCollection.removeSocket(socket);

    std::string svrName(sockdata->svrName);
    sockCollectionMap[svrName].erase(socket);

    delete socket;
    delete sockdata;
}

void NSCHttpBrowser::socketFailure(int connId, void *yourPtr, int code)
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

    NSCSockData *sockdata = (NSCSockData*)yourPtr;

    spdylay_zlib_deflate_free(&(sockdata->zlib.deflater));
    spdylay_zlib_inflate_free(&(sockdata->zlib.inflater));

    TCPSocket *socket = sockdata->socket;
    sockCollection.removeSocket(socket);

    std::string svrName(sockdata->svrName);
    sockCollectionMap[svrName].erase(socket);

    delete socket;
    delete sockdata;
}

void NSCHttpBrowser::submitToSocket(const char* moduleName, int connectPort, HttpRequestQueue &queue)
{
    // Dont do anything if the queue is empty.s
    if (queue.empty())
    {
        EV_INFO << "Submitting to socket. No data to send to " << moduleName << ". Skipping connection." << endl;
        return;
    }

    EV_DEBUG << "Pipe Submitting to socket. Module: " << moduleName << ", port: " << connectPort << ". Total messages: " << queue.size() << endl;

    std::string svrName(moduleName);

    EV_DEBUG << "Submitting server name " << svrName.c_str() << ". " << endl;

    /*
     * check if there is closed socket, if exist, delete the information related to it
     */
    for (SocketSet::iterator iter = sockCollectionMap[svrName].begin(); iter != sockCollectionMap[svrName].end(); )
    {
        if ( (*iter)->getState() != TCPSocket::CONNECTED && (*iter)->getState() != TCPSocket::CONNECTING)
        {
            EV_DEBUG << "Delete undeleted socket from server " << svrName.c_str() << ". Socket state is" << (*iter)->getState() << endl;
            sockCollection.removeSocket(*iter);
            delete *iter;
            sockCollectionMap[svrName].erase(iter++);
        }
        else
        {
            EV_DEBUG << "Don't need to delete socket from server " << svrName.c_str() << ". Socket state is" << (*iter)->getState() << endl;
            iter++;
        }
    }


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
            // change the dataTransferMode from TCP_TRANSFER_OBJECT to TCP_TRANSFER_BYTESTREAM to support NSC TCP
            socket->setDataTransferMode(TCP_TRANSFER_BYTESTREAM);
            socket->setOutputGate(gate("tcpOut"));
            sockCollection.addSocket(socket);
            sockCollectionMap[svrName].insert(socket);

            // Initialize the associated data structure
            NSCSockData *sockdata = new NSCSockData();
    //        sockdata->messageQueue = HttpRequestQueue(queue);
            sockdata->svrName.assign(moduleName);
            sockdata->socket = socket;
            sockdata->pending = 0;
            sockdata->praser = NULL;

            /*
             * Initialize the zlib things
             */
            int rvD = spdylay_zlib_deflate_hd_init(&(sockdata->zlib.deflater), 1, SPDYLAY_PROTO_SPDY3);
            if (rvD != 0)
            {
                EV_ERROR << "spdylay_zlib_deflate_hd_init failed!" << endl;
            }

            int rvI = spdylay_zlib_inflate_hd_init(&(sockdata->zlib.inflater), SPDYLAY_PROTO_SPDY3);
            if (rvI != 0)
            {
                EV_ERROR_NOMODULE << "spdylay_zlib_inflate_hd_init failed!" << endl;
            }
            sockdata->zlib.setZlib = !(rvD | rvI);


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
void NSCHttpBrowser::chooseStrategy(Pipelining_Mode_Type pipeliningMode, SvrSupportDetect_Method_Type SvrSupportDetect)
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

/** send HTTP requesst message though TCPSocket depends on the TCP DataTransferMode*/
void NSCHttpBrowser::sendMessage(NSCSockData *sockdata, HttpRequestMessage *req)
{
    long sendBytes = 0;
    cMessage *sendMsg = NULL;

    TCPSocket *socket = sockdata->socket;

    // add msg log when send it
    EV_INFO << "Sending request " << req->getName() << " to socket. Size is " << req->getByteLength() << " bytes" << endl;

    switch (socket->getDataTransferMode())
    {
        case TCP_TRANSFER_BYTECOUNT:
        case TCP_TRANSFER_OBJECT:
        {
            sendMsg = dynamic_cast<cMessage*>(req);
            socket->send(sendMsg);
            break;
        }

        case TCP_TRANSFER_BYTESTREAM:
        {
            ByteArrayMessage *byMsg = new ByteArrayMessage(req->getName());

            std::string resByteArray = formatByteRequestMessage(sockdata, req);

            sendBytes = resByteArray.length();

            char *ptr = new char[sendBytes];
            ptr[0] = '\0';
            memcpy(ptr, resByteArray.c_str(), sendBytes);
            byMsg->getByteArray().assignBuffer(ptr, sendBytes);
            byMsg->setByteLength(sendBytes);

//            delete[] ptr;

            EV_DEBUG << "Send ByteArray. MessageName is: " << byMsg->getName() << ". " << endl;
            EV_DEBUG << "Send ByteArray. SendBytes are: " << resByteArray << ". " << endl;
            EV_DEBUG << "Send ByteArray. Bytelength is: " << sendBytes << ". " << endl;

            sendMsg = dynamic_cast<cMessage*>(byMsg);
            socket->send(sendMsg);

            // doing statistics
            sendReqTimeVec.record(simTime());
            totalBytesSent += sendBytes;

//            //#################   added for debug, should be deleted
//            HttpRequestPraser *praser = new HttpRequestPraser();
//
//            cPacket *prasedMsg = praser->praseHttpRequest(dynamic_cast<cPacket*>(sendMsg), protocolType);
//
//            EV_DEBUG << "try to prase the sendmsg " << prasedMsg->getName() << ", kind=" << prasedMsg->getKind() << endl;
//            //#################   added end


            break;
        }

        default:
            throw cRuntimeError("Invalid TCP data transfer mode: %d", socket->getDataTransferMode());
    }
}

/*
 * Format an application TCP_TRANSFER_BYTESTREAM response message which can be sent though NSC TCP depence on the application layer protocol
 * the protocol type can be HTTP \ SPDY \ HTTPSM \ HTTPNF
 */
std::string NSCHttpBrowser::formatByteRequestMessage(NSCSockData *sockdata, HttpRequestMessage *httpRequest)
{
    RealHttpRequestMessage *realHttpRequest = changeRequestToReal(httpRequest);

    std::string reqHeader;

    switch(protocolType)
    {
        case HTTP:
            reqHeader = formatHttpRequestMessageHeader(realHttpRequest);
            break;
        case SPDY_ZLIB_HTTP:
            reqHeader = formatSpdyZlibHttpRequestMessageHeader(sockdata, realHttpRequest);
            break;
        case SPDY_HEADER_BLOCK:
            reqHeader = formatSpdyZlibHeaderBlockRequestMessageHeader(realHttpRequest);
            break;
        case HTTPNF:
            reqHeader = formatHttpNFRequestMessageHeader(realHttpRequest);
            break;
        default:
            throw cRuntimeError("Invalid Application protocol: %d", protocolType);
    }


    /*************************************Generate HTTP Request Body*************************************/

    reqHeader.append(realHttpRequest->payload());

    /*************************************Finish generating HTTP Request Body*************************************/

    delete realHttpRequest;
    realHttpRequest = NULL;

    return reqHeader;
}

/*
 * Format a Request message to HTTP Request Message Header
 * Request   = Request-Line
                *(( general-header)
                | request-header
                | entity-header)CRLF)
                CRLF
                [ message-body ]
 */
std::string NSCHttpBrowser::formatHttpRequestMessageHeader(const RealHttpRequestMessage *httpRequest)
{
    std::ostringstream str;

    /*************************************Generate HTTP Request Header*************************************/

    /**
     * 1.Generate Request-Line:
     * Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
     */

//    /** 1.1 Method SP */
//    str << "GET ";
//
//    /** 1.2 Request-URI SP */
//    str << httpRequest->targetUrl() << " ";
//
//
//    /** 1.3 HTTP-Version CRLF */
//    switch (httpRequest->protocol())
//    {
//      case 10:
//        str << "HTTP/1.0";
//        break;
//      case 11:
//        str << "HTTP/1.1";
//        break;
//      default:
//        throw cRuntimeError("Invalid HTTP Status code: %d", httpRequest->protocol());
//        break;
//    }
    str << httpRequest->heading();

    str << "\r\n";


    /**
     * 2.Generate general-header:
     * general-header = Cache-Control
                        | Connection
                        | Date
                        | Pragma
                        | Trailer
                        | Transfer-Encoding
                        | Upgrade
                        | Via
                        | Warning
     */

    /** 2.1 Cache-Control */
    /*
     * The Cache-Control general-header field is used to specify directives
       that MUST be obeyed by all caching mechanisms along the
       request/Request chain.
     * Cache-Control   = "Cache-Control" ":" 1#cache-directive
       cache-directive = cache-request-directive
                         | cache-Request-directive
       cache-request-directive = "no-cache"
                                 | "no-store"
                                 | "max-age" "=" delta-seconds
                                 | "max-stale" [ "=" delta-seconds ]
                                 | "min-fresh" "=" delta-seconds
                                 | "no-transform"
                                 | "only-if-cached"
                                 | cache-extension
       cache-Request-directive =  "public"
                                   | "private" [ "=" <"> 1#field-name <"> ]
                                   | "no-cache" [ "=" <"> 1#field-name <"> ]
                                   | "no-store"
                                   | "no-transform"
                                   | "must-revalidate"
                                   | "proxy-revalidate"
                                   | "max-age" "=" delta-seconds
                                   | "s-maxage" "=" delta-seconds
                                   | cache-extension
       cache-extension = token [ "=" ( token | quoted-string ) ]
     */

    /** 2.2 Connection */
    /*
     * HTTP/1.1 applications that do not support persistent connections MUST
       include the "close" connection option in every message.
     * Connection = "Connection" ":" 1#(connection-token)
       connection-token  = token
     */
    /*
     * the Connection headers are not valid and MUST not be send when use SPDY formed fream and use the zlib dictionary
     */
    if (protocolType == HTTP)
    {
        if (httpRequest->keepAlive())
        {
            str << "Connection: Keep-Alive\r\n";
        }
        else
        {
            str << "Connection: close\r\n";
        }
    }

    /** 2.3 Date */
    /*
     * The Date general-header field represents the date and time at which
       the message was originated
     * Date  = "Date" ":" HTTP-date
     */

    /** 2.4 Pragma */
    /*
     * The Pragma general-header field is used to include implementation-
       specific directives that might apply to any recipient along the
       request/Request chain. All pragma directives specify optional
       behavior from the viewpoint of the protocol; however, some systems
       MAY require that behavior be consistent with the directives.
     * Pragma            = "Pragma" ":" 1#pragma-directive
       pragma-directive  = "no-cache" | extension-pragma
       extension-pragma  = token [ "=" ( token | quoted-string ) ]
     */

    /** 2.5 Trailer */
    /*
     * The Trailer general field value indicates that the given set of
       header fields is present in the trailer of a message encoded with
       chunked transfer-coding.
     * Trailer  = "Trailer" ":" 1#field-name
     * An HTTP/1.1 message SHOULD include a Trailer header field in a
       message using chunked transfer-coding with a non-empty trailer. Doing
       so allows the recipient to know which header fields to expect in the
       trailer.
     */

    /** 2.6 Transfer-Encoding */
    /*
     * The Transfer-Encoding general-header field indicates what (if any)
       type of transformation has been applied to the message body in order
       to safely transfer it between the sender and the recipient. This
       differs from the content-coding in that the transfer-coding is a
       property of the message, not of the entity.
     * Transfer-Encoding       = "Transfer-Encoding" ":" 1#transfer-coding
     */

    /** 2.7 Upgrade */
    /*
     * The Upgrade general-header allows the client to specify what
       additional communication protocols it supports and would like to use
       if the server finds it appropriate to switch protocols. The server
       MUST use the Upgrade header field within a 101 (Switching Protocols)
       Request to indicate which protocol(s) are being switched.
     * Upgrade        = "Upgrade" ":" 1#product
     */

    /** 2.8 Via */
    /*
     * The Via general-header field MUST be used by gateways and proxies to
       indicate the intermediate protocols and recipients between the user
       agent and the server on requests, and between the origin server and
       the client on Requests. It is analogous to the "Received" field of
       RFC 822 [9] and is intended to be used for tracking message forwards,
       avoiding request loops, and identifying the protocol capabilities of
       all senders along the request/Request chain.
    * Via =  "Via" ":" 1#( received-protocol received-by [ comment ] )
      received-protocol = [ protocol-name "/" ] protocol-version
      protocol-name     = token
      protocol-version  = token
      received-by       = ( host [ ":" port ] ) | pseudonym
      pseudonym         = token
     */

    /** 2.9 Warning */
    /*
     * The Warning general-header field is used to carry additional
       information about the status or transformation of a message which
       might not be reflected in the message. This information is typically
       used to warn about a possible lack of semantic transparency from
       caching operations or transformations applied to the entity body of
       the message.
     * Warning headers are sent with Requests using:
       Warning    = "Warning" ":" 1#warning-value
       warning-value = warn-code SP warn-agent SP warn-text  [SP warn-date]
       warn-code  = 3DIGIT
       warn-agent = ( host [ ":" port ] ) | pseudonym
                       ; the name or pseudonym of the server adding
                       ; the Warning header, for use in debugging
       warn-text  = quoted-string
       warn-date  = <"> HTTP-date <">
     */


    /**
     * 3. Generate Request-header:
     * request-header = Accept
                      | Accept-Charset
                      | Accept-Encoding
                      | Accept-Language
                      | Authorization
                      | Expect
                      | From
                      | Host
                      | If-Match
                      | If-Modified-Since
                      | If-None-Match
                      | If-Range
                      | If-Unmodified-Since
                      | Max-Forwards
                      | Proxy-Authorization
                      | Range
                      | Referer
                      | TE
                      | User-Agent
     */

    /** 3.1 Accept */
    /*
     * The Accept request-header field can be used to specify certain media
       types which are acceptable for the response. Accept headers can be
       used to indicate that the request is specifically limited to a small
       set of desired types, as in the case of a request for an in-line
       image.
     * Accept         = "Accept" ":"
                        #( media-range [ accept-params ] )
       media-range    = ( "*"/"*"
                        | ( type "/" "*" )
                        | ( type "/" subtype )
                        ) *( ";" parameter )
       accept-params  = ";" "q" "=" qvalue *( accept-extension )
       accept-extension = ";" token [ "=" ( token | quoted-string ) ]
     */
     if (strcmp(httpRequest->accept(),"") != 0)
     {
       str << "Accept: "<< httpRequest->accept() << "\r\n";
     }
     else
     {
       str << "Accept: text/plain; q=0.5, text/html,text/x-dvi; q=0.8, text/x-c\r\n";
     }

    /** 3.2 Accept-Charset */
    /*
     * The Accept-Charset request-header field can be used to indicate what
       character sets are acceptable for the response. This field allows
       clients capable of understanding more comprehensive or special-
       purpose character sets to signal that capability to a server which is
       capable of representing documents in those character sets.
     * Accept-Charset = "Accept-Charset" ":"
              1#( ( charset | "*" )[ ";" "q" "=" qvalue ] )
     */
    if (strcmp(httpRequest->acceptCharset(),"") != 0)
    {
      str << "Accept-Charset: "<< httpRequest->acceptCharset() << "\r\n";
    }
    else
    {
      //if the Accept-Charset not set, don't send it
    }

    /** 3.3 Accept-Encoding */
    /*
     * The Accept-Encoding request-header field is similar to Accept, but
       restricts the content-codings (section 3.5) that are acceptable in
       the response.
     * Accept-Encoding  = "Accept-Encoding" ":"
                          1#( codings [ ";" "q" "=" qvalue ] )
       codings          = ( content-coding | "*" )
     */
    if (strcmp(httpRequest->acceptEncoding(),"") != 0)
    {
      str << "Accept-Encoding: "<< httpRequest->acceptEncoding() << "\r\n";
    }
    else
    {
      str << "Accept-Encoding: gzip, deflate\r\n";
    }

    /** 3.4 Accept-Language */
    /*
     * The Accept-Language request-header field is similar to Accept, but
       restricts the set of natural languages that are preferred as a
       response to the request. Language tags are defined in section 3.10.
     * Accept-Language = "Accept-Language" ":"
                         1#( language-range [ ";" "q" "=" qvalue ] )
       language-range  = ( ( 1*8ALPHA *( "-" 1*8ALPHA ) ) | "*" )
     */
    if (strcmp(httpRequest->acceptLanguage(),"") != 0)
    {
      str << "Accept-Language: "<< httpRequest->acceptLanguage() << "\r\n";
    }
    else
    {
      str << "Accept-Language: zh-cn,zh;q=0.8,en-us;q-0.5,en;q=0.3\r\n";
    }

    /** 3.5 Authorization */
    /*
     * A user agent that wishes to authenticate itself with a server--
       usually, but not necessarily, after receiving a 401 response--does
       so by including an Authorization request-header field with the
       request.  The Authorization field value consists of credentials
       containing the authentication information of the user agent for
       the realm of the resource being requested.
     * Authorization  = "Authorization" ":" credentials
     */

    /** 3.6 Expect */
    /*
     * The Expect request-header field is used to indicate that particular
       server behaviors are required by the client.
     * Expect       =  "Expect" ":" 1#expectation
       expectation  =  "100-continue" | expectation-extension
       expectation-extension =  token [ "=" ( token | quoted-string )
                                *expect-params ]
       expect-params =  ";" token [ "=" ( token | quoted-string ) ]
     */

    /** 3.7 From */
    /*
     * The From request-header field, if given, SHOULD contain an Internet
       e-mail address for the human user who controls the requesting user
       agent. The address SHOULD be machine-usable, as defined by "mailbox"
       in RFC 822 [9] as updated by RFC 1123 [8]:
     * From   = "From" ":" mailbox
     */

    /** 3.8 Host */
    /*
     * The Host request-header field specifies the Internet host and port
       number of the resource being requested, as obtained from the original
       URI given by the user or referring resource (generally an HTTP URL,
       as described in section 3.2.2). The Host field value MUST represent
       the naming authority of the origin server or gateway given by the
       original URL. This allows the origin server or gateway to
       differentiate between internally-ambiguous URLs, such as the root "/"
       URL of a server for multiple host names on a single IP address.
     * Host = "Host" ":" host [ ":" port ]
     */
    if (strcmp(httpRequest->host(),"") != 0)
    {
      str << "Host: "<< httpRequest->host() << "\r\n";
      EV_DEBUG << "host not null Set Host: "<< httpRequest->host() << "\r\n";
    }
    else
    {
      str << "Host: "<< httpRequest->targetUrl() << "\r\n";
      EV_DEBUG << "Set Host: "<< httpRequest->targetUrl() << "\r\n";
    }

    /** 3.9 If-Match */
    /*
     * The If-Match request-header field is used with a method to make it
       conditional. A client that has one or more entities previously
       obtained from the resource can verify that one of those entities is
       current by including a list of their associated entity tags in the
       If-Match header field. Entity tags are defined in section 3.11. The
       purpose of this feature is to allow efficient updates of cached
       information with a minimum amount of transaction overhead. It is also
       used, on updating requests, to prevent inadvertent modification of
       the wrong version of a resource. As a special case, the value "*"
       matches any current entity of the resource.
     * If-Match = "If-Match" ":" ( "*" | 1#entity-tag )
     */

    /** 3.10 If-Modified-Since */
    /*
     * The If-Modified-Since request-header field is used with a method to
       make it conditional: if the requested variant has not been modified
       since the time specified in this field, an entity will not be
       returned from the server; instead, a 304 (not modified) response will
       be returned without any message-body.
     * If-Modified-Since = "If-Modified-Since" ":" HTTP-date
     */
    if (strcmp(httpRequest->ifModifiedSince(),"") != 0)
    {
      str << "If-Modified-Since: "<< httpRequest->ifModifiedSince() << "\r\n";
    }
    else
    {
      //if the If-Modified-Since not set, don't send it
    }

    /** 3.11 If-None-Match */
    /*
     * The If-None-Match request-header field is used with a method to make
       it conditional. A client that has one or more entities previously
       obtained from the resource can verify that none of those entities is
       current by including a list of their associated entity tags in the
       If-None-Match header field. The purpose of this feature is to allow
       efficient updates of cached information with a minimum amount of
       transaction overhead. It is also used to prevent a method (e.g. PUT)
       from inadvertently modifying an existing resource when the client
       believes that the resource does not exist.
     * As a special case, the value "*" matches any current entity of the
       resource.
     * If-None-Match = "If-None-Match" ":" ( "*" | 1#entity-tag )
     */
    if (strcmp(httpRequest->ifNoneMatch(),"") != 0)
    {
      str << "If-None-Match: "<< httpRequest->ifNoneMatch() << "\r\n";
    }
    else
    {
      //if the If-None-Match not set, don't send it
    }

    /** 3.12 If-Range */
    /*
     * If a client has a partial copy of an entity in its cache, and wishes
       to have an up-to-date copy of the entire entity in its cache, it
       could use the Range request-header with a conditional GET (using
       either or both of If-Unmodified-Since and If-Match.) However, if the
       condition fails because the entity has been modified, the client
       would then have to make a second request to obtain the entire current
       entity-body.
     * The If-Range header allows a client to "short-circuit" the second
       request. Informally, its meaning is `if the entity is unchanged, send
       me the part(s) that I am missing; otherwise, send me the entire new
       entity'.
     *  If-Range = "If-Range" ":" ( entity-tag | HTTP-date )
     */

    /** 3.13 If-Unmodified-Since */
    /*
     * The If-Unmodified-Since request-header field is used with a method to
       make it conditional. If the requested resource has not been modified
       since the time specified in this field, the server SHOULD perform the
       requested operation as if the If-Unmodified-Since header were not
       present.
     * If the requested variant has been modified since the specified time,
       the server MUST NOT perform the requested operation, and MUST return
       a 412 (Precondition Failed).
     * If-Unmodified-Since = "If-Unmodified-Since" ":" HTTP-date
     */

    /** 3.14 Max-Forwards */
    /*
     * The Max-Forwards request-header field provides a mechanism with the
       TRACE (section 9.8) and OPTIONS (section 9.2) methods to limit the
       number of proxies or gateways that can forward the request to the
       next inbound server. This can be useful when the client is attempting
       to trace a request chain which appears to be failing or looping in
       mid-chain.
     * Max-Forwards   = "Max-Forwards" ":" 1*DIGIT
     */

    /** 3.15 Proxy-Authorization */
    /*
     * The Proxy-Authenticate response-header field MUST be included as part
       of a 407 (Proxy Authentication Required) response. The field value
       consists of a challenge that indicates the authentication scheme and
       parameters applicable to the proxy for this Request-URI.
     * Proxy-Authenticate  = "Proxy-Authenticate" ":" 1#challenge
     */

    /** 3.16 Range */

    /** 3.17 Referer */
    /*
     * The Referer[sic] request-header field allows the client to specify,
       for the server's benefit, the address (URI) of the resource from
       which the Request-URI was obtained (the "referrer", although the
       header field is misspelled.) The Referer request-header allows a
       server to generate lists of back-links to resources for interest,
       logging, optimized caching, etc. It also allows obsolete or mistyped
       links to be traced for maintenance. The Referer field MUST NOT be
       sent if the Request-URI was obtained from a source that does not have
       its own URI, such as input from the user keyboard.
     * Referer        = "Referer" ":" ( absoluteURI | relativeURI )
     */
    if (strcmp(httpRequest->referer(),"") != 0)
    {
      str << "Referer: "<< httpRequest->referer() << "\r\n";
    }
    else
    {
      str << "Referer: "<< httpRequest->targetUrl() << "\r\n";
    }

    /** 3.18 TE */
    /*
     * The TE request-header field indicates what extension transfer-codings
       it is willing to accept in the response and whether or not it is
       willing to accept trailer fields in a chunked transfer-coding. Its
       value may consist of the keyword "trailers" and/or a comma-separated
       list of extension transfer-coding names with optional accept
       parameters (as described in section 3.6).
     * TE        = "TE" ":" #( t-codings )
       t-codings = "trailers" | ( transfer-extension [ accept-params ] )
     */

    /** 3.19 User-Agent */
    /*
     * The User-Agent request-header field contains information about the
       user agent originating the request. This is for statistical purposes,
       the tracing of protocol violations, and automated recognition of user
       agents for the sake of tailoring responses to avoid particular user
       agent limitations. User agents SHOULD include this field with
       requests. The field can contain multiple product tokens (section 3.8)
       and comments identifying the agent and any subproducts which form a
       significant part of the user agent. By convention, the product tokens
       are listed in order of their significance for identifying the
       application.
     * User-Agent     = "User-Agent" ":" 1*( product | comment )
     */
    if (strcmp(httpRequest->userAgent(),"") != 0)
    {
      str << "User-Agent: "<< httpRequest->userAgent() << "\r\n";
    }
    else
    {
      str << "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:16.0) Gecko/20100101 Firefox/16.0\r\n";
    }

    /**
     * 4. Generate entity-header:
     * entity-header = Allow
                       | Content-Encoding
                       | Content-Language
                       | Content-Length
                       | Content-Location
                       | Content-MD5
                       | Content-Range
                       | Content-Type
                       | Expires
                       | Last-Modified
                       | extension-header
     * extension-header = message-header
     */

    /** 4.1 Allow */
    /*
     * The Allow entity-header field lists the set of methods supported
       by the resource identified by the Request-URI.
     * Allow   = "Allow" ":" #Method
     */

    /** 4.2 Content-Encoding */
    /*
     * The Content-Encoding entity-header field is used as a modifier to the
       media-type. When present, its value indicates what additional content
       codings have been applied to the entity-body, and thus what decoding
       mechanisms must be applied in order to obtain the media-type
       referenced by the Content-Type header field.
     * If the content-coding of an entity is not "identity", then the
       Request MUST include a Content-Encoding entity-header (section
       14.11) that lists the non-identity content-coding(s) used.
     * Content-Encoding  = "Content-Encoding" ":" 1#content-coding
     */

    /** 4.3 Content-Language */
    /*
     * The Content-Language entity-header field describes the natural
       language(s) of the intended audience for the enclosed entity.
     * Content-Language  = "Content-Language" ":" 1#language-tag
     */

    /** 4.4 Content-Length */
    /*
     * The Content-Length entity-header field indicates the size of the
       entity-body, in decimal number of OCTETs, sent to the recipient or,
       in the case of the HEAD method, the size of the entity-body that
       would have been sent had the request been a GET.
     * Content-Length    = "Content-Length" ":" 1*DIGIT
     */

    /** 4.5 Content-Location */
    /*
     * The Content-Location entity-header field MAY be used to supply the
       resource location for the entity enclosed in the message when that
       entity is accessible from a location separate from the requested
       resource's URI.
     * Content-Location = "Content-Location" ":" ( absoluteURI | relativeURI )
     */

    /** 4.6 Content-MD5 */
    /*
     * The Content-MD5 entity-header field, as defined in RFC 1864 [23], is
       an MD5 digest of the entity-body for the purpose of providing an
       end-to-end message integrity check (MIC) of the entity-body.
     * Content-MD5   = "Content-MD5" ":" md5-digest
       md5-digest   = <base64 of 128 bit MD5 digest as per RFC 1864>
     */

    /** 4.7 Content-Range */
    /*
     * The Content-Range entity-header is sent with a partial entity-body to
       specify where in the full entity-body the partial body should be
       applied.
     * Content-Range = "Content-Range" ":" content-range-spec
       content-range-spec      = byte-content-range-spec
       byte-content-range-spec = bytes-unit SP
                                 byte-range-resp-spec "/"
                                 ( instance-length | "*" )
       byte-range-resp-spec = (first-byte-pos "-" last-byte-pos)
                                      | "*"
       instance-length           = 1*DIGIT
     */

    /** 4.8 Content-Type */
    /*
     * The Content-Type entity-header field indicates the media type of the
       entity-body sent to the recipient or, in the case of the HEAD method,
       the media type that would have been sent had the request been a GET.
     * Content-Type   = "Content-Type" ":" media-type
     * media-type     = type "/" subtype *( ";" parameter )
       type           = token
       subtype        = token
     * Any HTTP/1.1 message containing an entity-body SHOULD include a
       Content-Type header field defining the media type of that body.
     */

    /** 4.9 Expires */
    /*
     * The Expires entity-header field gives the date/time after which the
       Request is considered stale.
     * The format is an absolute date and time as defined by HTTP-date in
       section 3.3.1; it MUST be in RFC 1123 date format:
     * Expires = "Expires" ":" HTTP-date
     */

    /** 4.10 Last-Modified */
    /*
     * The Last-Modified entity-header field indicates the date and time at
       which the origin server believes the variant was last modified.
     * Last-Modified  = "Last-Modified" ":" HTTP-date
     */

    /*************************************Finish generating HTTP Request Header*************************************/

    str << "\r\n";

    return str.str();
}

/** Deflate a HTTP Request message header using the Name-Value zlib dictionary */
std::string NSCHttpBrowser::formatSpdyZlibHttpRequestMessageHeader(NSCSockData *sockdata, const RealHttpRequestMessage *httpRequest)
{
    std::string reqHeader = formatHttpRequestMessageHeader(httpRequest);

    uint8_t *buf = NULL, *nvbuf = NULL;
    size_t buflen = 0, nvbuflen = 0;

    nvbuf = (uint8_t *)reqHeader.c_str();
    nvbuflen = reqHeader.length();

    spdylay_zlib deflater = sockdata->zlib.deflater;

    buflen = spdylay_zlib_deflate_hd_bound(&deflater, nvbuflen);
    buf = new uint8_t[buflen];

    EV_DEBUG << "header length bound to deflate is: " << buflen << endl;

    ssize_t framelen;
    framelen = spdylay_zlib_deflate_hd(&deflater, buf, buflen, nvbuf, nvbuflen);

    EV_DEBUG << "header length before deflate is: " << nvbuflen << endl;
    EV_DEBUG << "header before deflate is: " << nvbuf << endl;
    EV_DEBUG << "header length after deflate is: " << framelen << endl;
    EV_DEBUG << "header after deflate is: " << buf << endl;

    if (framelen == SPDYLAY_ERR_ZLIB)
    {
        throw cRuntimeError("spdylay_zlib_deflate_hd get Zlib error!");
    }
    else
    {
        // doing statistics
        double bodyLength = (double)strlen(httpRequest->payload());
        headerBytesBeforeDeflate += nvbuflen;
        headerBytesAfterDeflate += framelen;
        headerDeflateRatioVec.recordWithTimestamp(simTime(), double(framelen)/double(nvbuflen));
        totalDeflateRatioVec.recordWithTimestamp(simTime(), (double)(framelen+bodyLength)/(double)(nvbuflen+bodyLength));

        std::ostringstream zlibReqHeader;

        //24 bit header length
        zlibReqHeader << std::setbase(16) << std::setw(3) << framelen ;

        char *charBuf = new char[framelen];

        EV_DEBUG << "header after deflate is as char: ";
        for (ssize_t i = 0; i < framelen; i++)
        {
            charBuf[i] = buf[i] - 128;
            EV << charBuf[i];
        }
        EV << endl;

//        memcpy(charBuf, buf, framelen);

//        EV_DEBUG << "header after deflate is as char: " << charBuf << endl;

        //zlib deflate header
//        zlibReqHeader << std::setw(framelen) << buf;
//        zlibReqHeader << std::setw(framelen) << charBuf;
//        zlibReqHeader << charBuf;

        std::string zlibReqHeaderStr;
        zlibReqHeaderStr.assign(charBuf, framelen);

        delete[] buf;
        delete[] charBuf;

        zlibReqHeader << zlibReqHeaderStr;

//        EV_DEBUG << "zlibReqHeader is: " << zlibReqHeader.str() << endl;

        EV_DEBUG << "zlibReqHeader length is: " << zlibReqHeader.str().length() << endl;

        return zlibReqHeader.str();
    }
}

/** Format a Request message header to zlib-deflated SPDY header block */
std::string NSCHttpBrowser::formatSpdyZlibHeaderBlockRequestMessageHeader(const RealHttpRequestMessage *httpRequest)
{
    std::ostringstream str;

    return str.str();
}

/** Format a Request message to HTTP Request Message Header */
std::string NSCHttpBrowser::formatHttpNFRequestMessageHeader(const RealHttpRequestMessage *httpRequest)
{
    std::ostringstream str;

    return str.str();
}

RealHttpRequestMessage *NSCHttpBrowser::changeRequestToReal(HttpRequestMessage *httpRequest)
{
    RealHttpRequestMessage *realHttpRequest;
    realHttpRequest = new RealHttpRequestMessage();

    realHttpRequest->setAccept("");
    realHttpRequest->setAcceptCharset("");
    realHttpRequest->setAcceptEncoding("");
    realHttpRequest->setAcceptLanguage("");
    realHttpRequest->setHost("");
    realHttpRequest->setIfModifiedSince("");
    realHttpRequest->setIfNoneMatch("");
    realHttpRequest->setReferer("");
    realHttpRequest->setUserAgent("");
    realHttpRequest->setTargetUrl(httpRequest->targetUrl());
    realHttpRequest->setProtocol(httpRequest->protocol());
    realHttpRequest->setHeading(httpRequest->heading());
    realHttpRequest->setSerial(httpRequest->serial());
    realHttpRequest->setByteLength(httpRequest->getByteLength());
    realHttpRequest->setKeepAlive(httpRequest->keepAlive());
    realHttpRequest->setBadRequest(httpRequest->badRequest());
    realHttpRequest->setKind(httpRequest->getKind());
    realHttpRequest->setPayload(httpRequest->payload());

    delete httpRequest;
    httpRequest = NULL;

    return realHttpRequest;
}

