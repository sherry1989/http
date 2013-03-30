/*
 * CSpdyHeaderBlockEncoder.h
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#ifndef CSPDYHEADERBLOCKENCODER_H_
#define CSPDYHEADERBLOCKENCODER_H_

#include "CHeaderEncoderBase.h"

class CSpdyHeaderBlockEncoder: public CHeaderEncoderBase
{
    public:
        CSpdyHeaderBlockEncoder();
        virtual ~CSpdyHeaderBlockEncoder();

    public:
        /** Generate an encoded request message header */
        virtual std::string generateReqMsgHeader(const HeaderFrame requestFrame, const string requestURI);

        /** Generate an encoded response message header */
        virtual std::string generateResMsgHeader(const HeaderFrame responseFrame);

    protected:
        /** Format a Name Value Pair */
        std::string formatNameValuePair(const std::string name, const std::string value);
};

#endif /* CSPDYHEADERBLOCKENCODER_H_ */
