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

Define_Module(NSCHttpServer);

NSCHttpServer::NSCHttpServer()
{
    replyInfoPerSocket.clear();
}
NSCHttpServer::~NSCHttpServer()
{

}

//--added by wangqian, 2012-10-16
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

    protocolType = par("protocolType");

    numBroken = 0;
    socketsOpened = 0;

    WATCH(numBroken);
    WATCH(socketsOpened);
}
//--added end

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

void NSCHttpServer::socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent)
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
    //--modified by wangqian, 2012-10-15
    // change from cMessage to RealRealHttpReplyMessage to record the return of handleReceivedMessage
    RealHttpReplyMessage *reply = handleReceivedMessage(msg);
    //--modified end
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
                //--modified by wangqian, 2012-10-15
                // change message send method to adapt to the use of NSC
                //socket->send(reply); // Send to socket if the reply is non-zero.
                sendMessage(socket, reply);
                //--modified end
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
                    //--modified by wangqian, 2012-10-15
                    // change message send method to adapt to the use of NSC
                    //socket->send(reply); // Send to socket if the reply is non-zero.
                    sendMessage(socket, reply);
                    //--modified end
                }
                // there is earlier reply not send, record this reply
                else
                {
                    //RealHttpReplyMessage *replyMsg = dynamic_cast<RealHttpReplyMessage*>(reply);
                    ReplyInfo replyInfo = {reply, true};
                    replyInfoPerSocket[socket].push_back(replyInfo);
                }
            }
        }
        else
        {
            //RealHttpReplyMessage *replyMsg = dynamic_cast<RealHttpReplyMessage*>(reply);
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
    delete msg; // Delete the received message here. Must not be deleted in the handler!
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
    RealHttpReplyMessage* reply = dynamic_cast<RealHttpReplyMessage*>(msg);
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
                    //--modified by wangqian, 2012-10-15
                    // change message send method to adapt to the use of NSC
                    //msg = dynamic_cast<cMessage*>(replyInfoPerSocket[socket].front().reply);
                    //socket->send(msg); // Send to socket if the reply is non-zero.
                    sendMessage(socket, replyInfoPerSocket[socket].front().reply);
                    //--modified end

                    EV_INFO << "Send earlier message # " << i << ". Message=" << msg->getName() << ", kind=" << msg->getKind()<< endl;
                    replyInfoPerSocket[socket].pop_front();
                }

                //--modified by wangqian, 2012-10-15
                // change message send method to adapt to the use of NSC
                //msg = dynamic_cast<cMessage*>(reply);
                //socket->send(msg); // Send to socket if the reply is non-zero.
                sendMessage(socket, reply);
                //--modified end

                EV_INFO << "Send message =" << msg->getName() << ", kind=" << msg->getKind()<< endl;
                replyInfoPerSocket[socket].pop_front();

                while (!replyInfoPerSocket[socket].empty())
                {
                    if (replyInfoPerSocket[socket].front().readyToSend)
                    {
                        //--modified by wangqian, 2012-10-15
                        // change message send method to adapt to the use of NSC
                        //msg = dynamic_cast<cMessage*>(replyInfoPerSocket[socket].front().reply);
                        //socket->send(msg); // Send to socket if the reply is non-zero.
                        sendMessage(socket, replyInfoPerSocket[socket].front().reply);
                        //--modified end

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
            //--modified by wangqian, 2012-10-15
            // change the dataTransferMode from TCP_TRANSFER_OBJECT to TCP_TRANSFER_BYTESTREAM to support NSC TCP
            socket->setDataTransferMode(TCP_TRANSFER_BYTESTREAM);
            //--modified end
            socket->setCallbackObject(this, socket);
            sockCollection.addSocket(socket);
        }
        EV_DEBUG << "Process the message " << msg->getName() << endl;
        socket->processMessage(msg);
    }
    updateDisplay();
}

//--added by wangqian, 2012-10-15
/*
 * Handle a received data message, e.g. check if the content requested exists.
 * change the HttpServerBase::handleReceivedMessage return type from cMessage to HttpReplyMessage to record the return of handleReceivedMessage
 */
RealHttpReplyMessage* NSCHttpServer::handleReceivedMessage(cMessage *msg)
{
    HttpRequestMessage *request = check_and_cast<HttpRequestMessage *>(msg);
    if (request==NULL)
        error("Message (%s)%s is not a valid request", msg->getClassName(), msg->getName());

    EV_DEBUG << "Handling received message " << msg->getName() << ". Target URL: " << request->targetUrl() << endl;

    logRequest(request);

    if (extractServerName(request->targetUrl()) != hostName)
    {
        // This should never happen but lets check
        error("Received message intended for '%s'", request->targetUrl()); // TODO: DEBUG HERE
        return NULL;
    }

    RealHttpReplyMessage* replymsg;

    // Parse the request string on spaces
    cStringTokenizer tokenizer = cStringTokenizer(request->heading(), " ");
    std::vector<std::string> res = tokenizer.asVector();
    if (res.size() != 3)
    {
        EV_ERROR << "Invalid request string: " << request->heading() << endl;
        replymsg = dynamic_cast<RealHttpReplyMessage*>(generateErrorReply(request, 400));
        logResponse(replymsg);
        return replymsg;
    }

    if (request->badRequest())
    {
        // Bad requests get a 404 reply.
        EV_ERROR << "Bad request - bad flag set. Message: " << request->getName() << endl;
        replymsg = dynamic_cast<RealHttpReplyMessage*>(generateErrorReply(request, 404));
    }
    else if (res[0] == "GET")
    {
        replymsg = dynamic_cast<RealHttpReplyMessage*>(handleGetRequest(request, res[1])); // Pass in the resource string part
    }
    else
    {
        EV_ERROR << "Unsupported request type " << res[0] << " for " << request->heading() << endl;
        replymsg = dynamic_cast<RealHttpReplyMessage*>(generateErrorReply(request, 400));
    }

    if (replymsg!=NULL)
        logResponse(replymsg);

    return replymsg;
}


/** send HTTP reply message though TCPSocket depends on the TCP DataTransferMode*/
void NSCHttpServer::sendMessage(TCPSocket *socket, RealHttpReplyMessage* reply)
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
            std::string resByteArray = formatByteResponseMessage(reply);

            sendBytes = resByteArray.length();

            ByteArrayMessage *sendMsg = new ByteArrayMessage("RealHttpReplyMessage");
            char *ptr = new char[sendBytes];
            strcpy(ptr, resByteArray.c_str());
            sendMsg->getByteArray().assignBuffer(ptr, sendBytes);
            sendMsg->setByteLength(sendBytes);

            socket->send(sendMsg);
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
std::string NSCHttpServer::formatByteResponseMessage(const RealHttpReplyMessage* httpResponse)
{
    switch(protocolType)
    {
        case HTTP:
            return formatHttpResponseMessage(httpResponse);
        case SPDY:
            return formatSpdyResponseMessage(httpResponse);
        case HTTPSM:
            return formatHttpSMResponseMessage(httpResponse);
        case HTTPNF:
            return formatHttpNFResponseMessage(httpResponse);
        default:
            throw cRuntimeError("Invalid Application protocol: %d", protocolType);
    }
}

/*
 * Format a response message to HTTP Response Message
 * response   = Status-Line
                *(( general-header)
                | response-header
                | entity-header)CRLF)
                CRLF
                [ message-body ]
 */
std::string NSCHttpServer::formatHttpResponseMessage(const RealHttpReplyMessage* httpResponse)
{
    std::ostringstream str;

    /*************************************Generate HTTP Response Header*************************************/

    /**
     * 1.Generate Status-Line:
     * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
     */

    /** 1.1 HTTP-Version SP */
    switch (httpResponse->protocol())
    {
      case 10:
        str << "HTTP/1.0";
        break;
      case 11:
        str << "HTTP/1.1";
        break;
      default:
        throw cRuntimeError("Invalid HTTP Status code: %d", httpResponse->protocol());
        break;
    }
    str << " ";

    /** 1.2 Status-Code SP Reason-Phrase CRLF */
    str << get_status_string(httpResponse->result()) << "\r\n";


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
       request/response chain.
     * Cache-Control   = "Cache-Control" ":" 1#cache-directive
       cache-directive = cache-request-directive
                         | cache-response-directive
       cache-request-directive = "no-cache"
                                 | "no-store"
                                 | "max-age" "=" delta-seconds
                                 | "max-stale" [ "=" delta-seconds ]
                                 | "min-fresh" "=" delta-seconds
                                 | "no-transform"
                                 | "only-if-cached"
                                 | cache-extension
       cache-response-directive =  "public"
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
    if (strcmp(httpResponse->cacheControl(),"") != 0)
    {
      str << "Cache-Control: "<< httpResponse->cacheControl() << "\r\n";
    }
    else
    {
      //if the Cache-Control not set, don't send it
    }

    /** 2.2 Connection */
    /*
     * HTTP/1.1 applications that do not support persistent connections MUST
       include the "close" connection option in every message.
     * Connection = "Connection" ":" 1#(connection-token)
       connection-token  = token
     */
    if (httpResponse->keepAlive())
    {
        str << "Connection: Keep-Alive\r\n";
    }
    else
    {
        str << "Connection: close\r\n";
    }

    /** 2.3 Date */
    /*
     * The Date general-header field represents the date and time at which
       the message was originated
     * Date  = "Date" ":" HTTP-date
     */
    if (strcmp(httpResponse->date(),"") != 0)
    {
      str << "Date: "<< httpResponse->date() << "\r\n";
    }
    else
    {
      str << "Date: Tue, 16 Oct 2012 05:35:24 GMT\r\n";
    }

    /** 2.4 Pragma */
    /*
     * The Pragma general-header field is used to include implementation-
       specific directives that might apply to any recipient along the
       request/response chain. All pragma directives specify optional
       behavior from the viewpoint of the protocol; however, some systems
       MAY require that behavior be consistent with the directives.
     * Pragma            = "Pragma" ":" 1#pragma-directive
       pragma-directive  = "no-cache" | extension-pragma
       extension-pragma  = token [ "=" ( token | quoted-string ) ]
     */
    if (strcmp(httpResponse->pragma(),"") != 0)
    {
      str << "Pragma: "<< httpResponse->pragma() << "\r\n";
    }
    else
    {
      //if the Pragma not set, don't send it
    }

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
       response to indicate which protocol(s) are being switched.
     * Upgrade        = "Upgrade" ":" 1#product
     */

    /** 2.8 Via */
    /*
     * The Via general-header field MUST be used by gateways and proxies to
       indicate the intermediate protocols and recipients between the user
       agent and the server on requests, and between the origin server and
       the client on responses. It is analogous to the "Received" field of
       RFC 822 [9] and is intended to be used for tracking message forwards,
       avoiding request loops, and identifying the protocol capabilities of
       all senders along the request/response chain.
    * Via =  "Via" ":" 1#( received-protocol received-by [ comment ] )
      received-protocol = [ protocol-name "/" ] protocol-version
      protocol-name     = token
      protocol-version  = token
      received-by       = ( host [ ":" port ] ) | pseudonym
      pseudonym         = token
     */
    if (strcmp(httpResponse->via(),"") != 0)
    {
      str << "Via: "<< httpResponse->via() << "\r\n";
    }
    else
    {
      //if the Via not set, don't send it
    }

    /** 2.9 Warning */
    /*
     * The Warning general-header field is used to carry additional
       information about the status or transformation of a message which
       might not be reflected in the message. This information is typically
       used to warn about a possible lack of semantic transparency from
       caching operations or transformations applied to the entity body of
       the message.
     * Warning headers are sent with responses using:
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
     * 3. Generate response-header:
     * response-header = Accept-Ranges
                         |Age
                         |Etag
                         |Location
                         |Proxy-Autenticate
                         |Retry-After
                         |Server
                         |Vary
                         |WWW-Authenticate
     */

    /** 3.1 Accept-Ranges */
    /*
     * The Accept-Ranges response-header field allows the server to
       indicate its acceptance of range requests for a resource:
     * Accept-Ranges     = "Accept-Ranges" ":" acceptable-ranges
       acceptable-ranges = 1#range-unit | "none"
     */
    str << "Accept-Ranges: "<< httpResponse->acceptRanges() << "\r\n";

    /** 3.2 Age */
    /*
     * The Age response-header field conveys the sender's estimate of the
       amount of time since the response (or its revalidation) was
       generated at the origin server. A cached response is "fresh" if
       its age does not exceed its freshness lifetime. Age values are
       calculated as specified in section 13.2.3.
     * Age values are non-negative decimal integers, representing time in
       seconds.
     * An HTTP/1.1 server that includes a cache MUST include an Age header
       field in every response generated from its own cache.
     * Age = "Age" ":" age-value
       age-value = delta-seconds
     */
    str << "Age: "<< httpResponse->age() << "\r\n";

    /** 3.3 Etag */
    /*
     * The ETag response-header field provides the current value of the
       entity tag for the requested variant.
     * The entity tag MAY be used for comparison with other entities from the same resource.
     * ETag = "ETag" ":" entity-tag
     */
    if (strcmp(httpResponse->etag(),"") != 0)
    {
      str << "Etag: "<< httpResponse->etag() << "\r\n";
    }
    else
    {
      //if the Etag not set, don't send it
    }

    /** 3.4 Location */
    /*
     * The Location response-header field is used to redirect the recipient
       to a location other than the Request-URI for completion of the
       request or identification of a new resource. For 201 (Created)
       responses, the Location is that of the new resource which was created
       by the request. For 3xx responses, the location SHOULD indicate the
       server's preferred URI for automatic redirection to the resource. The
       field value consists of a single absolute URI.
     * Location       = "Location" ":" absoluteURI
     */
    if (strcmp(httpResponse->location(),"") != 0)
    {
      str << "Location: "<< httpResponse->location() << "\r\n";
    }
    else
    {
      //if the Location not set, don't send it
    }

    /** 3.5 Proxy-Autenticate */
    /*
     * The Proxy-Authenticate response-header field MUST be included as part
       of a 407 (Proxy Authentication Required) response. The field value
       consists of a challenge that indicates the authentication scheme and
       parameters applicable to the proxy for this Request-URI.
     * Proxy-Authenticate  = "Proxy-Authenticate" ":" 1#challenge
     */

    /** 3.6 Retry-After */
    /*
     * The Retry-After response-header field can be used with a 503 (Service
       Unavailable) response to indicate how long the service is expected to
       be unavailable to the requesting client. This field MAY also be used
       with any 3xx (Redirection) response to indicate the minimum time the
       user-agent is asked wait before issuing the redirected request. The
       value of this field can be either an HTTP-date or an integer number
       of seconds (in decimal) after the time of the response.
     * Retry-After  = "Retry-After" ":" ( HTTP-date | delta-seconds )
     */

    /** 3.7 Server */
    /*
     * The Server response-header field contains information about the
       software used by the origin server to handle the request. The field
       can contain multiple product tokens (section 3.8) and comments
       identifying the server and any significant subproducts. The product
       tokens are listed in order of their significance for identifying the
       application.
     *  Server         = "Server" ":" 1*( product | comment )
     */
    if (strcmp(httpResponse->server(),"") != 0)
    {
      str << "Server: "<< httpResponse->server() << "\r\n";
    }
    else
    {
      str << "Server: Apache/2.2.20 (Ubuntu)\r\n";
    }


    /** 3.8 Vary */
    /*
     * An HTTP/1.1 server SHOULD include a Vary header field with any
       cacheable response that is subject to server-driven negotiation.
       Doing so allows a cache to properly interpret future requests on that
       resource and informs the user agent about the presence of negotiation
       on that resource.
     * Vary  = "Vary" ":" ( "*" | 1#field-name )
     */
    if (strcmp(httpResponse->vary(),"") != 0)
    {
      str << "Vary: "<< httpResponse->vary() << "\r\n";
    }
    else
    {
      str << "Vary: Accept-Encoding\r\n";
    }

    /** 3.9 WWW-Authenticate */
    /*
     * The WWW-Authenticate response-header field MUST be included in 401
       (Unauthorized) response messages. The field value consists of at
       least one challenge that indicates the authentication scheme(s) and
       parameters applicable to the Request-URI.
     * WWW-Authenticate  = "WWW-Authenticate" ":" 1#challenge
     */


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
       response MUST include a Content-Encoding entity-header (section
       14.11) that lists the non-identity content-coding(s) used.
     * Content-Encoding  = "Content-Encoding" ":" 1#content-coding
     */
    if (strcmp(httpResponse->contentEncoding(),"") != 0)
    {
      str << "Content-Encoding: "<< httpResponse->contentEncoding() << "\r\n";
    }
    else
    {
      str << "Content-Encoding: gzip\r\n";
    }

    /** 4.3 Content-Language */
    /*
     * The Content-Language entity-header field describes the natural
       language(s) of the intended audience for the enclosed entity.
     * Content-Language  = "Content-Language" ":" 1#language-tag
     */
    if (strcmp(httpResponse->contentLanguage(),"") != 0)
    {
      str << "Content-Language: "<< httpResponse->contentLanguage() << "\r\n";
    }
    else
    {
      //if the Content-Language not set, don't send it
    }

    /** 4.4 Content-Length */
    /*
     * The Content-Length entity-header field indicates the size of the
       entity-body, in decimal number of OCTETs, sent to the recipient or,
       in the case of the HEAD method, the size of the entity-body that
       would have been sent had the request been a GET.
     * Content-Length    = "Content-Length" ":" 1*DIGIT
     */
    str << "Content-Length: " << httpResponse->contentLength() << "\r\n";

    /** 4.5 Content-Location */
    /*
     * The Content-Location entity-header field MAY be used to supply the
       resource location for the entity enclosed in the message when that
       entity is accessible from a location separate from the requested
       resource's URI.
     * Content-Location = "Content-Location" ":" ( absoluteURI | relativeURI )
     */
    if (strcmp(httpResponse->contentLocation(),"") != 0)
    {
      str << "Content-Location: "<< httpResponse->contentLocation() << "\r\n";
    }
    else
    {
      //if the Content-Location not set, don't send it
    }

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
    switch (httpResponse->contentType())
    {
      case CT_HTML:
        str << "Content-Type: text/html\r\n";
        break;
      case CT_TEXT:
        str << "Content-Type: application/javascript\r\n";
        break;
      case CT_IMAGE:
        str << "Content-Type: image/jpeg\r\n";
        break;
      default:
        throw cRuntimeError("Invalid HTTP Content Type: %d", httpResponse->contentType());
        break;
    }

    /** 4.9 Expires */
    /*
     * The Expires entity-header field gives the date/time after which the
       response is considered stale.
     * The format is an absolute date and time as defined by HTTP-date in
       section 3.3.1; it MUST be in RFC 1123 date format:
     * Expires = "Expires" ":" HTTP-date
     */
    if (strcmp(httpResponse->expires(),"") != 0)
    {
      str << "Expires: "<< httpResponse->expires() << "\r\n";
    }
    else
    {
      str << "Expires: Wed, 16 Oct 2013 05:35:24 GMT\r\n";
    }

    /** 4.10 Last-Modified */
    /*
     * The Last-Modified entity-header field indicates the date and time at
       which the origin server believes the variant was last modified.
     * Last-Modified  = "Last-Modified" ":" HTTP-date
     */
    if (strcmp(httpResponse->lastModified(),"") != 0)
    {
      str << "Last-Modified: "<< httpResponse->lastModified() << "\r\n";
    }
    else
    {
      str << "Last-Modified: Tue, 16 Oct 2012 05:35:24 GMT\r\n";
    }

    /*************************************Finish generating HTTP Response Header*************************************/

    str << "\r\n";

    /*************************************Generate HTTP Response Body*************************************/

    str << httpResponse->payload();

    /*************************************Finish generating HTTP Response Body*************************************/

    return str.str();
}

/** Format a response message to HTTP Response Message */
std::string NSCHttpServer::formatSpdyResponseMessage(const RealHttpReplyMessage* httpResponse)
{
    std::ostringstream str;


    return str.str();
}

/** Format a response message to HTTP Response Message */
std::string NSCHttpServer::formatHttpSMResponseMessage(const RealHttpReplyMessage* httpResponse)
{
    std::ostringstream str;



    return str.str();
}

/** Format a response message to HTTP Response Message */
std::string NSCHttpServer::formatHttpNFResponseMessage(const RealHttpReplyMessage* httpResponse)
{
    std::ostringstream str;



    return str.str();
}

//--added end
