#include <image.h>

namespace avlib
{

template <class T>
CImage<T>::CImage()
{
}

template <class T>
CImage<T>::CImage(enum ImageType type, int height, int width)
{
}

template <class T>
CImage<T>::~CImage()
{
}

template class CImage<int>;
template class CImage<float>;
}
