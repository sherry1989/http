/*
 * HttpRequestPraser.cc
 *
 *  Created on: Oct 18, 2012
 *      Author: qian
 */

#include "HttpRequestPraser.h"
#include "HttpNodeBase.h"
#include "ByteArrayMessage.h"

#include <cassert>

namespace RequestPraser
{
    static http_parser_settings parserCallback=
    {
        message_begin_cb,           //http_cb      on_message_begin;
        request_url_cb,             //http_data_cb on_url;   //request only
        header_field_cb,            //http_data_cb on_header_field;
        header_value_cb,            //http_data_cb on_header_value;
        headers_complete_cb,        //http_cb      on_headers_complete;
        body_cb,                    //http_data_cb on_body;
        message_complete_cb         //http_cb      on_message_complete;
    };

    int request_url_cb (http_parser *htp, const char *buf, size_t len)
    {
        HttpRequestPraser *parser;
        parser = reinterpret_cast<HttpRequestPraser*>(htp->data);

        /*
         * set Request-Line of this http request message
         */
        char szReq[MAX_URL_LENGTH+24];
        sprintf(szReq, "GET %.*s HTTP/1.1", len, buf);
        parser->httpRequest->setHeading(szReq);

        return 0;
    }

    int header_field_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpRequestPraser *parser;
        parser = reinterpret_cast<HttpRequestPraser*>(htp->data);

        if (parser->get_request_header_key_prev())
        {
            parser->append_last_request_header_key(buf, len);
        }
        else
        {
            parser->add_request_header(std::string(buf, len), "");
        }

        return 0;
    }

    int header_value_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpRequestPraser *parser;
        parser = reinterpret_cast<HttpRequestPraser*>(htp->data);

        if (parser->get_request_header_key_prev())
        {
            parser->set_last_request_header_value(std::string(buf, len));
        }
        else
        {
            parser->append_last_request_header_value(buf, len);
        }

        return 0;
    }

    int body_cb(http_parser *htp, const char *buf, size_t len)
    {
        HttpRequestPraser *parser;
        parser = reinterpret_cast<HttpRequestPraser*>(htp->data);

        parser->httpRequest->setPayload(buf);

        return 0;
    }

    int message_begin_cb(http_parser *htp)
    {
        HttpRequestPraser *parser;
        parser = reinterpret_cast<HttpRequestPraser*>(htp->data);

        parser->httpRequest->setKind(HTTPT_REQUEST_MESSAGE);

        parser->httpRequest->setBadRequest(false);
        return 0;
    }

    int headers_complete_cb(http_parser *htp)
    {
        HttpRequestPraser *parser;
        parser = reinterpret_cast<HttpRequestPraser*>(htp->data);

        parser->set_request_major(htp->http_major);
        parser->set_request_minor(htp->http_minor);
        parser->httpRequest->setKeepAlive(http_should_keep_alive(htp));
        parser->set_request_state(parser->HEADER_COMPLETE);
        parser->httpRequest->setProtocol(11);

        /*
         * set targetUrl of this http response message
         */
        std::string targetUrl;
        targetUrl.assign(parser->getRequestHeader("Host"));
        parser->httpRequest->setTargetUrl(targetUrl.c_str());

        return 0;
    }

    int message_complete_cb(http_parser *htp)
    {
    //    HttpRequestPraser *parser;
    //    parser = reinterpret_cast<HttpRequestPraser*>(htp->data);

        return 0;
    }
}


HttpRequestPraser::HttpRequestPraser()
:httpRequest(new RealHttpRequestMessage()),
 request_state_(INITIAL),
 request_major_(1),
 request_minor_(1),
 chunked_request_(false),
 request_header_key_prev_(false),
 pRequestParser(new http_parser()),
 recv_window_size_(0)
{
    // TODO Auto-generated constructor stub
    http_parser_init(pRequestParser, HTTP_REQUEST);
    pRequestParser->data = this;
}

HttpRequestPraser::~HttpRequestPraser()
{
    // TODO Auto-generated destructor stub
    delete pRequestParser;
    pRequestParser = NULL;
}

RealHttpRequestMessage* HttpRequestPraser::praseHttpRequest(cPacket *msg)
{
    ByteArrayMessage *byMsg = check_and_cast<ByteArrayMessage*>(msg);
    size_t bufLength = byMsg->getByteLength();
    char *buf = new char[bufLength];
    byMsg->getByteArray().copyDataToBuffer(buf, bufLength, 0);

    httpRequest->setName(msg->getName());
    httpRequest->setSentFrom(msg->getSenderModule(), msg->getSenderGateId(), msg->getSendingTime());

    /*size_t nread = */http_parser_execute(pRequestParser, &RequestPraser::parserCallback, const_cast<const char*>(buf), bufLength);
//
//    evbuffer_drain(input, nread);
//    http_errno htperr = HTTP_PARSER_ERRNO(request_htp_);
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

    delete msg;
    return httpRequest;
}

void HttpRequestPraser::add_request_header(const std::string& name, const std::string& value)
{
  request_header_key_prev_ = true;
  requestHeaders.push_back(std::make_pair(name, value));
}

void HttpRequestPraser::set_last_request_header_value(const std::string& value)
{
  request_header_key_prev_ = false;
  Headers::value_type &item = requestHeaders.back();
  item.second = value;
  check_transfer_encoding_chunked(&chunked_request_, item);
  //check_connection_close(&request_connection_close_, item);
}

bool HttpRequestPraser::get_request_header_key_prev() const
{
  return request_header_key_prev_;
}

void HttpRequestPraser::append_last_request_header_key(const char *data, size_t len)
{
  assert(request_header_key_prev_);
  Headers::value_type &item = requestHeaders.back();
  item.first.append(data, len);
}

void HttpRequestPraser::append_last_request_header_value(const char *data, size_t len)
{
  assert(!request_header_key_prev_);
  Headers::value_type &item = requestHeaders.back();
  item.second.append(data, len);
}

void HttpRequestPraser::set_request_major(int major)
{
  request_major_ = major;
}

void HttpRequestPraser::set_request_minor(int minor)
{
  request_minor_ = minor;
}

int HttpRequestPraser::get_request_major() const
{
  return request_major_;
}

int HttpRequestPraser::get_request_minor() const
{
  return request_minor_;
}

void HttpRequestPraser::set_request_state(int state)
{
  request_state_ = state;
}

int HttpRequestPraser::get_request_state() const
{
  return request_state_;
}

const Headers& HttpRequestPraser::get_request_headers() const
{
  return requestHeaders;
}

void HttpRequestPraser::check_header_field(bool *result, const Headers::value_type &item, const char *name,
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

std::string HttpRequestPraser::getRequestHeader(const char *name)
{
    for (size_t i = 0; i < requestHeaders.size(); i++)
    {
        if (strcmp(requestHeaders[i].first.c_str(), name) == 0)
        {
            return requestHeaders[i].second;
        }
    }
    return NULL;
}

void HttpRequestPraser::check_transfer_encoding_chunked(bool *chunked, const Headers::value_type &item)
{
    check_header_field(chunked, item, "transfer-encoding", "chunked");
}

bool HttpRequestPraser::get_chunked_request() const
{
    return chunked_request_;
}
