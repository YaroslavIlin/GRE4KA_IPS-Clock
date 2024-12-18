#ifndef FIXIC_HPP
#define FIXIC_HPP
#include <stdint.h>
extern const uint16_t pix_width;
extern const uint16_t pix_height;
extern const uint8_t pix0_data[];
extern const uint8_t pix1_data[];
extern const uint8_t pix2_data[];
extern const uint8_t pix3_data[];
extern const uint8_t pix4_data[];
extern const uint8_t pix5_data[];
extern const uint8_t pix6_data[];
extern const uint8_t pix7_data[];
extern const uint8_t pix8_data[];
extern const uint8_t pix9_data[];

inline
const uint8_t *pixnum(uint8_t num){
	num %= 10;
	#define hehe(i) case i: return pix##i##_data;
	switch(num){
	hehe(0)
	hehe(1)
	hehe(2)
	hehe(3)
	//hehe(4)
	//hehe(5)
	//hehe(6)
	//hehe(7)
	//hehe(8)
	//hehe(9)
	}
	#undef hehe
	return pix0_data;
}

#endif
