/*
 * dab_defs.h
 *
 *  Created on: 9 wrz 2022
 */
#ifndef INC_DAB_DEFS_H_
#define INC_DAB_DEFS_H_

enum dab_channels_list{
CH_5A,   // 174928 kHz
CH_5B,   // 176640 kHz
CH_5C,   // 178352 kHz
CH_5D,   // 180064 kHz
CH_6A,   // 181936 kHz
CH_6B,   // 183648 kHz
CH_6C,   // 185360 kHz
CH_6D,   // 187072 kHz
CH_7A,   // 188928 kHz
CH_7B,   // 190640 kHz
CH_7C,   // 192352 kHz
CH_7D,   // 194064 kHz
CH_8A,   // 195936 kHz
CH_8B,   // 197648 kHz
CH_8C,   // 199360 kHz
CH_8D,   // 201072 kHz
CH_9A,   // 202928 kHz
CH_9B,   // 204640 kHz
CH_9C,   // 206352 kHz
CH_9D,   // 208064 kHz
CH_10A,   // 209936 kHz
CH_undef_1,   // 210096 kHz
CH_10B,   // 211648 kHz
CH_10C,   // 213360 kHz
CH_10D,   // 215072 kHz
CH_11A,   // 216928 kHz
CH_undef_2,   // 217088 kHz
CH_11B,   // 218640 kHz
CH_11C,   // 220352 kHz
CH_11D,   // 222064 kHz
CH_12A,   // 223936 kHz
CH_undef_3,   // 224096 kHz
CH_12B,   // 225648 kHz
CH_12C,   // 227360 kHz
CH_12D,   // 229072 kHz
CH_13A,   // 230784 kHz
CH_13B,   // 232496 kHz
CH_13C,   // 234208 kHz
CH_13D,   // 235776 kHz
CH_13E,   // 237488 kHz
CH_13F   // 239200 kHz
};
static const char* dab_channels_names[] = {
"CH_5A",   // 174928 kHz
"CH_5B",   // 176640 kHz
"CH_5C",   // 178352 kHz
"CH_5D",   // 180064 kHz
"CH_6A",   // 181936 kHz
"CH_6B",   // 183648 kHz
"CH_6C",   // 185360 kHz
"CH_6D",   // 187072 kHz
"CH_7A",   // 188928 kHz
"CH_7B",   // 190640 kHz
"CH_7C",   // 192352 kHz
"CH_7D",   // 194064 kHz
"CH_8A",   // 195936 kHz
"CH_8B",   // 197648 kHz
"CH_8C",   // 199360 kHz
"CH_8D",   // 201072 kHz
"CH_9A",   // 202928 kHz
"CH_9B",   // 204640 kHz
"CH_9C",   // 206352 kHz
"CH_9D",   // 208064 kHz
"CH_10A",   // 209936 kHz
"CH_undef_1",   // 210096 kHz
"CH_10B",   // 211648 kHz
"CH_10C",   // 213360 kHz
"CH_10D",   // 215072 kHz
"CH_11A",   // 216928 kHz
"CH_undef_2",   // 217088 kHz
"CH_11B",   // 218640 kHz
"CH_11C",   // 220352 kHz
"CH_11D",   // 222064 kHz
"CH_12A",   // 223936 kHz
"CH_undef_3",   // 224096 kHz
"CH_12B",   // 225648 kHz
"CH_12C",   // 227360 kHz
"CH_12D",   // 229072 kHz
"CH_13A",   // 230784 kHz
"CH_13B",   // 232496 kHz
"CH_13C",   // 234208 kHz
"CH_13D",   // 235776 kHz
"CH_13E",   // 237488 kHz
"CH_13F"   // 239200 kHz
};

#endif /* INC_DAB_DEFS_H_ */
