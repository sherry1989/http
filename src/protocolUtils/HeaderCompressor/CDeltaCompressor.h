/*
 * CDeltaCompressor.h
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#ifndef CDELTACOMPRESSOR_H_
#define CDELTACOMPRESSOR_H_

#include "CHeaderCompressorBase.h"

class CDeltaCompressor: public CHeaderCompressorBase
{
    public:
        CDeltaCompressor();
        virtual ~CDeltaCompressor();
};

#endif /* CDELTACOMPRESSOR_H_ */
