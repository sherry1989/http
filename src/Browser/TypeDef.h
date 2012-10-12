/*
 * TypeDef.h
 *
 *  Created on: 2012-5-22
 *      Author: Administrator
 */

#ifndef TYPEDEF_H_
#define TYPEDEF_H_

#include "HttpBrowser.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <map>
#include <deque>
#include <set>

typedef unsigned int Pipelining_Mode_Type;
const Pipelining_Mode_Type FILL_FIRST = 1;
const Pipelining_Mode_Type DISTRIBUTE_FIRST = 2;

//--added by wangqian, 2012-05-16
typedef unsigned int SvrSupportDetect_Method_Type;
const SvrSupportDetect_Method_Type PER_CONNECTION = 1;
const SvrSupportDetect_Method_Type PER_HOST = 2;

enum SvrSupport_Type
{
    e_Unknown = 0,
    e_Support = 1,
    e_NotSupport =2
};
//--added end

/**
 * Data structure used to keep state for each opened socket.
 *
 * An instance of this struct is created for each opened socket and assigned to
 * it as a myPtr. See the TCPSocket::CallbackInterface methods of HttpBrowser for more
 * details.
 */
//redefine SockData as PipeSockData
struct PipeSockData
{
    //--modified by wangqian, 2012-05-15
//            HttpRequestQueue messageQueue; ///< Queue of pending messages.
    std::string svrName;           ///< A reference to the server name.
    //--modified end
    TCPSocket *socket;             ///< A reference to the socket object.
    unsigned int pending;                   ///< A counter for the number of outstanding replies.

    //--added by wangqian, 2012-05-16
    SvrSupport_Type svrSupport;          ///< An identification of whether the server support pipelining.
    //--added end
};

/**
 * A list of HTTP requests to send.
 */
typedef std::deque<HttpRequestMessage*> HttpReqQueue;

typedef std::set<TCPSocket*> SocketSet;

#endif /* TYPEDEF_H_ */
