#include <cl_device_info.h>

template <class T>
T getInfo_type(cl_device_id dev_id,cl_device_info info)
{
	cl_int err;
	size_t size;
	T ret = 0;
	err = clGetDeviceInfo(dev_id,info,0,NULL,&size);
	if(CL_SUCCESS != err || sizeof(T) != size)
	{
		return ret;
	}
	err = clGetDeviceInfo(dev_id,info,size,&ret,NULL);
	if(CL_SUCCESS != err)
	{
		return ret;
	}

	return ret;
}

template <class T>
T * getInfo_typeArray(cl_device_id dev_id,cl_device_info info,size_t * num)
{
	cl_int err;
	size_t size;
	err = clGetDeviceInfo(dev_id,info,0,NULL,&size);
	if(CL_SUCCESS != err || 0 == size)
	{
		return NULL;
	}
	*num = size/sizeof(T);
	T * ptr = new T[(*num)];
	if(NULL == ptr)
	{
		return NULL;
	}
	err = clGetDeviceInfo(dev_id,info,size,ptr,NULL);
	if(CL_SUCCESS != err)
	{
		delete [] ptr;
		return NULL;
	}
	return ptr;
}


CCLDeviceInfo::CCLDeviceInfo(void)
{
}

CCLDeviceInfo::CCLDeviceInfo(cl_platform_id platform_id, cl_device_id device_id)
{
	this->setIds(platform_id,device_id);
}

CCLDeviceInfo::~CCLDeviceInfo(void)
{
}

void CCLDeviceInfo::setIds(cl_platform_id platform_id,cl_device_id device_id)
{
	this->platform_id = platform_id;
	this->device_id = device_id;
}

std::string CCLDeviceInfo::getExtensions()
{
	return this->getInfo_string(CL_DEVICE_EXTENSIONS);
}

std::string CCLDeviceInfo::getName()
{
	return this->getInfo_string(CL_DEVICE_NAME);
}

std::string CCLDeviceInfo::getProfile()
{
	return this->getInfo_string(CL_DEVICE_PROFILE);
}

std::string CCLDeviceInfo::getVendor()
{
	return this->getInfo_string(CL_DEVICE_VENDOR);
}

std::string CCLDeviceInfo::getDeviceVersion()
{
	return this->getInfo_string(CL_DEVICE_VERSION);
}

std::string CCLDeviceInfo::getDriverVersion()
{
	return this->getInfo_string(CL_DRIVER_VERSION);
}

cl_ulong CCLDeviceInfo::getGlobalMemSize()
{
	return getInfo_type<cl_ulong>(this->device_id,CL_DEVICE_GLOBAL_MEM_SIZE);
}

cl_ulong CCLDeviceInfo::getGlobalMemCacheSize()
{
	return getInfo_type<cl_ulong>(this->device_id,CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
}

cl_ulong CCLDeviceInfo::getLocalMemSize()
{
	return getInfo_type<cl_ulong>(this->device_id,CL_DEVICE_LOCAL_MEM_SIZE);
}

cl_ulong CCLDeviceInfo::getMaxConstBuffSize()
{
	return getInfo_type<cl_ulong>(this->device_id,CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
}

cl_ulong CCLDeviceInfo::getMaxMemAllocSize()
{
	return getInfo_type<cl_ulong>(this->device_id,CL_DEVICE_MAX_MEM_ALLOC_SIZE);
}

cl_uint CCLDeviceInfo::getAddressBits()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_ADDRESS_BITS);
}

cl_bool CCLDeviceInfo::getAvailable()
{
	return getInfo_type<cl_bool>(this->device_id,CL_DEVICE_AVAILABLE);
}

cl_bool CCLDeviceInfo::getCompilerAvailable()
{
	return getInfo_type<cl_bool>(this->device_id,CL_DEVICE_COMPILER_AVAILABLE);
}

cl_device_fp_config CCLDeviceInfo::getDoubleFpConfig()
{
	return getInfo_type<cl_device_fp_config>(this->device_id,CL_DEVICE_DOUBLE_FP_CONFIG);
}

cl_bool CCLDeviceInfo::getEndianLittle()
{
	return getInfo_type<cl_bool>(this->device_id,CL_DEVICE_ENDIAN_LITTLE);
}

cl_bool CCLDeviceInfo::getErrorCorrectionSupport()
{
	return getInfo_type<cl_bool>(this->device_id,CL_DEVICE_ERROR_CORRECTION_SUPPORT);
}

cl_device_exec_capabilities CCLDeviceInfo::getExecutionCapabilities()
{
	return getInfo_type<cl_device_exec_capabilities>(this->device_id,CL_DEVICE_EXECUTION_CAPABILITIES);
}

cl_device_mem_cache_type CCLDeviceInfo::getGlobalMemCacheType()
{
	return getInfo_type<cl_device_mem_cache_type>(this->device_id,CL_DEVICE_GLOBAL_MEM_CACHE_TYPE);
}

cl_uint CCLDeviceInfo::getGlobalMemCacheLineSize()
{
	return getInfo_type<cl_uint >(this->device_id,CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
}

cl_device_fp_config CCLDeviceInfo::getHalfFpConfig()
{
	return getInfo_type<cl_device_fp_config>(this->device_id,CL_DEVICE_HALF_FP_CONFIG);
}

cl_bool CCLDeviceInfo::getImageSupport()
{
	return getInfo_type<cl_bool>(this->device_id,CL_DEVICE_IMAGE_SUPPORT);
}

size_t CCLDeviceInfo::getImage2DMaxHeight()
{
	return getInfo_type<size_t>(this->device_id,CL_DEVICE_IMAGE2D_MAX_HEIGHT);
}

size_t CCLDeviceInfo::getImage2DMaxWidth()
{
	return getInfo_type<size_t>(this->device_id,CL_DEVICE_IMAGE2D_MAX_WIDTH);
}
size_t CCLDeviceInfo::getImage3DMaxDepth()
{
	return getInfo_type<size_t>(this->device_id,CL_DEVICE_IMAGE3D_MAX_DEPTH);
}
size_t CCLDeviceInfo::getImage3DMaxHeight()
{
	return getInfo_type<size_t>(this->device_id,CL_DEVICE_IMAGE3D_MAX_HEIGHT);
}

size_t CCLDeviceInfo::getImage3DMaxWidth()
{
	return getInfo_type<size_t>(this->device_id,CL_DEVICE_IMAGE3D_MAX_WIDTH);
}

cl_device_local_mem_type CCLDeviceInfo::getLocalMemType()
{
		return getInfo_type<cl_device_local_mem_type>(this->device_id,CL_DEVICE_LOCAL_MEM_TYPE);
}

cl_uint CCLDeviceInfo::getMaxClockFrequency()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_MAX_CLOCK_FREQUENCY);
}

cl_uint CCLDeviceInfo::getMaxComputeUnits()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_MAX_COMPUTE_UNITS);
}

cl_uint CCLDeviceInfo::getMaxConstantArgs()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_MAX_CONSTANT_ARGS);
}

size_t CCLDeviceInfo::getMaxParameterSize()
{
	return getInfo_type<size_t >(this->device_id,CL_DEVICE_MAX_PARAMETER_SIZE);
}

cl_uint CCLDeviceInfo::getMaxReadImageArgs()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_MAX_READ_IMAGE_ARGS);
}

cl_uint CCLDeviceInfo::getMaxSamplers()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_MAX_SAMPLERS);
}

size_t CCLDeviceInfo::getMaxWorkGroupSize()
{
	return getInfo_type<size_t>(this->device_id,CL_DEVICE_MAX_WORK_GROUP_SIZE);
}

cl_uint CCLDeviceInfo::getMaxWorkItemDimensions()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
}

cl_uint CCLDeviceInfo::getMaxWriteImageArgs()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
}

cl_uint CCLDeviceInfo::getMemBaseAddrAlign()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_MEM_BASE_ADDR_ALIGN);
}

cl_uint CCLDeviceInfo::getMinDataTypeAlignSize()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
}

cl_uint CCLDeviceInfo::getPrefferedVectorWidthChar()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR);
}

cl_uint CCLDeviceInfo::getPrefferedVectorWidthShort()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT);
}

cl_uint CCLDeviceInfo::getPrefferedVectorWidthInt()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT);
}

cl_uint CCLDeviceInfo::getPrefferedVectorWidthLong()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG);
}

cl_uint CCLDeviceInfo::getPrefferedVectorWidthFloat()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT);
}

cl_uint CCLDeviceInfo::getPrefferedVectorWidthDouble()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
}
size_t CCLDeviceInfo::getProfilingTimerResolution()
{
	return getInfo_type<size_t>(this->device_id,CL_DEVICE_PROFILING_TIMER_RESOLUTION);
}

cl_command_queue_properties CCLDeviceInfo::getQueueProperties()
{
	return getInfo_type<cl_command_queue_properties>(this->device_id,CL_DEVICE_QUEUE_PROPERTIES);
}

cl_device_fp_config CCLDeviceInfo::getSingleFpConfig()
{
	return getInfo_type<cl_device_fp_config>(this->device_id,CL_DEVICE_SINGLE_FP_CONFIG);
}

cl_device_type CCLDeviceInfo::getDeviceType()
{
	return getInfo_type<cl_device_type>(this->device_id,CL_DEVICE_TYPE);
}

cl_uint CCLDeviceInfo::getVendorId()
{
	return getInfo_type<cl_uint>(this->device_id,CL_DEVICE_VENDOR_ID);
}

std::vector<size_t> CCLDeviceInfo::getMaxWorkItemSizes()
{
	size_t num;
	size_t * ptr = getInfo_typeArray<size_t>(this->device_id,CL_DEVICE_MAX_WORK_ITEM_SIZES,&num);
	std::vector<size_t> sizes(num);
	for(int i= 0 ; i < num ; i++)
		sizes[i] = ptr[i];
	delete [] ptr;
	return sizes;
}

std::string CCLDeviceInfo::getInfo_string(cl_device_info info)
{
	size_t num;
	char * c_str = getInfo_typeArray<char>(this->device_id,info,&num);
	std::string str(c_str);
	delete [] c_str;
	return str;
}
