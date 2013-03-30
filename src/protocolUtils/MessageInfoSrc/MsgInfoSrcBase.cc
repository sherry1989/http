/*
 * MsgInfoSrcBase.cc
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#include "MsgInfoSrcBase.h"

Define_Module(MsgInfoSrcBase);

MsgInfoSrcBase::MsgInfoSrcBase()
{
    // TODO Auto-generated constructor stub
    
}

MsgInfoSrcBase::~MsgInfoSrcBase()
{
    // TODO Auto-generated destructor stub
}

void MsgInfoSrcBase::initialize(int stage)
{
    // TODO - Generated method body
    if (stage == 0)
    {
        headerEncodeType = par("headerEncodeType");
    }
}

void MsgInfoSrcBase::finish()
{
}

void MsgInfoSrcBase::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

/**
 *
 * @param httpRequest
 * @return
 */
HeaderFrame MsgInfoSrcBase::getReqHeaderFrame(const RealHttpRequestMessage *httpRequest, HttpNodeBase *pHttpNode)
{
    HeaderFrame requestFrame;

    /*************************************Generate HTTP Request Header*************************************/

    /**
     * 1.Generate Request-Line:
     * Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
     */

    /** 1.1 Method SP */
    requestFrame.push_back(makeKVPair(":method", "GET"));

    /** 1.2 Request-URI SP */
    //not record it in the headerFrame

    /** 1.3 HTTP-Version CRLF */
    switch (httpRequest->protocol())
    {
        case 10:
            requestFrame.push_back(makeKVPair(":version", "1.0"));
            break;
        case 11:
            requestFrame.push_back(makeKVPair(":version", "1.1"));
            break;
        default:
            throw cRuntimeError("Invalid HTTP Status code: %d", httpRequest->protocol());
            break;
    }

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
     request/Request chain.
     * Cache-Control   = "Cache-Control" ":" 1#cache-directive
     cache-directive = cache-request-directive
     | cache-Request-directive
     cache-request-directive = "no-cache"
     | "no-store"
     | "max-age" "=" delta-seconds
     | "max-stale" [ "=" delta-seconds ]
     | "min-fresh" "=" delta-seconds
     | "no-transform"
     | "only-if-cached"
     | cache-extension
     cache-Request-directive =  "public"
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

    /** 2.2 Connection */
    /*
     * HTTP/1.1 applications that do not support persistent connections MUST
     include the "close" connection option in every message.
     * Connection = "Connection" ":" 1#(connection-token)
     connection-token  = token
     */
    /*
     * the Connection headers are not valid and MUST not be send when use SPDY formed fream and use the zlib dictionary
     */
    if (httpRequest->keepAlive())
    {
        requestFrame.push_back(makeKVPair("Connection", "Keep-Alive"));
    }
    else
    {
        requestFrame.push_back(makeKVPair("Connection", "close"));
    }

    /** 2.3 Date */
    /*
     * The Date general-header field represents the date and time at which
     the message was originated
     * Date  = "Date" ":" HTTP-date
     */

    /** 2.4 Pragma */
    /*
     * The Pragma general-header field is used to include implementation-
     specific directives that might apply to any recipient along the
     request/Request chain. All pragma directives specify optional
     behavior from the viewpoint of the protocol; however, some systems
     MAY require that behavior be consistent with the directives.
     * Pragma            = "Pragma" ":" 1#pragma-directive
     pragma-directive  = "no-cache" | extension-pragma
     extension-pragma  = token [ "=" ( token | quoted-string ) ]
     */

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
     Request to indicate which protocol(s) are being switched.
     * Upgrade        = "Upgrade" ":" 1#product
     */

    /** 2.8 Via */
    /*
     * The Via general-header field MUST be used by gateways and proxies to
     indicate the intermediate protocols and recipients between the user
     agent and the server on requests, and between the origin server and
     the client on Requests. It is analogous to the "Received" field of
     RFC 822 [9] and is intended to be used for tracking message forwards,
     avoiding request loops, and identifying the protocol capabilities of
     all senders along the request/Request chain.
     * Via =  "Via" ":" 1#( received-protocol received-by [ comment ] )
     received-protocol = [ protocol-name "/" ] protocol-version
     protocol-name     = token
     protocol-version  = token
     received-by       = ( host [ ":" port ] ) | pseudonym
     pseudonym         = token
     */

    /** 2.9 Warning */
    /*
     * The Warning general-header field is used to carry additional
     information about the status or transformation of a message which
     might not be reflected in the message. This information is typically
     used to warn about a possible lack of semantic transparency from
     caching operations or transformations applied to the entity body of
     the message.
     * Warning headers are sent with Requests using:
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
     * 3. Generate Request-header:
     * request-header = Accept
     | Accept-Charset
     | Accept-Encoding
     | Accept-Language
     | Authorization
     | Expect
     | From
     | Host
     | If-Match
     | If-Modified-Since
     | If-None-Match
     | If-Range
     | If-Unmodified-Since
     | Max-Forwards
     | Proxy-Authorization
     | Range
     | Referer
     | TE
     | User-Agent
     */

    /** 3.1 Accept */
    /*
     * The Accept request-header field can be used to specify certain media
     types which are acceptable for the response. Accept headers can be
     used to indicate that the request is specifically limited to a small
     set of desired types, as in the case of a request for an in-line
     image.
     * Accept         = "Accept" ":"
     #( media-range [ accept-params ] )
     media-range    = ( "*"/"*"
     | ( type "/" "*" )
     | ( type "/" subtype )
     ) *( ";" parameter )
     accept-params  = ";" "q" "=" qvalue *( accept-extension )
     accept-extension = ";" token [ "=" ( token | quoted-string ) ]
     */
    if (strcmp(httpRequest->accept(), "") != 0)
    {
        requestFrame.push_back(makeKVPair("Accept", httpRequest->accept()));
    }
    else
    {
        requestFrame.push_back(makeKVPair("Accept", "text/plain; q=0.5, text/html,text/x-dvi; q=0.8, text/x-c"));
    }

    /** 3.2 Accept-Charset */
    /*
     * The Accept-Charset request-header field can be used to indicate what
     character sets are acceptable for the response. This field allows
     clients capable of understanding more comprehensive or special-
     purpose character sets to signal that capability to a server which is
     capable of representing documents in those character sets.
     * Accept-Charset = "Accept-Charset" ":"
     1#( ( charset | "*" )[ ";" "q" "=" qvalue ] )
     */
    if (strcmp(httpRequest->acceptCharset(), "") != 0)
    {
        requestFrame.push_back(makeKVPair("Accept-Charset", httpRequest->acceptCharset()));
    }
    else
    {
        //if the Accept-Charset not set, don't send it
    }

    /** 3.3 Accept-Encoding */
    /*
     * The Accept-Encoding request-header field is similar to Accept, but
     restricts the content-codings (section 3.5) that are acceptable in
     the response.
     * Accept-Encoding  = "Accept-Encoding" ":"
     1#( codings [ ";" "q" "=" qvalue ] )
     codings          = ( content-coding | "*" )
     */
    if (strcmp(httpRequest->acceptEncoding(), "") != 0)
    {
        requestFrame.push_back(makeKVPair("Accept-Encoding", httpRequest->acceptEncoding()));
    }
    else
    {
        requestFrame.push_back(makeKVPair("Accept-Encoding", "gzip, deflate"));
    }

    /** 3.4 Accept-Language */
    /*
     * The Accept-Language request-header field is similar to Accept, but
     restricts the set of natural languages that are preferred as a
     response to the request. Language tags are defined in section 3.10.
     * Accept-Language = "Accept-Language" ":"
     1#( language-range [ ";" "q" "=" qvalue ] )
     language-range  = ( ( 1*8ALPHA *( "-" 1*8ALPHA ) ) | "*" )
     */
    if (strcmp(httpRequest->acceptLanguage(), "") != 0)
    {
        requestFrame.push_back(makeKVPair("Accept-Language", httpRequest->acceptLanguage()));
    }
    else
    {
        requestFrame.push_back(makeKVPair("Accept-Language", "zh-cn,zh;q=0.8,en-us;q-0.5,en;q=0.3"));
    }

    /** 3.5 Authorization */
    /*
     * A user agent that wishes to authenticate itself with a server--
     usually, but not necessarily, after receiving a 401 response--does
     so by including an Authorization request-header field with the
     request.  The Authorization field value consists of credentials
     containing the authentication information of the user agent for
     the realm of the resource being requested.
     * Authorization  = "Authorization" ":" credentials
     */

    /** 3.6 Expect */
    /*
     * The Expect request-header field is used to indicate that particular
     server behaviors are required by the client.
     * Expect       =  "Expect" ":" 1#expectation
     expectation  =  "100-continue" | expectation-extension
     expectation-extension =  token [ "=" ( token | quoted-string )
     *expect-params ]
     expect-params =  ";" token [ "=" ( token | quoted-string ) ]
     */

    /** 3.7 From */
    /*
     * The From request-header field, if given, SHOULD contain an Internet
     e-mail address for the human user who controls the requesting user
     agent. The address SHOULD be machine-usable, as defined by "mailbox"
     in RFC 822 [9] as updated by RFC 1123 [8]:
     * From   = "From" ":" mailbox
     */

    /** 3.8 Host */
    /*
     * The Host request-header field specifies the Internet host and port
     number of the resource being requested, as obtained from the original
     URI given by the user or referring resource (generally an HTTP URL,
     as described in section 3.2.2). The Host field value MUST represent
     the naming authority of the origin server or gateway given by the
     original URL. This allows the origin server or gateway to
     differentiate between internally-ambiguous URLs, such as the root "/"
     URL of a server for multiple host names on a single IP address.
     * Host = "Host" ":" host [ ":" port ]
     */
    if (strcmp(httpRequest->host(), "") != 0)
    {
        requestFrame.push_back(makeKVPair("Host", httpRequest->host()));
        EV_DEBUG << "host not null Set Host: " << httpRequest->host() << "\r\n";
    }
    else
    {
        requestFrame.push_back(makeKVPair("Host", httpRequest->targetUrl()));
        EV_DEBUG << "Set Host: " << httpRequest->targetUrl() << "\r\n";
    }

    /** 3.9 If-Match */
    /*
     * The If-Match request-header field is used with a method to make it
     conditional. A client that has one or more entities previously
     obtained from the resource can verify that one of those entities is
     current by including a list of their associated entity tags in the
     If-Match header field. Entity tags are defined in section 3.11. The
     purpose of this feature is to allow efficient updates of cached
     information with a minimum amount of transaction overhead. It is also
     used, on updating requests, to prevent inadvertent modification of
     the wrong version of a resource. As a special case, the value "*"
     matches any current entity of the resource.
     * If-Match = "If-Match" ":" ( "*" | 1#entity-tag )
     */

    /** 3.10 If-Modified-Since */
    /*
     * The If-Modified-Since request-header field is used with a method to
     make it conditional: if the requested variant has not been modified
     since the time specified in this field, an entity will not be
     returned from the server; instead, a 304 (not modified) response will
     be returned without any message-body.
     * If-Modified-Since = "If-Modified-Since" ":" HTTP-date
     */
    if (strcmp(httpRequest->ifModifiedSince(), "") != 0)
    {
        requestFrame.push_back(makeKVPair("If-Modified-Since", httpRequest->ifModifiedSince()));
    }
    else
    {
        //if the If-Modified-Since not set, don't send it
    }

    /** 3.11 If-None-Match */
    /*
     * The If-None-Match request-header field is used with a method to make
     it conditional. A client that has one or more entities previously
     obtained from the resource can verify that none of those entities is
     current by including a list of their associated entity tags in the
     If-None-Match header field. The purpose of this feature is to allow
     efficient updates of cached information with a minimum amount of
     transaction overhead. It is also used to prevent a method (e.g. PUT)
     from inadvertently modifying an existing resource when the client
     believes that the resource does not exist.
     * As a special case, the value "*" matches any current entity of the
     resource.
     * If-None-Match = "If-None-Match" ":" ( "*" | 1#entity-tag )
     */
    if (strcmp(httpRequest->ifNoneMatch(), "") != 0)
    {
        requestFrame.push_back(makeKVPair("If-None-Match", httpRequest->ifNoneMatch()));
    }
    else
    {
        //if the If-None-Match not set, don't send it
    }

    /** 3.12 If-Range */
    /*
     * If a client has a partial copy of an entity in its cache, and wishes
     to have an up-to-date copy of the entire entity in its cache, it
     could use the Range request-header with a conditional GET (using
     either or both of If-Unmodified-Since and If-Match.) However, if the
     condition fails because the entity has been modified, the client
     would then have to make a second request to obtain the entire current
     entity-body.
     * The If-Range header allows a client to "short-circuit" the second
     request. Informally, its meaning is `if the entity is unchanged, send
     me the part(s) that I am missing; otherwise, send me the entire new
     entity'.
     *  If-Range = "If-Range" ":" ( entity-tag | HTTP-date )
     */

    /** 3.13 If-Unmodified-Since */
    /*
     * The If-Unmodified-Since request-header field is used with a method to
     make it conditional. If the requested resource has not been modified
     since the time specified in this field, the server SHOULD perform the
     requested operation as if the If-Unmodified-Since header were not
     present.
     * If the requested variant has been modified since the specified time,
     the server MUST NOT perform the requested operation, and MUST return
     a 412 (Precondition Failed).
     * If-Unmodified-Since = "If-Unmodified-Since" ":" HTTP-date
     */

    /** 3.14 Max-Forwards */
    /*
     * The Max-Forwards request-header field provides a mechanism with the
     TRACE (section 9.8) and OPTIONS (section 9.2) methods to limit the
     number of proxies or gateways that can forward the request to the
     next inbound server. This can be useful when the client is attempting
     to trace a request chain which appears to be failing or looping in
     mid-chain.
     * Max-Forwards   = "Max-Forwards" ":" 1*DIGIT
     */

    /** 3.15 Proxy-Authorization */
    /*
     * The Proxy-Authenticate response-header field MUST be included as part
     of a 407 (Proxy Authentication Required) response. The field value
     consists of a challenge that indicates the authentication scheme and
     parameters applicable to the proxy for this Request-URI.
     * Proxy-Authenticate  = "Proxy-Authenticate" ":" 1#challenge
     */

    /** 3.16 Range */

    /** 3.17 Referer */
    /*
     * The Referer[sic] request-header field allows the client to specify,
     for the server's benefit, the address (URI) of the resource from
     which the Request-URI was obtained (the "referrer", although the
     header field is misspelled.) The Referer request-header allows a
     server to generate lists of back-links to resources for interest,
     logging, optimized caching, etc. It also allows obsolete or mistyped
     links to be traced for maintenance. The Referer field MUST NOT be
     sent if the Request-URI was obtained from a source that does not have
     its own URI, such as input from the user keyboard.
     * Referer        = "Referer" ":" ( absoluteURI | relativeURI )
     */
    if (strcmp(httpRequest->referer(), "") != 0)
    {
        requestFrame.push_back(makeKVPair("Referer", httpRequest->referer()));
    }
    else
    {
        requestFrame.push_back(makeKVPair("Referer", httpRequest->targetUrl()));
    }

    /** 3.18 TE */
    /*
     * The TE request-header field indicates what extension transfer-codings
     it is willing to accept in the response and whether or not it is
     willing to accept trailer fields in a chunked transfer-coding. Its
     value may consist of the keyword "trailers" and/or a comma-separated
     list of extension transfer-coding names with optional accept
     parameters (as described in section 3.6).
     * TE        = "TE" ":" #( t-codings )
     t-codings = "trailers" | ( transfer-extension [ accept-params ] )
     */

    /** 3.19 User-Agent */
    /*
     * The User-Agent request-header field contains information about the
     user agent originating the request. This is for statistical purposes,
     the tracing of protocol violations, and automated recognition of user
     agents for the sake of tailoring responses to avoid particular user
     agent limitations. User agents SHOULD include this field with
     requests. The field can contain multiple product tokens (section 3.8)
     and comments identifying the agent and any subproducts which form a
     significant part of the user agent. By convention, the product tokens
     are listed in order of their significance for identifying the
     application.
     * User-Agent     = "User-Agent" ":" 1*( product | comment )
     */
    if (strcmp(httpRequest->userAgent(), "") != 0)
    {
        requestFrame.push_back(makeKVPair("User-Agent", httpRequest->userAgent()));
    }
    else
    {
        requestFrame.push_back(
                KVPair("User-Agent", "Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:16.0) Gecko/20100101 Firefox/16.0"));
    }

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
     Request MUST include a Content-Encoding entity-header (section
     14.11) that lists the non-identity content-coding(s) used.
     * Content-Encoding  = "Content-Encoding" ":" 1#content-coding
     */

    /** 4.3 Content-Language */
    /*
     * The Content-Language entity-header field describes the natural
     language(s) of the intended audience for the enclosed entity.
     * Content-Language  = "Content-Language" ":" 1#language-tag
     */

    /** 4.4 Content-Length */
    /*
     * The Content-Length entity-header field indicates the size of the
     entity-body, in decimal number of OCTETs, sent to the recipient or,
     in the case of the HEAD method, the size of the entity-body that
     would have been sent had the request been a GET.
     * Content-Length    = "Content-Length" ":" 1*DIGIT
     */

    /** 4.5 Content-Location */
    /*
     * The Content-Location entity-header field MAY be used to supply the
     resource location for the entity enclosed in the message when that
     entity is accessible from a location separate from the requested
     resource's URI.
     * Content-Location = "Content-Location" ":" ( absoluteURI | relativeURI )
     */

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

    /** 4.9 Expires */
    /*
     * The Expires entity-header field gives the date/time after which the
     Request is considered stale.
     * The format is an absolute date and time as defined by HTTP-date in
     section 3.3.1; it MUST be in RFC 1123 date format:
     * Expires = "Expires" ":" HTTP-date
     */

    /** 4.10 Last-Modified */
    /*
     * The Last-Modified entity-header field indicates the date and time at
     which the origin server believes the variant was last modified.
     * Last-Modified  = "Last-Modified" ":" HTTP-date
     */

    /*************************************Finish generating HTTP Request Header*************************************/

    EV_DEBUG << "Self Generate HTTP Request Header Frame is: " << endl;

    outputHeaderFrame(requestFrame);

    return requestFrame;
}

HeaderFrame MsgInfoSrcBase::getResHeaderFrame(const RealHttpReplyMessage *httpResponse, HttpNodeBase *pHttpNode)
{
    HeaderFrame responseFrame;

    /*************************************Generate HTTP Response Header*************************************/

    /**
     * 1.Generate Status-Line:
     * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
     */

    /** 1.1 HTTP-Version SP */
    switch (httpResponse->protocol())
    {
        case 10:
            responseFrame.push_back(makeKVPair(":version", "1.0"));
            break;
        case 11:
            responseFrame.push_back(makeKVPair(":version", "1.1"));
            break;
        default:
            throw cRuntimeError("Invalid HTTP Status code: %d", httpResponse->protocol());
            break;
    }

    /** 1.2 Status-Code SP Reason-Phrase CRLF */
    std::string statusText = get_status_string(httpResponse->result());
    responseFrame.push_back(makeKVPair(":status", statusText.substr(0, 3).c_str()));
    responseFrame.push_back(makeKVPair(":status-text", statusText.substr(4, std::string::npos).c_str()));

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
    if (strcmp(httpResponse->cacheControl(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Cache-Control", httpResponse->cacheControl()));
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
    /*
     * the Connection headers are not valid and MUST not be send when use SPDY formed fream and use the spdy zlib dictionary
     * NOTE TODO may need to process when use spdy3 compression
     */
    if (headerEncodeType == HTTP_ASCII)
    {
        if (httpResponse->keepAlive())
        {
            responseFrame.push_back(makeKVPair("Connection", "Keep-Alive"));
        }
        else
        {
            responseFrame.push_back(makeKVPair("Connection", "close"));
        }
    }

    /** 2.3 Date */
    /*
     * The Date general-header field represents the date and time at which
     the message was originated
     * Date  = "Date" ":" HTTP-date
     */
    if (strcmp(httpResponse->date(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Date", httpResponse->date()));
    }
    else
    {
        responseFrame.push_back(makeKVPair("Date", "Tue, 16 Oct 2012 05:35:24 GMT"));
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
    if (strcmp(httpResponse->pragma(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Pragma", httpResponse->pragma()));
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
    if (strcmp(httpResponse->via(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Via", httpResponse->via()));
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
    responseFrame.push_back(makeKVPair("Accept-Ranges", httpResponse->acceptRanges()));

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
    std::ostringstream age;
    age << httpResponse->age();
    responseFrame.push_back(makeKVPair("Age", age.str()));

    /** 3.3 Etag */
    /*
     * The ETag response-header field provides the current value of the
     entity tag for the requested variant.
     * The entity tag MAY be used for comparison with other entities from the same resource.
     * ETag = "ETag" ":" entity-tag
     */
    if (strcmp(httpResponse->etag(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Etag", httpResponse->etag()));
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
    if (strcmp(httpResponse->location(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Location", httpResponse->location()));
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
    if (strcmp(httpResponse->server(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Server", httpResponse->server()));
    }
    else
    {
        responseFrame.push_back(makeKVPair("Server", "Apache/2.2.20 (Ubuntu)"));
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
    if (strcmp(httpResponse->vary(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Vary", httpResponse->vary()));
    }
    else
    {
        responseFrame.push_back(makeKVPair("Vary", "Accept-Encoding"));
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
    if (strcmp(httpResponse->contentEncoding(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Content-Encoding", httpResponse->contentEncoding()));
    }
    else
    {
        responseFrame.push_back(makeKVPair("Content-Encoding", "gzip"));
    }

    /** 4.3 Content-Language */
    /*
     * The Content-Language entity-header field describes the natural
     language(s) of the intended audience for the enclosed entity.
     * Content-Language  = "Content-Language" ":" 1#language-tag
     */
    if (strcmp(httpResponse->contentLanguage(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Content-Language", httpResponse->contentLanguage()));
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
    //TODO somewhere need to set contentLength
    std::ostringstream contentLength;
    contentLength << httpResponse->contentLength();
    responseFrame.push_back(makeKVPair("Content-Length", contentLength.str()));
    EV_DEBUG << "Generate HTTP Content-Length: " << httpResponse->contentLength() << endl;

    /** 4.5 Content-Location */
    /*
     * The Content-Location entity-header field MAY be used to supply the
     resource location for the entity enclosed in the message when that
     entity is accessible from a location separate from the requested
     resource's URI.
     * Content-Location = "Content-Location" ":" ( absoluteURI | relativeURI )
     */
    if (strcmp(httpResponse->contentLocation(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Content-Location", httpResponse->contentLocation()));
    }
    else
    {
        //if the Content-Location not set, use the originatorUrl instead
        responseFrame.push_back(makeKVPair("Content-Location", httpResponse->originatorUrl()));
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
    if (httpResponse->result() == 200)
    {
        switch (httpResponse->contentType())
        {
            case CT_HTML:
                responseFrame.push_back(makeKVPair("Content-Type", "text/html"));
                //TODO textResourcesServed++;
                break;
            case CT_TEXT:
                responseFrame.push_back(makeKVPair("Content-Type", "application/javascript"));
                //TODO textResourcesServed++;
                break;
            case CT_IMAGE:
                responseFrame.push_back(makeKVPair("Content-Type", "image/jpeg"));
                //TODO imgResourcesServed++;
                break;
            default:
                throw cRuntimeError("Invalid HTTP Content Type: %d", httpResponse->contentType());
                break;
        }
    }

    /** 4.9 Expires */
    /*
     * The Expires entity-header field gives the date/time after which the
     response is considered stale.
     * The format is an absolute date and time as defined by HTTP-date in
     section 3.3.1; it MUST be in RFC 1123 date format:
     * Expires = "Expires" ":" HTTP-date
     */
    if (strcmp(httpResponse->expires(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Expires", httpResponse->expires()));
    }
    else
    {
        responseFrame.push_back(makeKVPair("Expires", "Wed, 16 Oct 2013 05:35:24 GMT"));
    }

    /** 4.10 Last-Modified */
    /*
     * The Last-Modified entity-header field indicates the date and time at
     which the origin server believes the variant was last modified.
     * Last-Modified  = "Last-Modified" ":" HTTP-date
     */
    if (strcmp(httpResponse->lastModified(), "") != 0)
    {
        responseFrame.push_back(makeKVPair("Last-Modified", httpResponse->lastModified()));
    }
    else
    {
        responseFrame.push_back(makeKVPair("Last-Modified", "Tue, 16 Oct 2012 05:35:24 GMT"));
    }

    /*************************************Finish generating HTTP Response Header*************************************/

    EV_DEBUG << "Self Generate HTTP Response Header Frame is: " << endl;

    outputHeaderFrame(responseFrame);

    return responseFrame;
}

void MsgInfoSrcBase::outputHeaderFrame(const HeaderFrame& hf)
{
    for (HeaderFrame::const_iterator i = hf.begin(); i != hf.end(); ++i)
    {
        auto line = *i;
        const string& k = line.key;
        const string& v = line.val;
        EV_DEBUG << k << ": " << v << "\n";
    }
}

KVPair MsgInfoSrcBase::makeKVPair(std::string name, const std::string value)
{
    // depend on the header encode type to set the right key-value
    switch (headerEncodeType)
    {
        case HTTP_ASCII:
            return KVPair(name, value);
        /**
         * SPDY_HEADER_BLOCK must change HTTP Header Field Names to only use Lowercase.
         * By only using lowercase, more redundancy and therefore greater compression are expected.
         */
        case SPDY_HEADER_BLOCK:
            std::transform(name.begin(), name.end(), name.begin(), (int(*)(int))std::tolower);
            return KVPair(name, value);
        default:
            throw cRuntimeError("Invalid header encode type: %d", headerEncodeType);
    }
}
