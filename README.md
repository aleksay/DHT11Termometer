# DHT11Termometer

This code let you manage a DHT11 humidity and termometer sensor connected to your raspberry pi like soc board as a systemd service that log into your syslog system.

It's compatible with any system with a running WiringPI version installed, we are going to use -lwiringPi, -lwiringPiDev  and -lpthread compile options, check your systems



Installation:

the Makefile offers the targets "deb" and "deb-install" that will respectively create and install a .deb package with a fresh compiled copy of the program.
After the installation is performed the service dht1temperature is automatically started.




