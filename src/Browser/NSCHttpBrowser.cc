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

#include "CHeaderEncoderBase.h"
#include "CHttpAsciiEncoder.h"
#include "CSpdyHeaderBlockEncoder.h"

#include "CHeaderCompressorBase.h"
#include "CNoneCompressor.h"
#include "CGzipCompressor.h"
#include "CSpdy3Compressor.h"
#include "CHuffmanCompressor.h"
#include "CDeltaCompressor.h"
#include "CHeaderdiffCompressor.h"

#include "MsgInfoSrcBase.h"
#include "HarParser.h"
#include "SelfGen.h"

#include "ByteArrayMessage.h"

#include <stdexcept>
#include <iomanip>
#include <memory.h>
#include <algorithm>

Define_Module(NSCHttpBrowser);

NSCHttpBrowser::NSCHttpBrowser() :
        maxConnections(0), maxConnectionsPerHost(0), maxPipelinedReqs(0), pipeliningMode(0), SvrSupportDetect(0), pPipeReq(
                NULL), pSvrSupportDetect(NULL)
{

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

void NSCHttpBrowser::incRequestHarGenerated()
{
    requestHarGenerated++;
}

void NSCHttpBrowser::recordCompressStatistic(size_t framelen, size_t nvbuflen, size_t bodyLength)
{
    headerBytesBeforeDeflate += nvbuflen;
    headerBytesAfterDeflate += framelen;
    headerDeflateRatioVec.recordWithTimestamp(simTime(), double(framelen) / double(nvbuflen));
    totalDeflateRatioVec.recordWithTimestamp(simTime(),
                                             (double) (framelen + bodyLength) / (double) (nvbuflen + bodyLength));
}

void NSCHttpBrowser::initialize(int stage)
{
    HttpBrowser::initialize(stage);

    if (stage == 0)
    {
        maxConnections = par("maxConnections");
        maxConnectionsPerHost = par("maxConnectionsPerHost");
        maxPipelinedReqs = par("maxPipelinedReqs");
        pipeliningMode = par("pipeliningMode");
        SvrSupportDetect = par("SvrSupportDetect");
        headerEncodeType = par("headerEncodeType");
        headerCompressType = par("headerCompressType");
        messageInfoSrc = par("messageInfoSrc");

        sockCollectionMap.clear();    ///< List of active sockets for each server

        selectPipeStrategy();
        setMessageFactory();

        // Initialize statistics
        responseMessageReceived = responseParsed = 0;
        totalBytesSent = 0;
        headerBytesBeforeDeflate = headerBytesAfterDeflate = 0;

        requestSent = requestHarGenerated = 0;

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
        WATCH(headerBytesBeforeDeflate);
        WATCH(headerBytesAfterDeflate);

        // Initialize watches
        WATCH(requestSent);
        WATCH(requestHarGenerated);

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
    EV_SUMMARY << "Bytes Before Deflate: " << headerBytesBeforeDeflate << endl;
    EV_SUMMARY << "Bytes After Deflate: " << headerBytesAfterDeflate << endl;
    EV_SUMMARY << "Request sent: " << requestSent << endl;
    EV_SUMMARY << "Request har generated: " << requestHarGenerated << endl;

    // Record the sockets related statistics
    recordScalar("response.messageRec", responseMessageReceived);
    recordScalar("response.parsed", responseParsed);
    recordScalar("bytes.totalBytesSent", totalBytesSent);
    recordScalar("bytes.beforeDeflate", headerBytesBeforeDeflate);
    recordScalar("bytes.AfterDeflate", headerBytesAfterDeflate);
    recordScalar("request.sent", requestSent);
    recordScalar("request.harGenerated", requestHarGenerated);
}

void NSCHttpBrowser::socketEstablished(int connId, void *yourPtr)
{
    EV_DEBUG << "Socket with id " << connId << " established" << endl;

    socketsOpened++;

    if (yourPtr == NULL)
    {
        EV_ERROR << "SocketEstablished failure. Null pointer" << endl;
        return;
    }

    // Get the socket and associated data structure.
    NSCSockData *sockdata = (NSCSockData*) yourPtr;
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
        EV_INFO << "Submitting request when established " << msg->getName() << " to socket " << connId << ". size is "
                << pckt->getByteLength() << " bytes" << endl;

        // change message send method to adapt to the use of NSC
        //socket->send(msg);
        sendMessage(sockdata, check_and_cast<HttpRequestMessage *>(msg));

        sockdata->pending++;
    }
}

void NSCHttpBrowser::socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent)
{
    EV_DEBUG << "Socket data arrived on connection " << connId << ": " << msg->getName() << endl;
    if (yourPtr == NULL)
    {
        EV_ERROR << "socketDataArrivedfailure. Null pointer" << endl;
        return;
    }

    responseMessageReceived++;

    NSCSockData *sockdata = (NSCSockData*) yourPtr;

    cPacket *parsedMsg = pMessageController->parseResponseMessage(msg, sockdata->sockID);

    parseResponseMessage(connId, sockdata, parsedMsg);
}

/** parse a parsed response message */
void NSCHttpBrowser::parseResponseMessage(int connId, NSCSockData *sockdata, cPacket *parsedMsg)
{
    TCPSocket *socket = sockdata->socket;
    Socket_ID_Type sockID = sockdata->sockID;
    /**
     * check if this response message is complete
     *      if it's a complete message, handle it and try to deal with the rest bytes may remained unparsed
     *      else, wait for the coming messages contain the else message body, do nothing
     */
    if (parsedMsg != NULL)
    {
        responseParsed++;
        EV_DEBUG << "response to parse is No. " << responseParsed << endl;

        /**
         * if infos get from har file
         * check the response message based on the recorded request URI
         */
        if (messageInfoSrc == HAR_FILE)
        {
            std::string requestURI = reqListPerSocket[socket].front();
            reqListPerSocket[socket].pop_front();
            HeaderFrame harResponse = pMessageController->getResponseFromHar(requestURI);

            if (harResponse.size() == 0)
            {
                EV_ERROR << "can not get response for " << requestURI << endl;
            }
            else
            {
                unsigned int i;
                for (i = 0; i < harResponse.size(); ++i)
                {
                    //---Note: here should cmp ":status-text" before ":status", or ":status" may get ":status-text" value
                    if (harResponse[i].key.find("Content-Length") != string::npos
                            || harResponse[i].key.find("content-length") != string::npos)
                    {
                        int64 harContentLength = (int64)(atoi(harResponse[i].val.c_str()));

                        int64 recivedContentLength = check_and_cast<HttpReplyMessage*>(parsedMsg)->getByteLength();

                        if (harContentLength == recivedContentLength)
                        {
                            EV_DEBUG << "parsed response No. " << responseParsed << " is the one recorded in har file."
                                    << endl;
                        }
                        else
                        {
                            throw cRuntimeError(
                                    "[In NSCHttpBrowser] parsed response is not the one recorded in har file ! "
                                    "recivedContentLength is %lld, while the harContentLength is %lld",
                                    recivedContentLength, harContentLength);
                        }
                        break;
                    }
                }
                if (i == harResponse.size())
                {
                    EV_ERROR << "can not get response's Content-Length for " << requestURI << endl;
                }
            }
        }

        HttpContentType contentType = CT_UNKNOWN;
        contentType = pSvrSupportDetect->setSvrSupportForSock(sockdata, parsedMsg);

        handleDataMessage(parsedMsg);
        recvResTimeVec.record(simTime());

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
                EV_INFO << "Submitting request first use html connection " << sendMsg->getName() << " to socket "
                        << connId << ". size is " << pckt->getByteLength() << " bytes" << endl;

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
                    EV_INFO << "Submitting request pipelined " << sendMsg->getName() << " to socket " << connId
                            << ". size is " << pckt->getByteLength() << " bytes" << endl;

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
                    EV_INFO << "Submitting request not pipelined " << sendMsg->getName() << " to socket " << connId
                            << ". size is " << pckt->getByteLength() << " bytes" << endl;

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
            EV_DEBUG
                    << "Received last expected reply on this socket　and there is no resource need to request. Issing a close"
                    << endl;
            socket->close();
        }
        else
        {
            EV_DEBUG << "Cannot close a socket. sockdata pending now is " << sockdata->pending << endl;
        }

        // Message deleted in handler - do not delete here!

        /**
         * since there may be some remaining bytes unparsed, should try to deal with them
         */
        dealWithRestBytes(connId, sockdata);
    }
    else
    {
        EV_DEBUG << "Need to wait for the following msgs" << endl;
    }
}

/** try to deal with the rest bytes */
void NSCHttpBrowser::dealWithRestBytes(int connId, NSCSockData *sockdata)
{
    cPacket *parsedMsg = pMessageController->dealWithRestBytes(sockdata->sockID);

    parseResponseMessage(connId, sockdata, parsedMsg);
}

void NSCHttpBrowser::socketPeerClosed(int connId, void *yourPtr)
{
    EV_DEBUG << "Socket " << connId << " closed by peer" << endl;
    if (yourPtr == NULL)
    {
        EV_ERROR << "socketPeerClosed failure. Null pointer" << endl;
        return;
    }

    NSCSockData *sockdata = (NSCSockData*) yourPtr;
    TCPSocket *socket = sockdata->socket;

    // close the connection (if not already closed)
    if (socket->getState() == TCPSocket::PEER_CLOSED)
    {
        EV_INFO << "remote TCP closed, closing here as well. Connection id is " << connId << endl;
        socket->close();
    }
}

void NSCHttpBrowser::socketClosed(int connId, void *yourPtr)
{
    EV_INFO << "Socket " << connId << " closed" << endl;

    if (yourPtr == NULL)
    {
        EV_ERROR << "socketClosed failure. Null pointer" << endl;
        return;
    }

    NSCSockData *sockdata = (NSCSockData*) yourPtr;

//    spdylay_zlib_deflate_free(&(sockdata->zlib.deflater));
//    spdylay_zlib_inflate_free(&(sockdata->zlib.inflater));
    pMessageController->releaseSockID(sockdata->sockID);

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

    if (yourPtr == NULL)
    {
        EV_ERROR << "socketFailure failure. Null pointer" << endl;
        return;
    }

    if (code == TCP_I_CONNECTION_RESET)
        EV_WARNING << "Connection reset!\n";
    else if (code == TCP_I_CONNECTION_REFUSED)
        EV_WARNING << "Connection refused!\n";

    NSCSockData *sockdata = (NSCSockData*) yourPtr;

//    spdylay_zlib_deflate_free(&(sockdata->zlib.deflater));
//    spdylay_zlib_inflate_free(&(sockdata->zlib.inflater));
    pMessageController->releaseSockID(sockdata->sockID);

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

    EV_DEBUG << "Pipe Submitting to socket. Module: " << moduleName << ", port: " << connectPort << ". Total messages: "
            << queue.size() << endl;

    std::string svrName(moduleName);

    EV_DEBUG << "Submitting server name " << svrName.c_str() << ". " << endl;

    /*
     * check if there is closed socket, if exist, delete the information related to it
     */
    for (SocketSet::iterator iter = sockCollectionMap[svrName].begin(); iter != sockCollectionMap[svrName].end();)
    {
        if ((*iter)->getState() != TCPSocket::CONNECTED && (*iter)->getState() != TCPSocket::CONNECTING)
        {
            EV_DEBUG << "Delete undeleted socket from server " << svrName.c_str() << ". Socket state is"
                    << (*iter)->getState() << endl;
            sockCollection.removeSocket(*iter);
            delete *iter;
            sockCollectionMap[svrName].erase(iter++);
        }
        else
        {
            EV_DEBUG << "Don't need to delete socket from server " << svrName.c_str() << ". Socket state is"
                    << (*iter)->getState() << endl;
            iter++;
        }
    }

    /*
     * Create and initialize sockets if there's not enough socket to use
     * Issue connect to the created sockets for the specified module and port.
     */
    if (maxConnectionsPerHost > sockCollectionMap[svrName].size())
    {
        unsigned int lackConnection = std::min((maxConnectionsPerHost - sockCollectionMap[svrName].size()),
                                               queue.size());
        EV_DEBUG << "Need to create " << lackConnection << " more TCP connections。" << endl;
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
            sockdata->sockID = pMessageController->getNewSockID();
//            sockdata->praser = NULL;
//
//            /*
//             * Initialize the zlib things
//             */
//            int rvD = spdylay_zlib_deflate_hd_init(&(sockdata->zlib.deflater), 1, SPDYLAY_PROTO_SPDY3);
//            if (rvD != 0)
//            {
//                EV_ERROR << "spdylay_zlib_deflate_hd_init failed!" << endl;
//            }
//
//            int rvI = spdylay_zlib_inflate_hd_init(&(sockdata->zlib.inflater), SPDYLAY_PROTO_SPDY3);
//            if (rvI != 0)
//            {
//                EV_ERROR_NOMODULE << "spdylay_zlib_inflate_hd_init failed!" << endl;
//            }
//            sockdata->zlib.setZlib = !(rvD | rvI);

            pSvrSupportDetect->initSvrSupportForSock(sockdata);
            socket->setCallbackObject(this, sockdata);

            // Issue a connect to the socket for the specified module and port.
            socket->connect(IPvXAddressResolver().resolve(moduleName), connectPort);
        }
    }
    else
    {
        EV_DEBUG << "Don't need to create more TCP connections。" << endl;
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

/** send HTTP requesst message though TCPSocket depends on the TCP DataTransferMode*/
void NSCHttpBrowser::sendMessage(NSCSockData *sockdata, HttpRequestMessage *req)
{
    long sendBytes = 0;
    cMessage *sendMsg = NULL;

    TCPSocket *socket = sockdata->socket;

    // add msg log when send it
    EV_INFO << "Sending request " << req->getName() << " to socket. Size is " << req->getByteLength() << " bytes"
            << endl;

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

            /**
             * record request-URI in this socket's requestURIList
             * for response check
             */
            std::string requestURI = pMessageController->getRequestUri(req);
            reqListPerSocket[socket].push_back(requestURI);

//            std::string resByteArray = formatByteRequestMessage(sockdata, req);
            std::string resByteArray = pMessageController->generateRequestMessage(req, sockdata->sockID);

            sendBytes = resByteArray.length();

            char *ptr = new char[sendBytes];
            ptr[0] = '\0';
            memcpy(ptr, resByteArray.c_str(), sendBytes);
            byMsg->getByteArray().assignBuffer(ptr, sendBytes);
            byMsg->setByteLength(sendBytes);

//            delete[] ptr;

            // doing statistics
            sendReqTimeVec.record(simTime());
            totalBytesSent += sendBytes;
            requestSent++;

            EV_DEBUG << "Send ByteArray NO. "<< requestSent << ".  MessageName is: " << byMsg->getName() << ". " << endl;
            EV_DEBUG << "Send ByteArray. SendBytes are: " << resByteArray << ". " << endl;
            EV_DEBUG << "Send ByteArray. Bytelength is: " << sendBytes << ". " << endl;

            sendMsg = dynamic_cast<cMessage*>(byMsg);
            socket->send(sendMsg);

            break;
        }

        default:
            throw cRuntimeError("Invalid TCP data transfer mode: %d", socket->getDataTransferMode());
    }
}

/*************************************************************************
 * Function：    selectPipeStrategy
 * Author:      Wang Qian
 * Description:
 *      select pipelining related strategies, pointer set to the configured derived classes
 * Args：
 *      none
 * Return Values:
 *      void
 *************************************************************************/
void NSCHttpBrowser::selectPipeStrategy()
{
    switch (pipeliningMode)
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
            throw std::runtime_error("[In NSCHttpBrowser] Browser get unrecognizable pipeling mode, can not work!");
        }
    }

    switch (SvrSupportDetect)
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
            throw std::runtime_error(
                    "[In NSCHttpBrowser] Browser get unrecognizable server support detect method, can not work!");
        }
    }
}

/*************************************************************************
 * Function：    setMessageFactory
 * Author:      Wang Qian
 * Description:
 *      set message manager depending on the message header encode type and compression type
 * Args：
 *      none
 * Return Values:
 *      void
 *************************************************************************/
void NSCHttpBrowser::setMessageFactory()
{
    CHeaderEncoderBase* pEncoder;
    CHeaderCompressorBase* pCompressor;
    MsgInfoSrcBase* pSrc;

    switch (headerEncodeType)
    {
        case HTTP_ASCII:
        {
            pEncoder = new CHttpAsciiEncoder();
            break;
        }
        case SPDY_HEADER_BLOCK:
        {
            pEncoder = new CSpdyHeaderBlockEncoder();
            break;
        }
        default:
        {
            throw std::runtime_error(
                    "[In NSCHttpBrowser] Browser get unrecognizable header encode type, can not work!");
        }
    }

    switch (headerCompressType)
    {
        case NONE:
        {
            pCompressor = new CNoneCompressor();
            break;
        }
        case GZIP:
        {
            pCompressor = new CGzipCompressor();
            break;
        }
        case SPDY3:
        {
            pCompressor = new CSpdy3Compressor();
            break;
        }
        case HUFFMAN:
        {
            pCompressor = new CHuffmanCompressor();
            break;
        }
        case DELTA:
        {
            pCompressor = new CDeltaCompressor();
            break;
        }
        case HEADERDIFF:
        {
            pCompressor = new CHeaderdiffCompressor();
            break;
        }
        default:
        {
            throw std::runtime_error(
                    "[In NSCHttpBrowser] Browser get unrecognizable server support header compress method, can not work!");
        }
    }

    switch (messageInfoSrc)
    {
        case GENERATE:
        {
            pSrc = dynamic_cast<SelfGen*>(getParentModule()->getParentModule()->getSubmodule("selfGen"));
            if (pSrc == NULL)
                error("selfGen module not found");
            break;
        }
        case HAR_FILE:
        {
            pSrc = dynamic_cast<HarParser*>(getParentModule()->getParentModule()->getSubmodule("harParser"));
            if (pSrc == NULL)
                error("harParser module not found");
            break;
        }
        default:
        {
            throw std::runtime_error(
                    "[In NSCHttpBrowser] Browser get unrecognizable header encode type, can not work!");
        }
    }

    pMessageController = new CMessageController(pEncoder, pCompressor, pSrc, this);
}
