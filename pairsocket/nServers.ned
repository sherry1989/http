//
// Pair scenario file for the sockets pair example.
//



package http.pairsocket;

import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;
import inet.nodes.inet.Router;
import inet.nodes.inet.StandardHost;
import inet.world.httptools.HttpController;


network nServers
{
    parameters:
        @display("bgb=474,374");
        int serverNum;
    types:
        channel gigabitline extends ned.DatarateChannel
        {
            parameters:
                delay = 0.1us;
                datarate = 1000Mbps;
        }
    submodules:
        configurator: IPv4NetworkConfigurator {
            parameters:
                //                moduleTypes = "StandardHost";
                //                nonIPModuleTypes = "";
//                config = xml("<config><interface hosts='*' address='145.236.x.x' netmask='255.255.0.0'/></config>");
                @display("i=block/cogwheel");
        }
        controller: HttpController {
            parameters:
                @display("p=62,60;i=block/cogwheel");
        }
        router: Router {
            parameters:
                @display("p=236,294;i=abstract/router_l");
        }
        srv[serverNum]: StandardHost {
            parameters:
                @display("p=89,188;i=device/server_l");
        }
        cli: StandardHost {
            parameters:
                @display("p=316,188;i=device/laptop_l");
        }
    connections:
        for i=0..(serverNum-1) {
            router.ethg++ <--> gigabitline <--> srv[i].ethg++;
        }
        cli.ethg++ <--> gigabitline <--> router.ethg++;

}

