#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

typedef unsigned char uint8_t;
 
/** 
 * @param src input nv12 raw data array  
 * @param dst output nv12 raw data result, 
 * the memory need to be allocated outside of the function 
 * @param srcWidth width of the input nv12 image 
 * @param srcHeight height of the input nv12 image 
 * @param dstWidth
 * @param dstHeight 
 */

void nv12_nearest_scale(uint8_t* __restrict src, uint8_t* __restrict dst,
                        int srcWidth, int srcHeight, int dstWidth, int
dstHeight)      //restrict keyword is for compiler to optimize program
{
    register int sw = srcWidth;  //register keyword is for local var to accelorate 
    register int sh = srcHeight;
    register int dw = dstWidth;
    register int dh = dstHeight;
    register int y, x;
    unsigned long int srcy, srcx, src_index, dst_index;
    unsigned long int xrIntFloat_16 = (sw << 16) / dw + 1; //better than float division
    unsigned long int yrIntFloat_16 = (sh << 16) / dh + 1;

    uint8_t* dst_uv = dst + dh * dw; //memory start pointer of dest uv
    uint8_t* src_uv = src + sh * sw; //memory start pointer of source uv
    uint8_t* dst_uv_yScanline;
    uint8_t* src_uv_yScanline;
    uint8_t* dst_y_slice = dst; //memory start pointer of dest y
    uint8_t* src_y_slice;
    uint8_t* sp;
    uint8_t* dp;
 
    for (y = 0; y < (dh & ~7); ++y)  
    {
        srcy = (y * yrIntFloat_16) >> 16;
        src_y_slice = src + srcy * sw;

        if((y & 1) == 0)
        {
            dst_uv_yScanline = dst_uv + (y / 2) * dw;
            src_uv_yScanline = src_uv + (srcy / 2) * sw;
        }

        for(x = 0; x < (dw & ~7); ++x)
        {
            srcx = (x * xrIntFloat_16) >> 16;
            dst_y_slice[x] = src_y_slice[srcx];

            if((y & 1) == 0) //y is even
	          {
	              if((x & 1) == 0) //x is even
                {
		                src_index = (srcx / 2) * 2;
			
                    sp = dst_uv_yScanline + x;
                    dp = src_uv_yScanline + src_index;
                    *sp = *dp;
                    ++sp;
                    ++dp;
                    *sp = *dp;
                }
	          }
        }
        dst_y_slice += dw;
    }
}

void nv12_bilinear_scale (uint8_t* src, uint8_t* dst,
        int srcWidth, int srcHeight, int dstWidth,int dstHeight)
{
    int x, y;
    int ox, oy;
    int tmpx, tmpy;
    int xratio = (srcWidth << 8)/dstWidth;
    int yratio = (srcHeight << 8)/dstHeight;
    uint8_t* dst_y = dst;
    uint8_t* dst_uv = dst + dstHeight * dstWidth;
    uint8_t* src_y = src;
    uint8_t* src_uv = src + srcHeight * srcWidth;
    uint8_t y_plane_color[2][2];
    int j,i;
    int offsetY;
    int y_final; 
	
	uint8_t* dst_uv_yScanline;
    uint8_t* src_uv_yScanline;
	unsigned long int src_index;
	uint8_t* sp;
    uint8_t* dp;

    tmpy = 0;
    for (j = 0; j < (dstHeight & ~7); ++j) {

        oy = tmpy >> 8;
        y = tmpy & 0xFF;
        tmpx = 0;

		if((j & 1) == 0) {
            dst_uv_yScanline = dst_uv + (j / 2) * dstWidth;
            src_uv_yScanline = src_uv + (oy / 2) * srcWidth;
        }
		
        for (i = 0; i < (dstWidth & ~7); ++i) {

            ox = tmpx >> 8;
            x = tmpx & 0xFF;
        
            offsetY = oy * srcWidth;
			//Y use bilinear
            y_plane_color[0][0] = src[ offsetY + ox ];
            y_plane_color[1][0] = src[ offsetY + ox + 1 ];
            y_plane_color[0][1] = src[ offsetY + srcWidth + ox ];
            y_plane_color[1][1] = src[ offsetY + srcWidth + ox + 1 ];
                
            int y_final = (0x100 - x) * (0x100 - y) * y_plane_color[0][0]
                + x * (0x100 - y) * y_plane_color[1][0]
                + (0x100 - x) * y * y_plane_color[0][1]
                + x * y * y_plane_color[1][1];
            y_final = y_final >> 16;
            if (y_final>255)
                y_final = 255;
            if (y_final<0)
                y_final = 0;
            dst_y[ j * dstWidth + i] = (uint8_t)y_final; //set Y in dest array

			//UV use nearest
            if((j & 1) == 0) { //y is even
				if((i & 1) == 0) { //x is even
					src_index = (ox / 2) * 2;
					sp = dst_uv_yScanline + i;
					dp = src_uv_yScanline + src_index;
					*sp = *dp;
					++sp;
					++dp;
					*sp = *dp;
                }
			}
            tmpx += xratio;
        }
        tmpy += yratio;
    }
}


int ImageResize(uint8_t * src, uint8_t* dst, int sw,
		int sh,int dw,int dh)
{
	if( (src == NULL) || (dst == NULL) || (0 == dw) || (0 == dh) ||
			(0 == sw) || (0 == sh))
	{
	    printf("params error\n");
	    return -1;
	}
	//nv12_nearest_scale(src, dst, sw, sh, dw, dh);
	nv12_bilinear_scale(src, dst, sw, sh, dw, dh);
	//greyscale(src, dst, sw, sh, dw, dh);
	return 0;
}
