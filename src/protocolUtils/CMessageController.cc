/*
 * CMessageController.cc
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#include "CMessageController.h"
#include "ByteArrayMessage.h"
#include "NSCHttpServer.h"
#include "HarParser.h"

CMessageController::CMessageController(CHeaderEncoderBase *vEncoder, CHeaderCompressorBase *vCompressor, MsgInfoSrcBase *vMsgInfoSrc, HttpNodeBase *vHttpNode)
{
    pEncoder = vEncoder;
    pCompressor = vCompressor;
    pMsgInfoSrc = vMsgInfoSrc;
    pHttpNode = vHttpNode;

    assignSockID = 0;
}

CMessageController::~CMessageController()
{
    // TODO Auto-generated destructor stub
    if (NULL != pEncoder)
    {
        delete pEncoder;
        pEncoder = NULL;
    }

    if (NULL != pCompressor)
    {
        delete pCompressor;
        pCompressor = NULL;
    }

    if (NULL != pMsgInfoSrc)
    {
        delete pMsgInfoSrc;
        pMsgInfoSrc = NULL;
    }

    if (NULL != pHttpNode)
    {
        delete pHttpNode;
        pHttpNode = NULL;
    }
}

/**
 * Format an application TCP_TRANSFER_BYTESTREAM request message which can be sent though NSC TCP depending on the application layer protocol
 */
std::string CMessageController::generateRequestMessage(HttpRequestMessage *httpRequest, Socket_ID_Type sockID)
{
    /**
     * 1. get request-URI from request message
     */
    std::string requestURI = getRequestUri(httpRequest);

    /**
     * 2. change message from httpRequest to realHttpRequest
     */
    RealHttpRequestMessage *realHttpRequest = changeRequestToReal(httpRequest);

    /**
     * 3. get header frame from the setting message information source
     */
    HeaderFrame requestFrame = pMsgInfoSrc->getReqHeaderFrame(realHttpRequest, pHttpNode);

    /**
     * 4. get the encoded message header from the setting message encoder
     */
    std::string reqHeader = pEncoder->generateReqMsgHeader(requestFrame, requestURI);

    /**
     * 5. compress the encoded message header
     */
    std::string reqMsg = pCompressor->compressMsgHeader(reqHeader, sockID, pHttpNode, strlen(realHttpRequest->payload()));

    /**
     * 6. add request message body
     */
    reqMsg.append(realHttpRequest->payload());


    delete realHttpRequest;
    realHttpRequest = NULL;

    return reqMsg;
}

/**
 * Format an application TCP_TRANSFER_BYTESTREAM response message which can be sent though NSC TCP depending on the application layer protocol
 */
std::string CMessageController::generateResponseMessage(HttpReplyMessage *httpResponse, Socket_ID_Type sockID)
{
    /**
     * 0. change message from httpRequest to realHttpRequest
     */
//    RealHttpReplyMessage *realhttpResponse = changeReplyToReal(httpResponse);
    /*
     * Note: this is a RealHttpReplyMessage * pointer indeed
     * just use dynamic_cast[check_and_cast] to change it
     */
    RealHttpReplyMessage *realhttpResponse = check_and_cast<RealHttpReplyMessage*>(httpResponse);

    /**
     * 1. get header frame from the setting message information source
     */
    HeaderFrame responseFrame = pMsgInfoSrc->getResHeaderFrame(realhttpResponse, pHttpNode);

    /**
     * 2. adjust the statistics and the realhttpResponse
     */
    if (responseFrame.size() != 0)
    {
        // Find the response, modify the statistics-----step 1, set this not badRequest
        dynamic_cast<NSCHttpServer*>(pHttpNode)->decBadRequests();
    }

    for (unsigned int i = 0; i < responseFrame.size(); ++i)
    {
        // reset content-length
        if (responseFrame[i].key.find("Content-Length") != string::npos)
        {
            realhttpResponse->setContentLength(std::atoi(responseFrame[i].val.c_str()));
            EV_DEBUG_NOMODULE << "set realhttpResponse ContentLength: "<< responseFrame[i].val << endl;
            EV_DEBUG_NOMODULE << "realhttpResponse's ContentLength is: "<< realhttpResponse->contentLength() << endl;
        }
        // modify the statistics-----step 2, set this served resource depend on the content-type
        /**
         * in RFC1521, the content-type definition is :
         *    In the Augmented BNF notation of RFC 822, a Content-Type header field value is defined as follows:
         content  :=   "Content-Type"  ":"  type  "/"  subtype  *(";" parameter)
                     ; case-insensitive matching of type and subtype
                     type :=          "application"     / "audio"
                     / "image"           / "message"
                     / "multipart"  / "text"
                     / "video"           / extension-token
                     ; All values case-insensitive
                     extension-token :=  x-token / iana-token
         */
        else if (responseFrame[i].key.find("Content-Type") != string::npos)
        {
            if (responseFrame[i].val.find("image") != string::npos)
            {
                dynamic_cast<NSCHttpServer*>(pHttpNode)->incImgResourcesServed();
            }
            else if (responseFrame[i].val.find("text") != string::npos
                    || responseFrame[i].val.find("javascript") != string::npos
                    || responseFrame[i].val.find("xml") != string::npos)
            {
                dynamic_cast<NSCHttpServer*>(pHttpNode)->incTextResourcesServed();
            }
            //----Note: since javascript and xml type has application token, should do text statistic before media
            else if (responseFrame[i].val.find("application") != string::npos
                    || responseFrame[i].val.find("audio") != string::npos
                    || responseFrame[i].val.find("video") != string::npos)
            {
                dynamic_cast<NSCHttpServer*>(pHttpNode)->incMediaResourcesServed();
            }
            else
            {
                dynamic_cast<NSCHttpServer*>(pHttpNode)->incOtherResourcesServed();
            }
        }
    }

    /**
     * 3. get the encoded message header from the setting message encoder
     */
    std::string resHeader = pEncoder->generateResMsgHeader(responseFrame);

    /**
     * 4. compress the encoded message header
     */
    std::string resMsg = pCompressor->compressMsgHeader(resHeader, sockID, pHttpNode, strlen(realhttpResponse->payload()));

    EV_DEBUG_NOMODULE << "Response Header length is: "<< resMsg.size() << endl;

    /**
     * 5. add request message body
     */
    EV_DEBUG_NOMODULE << "Generate HTTP Response Body:"<< realhttpResponse->payload()<< endl;
    resMsg.append(realhttpResponse->payload());
    EV_DEBUG_NOMODULE << "Payload Length is" <<(int64_t)strlen(realhttpResponse->payload()) << ". " << endl;
    if (realhttpResponse->contentLength() > (int64_t)strlen(realhttpResponse->payload()))
    {
        resMsg.append((realhttpResponse->contentLength() - (int64_t)strlen(realhttpResponse->payload())), '\n');
        EV_DEBUG_NOMODULE << "Add NULL char. contentLength is" << realhttpResponse->contentLength() << ", payload Length is" <<(int64_t)strlen(realhttpResponse->payload()) << ". " << endl;
    }

    /*************************************Finish generating HTTP Response Body*************************************/

    delete realhttpResponse;
    realhttpResponse = NULL;

    return resMsg;
}

/** Parse a received byte message to a RealHttpRequestMessage */
RealHttpRequestMessage *CMessageController::parseRequestMessage(cPacket *msg, Socket_ID_Type sockID)
{
    /**
     * 1. get application layer message content
     */
    ByteArrayMessage *byMsg = check_and_cast<ByteArrayMessage*>(msg);
    size_t bufLength = byMsg->getByteLength();
    char *buf = new char[bufLength];
    byMsg->getByteArray().copyDataToBuffer(buf, bufLength, 0);
//    buf[bufLength] = '\0';

    EV_DEBUG_NOMODULE << "HTTP Request to decompress ByteLength is:" << bufLength << endl;
    EV_DEBUG_NOMODULE << "HTTP Request to decompress ByteArray is:" << buf << endl;

    /**
     * 2. decompress the message header
     */
    std::string deMsg = pCompressor->decompressMsgHeader(const_cast<const char*>(buf), bufLength, sockID);

    /**
     * 3. parse the message header
     */
    EV_DEBUG_NOMODULE << "HTTP Request to parse ByteLength is:" << deMsg.size() << endl;
    EV_DEBUG_NOMODULE << "HTTP Request to parse ByteArray is:" << deMsg << endl;
    RealHttpRequestMessage *httpRequest = pEncoder->parseReqMsg(deMsg.c_str(), deMsg.size(), msg, sockID);

    delete[] buf;
    delete msg;
    return httpRequest;
}

/** Parse a received byte message to a RealHttpReplyMessage */
RealHttpReplyMessage *CMessageController::praseResponseMessage(cPacket *msg, Socket_ID_Type sockID)
{
    /**
     * 1. get application layer message content
     */
    ByteArrayMessage *byMsg = check_and_cast<ByteArrayMessage*>(msg);
    size_t bufLength = byMsg->getByteLength();
    char *buf = new char[bufLength];
    byMsg->getByteArray().copyDataToBuffer(buf, bufLength, 0);
//    buf[bufLength] = '\0';

    EV_DEBUG_NOMODULE << "HTTP Response to decompress ByteLength is:" << bufLength << endl;
    EV_DEBUG_NOMODULE << "HTTP Response to decompress ByteArray is:" << buf << endl;

    /**
     * 2. decompress the message header
     */
    std::string deMsg = pCompressor->decompressMsgHeader(const_cast<const char*>(buf), bufLength, sockID);

    /**
     * 3. parse the message header
     */
    EV_DEBUG_NOMODULE << "HTTP Response to parse ByteLength is:" << deMsg.size() << endl;
    EV_DEBUG_NOMODULE << "HTTP Response to parse ByteArray is:" << deMsg << endl;
    RealHttpReplyMessage* httpResponse = pEncoder->parseResMsg(deMsg.c_str(), deMsg.size(), msg, sockID);

    delete[] buf;
    delete msg;
    return httpResponse;
}

/** try to deal with the rest bytes */
RealHttpReplyMessage *CMessageController::dealWithRestBytes(Socket_ID_Type sockID)
{
    return pEncoder->dealWithRestBytes(sockID);
}

Socket_ID_Type CMessageController::getNewSockID()
{
    assignSockID++;

    pEncoder->initParsers(assignSockID);

    pCompressor->initCompressors(assignSockID);

    return assignSockID;
}

void CMessageController::releaseSockID(Socket_ID_Type sockID)
{
    pEncoder->releaseParsers(sockID);

    pCompressor->releaseCompressors(sockID);
}

HeaderFrame CMessageController::getTimingFromHar(string requestURI)
{
    return dynamic_cast<HarParser*>(pMsgInfoSrc)->getTiming(requestURI);
}

HeaderFrame CMessageController::getResponseFromHar(string requestURI)
{
    return dynamic_cast<HarParser*>(pMsgInfoSrc)->getResponse(requestURI);
}

RealHttpRequestMessage *CMessageController::changeRequestToReal(HttpRequestMessage *httpRequest)
{
    RealHttpRequestMessage *realHttpRequest;
    realHttpRequest = new RealHttpRequestMessage();

    realHttpRequest->setAccept("");
    realHttpRequest->setAcceptCharset("");
    realHttpRequest->setAcceptEncoding("");
    realHttpRequest->setAcceptLanguage("");
    realHttpRequest->setHost("");
    realHttpRequest->setIfModifiedSince("");
    realHttpRequest->setIfNoneMatch("");
    realHttpRequest->setReferer("");
    realHttpRequest->setUserAgent("");
    realHttpRequest->setTargetUrl(httpRequest->targetUrl());
    realHttpRequest->setProtocol(httpRequest->protocol());
    realHttpRequest->setHeading(httpRequest->heading());
    realHttpRequest->setSerial(httpRequest->serial());
    realHttpRequest->setByteLength(httpRequest->getByteLength());
    realHttpRequest->setKeepAlive(httpRequest->keepAlive());
    realHttpRequest->setBadRequest(httpRequest->badRequest());
    realHttpRequest->setKind(httpRequest->getKind());
    realHttpRequest->setPayload(httpRequest->payload());

    delete httpRequest;
    httpRequest = NULL;

    return realHttpRequest;
}

RealHttpReplyMessage *CMessageController::changeReplyToReal(HttpReplyMessage *httpResponse)
{
    RealHttpReplyMessage *realhttpResponse;
    realhttpResponse = new RealHttpReplyMessage();

    realhttpResponse->setRequestURI("");
    realhttpResponse->setAcceptRanges("none");
    realhttpResponse->setAge(0);
    realhttpResponse->setCacheControl("");
    realhttpResponse->setContentEncoding("");
    realhttpResponse->setContentLanguage("");
    realhttpResponse->setContentLocation("");
    realhttpResponse->setDate("");
    realhttpResponse->setEtag("");
    realhttpResponse->setExpires("");
    realhttpResponse->setLastModified("");
    realhttpResponse->setLocation("");
    realhttpResponse->setPragma("");
    realhttpResponse->setServer("");
    realhttpResponse->setTransferEncoding("");
    realhttpResponse->setVary("");
    realhttpResponse->setVia("");
    realhttpResponse->setXPoweredBy("");

    realhttpResponse->setHeading(httpResponse->heading());
    realhttpResponse->setOriginatorUrl(httpResponse->originatorUrl());
    realhttpResponse->setTargetUrl(httpResponse->targetUrl());
    realhttpResponse->setProtocol(httpResponse->protocol());
    realhttpResponse->setSerial(httpResponse->serial());
    realhttpResponse->setResult(httpResponse->result());
    realhttpResponse->setContentType(httpResponse->contentType()); // Emulates the content-type header field
    realhttpResponse->setKind(httpResponse->getKind());
    realhttpResponse->setPayload(httpResponse->payload());
    realhttpResponse->setKeepAlive(httpResponse->keepAlive());
    realhttpResponse->setContentLength(std::max(httpResponse->getByteLength(), (int64_t)strlen(httpResponse->payload())));

    delete httpResponse;
    httpResponse = NULL;

    return realhttpResponse;
}

/**
 * get request-URI from request string of requestMessage
 */
std::string CMessageController::getRequestUri(HttpRequestMessage *httpRequest)
{
    // Parse the request string on spaces
    cStringTokenizer tokenizer = cStringTokenizer(httpRequest->heading(), " ");
    std::vector < std::string > res = tokenizer.asVector();
    if (res.size() != 3)
    {
        EV_ERROR_NOMODULE << "Invalid request string: " << httpRequest->heading() << endl;
    }

    return res[1];
}
