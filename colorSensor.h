#ifndef __COLORSENSOR_H_
#define __COLORSENSOR_H_

// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TCS34725
// This code is designed to work with the TCS34725_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

int ledColorSensor(int state)
{
	if ( wiringPiSetup() == -1 )
		printf("wiringPi error\n");
	pinMode( ledPin, OUTPUT );
	delay(20);
	// LED off
	digitalWrite( ledPin, state );
}

int initColorSensor(int fp)
{
	// Create I2C bus
	int errnum;
	if ((fp = open(bus, O_RDWR)) < 0){
		printf("Failed to open the bus. \n");
		// Error output
		errnum = errno;
		fprintf(stderr, "Value of errno: %d\n", errno);
		perror("Error printed by perror");
		fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
	return -1;
	}
	else {
		printf("I2C-Bus is open. \n");
		// Get I2C device, TCS34725 I2C address is 0x29(41)
		ioctl(fp, I2C_SLAVE, 0x29);
		// Select enable register(0x80)
		// Power ON, RGBC enable, wait time disable(0x03)
		char config[2] = {0};
		config[0] = 0x80;
		config[1] = 0x03;
		write(fp, config, 2);
		// Select ALS time register(0x81)
		// Atime = 700 ms(0x00)
		config[0] = 0x81;
		config[1] = 0x00;
		write(fp, config, 2);
		// Select Wait Time register(0x83)
		// WTIME : 2.4ms(0xFF)
		config[0] = 0x83;
		config[1] = 0xFF;
		write(fp, config, 2);
		// Select control register(0x8F)
		// AGAIN = 1x(0x00)
		config[0] = 0x8F;
		config[1] = 0x00;
		write(fp, config, 2);
		sleep(1);
		return fp;
	}
}

int readColorSensor(int fp, int unixCounter)
{
	// Read 8 bytes of data from register(0x94)
	// cData lsb, cData msb, red lsb, red msb, green lsb, green msb, blue lsb, blue msb
	char reg[1] = {0x94};
	write(fp, reg, 1);
	char data[8] = {0};
	if(read(fp, data, 8) != 8) {
		printf("Erorr : Input/output Erorr \n");
		loop = 0;
		return loop;
	}
	else {
		// Convert the data
		float cData = (data[1] * 256 + data[0]);
		float red   = (data[3] * 256 + data[2]);
		float green = (data[5] * 256 + data[4]);
		float blue  = (data[7] * 256 + data[6]);

		// Calculate luminance
		float luminance = (-0.32466) * (red) + (1.57837) * (green) + (-0.73191) * (blue);
		if(luminance < 0) {
			luminance = 0;
		}

		// Calculate RGB offset for LED
		float red256   = ((roOffset) * red  ) / 10000 *256 * ledOffset;
		ro = red256;
		float green256 = ((grOffset) * green) / 10000 *256 * ledOffset;
		gr = green256;
		float blue256  = ((blOffset) * blue ) / 10000 *256 * ledOffset;
		bl = blue256;

		// Output data to screen
		printf("Red   luminance: %.2f lux red256  : %.2f\n", red,red256);
		printf("Green luminance: %.2f lux green256: %.2f\n", green, green256);
		printf("Blue  luminance: %.2f lux blue256 : %.2f\n", blue, blue256);
		printf("IR    luminance: %.2f lux \n", cData);
		printf("Light luminance: %.2f lux \n", luminance);

		// Watchdog TCS34725
		if (unixCounter >= 60){
			int AktuallTime = time(NULL);
			writeUnixtime(UnixtimeTCS, AktuallTime);
		}
		loop = 1;
	}
	return loop;
}
#endif // __COLORSENSOR_H_
