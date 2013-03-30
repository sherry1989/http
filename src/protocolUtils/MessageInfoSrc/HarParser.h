/*
 * HarParser.h
 *
 *  Created on: Nov 19, 2012
 *      Author: qian
 */

#ifndef HARPARSER_H_
#define HARPARSER_H_

#include "MsgInfoSrcBase.h"

typedef unsigned int TimeSetting_Type;
const TimeSetting_Type FROM_HAR = 1;
const TimeSetting_Type SAME_CONFIG = 2;
const TimeSetting_Type AVE_VALUE = 3;
const TimeSetting_Type ASCEND_SORT = 4;
const TimeSetting_Type DESCEND_SORT = 5;

class HarParser : public MsgInfoSrcBase
{
    public:
        HarParser();
        virtual ~HarParser();

        virtual HeaderFrame getReqHeaderFrame(const RealHttpRequestMessage *httpRequest, HttpNodeBase *pHttpNode);
        virtual HeaderFrame getResHeaderFrame(const RealHttpReplyMessage *httpResponse, HttpNodeBase *pHttpNode);

    public:

        HeaderFrame getRequest(string requestURI);
        HeaderFrame getResponse(string requestURI);
        HeaderFrame getTiming(string requestURI);

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

    private:
        int ParseHarFiles(int n_files, char** files, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses, vector<HeaderFrame>* timings);

        static int ParseFile(const string& fn, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses, vector<HeaderFrame>* timings);
        static int ParseStream(istream& istrm, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses, vector<HeaderFrame>* timings);
        static HeaderFrame* GetHeaderFramePtr(vector<HeaderFrame>* frames, unsigned int expected_previous_len);

        //Depends on the timeSettingType, config the timings of each resource
        void ConfigTimings(vector<HeaderFrame>& timings);

        /**
         * A list of active sockets for each server
         */
        typedef std::map<string, HeaderFrame> HeaderMap;

        HeaderMap requestMap;
        HeaderMap responseMap;
        HeaderMap timingsMap;

        string harInfoFile;          // har file info configuration file.
        TimeSetting_Type timeSettingType;
        int waitTime;

};

#endif /* HARPARSER_H_ */
