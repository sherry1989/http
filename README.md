# http simulation #


## Introduction ##

----------

This is a HTTP protocol simulation to research the HTTP/1.1 protocol improvement.

### Tools ###

* **OMNeT++ 4.2** as the simulation tools
* **INET Framework 2.0** to give the support of the 4 layer and below Internet protocl simulation
* **http-tools** in INET Framework to start up the browser-server simulation framework
* **TCP-NSC(Network simulation Cradle)** to simulate the real TCP stack instead of the TCP stack in INET Framework using the old TCP congestion control algorithms
* **har file type** to get the real website info as one of the simulation input

### Features ###

Till now, this simulation concentrates on the following improvement of HTTP/1.1:  
 
* **http pipelining** introduced by HTTP/1.1 but not in common use
* **http header compression** introduced by HTTP/2.0