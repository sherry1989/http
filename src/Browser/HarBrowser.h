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

#ifndef __HTTP_HARBROWSER_H_
#define __HTTP_HARBROWSER_H_

#include "NSCHttpBrowser.h"
#include "HarParser.h"

/**
 * TODO - Generated class
 */
class HarBrowser : public NSCHttpBrowser
{
  protected:
    virtual void initialize(int stage);

    /** Format a Request message to HTTP Request Message Header */
    virtual std::string formatHttpRequestMessageHeader(const RealHttpRequestMessage *httpRequest);

    HarParser *pHarParser;
};

#endif
