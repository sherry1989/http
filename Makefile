#
# OMNeT++/OMNEST Makefile for http
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out -I../inet/src/linklayer/ethernet -I../inet/src/linklayer/ieee80211/radio -I../inet/src/networklayer/common -I../inet/src -I../inet/src/networklayer/icmpv6 -I../inet/src/world/obstacles -I../inet/src/networklayer/queue -I../inet/src/networklayer/manetrouting/base -I../inet/src/mobility -I../inet/src/networklayer/contract -I../inet/src/networklayer/xmipv6 -I../inet/src/linklayer/ieee80211/mgmt -I../inet/src/applications/httptools -I../inet/src/util -I../inet/src/transport/contract -I../inet/src/linklayer/radio/propagation -I../inet/src/linklayer/ieee80211/radio/errormodel -I../inet/src/world/powercontrol -I../inet/src/linklayer/radio -I../inet/src/util/headerserializers/tcp -I../inet/src/networklayer/ipv4 -I../inet/src/util/headerserializers/ipv4 -I../inet/src/base -I../inet/src/world/httptools -I../inet/src/util/headerserializers -I../inet/src/world/radio -I../inet/src/linklayer/ieee80211/mac -I../inet/src/networklayer/ipv6 -I../inet/src/transport/sctp -I../inet/src/util/headerserializers/udp -I../inet/src/networklayer/ipv6tunneling -I../inet/src/battery/models -I../inet/src/applications/pingapp -I../inet/src/linklayer/contract -I../inet/src/util/headerserializers/sctp -I../inet/src/networklayer/arp -I../inet/src/transport/tcp_common -I../inet/src/transport/udp -L../inet/out/$(CONFIGNAME)/src -linet -DINET_IMPORT -KINET_PROJ=../inet
#

# Name of target to be created (-o option)
TARGET = http$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I../inet/src/linklayer/ethernet \
    -I../inet/src/linklayer/ieee80211/radio \
    -I../inet/src/networklayer/common \
    -I../inet/src \
    -I../inet/src/networklayer/icmpv6 \
    -I../inet/src/world/obstacles \
    -I../inet/src/networklayer/queue \
    -I../inet/src/networklayer/manetrouting/base \
    -I../inet/src/mobility \
    -I../inet/src/networklayer/contract \
    -I../inet/src/networklayer/xmipv6 \
    -I../inet/src/linklayer/ieee80211/mgmt \
    -I../inet/src/applications/httptools \
    -I../inet/src/util \
    -I../inet/src/transport/contract \
    -I../inet/src/linklayer/radio/propagation \
    -I../inet/src/linklayer/ieee80211/radio/errormodel \
    -I../inet/src/world/powercontrol \
    -I../inet/src/linklayer/radio \
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
    -I../inet/src/battery/models \
    -I../inet/src/applications/pingapp \
    -I../inet/src/linklayer/contract \
    -I../inet/src/util/headerserializers/sctp \
    -I../inet/src/networklayer/arp \
    -I../inet/src/transport/tcp_common \
    -I../inet/src/transport/udp \
    -I. \
    -Ipairsocket \
    -Ipairsocket/results \
    -Isrc \
    -Isrc/Browser \
    -Isrc/Browser/Strategy \
    -Isrc/Browser/Strategy/PipeReqMode \
    -Isrc/Browser/Strategy/SvrSupportDetectMethod

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
    $O/src/Browser/PipeHttpBrowser.o \
    $O/src/Browser/Strategy/PipeReqMode/CDFPipeReq.o \
    $O/src/Browser/Strategy/PipeReqMode/CFFPipeReq.o \
    $O/src/Browser/Strategy/PipeReqMode/CPipeReqBase.o \
    $O/src/Browser/Strategy/SvrSupportDetectMethod/CPCSvrSupportDetect.o \
    $O/src/Browser/Strategy/SvrSupportDetectMethod/CPHSvrSupportDetect.o \
    $O/src/Browser/Strategy/SvrSupportDetectMethod/CSvrSupportDetectBase.o

# Message files
MSGFILES =

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
	-rm -f pairsocket/*_m.cc pairsocket/*_m.h
	-rm -f pairsocket/results/*_m.cc pairsocket/results/*_m.h
	-rm -f src/*_m.cc src/*_m.h
	-rm -f src/Browser/*_m.cc src/Browser/*_m.h
	-rm -f src/Browser/Strategy/*_m.cc src/Browser/Strategy/*_m.h
	-rm -f src/Browser/Strategy/PipeReqMode/*_m.cc src/Browser/Strategy/PipeReqMode/*_m.h
	-rm -f src/Browser/Strategy/SvrSupportDetectMethod/*_m.cc src/Browser/Strategy/SvrSupportDetectMethod/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc pairsocket/*.cc pairsocket/results/*.cc src/*.cc src/Browser/*.cc src/Browser/Strategy/*.cc src/Browser/Strategy/PipeReqMode/*.cc src/Browser/Strategy/SvrSupportDetectMethod/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
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
	src/Browser/Strategy/PipeReqMode/CPipeReqBase.h
$O/src/Browser/Strategy/PipeReqMode/CFFPipeReq.o: src/Browser/Strategy/PipeReqMode/CFFPipeReq.cc \
	src/Browser/Strategy/PipeReqMode/CFFPipeReq.h \
	src/Browser/Strategy/PipeReqMode/CPipeReqBase.h
$O/src/Browser/Strategy/PipeReqMode/CPipeReqBase.o: src/Browser/Strategy/PipeReqMode/CPipeReqBase.cc \
	src/Browser/Strategy/PipeReqMode/CPipeReqBase.h
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

