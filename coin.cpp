
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;
Vec3b RandomColor(int value);

int main(int argc, char** argv)
{
    // Display
    Mat img;
    Mat gray;
    Mat bin, bin2;
    Mat pixelized;
    Mat normalized;

    img = imread("../coins.jpg");
    if (!img.data) {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Coin Image", CV_WINDOW_AUTOSIZE);
    imshow("Display Coin Image", img);

    // Convert to grayscale and siplay

    cvtColor(img, gray, CV_BGR2GRAY);

    imwrite("../gray.jpg", gray);
    namedWindow("Gray image", CV_WINDOW_AUTOSIZE);
    imshow("Gray image", gray);

    // Convert to binary

    threshold(gray, bin, 40, 255, THRESH_BINARY);
    imwrite("../bin.jpg", bin);
    namedWindow("Binary image", CV_WINDOW_AUTOSIZE);
    imshow("Binary image", bin);

    distanceTransform(bin, pixelized, CV_DIST_L2, 3, CV_32F);
    // imwrite("../bin.jpg", bin);
    namedWindow("Pixelized image", CV_WINDOW_AUTOSIZE);
    imshow("Pixelized image", pixelized);

    normalize(pixelized, normalized, 1, 0, NORM_MINMAX, -1, noArray());
    namedWindow("Normalized image", CV_WINDOW_AUTOSIZE);
    imshow("Normalized image", normalized);

    threshold(normalized, bin2, 0.5, 1, THRESH_BINARY);
    imwrite("../bin2.jpg", bin2);
    namedWindow("Binary n Normalized image", CV_WINDOW_AUTOSIZE);
    imshow("Binary n Normalized image", bin2);

    Mat dst;
    convertScaleAbs(bin2, dst);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(dst, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    cout << "AI detected " << contours.size() << " coins !" << endl;
    (contours.size() == 5) ? (cout << "Congrats \n") : (cout << "Try again \n");

    imwrite("../exo1.jpg", bin2);

    Mat marker = Mat::zeros(img.rows, img.cols, CV_32SC1);
    circle(marker, Point(5, 5), 3, CV_RGB(255, 255, 255), 10, 1, 0);
    findContours(dst, contours, hierarchy, RETR_CCOMP, CV_CHAIN_APPROX_NONE);

    int counter = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        drawContours(marker, contours, static_cast<int>(i),
            Scalar(static_cast<int>(i) + 1), -1);
        counter++;
    }
    // cout << counter << endl;

    watershed(img, marker);
    vector<Vec3b> color;
    for (int i = 0; i < counter; i++) {
        int value = 255;

        int aa = theRNG().uniform(0, value); // Vec3b(255, 255, 255);
        int bb = theRNG().uniform(0, value);
        int cc = theRNG().uniform(0, value);
        color.push_back(Vec3b((uchar)aa, (uchar)bb, (uchar)cc));
    }

    Mat wshed = Mat::zeros(marker.size(), CV_8UC3);

    // paint the watershed image
    for (int i = 0; i < marker.rows; i++)
        for (int j = 0; j < marker.cols; j++) {
            int index = marker.at<int>(i, j);
            //(index != -1) ? cout << index << " " : cout << "";
            if (index > 0 && index != 255)
                wshed.at<Vec3b>(i, j) = color[index - 1];
            else if (index == 255)
                wshed.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            else
                wshed.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
        }
    imshow("watershed transform", wshed);
    waitKey(0);
    return 0;
}
