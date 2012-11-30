#
# OMNeT++/OMNEST Makefile for http
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out -I/usr/include/c++/4.6.3 -I/usr/include -I../inet/src/linklayer/ethernet -I../inet/src/linklayer/ieee80211/radio -I../inet/src/networklayer/common -I../inet/src/networklayer/icmpv6 -I../inet/src -I../inet/src/world/obstacles -I../inet/src/networklayer/xmipv6 -I../inet/src/networklayer/manetrouting/base -I../inet/src/mobility -I../inet/src/networklayer/contract -I../inet/src/linklayer/ieee80211/mgmt -I../inet/src/applications/httptools -I../inet/src/util -I../inet/src/transport/contract -I../inet/src/linklayer/radio/propagation -I../inet/src/linklayer/ieee80211/radio/errormodel -I../inet/src/linklayer/radio -I../inet/src/world/powercontrol -I../inet/src/util/headerserializers/tcp -I../inet/src/networklayer/ipv4 -I../inet/src/util/headerserializers/ipv4 -I../inet/src/base -I../inet/src/world/httptools -I../inet/src/util/headerserializers -I../inet/src/world/radio -I../inet/src/linklayer/ieee80211/mac -I../inet/src/networklayer/ipv6 -I../inet/src/transport/sctp -I../inet/src/util/headerserializers/udp -I../inet/src/networklayer/ipv6tunneling -I../inet/src/applications/pingapp -I../inet/src/battery/models -I../inet/src/util/headerserializers/sctp -I../inet/src/linklayer/contract -I../inet/src/networklayer/arp -I../inet/src/transport/tcp_common -I../inet/src/transport/udp -L../inet/out/$(CONFIGNAME)/src -linet -DINET_IMPORT -KINET_PROJ=../inet
#

# Name of target to be created (-o option)
TARGET = http$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I/usr/include/c++/4.6.3 \
    -I/usr/include \
    -I../inet/src/linklayer/ethernet \
    -I../inet/src/linklayer/ieee80211/radio \
    -I../inet/src/networklayer/common \
    -I../inet/src/networklayer/icmpv6 \
    -I../inet/src \
    -I../inet/src/world/obstacles \
    -I../inet/src/networklayer/xmipv6 \
    -I../inet/src/networklayer/manetrouting/base \
    -I../inet/src/mobility \
    -I../inet/src/networklayer/contract \
    -I../inet/src/linklayer/ieee80211/mgmt \
    -I../inet/src/applications/httptools \
    -I../inet/src/util \
    -I../inet/src/transport/contract \
    -I../inet/src/linklayer/radio/propagation \
    -I../inet/src/linklayer/ieee80211/radio/errormodel \
    -I../inet/src/linklayer/radio \
    -I../inet/src/world/powercontrol \
    -I../inet/src/util/headerserializers/tcp \
    -I../inet/src/networklayer/ipv4 \
    -I../inet/src/util/headerserializers/ipv4 \
    -I../inet/src/base \
    -I../inet/src/world/httptools \
    -I../inet/src/util/headerserializers \
    -I../inet/src/world/radio \
    -I../inet/src/linklayer/ieee80211/mac \
    -I../inet/src/networklayer/ipv6 \
    -I../inet/src/transport/sctp \
    -I../inet/src/util/headerserializers/udp \
    -I../inet/src/networklayer/ipv6tunneling \
    -I../inet/src/applications/pingapp \
    -I../inet/src/battery/models \
    -I../inet/src/util/headerserializers/sctp \
    -I../inet/src/linklayer/contract \
    -I../inet/src/networklayer/arp \
    -I../inet/src/transport/tcp_common \
    -I../inet/src/transport/udp \
    -I. \
    -I3rdparty \
    -I3rdparty/http-nf \
    -I3rdparty/http-parser \
    -I3rdparty/spdy4_headers_sample \
    -I3rdparty/spdylay_lib \
    -I3rdparty/spdylay_lib/includes \
    -I3rdparty/spdylay_lib/includes/spdylay \
    -Ihar \
    -Ihar/mnot \
    -Ihar/results \
    -Imodel \
    -Imodel/multiradio \
    -Imodel/results \
    -Imodel/results1 \
    -Imodel/results2 \
    -Imodel/results3 \
    -Ipairsocket \
    -Isites \
    -Isites/music_10086_cn \
    -Isites/www_10086_com \
    -Isites/www_google_com_hk \
    -Isites/www_sina_com_cn \
    -Isrc \
    -Isrc/Browser \
    -Isrc/Browser/Strategy \
    -Isrc/Browser/Strategy/PipeReqMode \
    -Isrc/Browser/Strategy/SvrSupportDetectMethod \
    -Isrc/Server \
    -Isrc/protocolUtils \
    -Iwired \
    -Iwired/results

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L../inet/out/$(CONFIGNAME)/src  -linet
LIBS += -Wl,-rpath,`abspath ../inet/out/$(CONFIGNAME)/src`

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/3rdparty/http-parser/http_parser.o \
    $O/3rdparty/spdy4_headers_sample/header_freq_tables.o \
    $O/3rdparty/spdy4_headers_sample/spdy4_headers_codec.o \
    $O/3rdparty/spdy4_headers_sample/spdy4_headers_sample.o \
    $O/3rdparty/spdy4_headers_sample/utils.o \
    $O/3rdparty/spdylay_lib/spdylay_buffer.o \
    $O/3rdparty/spdylay_lib/spdylay_zlib.o \
    $O/3rdparty/spdylay_lib/spdylay_pq.o \
    $O/3rdparty/spdylay_lib/spdylay_outbound_item.o \
    $O/3rdparty/spdylay_lib/spdylay_queue.o \
    $O/3rdparty/spdylay_lib/spdylay_npn.o \
    $O/3rdparty/spdylay_lib/spdylay_frame.o \
    $O/3rdparty/spdylay_lib/spdylay_gzip.o \
    $O/3rdparty/spdylay_lib/spdylay_stream.o \
    $O/3rdparty/spdylay_lib/spdylay_helper.o \
    $O/3rdparty/spdylay_lib/spdylay_submit.o \
    $O/3rdparty/spdylay_lib/spdylay_session.o \
    $O/3rdparty/spdylay_lib/spdylay_client_cert_vector.o \
    $O/3rdparty/spdylay_lib/spdylay_map.o \
    $O/src/Browser/HarBrowser.o \
    $O/src/Browser/NSCHttpBrowser.o \
    $O/src/Browser/PipeHttpBrowser.o \
    $O/src/Browser/Strategy/PipeReqMode/CPipeReqBase.o \
    $O/src/Browser/Strategy/PipeReqMode/CDFPipeReq.o \
    $O/src/Browser/Strategy/PipeReqMode/CFFPipeReq.o \
    $O/src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.o \
    $O/src/Browser/Strategy/SvrSupportDetectMethod/CPCSvrSupportDetect.o \
    $O/src/Browser/Strategy/SvrSupportDetectMethod/CPHSvrSupportDetect.o \
    $O/src/Server/HarServer.o \
    $O/src/Server/PipeHttpServer.o \
    $O/src/Server/NSCHttpServer.o \
    $O/src/protocolUtils/HarParser.o \
    $O/src/protocolUtils/HttpRequestPraser.o \
    $O/src/protocolUtils/HttpResponsePraser.o \
    $O/src/protocolUtils/httpProtocol.o \
    $O/src/protocolUtils/realHttpMessages_m.o

# Message files
MSGFILES = \
    src/protocolUtils/realHttpMessages.msg

# Other makefile variables (-K)
INET_PROJ=../inet

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) -DINET_IMPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# inserted from file 'makefrag':
#$O/%.o: %.cc $(COPTS_FILE)
#	@$(MKPATH) $(dir $@)
#	$(CXX) -c $(COPTS)  -std=c++0x -o $@ $<
#STD        = -std=c++0x
#CFLAGS =  -std=c++0x

DEBUGFLAGS = -DDEBUG -g -D_GLIBCXX_DEBUG
OPTFLAGS   = -O3 -DNDEBUG
#WARN       = -Wall -Werror
STD        = -std=c++0x
CFLAGS   = $(DEBUG) $(OPT) $(WARN) $(STD)

ifndef DEBUG
OPT = $(OPTFLAGS)
DEBUG = -g
else
OPT =
DEBUG = $(DEBUGFLAGS)
endif

# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f http http.exe libhttp.so libhttp.a libhttp.dll libhttp.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f 3rdparty/*_m.cc 3rdparty/*_m.h
	-rm -f 3rdparty/http-nf/*_m.cc 3rdparty/http-nf/*_m.h
	-rm -f 3rdparty/http-parser/*_m.cc 3rdparty/http-parser/*_m.h
	-rm -f 3rdparty/spdy4_headers_sample/*_m.cc 3rdparty/spdy4_headers_sample/*_m.h
	-rm -f 3rdparty/spdylay_lib/*_m.cc 3rdparty/spdylay_lib/*_m.h
	-rm -f 3rdparty/spdylay_lib/includes/*_m.cc 3rdparty/spdylay_lib/includes/*_m.h
	-rm -f 3rdparty/spdylay_lib/includes/spdylay/*_m.cc 3rdparty/spdylay_lib/includes/spdylay/*_m.h
	-rm -f har/*_m.cc har/*_m.h
	-rm -f har/mnot/*_m.cc har/mnot/*_m.h
	-rm -f har/results/*_m.cc har/results/*_m.h
	-rm -f model/*_m.cc model/*_m.h
	-rm -f model/multiradio/*_m.cc model/multiradio/*_m.h
	-rm -f model/results/*_m.cc model/results/*_m.h
	-rm -f model/results1/*_m.cc model/results1/*_m.h
	-rm -f model/results2/*_m.cc model/results2/*_m.h
	-rm -f model/results3/*_m.cc model/results3/*_m.h
	-rm -f pairsocket/*_m.cc pairsocket/*_m.h
	-rm -f sites/*_m.cc sites/*_m.h
	-rm -f sites/music_10086_cn/*_m.cc sites/music_10086_cn/*_m.h
	-rm -f sites/www_10086_com/*_m.cc sites/www_10086_com/*_m.h
	-rm -f sites/www_google_com_hk/*_m.cc sites/www_google_com_hk/*_m.h
	-rm -f sites/www_sina_com_cn/*_m.cc sites/www_sina_com_cn/*_m.h
	-rm -f src/*_m.cc src/*_m.h
	-rm -f src/Browser/*_m.cc src/Browser/*_m.h
	-rm -f src/Browser/Strategy/*_m.cc src/Browser/Strategy/*_m.h
	-rm -f src/Browser/Strategy/PipeReqMode/*_m.cc src/Browser/Strategy/PipeReqMode/*_m.h
	-rm -f src/Browser/Strategy/SvrSupportDetectMethod/*_m.cc src/Browser/Strategy/SvrSupportDetectMethod/*_m.h
	-rm -f src/Server/*_m.cc src/Server/*_m.h
	-rm -f src/protocolUtils/*_m.cc src/protocolUtils/*_m.h
	-rm -f wired/*_m.cc wired/*_m.h
	-rm -f wired/results/*_m.cc wired/results/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc 3rdparty/*.cc 3rdparty/http-nf/*.cc 3rdparty/http-parser/*.cc 3rdparty/spdy4_headers_sample/*.cc 3rdparty/spdylay_lib/*.cc 3rdparty/spdylay_lib/includes/*.cc 3rdparty/spdylay_lib/includes/spdylay/*.cc har/*.cc har/mnot/*.cc har/results/*.cc model/*.cc model/multiradio/*.cc model/results/*.cc model/results1/*.cc model/results2/*.cc model/results3/*.cc pairsocket/*.cc sites/*.cc sites/music_10086_cn/*.cc sites/www_10086_com/*.cc sites/www_google_com_hk/*.cc sites/www_sina_com_cn/*.cc src/*.cc src/Browser/*.cc src/Browser/Strategy/*.cc src/Browser/Strategy/PipeReqMode/*.cc src/Browser/Strategy/SvrSupportDetectMethod/*.cc src/Server/*.cc src/protocolUtils/*.cc wired/*.cc wired/results/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/3rdparty/http-parser/http_parser.o: 3rdparty/http-parser/http_parser.cc \
	3rdparty/http-parser/http_parser.h
$O/3rdparty/spdy4_headers_sample/header_freq_tables.o: 3rdparty/spdy4_headers_sample/header_freq_tables.cc \
	3rdparty/spdy4_headers_sample/header_freq_tables.h
$O/3rdparty/spdy4_headers_sample/spdy4_headers_codec.o: 3rdparty/spdy4_headers_sample/spdy4_headers_codec.cc \
	3rdparty/spdy4_headers_sample/bit_bucket.h \
	3rdparty/spdy4_headers_sample/header_freq_tables.h \
	3rdparty/spdy4_headers_sample/huffman.h \
	3rdparty/spdy4_headers_sample/pretty_print_tree.h \
	3rdparty/spdy4_headers_sample/spdy4_headers_codec.h \
	3rdparty/spdy4_headers_sample/trivial_http_parse.h \
	3rdparty/spdy4_headers_sample/utils.h
$O/3rdparty/spdy4_headers_sample/spdy4_headers_sample.o: 3rdparty/spdy4_headers_sample/spdy4_headers_sample.cc
$O/3rdparty/spdy4_headers_sample/utils.o: 3rdparty/spdy4_headers_sample/utils.cc \
	3rdparty/spdy4_headers_sample/utils.h
$O/3rdparty/spdylay_lib/spdylay_buffer.o: 3rdparty/spdylay_lib/spdylay_buffer.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_helper.h \
	3rdparty/spdylay_lib/spdylay_net.h
$O/3rdparty/spdylay_lib/spdylay_client_cert_vector.o: 3rdparty/spdylay_lib/spdylay_client_cert_vector.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_client_cert_vector.h \
	3rdparty/spdylay_lib/spdylay_helper.h
$O/3rdparty/spdylay_lib/spdylay_frame.o: 3rdparty/spdylay_lib/spdylay_frame.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_client_cert_vector.h \
	3rdparty/spdylay_lib/spdylay_frame.h \
	3rdparty/spdylay_lib/spdylay_helper.h \
	3rdparty/spdylay_lib/spdylay_net.h \
	3rdparty/spdylay_lib/spdylay_zlib.h
$O/3rdparty/spdylay_lib/spdylay_gzip.o: 3rdparty/spdylay_lib/spdylay_gzip.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_gzip.h
$O/3rdparty/spdylay_lib/spdylay_helper.o: 3rdparty/spdylay_lib/spdylay_helper.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_helper.h \
	3rdparty/spdylay_lib/spdylay_net.h
$O/3rdparty/spdylay_lib/spdylay_map.o: 3rdparty/spdylay_lib/spdylay_map.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_int.h \
	3rdparty/spdylay_lib/spdylay_map.h
$O/3rdparty/spdylay_lib/spdylay_npn.o: 3rdparty/spdylay_lib/spdylay_npn.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_npn.h
$O/3rdparty/spdylay_lib/spdylay_outbound_item.o: 3rdparty/spdylay_lib/spdylay_outbound_item.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_client_cert_vector.h \
	3rdparty/spdylay_lib/spdylay_frame.h \
	3rdparty/spdylay_lib/spdylay_outbound_item.h \
	3rdparty/spdylay_lib/spdylay_zlib.h
$O/3rdparty/spdylay_lib/spdylay_pq.o: 3rdparty/spdylay_lib/spdylay_pq.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_int.h \
	3rdparty/spdylay_lib/spdylay_pq.h
$O/3rdparty/spdylay_lib/spdylay_queue.o: 3rdparty/spdylay_lib/spdylay_queue.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_queue.h
$O/3rdparty/spdylay_lib/spdylay_session.o: 3rdparty/spdylay_lib/spdylay_session.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_client_cert_vector.h \
	3rdparty/spdylay_lib/spdylay_frame.h \
	3rdparty/spdylay_lib/spdylay_helper.h \
	3rdparty/spdylay_lib/spdylay_int.h \
	3rdparty/spdylay_lib/spdylay_map.h \
	3rdparty/spdylay_lib/spdylay_net.h \
	3rdparty/spdylay_lib/spdylay_outbound_item.h \
	3rdparty/spdylay_lib/spdylay_pq.h \
	3rdparty/spdylay_lib/spdylay_session.h \
	3rdparty/spdylay_lib/spdylay_stream.h \
	3rdparty/spdylay_lib/spdylay_zlib.h
$O/3rdparty/spdylay_lib/spdylay_stream.o: 3rdparty/spdylay_lib/spdylay_stream.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_client_cert_vector.h \
	3rdparty/spdylay_lib/spdylay_frame.h \
	3rdparty/spdylay_lib/spdylay_int.h \
	3rdparty/spdylay_lib/spdylay_map.h \
	3rdparty/spdylay_lib/spdylay_outbound_item.h \
	3rdparty/spdylay_lib/spdylay_stream.h \
	3rdparty/spdylay_lib/spdylay_zlib.h
$O/3rdparty/spdylay_lib/spdylay_submit.o: 3rdparty/spdylay_lib/spdylay_submit.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_client_cert_vector.h \
	3rdparty/spdylay_lib/spdylay_frame.h \
	3rdparty/spdylay_lib/spdylay_helper.h \
	3rdparty/spdylay_lib/spdylay_int.h \
	3rdparty/spdylay_lib/spdylay_map.h \
	3rdparty/spdylay_lib/spdylay_outbound_item.h \
	3rdparty/spdylay_lib/spdylay_pq.h \
	3rdparty/spdylay_lib/spdylay_session.h \
	3rdparty/spdylay_lib/spdylay_stream.h \
	3rdparty/spdylay_lib/spdylay_submit.h \
	3rdparty/spdylay_lib/spdylay_zlib.h
$O/3rdparty/spdylay_lib/spdylay_zlib.o: 3rdparty/spdylay_lib/spdylay_zlib.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_zlib.h
$O/src/Browser/HarBrowser.o: src/Browser/HarBrowser.cc \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_zlib.h \
	src/Browser/HarBrowser.h \
	src/Browser/NSCHttpBrowser.h \
	src/Browser/Strategy/PipeReqMode/CPipeReqBase.h \
	src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.h \
	src/Browser/TypeDef.h \
	src/protocolUtils/HarParser.h \
	src/protocolUtils/ProtocolTypeDef.h \
	src/protocolUtils/httpProtocol.h \
	src/protocolUtils/realHttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowser.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowserBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Browser/NSCHttpBrowser.o: src/Browser/NSCHttpBrowser.cc \
	3rdparty/http-parser/http_parser.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_zlib.h \
	src/Browser/NSCHttpBrowser.h \
	src/Browser/Strategy/PipeReqMode/CDFPipeReq.h \
	src/Browser/Strategy/PipeReqMode/CFFPipeReq.h \
	src/Browser/Strategy/PipeReqMode/CPipeReqBase.h \
	src/Browser/Strategy/SvrSupportDetectMethod/CPCSvrSupportDetect.h \
	src/Browser/Strategy/SvrSupportDetectMethod/CPHSvrSupportDetect.h \
	src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.h \
	src/Browser/TypeDef.h \
	src/protocolUtils/HttpRequestPraser.h \
	src/protocolUtils/HttpResponsePraser.h \
	src/protocolUtils/ProtocolTypeDef.h \
	src/protocolUtils/httpProtocol.h \
	src/protocolUtils/realHttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowser.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowserBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/ByteArray.h \
	$(INET_PROJ)/src/base/ByteArrayMessage.h \
	$(INET_PROJ)/src/base/ByteArrayMessage_m.h \
	$(INET_PROJ)/src/base/ByteArray_m.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Browser/PipeHttpBrowser.o: src/Browser/PipeHttpBrowser.cc \
	src/Browser/PipeHttpBrowser.h \
	src/Browser/Strategy/PipeReqMode/CDFPipeReq.h \
	src/Browser/Strategy/PipeReqMode/CFFPipeReq.h \
	src/Browser/Strategy/PipeReqMode/CPipeReqBase.h \
	src/Browser/Strategy/SvrSupportDetectMethod/CPCSvrSupportDetect.h \
	src/Browser/Strategy/SvrSupportDetectMethod/CPHSvrSupportDetect.h \
	src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.h \
	src/Browser/TypeDef.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowser.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowserBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Browser/Strategy/PipeReqMode/CDFPipeReq.o: src/Browser/Strategy/PipeReqMode/CDFPipeReq.cc \
	src/Browser/Strategy/PipeReqMode/CDFPipeReq.h \
	src/Browser/Strategy/PipeReqMode/CPipeReqBase.h \
	src/Browser/TypeDef.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowser.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowserBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Browser/Strategy/PipeReqMode/CFFPipeReq.o: src/Browser/Strategy/PipeReqMode/CFFPipeReq.cc \
	src/Browser/Strategy/PipeReqMode/CFFPipeReq.h \
	src/Browser/Strategy/PipeReqMode/CPipeReqBase.h \
	src/Browser/TypeDef.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowser.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowserBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Browser/Strategy/PipeReqMode/CPipeReqBase.o: src/Browser/Strategy/PipeReqMode/CPipeReqBase.cc \
	src/Browser/Strategy/PipeReqMode/CPipeReqBase.h \
	src/Browser/TypeDef.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowser.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowserBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Browser/Strategy/SvrSupportDetectMethod/CPCSvrSupportDetect.o: src/Browser/Strategy/SvrSupportDetectMethod/CPCSvrSupportDetect.cc \
	src/Browser/Strategy/SvrSupportDetectMethod/CPCSvrSupportDetect.h \
	src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.h \
	src/Browser/TypeDef.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowser.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowserBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Browser/Strategy/SvrSupportDetectMethod/CPHSvrSupportDetect.o: src/Browser/Strategy/SvrSupportDetectMethod/CPHSvrSupportDetect.cc \
	src/Browser/Strategy/SvrSupportDetectMethod/CPHSvrSupportDetect.h \
	src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.h \
	src/Browser/TypeDef.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowser.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowserBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.o: src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.cc \
	src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.h \
	src/Browser/TypeDef.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowser.h \
	$(INET_PROJ)/src/applications/httptools/HttpBrowserBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Server/HarServer.o: src/Server/HarServer.cc \
	3rdparty/http-parser/http_parser.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_zlib.h \
	src/Server/HarServer.h \
	src/Server/NSCHttpServer.h \
	src/protocolUtils/HarParser.h \
	src/protocolUtils/HttpRequestPraser.h \
	src/protocolUtils/ProtocolTypeDef.h \
	src/protocolUtils/httpProtocol.h \
	src/protocolUtils/realHttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpServer.h \
	$(INET_PROJ)/src/applications/httptools/HttpServerBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/ByteArray.h \
	$(INET_PROJ)/src/base/ByteArrayMessage.h \
	$(INET_PROJ)/src/base/ByteArrayMessage_m.h \
	$(INET_PROJ)/src/base/ByteArray_m.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Server/NSCHttpServer.o: src/Server/NSCHttpServer.cc \
	3rdparty/http-parser/http_parser.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_zlib.h \
	src/Server/NSCHttpServer.h \
	src/protocolUtils/HttpRequestPraser.h \
	src/protocolUtils/ProtocolTypeDef.h \
	src/protocolUtils/httpProtocol.h \
	src/protocolUtils/realHttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpServer.h \
	$(INET_PROJ)/src/applications/httptools/HttpServerBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/ByteArray.h \
	$(INET_PROJ)/src/base/ByteArrayMessage.h \
	$(INET_PROJ)/src/base/ByteArrayMessage_m.h \
	$(INET_PROJ)/src/base/ByteArray_m.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/Server/PipeHttpServer.o: src/Server/PipeHttpServer.cc \
	src/Server/PipeHttpServer.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpServer.h \
	$(INET_PROJ)/src/applications/httptools/HttpServerBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/transport/contract/TCPCommand_m.h \
	$(INET_PROJ)/src/transport/contract/TCPSocket.h \
	$(INET_PROJ)/src/transport/contract/TCPSocketMap.h \
	$(INET_PROJ)/src/util/uint128.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/protocolUtils/HarParser.o: src/protocolUtils/HarParser.cc \
	src/protocolUtils/HarParser.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h
$O/src/protocolUtils/HttpRequestPraser.o: src/protocolUtils/HttpRequestPraser.cc \
	3rdparty/http-parser/http_parser.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_zlib.h \
	src/protocolUtils/HttpRequestPraser.h \
	src/protocolUtils/ProtocolTypeDef.h \
	src/protocolUtils/httpProtocol.h \
	src/protocolUtils/realHttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/ByteArray.h \
	$(INET_PROJ)/src/base/ByteArrayMessage.h \
	$(INET_PROJ)/src/base/ByteArrayMessage_m.h \
	$(INET_PROJ)/src/base/ByteArray_m.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/protocolUtils/HttpResponsePraser.o: src/protocolUtils/HttpResponsePraser.cc \
	3rdparty/http-parser/http_parser.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylay.h \
	3rdparty/spdylay_lib/includes/spdylay/spdylayver.h \
	3rdparty/spdylay_lib/spdylay_buffer.h \
	3rdparty/spdylay_lib/spdylay_zlib.h \
	src/protocolUtils/HttpResponsePraser.h \
	src/protocolUtils/ProtocolTypeDef.h \
	src/protocolUtils/httpProtocol.h \
	src/protocolUtils/realHttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpEventMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpLogdefs.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpNodeBase.h \
	$(INET_PROJ)/src/applications/httptools/HttpRandom.h \
	$(INET_PROJ)/src/applications/httptools/HttpUtils.h \
	$(INET_PROJ)/src/base/ByteArray.h \
	$(INET_PROJ)/src/base/ByteArrayMessage.h \
	$(INET_PROJ)/src/base/ByteArrayMessage_m.h \
	$(INET_PROJ)/src/base/ByteArray_m.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/world/httptools/HttpController.h
$O/src/protocolUtils/httpProtocol.o: src/protocolUtils/httpProtocol.cc \
	src/protocolUtils/httpProtocol.h \
	src/protocolUtils/realHttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h
$O/src/protocolUtils/realHttpMessages_m.o: src/protocolUtils/realHttpMessages_m.cc \
	src/protocolUtils/realHttpMessages_m.h \
	$(INET_PROJ)/src/applications/httptools/HttpMessages_m.h

