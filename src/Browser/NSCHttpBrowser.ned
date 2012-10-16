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

package http.src.Browser;

import inet.applications.httptools.HttpBrowser;

//
// TODO auto-generated module
//
simple NSCHttpBrowser extends HttpBrowser
{
    parameters:
        //--added by wangqian, 2012-05-15
        int maxConnections = default(256);
        int maxConnectionsPerHost = default(8);
        int maxPipelinedReqs = default(8);
        int pipeliningMode = default(1);
        //--added end
        
        //--added by wangqian, 2012-05-16
        int SvrSupportDetect = default(1);
        //--added end
        
        //--added by wangqian, 2012-10-16
        int protocolType;
        //--added end
        
    @class(NSCHttpBrowser);
}
