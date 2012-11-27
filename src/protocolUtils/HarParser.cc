/*
 * HarParser.cc
 *
 *  Created on: Nov 19, 2012
 *      Author: qian
 */

#include "HarParser.h"
#include "HttpLogdefs.h"
#include <omnetpp.h>
#include <cassert>

HarParser::HarParserPtr HarParser::_instance;

HarParser::HarParser()
{
    // TODO Auto-generated constructor stub
    vector<HeaderFrame> requests;
    vector<HeaderFrame> responses;
    vector<HeaderFrame> timings;

    /*
     * 1. get the har file and parse it
     */
    int n_files;
    n_files = 1;
    char** files = new char*[n_files];

    files[0] = new char[50];
    files[0][0] = '\0';
//    strcpy(files[0], "www.baidu.com.har\0");
//    strcpy(files[0], "www.sina.com.cn.har\0");
    strcpy(files[0], "www.sina.com.cn_firebug.har\0");
//    strcpy(files[0], "music.10086.cn.har\0");

    ParseHarFiles(n_files, files, &requests, &responses, &timings);

    EV_DEBUG_NOMODULE << "after ParseHarFiles, requests size is " << requests.size() << endl;
    EV_DEBUG_NOMODULE << "after ParseHarFiles, responses size is " << responses.size() << endl;
    EV_DEBUG_NOMODULE << "after ParseHarFiles, timings size is " << timings.size() << endl;

    for (unsigned int i = 0; i < n_files; i++)
    {
        delete[] files[i];
    }
    delete[] files;


    /*
     * 2. prepare for the server and browser using
     */
    size_t pair_count = 0;

    //initialize sitedef file
    ofstream sitedef("har.sitedef", ios::out|ios::trunc);
    sitedef << "[HTML]" << endl;
    sitedef << "/har;har.pagedef;100" << endl;
    sitedef << endl;
    sitedef << "[RESOURCES]" << endl;
    ofstream pagedef("har.pagedef", ios::out|ios::trunc);

    for (unsigned int i = 0; i < requests.size(); ++i)
    {
        ++pair_count;
        unsigned int j;
        for (j = 0; j < requests[i].size(); ++j)
        {
            if (requests[i][j].key.find(":path") != string::npos)
            {
                /*
                 * record request and response map for browser and server check to generate messages
                 * the key is the request-uri in each request
                 */
                EV_DEBUG_NOMODULE << "Initialize No." << i << " pair of headers, key in map is " << requests[i][j].val << endl;

                string key = requests[i][j].val;

                /*
                 * if there is ";" in path, cannot simply use it as key
                 * if do so, when parse the sitedef file, will get error
                 * so just use the substring before first';' to be the key
                 */
                if (key.find(";") != string::npos)
                {
                    // Parse the key string on ;, and just use the substring before first';' to be the key
                    cStringTokenizer tokenizer = cStringTokenizer(key.c_str(), ";");
                    std::vector<std::string> res = tokenizer.asVector();
                    key = res[0];
                    EV_DEBUG_NOMODULE << "change key to " << key << endl;
                }

                EV_DEBUG_NOMODULE << "Request is" << endl;
                OutputHeaderFrame(requests[i]);
                requestMap.insert(make_pair(key, requests[i]));

                EV_DEBUG_NOMODULE << "Response is" << endl;
                OutputHeaderFrame(responses[i]);
                responseMap.insert(make_pair(key, responses[i]));

                EV_DEBUG_NOMODULE << "Timings is" << endl;
                OutputHeaderFrame(timings[i]);
                timingsMap.insert(make_pair(key, timings[i]));

                /*
                 * generate sitedef file and the pagedef file
                 */
                pagedef << key << endl;
                sitedef << key << ";";
                unsigned int k;
                for (k = 0; k < responses[i].size(); ++k)
                {
                    if (responses[i][k].key.find("Content-Length") != string::npos)
                    {
                        sitedef << responses[i][k].val << endl;
                        break;
                    }
                }
                //if cannot find some resources' content-length, use 0 to let the sitedef file has the right style.
                if (k == responses[i].size())
                {
                    sitedef << "0" << endl;
                }

                break;
            }
        }
        if (j == requests[i].size())
        {
            EV_ERROR_NOMODULE << "When i is " << i << " cannot find path key" << endl;
            EV_DEBUG_NOMODULE << "Request is" << endl;
            OutputHeaderFrame(requests[i]);
            EV_DEBUG_NOMODULE << "Response is" << endl;
            OutputHeaderFrame(responses[i]);
            EV_DEBUG_NOMODULE << "Timings is" << endl;
            OutputHeaderFrame(timings[i]);
        }

    }

    sitedef.close();
    pagedef.close();

    EV_DEBUG_NOMODULE << "Total messages pair number is " << pair_count << endl;
    EV_DEBUG_NOMODULE << "Total request map size is " << requestMap.size() << endl;
}

HarParser::~HarParser()
{
    // TODO Auto-generated destructor stub
}

HeaderFrame HarParser::getRequest(string requestURI)
{
    HeaderMap::iterator iter;
    iter = requestMap.find(requestURI);
    if (iter != requestMap.end())
    {
        EV_DEBUG_NOMODULE << "Find the requestURI [" << requestURI << "] related request message, header frame is" << endl;
        OutputHeaderFrame(iter->second);
        return iter->second;
    }
    else
    {
        EV_DEBUG_NOMODULE << "Do not find the requestURI [" << requestURI << "] related request message" << endl;
        HeaderFrame emptyFrame;
        return emptyFrame;
    }
}

HeaderFrame HarParser::getResponse(string requestURI)
{
    HeaderMap::iterator iter;
    iter = responseMap.find(requestURI);
    if (iter != responseMap.end())
    {
        EV_DEBUG_NOMODULE << "Find the requestURI [" << requestURI << "] related response message, header frame is" << endl;
        OutputHeaderFrame(iter->second);
        return iter->second;
    }
    else
    {
        EV_DEBUG_NOMODULE << "Do not find the requestURI [" << requestURI << "] related response message" << endl;
        HeaderFrame emptyFrame;
        return emptyFrame;
    }
}

HeaderFrame HarParser::getTiming(string requestURI)
{
    HeaderMap::iterator iter;
    iter = timingsMap.find(requestURI);
    if (iter != timingsMap.end())
    {
        EV_DEBUG_NOMODULE << "Find the requestURI [" << requestURI << "] related timings, timing info is" << endl;
        OutputHeaderFrame(iter->second);
        return iter->second;
    }
    else
    {
        EV_DEBUG_NOMODULE << "Do not find the requestURI [" << requestURI << "] related timings message" << endl;
        HeaderFrame emptyFrame;
        return emptyFrame;
    }
}

int HarParser::ParseHarFiles(int n_files, char** files, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses, vector<HeaderFrame>* timings)
{
    int pipe_fds[2];  // read, write
    int pipe_retval = pipe(pipe_fds);
    if (pipe_retval == -1)
    {
        perror("");
        abort();
    }
    pid_t child_pid;
    if ((child_pid = fork()) == -1)
    {
        perror("Fork failed");
        abort();
    }
    if (child_pid == 0)
    {
        dup2(pipe_fds[1], 1);
        char** new_argv = new char*[n_files + 2];
        new_argv[0] = (char*) "./../src/protocolUtils/harfile_translator.py";
        for (int i = 0; i < n_files; ++i)
        {
            new_argv[i + 1] = files[i];
        }
        new_argv[n_files + 1] = 0;
        if (execvp(new_argv[0], new_argv) == -1)
        {
            perror("Great: ");
            abort();
        }
    }
    else
    {
        close(pipe_fds[1]);
    }

    stringstream input;
    char buf[256];
    ssize_t bytes_read = 0;
    while ((bytes_read = read(pipe_fds[0], &buf, sizeof(buf) - 1)))
    {
        if (bytes_read < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            break;
        }
        else
        {
            buf[bytes_read] = 0;
            input << buf;
        }
    }

    ofstream inputfile("input.txt", ios::out|ios::trunc);
    inputfile << input.str();
    inputfile.close();


    if (!ParseStream(input, requests, responses, timings))
    {
        EV_ERROR_NOMODULE << "Failed to parse correctly. Exiting\n";
        return 0;
    }
    return 1;
}

void HarParser::OutputHeaderFrame(const HeaderFrame& hf)
{
    for (HeaderFrame::const_iterator i = hf.begin(); i != hf.end(); ++i)
    {
        auto line = *i;
        const string& k = line.key;
        const string& v = line.val;
        EV_DEBUG_NOMODULE << k << ": " << v << "\n";
    }
}

HeaderFrame* HarParser::GetHeaderFramePtr(vector<HeaderFrame>* frames, unsigned int expected_previous_len)
{
    if (frames->size() <= expected_previous_len)
    {
        frames->push_back(HeaderFrame());
    }
    return &(frames->back());
}

int HarParser::ParseFile(const string& fn, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses, vector<HeaderFrame>* timings)
{
    ifstream ifs(fn.c_str());
    ParseStream(ifs, requests, responses, timings);
    return 1;
}

int HarParser::ParseStream(istream& istrm, vector<HeaderFrame>* requests, vector<HeaderFrame>* responses, vector<HeaderFrame>* timings)
{
    int frames_len = 0;
    int frames_idx = 0;
    vector<HeaderFrame>* frames[3] = { requests, responses, timings };
    if (!(requests->empty() && responses->empty() && timings->empty()))
    {
        return -1;
    }
    HeaderFrame* cur_frame = GetHeaderFramePtr(frames[frames_idx], frames_len);

    unsigned int i = 1;
    while (istrm.good())
    {
        string line;
        getline(istrm, line);
        EV_DEBUG_NOMODULE << "read line #" << i << ". line content is: " << line << endl;
        size_t colon_pos = line.find_first_of(":", 1);
        if (line.size() == 0)
        {
            // finished with this frame.
            if (frames_idx == 2)
            {
                ++frames_len;
                frames_idx = -1;
                EV_DEBUG_NOMODULE << "finished with this frame. frames_len next is" << frames_len << endl;
            }
            frames_idx++;
            cur_frame = GetHeaderFramePtr(frames[frames_idx], frames_len);
            continue;
        }
        else if (colon_pos == string::npos || colon_pos + 1 > line.size() || line[colon_pos + 1] != ' ')
        {
            EV_ERROR_NOMODULE << "Misformatted line. Was expecting to see a ': ' in there.\n";
            EV_ERROR_NOMODULE << "Line:\n";
            EV_ERROR_NOMODULE << line << "\n";
            EV_ERROR_NOMODULE << "colon_pos: " << colon_pos << "\n";
            return 0;
        }
        size_t val_start = colon_pos + 2;
        size_t val_size = line.size() - val_start;
        cur_frame->push_back(KVPair(line.substr(0, colon_pos), line.substr(val_start, val_size)));

        i++;
    }
    if (requests->back().empty())
    {
        requests->pop_back();
    }
    return 1;
}
