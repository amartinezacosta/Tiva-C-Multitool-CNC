#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "print.h"
#include "serial.h"


void print_string(unsigned char *string){
	while(*string){
		serial_write(*string);
		string++;
	}
}

void print_uint8(uint8_t n){
	unsigned char digit;
	digit = n % 10;
	serial_write('0' + digit);
}

void print_uint32_base10(uint32_t n){
	if (n == 0) {
		serial_write('0');
		return;
	}

	unsigned char buf[10];
	uint8_t i = 0;

	while (n > 0) {
		buf[i++] = n % 10;
		n /= 10;
	}

	for (; i > 0; i--)
		serial_write('0' + buf[i-1]);
}


void printInteger(long n){
	if (n < 0) {
		serial_write('-');
		print_uint32_base10(-n);
	} else {
		print_uint32_base10(n);
	}
}

void printFloat(float n, uint8_t decimal_places){
  if (n < 0) {
    serial_write('-');
    n = -n;
  }

  uint8_t decimals = decimal_places;
  while (decimals >= 2) { // Quickly convert values expected to be E0 to E-4.
    n *= 100;
    decimals -= 2;
  }
  if (decimals) { n *= 10; }
  n += 0.5; // Add rounding factor. Ensures carryover through entire value.

  // Generate digits backwards and store in string.
  unsigned char buf[10];
  uint8_t i = 0;
  uint32_t a = (long)n;
  buf[decimal_places] = '.'; // Place decimal point, even if decimal places are zero.
  while(a > 0) {
    if (i == decimal_places) { i++; } // Skip decimal point location
    buf[i++] = (a % 10) + '0'; // Get digit
    a /= 10;
  }
  while (i < decimal_places) {
     buf[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
  }
  if (i == decimal_places) { // Fill in leading zero, if needed.
    i++;
    buf[i++] = '0';
  }

  // Print the generated string.
  for (; i > 0; i--)
    serial_write(buf[i-1]);
}
