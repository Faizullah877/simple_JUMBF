#pragma once
#include <iostream>

#define MAX_PATH (256)

typedef enum {                  /* JPEG marker codes */
	M_SOF0 = 0xc0,
	M_SOF1 = 0xc1,
	M_SOF2 = 0xc2,
	M_SOF3 = 0xc3,

	M_SOF5 = 0xc5,
	M_SOF6 = 0xc6,
	M_SOF7 = 0xc7,

	M_JPG = 0xc8,
	M_SOF9 = 0xc9,
	M_SOF10 = 0xca,
	M_SOF11 = 0xcb,

	M_SOF13 = 0xcd,
	M_SOF14 = 0xce,
	M_SOF15 = 0xcf,

	M_DHT = 0xc4,

	M_DAC = 0xcc,

	M_RST0 = 0xd0,
	M_RST1 = 0xd1,
	M_RST2 = 0xd2,
	M_RST3 = 0xd3,
	M_RST4 = 0xd4,
	M_RST5 = 0xd5,
	M_RST6 = 0xd6,
	M_RST7 = 0xd7,

	M_SOI = 0xd8,
	M_EOI = 0xd9,
	M_SOS = 0xda,
	M_DQT = 0xdb,
	M_DNL = 0xdc,
	M_DRI = 0xdd,
	M_DHP = 0xde,
	M_EXP = 0xdf,

	M_APP0 = 0xe0,
	M_APP1 = 0xe1,
	M_APP2 = 0xe2,
	M_APP3 = 0xe3,
	M_APP4 = 0xe4,
	M_APP5 = 0xe5,
	M_APP6 = 0xe6,
	M_APP7 = 0xe7,
	M_APP8 = 0xe8,
	M_APP9 = 0xe9,
	M_APP10 = 0xea,
	M_APP11 = 0xeb,
	M_APP12 = 0xec,
	M_APP13 = 0xed,
	M_APP14 = 0xee,
	M_APP15 = 0xef,

	M_JPG0 = 0xf0,
	M_JPG13 = 0xfd,
	M_COM = 0xfe,

	M_TEM = 0x01,

	M_ERROR = 0x100
} JPEG_MARKER;

#define SIZE_OF_SIGNATURE	32

#define SIZE_SEGMENT_ELEMENT 20
#define SIZE_LABEL_ELEMENT 5

typedef struct SJ_SEGMENT_ELEMENT {
	bool selected = false;
	unsigned int offset;
	unsigned int length;
	unsigned int ID;

}  *SJ_SEGMENT_ELEMENT_PTR;

typedef struct SJ_SEGMENT_STRUCT {
	SJ_SEGMENT_ELEMENT_PTR* pSegment_element;
	char size = -1;
}*SJ_SEGMENT_STRUCT_PTR;


typedef struct SJ_LABEL_ELEMENT {
	bool selected = false;
	unsigned char* label;
	unsigned int offset;
	unsigned int length;
	char size;
}*SJ_LABEL_ELEMENT_PTR;


typedef struct SJ_LABEL {
	SJ_LABEL_ELEMENT_PTR* pLabel;
	char size = -1;
} *SJ_LABEL_PTR;


typedef struct {


	SJ_LABEL m_label;

	SJ_SEGMENT_STRUCT m_segment;


}SJ_SEGMENT, * SJ_SEGMENT_PTR;






struct jumbfSupBoxInfo {
	bool used = false;
	int boxType;
	int length;
};
struct jumbfDesBox {
	bool used = false;
	int jumd_lenght = 0;
	int type = 0;
	int UUID[16] = { 0 };
	int toggle = 0;
	bool requestable = false;
	bool label_present = false;
	unsigned char lable[256] = { 0 };
	bool ID_present = false;
	int ID = 0;
	bool sign_present = false;
	int signature[32] = { 0 };
};


struct APP11_marker
{
	bool APP11Flag = false;
	int markerOffst = 0;
	int Le = 0;
	int CI_dec = 0;
	int En = 0;
	int Z = 0;
	int LBox = 0;
	int Tbox;
	bool XLBox_present = false;
	unsigned long int XLBox = 0;
};


unsigned int sj_get_2IIbyte(unsigned char** buf);
unsigned int sj_get_byte(unsigned char** buf);

unsigned int sj_get_2byte(unsigned char** buf);

long sj_get_4byte(unsigned char** buf);
