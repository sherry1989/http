/*
 * HttpResponseParser.h
 *
 *  Created on: Oct 17, 2012
 *      Author: qian
 */

#ifndef HTTPRESPONSEPARSER_H_
#define HTTPRESPONSEPARSER_H_

extern "C" {
  #include "http_parser.h"
}

#include "httpProtocol.h"
#include "ProtocolTypeDef.h"

#include <stdint.h>

#include <vector>
#include <string>
//
//#include <event.h>
//#include <event2/bufferevent.h>

#include "spdylay_zlib.h"

typedef std::vector<std::pair<std::string, std::string> > Headers;

class HttpResponseParser
{
    public:
        HttpResponseParser();
        virtual ~HttpResponseParser();

        RealHttpReplyMessage* praseHttpResponse(cPacket *msg, Protocol_Type protocolType, spdylay_zlib *inflater);

        void spdyParser(const char *data,size_t len, spdylay_zlib *inflater);

        void add_response_header(const std::string& name, const std::string& value);

        void set_last_response_header_value(const std::string& value);

        bool get_response_header_key_prev() const;

        void append_last_response_header_key(const char *data, size_t len);

        void append_last_response_header_value(const char *data, size_t len);

        void set_response_major(int major);

        void set_response_minor(int minor);

        int get_response_major() const;

        int get_response_minor() const;

        void set_response_state(int state);

        int get_response_state() const;

        const Headers& get_response_headers() const;

        void check_header_field(bool *result, const Headers::value_type &item,
                                const char *name, const char *value);

        void check_transfer_encoding_chunked(bool *chunked,
                                             const Headers::value_type &item);

        bool get_chunked_response() const;

        std::string getResponseHeader(const char *name);

        void set_recv_body_length(int64 length);

        int64 get_recv_body_length() const;

        RealHttpReplyMessage* httpResponse;

        enum {
          INITIAL,
          HEADER_COMPLETE,
          MSG_COMPLETE,
          STREAM_CLOSED,
          CONNECT_FAIL,
          IDLE
        };

    private:

        int response_state_;
        int response_major_;
        int response_minor_;
        bool chunked_response_;
        Headers responseHeaders;
        bool response_header_key_prev_;
        http_parser *pResponseParser;
        // This buffer is used to temporarily store downstream response
        // body. Spdylay reads data from this in the callback.
//        evbuffer *response_body_buf_;
        int32_t recv_window_size_;
        int64 recv_body_length;

        static bool nextHeaderSticked;
        static std::string nextHeader;
};

namespace ResponseParser
{
    /*
     * Callbacks for http message prasing
     */
    int header_field_cb(http_parser *htp, const char *buf, size_t len);

    int header_value_cb(http_parser *htp, const char *buf, size_t len);

    int body_cb(http_parser *htp, const char *buf, size_t len);

    int message_begin_cb(http_parser *htp);

    int headers_complete_cb(http_parser *htp);

    int message_complete_cb(http_parser *htp);
}


#endif /* HTTPRESPONSEPARSER_H_ */
