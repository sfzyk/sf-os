#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <kernel/vsnprintf.h> 
#include <kernel/printk.h>


static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);
	char tmp_buf[2024];
	int written  = 0;
	written = vsnprintf(tmp_buf, sizeof(tmp_buf), format, parameters); 
	if (!print(tmp_buf , written )){
			printk("printf is wrong !!!\n");
	}
	va_end(parameters);
	return 1;
}
