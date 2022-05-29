#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <syslog.h>
#include <unistd.h>
#define MAXTIMINGS	85

int DHTPIN = -1;
int pollingTime = -1;
int dht11_dat[5] = { 0, 0, 0, 0, 0 };
int lastTemp = 0;
int lastHum = 0;


void usage()
{

	printf("Usage: dht11temperature [options] ...\n");
	printf("Options:\n");
	printf("\t-p\t\tPin number, refer to WiringPI library for numeration, default: 7\n");
	printf("\t-n\t\tPolling time in milliseconds, default: 1000\n");
	printf("\t-c CONFIGFILE\tPath for the configuration file, default: \"/etc/dht11temperature/dht11temperature.conf\"\n");
	printf("\t-h\t\tPrint this usage menu\n");
	printf("\n");
	printf("Report bugs to <aleksay@protonmail.com>\n");
}

void parseArgs(int argc, char *argv[])
{

  int option;
  while((option = getopt(argc, argv, "p:c:n:h")) != -1)
  { 
      switch(option)
      {
         case 'p':
		 DHTPIN = atoi(optarg);
		 break;
         case 'n':
		 pollingTime = atoi(optarg);
		 break;
         case 'c': 
            printf("Given File: %s\n", optarg);
            break;
         case 'h':
            usage();
            exit(0);
         case '?': //used for some unknown options
            usage();
            exit(0);
      }
   }
}


int init(int argc, char *argv[]){

// read inline parameters:
// 	-p --pin 
// 	-n --polling-time
// 	-c --config-file	

	parseArgs(argc, argv);


//
//
// look for:
// 	/etc/dht11temperature/dht11temperature.conf
// 	./dht11temperature.conf
// defaults:	
// 	DHTPIN=7
// 	pollingTime=1000

}


void read_dht11_dat()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float	f; 
 
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
 
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	pinMode( DHTPIN, INPUT );
 
	for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );
 
		if ( counter == 255 )
			break;
 
		if ( (i >= 4) && (i % 2 == 0) )
		{
			dht11_dat[j / 8] <<= 1;
			if ( counter > 16 )
				dht11_dat[j / 8] |= 1;
			j++;
		}
	}
 
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
	{
		f = dht11_dat[2] * 9. / 5. + 32;
		if (lastTemp != dht11_dat[2] || lastHum != dht11_dat[0])
		{
			 syslog(LOG_NOTICE, "Humidity = %d.%d %% Temperature = %d.%d C (%.1f F)\n",
				dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
			lastTemp = dht11_dat[2];
			lastHum  = dht11_dat[0];
		}
	}
}
 
int main( int argc, char *argv[] )
{

	init(argc,argv);

	openlog ("DHT11 Temperature Sensor", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
        syslog(LOG_NOTICE, "Sensor starting on pin %d, read rate at %d milliseconds",DHTPIN,pollingTime);

	if ( wiringPiSetup() == -1 )
	{
		syslog(LOG_NOTICE,"WiringPi init errors, quit.");
		closelog();
		exit( 1 );
	}

	if(DHTPIN == -1) 
	{
		syslog(LOG_NOTICE,"Invalid pin number, configuration not loaded, quit.");
		closelog();
		exit (1);
	}

	while ( 1 )
	{
		read_dht11_dat();
		delay( pollingTime ); 
	}
        closelog();
	return(0);
}
