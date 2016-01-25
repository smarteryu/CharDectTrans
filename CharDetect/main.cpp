#include<stdio.h>
#include"chardet.h"
#include"iconv.h"

#ifdef _DEBUG
#pragma comment(lib, "charsetdll_debug.lib")
#else
#pragma comment(lib, "charsetdll_release.lib")
#endif
#pragma comment(lib, "libiconv.lib")

#define MAX_LEN 100*1024
#define MAX_ENCODE_LEN 256
#define _TEST

int  toUtf8(unsigned char *inBuffer, int inLen, unsigned char *outBuffer, int outLen)
{
	chardet_t chardet = NULL;
	char inEncode[MAX_ENCODE_LEN];
	if (chardet_create(&chardet) == CHARDET_RESULT_OK){
		if (chardet_handle_data(chardet, (const char*)inBuffer, inLen) == CHARDET_RESULT_OK){
			if (chardet_data_end(chardet) == CHARDET_RESULT_OK){
				chardet_get_charset(chardet, inEncode, MAX_ENCODE_LEN);
			}
		}
	}
	if (chardet!=NULL)
		chardet_destroy(chardet);

#ifdef _TEST
	printf("Encode: %s\n", inEncode);
#endif
	
	int outBufferLen = outLen;
	iconv_t hIconv = iconv_open("UTF-8//IGNORE", inEncode);
	if (-1 == (int)hIconv){
		fprintf(stderr, "iconv_open failed!\n");
		return -1;
	}
	iconv(hIconv, (const char**)(&inBuffer), (size_t*)&inLen, (char**)(&outBuffer), (size_t*)&outLen);
	iconv_close(hIconv);
	return outBufferLen - outLen;
}

int main()
{
	chardet_t chardect = NULL;
	unsigned char inBuffer[MAX_LEN], outBuffer[2 * MAX_LEN];
	int inLen = 0, outLen = 2 * MAX_LEN;

	FILE *fp = fopen("input.txt", "rb");
	if (fp == NULL){
		fprintf(stderr, "Open file input.txt failed!\n");
	}
	inLen = fread(inBuffer, 1, MAX_LEN, fp);
	fclose(fp);

	int ret = toUtf8(inBuffer, inLen, outBuffer, outLen);

	FILE *fo = fopen("output.txt", "wb");
	fwrite(outBuffer, 1, ret, fo);
	fclose(fo);

	return 0;
}