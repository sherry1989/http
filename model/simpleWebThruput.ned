//
// Pair scenario file for the sockets pair example.
//



package http.model;

import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;
import inet.nodes.inet.Router;
import inet.nodes.inet.StandardHost;
import inet.world.httptools.HttpController;
import http.src.protocolUtils.MessageInfoSrc.MsgInfoSrcBase;
import http.src.protocolUtils.MessageInfoSrc.HarParser;
import http.src.protocolUtils.MessageInfoSrc.SelfGen;
import http.src.Browser.PipeHttpBrowser;
import http.src.Browser.NSCHttpBrowser;
import inet.applications.httptools.HttpBrowser;
import http.src.Server.PipeHttpServer;
import http.src.Server.NSCHttpServer;
import inet.applications.httptools.HttpServer;
import inet.util.ThruputMeteringChannel;
import inet.util.ThruputMeter;
import ned.IdealChannel;

network simpleWebThruput
{
    parameters:
        @display("bgb=1018,374");
        int serverNum;
        int clientNum;
    types:
        channel userUpLine extends ThruputMeteringChannel
        {
            parameters:
                delay = 80ms;
                datarate = 0.5Mbps;
                per = 0;
                thruputDisplayFormat = "b (u)";
        }
        channel userDownLine extends ThruputMeteringChannel
        {
            parameters:
                delay = 80ms;
                datarate = 1.45Mbps;
                per = 0;
                thruputDisplayFormat = "b (u)";
        }
        channel serverLine extends ThruputMeteringChannel
        {
            parameters:
                delay = 0.1us;
                datarate = 100Mbps;
                per = 0;
                thruputDisplayFormat = "b (u)";
        }
        channel idealLine extends ThruputMeteringChannel
        {
            parameters:
                delay = 0s;
                datarate = 100Gbps;
                per = 0;
                thruputDisplayFormat = "b (u)";
        }
    submodules:
        configurator: IPv4NetworkConfigurator {
            parameters:
                //                moduleTypes = "StandardHost";
                //                nonIPModuleTypes = "";
                //                config = xml("<config><interface hosts='*' address='145.236.x.x' netmask='255.255.0.0'/></config>");
                @display("i=block/cogwheel;p=596,46");
        }
        controller: HttpController {
            parameters:
                @display("p=417,46;i=block/cogwheel");
        }
//        harParser: HarParser {
//            parameters:
//                @display("p=214,46;i=block/cogwheel");
//        }
        selfGen: SelfGen {
            parameters:
                @display("p=88,46;i=block/cogwheel");
        }

        router: Router {
            parameters:
                @display("p=508,226;i=abstract/router_l");
        }
        srv[serverNum]: StandardHost {
            parameters:
                @display("p=64,226;i=device/server_l");
        }
        srvUpTh[serverNum]: ThruputMeter {
            parameters:
                @display("p=291,132");
        }
        srvDownTh[serverNum]: ThruputMeter {
            parameters:
                @display("p=291,328");
        }
        cli[clientNum]: StandardHost {
            parameters:
                @display("p=948,226;i=device/laptop_l");
        }
        cliUpTh[clientNum]: ThruputMeter {
            parameters:
                @display("p=722,132");
        }
        cliDownTh[clientNum]: ThruputMeter {
            parameters:
                @display("p=722,328");
        }
    connections:
        for i=0..(serverNum-1) {
            srv[i].pppg$o++ --> serverLine --> srvUpTh[i].in;
            srvUpTh[i].out --> idealLine --> router.pppg$i++;
            srv[i].pppg$i++ <-- idealLine <-- srvDownTh[i].out;
            srvDownTh[i].in <-- serverLine <-- router.pppg$o++;
        }
        for i=0..(clientNum-1) {
            cli[i].pppg$o++ --> userUpLine --> cliUpTh[i].in;
            cliUpTh[i].out --> idealLine --> router.pppg$i++;
            cli[i].pppg$i++ <-- idealLine <-- cliDownTh[i].out;
            cliDownTh[i].in <-- userDownLine <-- router.pppg$o++;
        }
}
