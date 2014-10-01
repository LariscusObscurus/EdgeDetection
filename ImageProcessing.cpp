/**  This file is part of EdgeDetection
 * 
 *  Copyright (C) 2014 Leonhardt Schwarz <if12b076@technikum-wien.at>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "ImageProcessing.h"
#include <cmath>
#include <exception>


inline static uchar CheckBorderGetPixel(cv::Mat src, int x, int y, int ch) {
        if ((x < 0) || (x >= src.cols) || (y < 0) || (y >= src.rows)) {
                return 0;
        } else {
                return src.at<cv::Vec3b>(y, x)[ch];
        }
}

std::unique_ptr<cv::Mat> ImageConvolute(cv::Mat& src, const std::vector<std::vector<int>>& kernel, const double multiplier = 1)
{
        auto outImage = std::unique_ptr<cv::Mat>(new cv::Mat(src.rows, src.cols, CV_8UC3));
        int size = kernel.size() / 2;
        for (int x = 0; x < src.cols ; x++)
        {
                for (int y = 0; y < src.rows; y++) {
                        double accumulator[3] = {0};
                        
                        for (int kx = -size; kx <= size; kx++) {
                                for (int ky = -size; ky <= size; ky++) {
                                        
                                       for (int ch = 0; ch < 3; ch++) {
                                               accumulator[ch] += multiplier * CheckBorderGetPixel(src, x + kx, y + ky, ch) * kernel[ky + size][kx + size];
                                       }
                                        
                                }
                                
                        }
                       cv::Vec3b pixel(0,0,0); 
                       for(int ch = 0; ch < 3; ch++) {
                               if(accumulator[ch] > 255)
                                       accumulator[ch] = 255;
                               else if (accumulator[ch] < 0)
                                       accumulator[ch] = 0;
                              pixel[ch] = static_cast<uchar>(accumulator[ch]); 
                       }
                       outImage->at<cv::Vec3b>(y, x) = pixel;
                       
                }
                
        }
        return outImage;
}

std::unique_ptr<cv::Mat> ApplyEdgeDetection(cv::Mat& src, const std::vector<std::vector<int>>& hKernel, const std::vector<std::vector<int>>& vKernel) 
{
        
        auto outImage = std::unique_ptr<cv::Mat>(new cv::Mat(src.rows, src.cols, CV_8UC3));
        int size = hKernel.size() / 2; //TODO: catch kernels of non equal size
        
        for (int x = 0; x < src.cols ; x++)
        {
                for (int y = 0; y < src.rows; y++) {
                        double hAccumulator[3] = {0};
                        double vAccumulator[3] = {0};
                        
                        for (int kx = -size; kx <= size; kx++) {
                                for (int ky = -size; ky <= size; ky++) {
                                        
                                       for (int ch = 0; ch < 3; ch++) {
                                               //auto tmp = src.at<cv::Vec3b>(y, x)[ch];
                                               hAccumulator[ch] += CheckBorderGetPixel(src, x + kx, y + ky, ch) * hKernel[ky + size][kx + size];
                                               vAccumulator[ch] += CheckBorderGetPixel(src, x + kx, y + ky, ch) * vKernel[ky + size][kx + size];
                                       }
                                        
                                }
                                
                        }
                       cv::Vec3b pixel(0,0,0); 
                       for(int ch = 0; ch < 3; ch++) {
                               
                               auto tmp = sqrt((hAccumulator[ch] * hAccumulator[ch]) + (vAccumulator[ch] * vAccumulator[ch]));
                               
                               if(tmp > 255)
                                       tmp = 255;
                               else if (tmp < 0)
                                       tmp = 0;
                               
                              pixel[ch] = static_cast<uchar>(tmp); 
                       }
                       outImage->at<cv::Vec3b>(y, x) = pixel;
                       
                }
                
        }
        return outImage;
        
}

std::vector<std::vector<int>> CalculateLaplacianOfGaussianKernel(int size, double sigma)
{
        if((size % 2) == 0) { std::runtime_error("Only even numbers are supported."); } //TODO: Richtige Exception Class schreiben
        
        std::vector<std::vector<int>> result(size, std::vector<int> (size, 0));
        int range = size / 2;
        
        int xcount = 0;
        for (int x = -range; x <= range; x++) {
                int ycount = 0;
                for (int y = -range; y <= range; y++) {
                        double tmp = (pow(x, 2) + pow(y, 2)) / (2.0 * pow(sigma, 2));
                        double val = -1.0 / (M_PI * pow(sigma, 4)) * (1.0 - tmp) * exp(-tmp);
                        result[xcount][ycount] = static_cast<int>(round(val*480));
                        ycount++;
                }
                xcount++;
        }
        
        return result;
}

std::vector<std::vector<int>> CalculateGaussianKernel(int size, double sigma)
{
        std::vector<std::vector<int>> result(size, std::vector<int> (size, 0));
        int range = size / 2;
        
        int xcount = 0;
        for (int x = -range; x <= range; x++) {
                int ycount = 0;
                for (int y = -range; y <= range; y++) {
                        double tmp = (pow(x, 2) + pow(y, 2)) / (2.0 * pow(sigma, 2));
                        double val = (1.0 / (2* M_PI* pow(sigma, 2))) * exp(-tmp);
                        result[xcount][ycount] = static_cast<int>(round(val*273));
                        ycount++;
                }
                xcount++;
        }
        
        return result;
        
}

std::unique_ptr<cv::Mat> ThresholdImage(cv::Mat& src, int threshold)
{
        auto outImage = std::unique_ptr<cv::Mat>(new cv::Mat(src.rows, src.cols, CV_8UC3));
        for (int x = 0; x < src.cols ; x++)
        {
                for (int y = 0; y < src.rows; y++) {
                               auto tmp = src.at<cv::Vec3b>(y, x);
                               for(int ch = 0; ch < 3; ch++) {
                                       if(tmp[ch] > threshold)
                                               tmp[ch] = 255;
                                       else
                                               tmp[ch] = 0;
                               }
                               outImage->at<cv::Vec3b>(y,x) = tmp;
                }
                
                
        }
        return outImage;
        
}
