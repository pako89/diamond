#include <dct.h>

namespace avlib
{

const float CDCT::s1 = 0.195090322f;
const float CDCT::s2 = 0.3826834324f;
const float CDCT::s3 = 0.555570233f;
const float CDCT::c1 = 0.9807852804f;
const float CDCT::c2 = 0.9238795325f;
const float CDCT::c3 = 0.8314696123f;
const float CDCT::c4 = 0.7071067812f;

CDCT::CDCT()
{
}

CDCT::~CDCT()
{
}

void CDCT::TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s)
{
	float temp[64];
	float * src = &pSrc[p.Y*s.Width+p.X];
	float * dst = &pDst[p.Y*s.Width+p.X];
	for(int i=0;i<8;i++)
	{
		Transform8(&src[i*s.Width], 1, &temp[i*8], 1);
	}
	for(int i=0;i<8;i++)
	{	
		Transform8(&temp[i], 8, &dst[i], s.Width);
	}
}

void CDCT::Transform8(float * pSrc, int srcStep, float * pdst, int dstStep)
{
	if(!pSrc || !pdst)
	{
		throw utils::NullReferenceException();
	}
	
	float * srcVector0 = pSrc;
	float * srcVector1 = srcVector0 + srcStep;
        float * srcVector2 = srcVector1 + srcStep;
        float * srcVector3 = srcVector2 + srcStep;
        float * srcVector4 = srcVector3 + srcStep;
        float * srcVector5 = srcVector4 + srcStep;
        float * srcVector6 = srcVector5 + srcStep;
        float * srcVector7 = srcVector6 + srcStep;

	float * dstVector0 = pdst;
	float * dstVector1 = dstVector0 + dstStep;
        float * dstVector2 = dstVector1 + dstStep;
        float * dstVector3 = dstVector2 + dstStep;
        float * dstVector4 = dstVector3 + dstStep;
        float * dstVector5 = dstVector4 + dstStep;
        float * dstVector6 = dstVector5 + dstStep;
        float * dstVector7 = dstVector6 + dstStep;
	
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

const float CIDCT::c1 = 1.387039845322148f;
const float CIDCT::c2 = 1.306562964876377f;
const float CIDCT::c3 = 1.175875602419359f;
const float CIDCT::c4 = 0.785694958387102f;
const float CIDCT::c5 = 0.541196100146197f;
const float CIDCT::c6 = 0.275899379282943f;
const float CIDCT::cn = 0.5*0.3535533905932737f;


CIDCT::CIDCT()
{
}

CIDCT::~CIDCT()
{
}

void CIDCT::TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s)
{
	float temp[64];
	float * src = &pSrc[p.Y*s.Width+p.X];
	float * dst = &pDst[p.Y*s.Width+p.X];
	for(int i=0;i<8;i++)
	{
		Transform8(&src[i], s.Width, &temp[i], 8);
	}
	for(int i=0;i<8;i++)
	{	
		Transform8(&temp[i*8], 1, &dst[i*s.Width], 1);
	}
}

void CIDCT::Transform8(float * pSrc, int srcStep, float * pdst, int dstStep)
{
	if(!pSrc || !pdst)
	{
		throw utils::NullReferenceException();
	}
	
	float * srcVector0 = pSrc;
	float * srcVector1 = srcVector0 + srcStep;
        float * srcVector2 = srcVector1 + srcStep;
        float * srcVector3 = srcVector2 + srcStep;
        float * srcVector4 = srcVector3 + srcStep;
        float * srcVector5 = srcVector4 + srcStep;
        float * srcVector6 = srcVector5 + srcStep;
        float * srcVector7 = srcVector6 + srcStep;

	float * dstVector0 = pdst;
        float * dstVector1 = dstVector0 + dstStep;
        float * dstVector2 = dstVector1 + dstStep;
        float * dstVector3 = dstVector2 + dstStep;
        float * dstVector4 = dstVector3 + dstStep;
        float * dstVector5 = dstVector4 + dstStep;
        float * dstVector6 = dstVector5 + dstStep;
        float * dstVector7 = dstVector6 + dstStep;

        float Y04P   = (*srcVector0) + (*srcVector4);
        float Y2b6eP = c2 * (*srcVector2) + c5 * (*srcVector6);

        float Y04P2b6ePP = Y04P + Y2b6eP;
        float Y04P2b6ePM = Y04P - Y2b6eP;


        float Y7f1aP3c5dPP = c6 * (*srcVector7) + c1 * (*srcVector1) + c3 * (*srcVector3) + c4 * (*srcVector5);
        float Y7a1fM3d5cMP = c1 * (*srcVector7) - c6 * (*srcVector1) + c4 * (*srcVector3) - c3 * (*srcVector5);

        float Y04M   = (*srcVector0) - (*srcVector4);
        float Y2e6bM = c5 * (*srcVector2) - c2 * (*srcVector6);

        float Y04M2e6bMP = Y04M + Y2e6bM;
        float Y04M2e6bMM = Y04M - Y2e6bM;

        float Y1c7dM3f5aPM = c3 * (*srcVector1) - c4 * (*srcVector7) - c6 * (*srcVector3) - c1 * (*srcVector5);
        float Y1d7cP3a5fMM = c4 * (*srcVector1) + c3 * (*srcVector7) - c1 * (*srcVector3) + c6 * (*srcVector5);

        (*dstVector0) = cn * ( Y04P2b6ePP + Y7f1aP3c5dPP );
        (*dstVector7) = cn * ( Y04P2b6ePP - Y7f1aP3c5dPP );
        (*dstVector4) = cn * ( Y04P2b6ePM + Y7a1fM3d5cMP );
        (*dstVector3) = cn * ( Y04P2b6ePM - Y7a1fM3d5cMP );

        (*dstVector1) = cn * ( Y04M2e6bMP + Y1c7dM3f5aPM );
        (*dstVector5) = cn * ( Y04M2e6bMM - Y1d7cP3a5fMM );
        (*dstVector2) = cn * ( Y04M2e6bMM + Y1d7cP3a5fMM );
        (*dstVector6) = cn * ( Y04M2e6bMP - Y1c7dM3f5aPM );

}

}
