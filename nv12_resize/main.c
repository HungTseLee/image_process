#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uint8_t;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	
	printf("test the nv12 resize \r\n");
	
	FILE *fp = fopen("123.nv12", "rb");
	int w = 352;
	int h = 288;
	int size = w * h * 3 / 2;
	printf("size %d \r\n", size);
	uint8_t *tmp_buf = (uint8_t *)malloc(size);
	int res = fread(tmp_buf, 1, size, fp);
	fclose(fp);

	int w_out = 800;
	int h_out = 800;
	int size_out = w_out * h_out * 3 / 2;
	uint8_t *tmp_buf_out = (uint8_t *)malloc(size_out);
	memset(tmp_buf_out, 0, size_out);
	
	//extern void nv12_bilinear_scale (uint8_t* src, uint8_t* dst, int srcWidth, int srcHeight, int dstWidth,int dstHeight);
	extern int ImageResize(uint8_t * src, uint8_t* dst, int sw, int sh,int dw,int dh);
	ImageResize(tmp_buf, tmp_buf_out, w, h, w_out, h_out);
	
	fp = fopen("123_out.nv12", "wb+");
	fwrite(tmp_buf_out, 1, size_out, fp);
	fclose(fp);
	
	
	free(tmp_buf);
	free(tmp_buf_out);	
	
	return 0;
}
