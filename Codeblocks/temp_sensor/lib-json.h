#ifndef LIB_JSON_H
#define LIB_JSON_H
#include <stdint.h>
int connect_tudelft(const char*,const char*,int,char*);
char *readchunk(int);
void cleanup(int);
char* find_message(void);
int get_content_length(void);
int json_decode_arr(char*,uint16_t**);
char* json_encode_arr(double*, int);
char* read_ans(int);
#endif // LIB_JSON_H
