#ifndef __READCOLOR_H_
#define __READCOLOR_H_

// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TCS34725
// This code is designed to work with the TCS34725_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

int ro;
int gr;
int bl;

void readColor(int unixCounter)
{

	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if ((file = open(bus, O_RDWR)) < 0)
	{
		printf("Failed to open the bus. \n");
		return;
	}
	// Get I2C device, TCS34725 I2C address is 0x29(41)
	ioctl(file, I2C_SLAVE, 0x29);

	// Select enable register(0x80)
	// Power ON, RGBC enable, wait time disable(0x03)
	char config[2] = {0};
	config[0] = 0x80;
	config[1] = 0x03;
	write(file, config, 2);
	// Select ALS time register(0x81)
	// Atime = 700 ms(0x00)
	config[0] = 0x81;
	config[1] = 0x00;
	write(file, config, 2);
	// Select Wait Time register(0x83)
	// WTIME : 2.4ms(0xFF)
	config[0] = 0x83;
	config[1] = 0xFF;
	write(file, config, 2);
	// Select control register(0x8F)
	// AGAIN = 1x(0x00)
	config[0] = 0x8F;
	config[1] = 0x00;
	write(file, config, 2);
	sleep(1);

	// Read 8 bytes of data from register(0x94)
	// cData lsb, cData msb, red lsb, red msb, green lsb, green msb, blue lsb, blue msb
	char reg[1] = {0x94};
	write(file, reg, 1);
	char data[8] = {0};
	if(read(file, data, 8) != 8) {
		printf("Erorr : Input/output Erorr \n");
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

		float red256   = ((1.0) * red  ) / 10000 *256;
		ro = red256;
		float green256 = ((0.6) * green) / 10000 *256;
		gr = green256;
		float blue256  = ((0.3) * blue ) / 10000 *256;
		bl = blue256;

		// Output data to screen
		printf("Red   luminance: %.2f lux red256  : %.2f\n", red,red256);
		printf("Green luminance: %.2f lux green256: %.2f\n", green, green256);
		printf("Blue  luminance: %.2f lux blue256 : %.2f\n", blue, blue256);
		printf("IR    luminance: %.2f lux \n", cData);
		printf("Light luminance: %.2f lux \n", luminance);

		//Watchdog TCS34725
		if (unixCounter >= 60){
			int AktuallTime = time(NULL);
			writeUnixtime(UnixtimeTCS, AktuallTime);
		}

	}
	return;
}
#endif // __READCOLOR_H_
