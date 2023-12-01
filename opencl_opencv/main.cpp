#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include "cl_util.h"
#include <opencv2/opencv.hpp>


#define MAX_PLATFORMS 8
#define MAX_DEVICES 8
#define MAX_SOURCE_SIZE (0x100000)

int main() {
    cl_int ret = CL_SUCCESS;
    int profiling = 1;

    cl_device_type specified_device_type = CL_DEVICE_TYPE_GPU; // CL_DEVICE_TYPE_CPU/CL_DEVICE_TYPE_GPU



    /* Platform -> Device */
    cl_platform_id platform_ids[MAX_PLATFORMS] = {0};
    cl_uint num_platforms = 0;
    CL_TRY(clGetPlatformIDs(MAX_PLATFORMS, platform_ids, &num_platforms));

    cl_device_id device_id[MAX_DEVICES] = {0};
    cl_uint num_devices = 0;
    int find_device = 0;
    for (int i = 0; i < num_platforms; i++) {
        cl_int ret_getDevice = clGetDeviceIDs(platform_ids[i], specified_device_type, MAX_DEVICES, device_id, &num_devices);
        if (ret_getDevice != CL_SUCCESS) {
            continue;
        }
        if (num_devices >= 1) {
            find_device = 1;
            break;
        }
    }
    if (!find_device) {
        printf("error: Cannot find specified device!\n");
        return 1;
    }

    /* Context */
    cl_context context = NULL;
    context = clCreateContext(NULL, 1, device_id, NULL, NULL, &ret);
    CL_TRY(ret);

    /* Command queue */
    cl_command_queue command_queue = NULL;
    if (profiling) {
        cl_command_queue_properties properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
        command_queue = clCreateCommandQueueWithProperties(context, device_id[0], properties, &ret);
    }
    else {
        const cl_queue_properties *properties = NULL;
        command_queue = clCreateCommandQueueWithProperties(context, device_id[0], properties, &ret);
    }
    CL_TRY(ret);



#if 0
    /* Memory Object */
    cl_mem memobj = NULL;
    int num_out = 9;
    memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, num_out*sizeof(cl_float4), NULL, &ret);
    // CL_TRY(clEnqueueWriteBuffer(command_queue, memobj, CL_TRUE, 0, LOCAL_MEM_SIZE*sizeof(int), memA, 0, NULL, NULL));
#endif



    std::string input_file = "../images/beer.jpg";
    cv::Mat input_img = cv::imread(input_file, cv::IMREAD_GRAYSCALE);
    if (input_img.empty() == true) {
        std::cout << "There is no input image: " << input_file << std::endl;
        return 1;
    }
    cv::Mat output_img = cv::Mat::zeros(input_img.size(), input_img.type());
    std::cout << "input size: " << input_img.cols << " x " << input_img.rows << std::endl;


printf("img\n");
    /* Image */
    int width = input_img.rows;
    int height = input_img.cols;
    cl_image_format image_format;
    image_format.image_channel_order = CL_R;
    image_format.image_channel_data_type = CL_UNSIGNED_INT8;
    cl_image_desc image_desc = {0};
    image_desc.image_type = CL_MEM_OBJECT_IMAGE2D;
    image_desc.image_width = width;
    image_desc.image_height = height;
    image_desc.buffer = NULL;
    cl_mem input_image = clCreateImage(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, &image_format, &image_desc, (void*)input_img.ptr<uchar>(0), &ret);
    CL_TRY(ret);
    // cl_mem output_image = clCreateImage(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, &image_format, &image_desc, (void*)output_img.ptr<uchar>(0), &ret);
    cl_mem output_image = clCreateImage(context, CL_MEM_WRITE_ONLY, &image_format, &image_desc, NULL, &ret);
    CL_TRY(ret);

printf("prog\n");
    /* Program */
    FILE *fp = NULL;
    char filename[] = "kernel.cl";
    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "error: Failed to load kernel file.\n");
        exit(1);
    }
    char *source_str = (char *)malloc(MAX_SOURCE_SIZE);
    size_t source_size = fread(source_str, sizeof(char), MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_program program = NULL;
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    CL_TRY(ret);
    CL_TRY(clBuildProgram(program, 1, device_id, NULL, NULL, NULL));

    /* Kernel */
    cl_kernel kernel = NULL;
    kernel = clCreateKernel(program, "square", &ret);
    CL_TRY(ret);
    CL_TRY(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&output_image));
    CL_TRY(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&input_image));

printf("befre\n");
    size_t global_work_size[3] = {1, 0, 0};
    size_t local_work_size[3] = {1, 0, 0};
    cl_event event_kernel;
    CL_TRY(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, &event_kernel));
    CL_TRY(clWaitForEvents(1, &event_kernel));
printf("after\n");


    size_t origin[] = {0, 0, 0};
    size_t region[] = {width, height, 1};
    CL_TRY((command_queue, output_image, CL_TRUE, origin, region, width*sizeof(u_char), 0, (void*)output_img.ptr<uchar>(0), 0, NULL, NULL));


    /* Release */
    CL_TRY(clFlush(command_queue));
    CL_TRY(clFinish(command_queue));


    cv::imwrite("../images/beer_sq_cl.jpg", output_img);

    if (profiling) {
        // print_time(event_write, "write");
        print_time(event_kernel, "kernel");
        // print_time(event_read, "read");
    }

    CL_TRY(clReleaseKernel(kernel));
    CL_TRY(clReleaseProgram(program));
    CL_TRY(clReleaseMemObject(input_image));
    CL_TRY(clReleaseMemObject(output_image));
    // CL_TRY(clReleaseEvent(event_write));
    CL_TRY(clReleaseEvent(event_kernel));
    // CL_TRY(clReleaseEvent(event_read));
    CL_TRY(clReleaseCommandQueue(command_queue));
    CL_TRY(clReleaseContext(context));

    free(source_str);

    return ret;
}