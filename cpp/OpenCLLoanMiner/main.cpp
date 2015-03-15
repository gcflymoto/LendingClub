#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>

const std::string hw("Hello World\n");

inline void
checkErr(cl_int err, const char * name)
{
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    // Create the two input vectors
    const int LIST_SIZE = 1000;
    int* A = new int[LIST_SIZE];
    int* B = new int[LIST_SIZE];
    for (int i = 0; i < LIST_SIZE; i++) {
        A[i] = i;
        B[i] = LIST_SIZE - i;
    }

    try {

        // Get available platforms
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
    
        checkErr(all_platforms.size() != 0 ? CL_SUCCESS : !CL_SUCCESS, "cl::Platform::get returned empty platform list");
        std::cerr << "Platform number is: " << all_platforms.size() << '\n';
    
        std::string platformVendor;
        all_platforms[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
        std::cerr << "Platform is by: " << platformVendor << '\n';
        cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM, 
                                            (cl_context_properties)(all_platforms[0])(),
                                            0 }; 
    
        cl::Platform default_platform = all_platforms[0];
        std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << '\n';

        //get default device of the default platform
        std::vector<cl::Device> all_devices;
        default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

        checkErr(all_devices.size() != 0 ? CL_SUCCESS : !CL_SUCCESS, "No devices found. Check OpenCL installation");

        for (auto& device : all_devices) {
            std::cout << "Available device: " << device.getInfo<CL_DEVICE_NAME>() << '\n';
        }

        cl::Device default_device = all_devices[0];
        std::cout << "Using first device: " << default_device.getInfo<CL_DEVICE_NAME>() << '\n';

        cl::Context context({ default_device });

        //cl_int err;
        //cl::Context context(CL_DEVICE_TYPE_CPU, cprops, NULL, NULL, &err);
        //checkErr(err, "Conext::Context() creation failed");

        // Get a list of devices on this platform
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

        // Create a command queue and use the first device
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

        // Read source file
        std::ifstream sourceFile("vector_add_kernel.cl");
        std::string sourceCode(
            std::istreambuf_iterator<char>(sourceFile),
            (std::istreambuf_iterator<char>()));
        cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));

        // Make program of the source code in the context
        cl::Program program = cl::Program(context, source);

        // Build program for these specific devices
        program.build(devices);

        // Make kernel
        cl::Kernel kernel(program, "vector_add");

        // Create memory buffers
        cl::Buffer bufferA = cl::Buffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int));
        cl::Buffer bufferB = cl::Buffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int));
        cl::Buffer bufferC = cl::Buffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int));

        // Copy lists A and B to the memory buffers
        queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, LIST_SIZE * sizeof(int), A);
        queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, LIST_SIZE * sizeof(int), B);

        // Set arguments to kernel
        kernel.setArg(0, bufferA);
        kernel.setArg(1, bufferB);
        kernel.setArg(2, bufferC);

        // Run the kernel on specific ND range
        cl::NDRange global(LIST_SIZE);
        cl::NDRange local(1);
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);

        // Read buffer C into a local list
        int* C = new int[LIST_SIZE];
        queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, LIST_SIZE * sizeof(int), C);

        for (int i = 0; i < LIST_SIZE; i++)
            std::cout << A[i] << " + " << B[i] << " = " << C[i] << '\n';
    }
    catch (cl::Error error) {
        std::cout << error.what() << "(" << error.err() << ")" << std::endl;
    }

    return 0;
}