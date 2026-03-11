#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct PHeader {
	uint8_t  sync_byte : 8; // 8b  - Must be 0x47
	uint8_t  tei : 1;       // 1b  - Transport Error Indicator
	uint8_t  pusi : 1;      // 1b  - Payload Unit Start Indicator
	uint8_t  tp : 1;        // 1b  - Transport Priority
	uint16_t pid : 13;      // 13b - Packet IDentifier
	uint8_t  tsc : 2;       // 2b  - Transport Scrambling Control
	uint8_t  afc : 2;       // 2b  - Adaptation Field Control
	uint8_t  cc : 4;        // 4b  - Continuity Counter
} PHeader;

PHeader read_header(uint8_t* buf) {
	PHeader ph = {0};
	uint32_t header_bits = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3] << 0);
	ph.sync_byte = (header_bits & 0xff000000) >> 24;
	ph.tei       = (header_bits & 0x00800000) >> 23;
	ph.pusi      = (header_bits & 0x00400000) >> 22;
	ph.tp        = (header_bits & 0x00200000) >> 21;
	ph.pid       = (header_bits & 0x001fff00) >> 8;
	ph.tsc       = (header_bits & 0x000000c0) >> 6;
	ph.afc       = (header_bits & 0x00000030) >> 4;
	ph.cc        = (header_bits & 0x0000000f) >> 0;
	return ph;
}

void analyze_packet(uint8_t* buf) {
	PHeader header = read_header(buf);
	printf("sync_byte=%02x ", header.sync_byte);
	printf("tei=%01x ",       header.tei);
	printf("pusi=%01x ",      header.pusi);
	printf("tp=%01x ",        header.tp);
	printf("pid=%04x ",       header.pid);
	printf("tsc=%01x ",       header.tsc);
	printf("afc=%01x ",       header.afc);
	printf("cc=%01x ",        header.cc);
	printf("\n");
}

int main() {
	FILE* f = fopen("telemedium.mpg", "rb");
	size_t offset = 0;
	for (;;) {
		uint8_t buf[188];
		int s;
		size_t n = fread(buf, sizeof(*buf), 188, f);
		if (feof(f)) { printf("file ended\n"); break; }
		if (n != 188) { printf("read failed\n"); break; }
		analyze_packet(buf);
		offset += 188;
	}
	fclose(f);
	return 0;
}
