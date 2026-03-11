#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct PHeader {
	uint8_t  sync_byte : 8;  // Must be 0x47
	uint8_t  tei       : 1;  // Transport Error Indicator
	uint8_t  pusi      : 1;  // Payload Unit Start Indicator
	uint8_t  tp        : 1;  // Transport Priority
	uint16_t pid       : 13; // Packet IDentifier
	uint8_t  tsc       : 2;  // Transport Scrambling Control
	uint8_t  afc       : 2;  // Adaptation Field Control
	uint8_t  cc        : 4;  // Continuity Counter
} PHeader;

typedef struct PAFHeader {
	uint8_t afl   : 8; // Adaptation Field Length
	uint8_t di    : 1; // Discontinuity Indicator
	uint8_t rai   : 1; // Random Access Indicator
	uint8_t pi    : 1; // Priority Indicator
	uint8_t pcrf  : 1; // PCR flag
	uint8_t opcrf : 1; // OPCR flag
	uint8_t spf   : 1; // Splicing poing flag
	uint8_t tpdf  : 1; // Transport Private Data flag
	uint8_t afef  : 1; // Adaptation Field Extension flag
} PAFHeader;

PHeader read_header(uint8_t* buf) {
	PHeader ph = {0};
	uint32_t header_bits = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3] << 0);
	ph.sync_byte = (header_bits & 0xff000000) >> 24;
	ph.tei       = (header_bits & 0x00800000) >> 23;
	ph.pusi      = (header_bits & 0x00400000) >> 22;
	ph.tp        = (header_bits & 0x00200000) >> 21;
	ph.pid       = (header_bits & 0x001fff00) >>  8;
	ph.tsc       = (header_bits & 0x000000c0) >>  6;
	ph.afc       = (header_bits & 0x00000030) >>  4;
	ph.cc        = (header_bits & 0x0000000f) >>  0;
	return ph;
}

PAFHeader read_adaptation_header(uint8_t* buf) {
	PAFHeader pafh = {0};
	uint32_t header_bits = (buf[0] << 8) | (buf[1] << 0);
	pafh.afl   = (header_bits & 0xff00) >> 16;
	pafh.di    = (header_bits & 0x0080) >> 7;
	pafh.rai   = (header_bits & 0x0040) >> 6;
	pafh.pi    = (header_bits & 0x0020) >> 5;
	pafh.pcrf  = (header_bits & 0x0010) >> 4;
	pafh.opcrf = (header_bits & 0x0008) >> 3;
	pafh.spf   = (header_bits & 0x0004) >> 2;
	pafh.tpdf  = (header_bits & 0x0002) >> 1;
	pafh.afef  = (header_bits & 0x0001) >> 0;
	return pafh;
}

void analyze_packet(uint8_t* buf) {
	PHeader ph = read_header(buf);
	PAFHeader pafh = read_adaptation_header(buf + 4);
	printf("sync_byte=%02x ", ph.sync_byte);
	printf("tei=%01x ",       ph.tei);
	printf("pusi=%01x ",      ph.pusi);
	printf("tp=%01x ",        ph.tp);
	printf("pid=%04x ",       ph.pid);
	printf("tsc=%01x ",       ph.tsc);
	printf("afc=%01x ",       ph.afc);
	printf("cc=%01x ",        ph.cc);
	printf("| ");
	printf("afl=%x ",  pafh.afl);
	printf("di=%x ",   pafh.di);
	printf("pi=%x ",   pafh.rai);
	printf("pcrf=%x ", pafh.pi);
	printf("pcrf=%x ", pafh.pcrf);
	printf("spf=%x ",  pafh.opcrf);
	printf("spf=%x ",  pafh.spf);
	printf("tpdf=%x ", pafh.tpdf);
	printf("afef=%x ", pafh.afef);
	printf("\n");
}

int main() {
	FILE* f = fopen("telemedium.mpg", "rb");
	size_t offset = 0;
	size_t i = 0;
	for (;;i++) {
		uint8_t buf[188];
		int s;
		size_t n = fread(buf, sizeof(*buf), 188, f);
		if (feof(f)) { printf("file ended\n"); break; }
		if (n != 188) { printf("read failed\n"); break; }
		printf("%zu: ", i);
		analyze_packet(buf);
		offset += 188;
	}
	fclose(f);
	return 0;
}
