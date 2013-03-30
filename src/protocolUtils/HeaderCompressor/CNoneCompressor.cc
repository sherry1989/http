/*
 * CNoneCompressor.cc
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#include "CNoneCompressor.h"

CNoneCompressor::CNoneCompressor()
{
    // TODO Auto-generated constructor stub
    
}

CNoneCompressor::~CNoneCompressor()
{
    // TODO Auto-generated destructor stub
}

/** Compress the encoded message header */
std::string CNoneCompressor::compressMsgHeader(std::string msgHeader, Socket_ID_Type sockID, HttpNodeBase *pHttpNode, size_t bodyLength)
{
    return CHeaderCompressorBase::compressMsgHeader(msgHeader, sockID, pHttpNode, bodyLength);
}

/** Decompress the encoded message header */
std::string CNoneCompressor::decompressMsgHeader(const char *data, size_t len, Socket_ID_Type sockID)
{
    return CHeaderCompressorBase::decompressMsgHeader(data, len, sockID);
}

/** initialize compressors related to sockID */
void CNoneCompressor::initCompressors(Socket_ID_Type sockID)
{
    CHeaderCompressorBase::initCompressors(sockID);
}

/** Release compressors assigned to the sockID */
void CNoneCompressor::releaseCompressors(Socket_ID_Type sockID)
{
    CHeaderCompressorBase::releaseCompressors(sockID);
}
