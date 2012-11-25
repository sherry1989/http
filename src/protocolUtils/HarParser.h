/*
 * HarParser.h
 *
 *  Created on: Nov 19, 2012
 *      Author: qian
 */

#ifndef HARPARSER_H_
#define HARPARSER_H_

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

//#include "bit_bucket.h"
#include "trivial_http_parse.h"

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

class HarParser
{
    public:
        static HarParser * Instance()
        {
            if (0 == _instance.get())
            {
                _instance.reset(new HarParser());
            }
            return _instance.get();
        }

        HeaderFrame getRequest(string requestURI);
        HeaderFrame getResponse(string requestURI);

    protected:
        HarParser();
        virtual ~HarParser();
        friend class auto_ptr<HarParser>;
        typedef auto_ptr<HarParser> HarParserPtr;
        static HarParserPtr _instance;

    private:
        int ParseHarFiles(int n_files, char** files, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses);
        void OutputHeaderFrame(const HeaderFrame& hf);

        /**
         * A list of active sockets for each server
         */
        typedef std::map<string, HeaderFrame> HeaderMap;

        HeaderMap requestMap;
        HeaderMap responseMap;

};

#endif /* HARPARSER_H_ */
