/*
 * MsgInfoSrcBase.h
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#ifndef CMSGINFOSRCBASE_H_
#define CMSGINFOSRCBASE_H_

#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include <zlib.h>

#include <memory>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <omnetpp.h>
#include "ProtocolTypeDef.h"
#include "HttpUtils.h"
#include "HttpLogdefs.h"
#include "HttpNodeBase.h"

using std::vector;
using std::memset;
using std::fixed;
using std::cout;
using std::cerr;
using std::stringstream;
using std::string;
using std::flush;
using std::max;
using std::min;
using std::min;
using std::ostream;
using std::map;
using std::auto_ptr;
using std::ofstream;
using std::ios;
using std::getline;
using std::ifstream;
using std::istream;

class MsgInfoSrcBase : public cSimpleModule
{
    public:
        MsgInfoSrcBase();
        virtual ~MsgInfoSrcBase();

        virtual HeaderFrame getReqHeaderFrame(const RealHttpRequestMessage *httpRequest, HttpNodeBase *pHttpNode);
        virtual HeaderFrame getResHeaderFrame(const RealHttpReplyMessage *httpResponse, HttpNodeBase *pHttpNode);

    protected:
        /** @name cSimpleModule redefinitions */
        //@{
        /**
         * Initialization of the component and startup of browse event scheduling.
         * Multi-stage is required to properly initialize the object for random site selection after
         * all servers have been registered.
         */
        virtual void initialize(int stage);

        /** Report final statistics */
        virtual void finish();

        /** Handle incoming messages */
        virtual void handleMessage(cMessage *msg);

        /** Returns the number of initialization stages. Two required. */
        int numInitStages() const {return 2;}
        //@}

    protected:
        void outputHeaderFrame(const HeaderFrame& hf);

        KVPair makeKVPair(std::string name, const std::string value);

        Header_Encode_Type headerEncodeType;                            //Message header encode type
};

#endif /* CMSGINFOSRCBASE_H_ */
