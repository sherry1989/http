//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "HarBrowser.h"
#include <cctype>
#include <algorithm>

Define_Module(HarBrowser);

void HarBrowser::initialize(int stage)
{
    // TODO - Generated method body
    if (stage==0)
    {
        pHarParser = HarParser::Instance();
    }

    NSCHttpBrowser::initialize(stage);
}

/*
 * Format a Request message to HTTP Request Message Header
 * Request   = Request-Line
                *(( general-header)
                | request-header
                | entity-header)CRLF)
                CRLF
                [ message-body ]
 */
std::string HarBrowser::formatHttpRequestMessageHeader(const RealHttpRequestMessage *httpRequest)
{
    std::ostringstream str;

    /*
     * get the har request for this request-uri
     */
    // Parse the request string on spaces
    cStringTokenizer tokenizer = cStringTokenizer(httpRequest->heading(), " ");
    std::vector<std::string> res = tokenizer.asVector();
    if (res.size() != 3)
    {
        EV_ERROR << "Invalid request string: " << httpRequest->heading() << endl;
    }

    HeaderFrame requestFrame;
    if (res[0] == "GET")
    {
        // use the resource string part whith is the request-uri to get the real har requests
        requestFrame = pHarParser->getRequest(res[1]);
        if (requestFrame.size() == 0)
        {
            EV_ERROR << "requestFrame size is 0, call NSCHttpBrowser::formatHttpRequestMessageHeader." << endl;
            return NSCHttpBrowser::formatHttpRequestMessageHeader(httpRequest);
        }
        EV_DEBUG << "requestFrame size is: " << requestFrame.size() << endl;
    }
    else
    {
        EV_ERROR << "Unsupported request type " << res[0] << " for " << httpRequest->heading() << endl;
        return NSCHttpBrowser::formatHttpRequestMessageHeader(httpRequest);
    }

    /*
     * pre-process the headerFrame
     */
    string method("");
    string version("");
    for (unsigned int i = 0; i < requestFrame.size(); ++i)
    {
        if (requestFrame[i].key.find(":method") != string::npos)
        {
            EV_DEBUG << "original method is " << requestFrame[i].val << endl;
            //since the method get from har is lower, need to transform to upper
            method.assign(requestFrame[i].val);
            std::transform(method.begin(), method.end(), method.begin(), (int (*)(int))std::toupper);
            EV_DEBUG << "generate method " << method << endl;
            continue;
        }
        else if (requestFrame[i].key.find(":version") != string::npos)
        {
            if (requestFrame[i].val.find("1.1") != string::npos)
            {
                version.assign("HTTP/1.1");
            }
            else if (requestFrame[i].val.find("1.0") != string::npos)
            {
                version.assign("HTTP/1.0");
            }
            else
            {
                throw cRuntimeError("Invalid HTTP version: %s", requestFrame[i].val.c_str());
            }
            EV_DEBUG << "generate version " << version << endl;
            continue;
        }
    }

    /*************************************Generate HTTP Request Header*************************************/

    /**
     * 1.Generate Request-Line:
     * Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
     */

    /** 1.1 Method SP */
    str << method << " ";

    /** 1.2 Request-URI SP */
    str << res[1] << " ";

    /** 1.3 HTTP-Version CRLF */
    str << version << "\r\n";


    /**
     * 2.Generate the rest header
     */
    for (unsigned int i = 0; i < requestFrame.size(); ++i)
    {
        EV_DEBUG << "read headerFrame, key is " << requestFrame[i].key << endl;
        const string& k = requestFrame[i].key;
        const string& v = requestFrame[i].val;
        // the key without ":" is the ones belong to HTTP/1.1 header
        if (k[0] != ':')
        {
            str << k << ": " << v << "\r\n";
            EV_DEBUG << "generate header line: " << k << ": " << v << endl;
        }
        else if (k.find(":host") != string::npos)
        {
            str << "host: " << v << "\r\n";
            EV_DEBUG << "generate header line: " << "host: " << v << endl;
        }
    }
    /*************************************Finish generating HTTP Request Header*************************************/

//    str << "\r\n";

    return str.str();
}