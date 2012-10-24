/*
 * httpResponsePraser.cc
 *
 *  Created on: Oct 17, 2012
 *      Author: qian
 */

#include "HttpResponsePraser.h"
#include "HttpNodeBase.h"
#include "ByteArrayMessage.h"

#include <cassert>

namespace ResponsePraser
{
    static http_parser_settings parserCallback=
    {
        message_begin_cb,           //http_cb      on_message_begin;
        0,                          //http_data_cb on_url;*//*request only
        header_field_cb,            //http_data_cb on_header_field;
        header_value_cb,            //http_data_cb on_header_value;
        headers_complete_cb,        //http_cb      on_headers_complete;
        body_cb,                    //http_data_cb on_body;
        message_complete_cb         //http_cb      on_message_complete;
    };

    int header_field_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpResponsePraser *parser;
        parser = reinterpret_cast<HttpResponsePraser*>(htp->data);

        if (parser->get_response_header_key_prev())
        {
            parser->append_last_response_header_key(buf, len);
        }
        else
        {
            parser->add_response_header(std::string(buf, len), "");
        }

        return 0;
    }

    int header_value_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpResponsePraser *parser;
        parser = reinterpret_cast<HttpResponsePraser*>(htp->data);

        if (parser->get_response_header_key_prev())
        {
            parser->set_last_response_header_value(std::string(buf, len));
        }
        else
        {
            parser->append_last_response_header_value(buf, len);
        }

        return 0;
    }

    int body_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpResponsePraser *parser;
        parser = reinterpret_cast<HttpResponsePraser*>(htp->data);

//        std::string payload;
//        if (len != 0)
//        {
//            payload.assign(buf, len);
//        }
//        parser->httpResponse->setPayload(payload.c_str());
        parser->httpResponse->setPayload(buf);
        parser->set_recv_body_length(len);

        EV << "payload strlen is:" << strlen(parser->httpResponse->payload()) << endl;
        EV << "buffer len is :" << len << endl;
        EV << "httpResponse bytelength is: " << parser->httpResponse->getByteLength() << endl;

//        parser->httpResponse->setPayload(buf);
//        parser->httpResponse->setContentLength(len);
//        parser->httpResponse->setByteLength(len);
        return 0;
    }

    int message_begin_cb(http_parser *htp)
    {
        HttpResponsePraser *parser;
        parser = reinterpret_cast<HttpResponsePraser*>(htp->data);

        parser->httpResponse->setKind(HTTPT_RESPONSE_MESSAGE);
        parser->httpResponse->setProtocol(11);

        return 0;
    }

    int headers_complete_cb(http_parser *htp)
    {
        HttpResponsePraser *parser;
        parser = reinterpret_cast<HttpResponsePraser*>(htp->data);

        parser->httpResponse->setResult(htp->status_code);
        parser->set_response_major(htp->http_major);
        parser->set_response_minor(htp->http_minor);
        parser->httpResponse->setKeepAlive(http_should_keep_alive(htp));
        parser->set_response_state(parser->HEADER_COMPLETE);

        /*
         * set contentType of this http response message
         */
        if (parser->httpResponse->result() == 200)
        {
            std::string contentType;
            contentType.assign(parser->getResponseHeader("Content-Type"));

            if (strcmp(contentType.c_str(), "text/html") == 0)
            {
                parser->httpResponse->setContentType(CT_HTML);
            }
            else if (strcmp(contentType.c_str(), "application/javascript") == 0)
            {
                parser->httpResponse->setContentType(CT_TEXT);
            }
            else if (strcmp(contentType.c_str(), "image/jpeg") == 0)
            {
                parser->httpResponse->setContentType(CT_IMAGE);
            }
            else
            {
                throw cRuntimeError("Invalid HTTP Content Type");
            }
        }

        /*
         * set originatorUrl of this http response message
         */
        std::string originatorUrl;
        originatorUrl.assign(parser->getResponseHeader("Content-Location"));
        parser->httpResponse->setOriginatorUrl(originatorUrl.c_str());

        /*
         * set ContentLength(ByteLength of the HttpReplyMessage) of this http response message
         */
        std::string contentLength;
        contentLength.assign(parser->getResponseHeader("Content-Length"));
        parser->httpResponse->setByteLength(atol(contentLength.c_str()));
        EV << "Parsing HTTP Response Content-Length is:"<< parser->httpResponse->getByteLength() << endl;

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
        return /*get_request_method() == "HEAD" ||*/ (100 <= status && status <= 199) || status == 204
                || status == 304 ? 1 : 0;
    }

    int message_complete_cb(http_parser *htp)
    {
    //    HttpResponsePraser *parser;
    //    parser = reinterpret_cast<HttpResponsePraser*>(htp->data);

        return 0;
    }
}

HttpResponsePraser::HttpResponsePraser()
:httpResponse(new RealHttpReplyMessage()),
 response_state_(INITIAL),
 response_major_(1),
 response_minor_(1),
 chunked_response_(false),
 response_header_key_prev_(false),
 pResponseParser(new http_parser()),
 recv_window_size_(0),
 recv_body_length(0)
{
    // TODO Auto-generated constructor stub
    http_parser_init(pResponseParser, HTTP_RESPONSE);
    pResponseParser->data = this;
}

HttpResponsePraser::~HttpResponsePraser()
{
    // TODO Auto-generated destructor stub
    delete pResponseParser;
    pResponseParser = NULL;
}

RealHttpReplyMessage* HttpResponsePraser::praseHttpResponse(cPacket *msg)
{
    ByteArrayMessage *byMsg = check_and_cast<ByteArrayMessage*>(msg);
    size_t bufLength = byMsg->getByteLength();
    char *buf = new char[bufLength];
    byMsg->getByteArray().copyDataToBuffer(buf, bufLength, 0);

    EV << "HTTP Response to parse ByteLength is:" << bufLength << endl;
    EV << "HTTP Response to parse ByteArray is:" << buf << endl;

    //--modified by wangqian, 2012-10-23
    /*
     * check if this response message contains message header
     *      if it contains message header, it's a start of response message, then try to parse it and check if it's a complete message
     *          if it's a complete message, return the httpResponse object
     *          else, wait for the coming messages contain the else message body
     *      else, it's a message contains the else message body of the message received earlier
     */
    if (strstr(buf, "\r\n\r\n") != NULL)
    {
        httpResponse->setName(msg->getName());
        httpResponse->setSentFrom(msg->getSenderModule(), msg->getSenderGateId(), msg->getSendingTime());

        /*size_t nread = */http_parser_execute(pResponseParser, &ResponsePraser::parserCallback, const_cast<const char*>(buf), bufLength);
    //
    //    evbuffer_drain(input, nread);
    //    http_errno htperr = HTTP_PARSER_ERRNO(response_htp_);
    //    if (htperr == HPE_OK)
    //    {
    //        return 0;
    //    }
    //    else
    //    {
    //        if (ENABLE_LOG)
    //        {
    //            LOG(INFO) << "Downstream HTTP parser failure: " << "(" << http_errno_name(htperr) << ") "
    //                    << http_errno_description(htperr);
    //        }
    //        return SHRPX_ERR_HTTP_PARSE;
    //    }

    }
    else
    {
        std::string payload;
        payload.assign(httpResponse->payload());
        payload.append(buf, bufLength);
        httpResponse->setPayload(payload.c_str());

        recv_body_length += bufLength;

//        EV << "payload add is: " << buf << endl;
    }

//    EV << "payload now is:" << httpResponse->payload() << endl;
//    EV << "payload length now is :" << recv_body_length << endl;
//    EV << "httpResponse bytelength is: " << httpResponse->getByteLength() << endl;


    delete msg;

    if (httpResponse->getByteLength() <= recv_body_length)
    {
        recv_body_length = 0;
        return httpResponse;
    }
    else
    {
        return NULL;
    }
    //--modified end
}

void HttpResponsePraser::add_response_header(const std::string& name, const std::string& value)
{
  response_header_key_prev_ = true;
  responseHeaders.push_back(std::make_pair(name, value));
}

void HttpResponsePraser::set_last_response_header_value(const std::string& value)
{
  response_header_key_prev_ = false;
  Headers::value_type &item = responseHeaders.back();
  item.second = value;
  check_transfer_encoding_chunked(&chunked_response_, item);
  //check_connection_close(&response_connection_close_, item);
}

bool HttpResponsePraser::get_response_header_key_prev() const
{
  return response_header_key_prev_;
}

void HttpResponsePraser::append_last_response_header_key(const char *data, size_t len)
{
  assert(response_header_key_prev_);
  Headers::value_type &item = responseHeaders.back();
  item.first.append(data, len);
}

void HttpResponsePraser::append_last_response_header_value(const char *data, size_t len)
{
  assert(!response_header_key_prev_);
  Headers::value_type &item = responseHeaders.back();
  item.second.append(data, len);
}

void HttpResponsePraser::set_response_major(int major)
{
  response_major_ = major;
}

void HttpResponsePraser::set_response_minor(int minor)
{
  response_minor_ = minor;
}

int HttpResponsePraser::get_response_major() const
{
  return response_major_;
}

int HttpResponsePraser::get_response_minor() const
{
  return response_minor_;
}

void HttpResponsePraser::set_response_state(int state)
{
  response_state_ = state;
}

int HttpResponsePraser::get_response_state() const
{
  return response_state_;
}

const Headers& HttpResponsePraser::get_response_headers() const
{
  return responseHeaders;
}

void HttpResponsePraser::check_header_field(bool *result, const Headers::value_type &item, const char *name,
                                            const char *value)
{
    if (strcmp(item.first.c_str(), name) == 0)
    {
        if (item.second.find(value) != std::string::npos)
        {
            *result = true;
        }
    }
}

std::string HttpResponsePraser::getResponseHeader(const char *name)
{
    for (size_t i = 0; i < responseHeaders.size(); i++)
    {
        if (strcmp(responseHeaders[i].first.c_str(), name) == 0)
        {
            return responseHeaders[i].second;
        }
    }
    return NULL;
}

void HttpResponsePraser::check_transfer_encoding_chunked(bool *chunked, const Headers::value_type &item)
{
    check_header_field(chunked, item, "transfer-encoding", "chunked");
}

bool HttpResponsePraser::get_chunked_response() const
{
    return chunked_response_;
}

void HttpResponsePraser::set_recv_body_length(int64 length)
{
    recv_body_length = length;
}

int64 HttpResponsePraser::get_recv_body_length() const
{
    return recv_body_length;
}
