#include <iostream>
#include <opencv2/opencv.hpp>
#include <omp.h>


#define USE_OPENMP


void square_naive(cv::Mat &output_img, cv::Mat &input_img) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    int x, y;
#ifdef USE_OPENMP
    #pragma omp parallel for private(x, y)
#endif
    for (y = 0; y < rows; y++) {
        uchar *p_input_img = input_img.ptr<uchar>(y);
        uchar *p_output_img = output_img.ptr<uchar>(y);
        for (x = 0; x < cols; x++) {
            uchar val = p_input_img[0];
            p_output_img[0] = val * val / 255;
            p_input_img++;
            p_output_img++;
        }
    }
}


int main() {
#ifdef USE_OPENMP
    omp_set_num_threads(8);
    std::cout << "Number of threads: " << omp_get_max_threads() << std::endl;
    std::cout << "Number of processor cores: " << omp_get_num_procs() << std::endl;
#endif

    /* Read input image */
    std::string input_file = "../images/beer.jpg";
    cv::Mat input_img = cv::imread(input_file, cv::IMREAD_GRAYSCALE);
    if (input_img.empty() == true) {
        std::cout << "There is no input image: " << input_file << std::endl;
        return 1;
    }
    cv::Mat output_img = cv::Mat::zeros(input_img.size(), input_img.type());
    std::cout << "input size: " << input_img.cols << " x " << input_img.rows << std::endl;

    /* Measure processing time */
    cv::TickMeter meter;
    int loop = 10;

    for (int i = 0; i < loop; i++) {
        meter.start();
        square_naive(output_img, input_img);
        meter.stop();
    }

    std::cout << "average time: " << meter.getAvgTimeMilli() << "[ms]" << std::endl;

    /* Save output image */
    cv::imwrite("../images/beer_square.jpg", output_img);
    return 0;
}
