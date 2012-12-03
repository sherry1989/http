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

#include "HarServer.h"
#include "HttpRequestPraser.h"
#include "ByteArrayMessage.h"
#include <algorithm>

#include <iomanip>

#include "spdylay_zlib.h"

Define_Module(HarServer);

void HarServer::initialize(int stage)
{
    // TODO - Generated method body
    if (stage==0)
    {
        pHarParser = HarParser::Instance();
    }
    else if (stage==1)
    {
        NSCHttpServer::initialize();

        getWaitTimeFromHar = par("getWaitTimeFromHar");

        // Initialize statistics
        mediaResourcesServed = otherResourcesServed = 0;

        // Initialize watches
        WATCH(mediaResourcesServed);
        WATCH(otherResourcesServed);
    }
}

void HarServer::finish()
{
    NSCHttpServer::finish();

    EV_SUMMARY << "Media resources served " << mediaResourcesServed << "\n";
    EV_SUMMARY << "Other resources served " << otherResourcesServed << "\n";

    recordScalar("media.served", mediaResourcesServed);
    recordScalar("other.served", otherResourcesServed);
}

/*
 * Handle a received data message, e.g. check if the content requested exists.
 * change the HttpServerBase::handleReceivedMessage return type from cMessage to HttpReplyMessage to record the return of handleReceivedMessage
 */
HttpReplyMessage* HarServer::handleReceivedMessage(cMessage *msg)
{
    HttpRequestMessage *request = check_and_cast<HttpRequestMessage *>(msg);
    if (request==NULL)
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
    std::vector<std::string> res = tokenizer.asVector();
    if (res.size() != 3)
    {
        EV_ERROR << "Invalid request string: " << request->heading() << endl;
        httpResponse = generateErrorReply(request, 400);
        logResponse(httpResponse);

        /*
         * Note: here change HttpReplyMessage * to RealHttpReplyMessage * to record request URI for the later check
         * for this case, request URI is set to "" to fit the procedure
         */
        RealHttpReplyMessage *realhttpResponse = changeReplyToReal(httpResponse);
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

    if (httpResponse!=NULL)
    {
        logResponse(httpResponse);

        /*
         * Note: here change HttpReplyMessage * to RealHttpReplyMessage * to record request URI for the later check
         */
        RealHttpReplyMessage *realhttpResponse = changeReplyToReal(httpResponse);
        realhttpResponse->setRequestURI(res[1].c_str());

        ///Note: here return the HttpReplyMessage * pointer, need to do static_cast
        return static_cast<HttpReplyMessage*>(realhttpResponse);
    }

    return httpResponse;
}

void HarServer::socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent)
{
    if (yourPtr==NULL)
    {
        EV_ERROR << "Socket establish failure. Null pointer" << endl;
        return;
    }
    TCPSocket *socket = (TCPSocket*)yourPtr;

    HttpRequestPraser *praser = new HttpRequestPraser();

    spdylay_zlib inflater = zlibPerSocket[socket].inflater;

    cPacket *prasedMsg = praser->praseHttpRequest(msg, protocolType, &inflater);

    // Should be a HttpRequestMessage
    EV_DEBUG << "Socket data arrived on connection " << connId << ". Message=" << prasedMsg->getName() << ", kind=" << prasedMsg->getKind() << endl;

    // call the message handler to process the message.
    HttpReplyMessage *reply = handleReceivedMessage(prasedMsg);
    recvReqTimeVec.record(simTime());

    if (reply!=NULL)
    {
        /*
         * add delay time to send reply, in order to make HOL possible
         */
//        socket->send(reply); // Send to socket if the reply is non-zero.
        double replyDelay;
        //--need to get wait time from har file
        if (getWaitTimeFromHar)
        {
            /*
             * 1. get request URI
             */
            RealHttpReplyMessage *httpResponse = check_and_cast<RealHttpReplyMessage*>(reply);

            //can not get request URI, generate it ramdomly
            if (strcmp(httpResponse->requestURI(),"") == 0)
            {
                replyDelay = (double)(rdReplyDelay->draw());
            }
            else
            {
                // use the request-uri to get the real har response
                HeaderFrame timings = pHarParser->getTiming(httpResponse->requestURI());

                if (timings.size() == 0)
                {
                    //can not find the timings, generate it ramdomly
                    replyDelay = (double)(rdReplyDelay->draw());
                }
                else
                {
                    unsigned int i;
                    for (i = 0; i < timings.size(); ++i)
                    {
                        //---Note: here should cmp ":status-text" before ":status", or ":status" may get ":status-text" value
                        if (timings[i].key.find("wait") != string::npos)
                        {
                            replyDelay = (double)(atof(timings[i].val.c_str())/1000);
                            EV_DEBUG << "Find wait time for requestURI " << httpResponse->requestURI() <<", replyDelay is " << replyDelay << endl;
                            break;
                        }
                    }
                    if (i == timings.size())
                    {
                        //can not find wait in the timings, generate it ramdomly
                        replyDelay = (double)(rdReplyDelay->draw());
                    }
                }
            }

        }
        else
        {
            replyDelay = (double)(rdReplyDelay->draw());
        }
        EV_DEBUG << "Need to send message on socket " << socket << ". Message=" << reply->getName() << ", kind=" << reply->getKind() <<", sendDelay = " << replyDelay << endl;

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
                for (HttpReplyInfoQueue::iterator iter = replyInfoPerSocket[socket].begin(); iter != replyInfoPerSocket[socket].end(); iter++)
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
                    ReplyInfo replyInfo = {reply, true};
                    replyInfoPerSocket[socket].push_back(replyInfo);
                }
            }
        }
        else
        {
            //HttpReplyMessage *realhttpResponse = dynamic_cast<HttpReplyMessage*>(reply);
            ReplyInfo replyInfo = {reply, false};
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
    delete prasedMsg; // Delete the received message here. Must not be deleted in the handler!
    delete praser;
}

/*
 * Format an application TCP_TRANSFER_BYTESTREAM response message which can be sent though NSC TCP depence on the application layer protocol
 * the protocol type can be HTTP \ SPDY \ HTTPSM \ HTTPNF
 */
std::string HarServer::formatByteResponseMessage(TCPSocket *socket, HttpReplyMessage *httpResponse)
{
    /*
     * Note: this is a RealHttpReplyMessage * pointer indeed
     * just use dynamic_cast[check_and_cast] to change it
     */
    RealHttpReplyMessage *realhttpResponse = check_and_cast<RealHttpReplyMessage*>(httpResponse);
    std::string resHeader;

    switch(protocolType)
    {
        case HTTP:
            resHeader = formatHttpResponseMessageHeader(realhttpResponse);
            break;
        case SPDY_ZLIB_HTTP:
            resHeader = formatSpdyZlibHttpResponseMessageHeader(socket, realhttpResponse);
            break;
        case SPDY_HEADER_BLOCK:
            resHeader = formatSpdyZlibHeaderBlockResponseMessageHeader(realhttpResponse);
            break;
        case HTTPNF:
            resHeader = formatHttpNFResponseMessageHeader(realhttpResponse);
            break;
        default:
            throw cRuntimeError("Invalid Application protocol: %d", protocolType);
    }

    /*************************************Generate HTTP Response Body*************************************/

    EV_DEBUG << "Generate HTTP Response Body:"<< realhttpResponse->payload()<< endl;
    resHeader.append(realhttpResponse->payload());
    EV_DEBUG << "Payload Length is" <<(int64_t)strlen(realhttpResponse->payload()) << ". " << endl;
    if (realhttpResponse->contentLength() > (int64_t)strlen(realhttpResponse->payload()))
    {
        resHeader.append((realhttpResponse->contentLength() - (int64_t)strlen(realhttpResponse->payload())), '\0');
        EV_DEBUG << "Add NULL char. contentLength is" << realhttpResponse->contentLength() << ", payload Length is" <<(int64_t)strlen(realhttpResponse->payload()) << ". " << endl;
    }

    /*************************************Finish generating HTTP Response Body*************************************/

    delete realhttpResponse;
    realhttpResponse = NULL;

    return resHeader;
}

/*
 * Format a response message to HTTP Response Message Header
 * response   = Status-Line
                *(( general-header)
                | response-header
                | entity-header)CRLF)
                CRLF
                [ message-body ]
 */
std::string HarServer::formatHttpResponseMessageHeader(RealHttpReplyMessage *httpResponse)
{
    std::ostringstream str;

    /*
     * for "Invalid request string" case, requestURI is "" and the size is 0
     * simply call base class's formatHttpResponseMessageHeader
     */
    if (strcmp(httpResponse->requestURI(),"") == 0)
    {
        return NSCHttpServer::formatHttpResponseMessageHeader(httpResponse);
    }

    /*
     * get the har request for this request-uri
     */
    // use the request-uri to get the real har response
    HeaderFrame responseFrame = pHarParser->getResponse(httpResponse->requestURI());

    if (responseFrame.size() == 0)
    {
        return NSCHttpServer::formatHttpResponseMessageHeader(httpResponse);
    }
    else
    {
        //Find the response, modify the statistics-----step 1, set this not badRequest
        badRequests--;
    }

    /*
     * pre-process the headerFrame
     */
    string status("200");
    string statusText("OK");
    string version("HTTP/1.1");
    for (unsigned int i = 0; i < responseFrame.size(); ++i)
    {
        //---Note: here should cmp ":status-text" before ":status", or ":status" may get ":status-text" value
        if (responseFrame[i].key.find(":status-text") != string::npos)
        {
            statusText.assign(responseFrame[i].val);
        }
        else if (responseFrame[i].key.find(":status") != string::npos)
        {
            status.assign(responseFrame[i].val);
        }
        else if (responseFrame[i].key.find(":version") != string::npos)
        {
            if (responseFrame[i].val.find("1.1") != string::npos)
            {
                version.assign("HTTP/1.1");
            }
            else if (responseFrame[i].val.find("1.0") != string::npos)
            {
                version.assign("HTTP/1.0");
            }
            else
            {
                throw cRuntimeError("Invalid HTTP version: %s", responseFrame[i].val.c_str());
            }
        }
        //reset content-length
        else if (responseFrame[i].key.find("Content-Length") != string::npos)
        {
            httpResponse->setContentLength(std::atoi(responseFrame[i].val.c_str()));
        }
        //modify the statistics-----step 2, set this served resource depend on the content-type
        /*
         * in RFC1521, the content-type definition is :
         *    In the Augmented BNF notation of RFC 822, a Content-Type header field value is defined as follows:
                 content  :=   "Content-Type"  ":"  type  "/"  subtype  *(";" parameter)
               ; case-insensitive matching of type and subtype
                 type :=          "application"     / "audio"
                                   / "image"           / "message"
                                   / "multipart"  / "text"
                                   / "video"           / extension-token
                                   ; All values case-insensitive
                 extension-token :=  x-token / iana-token
         */
        else if (responseFrame[i].key.find("Content-Type") != string::npos)
        {
            if (responseFrame[i].val.find("image") != string::npos)
            {
                imgResourcesServed++;
            }
            else if (responseFrame[i].val.find("text") != string::npos || responseFrame[i].val.find("javascript") != string::npos
                    || responseFrame[i].val.find("xml") != string::npos)
            {
                textResourcesServed++;
            }
            //----Note: since javascript and xml type has application token, should do text statistic before media
            else if (responseFrame[i].val.find("application") != string::npos || responseFrame[i].val.find("audio") != string::npos
                    || responseFrame[i].val.find("video") != string::npos)
            {
                mediaResourcesServed++;
            }
            else
            {
                otherResourcesServed++;
            }
        }
    }

    /*************************************Generate HTTP Request Header*************************************/

    /**
     * 1.Generate Status-Line:
     * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
     */

    /** 1.1 HTTP-Version SP */
    str << version << " ";

    /** 1.2 Status-Code SP Reason-Phrase CRLF */
    str << status << " " << statusText << "\r\n";


    /**
     * 2.Generate the rest header
     */
    for (unsigned int i = 0; i < responseFrame.size(); ++i)
    {
        EV_DEBUG << "read headerFrame, key is " << responseFrame[i].key << endl;
        const string& k = responseFrame[i].key;
        const string& v = responseFrame[i].val;
        // the key without ":" is the ones belong to HTTP/1.1 header
        if (k[0] != ':')
        {
            str << k << ": " << v << "\r\n";
            EV_DEBUG << "generate header line: " << k << ": " << v << endl;
        }
    }
    /*************************************Finish generating HTTP Request Header*************************************/

    str << "\r\n";

    return str.str();
}

