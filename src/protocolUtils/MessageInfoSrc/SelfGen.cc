/*
 * SelfGen.cc
 *
 *  Created on: Mar 26, 2013
 *      Author: qian
 */

#include "SelfGen.h"

Define_Module(SelfGen);

SelfGen::SelfGen()
{

}

SelfGen::~SelfGen()
{

}

void SelfGen::initialize(int stage)
{
    // TODO - Generated method body
    EV_DEBUG << "Initializing stage " << stage << endl;

    MsgInfoSrcBase::initialize(stage);
}

void SelfGen::finish()
{
}

void SelfGen::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

HeaderFrame SelfGen::getReqHeaderFrame(const RealHttpRequestMessage *httpRequest, HttpNodeBase *pHttpNode)
{
    return MsgInfoSrcBase::getReqHeaderFrame(httpRequest, pHttpNode);
}

HeaderFrame SelfGen::getResHeaderFrame(const RealHttpReplyMessage *httpResponse, HttpNodeBase *pHttpNode)
{
    return MsgInfoSrcBase::getResHeaderFrame(httpResponse, pHttpNode);
}
