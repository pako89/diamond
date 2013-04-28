#ifndef _CL_DEVICE_INFO_H
#define _CL_DEVICE_INFO_H

#include <CL/opencl.h>
#include <string>
#include <vector>

class CCLDeviceInfo
{
public:
	CCLDeviceInfo(void);
	CCLDeviceInfo(cl_platform_id platform_id,cl_device_id device_id);
	~CCLDeviceInfo(void);
	void setIds(cl_platform_id platform_id,cl_device_id device_id);

	std::string getName();
	std::string getProfile();
	std::string getVendor();
	std::string getDeviceVersion();
	std::string getDriverVersion();
	std::string getExtensions();
	size_t getImage2DMaxHeight();
	size_t getImage2DMaxWidth();
	size_t getImage3DMaxDepth();
	size_t getImage3DMaxHeight();
	size_t getImage3DMaxWidth();
	size_t getMaxParameterSize();
	size_t getMaxWorkGroupSize();
	size_t getProfilingTimerResolution();
	cl_bool getAvailable();
	cl_bool getCompilerAvailable();
	cl_bool getEndianLittle();
	cl_bool getErrorCorrectionSupport();
	cl_bool getImageSupport();
	cl_uint getAddressBits();
	cl_uint getMaxClockFrequency();
	cl_uint getMaxComputeUnits();
	cl_uint getMaxConstantArgs();
	cl_uint getMaxReadImageArgs();
	cl_uint getMaxSamplers();
	cl_uint getMaxWorkItemDimensions();
	cl_uint getMaxWriteImageArgs();
	cl_uint getMemBaseAddrAlign();
	cl_uint getMinDataTypeAlignSize();
	cl_uint getPrefferedVectorWidthChar();
	cl_uint getPrefferedVectorWidthShort();
	cl_uint getPrefferedVectorWidthInt();
	cl_uint getPrefferedVectorWidthLong();
	cl_uint getPrefferedVectorWidthFloat();
	cl_uint getPrefferedVectorWidthDouble();
	cl_uint getVendorId();
	cl_uint getGlobalMemCacheLineSize();
	cl_ulong getGlobalMemSize();
	cl_ulong getGlobalMemCacheSize();
	cl_ulong getLocalMemSize();
	cl_ulong getMaxConstBuffSize();
	cl_ulong getMaxMemAllocSize();
	cl_device_type getDeviceType();
	cl_device_fp_config getDoubleFpConfig();
	cl_device_fp_config getSingleFpConfig();
	cl_device_fp_config getHalfFpConfig();
	cl_device_mem_cache_type getGlobalMemCacheType();
	cl_device_local_mem_type getLocalMemType();
	cl_device_exec_capabilities getExecutionCapabilities();
	cl_command_queue_properties getQueueProperties();
	std::vector<size_t> getMaxWorkItemSizes();
protected:
	cl_uint getInfo_uint(cl_device_info info);
	cl_bool getInfo_bool(cl_device_info info);
	cl_device_fp_config getInfo_fp(cl_device_info info);
	cl_device_exec_capabilities getInfo_exec_capabilities(cl_device_info info);
	std::string getInfo_string(cl_device_info info);
	cl_ulong getInfo_ulong(cl_device_info info);
	cl_device_mem_cache_type getInfo_mem_cache_type(cl_device_info info);
	size_t getInfo_size_t(cl_device_info info);
	cl_device_local_mem_type getInfo_local_mem_type(cl_device_info info);
	cl_command_queue_properties getInfo_command_queue_properties(cl_device_info info);	
	cl_platform_id platform_id;
	cl_device_id device_id;
};

#endif //_CL_DEVICE_INFO_H
