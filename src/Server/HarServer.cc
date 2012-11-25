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
    }
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
        return httpResponse;
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

/*
 * Format an application TCP_TRANSFER_BYTESTREAM response message which can be sent though NSC TCP depence on the application layer protocol
 * the protocol type can be HTTP \ SPDY \ HTTPSM \ HTTPNF
 */
std::string HarServer::formatByteResponseMessage(HttpReplyMessage *httpResponse)
{
    /*
     * Note: this is a RealHttpReplyMessage * pointer indeed
     * just use dynamic_cast to change it
     */
    RealHttpReplyMessage *realhttpResponse = dynamic_cast<RealHttpReplyMessage*>(httpResponse);
    std::string resHeader;

    switch(protocolType)
    {
        case HTTP:
            resHeader = formatHttpResponseMessageHeader(realhttpResponse);
            break;
        case SPDY_ZLIB_HTTP:
            resHeader = formatSpdyZlibHttpResponseMessageHeader(realhttpResponse);
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
std::string HarServer::formatHttpResponseMessageHeader(const RealHttpReplyMessage *httpResponse)
{
    std::ostringstream str;

    /////////###### need to modify

    /*
     * get the har request for this request-uri
     */
    // use the request-uri to get the real har response
    HeaderFrame responseFrame = pHarParser->getResponse(httpResponse->requestURI());

    if (responseFrame.size() == 0)
    {
        return NSCHttpServer::formatHttpResponseMessageHeader(httpResponse);
    }

    /*
     * pre-process the headerFrame
     */
    string status("200");
    string statusText("OK");
    string version("HTTP/1.1");
    for (unsigned int i = 0; i < responseFrame.size(); ++i)
    {
        if (responseFrame[i].key.find(":status") != string::npos)
        {
            statusText.assign(responseFrame[i].val);
        }
        else if (responseFrame[i].key.find(":status-text") != string::npos)
        {
            statusText.assign(responseFrame[i].val);
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

//    str << "\r\n";

    return str.str();
}
