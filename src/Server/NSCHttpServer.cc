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

#include "NSCHttpServer.h"
#include "ByteArrayMessage.h"

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

#include <algorithm>

#include <iomanip>


Define_Module(NSCHttpServer);

NSCHttpServer::NSCHttpServer()
{
    replyInfoPerSocket.clear();

}
NSCHttpServer::~NSCHttpServer()
{

}

void NSCHttpServer::incImgResourcesServed()
{
    imgResourcesServed++;
}

void NSCHttpServer::incTextResourcesServed()
{
    textResourcesServed++;
}

void NSCHttpServer::incMediaResourcesServed()
{
    mediaResourcesServed++;
}

void NSCHttpServer::incOtherResourcesServed()
{
    otherResourcesServed++;
}

void NSCHttpServer::decBadRequests()
{
    badRequests--;
}

void NSCHttpServer::recordCompressStatistic(size_t framelen, size_t nvbuflen, size_t bodyLength)
{
    headerBytesBeforeDeflate += nvbuflen;
    headerBytesAfterDeflate += framelen;
    headerDeflateRatioVec.recordWithTimestamp(simTime(), double(framelen) / double(nvbuflen));
    totalDeflateRatioVec.recordWithTimestamp(simTime(),
                                             (double) (framelen + bodyLength) / (double) (nvbuflen + bodyLength));
}

void NSCHttpServer::initialize()
{
    HttpServerBase::initialize();

    EV_DEBUG << "Initializing nsc http server " << endl;

    int port = par("port");

    TCPSocket listensocket;
    listensocket.setOutputGate(gate("tcpOut"));
    listensocket.setDataTransferMode(TCP_TRANSFER_BYTESTREAM);
    listensocket.bind(port);
    listensocket.setCallbackObject(this);
    listensocket.listen();

    headerEncodeType = par("headerEncodeType");
    headerCompressType = par("headerCompressType");
    messageInfoSrc = par("messageInfoSrc");

    getWaitTimeFromHar = par("getWaitTimeFromHar");

    setMessageFactory();

    // Initialize statistics
    numBroken = 0;
    socketsOpened = 0;
    totalBytesSent = 0;
    headerBytesBeforeDeflate = headerBytesAfterDeflate = 0;
    mediaResourcesServed = otherResourcesServed = 0;
    responseSent = 0;

    // Initialize watches
    WATCH(numBroken);
    WATCH(socketsOpened);
    WATCH(totalBytesSent);
    WATCH(headerBytesBeforeDeflate);
    WATCH(headerBytesAfterDeflate);
    WATCH(mediaResourcesServed);
    WATCH(otherResourcesServed);
    WATCH(responseSent);

    // Initialize cOutputVectors
    recvReqTimeVec.setName("Receive Request SimTime");
    sendResTimeVec.setName("Send Response SimTime");
    headerDeflateRatioVec.setName("Header Deflate Ratio");
    totalDeflateRatioVec.setName("Total Deflate Ratio");
}

void NSCHttpServer::finish()
{
    HttpServer::finish();

    // Report sockets related statistics.
    EV_SUMMARY << "Total Bytes Sent: " << totalBytesSent << endl;
    EV_SUMMARY << "Bytes Before Deflate: " << headerBytesBeforeDeflate << endl;
    EV_SUMMARY << "Bytes After Deflate: " << headerBytesAfterDeflate << endl;
    EV_SUMMARY << "Media resources served " << mediaResourcesServed << "\n";
    EV_SUMMARY << "Other resources served " << otherResourcesServed << "\n";
    EV_SUMMARY << "Response sent: " << responseSent << endl;

    // Record the sockets related statistics
    recordScalar("bytes.totalBytesSent", totalBytesSent);
    recordScalar("bytes.beforeDeflate", headerBytesBeforeDeflate);
    recordScalar("bytes.AfterDeflate", headerBytesAfterDeflate);
    recordScalar("media.served", mediaResourcesServed);
    recordScalar("other.served", otherResourcesServed);
    recordScalar("response.sent", responseSent);
}

std::string NSCHttpServer::generateBody()
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

void NSCHttpServer::socketEstablished(int connId, void *yourPtr)
{
    HttpServer::socketEstablished(connId, yourPtr);

    TCPSocket *socket = (TCPSocket*)yourPtr;
//    if (true == initSockZlibInfo(socket))
//    {
//        EV_DEBUG << "Socket established and initSockZlibInfo success on connection " << connId << endl;
//    }
//    else
//    {
//        EV_ERROR << "Socket established and initSockZlibInfo failed on connection " << connId << endl;
//    }

    Socket_ID_Type sockID = pMessageController->getNewSockID();

    idPerSocket.insert(TCPSocket_ID_Map::value_type(socket, sockID));
}

void NSCHttpServer::socketClosed(int connId, void *yourPtr)
{
    EV_INFO << "connection closed. Connection id " << connId << endl;

    if (yourPtr==NULL)
    {
        EV_ERROR << "Socket establish failure. Null pointer" << endl;
        return;
    }
    // Cleanup
    TCPSocket *socket = (TCPSocket*)yourPtr;
    removeIDInfo(socket);
    sockCollection.removeSocket(socket);
    delete socket;
}

void NSCHttpServer::socketFailure(int connId, void *yourPtr, int code)
{
    EV_WARNING << "connection broken. Connection id " << connId << endl;
    numBroken++;

    EV_INFO << "connection closed. Connection id " << connId << endl;

    if (yourPtr==NULL)
    {
        EV_ERROR << "Socket establish failure. Null pointer" << endl;
        return;
    }
    TCPSocket *socket = (TCPSocket*)yourPtr;

    if (code==TCP_I_CONNECTION_RESET)
        EV_WARNING << "Connection reset!\n";
    else if (code==TCP_I_CONNECTION_REFUSED)
        EV_WARNING << "Connection refused!\n";

    // Cleanup
    removeIDInfo(socket);
    sockCollection.removeSocket(socket);
    delete socket;
}

void NSCHttpServer::socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent)
{
    if (yourPtr==NULL)
    {
        EV_ERROR << "Socket establish failure. Null pointer" << endl;
        return;
    }
    TCPSocket *socket = (TCPSocket*)yourPtr;

    cPacket *prasedMsg = pMessageController->parseRequestMessage(msg, idPerSocket[socket]);

    // Should be a HttpRequestMessage
    EV_DEBUG << "Socket data arrived on connection " << connId << ". Message=" << prasedMsg->getName() << ", kind="
            << prasedMsg->getKind() << endl;

    // call the message handler to process the message.
    HttpReplyMessage *reply = handleReceivedMessage(prasedMsg);
    recvReqTimeVec.record(simTime());

    if (reply != NULL)
    {
        /*
         * add delay time to send reply, in order to make HOL possible
         */
//        socket->send(reply); // Send to socket if the reply is non-zero.
        double replyDelay;
        //--need to get wait time from har file
        if ((messageInfoSrc == HAR_FILE) && getWaitTimeFromHar)
        {
            /*
             * 1. get request URI
             */
            RealHttpReplyMessage *httpResponse = check_and_cast<RealHttpReplyMessage*>(reply);

            //can not get request URI, generate it ramdomly
            if (strcmp(httpResponse->requestURI(), "") == 0)
            {
                replyDelay = (double) (rdReplyDelay->draw());
            }
            else
            {
                // use the request-uri to get the real har response's timings
                HeaderFrame timings = pMessageController->getTimingFromHar(httpResponse->requestURI());

                if (timings.size() == 0)
                {
                    //can not find the timings, generate it ramdomly
                    replyDelay = (double) (rdReplyDelay->draw());
                }
                else
                {
                    unsigned int i;
                    for (i = 0; i < timings.size(); ++i)
                    {
                        //---Note: here should cmp ":status-text" before ":status", or ":status" may get ":status-text" value
                        if (timings[i].key.find("wait") != string::npos)
                        {
                            replyDelay = (double) (atof(timings[i].val.c_str()) / 1000);
                            EV_DEBUG << "Find wait time for requestURI " << httpResponse->requestURI()
                                    << ", replyDelay is " << replyDelay << endl;
                            break;
                        }
                    }
                    if (i == timings.size())
                    {
                        //can not find wait in the timings, generate it ramdomly
                        replyDelay = (double) (rdReplyDelay->draw());
                    }
                }
            }

        }
        else
        {
            replyDelay = (double) (rdReplyDelay->draw());
        }
        EV_DEBUG << "Need to send message on socket " << socket << ". Message=" << reply->getName() << ", kind="
                << reply->getKind() << ", sendDelay = " << replyDelay << endl;

        if (replyDelay == 0)
        {
            // if there is no other msg wait to send, send it directly
            if (replyInfoPerSocket.find(socket) == replyInfoPerSocket.end() || replyInfoPerSocket[socket].empty())
            {
                // change message send method to adapt to the use of NSC
                //socket->send(reply); // Send to socket if the reply is non-zero.
                sendMessage(socket, reply);
            }
            else
            {
                /*
                 * check if there's earlier reply not send
                 */
                bool readyToSend = true;
                for (HttpReplyInfoQueue::iterator iter = replyInfoPerSocket[socket].begin();
                        iter != replyInfoPerSocket[socket].end(); iter++)
                {
                    readyToSend &= iter->readyToSend;
                }
                if (readyToSend)
                {
                    // change message send method to adapt to the use of NSC
                    //socket->send(reply); // Send to socket if the reply is non-zero.
                    sendMessage(socket, reply);
                }
                // there is earlier reply not send, record this reply
                else
                {
                    //HttpReplyMessage *realhttpResponse = dynamic_cast<HttpReplyMessage*>(reply);
                    ReplyInfo replyInfo =
                    { reply, true };
                    replyInfoPerSocket[socket].push_back(replyInfo);
                }
            }
        }
        else
        {
            //HttpReplyMessage *realhttpResponse = dynamic_cast<HttpReplyMessage*>(reply);
            ReplyInfo replyInfo =
            { reply, false };
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
            scheduleAt(simTime() + replyDelay, reply);
        }
    }
    delete prasedMsg; // Delete the received message here. Must not be deleted in the handler!
}

void NSCHttpServer::handleSelfMessages(cMessage *msg)
{
    switch (msg->getKind())
    {
        case HTTPT_DELAYED_RESPONSE_MESSAGE:
            handleSelfDelayedReplyMessage(msg);
            break;
    }
}

void NSCHttpServer::handleSelfDelayedReplyMessage(cMessage *msg)
{
    EV_DEBUG << "Need to send delayed message " << msg->getName() << " @ T=" << simTime() << endl;
    HttpReplyMessage *reply = dynamic_cast<HttpReplyMessage*>(msg);
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
                    // change message send method to adapt to the use of NSC
                    //msg = dynamic_cast<cMessage*>(replyInfoPerSocket[socket].front().reply);
                    //socket->send(msg); // Send to socket if the reply is non-zero.
                    sendMessage(socket, replyInfoPerSocket[socket].front().reply);

                    EV_INFO << "Send earlier message # " << i << ". Message=" << msg->getName() << ", kind=" << msg->getKind()<< endl;
                    replyInfoPerSocket[socket].pop_front();
                }

                // change message send method to adapt to the use of NSC
                //msg = dynamic_cast<cMessage*>(reply);
                //socket->send(msg); // Send to socket if the reply is non-zero.

                EV_INFO << "Send message =" << reply->getName() << ", kind=" << reply->getKind()<< endl;
                sendMessage(socket, reply);

                replyInfoPerSocket[socket].pop_front();

                while (!replyInfoPerSocket[socket].empty())
                {
                    if (replyInfoPerSocket[socket].front().readyToSend)
                    {
                        // change message send method to adapt to the use of NSC
                        //msg = dynamic_cast<cMessage*>(replyInfoPerSocket[socket].front().reply);
                        //socket->send(msg); // Send to socket if the reply is non-zero.
                        sendMessage(socket, replyInfoPerSocket[socket].front().reply);

//                        EV_INFO << "Send message =" << replyInfoPerSocket[socket].front().reply->getName() << ", kind=" << replyInfoPerSocket[socket].front().reply->getKind()<< endl;
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

void NSCHttpServer::handleMessage(cMessage *msg)
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

            // change the dataTransferMode from TCP_TRANSFER_OBJECT to TCP_TRANSFER_BYTESTREAM to support NSC TCP
            socket->setDataTransferMode(TCP_TRANSFER_BYTESTREAM);
            socket->setCallbackObject(this, socket);
            sockCollection.addSocket(socket);
        }
        EV_DEBUG << "Process the message " << msg->getName() << endl;
        socket->processMessage(msg);
    }
    updateDisplay();
}

/*
 * Handle a received data message, e.g. check if the content requested exists.
 * change the HttpServerBase::handleReceivedMessage return type from cMessage to HttpReplyMessage to record the return of handleReceivedMessage
 */
HttpReplyMessage* NSCHttpServer::handleReceivedMessage(cMessage *msg)
{
    HttpRequestMessage *request = check_and_cast<HttpRequestMessage *>(msg);

    if (request == NULL)
        error("Message (%s)%s is not a valid request", msg->getClassName(), msg->getName());

    EV_DEBUG << "Handling received message " << msg->getName() << ". Target URL: " << request->targetUrl() << endl;

    logRequest(request);

//    if (extractServerName(request->targetUrl()) != hostName)
//    {
//        // This should never happen but lets check
////        error("Received message intended for '%s'", request->targetUrl()); // TODO: DEBUG HERE
//        EV_ERROR << "Received message intended for " << request->targetUrl() << endl;
//        return NULL;
//    }

    HttpReplyMessage *httpResponse;

    // Parse the request string on spaces
    cStringTokenizer tokenizer = cStringTokenizer(request->heading(), " ");
    std::vector < std::string > res = tokenizer.asVector();
    if (res.size() != 3)
    {
        EV_ERROR << "Invalid request string: " << request->heading() << endl;
        httpResponse = generateErrorReply(request, 400);
        logResponse(httpResponse);

        /*
         * Note: here change HttpReplyMessage * to RealHttpReplyMessage * to record request URI for the later check
         * for this case, request URI is set to "" to fit the procedure
         */
        RealHttpReplyMessage *realhttpResponse = pMessageController->changeReplyToReal(httpResponse);
        realhttpResponse->setRequestURI("");

        ///Note: here return the HttpReplyMessage * pointer, need to do static_cast
        return static_cast<HttpReplyMessage*>(realhttpResponse);
    }

    if (request->badRequest())
    {
        // Bad requests get a 404 reply.
        EV_ERROR << "Bad request - bad flag set. Message: " << request->getName() << endl;
        httpResponse = generateErrorReply(request, 404);
    }
    else if (res[0] == "GET")
    {
        httpResponse = handleGetRequest(request, res[1]); // Pass in the resource string part
    }
    else
    {
        EV_ERROR << "Unsupported request type " << res[0] << " for " << request->heading() << endl;
        httpResponse = generateErrorReply(request, 400);
    }

    if (httpResponse != NULL)
    {
        logResponse(httpResponse);

        /*
         * Note: here change HttpReplyMessage * to RealHttpReplyMessage * to record request URI for the later check
         */
        RealHttpReplyMessage *realhttpResponse = pMessageController->changeReplyToReal(httpResponse);
        realhttpResponse->setRequestURI(res[1].c_str());

        ///Note: here return the HttpReplyMessage * pointer, need to do static_cast
        return static_cast<HttpReplyMessage*>(realhttpResponse);
    }

    return httpResponse;
}

/** send HTTP reply message though TCPSocket depends on the TCP DataTransferMode*/
void NSCHttpServer::sendMessage(TCPSocket *socket, HttpReplyMessage *reply)
{
    long sendBytes = 0;

    cMessage *sendMsg = NULL;

    switch (socket->getDataTransferMode())
    {
        case TCP_TRANSFER_BYTECOUNT:
        case TCP_TRANSFER_OBJECT:
        {
            sendMsg = dynamic_cast<cMessage*>(reply);
            socket->send(sendMsg);
            break;
        }

        case TCP_TRANSFER_BYTESTREAM:
        {
            ByteArrayMessage *byMsg = new ByteArrayMessage(reply->getName());

//            std::string resByteArray = formatByteResponseMessage(socket, reply);
            std::string resByteArray = pMessageController->generateResponseMessage(reply, idPerSocket[socket]);

            sendBytes = resByteArray.length();

            char *ptr = new char[sendBytes];
            ptr[0] = '\0';
            memcpy(ptr, resByteArray.c_str(), sendBytes);
            byMsg->getByteArray().assignBuffer(ptr, sendBytes);
            byMsg->setByteLength(sendBytes);

//            delete[] ptr;

            // doing statistics
            sendResTimeVec.record(simTime());
            totalBytesSent += sendBytes;
            responseSent++;

            EV_DEBUG << "Send ByteArray No. "<< responseSent << ". resByteArray are" << resByteArray  << endl;
//            EV_DEBUG << "Send ByteArray. byMsg->getByteArray() is" << byMsg->getByteArray() << endl;
            EV_DEBUG << "Send ByteArray. Bytelength is" << sendBytes << ". " << endl;

            sendMsg = dynamic_cast<cMessage*>(byMsg);
            socket->send(sendMsg);


            break;
        }

        default:
            throw cRuntimeError("Invalid TCP data transfer mode: %d", socket->getDataTransferMode());
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
void NSCHttpServer::setMessageFactory()
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
            throw std::runtime_error("[In NSCHttpBrowser] Browser get unrecognizable header encode type, can not work!");
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
            throw std::runtime_error("[In NSCHttpBrowser] Browser get unrecognizable header encode type, can not work!");
        }
    }

    pMessageController = new CMessageController(pEncoder, pCompressor, pSrc, this);
}

/** when socket is closed or failed, release the information related to the socket */
void NSCHttpServer::removeIDInfo(TCPSocket *sock)
{
    TCPSocket_ID_Map::iterator i = idPerSocket.find(sock);
    if (i!=idPerSocket.end())
    {
        pMessageController->releaseSockID(i->second);
        idPerSocket.erase(i);
    }
}
