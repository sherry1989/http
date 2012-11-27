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

struct KVPair
{
        string key;
        string val;
        KVPair()
        {
        }
        KVPair(string key, string val) :
                key(key), val(val)
        {
        }
        friend ostream& operator<<(ostream& os, const KVPair& kv)
        {
            os << "\"" << kv.key << "\" \"" << kv.val << "\"";
            return os;
        }
        size_t size() const
        {
            return key.size() + val.size();
        }
};

typedef vector<KVPair> Lines;

typedef Lines HeaderFrame;

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
        HeaderFrame getTiming(string requestURI);

    protected:
        HarParser();
        virtual ~HarParser();
        friend class auto_ptr<HarParser>;
        typedef auto_ptr<HarParser> HarParserPtr;
        static HarParserPtr _instance;

    private:
        int ParseHarFiles(int n_files, char** files, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses, vector<HeaderFrame>* timings);
        void OutputHeaderFrame(const HeaderFrame& hf);

        static int ParseFile(const string& fn, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses, vector<HeaderFrame>* timings);
        static int ParseStream(istream& istrm, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses, vector<HeaderFrame>* timings);
        static HeaderFrame* GetHeaderFramePtr(vector<HeaderFrame>* frames, unsigned int expected_previous_len);

        /**
         * A list of active sockets for each server
         */
        typedef std::map<string, HeaderFrame> HeaderMap;

        HeaderMap requestMap;
        HeaderMap responseMap;
        HeaderMap timingsMap;

};

#endif /* HARPARSER_H_ */
