const sampler_t s_nearest = CLK_FILTER_NEAREST | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE;

__kernel void
square(
   __write_only image2d_t output_image,
   __read_only image2d_t input_image)
{
   // int2 coord = (int2)(get_global_id(0),  get_global_id(1));
   int const height = 4000;
   int const width = 3000;

   for (int y = 0; y < height/2; y++) {
      for (int x = 0; x < width/2; x++) {
         int2 coord = (int2)(y, x);
         float4 out_color = (float4)(255.0, 255.0, 255.0, 255.0);
         write_imagef(output_image, coord, out_color);
      }
   }

#if 0
   float val = read_imagef(input_image, s_nearest, coord).x;
   float tmp = val * val / 255;
   // float4 out_color = (float4)(tmp, 0.0f, 0.0f, 0.0f);
   float4 out_color = (float4)(255.0, 255.0, 255.0, 255.0);
   write_imagef(output_image, coord, out_color);
#endif
}


__kernel void gaussian_filter(__read_only image2d_t srcImg,
                              __write_only image2d_t dstImg,
                              sampler_t sampler,
                              int width, int height)
{
    // Gaussian Kernel is:
    // 1  2  1
    // 2  4  2
    // 1  2  1
    float kernelWeights[9] = { 1.0f, 2.0f, 1.0f,
                               2.0f, 4.0f, 2.0f,
                               1.0f, 2.0f, 1.0f };

    int2 startImageCoord = (int2) (get_global_id(0) - 1, get_global_id(1) - 1);
    int2 endImageCoord   = (int2) (get_global_id(0) + 1, get_global_id(1) + 1);
    int2 outImageCoord = (int2) (get_global_id(0), get_global_id(1));

    if (outImageCoord.x < width && outImageCoord.y < height)
    {
        int weight = 0;
        float4 outColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
        for( int y = startImageCoord.y; y <= endImageCoord.y; y++)
        {
            for( int x = startImageCoord.x; x <= endImageCoord.x; x++)
            {
                outColor += (read_imagef(srcImg, sampler, (int2)(x, y)) * (kernelWeights[weight] / 16.0f));
                weight += 1;
            }
        }

        // Write the output value to image
        write_imagef(dstImg, outImageCoord, outColor);
    }
}

