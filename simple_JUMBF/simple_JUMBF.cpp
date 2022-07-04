#include <iostream>
#include "Box.h"
#include "write_jpeg.h"
using namespace ::std;
int read_file_in_binary(const char* filename, unsigned char** file_data, unsigned int* file_size)
{
	FILE* fp;
	errno_t err;

	err = fopen_s(&fp, filename, "rb");
	if (err == 0)
	{
		printf("\n=======>Input File  === %s ", filename);
	}
	else
	{
		printf("The %s was not opened ", filename);
		exit(EXIT_FAILURE);
		return 0;
	}
	std::cout << std::endl;
	fseek(fp, 0L, SEEK_END);
	long inFileSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	unsigned char* inputFileByteStream = new unsigned char[inFileSize];
	long readsize = (int)fread(inputFileByteStream, sizeof(char), inFileSize, fp);
	*file_data = inputFileByteStream;
	*file_size = unsigned int(readsize);
	if (inFileSize != readsize)
	{
		std::cout << "Error - file reading\n";
		fclose(fp);
		return 0;
	}
	return 1;
}

int main()
{
	const char* inputFileName = "C:\\Users\\FAIZULLAH\\Desktop\\Hajj.jpg";
	unsigned char* keyFrame;
	unsigned int keyFrame_Size;
	read_file_in_binary(inputFileName, &keyFrame, &keyFrame_Size);
	cout << "Key Frame loaded to memory\n";

	const char* SecondImag = "C:\\Users\\FAIZULLAH\\Desktop\\new.jpg";
	unsigned char* SecondaryFrame;
	unsigned int SecondaryFrame_Size;
	read_file_in_binary(SecondImag, &SecondaryFrame, &SecondaryFrame_Size);
	cout << "Second Image loaded to memory\n\n";

	unsigned char* final_data = NULL;
	unsigned int final_data_size = 0;

	sj_jumbf_super_box_ptr uuid_box = sj_create_jumbf_codestream_box(SecondaryFrame, SecondaryFrame_Size, (char*)"JPEG_Img_in_JPEG");
	jpeg_write_jumbf(keyFrame, keyFrame_Size, uuid_box, &final_data, &final_data_size);


	const char* out_file_name = "C:\\Users\\FAIZULLAH\\Desktop\\new_faiz.jpg";
	FILE* outFile;
	errno_t err1 = fopen_s(&outFile, out_file_name, "wb");
	if (err1 == 0)
	{
		//printf("\Output File  : %s ", out_file_name);
		//cout << endl;
	}
	else
	{
		printf("The %s for Output was not opened ", out_file_name);
		cout << endl;
		return 0;
	}
	fwrite(final_data, sizeof(unsigned char), final_data_size, outFile);
	fclose(outFile);
}

