//
// Generated file, do not edit! Created by opp_msgc 4.2 from src/protocolUtils/realHttpMessages.msg.
//

#ifndef _REALHTTPMESSAGES_M_H_
#define _REALHTTPMESSAGES_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "HttpMessages_m.h"
// }}



/**
 * Class generated from <tt>src/protocolUtils/realHttpMessages.msg</tt> by opp_msgc.
 * <pre>
 * packet RealHttpRequestMessage extends HttpRequestMessage
 * {
 *     @omitGetVerb(true);         
 *     string accept = "";
 * 	string acceptCharset = "";
 * 	string acceptEncoding = "";
 * 	string acceptLanguage = "";
 * 	string host = "";
 * 	string ifModifiedSince = "";
 * 	string ifNoneMatch = "";
 * 	string referer = "";
 * 	string userAgent = "";
 * }
 * </pre>
 */
class RealHttpRequestMessage : public ::HttpRequestMessage
{
  protected:
    opp_string accept_var;
    opp_string acceptCharset_var;
    opp_string acceptEncoding_var;
    opp_string acceptLanguage_var;
    opp_string host_var;
    opp_string ifModifiedSince_var;
    opp_string ifNoneMatch_var;
    opp_string referer_var;
    opp_string userAgent_var;

  private:
    void copy(const RealHttpRequestMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const RealHttpRequestMessage&);

  public:
    RealHttpRequestMessage(const char *name=NULL, int kind=0);
    RealHttpRequestMessage(const RealHttpRequestMessage& other);
    virtual ~RealHttpRequestMessage();
    RealHttpRequestMessage& operator=(const RealHttpRequestMessage& other);
    virtual RealHttpRequestMessage *dup() const {return new RealHttpRequestMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * accept() const;
    virtual void setAccept(const char * accept);
    virtual const char * acceptCharset() const;
    virtual void setAcceptCharset(const char * acceptCharset);
    virtual const char * acceptEncoding() const;
    virtual void setAcceptEncoding(const char * acceptEncoding);
    virtual const char * acceptLanguage() const;
    virtual void setAcceptLanguage(const char * acceptLanguage);
    virtual const char * host() const;
    virtual void setHost(const char * host);
    virtual const char * ifModifiedSince() const;
    virtual void setIfModifiedSince(const char * ifModifiedSince);
    virtual const char * ifNoneMatch() const;
    virtual void setIfNoneMatch(const char * ifNoneMatch);
    virtual const char * referer() const;
    virtual void setReferer(const char * referer);
    virtual const char * userAgent() const;
    virtual void setUserAgent(const char * userAgent);
};

inline void doPacking(cCommBuffer *b, RealHttpRequestMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, RealHttpRequestMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>src/protocolUtils/realHttpMessages.msg</tt> by opp_msgc.
 * <pre>
 * packet RealHttpReplyMessage extends HttpReplyMessage
 * {
 *     @omitGetVerb(true);
 *     string requestURI = "";
 *     string acceptRanges = "none";
 * 	int age = 0;
 * 	string cacheControl = "";
 * 	string contentEncoding = "";
 * 	string contentLanguage = "";
 * 	int contentLength = 0;
 * 	string contentLocation = "";
 * 	string date = "";
 * 	string etag = "";
 * 	string expires = "";
 * 	string lastModified = "";
 * 	string location = "";
 * 	string pragma = "";
 * 	string server = "";
 * 	string transferEncoding = "";
 * 	string vary = "";
 * 	string via = "";
 * 	string xPoweredBy = "";
 * }
 * </pre>
 */
class RealHttpReplyMessage : public ::HttpReplyMessage
{
  protected:
    opp_string requestURI_var;
    opp_string acceptRanges_var;
    int age_var;
    opp_string cacheControl_var;
    opp_string contentEncoding_var;
    opp_string contentLanguage_var;
    int contentLength_var;
    opp_string contentLocation_var;
    opp_string date_var;
    opp_string etag_var;
    opp_string expires_var;
    opp_string lastModified_var;
    opp_string location_var;
    opp_string pragma_var;
    opp_string server_var;
    opp_string transferEncoding_var;
    opp_string vary_var;
    opp_string via_var;
    opp_string xPoweredBy_var;

  private:
    void copy(const RealHttpReplyMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const RealHttpReplyMessage&);

  public:
    RealHttpReplyMessage(const char *name=NULL, int kind=0);
    RealHttpReplyMessage(const RealHttpReplyMessage& other);
    virtual ~RealHttpReplyMessage();
    RealHttpReplyMessage& operator=(const RealHttpReplyMessage& other);
    virtual RealHttpReplyMessage *dup() const {return new RealHttpReplyMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * requestURI() const;
    virtual void setRequestURI(const char * requestURI);
    virtual const char * acceptRanges() const;
    virtual void setAcceptRanges(const char * acceptRanges);
    virtual int age() const;
    virtual void setAge(int age);
    virtual const char * cacheControl() const;
    virtual void setCacheControl(const char * cacheControl);
    virtual const char * contentEncoding() const;
    virtual void setContentEncoding(const char * contentEncoding);
    virtual const char * contentLanguage() const;
    virtual void setContentLanguage(const char * contentLanguage);
    virtual int contentLength() const;
    virtual void setContentLength(int contentLength);
    virtual const char * contentLocation() const;
    virtual void setContentLocation(const char * contentLocation);
    virtual const char * date() const;
    virtual void setDate(const char * date);
    virtual const char * etag() const;
    virtual void setEtag(const char * etag);
    virtual const char * expires() const;
    virtual void setExpires(const char * expires);
    virtual const char * lastModified() const;
    virtual void setLastModified(const char * lastModified);
    virtual const char * location() const;
    virtual void setLocation(const char * location);
    virtual const char * pragma() const;
    virtual void setPragma(const char * pragma);
    virtual const char * server() const;
    virtual void setServer(const char * server);
    virtual const char * transferEncoding() const;
    virtual void setTransferEncoding(const char * transferEncoding);
    virtual const char * vary() const;
    virtual void setVary(const char * vary);
    virtual const char * via() const;
    virtual void setVia(const char * via);
    virtual const char * xPoweredBy() const;
    virtual void setXPoweredBy(const char * xPoweredBy);
};

inline void doPacking(cCommBuffer *b, RealHttpReplyMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, RealHttpReplyMessage& obj) {obj.parsimUnpack(b);}


#endif // _REALHTTPMESSAGES_M_H_
