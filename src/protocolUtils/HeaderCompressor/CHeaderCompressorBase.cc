/*
 * CHeaderCompressorBase.cc
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#include "CHeaderCompressorBase.h"

CHeaderCompressorBase::CHeaderCompressorBase()
{
    // TODO Auto-generated constructor stub
    
}

CHeaderCompressorBase::~CHeaderCompressorBase()
{
    // TODO Auto-generated destructor stub
}

/** Compress the encoded message header */
std::string CHeaderCompressorBase::compressMsgHeader(std::string msgHeader, Socket_ID_Type sockID, HttpNodeBase *pHttpNode, size_t bodyLength)
{
    return msgHeader;
}

/** Decompress the encoded message header */
std::string CHeaderCompressorBase::decompressMsgHeader(const char *data, size_t len, Socket_ID_Type sockID)
{
    std::string defHeaderLenString;
    defHeaderLenString.assign(data, len);
    return defHeaderLenString;
}

/** initialize compressors related to sockID */
void CHeaderCompressorBase::initCompressors(Socket_ID_Type sockID)
{

}

/** Release compressors assigned to the sockID */
void CHeaderCompressorBase::releaseCompressors(Socket_ID_Type sockID)
{

}
