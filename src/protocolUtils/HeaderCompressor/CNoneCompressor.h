/*
 * CNoneCompressor.h
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#ifndef CNONECOMPRESSOR_H_
#define CNONECOMPRESSOR_H_

#include "CHeaderCompressorBase.h"

class CNoneCompressor: public CHeaderCompressorBase
{
    public:
        CNoneCompressor();
        virtual ~CNoneCompressor();

        /** Compress the encoded message header */
        virtual std::string compressMsgHeader(std::string msgHeader, Socket_ID_Type sockID, HttpNodeBase *pHttpNode, size_t bodyLength);

        /** Decompress the encoded message header */
        virtual std::string decompressMsgHeader(const char *data, size_t len, Socket_ID_Type sockID);

        /** initialize compressors related to sockID */
        virtual void initCompressors(Socket_ID_Type sockID);

        /** Release compressors assigned to the sockID */
        virtual void releaseCompressors(Socket_ID_Type sockID);
};

#endif /* CNONECOMPRESSOR_H_ */
