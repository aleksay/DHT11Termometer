VERSION=1.2
DEBIANREVISION=2
ARCH=armhf

default: deb

DHT11Temperature: DHT11Temperature.c 
	gcc -o DHT11Temperature DHT11Temperature.c -lwiringPi -lwiringPiDev -lpthread

install: DHT11Temperature
	cp resources/dht11temperature.service   /etc/systemd/system/
	cp DHT11Temperature /usr/bin/dht11temperature
	systemctl daemon-reload
	systemctl start dht11temperature.service
	systemctl status dht11temperature.service

#uninstall:
#	systemctl stop dht11temperature.service
#	rm /usr/bin/dht11temperature
#	rm /etc/systemd/system/dht11temperature.service
#	systemctl daemon-reload

deb: DHT11Temperature
	mkdir -p dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH)/DEBIAN
	mkdir -p dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH)/usr/bin 
	mkdir -p dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH)/etc/systemd/system
	sed -e "s/Version: .*/Version: $(VERSION)/g" resources/DEBIAN_control > dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH)/DEBIAN/control
	sed -ie "s/Architecture: .*/Architecture: $(ARCH)/g"  dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH)/DEBIAN/control
	cp resources/dht11temperature.service   dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH)/etc/systemd/system/
	cp DHT11Temperature dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH)/usr/bin/dht11temperature
	dpkg-deb --build  dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH)

deb-install: deb
	dpkg -i dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH).deb
	systemctl daemon-reload
	systemctl start dht11temperature.service
	systemctl status dht11temperature.service

deb-uninstall:
	dpkg -r dht11temperature

clean: 
	rm -f DHT11Temperature
	rm -f dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH).deb
	rm -rf dht11temperature_$(VERSION)-$(DEBIANREVISION)_$(ARCH) 

cleanall: deb-uninstall clean

rebuild: cleanall deb-install
