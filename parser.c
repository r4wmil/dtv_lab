#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//uint64_t read_bits(uint8_t* ptr, size_t bit_offset) {
//	
//}

void analyze_packet(uint8_t* buf) {
	printf("buf[0]=%x\n", buf[0]);
	//size_t o = 0; // Current bit
	//size_t bo = bit_offset / 8; // Current byte
	//size_t bbo = o - bo * 8; // Current bit in byte
}

int main() {
	FILE* f = fopen("telemedium.mpg", "rb");
	size_t offset = 0;
	for (;;) {
		uint8_t buf[188];
		int s;
		s = fseek(f, offset, SEEK_SET);
		if (s != 0) { printf("seek failed\n"); break; }
		size_t n = fread(buf, sizeof(*buf), 188, f);
		if (n != 188) { printf("read failed\n"); break; }
		analyze_packet(buf);
		offset += 188;
	}
	fclose(f);
	return 0;
}
