
__constant float s1 = 0.195090322f;
__constant float s2 = 0.3826834324f;
__constant float s3 = 0.555570233f;
__constant float c1 = 0.9807852804f;
__constant float c2 = 0.9238795325f;
__constant float c3 = 0.8314696123f;
__constant float c4 = 0.7071067812f;

__kernel void dct8x8(__local float *pSrc, int srcStep, __local float * pDst, int dstStep)
{
	__local float * srcVector0 = pSrc;
	__local float * srcVector1 = srcVector0 + srcStep;
        __local float * srcVector2 = srcVector1 + srcStep;
        __local float * srcVector3 = srcVector2 + srcStep;
        __local float * srcVector4 = srcVector3 + srcStep;
        __local float * srcVector5 = srcVector4 + srcStep;
        __local float * srcVector6 = srcVector5 + srcStep;
        __local float * srcVector7 = srcVector6 + srcStep;

	__local float * dstVector0 = pDst;
	__local float * dstVector1 = dstVector0 + dstStep;
        __local float * dstVector2 = dstVector1 + dstStep;
        __local float * dstVector3 = dstVector2 + dstStep;
        __local float * dstVector4 = dstVector3 + dstStep;
        __local float * dstVector5 = dstVector4 + dstStep;
        __local float * dstVector6 = dstVector5 + dstStep;
        __local float * dstVector7 = dstVector6 + dstStep;
	
        float X07P = (*srcVector0) + (*srcVector7);
        float X07M = (*srcVector0) - (*srcVector7);
    
        float X16P = (*srcVector1) + (*srcVector6);
        float X16M = (*srcVector1) - (*srcVector6);

        float X25P = (*srcVector2) + (*srcVector5);
        float X25M = (*srcVector2) - (*srcVector5);

        float X34P = (*srcVector3) + (*srcVector4);
        float X34M = (*srcVector3) - (*srcVector4);

        float X07P34PP = X07P + X34P;
        float X07P34PM = X07P - X34P;

        float X16P25PP = X16P + X25P;
        float X16P25PM = X16P - X25P;

        float c4X16M25MP = c4*(X16M + X25M);
        float c4X16M25MM = c4*(X16M - X25M);

        float X34Mc4X16M25MMP = X34M + c4X16M25MM;
        float X34Mc4X16M25MMM = X34M - c4X16M25MM;

        float X07Mc4X16M25MPP = X07M + c4X16M25MP;
        float X07Mc4X16M25MPM = X07M - c4X16M25MP;

        (*dstVector0) = c4 * ( X07P34PP + X16P25PP );
        (*dstVector2) = ( s2 * X16P25PM  + c2 * X07P34PM );
        (*dstVector4) = c4 * ( X07P34PP - X16P25PP );
        (*dstVector6) = ( s2 * X07P34PM - c2 * X16P25PM );

        (*dstVector1) = ( s1 * X34Mc4X16M25MMP + c1 * X07Mc4X16M25MPP );
        (*dstVector3) = ( c3 * X07Mc4X16M25MPM - s3 * X34Mc4X16M25MMM );
        (*dstVector5) = ( c3 * X34Mc4X16M25MMM + s3 * X07Mc4X16M25MPM );
        (*dstVector7) = ( s1 * X07Mc4X16M25MPP - c1 * X34Mc4X16M25MMP );
}

__kernel void dct_transform(__global float * pSrc, __global float * pDst, int height, int width)
{
	__local float tmp[64];
	__local float src[64];
	
	int lid = get_local_id(0);
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int lsy = get_local_size(0);
	int lsx = get_local_size(1);
	
	int srcIndex = gy*width + gx*8;
	
	__local float * localPtr = &src[lid*8];
	__global float * srcPtr = &pSrc[srcIndex];
	for(int i=0;i<8;i++)
	{
		*(localPtr++) = *(srcPtr++);
	}
	
	dct8x8(&src[8*lid], 1, &tmp[8*lid], 1);
	barrier(CLK_LOCAL_MEM_FENCE);
	dct8x8(&tmp[lid], 8, &src[lid], 8);
	barrier(CLK_LOCAL_MEM_FENCE);
	
	localPtr = &src[lid*8];
	__global float * dstPtr = &pDst[srcIndex];
	for(int i=0;i<8;i++)
	{
		*(dstPtr++) = *(localPtr++);
	}
}

__constant float idct_c1 = 1.387039845322148f;
__constant float idct_c2 = 1.306562964876377f;
__constant float idct_c3 = 1.175875602419359f;
__constant float idct_c4 = 0.785694958387102f;
__constant float idct_c5 = 0.541196100146197f;
__constant float idct_c6 = 0.275899379282943f;
__constant float idct_cn = 0.5*0.3535533905932737f;

__kernel void idct8x8(__local float *pSrc, int srcStep, __local float * pDst, int dstStep)
{
	__local float * srcVector0 = pSrc;
	__local float * srcVector1 = srcVector0 + srcStep;
        __local float * srcVector2 = srcVector1 + srcStep;
        __local float * srcVector3 = srcVector2 + srcStep;
        __local float * srcVector4 = srcVector3 + srcStep;
        __local float * srcVector5 = srcVector4 + srcStep;
        __local float * srcVector6 = srcVector5 + srcStep;
        __local float * srcVector7 = srcVector6 + srcStep;

	__local float * dstVector0 = pDst;
        __local float * dstVector1 = dstVector0 + dstStep;
        __local float * dstVector2 = dstVector1 + dstStep;
        __local float * dstVector3 = dstVector2 + dstStep;
        __local float * dstVector4 = dstVector3 + dstStep;
        __local float * dstVector5 = dstVector4 + dstStep;
        __local float * dstVector6 = dstVector5 + dstStep;
        __local float * dstVector7 = dstVector6 + dstStep;

        float Y04P   = (*srcVector0) + (*srcVector4);
        float Y2b6eP = idct_c2 * (*srcVector2) + idct_c5 * (*srcVector6);

        float Y04P2b6ePP = Y04P + Y2b6eP;
        float Y04P2b6ePM = Y04P - Y2b6eP;


        float Y7f1aP3c5dPP = idct_c6 * (*srcVector7) + idct_c1 * (*srcVector1) + idct_c3 * (*srcVector3) + idct_c4 * (*srcVector5);
        float Y7a1fM3d5cMP = idct_c1 * (*srcVector7) - idct_c6 * (*srcVector1) + idct_c4 * (*srcVector3) - idct_c3 * (*srcVector5);

        float Y04M   = (*srcVector0) - (*srcVector4);
        float Y2e6bM = idct_c5 * (*srcVector2) - idct_c2 * (*srcVector6);

        float Y04M2e6bMP = Y04M + Y2e6bM;
        float Y04M2e6bMM = Y04M - Y2e6bM;

        float Y1c7dM3f5aPM = idct_c3 * (*srcVector1) - idct_c4 * (*srcVector7) - idct_c6 * (*srcVector3) - idct_c1 * (*srcVector5);
        float Y1d7cP3a5fMM = idct_c4 * (*srcVector1) + idct_c3 * (*srcVector7) - idct_c1 * (*srcVector3) + idct_c6 * (*srcVector5);

        (*dstVector0) = idct_cn * ( Y04P2b6ePP + Y7f1aP3c5dPP );
        (*dstVector7) = idct_cn * ( Y04P2b6ePP - Y7f1aP3c5dPP );
        (*dstVector4) = idct_cn * ( Y04P2b6ePM + Y7a1fM3d5cMP );
        (*dstVector3) = idct_cn * ( Y04P2b6ePM - Y7a1fM3d5cMP );

        (*dstVector1) = idct_cn * ( Y04M2e6bMP + Y1c7dM3f5aPM );
        (*dstVector5) = idct_cn * ( Y04M2e6bMM - Y1d7cP3a5fMM );
        (*dstVector2) = idct_cn * ( Y04M2e6bMM + Y1d7cP3a5fMM );
        (*dstVector6) = idct_cn * ( Y04M2e6bMP - Y1c7dM3f5aPM );
}

__kernel void idct_transform(__global float * pSrc, __global float * pDst, int height, int width)
{
	__local float tmp[64];
	__local float src[64];
	
	int lid = get_local_id(0);
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int lsy = get_local_size(0);
	int lsx = get_local_size(1);
	
	int srcIndex = gy*width + gx*8;
	
	__local float * localPtr = &src[lid*8];
	__global float * srcPtr = &pSrc[srcIndex];
	for(int i=0;i<8;i++)
	{
		*(localPtr++) = *(srcPtr++);
	}
	
	idct8x8(&src[8*lid], 1, &tmp[8*lid], 1);
	barrier(CLK_LOCAL_MEM_FENCE);
	idct8x8(&tmp[lid], 8, &src[lid], 8);
	barrier(CLK_LOCAL_MEM_FENCE);
	
	localPtr = &src[lid*8];
	__global float * dstPtr = &pDst[srcIndex];
	for(int i=0;i<8;i++)
	{
		*(dstPtr++) = *(localPtr++);
	}
}

#define LIMIT(v, l)	( (v) > (l) ? (l) : ((v) < -(l) ? -(l) : (v)) )

__kernel void quant_transform(__global float * pSrc, __global float * pDst, __global float * pQ, int height, int width)
{
	int ly = get_local_id(0);
	int lx = get_local_id(1);
	int gy = get_global_id(0);
	int gx = get_global_id(1);

	float q = pQ[ly*8+lx];
	int index = gy*width+gx;
	float d = q*pSrc[index];
	if(!ly && !lx) d = LIMIT(d, 2047.0f);
	else d = LIMIT(d, 1023.0f);	
	pDst[index] = d;
}

__kernel void iquant_transform(__global float * pSrc, __global float * pDst, __global float * pQ, int height, int width)
{
	int ly = get_local_id(0);
	int lx = get_local_id(1);
	int gy = get_global_id(0);
	int gx = get_global_id(1);

	float q = pQ[ly*8+lx];
	int index = gy*width+gx;
	float d = pSrc[index]*q;
	pDst[index] = d;
}


struct Point
{
	int X;
	int Y;
	int Z;
};

__constant unsigned char lut[] = {
 0,   1,   5,   6,  14,  15,  27,  28, 
 2,   4,   7,  13,  16,  26,  29,  42, 
 3,   8,  12,  17,  25,  30,  41,  43, 
 9,  11,  18,  24,  31,  40,  44,  53, 
10,  19,  23,  32,  39,  45,  52,  54, 
20,  22,  33,  38,  46,  51,  55,  60, 
21,  34,  37,  47,  50,  56,  59,  61, 
35,  36,  48,  49,  57,  58,  62,  63
};

struct Point getPoint(struct Point p)
{
	int index = p.Y * 8 + p.X;
	int new_index = lut[index];
	struct Point r;
	r.Y = new_index/8;
	r.X = new_index%8;
	return r;
}

__kernel void lut_transform_float_int16(__global float * pSrc, __global short * pDst, int height, int width)
{
	int ly = get_local_id(0);
	int lx = get_local_id(1);
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int y = gy-ly;
	int x = gx-lx;
	
	int index = gy*width+gx;
	struct Point p;
	p.Y = ly;
	p.X = lx;
	struct Point np = getPoint(p);
	int newindex = (y+np.Y)*width + (x+np.X);
	pDst[newindex] = (short)pSrc[index];
}

__kernel void shift(__global float * pSrc, __global float * pDst, int height, int width, float s)
{
	int ly = get_local_id(0);
	int lx = get_local_id(1);
	int gy = get_global_id(0);
	int gx = get_global_id(1);

	int index = gy*width+gx;
	pDst[index] = pSrc[index] + s;
}

typedef struct prediction_info
{
	int dx;
	int dy;
} prediction_info_t;

__kernel void copyBlock2Local(__global float * ptr, int width, __local float * dst)
{
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			dst[y*8+x] = ptr[y*width+x];
		}
	}

}

__kernel void copyBlock2Global(__global float * ptr, int width, __local float * dst)
{
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			ptr[y*width+x] = dst[y*8+x];
		}
	}

}

__kernel void prediction_transform(
		__global float * pSrc, 
		__global float * pDst, 
		__global float * pLast,
		__global prediction_info_t * pPred, 
		int width,
		int predWidth,
		int scale
		)
{
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int index = gy*width + gx;
	int py = gy/scale;
	int px = gx/scale;
	prediction_info_t info = pPred[py*predWidth+px];
	int y = gy + info.dy*scale;
	int x = gx + info.dx*scale;
	int lastIndex = y*width+x;
	float last = pLast[lastIndex];
	pDst[index] = pSrc[index] - last;
}

__kernel void prediction_transform_interpolation(
		__global float * pSrc, 
		__global float * pDst, 
		__global float * pLast,
		__global prediction_info_t * pPred, 
		int width,
		int predWidth,
		int blockScale,
		int lastWidth,
		int interpolScale
		)
{
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int index = gy*width + gx;
	int py = gy/blockScale;
	int px = gx/blockScale;
	prediction_info_t info = pPred[py*predWidth+px];
	int y = gy*interpolScale + info.dy*blockScale;
	int x = gx*interpolScale + info.dx*blockScale;
	int lastIndex = y*lastWidth+x;
	float last = pLast[lastIndex];
	float src = pSrc[index];
	float dst = src - last;
	pDst[index] = dst;
}

__kernel void prediction_itransform(
		__global float * pSrc, 
		__global float * pDst, 
		__global float * pLast,
		__global prediction_info_t * pPred, 
		int width,
		int predWidth,
		int scale
		)
{
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int index = gy*width + gx;
	int py = gy/scale;
	int px = gx/scale;
	prediction_info_t info = pPred[py*predWidth+px];
	int y = gy + info.dy*scale;
	int x = gx + info.dx*scale;
	int lastIndex = y*width+x;
	float last = pLast[lastIndex];
	pDst[index] = pSrc[index] + last;
}

__kernel void prediction_itransform_interpolation(
		__global float * pSrc, 
		__global float * pDst, 
		__global float * pLast,
		__global prediction_info_t * pPred, 
		int width,
		int predWidth,
		int blockScale,
		int lastWidth,
		int interpolScale
		)
{
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int index = gy*width + gx;
	int py = gy/blockScale;
	int px = gx/blockScale;
	prediction_info_t info = pPred[py*predWidth+px];
	int y = gy*interpolScale + info.dy*blockScale;
	int x = gx*interpolScale + info.dx*blockScale;
	int lastIndex = y*lastWidth+x;
	float last = pLast[lastIndex];
	float src = pSrc[index];
	float dst = src + last;
	pDst[index] = dst;
}

void copyGlobal2Array(__global float * ptr, int width, float * dst)
{
	for(int y=0;y<16;y++)
	{
		for(int x=0;x<16;x++)
		{
			dst[y*16+x] = ptr[y*width+x];
		}
	}

}

float diff_mse(float * block, __global float * ref, int refWidth)
{
	float ret = 0.0;
	int index = 0;
	for(int y=0;y<16;y++)
	{
		for(int x=0;x<16;x++)
		{
			float d = block[index] - ref[y*refWidth+x];
			ret += d*d;
			index++;
		}
	}
	return ret;
}

__kernel void prediction_predict(
		__global float * pSrc,
		__global float * pLast,
		__global prediction_info_t * pPred,
		int height,
		int width,
		int predHeight,
		int predWidth,
		int max_d
	)
{
	float block[16*16];
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int y = gy*16;
	int x = gx*16;
	copyGlobal2Array(&pSrc[y*width+x], width, block);
	struct 
	{
		int dy;
		int dx;
		float d;
	} _min;
	_min.dy = 0;
	_min.dx = 0;
	_min.d = MAXFLOAT;
	for(int dy = -max_d ; dy <= max_d ; dy++)
	{
		for(int dx=-max_d ; dx <= max_d ; dx++)
		{
			int dpy = y + dy*16;
			int dpx = x + dx*16;
			if(dpx >= 0 && dpy >= 0 && (dpy+16) < height && (dpx+16) < width)
			{
				float d = diff_mse(block, &pLast[dpy*width+dpx], width);
				if(d < _min.d)
				{
					_min.d = d;
					_min.dy = dy;
					_min.dx = dx;
				}
			}
		}
	}
	pPred[gy*predWidth+gx].dy = _min.dy;
	pPred[gy*predWidth+gx].dx = _min.dx;
}

__kernel void prediction_predict_interpolation(
		__global float * pSrc,
		__global float * pLast,
		__global prediction_info_t * pPred,
		int height,
		int width,
		int predHeight,
		int predWidth,
		int max_d,
		int lastHeight,
		int lastWidth,
		int scale
	)
{
	float block[16*16];
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int y = gy*16*scale;
	int x = gx*16*scale;
	copyGlobal2Array(&pSrc[y*width+x], width, block);
	struct 
	{
		int dy;
		int dx;
		float d;
	} _min;
	_min.dy = 0;
	_min.dx = 0;
	_min.d = MAXFLOAT;
	for(int dy = -max_d ; dy <= max_d ; dy++)
	{
		for(int dx=-max_d ; dx <= max_d ; dx++)
		{
			int dpy = y + dy*16;
			int dpx = x + dx*16;
			if(dpx >= 0 && dpy >= 0 && (dpy+16) < lastHeight && (dpx+16) < lastWidth)
			{
				float d = diff_mse(block, &pLast[dpy*lastWidth+dpx], lastWidth);
				if(d < _min.d)
				{
					_min.d = d;
					_min.dy = dy;
					_min.dx = dx;
				}
			}
		}
	}
	pPred[gy*predWidth+gx].dy = _min.dy;
	pPred[gy*predWidth+gx].dx = _min.dx;
}

__kernel void interpolation_float(
		__global float * pSrc,
		int srcHeight,
		int srcWidth,
		__global float * pDst,
		int dstHeight,
		int dstWidth,
		int scale
		)
{
	int y = get_global_id(0);
	int x = get_global_id(1);
	int dstIndex = y*dstWidth+x;
	int srcIndex = y/scale*srcWidth + x/scale;
	pDst[dstIndex] = pSrc[srcIndex];	
}

__kernel void dctqzz_transform(
		__global float * pSrc, 
//		__global float * pQDst,
		__global short * pDst, 
		__global float * pQ, 
		int height, 
		int width
		)
{
	__local float tmp[64];
	__local float src[64];
	__local short dst[64];
	float q[8];
	float q0 = pQ[0];

	int lid = get_local_id(0);
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int y = get_group_id(0)*8;
	int x = get_group_id(1)*8;

	int srcIndex = gy*width + gx*8;
	
	__local float * localPtr = &src[lid*8];
	__global float * srcPtr = &pSrc[srcIndex];
	__global float * qPtr = &pQ[lid*8];
	for(int i=0;i<8;i++)
	{
		*(localPtr++) = *(srcPtr++);
		q[i] = *(qPtr++);
	}

	// DCT
	dct8x8(&src[8*lid], 1, &tmp[8*lid], 1);
	barrier(CLK_LOCAL_MEM_FENCE);
	dct8x8(&tmp[lid], 8, &src[lid], 8);
	barrier(CLK_LOCAL_MEM_FENCE);

	//Quantization
	int i=0;
	localPtr = &src[lid*8];

	if(!lid)
	{
		float d = *localPtr;
		d = d*q0;
		d = LIMIT(d, 2047.0f);
		*(localPtr++) = d;
		i=1;

	}

	for(;i<8;i++)
	{
		float d = *localPtr;
		d = d*q[i];
		d = LIMIT(d, 1023.0f);
		*(localPtr++) = d;
	}

	// Copy Quantized data
	localPtr = &src[lid*8];
	__global float * qdstPtr = &pSrc[srcIndex];
	for(int i=0;i<8;i++)
	{
		*(qdstPtr++) = *(localPtr++);
	}

	// ZigZag
	for(int i=0;i<8;i++)
	{
		struct Point p;
		p.Y = lid;
		p.X = i;
		struct Point np = getPoint(p);
		dst[np.Y*8+np.X] = (short)src[8*lid+i];
	}
	
	// Copy ZigZag data
	__local short * localDstPtr = &dst[lid*8];
	__global short * globalDstPtr = &pDst[srcIndex];
	for(int i=0;i<8;i++)
	{
		*(globalDstPtr++) = *(localDstPtr++);
	}
}

__kernel void idctq_transform(
		__global float * pSrc, 
		__global float * pDst, 
		__global float * pQ, 
		int height, 
		int width
		)
{
	__local float tmp[64];
	__local float src[64];
	float q[8];	
	
	int lid = get_local_id(0);
	int gy = get_global_id(0);
	int gx = get_global_id(1);
	int lsy = get_local_size(0);
	int lsx = get_local_size(1);
	
	int srcIndex = gy*width + gx*8;
	
	__local float * localPtr = &src[lid*8];
	__global float * srcPtr = &pSrc[srcIndex];
	__global float * qPtr = &pQ[lid*8];
	for(int i=0;i<8;i++)
	{
		*(localPtr++) = *(srcPtr++);
		q[i] = *(qPtr++);
	}
	
	localPtr = &src[lid*8];
	for(int i=0;i<8;i++)
	{
		float d = *localPtr;
		d = d*q[i];
		*(localPtr++) = d;
	}

	idct8x8(&src[8*lid], 1, &tmp[8*lid], 1);
	barrier(CLK_LOCAL_MEM_FENCE);
	idct8x8(&tmp[lid], 8, &src[lid], 8);
	barrier(CLK_LOCAL_MEM_FENCE);

	localPtr = &src[lid*8];
	srcPtr = &pDst[srcIndex];
	for(int i=0;i<8;i++)
	{
		*(srcPtr++) = *(localPtr++);
	}
}
