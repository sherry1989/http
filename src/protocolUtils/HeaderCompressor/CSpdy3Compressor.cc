/*
 * CSpdy3Compressor.cc
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#include "CSpdy3Compressor.h"
#include "NSCHttpBrowser.h"
#include "NSCHttpServer.h"

#include <stdexcept>
#include <iomanip>
#include <memory.h>
#include <algorithm>

CSpdy3Compressor::CSpdy3Compressor()
{
    // TODO Auto-generated constructor stub
    
}

CSpdy3Compressor::~CSpdy3Compressor()
{
    // TODO Auto-generated destructor stub
}

/** Compress the encoded message header */
std::string CSpdy3Compressor::compressMsgHeader(std::string msgHeader, Socket_ID_Type sockID, HttpNodeBase *pHttpNode, size_t bodyLength)
{
    uint8_t *buf = NULL, *nvbuf = NULL;
    size_t buflen = 0, nvbuflen = 0;

    nvbuf = (uint8_t *) msgHeader.c_str();
    nvbuflen = msgHeader.length();

    spdylay_zlib deflater = zlibInfos[sockID].deflater;

    buflen = spdylay_zlib_deflate_hd_bound(&deflater, nvbuflen);
    buf = new uint8_t[buflen];

    EV_DEBUG_NOMODULE << "header length bound to deflate is: " << buflen << endl;

    ssize_t framelen;
    framelen = spdylay_zlib_deflate_hd(&deflater, buf, buflen, nvbuf, nvbuflen);

    EV_DEBUG_NOMODULE << "header length before deflate is: " << nvbuflen << endl;
    EV_DEBUG_NOMODULE << "header before deflate is: " << nvbuf << endl;
    EV_DEBUG_NOMODULE << "header length after deflate is: " << framelen << endl;
    EV_DEBUG_NOMODULE << "header after deflate is: " << buf << endl;

    if (framelen == SPDYLAY_ERR_ZLIB)
    {
        throw cRuntimeError("spdylay_zlib_deflate_hd get Zlib error!");
    }
    else
    {
        // doing statistics
        std::string httpNodeClassName = pHttpNode->getClassName();

        if (httpNodeClassName.find("NSCHttpBrowser") != std::string::npos)
        {
            dynamic_cast<NSCHttpBrowser*>(pHttpNode)->recordCompressStatistic(framelen, nvbuflen, bodyLength);
        }
        else if (httpNodeClassName.find("NSCHttpServer") != std::string::npos)
        {
            dynamic_cast<NSCHttpServer*>(pHttpNode)->recordCompressStatistic(framelen, nvbuflen, bodyLength);
        }
        else
        {
            throw cRuntimeError("get unrecognized httpNode class name");
        }

        std::ostringstream zlibReqHeader;

        //24 bit header length
        zlibReqHeader << std::setbase(16) << std::setw(3) << framelen;

        char *charBuf = new char[framelen];

        EV_DEBUG_NOMODULE << "header after deflate is as char: ";
        for (ssize_t i = 0; i < framelen; i++)
        {
            charBuf[i] = buf[i] - 128;
            EV << charBuf[i];
        }
        EV << endl;

//        memcpy(charBuf, buf, framelen);

//        EV_DEBUG_NOMODULE << "header after deflate is as char: " << charBuf << endl;

        //zlib deflate header
//        zlibReqHeader << std::setw(framelen) << buf;
//        zlibReqHeader << std::setw(framelen) << charBuf;
//        zlibReqHeader << charBuf;

        std::string zlibReqHeaderStr;
        zlibReqHeaderStr.assign(charBuf, framelen);

        delete[] buf;
        delete[] charBuf;

        zlibReqHeader << zlibReqHeaderStr;

//        EV_DEBUG_NOMODULE << "zlibReqHeader is: " << zlibReqHeader.str() << endl;

        EV_DEBUG_NOMODULE << "zlibReqHeader length is: " << zlibReqHeader.str().length() << endl;

        return zlibReqHeader.str();
    }
}

/** Decompress the encoded message header */
std::string CSpdy3Compressor::decompressMsgHeader(const char *data, size_t len, Socket_ID_Type sockID)
{
    /*
     * 1. get the deflated header length out from the received message
     */

    size_t defHeaderLen = 0;
    std::string defHeaderLenString;
    defHeaderLenString.assign(data, 3);
    std::istringstream defHeaderLenStream(defHeaderLenString);
    defHeaderLenStream >> std::setbase(16) >> defHeaderLen;

    /*
     * 2. inflate the header depending on the length
     */

    spdylay_buffer spdyBuf;
    spdylay_buffer_init(&spdyBuf, 4096);

    ssize_t framelen;

    uint8_t *uCharBuf = new uint8_t[defHeaderLen];

//    EV_DEBUG_NOMODULE << "header before inflate is as char: ";
    for (size_t i = 0; i < defHeaderLen; i++)
    {
        uCharBuf[i] = data[i+3] + 128;
//        EV << data[i+3];
    }
//    EV << endl;

//    memcpy(uCharBuf, &data[3], defHeaderLen);

    framelen = spdylay_zlib_inflate_hd(&(zlibInfos[sockID].inflater), &spdyBuf, uCharBuf, defHeaderLen);

    if (framelen < 0)
    {
        delete[] uCharBuf;

        if (framelen == SPDYLAY_ERR_ZLIB)
        {
            throw cRuntimeError("spdylay_zlib_inflate_hd get Zlib error!");
        }
    }
    else
    {
        /*
         * 3. assign the inflated header to the http message to parse
         */
        uint8_t *infBuf = new uint8_t[framelen];
        spdylay_buffer_serialize(&spdyBuf, infBuf);

        size_t bufLength = len - 3 - defHeaderLen + framelen;
        std::string buf;
        buf.assign((char *)infBuf, framelen);

        EV_DEBUG_NOMODULE << "header length before inflate is: " << defHeaderLen << endl;
//        EV_DEBUG_NOMODULE << "header before inflate is: " << (char *)(data + 3) << endl;
//        EV_DEBUG_NOMODULE << "header before inflate is: " << uCharBuf << endl;
        EV_DEBUG_NOMODULE << "header length after inflate is: " << framelen << endl;
//        EV_DEBUG_NOMODULE << "header after inflate is: " << infBuf << endl;

        delete[] infBuf;
        delete[] uCharBuf;

        /*
         * 4. add body back to the request message to form the original http message
         */
        buf.append(&data[3+defHeaderLen], len - 3 - defHeaderLen);

        return buf;
    }
}

/** initialize compressors related to sockID */
void CSpdy3Compressor::initCompressors(Socket_ID_Type sockID)
{
    int rvD = spdylay_zlib_deflate_hd_init(&(zlibInfos[sockID].deflater), 1, SPDYLAY_PROTO_SPDY3);
    if (rvD != 0)
    {
        EV_ERROR_NOMODULE << "spdylay_zlib_deflate_hd_init failed!" << endl;
    }

    int rvI = spdylay_zlib_inflate_hd_init(&(zlibInfos[sockID].inflater), SPDYLAY_PROTO_SPDY3);
    if (rvI != 0)
    {
        EV_ERROR_NOMODULE << "spdylay_zlib_inflate_hd_init failed!" << endl;
    }
    zlibInfos[sockID].setZlib = !(rvD | rvI);
}

/** Release compressors assigned to the sockID */
void CSpdy3Compressor::releaseCompressors(Socket_ID_Type sockID)
{
    spdylay_zlib_deflate_free(&(zlibInfos[sockID].deflater));
    spdylay_zlib_inflate_free(&(zlibInfos[sockID].inflater));
}
