#ifndef __base64_h_included__
#define __base64_h_included__

#include <stdio.h>

extern size_t base64_estimated_encoded_len(size_t input_length);
extern char *base64_encode(char *data, size_t input_length, size_t *output_length);
extern void build_decoding_table();

#endif