#include "write_jpeg.h"


void create_jumd_buf(unsigned char** jumd_buf, unsigned int* jumd_buf_size, sj_jumbf_super_box* super_box)
{

	sj_jumbf_box_ptr next_box = super_box->next_box;

	while (next_box != NULL)
	{
		if (next_box->box_type == BOX_TYPE_JUMD)
		{
			sj_jumbf_desc_box_ptr desc_box = (sj_jumbf_desc_box_ptr)(next_box);
			//length
			*jumd_buf = new unsigned char[desc_box->length];
			*jumd_buf_size = desc_box->length;
			unsigned char* jumd_buf_position = *jumd_buf;
			emit_4byte(&jumd_buf_position, desc_box->length);
			//box type
			emit_4byte(&jumd_buf_position, desc_box->box_type);

			//type
			for (int i = 0; i < 16; i++)
				emit_byte(&jumd_buf_position, desc_box->type[i]);

			//toggle
			emit_byte(&jumd_buf_position, desc_box->toggles);

			//label;
			if (desc_box->label != NULL)
			{
				for (int i = 0; i <= strlen(desc_box->label); i++)
					emit_byte(&jumd_buf_position, desc_box->label[i]);

			}

			//id
			if (desc_box->id != 0)
			{
				emit_4byte(&jumd_buf_position, desc_box->id);
			}

			//signature
			if (desc_box->signature != NULL)
			{
				for (int i = 0; i < SIZE_OF_SIGNATURE; i++)
					emit_byte(&jumd_buf_position, desc_box->signature[i]);
			}
			next_box = desc_box->next_box;
		}
		else if (next_box->box_type == BOX_TYPE_CODESTREAM) {

			sj_jumbf_contiguous_codestream_box_ptr codestream_box = (sj_jumbf_contiguous_codestream_box_ptr)(next_box);

			next_box = codestream_box->next;
			//std::cout << "\nCan't create buffer for UUID Box\n";
		}
	}

}

bool jpeg_write_jumbf(unsigned char* primary_frame, unsigned int primary_frame_size, sj_jumbf_super_box* super_box, unsigned char** dst_buf, unsigned int* dst_size)
{
	if (primary_frame == NULL)
	{
		return false;
	}
	unsigned char* jumd_buf;
	unsigned int jumd_buf_size;

	create_jumd_buf(&jumd_buf, &jumd_buf_size, super_box);

	unsigned int amount_per_marker = 65000;  // total capacity is 65535;



	unsigned int no_of_app11_markers_req = super_box->length / amount_per_marker; // one extra is needed for remainder
	no_of_app11_markers_req++;



	unsigned char* codestream = NULL;
	unsigned int codestream_size = 0;
	sj_jumbf_box_ptr next_box = super_box->next_box;

	while (next_box != NULL)
	{
		if (next_box->box_type == BOX_TYPE_JUMD)
		{
			sj_jumbf_desc_box_ptr desc_box = (sj_jumbf_desc_box_ptr)(next_box);

			next_box = desc_box->next_box;
		}
		else if (next_box->box_type == BOX_TYPE_CODESTREAM) {

			sj_jumbf_contiguous_codestream_box_ptr codestream_box = (sj_jumbf_contiguous_codestream_box_ptr)(next_box);
			codestream = codestream_box->codestream;
			codestream_size = codestream_box->length - 8;
			next_box = codestream_box->next;
			//std::cout << "\nCan't create buffer for UUID Box\n";
		}
	}


	unsigned int remaining_data = codestream_size;

	unsigned int all_app11_size = no_of_app11_markers_req * (2 + 2 + 2 + 2 + 4 + 4 + 4 + jumd_buf_size + 8) + codestream_size; // 2 app11, 2 Le, 2 CI, 2 En, 4 Z, 4 Lbox, 4 Tbox, 8 jumbcodestream box lenght and type



	unsigned char* marker_buf;
	unsigned int marker_size = 0;
	bool meet_DQT = false;
	bool meet_EOI = false;
	unsigned int length = 0;
	int val1 = 0, val2 = 0;




	unsigned char* buf = primary_frame;
	unsigned int buf_size = 0;
	//SOS marker 
	do {
		val1 = sj_get_byte(&buf);
		buf_size++;

		if (val1 == 0xFF)
		{
			val2 = sj_get_byte(&buf);
			buf_size++;
			switch (val2)
			{
			case M_SOI:
			{

			}break;
			case M_APP0:
			case M_APP1:
			case M_APP2:
			case M_APP3:
			case M_APP4:
			case M_APP5:
			case M_APP6:
			case M_APP7:
			case M_APP8:
			case M_APP9:
			case M_APP10:
			case M_APP12:
			case M_APP13:
			case M_APP14:
			case M_APP15:
			case M_APP11:
			{
				length = sj_get_2byte(&buf);
				buf_size += length;

				length -= 2;
				buf += length;


			}break;
			case M_DQT:
			{
				meet_DQT = true;
				buf -= 2;
				buf_size -= 2;
				*dst_size = primary_frame_size + all_app11_size;

				*dst_buf = new unsigned char[*dst_size];
				unsigned char* dst_buf_position = *dst_buf;
				memcpy(dst_buf_position, primary_frame, buf_size);// copies data till SOS from source

				dst_buf_position += buf_size;
				int APP11_marker = 0xffeb;
				int common_identifier = 0x4A50;
				int box_instance_num = 1;
				unsigned int packet_seq_num = 1;
				unsigned int code_strm_box_length = 0;
				for (unsigned int i = 0; i < no_of_app11_markers_req; ++i) {
					if (remaining_data < amount_per_marker)
					{
						marker_size = remaining_data + 2 + 2 + 2 + 4 + 4 + 4 + jumd_buf_size + 8;
						code_strm_box_length = remaining_data + 8;
					}
					else {
						marker_size = amount_per_marker + 2 + 2 + 2 + 4 + 4 + 4 + jumd_buf_size + 8;
						code_strm_box_length = amount_per_marker + 8;
					}
					emit_2byte(&dst_buf_position, APP11_marker);
					emit_2byte(&dst_buf_position, marker_size); // Le  // current marker size
					emit_2byte(&dst_buf_position, common_identifier); //CI : same for every marker
					emit_2byte(&dst_buf_position, box_instance_num);  // En  : same for every marker
					emit_4byte(&dst_buf_position, packet_seq_num++);	// Z : different for same jumbbox distributed in multiple markers 
					emit_4byte(&dst_buf_position, super_box->length); // Lbox same for every marker
					emit_4byte(&dst_buf_position, BOX_TYPE_JUMB);
					memcpy(dst_buf_position, jumd_buf, jumd_buf_size);
					dst_buf_position += jumd_buf_size;
					emit_4byte(&dst_buf_position, code_strm_box_length);
					emit_4byte(&dst_buf_position, BOX_TYPE_CODESTREAM);
					memcpy(dst_buf_position, codestream, code_strm_box_length - 8);
					dst_buf_position += (code_strm_box_length - 8);
					codestream += (code_strm_box_length - 8);
					remaining_data -= (code_strm_box_length - 8);
				}

				memcpy(dst_buf_position, buf, primary_frame_size - buf_size);


			}break;


			case M_SOF0:
			case M_SOF1:
			case M_SOF2:
			case M_SOF9:
			case M_SOF10:
			case M_SOF3:                /* Lossless, Huffman */
			case M_SOF5:                /* Differential sequential, Huffman */
			case M_SOF6:                /* Differential progressive, Huffman */
			case M_SOF7:                /* Differential lossless, Huffman */
			case M_JPG:                 /* Reserved for JPEG extensions */
			case M_SOF11:               /* Lossless, arithmetic */
			case M_SOF13:               /* Differential sequential, arithmetic */
			case M_SOF14:               /* Differential progressive, arithmetic */
			case M_SOF15:               /* Differential lossless, arithmetic */
			case M_DAC:
			case M_DHT:

			case M_DRI:

			case M_COM:
			case M_RST0:
			case M_RST1:
			case M_RST2:
			case M_RST3:
			case M_RST4:
			case M_RST5:
			case M_RST6:
			case M_RST7:
			case M_TEM:
			case M_DNL:

			case M_SOS:


			case M_EOI:
			{
				meet_EOI = true;
			}
			break;
			default:


				break;
			}
		}

		if (buf > primary_frame + primary_frame_size)
		{
			//src_buf isn't jpeg file.
			cout << "something wrong" << endl;
			return false;
		}

	} while (meet_DQT == false);



	return true;
}

