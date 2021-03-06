/*
 * CHttpAsciiEncoder.cc
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#include "CHttpAsciiEncoder.h"

#include <stdexcept>
#include <iomanip>
#include <memory.h>
#include <algorithm>

CHttpAsciiEncoder::CHttpAsciiEncoder()
{
    // TODO Auto-generated constructor stub
}

CHttpAsciiEncoder::~CHttpAsciiEncoder()
{
    // TODO Auto-generated destructor stub
}

/**
 * Format a Request message to HTTP Request Message Header
 * Request   = Request-Line
 *(( general-header)
 | request-header
 | entity-header)CRLF)
 CRLF
 [ message-body ]
 */
std::string CHttpAsciiEncoder::generateReqMsgHeader(const HeaderFrame requestFrame, const string requestURI)
{
    std::ostringstream str;

    /**
     * pre-process the headerFrame
     */
    string method("");
    string version("");
    for (unsigned int i = 0; i < requestFrame.size(); ++i)
    {
        if (requestFrame[i].key.find(":method") != string::npos)
        {
            EV_DEBUG_NOMODULE << "original method is " << requestFrame[i].val << endl;
            //since the method get from har is lower, need to transform to upper
            method.assign(requestFrame[i].val);
            std::transform(method.begin(), method.end(), method.begin(), (int(*)(int))std::toupper);
            EV_DEBUG_NOMODULE << "generate method " << method << endl;
            continue;
        }
        else if (requestFrame[i].key.find(":version") != string::npos)
        {
            if (requestFrame[i].val.find("1.1") != string::npos)
            {
                version.assign("HTTP/1.1");
            }
            else if (requestFrame[i].val.find("1.0") != string::npos)
            {
                version.assign("HTTP/1.0");
            }
            else
            {
                throw cRuntimeError("Invalid HTTP version: %s", requestFrame[i].val.c_str());
            }
            EV_DEBUG_NOMODULE << "generate version " << version << endl;
            continue;
        }
    }

    /*************************************Generate HTTP Request Header*************************************/

    /**
     * 1.Generate Request-Line:
     * Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
     */

    /** 1.1 Method SP */
    str << method << " ";

    /** 1.2 Request-URI SP */
    str << requestURI << " ";

    /** 1.3 HTTP-Version CRLF */
    str << version << "\r\n";

    /**
     * 2.Generate the rest header
     */
    for (unsigned int i = 0; i < requestFrame.size(); ++i)
    {
        EV_DEBUG_NOMODULE << "read headerFrame, key is " << requestFrame[i].key << endl;
        const string& k = requestFrame[i].key;
        const string& v = requestFrame[i].val;
        // the key without ":" is the ones belong to HTTP/1.1 header
        if (k[0] != ':')
        {
            str << k << ": " << v << "\r\n";
            EV_DEBUG_NOMODULE << "generate header line: " << k << ": " << v << endl;
        }
        else if (k.find(":host") != string::npos)
        {
            str << "Host: " << v << "\r\n";
            EV_DEBUG_NOMODULE << "generate header line: " << "Host: " << v << endl;
        }
    }
    /*************************************Finish generating HTTP Request Header*************************************/

    str << "\r\n";

    return str.str();
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
std::string CHttpAsciiEncoder::generateResMsgHeader(const HeaderFrame responseFrame)
{
    std::ostringstream str;

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
        EV_DEBUG_NOMODULE << "read headerFrame, key is " << responseFrame[i].key << endl;
        const string& k = responseFrame[i].key;
        const string& v = responseFrame[i].val;
        // the key without ":" is the ones belong to HTTP/1.1 header
        if (k[0] != ':')
        {
            str << k << ": " << v << "\r\n";
            EV_DEBUG_NOMODULE << "generate header line: " << k << ": " << v << endl;
        }
    }
    /*************************************Finish generating HTTP Request Header*************************************/

    str << "\r\n";

    return str.str();
}

/** Parse a received byte message to a RealHttpRequestMessage */
RealHttpRequestMessage* CHttpAsciiEncoder::parseReqMsg(const char *data, size_t len, cPacket *msg, Socket_ID_Type sockID)
{
    if (httpParsers[sockID] == NULL)
    {
        httpParsers[sockID] = new HttpRequestParser();
        EV_DEBUG_NOMODULE << "New a HttpRequestParser" << endl;
    }
    else
    {
        EV_DEBUG_NOMODULE << "Use an old HttpRequestParser" << endl;
    }

    RealHttpRequestMessage* httpRequest = dynamic_cast<HttpRequestParser*>(httpParsers[sockID])->parseReqMsg(data, len, msg);

    if (httpRequest != NULL)
    {
        delete httpParsers[sockID];
        httpParsers[sockID] = NULL;
    }

    return httpRequest;
}

/** try to deal with the rest bytes in request message */
RealHttpRequestMessage *CHttpAsciiEncoder::dealWithRestReqBytes(Socket_ID_Type sockID)
{
    if (httpParsers[sockID] == NULL)
    {
        httpParsers[sockID] = new HttpRequestParser();
        EV_DEBUG_NOMODULE << "New a HttpRequestParser" << endl;
    }
    else
    {
        EV_DEBUG_NOMODULE << "Use an old HttpRequestParser" << endl;
    }

    /**
     * if there's bytes belong to next request, here must try to parse the bytes
     * (wait for the next application message may lose to parse a request)
     */
    if (dynamic_cast<HttpRequestParser*>(httpParsers[sockID])->ifNextHeaderSticked())
    {
        EV_DEBUG_NOMODULE << "There's bytes belong to next request, needs to try parse!" << endl;

        RealHttpRequestMessage* httpRequest = dynamic_cast<HttpRequestParser*>(httpParsers[sockID])->parseReqMsg(NULL, 0, NULL);

        if (httpRequest != NULL)
        {
            delete httpParsers[sockID];
            httpParsers[sockID] = NULL;
            EV_DEBUG_NOMODULE << "Finish this request's parsing, delete the HttpRequestParser" << endl;
        }
        else
        {
            /**
             * if there's bytes belong to next request, the parser must be released
             */
            if (dynamic_cast<HttpRequestParser*>(httpParsers[sockID])->ifNextHeaderSticked())
            {
                delete httpParsers[sockID];
                httpParsers[sockID] = NULL;
                EV_DEBUG_NOMODULE << "The request is not complete, cannot parse this time, delete the HttpRequestParser" << endl;
            }
        }

        return httpRequest;
    }
    else
    {
        EV_DEBUG_NOMODULE << "There's  no bytes left, don't need to try parse!" << endl;
        return NULL;
    }
}

/** Parse a received byte message to a RealHttpReplyMessage */
RealHttpReplyMessage* CHttpAsciiEncoder::parseResMsg(const char *data, size_t len, cPacket *msg, Socket_ID_Type sockID)
{
    if (httpParsers[sockID] == NULL)
    {
        httpParsers[sockID] = new HttpResponseParser();
        EV_DEBUG_NOMODULE << "New a HttpResponseParser in parseResMsg" << endl;
    }
    else
    {
        EV_DEBUG_NOMODULE << "Use an old HttpResponseParser in parseResMsg" << endl;
    }

    RealHttpReplyMessage* httpResponse = dynamic_cast<HttpResponseParser*>(httpParsers[sockID])->parseResMsg(const_cast<char*>(data), len, msg);

    if (httpResponse != NULL)
    {
        delete httpParsers[sockID];
        httpParsers[sockID] = NULL;
        EV_DEBUG_NOMODULE << "Finish this response's parsing, delete the HttpResponseParser" << endl;
    }

    return httpResponse;
}

/** try to deal with the rest bytes */
RealHttpReplyMessage *CHttpAsciiEncoder::dealWithRestResBytes(Socket_ID_Type sockID)
{
    if (httpParsers[sockID] == NULL)
    {
        httpParsers[sockID] = new HttpResponseParser();
        EV_DEBUG_NOMODULE << "New a HttpResponseParser in dealWithRestBytes" << endl;
    }
    else
    {
        EV_DEBUG_NOMODULE << "Use an old HttpResponseParser in dealWithRestBytes" << endl;
    }

    /**
     * if there's bytes belong to next response, here must try to parse the bytes
     * (wait for the next application message may lose to parse a response)
     */
    if (dynamic_cast<HttpResponseParser*>(httpParsers[sockID])->ifNextHeaderSticked())
    {
        EV_DEBUG_NOMODULE << "There's bytes belong to next response, needs to try parse!" << endl;

        RealHttpReplyMessage* httpResponse = dynamic_cast<HttpResponseParser*>(httpParsers[sockID])->parseResMsg(NULL, 0, NULL);

        if (httpResponse != NULL)
        {
            delete httpParsers[sockID];
            httpParsers[sockID] = NULL;
            EV_DEBUG_NOMODULE << "Finish this response's parsing, delete the HttpResponseParser" << endl;
        }
        else
        {
            /**
             * if there's bytes belong to next response, the parser must be released
             */
            if (dynamic_cast<HttpResponseParser*>(httpParsers[sockID])->ifNextHeaderSticked())
            {
                delete httpParsers[sockID];
                httpParsers[sockID] = NULL;
                EV_DEBUG_NOMODULE << "The response is not complete, cannot parse this time, delete the HttpResponseParser" << endl;
            }
        }

        return httpResponse;
    }
    else
    {
        EV_DEBUG_NOMODULE << "There's  no bytes left, don't need to try parse!" << endl;
        return NULL;
    }
}

/** initialize parsers related to sockID */
void CHttpAsciiEncoder::initParsers(Socket_ID_Type sockID)
{
    httpParsers[sockID] = NULL;
}

/** release parsers related to sockID */
void CHttpAsciiEncoder::releaseParsers(Socket_ID_Type sockID)
{
    if (httpParsers[sockID] != NULL)
    {
        delete httpParsers[sockID];
        httpParsers[sockID] = NULL;
    }
}

bool CHttpAsciiEncoder::ifNewMsg(Socket_ID_Type sockID)
{
    if (httpParsers[sockID] == NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}
