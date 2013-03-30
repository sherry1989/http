/*
 * CHttpParser.h
 *
 *  Created on: Mar 28, 2013
 *      Author: qian
 */

#ifndef CHTTPPARSER_H_
#define CHTTPPARSER_H_

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

typedef std::vector<std::pair<std::string, std::string> > Headers;

class CHttpParser
{
    public:
        CHttpParser();
        virtual ~CHttpParser();

    public:
        /** @name methods for request message parsing */
        //@{
        void add_message_header(const std::string& name, const std::string& value);

        void set_last_message_header_value(const std::string& value);

        bool get_message_header_key_prev() const;

        void append_last_message_header_key(const char *data, size_t len);

        void append_last_message_header_value(const char *data, size_t len);

        void set_message_major(int major);

        void set_message_minor(int minor);

        int get_message_major() const;

        int get_message_minor() const;

        void set_message_state(int state);

        int get_message_state() const;

        const Headers& get_message_headers() const;

        void check_header_field(bool *result, const Headers::value_type &item,
                                const char *name, const char *value);

        void check_transfer_encoding_chunked(bool *chunked,
                                             const Headers::value_type &item);

        bool get_chunked_request() const;

        std::string get_message_header(const char *name);

        void set_recv_body_length(int64 length);

        int64 get_recv_body_length() const;

        enum {
          INITIAL,
          HEADER_COMPLETE,
          MSG_COMPLETE,
          STREAM_CLOSED,
          CONNECT_FAIL,
          IDLE
        };

    protected:

        int message_state_;
        int message_major_;
        int message_minor_;
        bool chunked_message_;
        Headers message_headers;
        bool message_header_key_prev_;

        // This buffer is used to temporarily store downstream request
        // body. Spdylay reads data from this in the callback.
//        evbuffer *message_body_buf_;
        int32_t recv_window_size_;
        int64 recv_body_length;
        //@}
};

class HttpRequestParser : public CHttpParser
{
    public:
        HttpRequestParser();
        virtual ~HttpRequestParser();

        RealHttpRequestMessage* parseReqMsg(const char *data, size_t len, cPacket *msg);

        RealHttpRequestMessage* httpRequest;

    private:
        http_parser *pRequestParser;
};

class HttpResponseParser : public CHttpParser
{
    public:
        HttpResponseParser();
        virtual ~HttpResponseParser();

        RealHttpReplyMessage* parseResMsg(char *buf, size_t bufLength, cPacket *msg);

        RealHttpReplyMessage* httpResponse;

    private:
        http_parser *pResponseParser;

        static bool nextHeaderSticked;
        static std::string nextHeader;
};

namespace RequestParser
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

#endif /* CHTTPPARSER_H_ */
