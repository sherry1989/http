/*
 * ProtocolTypeDef.h
 *
 *  Created on: Oct 15, 2012
 *      Author: qian
 */

#ifndef PROTOCOLTYPEDEF_H_
#define PROTOCOLTYPEDEF_H_

typedef unsigned int Protocol_Type;
const Protocol_Type HTTP = 1;
const Protocol_Type SPDY_ZLIB_HTTP = 2;
const Protocol_Type SPDY_HEADER_BLOCK = 3;
const Protocol_Type HTTPNF = 4;

#include "httpProtocol.h"


#endif /* PROTOCOLTYPEDEF_H_ */
