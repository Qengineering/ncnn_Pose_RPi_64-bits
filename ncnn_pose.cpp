#include "net.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>

//using namespace cv;
using namespace std;
//-----------------------------------------------------------------------------------------------------------------------
struct KeyPoint
{
    cv::Point2f p;
    float prob;
};
//-----------------------------------------------------------------------------------------------------------------------
static void draw_pose(const cv::Mat& image, const std::vector<KeyPoint>& keypoints)
{
    // draw bone
    static const int joint_pairs[16][2] = {
        {0, 1}, {1, 3}, {0, 2}, {2, 4}, {5, 6}, {5, 7}, {7, 9}, {6, 8}, {8, 10}, {5, 11}, {6, 12}, {11, 12}, {11, 13}, {12, 14}, {13, 15}, {14, 16}
    };
    for (int i = 0; i < 16; i++)
    {
        const KeyPoint& p1 = keypoints[joint_pairs[i][0]];
        const KeyPoint& p2 = keypoints[joint_pairs[i][1]];
        if (p1.prob < 0.2f || p2.prob < 0.2f)
            continue;
        cv::line(image, p1.p, p2.p, cv::Scalar(255, 0, 0), 2);
    }
    // draw joint
    for (size_t i = 0; i < keypoints.size(); i++)
    {
        const KeyPoint& keypoint = keypoints[i];
        //fprintf(stderr, "%.2f %.2f = %.5f\n", keypoint.p.x, keypoint.p.y, keypoint.prob);
        if (keypoint.prob < 0.2f)
            continue;
        cv::circle(image, keypoint.p, 3, cv::Scalar(0, 255, 0), -1);
    }
}
//-----------------------------------------------------------------------------------------------------------------------
int runpose(cv::Mat& roi, ncnn::Net &posenet, int pose_size_width, int pose_size_height, std::vector<KeyPoint>& keypoints,float x1, float y1)
{
    int w = roi.cols;
    int h = roi.rows;
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(roi.data, ncnn::Mat::PIXEL_BGR2RGB,\
                                                 roi.cols, roi.rows, pose_size_width, pose_size_height);
    //数据预处理
    const float mean_vals[3] = {0.485f * 255.f, 0.456f * 255.f, 0.406f * 255.f};
    const float norm_vals[3] = {1 / 0.229f / 255.f, 1 / 0.224f / 255.f, 1 / 0.225f / 255.f};
    in.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = posenet.create_extractor();
    ex.set_num_threads(4);
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("hybridsequential0_conv7_fwd", out);
    keypoints.clear();
    for (int p = 0; p < out.c; p++)
    {
        const ncnn::Mat m = out.channel(p);

        float max_prob = 0.f;
        int max_x = 0;
        int max_y = 0;
        for (int y = 0; y < out.h; y++)
        {
            const float* ptr = m.row(y);
            for (int x = 0; x < out.w; x++)
            {
                float prob = ptr[x];
                if (prob > max_prob)
                {
                    max_prob = prob;
                    max_x = x;
                    max_y = y;
                }
            }
        }

        KeyPoint keypoint;
        keypoint.p = cv::Point2f(max_x * w / (float)out.w+x1, max_y * h / (float)out.h+y1);
        keypoint.prob = max_prob;
        keypoints.push_back(keypoint);
    }
    return 0;
}
//-----------------------------------------------------------------------------------------------------------------------
int demo(cv::Mat& image, ncnn::Net &detectornet, int detector_size_width, int detector_size_height,
         ncnn::Net &posenet, int pose_size_width, int pose_size_height)
{
    cv::Mat bgr = image.clone();
    int img_w = bgr.cols;
    int img_h = bgr.rows;

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR2RGB,\
                                                 bgr.cols, bgr.rows, detector_size_width, detector_size_height);

    //数据预处理
    const float mean_vals[3] = {0.f, 0.f, 0.f};
    const float norm_vals[3] = {1/255.f, 1/255.f, 1/255.f};
    in.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = detectornet.create_extractor();
    ex.set_num_threads(4);
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("output", out);

    for (int i = 0; i < out.h; i++)
    {
        float x1, y1, x2, y2;
        float pw,ph,cx,cy;
        const float* values = out.row(i);

        x1 = values[2] * img_w;
        y1 = values[3] * img_h;
        x2 = values[4] * img_w;
        y2 = values[5] * img_h;

        pw = x2-x1;
        ph = y2-y1;
        cx = x1+0.5*pw;
        cy = y1+0.5*ph;

        x1 = cx - 0.7*pw;
        y1 = cy - 0.6*ph;
        x2 = cx + 0.7*pw;
        y2 = cy + 0.6*ph;

        //处理坐标越界问题
        if(x1<0) x1=0;
        if(y1<0) y1=0;
        if(x2<0) x2=0;
        if(y2<0) y2=0;

        if(x1>img_w) x1=img_w;
        if(y1>img_h) y1=img_h;
        if(x2>img_w) x2=img_w;
        if(y2>img_h) y2=img_h;
        //截取人体ROI
        //printf("x1:%f y1:%f x2:%f y2:%f\n",x1,y1,x2,y2);
        cv::Mat roi;
        roi = bgr(cv::Rect(x1, y1, x2-x1, y2-y1)).clone();
        std::vector<KeyPoint> keypoints;
        runpose(roi, posenet, pose_size_width, pose_size_height,keypoints, x1, y1);
        draw_pose(image, keypoints);
        cv::rectangle (image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 255), 2, 8, 0);
    }
    return 0;
}
//-----------------------------------------------------------------------------------------------------------------------
int main(int argc,char ** argv)
{
    float f;
    float FPS[16];
    int i;
    int Fcnt=0;
    cv::Mat frame;
    chrono::steady_clock::time_point Tbegin, Tend;

    for(i=0;i<16;i++) FPS[i]=0.0;

    //定义检测器
    ncnn::Net detectornet;
    detectornet.load_param("./person_detector.param");
    detectornet.load_model("./person_detector.bin");
    int detector_size_width  =  320;
    int detector_size_height = 320;

    //定义人体姿态关键点预测器
    ncnn::Net posenet;
    posenet.load_param("./Ultralight-Nano-SimplePose.param");
    posenet.load_model("./Ultralight-Nano-SimplePose.bin");
    int pose_size_width  =  192;
    int pose_size_height =  256;

    cv::VideoCapture cap("Dance.mp4");
    if (!cap.isOpened()) {
        cerr << "ERROR: Unable to open the camera" << endl;
        return 0;
    }

    Tbegin = chrono::steady_clock::now();

    cout << "Start grabbing, press ESC on Live window to terminate" << endl;
    while(1){
        cap >> frame;
        if (frame.empty()) {
            cerr << "End of movie" << endl;
            break;
        }

        demo(frame, detectornet, detector_size_width, detector_size_height, posenet, pose_size_width,pose_size_height);

        Tend = chrono::steady_clock::now();
        //calculate frame rate
        f = chrono::duration_cast <chrono::milliseconds> (Tend - Tbegin).count();

        Tbegin = chrono::steady_clock::now();

        FPS[((Fcnt++)&0x0F)]=1000.0/f;
        for(f=0.0, i=0;i<16;i++){ f+=FPS[i]; }
        putText(frame, cv::format("FPS %0.2f",f/16),cv::Point(10,20),cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255));

        //show output
        cv::imshow("RPi 4 - 1.95 GHz - 2 Mb RAM", frame);

        char esc = cv::waitKey(5);
        if(esc == 27) break;
    }


    return 0;
}
