/*
 * HttpRequestPraser.h
 *
 *  Created on: Oct 18, 2012
 *      Author: qian
 */

#ifndef HTTPREQUESTRASER_H_
#define HTTPREQUESTRASER_H_

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

class HttpRequestPraser
{
    public:
        HttpRequestPraser();
        virtual ~HttpRequestPraser();

        RealHttpRequestMessage* praseHttpRequest(cPacket *msg, Protocol_Type protocolType, spdylay_zlib *inflater);

        void spdyParser(const char *data,size_t len, spdylay_zlib *inflater);

        void add_request_header(const std::string& name, const std::string& value);

        void set_last_request_header_value(const std::string& value);

        bool get_request_header_key_prev() const;

        void append_last_request_header_key(const char *data, size_t len);

        void append_last_request_header_value(const char *data, size_t len);

        void set_request_major(int major);

        void set_request_minor(int minor);

        int get_request_major() const;

        int get_request_minor() const;

        void set_request_state(int state);

        int get_request_state() const;

        const Headers& get_request_headers() const;

        void check_header_field(bool *result, const Headers::value_type &item,
                                const char *name, const char *value);

        void check_transfer_encoding_chunked(bool *chunked,
                                             const Headers::value_type &item);

        bool get_chunked_request() const;

        std::string getRequestHeader(const char *name);

        RealHttpRequestMessage* httpRequest;

        enum {
          INITIAL,
          HEADER_COMPLETE,
          MSG_COMPLETE,
          STREAM_CLOSED,
          CONNECT_FAIL,
          IDLE
        };

    private:

        int request_state_;
        int request_major_;
        int request_minor_;
        bool chunked_request_;
        Headers requestHeaders;
        bool request_header_key_prev_;
        http_parser *pRequestParser;
        // This buffer is used to temporarily store downstream request
        // body. Spdylay reads data from this in the callback.
//        evbuffer *request_body_buf_;
        int32_t recv_window_size_;
};

namespace RequestPraser
{
    #define MAX_URL_LENGTH 2048

    /*
     * Callbacks for http message prasing
     */
    int request_url_cb (http_parser *htp, const char *buf, size_t len);

    int header_field_cb(http_parser *htp, const char *buf, size_t len);

    int header_value_cb(http_parser *htp, const char *buf, size_t len);

    int body_cb(http_parser *htp, const char *buf, size_t len);

    int message_begin_cb(http_parser *htp);

    int headers_complete_cb(http_parser *htp);

    int message_complete_cb(http_parser *htp);
}

#endif /* HTTPREQUESTRASER_H_ */
