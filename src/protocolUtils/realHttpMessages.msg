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

//
// TODO generated message class
//

cplusplus
{{
    #include "HttpMessages_m.h"
}}

packet HttpRequestMessage;
packet HttpReplyMessage;

packet RealHttpRequestMessage extends HttpRequestMessage
{
    @omitGetVerb(true);         // Dont append get to generated getter methods.
    string accept = "";
	string acceptCharset = "";
	string acceptEncoding = "";
	string acceptLanguage = "";
	string host = "";
	string ifModifiedSince = "";
	string ifNoneMatch = "";
	string referer = "";
	string userAgent = "";
}

packet RealHttpReplyMessage extends HttpReplyMessage
{
    @omitGetVerb(true);
    string requestURI = "";
    string acceptRanges = "none";
	int age = 0;
	string cacheControl = "";
	string contentEncoding = "";
	string contentLanguage = "";
	int contentLength = 0;
	string contentLocation = "";
	string date = "";
	string etag = "";
	string expires = "";
	string lastModified = "";
	string location = "";
	string pragma = "";
	string server = "";
	string transferEncoding = "";
	string vary = "";
	string via = "";
	string xPoweredBy = "";
}
