/*
 * CSpdy3Compressor.h
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#ifndef CSPDY3COMPRESSOR_H_
#define CSPDY3COMPRESSOR_H_

#include "CHeaderCompressorBase.h"

typedef std::map<Socket_ID_Type, ZlibInfo> Compressors;

class CSpdy3Compressor: public CHeaderCompressorBase
{
    public:
        CSpdy3Compressor();
        virtual ~CSpdy3Compressor();

        /** Compress the encoded message header */
        virtual std::string compressMsgHeader(std::string msgHeader, Socket_ID_Type sockID, HttpNodeBase *pHttpNode, size_t bodyLength);

        /** Decompress the encoded message header */
        virtual std::string decompressMsgHeader(const char *data, size_t len, Socket_ID_Type sockID);

        /** initialize compressors related to sockID */
        virtual void initCompressors(Socket_ID_Type sockID);

        /** Release compressors assigned to the sockID */
        virtual void releaseCompressors(Socket_ID_Type sockID);

    private:
        Compressors zlibInfos;
};

#endif /* CSPDY3COMPRESSOR_H_ */
