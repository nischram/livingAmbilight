/*
gcc -g -o stop  stop.c
*/

#include <stdlib.h>

int main()
{
	system("sudo killall livingAmbilight");
	system("sudo killall watchdog");
}
