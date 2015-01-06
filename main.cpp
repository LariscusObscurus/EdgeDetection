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

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "ImageProcessing.h"
#include "Debug.h"

int main() 
{
        auto test = CalculateLaplacianOfGaussianKernel(7, 1.4);
        printKernel(test);
        
        cv::namedWindow( "Test 1");
        
        auto src = cv::imread("TestData/lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        
        auto dst = ImageConvolute(src, test);
        auto dst2 = FindZeroCrossings(src);
        
        //auto dst2 = ApplyEdgeDetection(src, sobel_h_kernel, sobel_v_kernel);
        //auto dst2 = ApplyRobertsEdgeDetection(*dst);
        //dst = ThresholdImage(*dst, 0);
        cv::imshow("Test 1", *dst2);
        cv::waitKey(0);
        return 0;
}
