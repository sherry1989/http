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

#ifndef __HTTP_HARSERVER_H_
#define __HTTP_HARSERVER_H_

#include "NSCHttpServer.h"
#include "HarParser.h"

/**
 * TODO - Generated class
 */
class HarServer : public NSCHttpServer
{
  protected:
    virtual void initialize(int stage);

    /** Returns the number of initialization stages. Two required. */
    int numInitStages() const {return 2;}

    /** Report final statistics */
    virtual void finish();

    /** Handle a received data message, e.g. check if the content requested exists. */
    virtual HttpReplyMessage *handleReceivedMessage(cMessage *msg);

    /**
     * Handler for socket data arrived events.
     * Dispatches the received message to the message handler in the base class and
     * finishes by deleting the received message.
     */
    virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);

    /*
     * Format an application TCP_TRANSFER_BYTESTREAM response message which can be sent though NSC TCP depence on the application layer protocol
     * the protocol type can be HTTP \ SPDY \ HTTPS+M \ HTTPNF
     */
    virtual std::string formatByteResponseMessage(TCPSocket *socket, HttpReplyMessage *httpResponse);

    /** Format a response message to HTTP Response Message Header */
    virtual std::string formatHttpResponseMessageHeader(RealHttpReplyMessage *httpResponse);

    /** Format a Response message to SPDY Header Block Response Message Header */
    virtual std::string formatHeaderBlockResponseMessageHeader(RealHttpReplyMessage *httpResponse);

    HarParser *pHarParser;

    // Basic statistics
    long mediaResourcesServed;
    long otherResourcesServed;

  private:
    bool getWaitTimeFromHar;
};

#endif
