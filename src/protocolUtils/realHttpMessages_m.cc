//
// Generated file, do not edit! Created by opp_msgc 4.3 from src/protocolUtils/realHttpMessages.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "realHttpMessages_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(RealHttpRequestMessage);

RealHttpRequestMessage::RealHttpRequestMessage(const char *name, int kind) : HttpRequestMessage(name,kind)
{
    this->accept_var = "";
    this->acceptCharset_var = "";
    this->acceptEncoding_var = "";
    this->acceptLanguage_var = "";
    this->host_var = "";
    this->ifModifiedSince_var = "";
    this->ifNoneMatch_var = "";
    this->referer_var = "";
    this->userAgent_var = "";
}

RealHttpRequestMessage::RealHttpRequestMessage(const RealHttpRequestMessage& other) : HttpRequestMessage(other)
{
    copy(other);
}

RealHttpRequestMessage::~RealHttpRequestMessage()
{
}

RealHttpRequestMessage& RealHttpRequestMessage::operator=(const RealHttpRequestMessage& other)
{
    if (this==&other) return *this;
    HttpRequestMessage::operator=(other);
    copy(other);
    return *this;
}

void RealHttpRequestMessage::copy(const RealHttpRequestMessage& other)
{
    this->accept_var = other.accept_var;
    this->acceptCharset_var = other.acceptCharset_var;
    this->acceptEncoding_var = other.acceptEncoding_var;
    this->acceptLanguage_var = other.acceptLanguage_var;
    this->host_var = other.host_var;
    this->ifModifiedSince_var = other.ifModifiedSince_var;
    this->ifNoneMatch_var = other.ifNoneMatch_var;
    this->referer_var = other.referer_var;
    this->userAgent_var = other.userAgent_var;
}

void RealHttpRequestMessage::parsimPack(cCommBuffer *b)
{
    HttpRequestMessage::parsimPack(b);
    doPacking(b,this->accept_var);
    doPacking(b,this->acceptCharset_var);
    doPacking(b,this->acceptEncoding_var);
    doPacking(b,this->acceptLanguage_var);
    doPacking(b,this->host_var);
    doPacking(b,this->ifModifiedSince_var);
    doPacking(b,this->ifNoneMatch_var);
    doPacking(b,this->referer_var);
    doPacking(b,this->userAgent_var);
}

void RealHttpRequestMessage::parsimUnpack(cCommBuffer *b)
{
    HttpRequestMessage::parsimUnpack(b);
    doUnpacking(b,this->accept_var);
    doUnpacking(b,this->acceptCharset_var);
    doUnpacking(b,this->acceptEncoding_var);
    doUnpacking(b,this->acceptLanguage_var);
    doUnpacking(b,this->host_var);
    doUnpacking(b,this->ifModifiedSince_var);
    doUnpacking(b,this->ifNoneMatch_var);
    doUnpacking(b,this->referer_var);
    doUnpacking(b,this->userAgent_var);
}

const char * RealHttpRequestMessage::accept() const
{
    return accept_var.c_str();
}

void RealHttpRequestMessage::setAccept(const char * accept)
{
    this->accept_var = accept;
}

const char * RealHttpRequestMessage::acceptCharset() const
{
    return acceptCharset_var.c_str();
}

void RealHttpRequestMessage::setAcceptCharset(const char * acceptCharset)
{
    this->acceptCharset_var = acceptCharset;
}

const char * RealHttpRequestMessage::acceptEncoding() const
{
    return acceptEncoding_var.c_str();
}

void RealHttpRequestMessage::setAcceptEncoding(const char * acceptEncoding)
{
    this->acceptEncoding_var = acceptEncoding;
}

const char * RealHttpRequestMessage::acceptLanguage() const
{
    return acceptLanguage_var.c_str();
}

void RealHttpRequestMessage::setAcceptLanguage(const char * acceptLanguage)
{
    this->acceptLanguage_var = acceptLanguage;
}

const char * RealHttpRequestMessage::host() const
{
    return host_var.c_str();
}

void RealHttpRequestMessage::setHost(const char * host)
{
    this->host_var = host;
}

const char * RealHttpRequestMessage::ifModifiedSince() const
{
    return ifModifiedSince_var.c_str();
}

void RealHttpRequestMessage::setIfModifiedSince(const char * ifModifiedSince)
{
    this->ifModifiedSince_var = ifModifiedSince;
}

const char * RealHttpRequestMessage::ifNoneMatch() const
{
    return ifNoneMatch_var.c_str();
}

void RealHttpRequestMessage::setIfNoneMatch(const char * ifNoneMatch)
{
    this->ifNoneMatch_var = ifNoneMatch;
}

const char * RealHttpRequestMessage::referer() const
{
    return referer_var.c_str();
}

void RealHttpRequestMessage::setReferer(const char * referer)
{
    this->referer_var = referer;
}

const char * RealHttpRequestMessage::userAgent() const
{
    return userAgent_var.c_str();
}

void RealHttpRequestMessage::setUserAgent(const char * userAgent)
{
    this->userAgent_var = userAgent;
}

class RealHttpRequestMessageDescriptor : public cClassDescriptor
{
  public:
    RealHttpRequestMessageDescriptor();
    virtual ~RealHttpRequestMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(RealHttpRequestMessageDescriptor);

RealHttpRequestMessageDescriptor::RealHttpRequestMessageDescriptor() : cClassDescriptor("RealHttpRequestMessage", "HttpRequestMessage")
{
}

RealHttpRequestMessageDescriptor::~RealHttpRequestMessageDescriptor()
{
}

bool RealHttpRequestMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<RealHttpRequestMessage *>(obj)!=NULL;
}

const char *RealHttpRequestMessageDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"omitGetVerb")) return "true";
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int RealHttpRequestMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount(object) : 9;
}

unsigned int RealHttpRequestMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<9) ? fieldTypeFlags[field] : 0;
}

const char *RealHttpRequestMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "accept",
        "acceptCharset",
        "acceptEncoding",
        "acceptLanguage",
        "host",
        "ifModifiedSince",
        "ifNoneMatch",
        "referer",
        "userAgent",
    };
    return (field>=0 && field<9) ? fieldNames[field] : NULL;
}

int RealHttpRequestMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='a' && strcmp(fieldName, "accept")==0) return base+0;
    if (fieldName[0]=='a' && strcmp(fieldName, "acceptCharset")==0) return base+1;
    if (fieldName[0]=='a' && strcmp(fieldName, "acceptEncoding")==0) return base+2;
    if (fieldName[0]=='a' && strcmp(fieldName, "acceptLanguage")==0) return base+3;
    if (fieldName[0]=='h' && strcmp(fieldName, "host")==0) return base+4;
    if (fieldName[0]=='i' && strcmp(fieldName, "ifModifiedSince")==0) return base+5;
    if (fieldName[0]=='i' && strcmp(fieldName, "ifNoneMatch")==0) return base+6;
    if (fieldName[0]=='r' && strcmp(fieldName, "referer")==0) return base+7;
    if (fieldName[0]=='u' && strcmp(fieldName, "userAgent")==0) return base+8;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *RealHttpRequestMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
    };
    return (field>=0 && field<9) ? fieldTypeStrings[field] : NULL;
}

const char *RealHttpRequestMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int RealHttpRequestMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    RealHttpRequestMessage *pp = (RealHttpRequestMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string RealHttpRequestMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    RealHttpRequestMessage *pp = (RealHttpRequestMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->accept());
        case 1: return oppstring2string(pp->acceptCharset());
        case 2: return oppstring2string(pp->acceptEncoding());
        case 3: return oppstring2string(pp->acceptLanguage());
        case 4: return oppstring2string(pp->host());
        case 5: return oppstring2string(pp->ifModifiedSince());
        case 6: return oppstring2string(pp->ifNoneMatch());
        case 7: return oppstring2string(pp->referer());
        case 8: return oppstring2string(pp->userAgent());
        default: return "";
    }
}

bool RealHttpRequestMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    RealHttpRequestMessage *pp = (RealHttpRequestMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setAccept((value)); return true;
        case 1: pp->setAcceptCharset((value)); return true;
        case 2: pp->setAcceptEncoding((value)); return true;
        case 3: pp->setAcceptLanguage((value)); return true;
        case 4: pp->setHost((value)); return true;
        case 5: pp->setIfModifiedSince((value)); return true;
        case 6: pp->setIfNoneMatch((value)); return true;
        case 7: pp->setReferer((value)); return true;
        case 8: pp->setUserAgent((value)); return true;
        default: return false;
    }
}

const char *RealHttpRequestMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<9) ? fieldStructNames[field] : NULL;
}

void *RealHttpRequestMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    RealHttpRequestMessage *pp = (RealHttpRequestMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(RealHttpReplyMessage);

RealHttpReplyMessage::RealHttpReplyMessage(const char *name, int kind) : HttpReplyMessage(name,kind)
{
    this->requestURI_var = "";
    this->acceptRanges_var = "none";
    this->age_var = 0;
    this->cacheControl_var = "";
    this->contentEncoding_var = "";
    this->contentLanguage_var = "";
    this->contentLength_var = 0;
    this->contentLocation_var = "";
    this->date_var = "";
    this->etag_var = "";
    this->expires_var = "";
    this->lastModified_var = "";
    this->location_var = "";
    this->pragma_var = "";
    this->server_var = "";
    this->transferEncoding_var = "";
    this->vary_var = "";
    this->via_var = "";
    this->xPoweredBy_var = "";
}

RealHttpReplyMessage::RealHttpReplyMessage(const RealHttpReplyMessage& other) : HttpReplyMessage(other)
{
    copy(other);
}

RealHttpReplyMessage::~RealHttpReplyMessage()
{
}

RealHttpReplyMessage& RealHttpReplyMessage::operator=(const RealHttpReplyMessage& other)
{
    if (this==&other) return *this;
    HttpReplyMessage::operator=(other);
    copy(other);
    return *this;
}

void RealHttpReplyMessage::copy(const RealHttpReplyMessage& other)
{
    this->requestURI_var = other.requestURI_var;
    this->acceptRanges_var = other.acceptRanges_var;
    this->age_var = other.age_var;
    this->cacheControl_var = other.cacheControl_var;
    this->contentEncoding_var = other.contentEncoding_var;
    this->contentLanguage_var = other.contentLanguage_var;
    this->contentLength_var = other.contentLength_var;
    this->contentLocation_var = other.contentLocation_var;
    this->date_var = other.date_var;
    this->etag_var = other.etag_var;
    this->expires_var = other.expires_var;
    this->lastModified_var = other.lastModified_var;
    this->location_var = other.location_var;
    this->pragma_var = other.pragma_var;
    this->server_var = other.server_var;
    this->transferEncoding_var = other.transferEncoding_var;
    this->vary_var = other.vary_var;
    this->via_var = other.via_var;
    this->xPoweredBy_var = other.xPoweredBy_var;
}

void RealHttpReplyMessage::parsimPack(cCommBuffer *b)
{
    HttpReplyMessage::parsimPack(b);
    doPacking(b,this->requestURI_var);
    doPacking(b,this->acceptRanges_var);
    doPacking(b,this->age_var);
    doPacking(b,this->cacheControl_var);
    doPacking(b,this->contentEncoding_var);
    doPacking(b,this->contentLanguage_var);
    doPacking(b,this->contentLength_var);
    doPacking(b,this->contentLocation_var);
    doPacking(b,this->date_var);
    doPacking(b,this->etag_var);
    doPacking(b,this->expires_var);
    doPacking(b,this->lastModified_var);
    doPacking(b,this->location_var);
    doPacking(b,this->pragma_var);
    doPacking(b,this->server_var);
    doPacking(b,this->transferEncoding_var);
    doPacking(b,this->vary_var);
    doPacking(b,this->via_var);
    doPacking(b,this->xPoweredBy_var);
}

void RealHttpReplyMessage::parsimUnpack(cCommBuffer *b)
{
    HttpReplyMessage::parsimUnpack(b);
    doUnpacking(b,this->requestURI_var);
    doUnpacking(b,this->acceptRanges_var);
    doUnpacking(b,this->age_var);
    doUnpacking(b,this->cacheControl_var);
    doUnpacking(b,this->contentEncoding_var);
    doUnpacking(b,this->contentLanguage_var);
    doUnpacking(b,this->contentLength_var);
    doUnpacking(b,this->contentLocation_var);
    doUnpacking(b,this->date_var);
    doUnpacking(b,this->etag_var);
    doUnpacking(b,this->expires_var);
    doUnpacking(b,this->lastModified_var);
    doUnpacking(b,this->location_var);
    doUnpacking(b,this->pragma_var);
    doUnpacking(b,this->server_var);
    doUnpacking(b,this->transferEncoding_var);
    doUnpacking(b,this->vary_var);
    doUnpacking(b,this->via_var);
    doUnpacking(b,this->xPoweredBy_var);
}

const char * RealHttpReplyMessage::requestURI() const
{
    return requestURI_var.c_str();
}

void RealHttpReplyMessage::setRequestURI(const char * requestURI)
{
    this->requestURI_var = requestURI;
}

const char * RealHttpReplyMessage::acceptRanges() const
{
    return acceptRanges_var.c_str();
}

void RealHttpReplyMessage::setAcceptRanges(const char * acceptRanges)
{
    this->acceptRanges_var = acceptRanges;
}

int RealHttpReplyMessage::age() const
{
    return age_var;
}

void RealHttpReplyMessage::setAge(int age)
{
    this->age_var = age;
}

const char * RealHttpReplyMessage::cacheControl() const
{
    return cacheControl_var.c_str();
}

void RealHttpReplyMessage::setCacheControl(const char * cacheControl)
{
    this->cacheControl_var = cacheControl;
}

const char * RealHttpReplyMessage::contentEncoding() const
{
    return contentEncoding_var.c_str();
}

void RealHttpReplyMessage::setContentEncoding(const char * contentEncoding)
{
    this->contentEncoding_var = contentEncoding;
}

const char * RealHttpReplyMessage::contentLanguage() const
{
    return contentLanguage_var.c_str();
}

void RealHttpReplyMessage::setContentLanguage(const char * contentLanguage)
{
    this->contentLanguage_var = contentLanguage;
}

int RealHttpReplyMessage::contentLength() const
{
    return contentLength_var;
}

void RealHttpReplyMessage::setContentLength(int contentLength)
{
    this->contentLength_var = contentLength;
}

const char * RealHttpReplyMessage::contentLocation() const
{
    return contentLocation_var.c_str();
}

void RealHttpReplyMessage::setContentLocation(const char * contentLocation)
{
    this->contentLocation_var = contentLocation;
}

const char * RealHttpReplyMessage::date() const
{
    return date_var.c_str();
}

void RealHttpReplyMessage::setDate(const char * date)
{
    this->date_var = date;
}

const char * RealHttpReplyMessage::etag() const
{
    return etag_var.c_str();
}

void RealHttpReplyMessage::setEtag(const char * etag)
{
    this->etag_var = etag;
}

const char * RealHttpReplyMessage::expires() const
{
    return expires_var.c_str();
}

void RealHttpReplyMessage::setExpires(const char * expires)
{
    this->expires_var = expires;
}

const char * RealHttpReplyMessage::lastModified() const
{
    return lastModified_var.c_str();
}

void RealHttpReplyMessage::setLastModified(const char * lastModified)
{
    this->lastModified_var = lastModified;
}

const char * RealHttpReplyMessage::location() const
{
    return location_var.c_str();
}

void RealHttpReplyMessage::setLocation(const char * location)
{
    this->location_var = location;
}

const char * RealHttpReplyMessage::pragma() const
{
    return pragma_var.c_str();
}

void RealHttpReplyMessage::setPragma(const char * pragma)
{
    this->pragma_var = pragma;
}

const char * RealHttpReplyMessage::server() const
{
    return server_var.c_str();
}

void RealHttpReplyMessage::setServer(const char * server)
{
    this->server_var = server;
}

const char * RealHttpReplyMessage::transferEncoding() const
{
    return transferEncoding_var.c_str();
}

void RealHttpReplyMessage::setTransferEncoding(const char * transferEncoding)
{
    this->transferEncoding_var = transferEncoding;
}

const char * RealHttpReplyMessage::vary() const
{
    return vary_var.c_str();
}

void RealHttpReplyMessage::setVary(const char * vary)
{
    this->vary_var = vary;
}

const char * RealHttpReplyMessage::via() const
{
    return via_var.c_str();
}

void RealHttpReplyMessage::setVia(const char * via)
{
    this->via_var = via;
}

const char * RealHttpReplyMessage::xPoweredBy() const
{
    return xPoweredBy_var.c_str();
}

void RealHttpReplyMessage::setXPoweredBy(const char * xPoweredBy)
{
    this->xPoweredBy_var = xPoweredBy;
}

class RealHttpReplyMessageDescriptor : public cClassDescriptor
{
  public:
    RealHttpReplyMessageDescriptor();
    virtual ~RealHttpReplyMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(RealHttpReplyMessageDescriptor);

RealHttpReplyMessageDescriptor::RealHttpReplyMessageDescriptor() : cClassDescriptor("RealHttpReplyMessage", "HttpReplyMessage")
{
}

RealHttpReplyMessageDescriptor::~RealHttpReplyMessageDescriptor()
{
}

bool RealHttpReplyMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<RealHttpReplyMessage *>(obj)!=NULL;
}

const char *RealHttpReplyMessageDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"omitGetVerb")) return "true";
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int RealHttpReplyMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 19+basedesc->getFieldCount(object) : 19;
}

unsigned int RealHttpReplyMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<19) ? fieldTypeFlags[field] : 0;
}

const char *RealHttpReplyMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "requestURI",
        "acceptRanges",
        "age",
        "cacheControl",
        "contentEncoding",
        "contentLanguage",
        "contentLength",
        "contentLocation",
        "date",
        "etag",
        "expires",
        "lastModified",
        "location",
        "pragma",
        "server",
        "transferEncoding",
        "vary",
        "via",
        "xPoweredBy",
    };
    return (field>=0 && field<19) ? fieldNames[field] : NULL;
}

int RealHttpReplyMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestURI")==0) return base+0;
    if (fieldName[0]=='a' && strcmp(fieldName, "acceptRanges")==0) return base+1;
    if (fieldName[0]=='a' && strcmp(fieldName, "age")==0) return base+2;
    if (fieldName[0]=='c' && strcmp(fieldName, "cacheControl")==0) return base+3;
    if (fieldName[0]=='c' && strcmp(fieldName, "contentEncoding")==0) return base+4;
    if (fieldName[0]=='c' && strcmp(fieldName, "contentLanguage")==0) return base+5;
    if (fieldName[0]=='c' && strcmp(fieldName, "contentLength")==0) return base+6;
    if (fieldName[0]=='c' && strcmp(fieldName, "contentLocation")==0) return base+7;
    if (fieldName[0]=='d' && strcmp(fieldName, "date")==0) return base+8;
    if (fieldName[0]=='e' && strcmp(fieldName, "etag")==0) return base+9;
    if (fieldName[0]=='e' && strcmp(fieldName, "expires")==0) return base+10;
    if (fieldName[0]=='l' && strcmp(fieldName, "lastModified")==0) return base+11;
    if (fieldName[0]=='l' && strcmp(fieldName, "location")==0) return base+12;
    if (fieldName[0]=='p' && strcmp(fieldName, "pragma")==0) return base+13;
    if (fieldName[0]=='s' && strcmp(fieldName, "server")==0) return base+14;
    if (fieldName[0]=='t' && strcmp(fieldName, "transferEncoding")==0) return base+15;
    if (fieldName[0]=='v' && strcmp(fieldName, "vary")==0) return base+16;
    if (fieldName[0]=='v' && strcmp(fieldName, "via")==0) return base+17;
    if (fieldName[0]=='x' && strcmp(fieldName, "xPoweredBy")==0) return base+18;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *RealHttpReplyMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "string",
        "int",
        "string",
        "string",
        "string",
        "int",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
    };
    return (field>=0 && field<19) ? fieldTypeStrings[field] : NULL;
}

const char *RealHttpReplyMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int RealHttpReplyMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    RealHttpReplyMessage *pp = (RealHttpReplyMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string RealHttpReplyMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    RealHttpReplyMessage *pp = (RealHttpReplyMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->requestURI());
        case 1: return oppstring2string(pp->acceptRanges());
        case 2: return long2string(pp->age());
        case 3: return oppstring2string(pp->cacheControl());
        case 4: return oppstring2string(pp->contentEncoding());
        case 5: return oppstring2string(pp->contentLanguage());
        case 6: return long2string(pp->contentLength());
        case 7: return oppstring2string(pp->contentLocation());
        case 8: return oppstring2string(pp->date());
        case 9: return oppstring2string(pp->etag());
        case 10: return oppstring2string(pp->expires());
        case 11: return oppstring2string(pp->lastModified());
        case 12: return oppstring2string(pp->location());
        case 13: return oppstring2string(pp->pragma());
        case 14: return oppstring2string(pp->server());
        case 15: return oppstring2string(pp->transferEncoding());
        case 16: return oppstring2string(pp->vary());
        case 17: return oppstring2string(pp->via());
        case 18: return oppstring2string(pp->xPoweredBy());
        default: return "";
    }
}

bool RealHttpReplyMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    RealHttpReplyMessage *pp = (RealHttpReplyMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setRequestURI((value)); return true;
        case 1: pp->setAcceptRanges((value)); return true;
        case 2: pp->setAge(string2long(value)); return true;
        case 3: pp->setCacheControl((value)); return true;
        case 4: pp->setContentEncoding((value)); return true;
        case 5: pp->setContentLanguage((value)); return true;
        case 6: pp->setContentLength(string2long(value)); return true;
        case 7: pp->setContentLocation((value)); return true;
        case 8: pp->setDate((value)); return true;
        case 9: pp->setEtag((value)); return true;
        case 10: pp->setExpires((value)); return true;
        case 11: pp->setLastModified((value)); return true;
        case 12: pp->setLocation((value)); return true;
        case 13: pp->setPragma((value)); return true;
        case 14: pp->setServer((value)); return true;
        case 15: pp->setTransferEncoding((value)); return true;
        case 16: pp->setVary((value)); return true;
        case 17: pp->setVia((value)); return true;
        case 18: pp->setXPoweredBy((value)); return true;
        default: return false;
    }
}

const char *RealHttpReplyMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<19) ? fieldStructNames[field] : NULL;
}

void *RealHttpReplyMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    RealHttpReplyMessage *pp = (RealHttpReplyMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


