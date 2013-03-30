/*
 * ProtocolTypeDef.h
 *
 *  Created on: Oct 15, 2012
 *      Author: qian
 */

#ifndef PROTOCOLTYPEDEF_H_
#define PROTOCOLTYPEDEF_H_

typedef unsigned int Header_Encode_Type;
const Header_Encode_Type HTTP_ASCII = 1;
const Header_Encode_Type SPDY_HEADER_BLOCK = 2;

typedef unsigned int Header_Compress_Type;
const Header_Compress_Type NONE = 1;
const Header_Compress_Type GZIP = 2;
const Header_Compress_Type SPDY3 = 3;
const Header_Compress_Type HUFFMAN = 4;
const Header_Compress_Type DELTA = 5;
const Header_Compress_Type HEADERDIFF = 6;

typedef unsigned int Message_Info_Src;
const Message_Info_Src GENERATE = 1;
const Message_Info_Src HAR_FILE = 2;

typedef unsigned int Socket_ID_Type;

#include "httpProtocol.h"

#include "spdylay_zlib.h"
#include <map>
#include <string>
#include <vector>
#include <sstream>

using std::vector;
using std::string;
using std::ostream;

struct ZlibInfo
{
        bool setZlib;
        spdylay_zlib deflater;
        spdylay_zlib inflater;
};

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


#endif /* PROTOCOLTYPEDEF_H_ */
