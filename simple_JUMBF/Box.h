#pragma once
#pragma once
#ifndef _BOX_H_
#define _BOX_H_
#include "Header.h"

#include <iostream>




#define TYPE_PSRD 0
#define TYPE_PSPD 1
#define TYPE_JP2C 2
#define TYPE_XML 3
#define TYPE_JSON 4
#define TYPE_UUID 5
#define TYPE_JPEG360 6
#define TYPE_SOSL 7

#define BOX_TYPE_FAIZ 0x6661697a
#define BOX_TYPE_JUMB 0x6a756d62
#define BOX_TYPE_JUMD 0x6a756d64
#define BOX_TYPE_CODESTREAM 0X6A703263
#define BOX_TYPE_XML 0X786D6C20
#define BOX_TYPE_JSON 0X6A736F6E
#define BOX_TYPE_UUID 0X75756964


typedef struct
{
	unsigned int length;
	unsigned int box_type;

}sj_jumbf_box, * sj_jumbf_box_ptr;

typedef struct _jumbf_super_box
{
	unsigned int length;
	unsigned int box_type;
	sj_jumbf_box_ptr next_box;
	char* playload;
}sj_jumbf_super_box, * sj_jumbf_super_box_ptr;

typedef struct _jumbf_desc_box
{
	unsigned int length;
	unsigned int box_type;
	unsigned char type[16];
	unsigned char toggles;
	char* label;
	unsigned int id;
	char* signature;
	sj_jumbf_box_ptr next_box;

}sj_jumbf_desc_box, * sj_jumbf_desc_box_ptr;


typedef struct _jumbf_contiguous_codestream_box
{
	unsigned int length;
	unsigned int box_type;
	unsigned char* codestream;
	sj_jumbf_box_ptr next;
} sj_jumbf_contiguous_codestream_box, * sj_jumbf_contiguous_codestream_box_ptr;




sj_jumbf_desc_box_ptr create_desc_box(char type, char* label, unsigned int	id, char* signature);
void create_box_buff(sj_jumbf_super_box* box_ptr, unsigned char** dst_buf, unsigned int* dst_size);

void destroy_jumbf_box(sj_jumbf_super_box_ptr box);

bool emit_byte(unsigned char** dst_buf, char data);
bool emit_2byte(unsigned char** dst_buf, unsigned int data);
bool emit_4byte(unsigned char** dst_buf, unsigned int data);

sj_jumbf_super_box_ptr sj_create_jumbf_codestream_box(unsigned char* uuid_data, unsigned int codestreamSize, char* lebel);

#endif // _BOX_H_