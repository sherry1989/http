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

#ifndef __HTTP_SELFGEN_H_
#define __HTTP_SELFGEN_H_

#include "MsgInfoSrcBase.h"

/**
 * TODO - Generated class
 */
class SelfGen : public MsgInfoSrcBase
{
   public:
        SelfGen();
        virtual ~SelfGen();

        virtual HeaderFrame getReqHeaderFrame(const RealHttpRequestMessage *httpRequest, HttpNodeBase *pHttpNode);
        virtual HeaderFrame getResHeaderFrame(const RealHttpReplyMessage *httpResponse, HttpNodeBase *pHttpNode);

    protected:
        /** @name cSimpleModule redefinitions */
        //@{
        /**
         * Initialization of the component and startup of browse event scheduling.
         * Multi-stage is required to properly initialize the object for random site selection after
         * all servers have been registered.
         */
        virtual void initialize(int stage);

        /** Report final statistics */
        virtual void finish();

        /** Handle incoming messages */
        virtual void handleMessage(cMessage *msg);

        /** Returns the number of initialization stages. Two required. */
        int numInitStages() const {return 2;}
        //@}
};

#endif
