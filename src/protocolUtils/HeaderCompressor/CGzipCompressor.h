/*
 * CGzipCompressor.h
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#ifndef CGZIPCOMPRESSOR_H_
#define CGZIPCOMPRESSOR_H_

#include "CHeaderCompressorBase.h"

class CGzipCompressor: public CHeaderCompressorBase
{
    public:
        CGzipCompressor();
        virtual ~CGzipCompressor();
};

#endif /* CGZIPCOMPRESSOR_H_ */
