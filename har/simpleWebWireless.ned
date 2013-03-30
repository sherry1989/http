//
// Pair scenario file for the sockets pair example.
//



package http.har;

import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;
import inet.nodes.inet.Router;
import inet.nodes.inet.StandardHost;
import inet.nodes.inet.WirelessHost;
import inet.nodes.wireless.AccessPoint;
import inet.world.radio.ChannelControl;
import inet.mobility.models.StationaryMobility;
import inet.world.httptools.HttpController;
import http.src.Browser.PipeHttpBrowser;
import http.src.Browser.NSCHttpBrowser;
import inet.applications.httptools.HttpBrowser;
import http.src.Server.PipeHttpServer;
import http.src.Server.NSCHttpServer;
import inet.applications.httptools.HttpServer;

network simpleWebWireless
{
    parameters:
        @display("bgb=474,352");
        int serverNum;
        int clientNum;
        bool useEthernetSwitching = default(true);

    types:
        channel configLine extends ned.DatarateChannel
        {
            parameters:
                delay = 0.1us;
                datarate = 10Mbps;
                per = 0;
        }

    submodules:
        iPv4NetworkConfigurator: IPv4NetworkConfigurator {
            parameters:
                //                moduleTypes = "StandardHost";
                //                nonIPModuleTypes = "";
                //                config = xml("<config><interface hosts='*' address='145.236.x.x' netmask='255.255.0.0'/></config>");
//                config=xml("<config><interface hosts='*' address='145.236.x.x' netmask='255.255.0.0'/></config>");
                @display("i=block/cogwheel;p=409,33");
        }

        channelControl: ChannelControl {
            @display("p=53,32");
            numChannels = 2;
        }

        accessPoint: AccessPoint {
            @display("p=195,200");
            wlan[*].mgmtType = "Ieee80211MgmtAPSimplified";
        }

        router: Router {
            @display("p=356,137");
        }

        controller: HttpController {
            parameters:
                @display("p=234,33;i=block/cogwheel");
        }
        srv[serverNum]: StandardHost {
            parameters:
                @display("p=422,280;i=device/server_l");
        }
        cli[clientNum]: WirelessHost {
            parameters:
                @display("p=72,295;i=device/laptop_l");
                wlan[*].mgmtType = "Ieee80211MgmtSTASimplified";
        }

    connections:
        accessPoint.ethg++ <--> configLine <--> router.ethg++ if useEthernetSwitching;

        for i=0..(serverNum-1) {
            router.ethg++ <--> configLine <--> srv[i].ethg++;
        }
}

