__constant float s1 = 0.195090322f;
__constant float s2 = 0.3826834324f;
__constant float s3 = 0.555570233f;
__constant float c1 = 0.9807852804f;
__constant float c2 = 0.9238795325f;
__constant float c3 = 0.8314696123f;
__constant float c4 = 0.7071067812f;

__kernel void dct8x8(__local float *pSrc, int srcStep, __local float * pDst, dstStep)
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
	int gy = get_group_id(0);
	int gx = get_group_id(1);
	int lsy = get_local_size(0);
	int lsx = get_local_size(1);
	
	int srcIndex = gy*8*width + gx*8;
	
	__local float * localPtr = &src[lid*8];
	__global float * srcPtr = &pSrc[srcIndex + lid*width];
	for(int i=0;i<8;i++)
	{
		*(localPtr++) = *(srcPtr++);
	}
	
	dct8x8(&src[8*lid], 1, &tmp[8*lid], 1);
	barrier(CLK_LOCAL_MEM_FENCE);
	dct8x8(&tmp[lid], 8, &src[lid], 8);
	barrier(CLK_LOCAL_MEM_FENCE);
	
	localPtr = &src[lid*8];
	__global float * dstPtr = &pDst[srcIndex + lid*width];
	for(int i=0;i<8;i++)
	{
		*(dstPtr++) = *(localPtr++);
	}
}

