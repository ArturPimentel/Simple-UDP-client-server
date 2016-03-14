Code created by Artur Pimentel de Oliveira
netid: apiment2
Based in codes from Prof. Dong Wang lectures

After compiling UDPserver.c to a executable called UDPserver, use the following command to run the program (in Linux):

	$ ./UDPserver 41033

In which the number 41033 may be changed, but it is my port number

Server can be quitted normally in the command prompt by using the Ctrl+C key
----------------------------------------------------------------------------
After compiling UDPclient.c to a executable called UDPclient, use the following command to run the program (in Linux):

	$ ./UDPclient [server host name] 41033 [message]

This UDP client talks to the UDP server, and has the following features: it tells regular strings from .txt file names, it sends messages and decrypt them from server, calculating RTT for doing that.