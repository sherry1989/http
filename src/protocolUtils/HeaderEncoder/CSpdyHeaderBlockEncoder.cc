/*
 * CSpdyHeaderBlockEncoder.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#include "CSpdyHeaderBlockEncoder.h"

CSpdyHeaderBlockEncoder::CSpdyHeaderBlockEncoder()
{
    // TODO Auto-generated constructor stub
    
}

CSpdyHeaderBlockEncoder::~CSpdyHeaderBlockEncoder()
{
    // TODO Auto-generated destructor stub
}

/** Format a Request message to SPDY Header Block Request Message Header
 *  +-------------------+                |
 | Number of Name/Value pairs (int32) |   <+
 +------------------------------------+    |
 |     Length of name (int32)         |    | This section is the "Name/Value
 +------------------------------------+    | Header Block", and is compressed.
 |           Name (string)            |    |
 +------------------------------------+    |
 |     Length of value  (int32)       |    |
 +------------------------------------+    |
 |          Value   (string)          |    |
 +------------------------------------+    |
 |           (repeats)                |   <+
 */
std::string CSpdyHeaderBlockEncoder::generateReqMsgHeader(const HeaderFrame requestFrame, const string requestURI)
{
    std::ostringstream str;

    /*************************************Generate Number of Name/Value pairs*************************************/

    uint32_t nvLen = requestFrame.size();

    str << nvLen;

    EV_DEBUG_NOMODULE << "Number of Name/Value pairs is: " << nvLen << endl;

    /*************************************Finish generating Number of Name/Value pairs*************************************/

    /*************************************Generate Name-Value Header Block*************************************/
    for (unsigned int i = 0; i < requestFrame.size(); ++i)
    {
        EV_DEBUG_NOMODULE << "read headerFrame, key is " << requestFrame[i].key << endl;
        const string& k = requestFrame[i].key;
        const string& v = requestFrame[i].val;

        str << formatNameValuePair(k, v);

    }
    /*************************************Finish generating Name-Value Header Block*************************************/

    return str.str();
}

/** Format a Response message to SPDY Header Block Response Message Header
 *  +-------------------+                |
 | Number of Name/Value pairs (int32) |   <+
 +------------------------------------+    |
 |     Length of name (int32)         |    | This section is the "Name/Value
 +------------------------------------+    | Header Block", and is compressed.
 |           Name (string)            |    |
 +------------------------------------+    |
 |     Length of value  (int32)       |    |
 +------------------------------------+    |
 |          Value   (string)          |    |
 +------------------------------------+    |
 |           (repeats)                |   <+
 */
std::string CSpdyHeaderBlockEncoder::generateResMsgHeader(const HeaderFrame responseFrame)
{
    std::ostringstream str;

    /*************************************Generate Number of Name/Value pairs*************************************/

    uint32_t nvLen = responseFrame.size();

    str << nvLen;

    EV_DEBUG_NOMODULE << "Number of Name/Value pairs is: " << nvLen << endl;

    /*************************************Finish generating Number of Name/Value pairs*************************************/

    /*************************************Generate Name-Value Header Block*************************************/
    for (unsigned int i = 0; i < responseFrame.size(); ++i)
    {
        EV_DEBUG_NOMODULE << "read headerFrame, key is " << responseFrame[i].key << endl;
        const string& k = responseFrame[i].key;
        const string& v = responseFrame[i].val;

        str << formatNameValuePair(k, v);

    }
    /*************************************Finish generating Name-Value Header Block*************************************/

    return str.str();
}


/** Format a Name Value Pair
 *
 +------------------------------------+
 |     Length of name (int32)         |
 +------------------------------------+
 |           Name (string)            |
 +------------------------------------+
 |     Length of value  (int32)       |
 +------------------------------------+
 |          Value   (string)          |
 +------------------------------------+
 */
std::string CSpdyHeaderBlockEncoder::formatNameValuePair(const std::string name, const std::string value)
{
    std::ostringstream str;

    uint32_t nameLen = name.length();
    str << nameLen << name;
    EV_DEBUG_NOMODULE << "generate header line name is: " << name << ". length is " << nameLen << endl;

    uint32_t valueLen = value.length();
    str << valueLen << value;
    EV_DEBUG_NOMODULE << "generate header line value is: " << value << ". length is " << valueLen << endl;

    return str.str();
}
