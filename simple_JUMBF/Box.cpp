#include "Box.h"


sj_jumbf_desc_box_ptr create_desc_box(char type, char* label, unsigned int	id, char* signature);
void create_box_buff(sj_jumbf_super_box* box_ptr, unsigned char** dst_buf, unsigned int* dst_size);
void destroy_jumbf_box(sj_jumbf_super_box_ptr box);


bool emit_byte(unsigned char** dst_buf, char data)
{

	**dst_buf = data;
	(*dst_buf)++;
	return true;
}

bool emit_2byte(unsigned char** dst_buf, unsigned int data)
{
	unsigned char temp_data = (unsigned char)(data >> 8);
	emit_byte(dst_buf, temp_data);

	temp_data = (unsigned char)(data);
	emit_byte(dst_buf, temp_data);

	return true;
}

bool emit_4byte(unsigned char** dst_buf, unsigned int data)
{
	unsigned char temp_data = (unsigned char)(data >> 24);
	emit_byte(dst_buf, temp_data);

	temp_data = (unsigned char)(data >> 16);
	emit_byte(dst_buf, temp_data);

	temp_data = (unsigned char)(data >> 8);
	emit_byte(dst_buf, temp_data);

	temp_data = (unsigned char)(data);
	emit_byte(dst_buf, temp_data);
	return true;
}



sj_jumbf_desc_box_ptr create_desc_box(char type, char* label, unsigned int	id, char* signature)
{
	sj_jumbf_desc_box_ptr desc_box = (sj_jumbf_desc_box_ptr)malloc(sizeof(sj_jumbf_desc_box));
	memset(desc_box, 0, sizeof(sj_jumbf_desc_box));

	unsigned char type_psrd[16] = { 0xDC, 0x28, 0xB9, 0x5F, 0xB6, 0x8A, 0x49, 0x8E, 0x80, 0x64, 0x0F, 0xCA, 0x84, 0x5D, 0x6B, 0x0E };
	unsigned char type_pspd[16] = { 0x74, 0xB1, 0x1B, 0xBF, 0xF2, 0x1D, 0x4E, 0xEA, 0x98, 0xC1, 0x0B, 0xEB, 0xF2, 0x3A, 0xEF, 0xD3 };
	unsigned char type_jp2c[16] = { 0x65, 0x79, 0xD6, 0xFB, 0xDB, 0xA2, 0x44, 0x6B, 0xB2, 0xAC, 0x1B, 0x82, 0xFE, 0xEB, 0x89, 0xD1 };
	unsigned char type_xml[16] = { 0x78, 0x6D, 0x6C, 0x20, 0x00, 0x11, 0x00, 0x10, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };
	unsigned char type_json[16] = { 0x6A, 0x73, 0x6F, 0x6E, 0x00, 0x11, 0x00, 0x10, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };
	unsigned char type_uuid[16] = { 0x75, 0x75, 0x69, 0x64, 0x00, 0x11, 0x00, 0x10, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };
	unsigned char type_jpeg360[16] = { 0x78, 0x5F, 0x34, 0xB7, 0x5D, 0x4B, 0x47, 0x4C, 0xB8, 0x9F, 0x1D, 0x99, 0xE0, 0xE3, 0xA8, 0xDD };
	unsigned char type_sosL[16] = { 0x65, 0x79, 0xD6, 0xFB, 0xDB, 0xA2, 0x44, 0x6B, 0xB2, 0xAC, 0x1B, 0x82, 0x66, 0x61, 0x69, 0x7a }; // faiz

	desc_box->length = 25;
	desc_box->box_type = BOX_TYPE_JUMD;
	desc_box->label = NULL;
	desc_box->id = 0;
	desc_box->signature = NULL;
	switch (type)
	{
	case TYPE_PSRD:
	{		memcpy(desc_box->type, type_psrd, 16);
	}break;
	case TYPE_PSPD:
	{		memcpy(desc_box->type, type_pspd, 16);
	}break;
	case TYPE_JP2C:
	{	memcpy(desc_box->type, type_jp2c, 16);
	}break;
	case TYPE_XML:
	{	memcpy(desc_box->type, type_xml, 16);
	}break;
	case TYPE_JSON:
	{	memcpy(desc_box->type, type_json, 16);
	}break;
	case TYPE_UUID:
	{	memcpy(desc_box->type, type_uuid, 16);
	}break;
	case TYPE_JPEG360:
	{	memcpy(desc_box->type, type_jpeg360, 16);
	}break;
	case TYPE_SOSL:
	{	memcpy(desc_box->type, type_sosL, 16);
	}break;
	default:
		break;
	}
	desc_box->toggles = 0;
	if (label != NULL)
	{
		desc_box->toggles += 2;
		desc_box->label = (char*)calloc(strlen(label) + 1, sizeof(char));
		memcpy(desc_box->label, label, strlen(label));
		desc_box->length += (strlen(label) + 1);
	}
	if (id != 0)
	{
		desc_box->toggles += 4;
		desc_box->id = id;
		desc_box->length += 4;
	}
	if (signature != NULL)
	{
		desc_box->toggles += 8;
		desc_box->signature = (char*)calloc(SIZE_OF_SIGNATURE + 1, sizeof(char)); // Prepare 1 more byte space to finish with null.
		memcpy(desc_box->signature, signature, SIZE_OF_SIGNATURE);
		desc_box->length += SIZE_OF_SIGNATURE;
	}
	return desc_box;
}

void destroy_jumbf_box(sj_jumbf_super_box_ptr box)
{
	sj_jumbf_box_ptr next_box = box->next_box;

	while (next_box != NULL)
	{
		if (next_box->box_type == BOX_TYPE_JUMD)
		{
			sj_jumbf_desc_box_ptr desc_box = (sj_jumbf_desc_box_ptr)(next_box);
			if (desc_box->label != NULL)
				free(desc_box->label);

			if (desc_box->signature != NULL)
				free(desc_box->signature);

			next_box = desc_box->next_box;
			free(desc_box);
		}

		else if (next_box->box_type == BOX_TYPE_CODESTREAM) {
			sj_jumbf_contiguous_codestream_box_ptr codestream_box = (sj_jumbf_contiguous_codestream_box_ptr)(next_box);
			if (codestream_box->codestream != NULL)
				free(codestream_box->codestream);
			next_box = codestream_box->next;
			free(codestream_box);
		}
	}

}
void create_box_buff(sj_jumbf_super_box* box_ptr, unsigned char** dst_buf, unsigned int* dst_size)
{
	//메모리 할당
	*dst_buf = new unsigned char[box_ptr->length];
	unsigned char* dst_buf_position = *dst_buf;
	*dst_size = box_ptr->length;

	//box_ptr->length == box_size야 함

	//super box length & box type
	emit_4byte(&dst_buf_position, box_ptr->length);
	emit_4byte(&dst_buf_position, box_ptr->box_type);

	sj_jumbf_box_ptr next_box = box_ptr->next_box;
	//add next box
	while (next_box != NULL)
	{
		//jumf box (description box)
		if (next_box->box_type == BOX_TYPE_JUMD)
		{
			sj_jumbf_desc_box_ptr desc_box = (sj_jumbf_desc_box_ptr)(next_box);
			//length
			emit_4byte(&dst_buf_position, desc_box->length);
			//box type
			emit_4byte(&dst_buf_position, desc_box->box_type);

			//type
			for (int i = 0; i < 16; i++)
				emit_byte(&dst_buf_position, desc_box->type[i]);

			//toggle
			emit_byte(&dst_buf_position, desc_box->toggles);

			//label;
			if (desc_box->label != NULL)
			{
				for (int i = 0; i <= strlen(desc_box->label); i++)
					emit_byte(&dst_buf_position, desc_box->label[i]);

			}

			//id
			if (desc_box->id != 0)
			{
				emit_4byte(&dst_buf_position, desc_box->id);
			}

			//signature
			if (desc_box->signature != NULL)
			{
				for (int i = 0; i < SIZE_OF_SIGNATURE; i++)
					emit_byte(&dst_buf_position, desc_box->signature[i]);
			}


			next_box = desc_box->next_box;
		}
		else if (next_box->box_type == BOX_TYPE_CODESTREAM) {

			sj_jumbf_contiguous_codestream_box_ptr codestream_box = (sj_jumbf_contiguous_codestream_box_ptr)(next_box);
			emit_4byte(&dst_buf_position, codestream_box->length);

			emit_4byte(&dst_buf_position, codestream_box->box_type);
			for (unsigned int i = 0; i < codestream_box->length; ++i) {
				emit_byte(&dst_buf_position, codestream_box->codestream[i]);
			}
			next_box = codestream_box->next;

			//std::cout << "\nCan't create buffer for UUID Box\n";
		}
	}
	destroy_jumbf_box(box_ptr);
	free(box_ptr);

	//add next box 


}

sj_jumbf_contiguous_codestream_box_ptr create_codestream_box(unsigned char* codestream_data, unsigned int codestreamSize)
{
	sj_jumbf_contiguous_codestream_box_ptr codestream_box = (sj_jumbf_contiguous_codestream_box_ptr)malloc(sizeof(sj_jumbf_contiguous_codestream_box));
	memset(codestream_box, 0, sizeof(sj_jumbf_contiguous_codestream_box));
	unsigned int length = codestreamSize + 8; //length 4 byte, box type 4 bytpe, 문자열 끝 \0 1byte
	unsigned int box_type = BOX_TYPE_CODESTREAM;
	codestream_box->codestream = (unsigned char*)calloc(length - 8, sizeof(unsigned char));
	codestream_box->length = length;
	codestream_box->box_type = box_type;

	memcpy(codestream_box->codestream, codestream_data, length - 8);
	codestream_box->next = NULL;
	return codestream_box;
}

sj_jumbf_super_box_ptr sj_create_jumbf_codestream_box(unsigned char* codestream_data, unsigned int codestreamSize, char* label)
{
	sj_jumbf_super_box_ptr jumbf_box = (sj_jumbf_super_box_ptr)malloc(sizeof(sj_jumbf_super_box));
	memset(jumbf_box, 0, sizeof(sj_jumbf_super_box));
	sj_jumbf_contiguous_codestream_box_ptr codestream_box = create_codestream_box(codestream_data, codestreamSize);

	sj_jumbf_desc_box_ptr desc_box = create_desc_box(TYPE_JP2C, label, 0, NULL);
	desc_box->next_box = (sj_jumbf_box_ptr)codestream_box;
	jumbf_box->length = codestream_box->length + desc_box->length + 8;
	jumbf_box->box_type = BOX_TYPE_JUMB;
	jumbf_box->next_box = (sj_jumbf_box_ptr)desc_box;
	return jumbf_box;
}