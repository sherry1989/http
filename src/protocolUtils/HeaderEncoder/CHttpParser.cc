/*
 * CHttpParser.cc
 *
 *  Created on: Mar 28, 2013
 *      Author: qian
 */

#include "CHttpParser.h"

#include "HttpNodeBase.h"
#include "ByteArrayMessage.h"
#include "HttpLogdefs.h"

#include <cassert>
#include <iomanip>
#include <cctype>
#include <algorithm>

namespace RequestParser
{
    static http_parser_settings parserCallback =
    { message_begin_cb,           //http_cb      on_message_begin;
      request_url_cb,             //http_data_cb on_url;   //request only
      header_field_cb,            //http_data_cb on_header_field;
      header_value_cb,            //http_data_cb on_header_value;
      headers_complete_cb,        //http_cb      on_headers_complete;
      body_cb,                    //http_data_cb on_body;
      message_complete_cb         //http_cb      on_message_complete;
            };

    int request_url_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpRequestParser *parser;
        parser = reinterpret_cast<HttpRequestParser*>(htp->data);

        /*
         * set Request-Line of this http request message
         */
        char szReq[MAX_URL_LENGTH + 24];
        sprintf(szReq, "GET %.*s HTTP/1.1", len, buf);
        parser->httpRequest->setHeading(szReq);

        return 0;
    }

    int header_field_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpRequestParser *parser;
        parser = reinterpret_cast<HttpRequestParser*>(htp->data);

        if (parser->get_message_header_key_prev())
        {
            parser->append_last_message_header_key(buf, len);
        }
        else
        {
            parser->add_message_header(std::string(buf, len), "");
        }

        return 0;
    }

    int header_value_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpRequestParser *parser;
        parser = reinterpret_cast<HttpRequestParser*>(htp->data);

        if (parser->get_message_header_key_prev())
        {
            parser->set_last_message_header_value(std::string(buf, len));
        }
        else
        {
            parser->append_last_message_header_value(buf, len);
        }

        return 0;
    }

    int body_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpRequestParser *parser;
        parser = reinterpret_cast<HttpRequestParser*>(htp->data);

        parser->httpRequest->setPayload(buf);

        return 0;
    }

    int message_begin_cb(http_parser *htp)
    {
        HttpRequestParser *parser;
        parser = reinterpret_cast<HttpRequestParser*>(htp->data);

        parser->httpRequest->setKind(HTTPT_REQUEST_MESSAGE);

        parser->httpRequest->setBadRequest(false);
        return 0;
    }

    int headers_complete_cb(http_parser *htp)
    {
        HttpRequestParser *parser;
        parser = reinterpret_cast<HttpRequestParser*>(htp->data);

        parser->set_message_major(htp->http_major);
        parser->set_message_minor(htp->http_minor);
        parser->httpRequest->setKeepAlive(http_should_keep_alive(htp));
        parser->set_message_state(parser->HEADER_COMPLETE);
        parser->httpRequest->setProtocol(11);

        /*
         * set targetUrl of this http request message
         */
        std::string targetUrl;
        targetUrl.assign(parser->get_message_header("Host"));
        parser->httpRequest->setTargetUrl(targetUrl.c_str());
        EV_DEBUG_NOMODULE << "Set targetUrl from Host: " << parser->httpRequest->targetUrl() << "\r\n";

        parser->httpRequest->setBadRequest(false);
        return 0;
    }

    int message_complete_cb(http_parser *htp)
    {
        //    HttpRequestParser *parser;
        //    parser = reinterpret_cast<HttpRequestParser*>(htp->data);

        return 0;
    }
}

namespace ResponseParser
{
    static http_parser_settings parserCallback =
    { message_begin_cb,           //http_cb      on_message_begin;
      0,                          //http_data_cb on_url;*//*request only
      header_field_cb,            //http_data_cb on_header_field;
      header_value_cb,            //http_data_cb on_header_value;
      headers_complete_cb,        //http_cb      on_headers_complete;
      body_cb,                    //http_data_cb on_body;
      message_complete_cb         //http_cb      on_message_complete;
            };

    int header_field_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpResponseParser *parser;
        parser = reinterpret_cast<HttpResponseParser*>(htp->data);

        if (parser->get_message_header_key_prev())
        {
            parser->append_last_message_header_key(buf, len);
        }
        else
        {
            parser->add_message_header(std::string(buf, len), "");
        }

        return 0;
    }

    int header_value_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpResponseParser *parser;
        parser = reinterpret_cast<HttpResponseParser*>(htp->data);

        if (parser->get_message_header_key_prev())
        {
            parser->set_last_message_header_value(std::string(buf, len));
        }
        else
        {
            parser->append_last_message_header_value(buf, len);
        }

        return 0;
    }

    int body_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpResponseParser *parser;
        parser = reinterpret_cast<HttpResponseParser*>(htp->data);

        if (!parser->getMsgParseFin())
        {
            std::string payload;
            if (len != 0)
            {
                payload.assign(buf, len);
            }
            parser->httpResponse->setPayload(payload.c_str());
            //        parser->httpResponse->setPayload(buf);
            parser->set_recv_body_length(len);

            EV_DEBUG_NOMODULE << "payload is:" << parser->httpResponse->payload() << endl;
            EV_DEBUG_NOMODULE << "payload strlen is:" << strlen(parser->httpResponse->payload()) << endl;
            EV_DEBUG_NOMODULE << "buffer len is :" << len << endl;
            EV_DEBUG_NOMODULE << "httpResponse bytelength is: " << parser->httpResponse->getByteLength() << endl;

            parser->setMsgParseFin(true);

            //        parser->httpResponse->setPayload(buf);
            //        parser->httpResponse->setContentLength(len);
            //        parser->httpResponse->setByteLength(len);
        }
        return 0;
    }

    int message_begin_cb(http_parser *htp)
    {
        HttpResponseParser *parser;
        parser = reinterpret_cast<HttpResponseParser*>(htp->data);

        parser->httpResponse->setKind(HTTPT_RESPONSE_MESSAGE);
        parser->httpResponse->setProtocol(11);

        return 0;
    }

    int headers_complete_cb(http_parser *htp)
    {
        HttpResponseParser *parser;
        parser = reinterpret_cast<HttpResponseParser*>(htp->data);

        parser->httpResponse->setResult(htp->status_code);
        parser->set_message_major(htp->http_major);
        parser->set_message_minor(htp->http_minor);
        parser->httpResponse->setKeepAlive(http_should_keep_alive(htp));
        parser->set_message_state(parser->HEADER_COMPLETE);

        EV_DEBUG_NOMODULE << "Parsing HTTP Response Status code is:" << parser->httpResponse->result() << endl;

        /*
         * set contentType of this http response message
         */
        if (parser->httpResponse->result() == 200 && parser->get_message_header("Content-Type").size() != 0)
        {
            std::string contentType;
            contentType.assign(parser->get_message_header("Content-Type"));

            if (contentType.find("text/html") != std::string::npos)
            {
                parser->httpResponse->setContentType(CT_HTML);
            }
            else if (contentType.find("image") != std::string::npos)
            {
                parser->httpResponse->setContentType(CT_IMAGE);
            }
            else if (contentType.find("text") != std::string::npos
                    || contentType.find("javascript") != std::string::npos
                    || contentType.find("xml") != std::string::npos)
            {
                parser->httpResponse->setContentType(CT_TEXT);
            }
            //----Note: since javascript and xml type has application token, should do text statistic before media
            else if (contentType.find("application") != std::string::npos
                    || contentType.find("audio") != std::string::npos || contentType.find("video") != std::string::npos)
            {
                parser->httpResponse->setContentType(CT_UNKNOWN);
            }
            else
            {
                parser->httpResponse->setContentType(CT_UNKNOWN);
            }
        }

        /*
         * set originatorUrl of this http response message
         */
        if (parser->get_message_header("Content-Location").size() != 0)
        {
            std::string originatorUrl;
            originatorUrl.assign(parser->get_message_header("Content-Location"));
            parser->httpResponse->setOriginatorUrl(originatorUrl.c_str());
        }

        /*
         * set ContentLength(ByteLength of the HttpReplyMessage) of this http response message
         */
        if (parser->get_message_header("Content-Length").size() != 0)
        {
            std::string contentLength;
            contentLength.assign(parser->get_message_header("Content-Length"));
            parser->httpResponse->setByteLength(atol(contentLength.c_str()));
            EV_DEBUG_NOMODULE << "Parsing HTTP Response Content-Length is:" << parser->httpResponse->getByteLength()
                    << endl;
        }

        unsigned int status = parser->httpResponse->result();

        /*
         * set Status-Line of this http response message
         */
        char szReq[100];
        sprintf(szReq, "HTTP/1.1 %s", get_status_string(status));

        parser->httpResponse->setHeading(szReq);

        // Ignore the response body. HEAD response may contain
        // Content-Length or Transfer-Encoding: chunked.  Some server send
        // 304 status code with nonzero Content-Length, but without response
        // body. See
        // http://tools.ietf.org/html/draft-ietf-httpbis-p1-messaging-20#section-3.3
        return /*get_message_method() == "HEAD" ||*/
        (100 <= status && status <= 199) || status == 204 || status == 304 ? 1 : 0;
    }

    int message_complete_cb(http_parser *htp)
    {
        //    HttpResponseParser *parser;
        //    parser = reinterpret_cast<HttpResponseParser*>(htp->data);

        return 0;
    }
}

bool HttpRequestParser::nextHeaderSticked = false;
std::string HttpRequestParser::nextHeader;

bool HttpResponseParser::nextHeaderSticked = false;
std::string HttpResponseParser::nextHeader;

CHttpParser::CHttpParser() :
        message_state_(INITIAL), message_major_(1), message_minor_(1), chunked_message_(false), message_header_key_prev_(
                false), recv_window_size_(0), recv_body_length(0)
{
    // TODO Auto-generated constructor stub
    
}

CHttpParser::~CHttpParser()
{
    // TODO Auto-generated destructor stub
}

HttpRequestParser::HttpRequestParser() :
        httpRequest(new RealHttpRequestMessage()), pRequestParser(new http_parser())
{
    // TODO Auto-generated constructor stub
    http_parser_init(pRequestParser, HTTP_REQUEST);
    pRequestParser->data = this;
}

HttpRequestParser::~HttpRequestParser()
{
    // TODO Auto-generated destructor stub
    delete pRequestParser;
    pRequestParser = NULL;
}

HttpResponseParser::HttpResponseParser() :
        httpResponse(new RealHttpReplyMessage()), pResponseParser(new http_parser()), msgParseFin(false)
{
    // TODO Auto-generated constructor stub
    http_parser_init(pResponseParser, HTTP_RESPONSE);
    pResponseParser->data = this;
}

HttpResponseParser::~HttpResponseParser()
{
    // TODO Auto-generated destructor stub
    delete pResponseParser;
    pResponseParser = NULL;
}

RealHttpRequestMessage* HttpRequestParser::parseReqMsg(const char *data, size_t len, cPacket *msg)
{
    if (msg != NULL)
    {
        EV_DEBUG_NOMODULE << "Start parse a new http request. message name is " << msg->getName() << endl;
        EV_DEBUG_NOMODULE << "income bytes before parse ByteLength is:" << len << endl;
        EV_DEBUG_NOMODULE << "income bytes before parse ByteArray is:" << data << endl;
        httpRequest->setName(msg->getName());
        httpRequest->setSentFrom(msg->getSenderModule(), msg->getSenderGateId(), msg->getSendingTime());
    }
    else
    {
        EV_DEBUG_NOMODULE << "Start to parse the rest bytes" << endl;
    }

    EV_DEBUG_NOMODULE << "nextHeaderSticked before if is " << nextHeaderSticked << endl;

    std::string totalData;
    if (nextHeaderSticked)
    {
        totalData = nextHeader;
        totalData.append(data, len);

        EV_DEBUG_NOMODULE << "HTTP Request after deal to parse ByteLength is:" << totalData.size() << endl;
        EV_DEBUG_NOMODULE << "HTTP Request after deal to parse ByteArray is:" << totalData << endl;
    }
    else
    {
        totalData.assign(data, len);
    }

    size_t reqEnd = totalData.find("\r\n\r\n") + 4;
    size_t parseResult;
    if (reqEnd == totalData.size())
    {
        nextHeaderSticked = false;
        parseResult = http_parser_execute(pRequestParser, &RequestParser::parserCallback, totalData.c_str(), totalData.size());
    }
    else
    {
        nextHeaderSticked = true;
        nextHeader.assign(totalData, reqEnd, (totalData.size() - reqEnd));
        parseResult = http_parser_execute(pRequestParser, &RequestParser::parserCallback, totalData.c_str(), reqEnd);
    }

    EV_DEBUG_NOMODULE << "parseResult is:" << parseResult << endl;

    if (get_message_state() != HEADER_COMPLETE)
    {
        EV_DEBUG_NOMODULE << "HTTP Response not complete! " << endl;

        /**
         * restore the unparsed bytes in nextHeader and set nextHeaderSticked to true to wait next time's parsing
         */
        if (nextHeader.size() != 0)
        {
            EV_DEBUG_NOMODULE << "The unparsed bytes should restore to nextHeader." << endl;
            nextHeaderSticked = true;
            EV_DEBUG_NOMODULE << "nextHeaderSticked is set to " << nextHeaderSticked << endl;
            nextHeader.assign(totalData);
            EV_DEBUG_NOMODULE << "nextHeader is: " << nextHeader << endl;
        }

        return NULL;
    }

    http_errno htperr = HTTP_PARSER_ERRNO(pRequestParser);
    if (htperr == HPE_OK)
    {
        EV_DEBUG_NOMODULE << "Finish prasing http request message" << endl;
    }
    else
    {
        EV_ERROR_NOMODULE << "HTTP request parser failure: " << "(" << http_errno_name(htperr) << ") "
                << http_errno_description(htperr) << endl;
    }

    return httpRequest;
}

bool HttpRequestParser::ifNextHeaderSticked()
{
    return nextHeaderSticked;
}

/** Parse a received byte message to a RealHttpReplyMessage */
RealHttpReplyMessage* HttpResponseParser::parseResMsg(char *buf, size_t bufLength, cPacket *msg)
{
    /*
     * when deflate http header, use "\r\n\r\n" is not captable, use response state to decide whether finish parsing http header is better
     */
    /*
     * check if this response message contains message header
     *      if it contains message header, it's a start of response message, then try to parse it and check if it's a complete message
     *          if it's a complete message, return the httpResponse object
     *          else, wait for the coming messages contain the else message body
     *      else, it's a message contains the else message body of the message received earlier
     */
    //    if (strstr(buf, "\r\n\r\n") != NULL)
    if (get_message_state() != HEADER_COMPLETE)
    {
        if (msg != NULL)
        {
            EV_DEBUG_NOMODULE << "Start parse a new http response. message name is " << msg->getName() << endl;
            EV_DEBUG_NOMODULE << "income bytes before parse ByteLength is:" << bufLength << endl;
            EV_DEBUG_NOMODULE << "income bytes before parse ByteArray is:" << buf << endl;
            httpResponse->setName(msg->getName());
            httpResponse->setSentFrom(msg->getSenderModule(), msg->getSenderGateId(), msg->getSendingTime());
        }
        else
        {
            EV_DEBUG_NOMODULE << "Start to parse the rest bytes" << endl;
        }

        EV_DEBUG_NOMODULE << "nextHeaderSticked before if is " << nextHeaderSticked << endl;
        if (nextHeaderSticked)
        {
            char *newBuf = new char[nextHeader.length() + bufLength];
            memcpy(newBuf, nextHeader.c_str(), nextHeader.length());
            if (buf != NULL)
            {
                memcpy(newBuf + nextHeader.length(), buf, bufLength);
            }
//            delete[] buf;
            buf = newBuf;
            bufLength += nextHeader.length();
            nextHeaderSticked = false;
            EV_DEBUG_NOMODULE << "nextHeaderSticked is set to " << nextHeaderSticked << endl;

            EV_DEBUG_NOMODULE << "HTTP Response after deal to parse ByteLength is:" << bufLength << endl;
            EV_DEBUG_NOMODULE << "HTTP Response after deal to parse ByteArray is:" << buf << endl;
        }

        size_t parseResult = http_parser_execute(pResponseParser, &ResponseParser::parserCallback,
                                                 const_cast<const char*>(buf), bufLength);
        EV_DEBUG_NOMODULE << "parseResult is:" << parseResult << endl;

        if (get_message_state() != HEADER_COMPLETE)
        {
            EV_DEBUG_NOMODULE << "HTTP Response not complete! " << endl;

            /**
             * restore the unparsed bytes in nextHeader and set nextHeaderSticked to true to wait next time's parsing
             */
            if (nextHeader.size() != 0)
            {
                EV_DEBUG_NOMODULE << "The unparsed bytes should restore to nextHeader." << endl;
                nextHeaderSticked = true;
                EV_DEBUG_NOMODULE << "nextHeaderSticked is set to " << nextHeaderSticked << endl;
                EV_DEBUG_NOMODULE << "nextHeader is: " << nextHeader << endl;
            }

            return NULL;
        }
        else
        {
            nextHeader.clear();
            nextHeaderSticked = false;
            EV_DEBUG_NOMODULE << "nextHeaderSticked is set to " << nextHeaderSticked << endl;
        }

        http_errno htperr = HTTP_PARSER_ERRNO(pResponseParser);
        if (htperr == HPE_OK)
        {
            EV_DEBUG_NOMODULE << "Finish prasing http response message" << endl;
        }
        else
        {
            EV_ERROR_NOMODULE << "HTTP response parser failure: " << "(" << http_errno_name(htperr) << ") "
                    << http_errno_description(htperr) << endl;

            if (HPE_INVALID_CHUNK_SIZE == htperr)
            {
                EV_DEBUG_NOMODULE << "The rest bytes should restore to nextHeader." << endl;
                nextHeaderSticked = true;
                EV_DEBUG_NOMODULE << "nextHeaderSticked is set to " << nextHeaderSticked << endl;
                std::string totalData;
                totalData.assign(buf, bufLength);
                nextHeader.assign(totalData.substr(parseResult));
                EV_DEBUG_NOMODULE << "nextHeader is: " << nextHeader << endl;
                recv_body_length = 0;
                return httpResponse;
            }
        }

        EV_DEBUG_NOMODULE << "income bytes after parse ByteLength is:" << bufLength << endl;
        EV_DEBUG_NOMODULE << "income bytes after parse ByteArray is:" << buf << endl;

        if (httpResponse->getByteLength() == recv_body_length)
        {
            if (0 == recv_body_length)
            {
                nextHeader.clear();
                nextHeaderSticked = false;
                EV_DEBUG_NOMODULE << "nextHeaderSticked is set to " << nextHeaderSticked << endl;
                return httpResponse;
            }

            std::string totalData;
            totalData.assign(buf, bufLength);
            size_t startOfPayload = totalData.find(httpResponse->payload(), 0, recv_body_length);
            if (startOfPayload != std::string::npos)
            {
                size_t resTotalLength = startOfPayload + recv_body_length;
                if (resTotalLength < bufLength)
                {
                    EV_DEBUG_NOMODULE << "income bytes not complete parsed, parsed length is " << resTotalLength
                            << endl;
                    nextHeaderSticked = true;
                    EV_DEBUG_NOMODULE << "nextHeaderSticked is set to " << nextHeaderSticked << endl;
                    nextHeader.assign(totalData.substr(resTotalLength));
                    EV_DEBUG_NOMODULE << "nextHeader is: " << nextHeader << endl;
                    EV_DEBUG_NOMODULE << "nextHeader length is: " << nextHeader.size() << endl;

                    recv_body_length = 0;
                    return httpResponse;
                }
            }
        }
    }
    else
    {
        EV_DEBUG_NOMODULE << "Continue parse a parsed http response. message name is " << msg->getName() << endl;
        std::string payload;
        payload.assign(httpResponse->payload());
        payload.append(buf, bufLength);
        httpResponse->setPayload(payload.c_str());

        recv_body_length += bufLength;

        EV_DEBUG_NOMODULE << "payload add is: " << buf << endl;
    }

    EV_DEBUG_NOMODULE << "payload now is:" << httpResponse->payload() << endl;
    EV_DEBUG_NOMODULE << "payload length now is :" << recv_body_length << endl;
    EV_DEBUG_NOMODULE << "httpResponse bytelength is: " << httpResponse->getByteLength() << endl;

//    delete[] buf;

    /*
     * if received application bytelength till now for this message equals response's bytelength,
     * means there's no Application layer stick package, just return the whole http msg back to browser
     */
    if (httpResponse->getByteLength() == recv_body_length)
    {
        nextHeaderSticked = false;
        EV_DEBUG_NOMODULE << "nextHeaderSticked is set to " << nextHeaderSticked << endl;
        recv_body_length = 0;
        return httpResponse;
    }
    /*
     * else if received application bytelength till now for this message is larger than response's bytelength,
     * means there's Application layer stick package, there's content belonging to the next response's
     */
    else if (httpResponse->getByteLength() < recv_body_length)
    {
        nextHeaderSticked = true;
        EV_DEBUG_NOMODULE << "nextHeaderSticked is set to " << nextHeaderSticked << endl;
        unsigned int nextHeaderLength = recv_body_length - httpResponse->getByteLength();
        EV_DEBUG_NOMODULE << "nextHeaderLength is: " << nextHeaderLength << endl;
        std::string totalData;
        totalData.assign(httpResponse->payload(), recv_body_length);
        EV_DEBUG_NOMODULE << "totalData length is: " << totalData.length() << endl;
        std::string payload;
        payload.assign(totalData.substr(0, httpResponse->getByteLength()));
        nextHeader.assign(totalData.substr(httpResponse->getByteLength(), nextHeaderLength));
        EV_DEBUG_NOMODULE << "payload is: " << payload << endl;
        EV_DEBUG_NOMODULE << "nextHeader is: " << nextHeader << endl;

        /**
         * FOR_DEBUG
         */
        size_t startOfNextHeader = nextHeader.find_first_of("HTTP");
        if (startOfNextHeader != string::npos)
        {
            if (startOfNextHeader != 0)
            {
                EV_DEBUG_NOMODULE << "real start of nextHeader is: " << startOfNextHeader << endl;
                std::string realNextHeader = nextHeader.substr(startOfNextHeader);
                nextHeader.assign(realNextHeader);
            }
            else
            {
                EV_DEBUG_NOMODULE << "get nextHeader right! " << endl;
            }
        }
        else
        {
            EV_DEBUG_NOMODULE << "the nextHeader recoded doesn't start as a response http message " << endl;

            size_t startOfpayload = payload.find_first_of("HTTP");
            if (startOfpayload != string::npos)
            {
                EV_DEBUG_NOMODULE << "the nextHeader appears in the payload, start of payload is: " << startOfpayload
                        << endl;
                std::string realNextHeader = payload.substr(startOfpayload);
                realNextHeader.append(nextHeader);
                nextHeader.assign(realNextHeader);
            }
            else
            {
                EV_DEBUG_NOMODULE << "the nextHeader doesn't appear in the payload " << endl;
            }
        }

        httpResponse->setPayload(payload.c_str());
        recv_body_length = 0;
        return httpResponse;
    }
    else
    {
        return NULL;
    }
}

bool HttpResponseParser::ifNextHeaderSticked()
{
    return nextHeaderSticked;
}

void HttpResponseParser::setMsgParseFin(bool fin)
{
    msgParseFin = fin;
}

bool HttpResponseParser::getMsgParseFin()
{
    return msgParseFin;
}

void CHttpParser::add_message_header(const std::string& name, const std::string& value)
{
    message_header_key_prev_ = true;
    message_headers.push_back(std::make_pair(name, value));
}

void CHttpParser::set_last_message_header_value(const std::string& value)
{
    message_header_key_prev_ = false;
    Headers::value_type &item = message_headers.back();
    item.second = value;
    check_transfer_encoding_chunked(&chunked_message_, item);
    //check_connection_close(&message_connection_close_, item);
}

bool CHttpParser::get_message_header_key_prev() const
{
    return message_header_key_prev_;
}

void CHttpParser::append_last_message_header_key(const char *data, size_t len)
{
    assert(message_header_key_prev_);
    Headers::value_type &item = message_headers.back();
    item.first.append(data, len);
}

void CHttpParser::append_last_message_header_value(const char *data, size_t len)
{
    assert(!message_header_key_prev_);
    Headers::value_type &item = message_headers.back();
    item.second.append(data, len);
}

void CHttpParser::set_message_major(int major)
{
    message_major_ = major;
}

void CHttpParser::set_message_minor(int minor)
{
    message_minor_ = minor;
}

int CHttpParser::get_message_major() const
{
    return message_major_;
}

int CHttpParser::get_message_minor() const
{
    return message_minor_;
}

void CHttpParser::set_message_state(int state)
{
    message_state_ = state;
}

int CHttpParser::get_message_state() const
{
    return message_state_;
}

const Headers& CHttpParser::get_message_headers() const
{
    return message_headers;
}

void CHttpParser::check_header_field(bool *result, const Headers::value_type &item, const char *name, const char *value)
{
    if (strcmp(item.first.c_str(), name) == 0)
    {
        if (item.second.find(value) != std::string::npos)
        {
            *result = true;
        }
    }
}

std::string CHttpParser::get_message_header(const char *name)
{
    for (size_t i = 0; i < message_headers.size(); i++)
    {
        if (strcmp(message_headers[i].first.c_str(), name) == 0)
        {
            return message_headers[i].second;
        }
    }
    return "";
}

void CHttpParser::check_transfer_encoding_chunked(bool *chunked, const Headers::value_type &item)
{
    check_header_field(chunked, item, "transfer-encoding", "chunked");
}

bool CHttpParser::get_chunked_request() const
{
    return chunked_message_;
}

void CHttpParser::set_recv_body_length(int64 length)
{
    recv_body_length = length;
}

int64 CHttpParser::get_recv_body_length() const
{
    return recv_body_length;
}
