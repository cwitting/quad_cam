#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <string>
using namespace cv;
using namespace std;
//#define IMAGE_PATH "/ardrone/image_raw" //Quadcopter
#define IMAGE_PATH "/image_raw" //Webcam

static const std::string OPENCV_WINDOW = "Image window";


class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  
public:

  void drawText(string text, Mat* img)
  {
  int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
  double fontScale = 2;
  int thickness = 3;

  //Mat img(600, 800, CV_8UC3, Scalar::all(0));

  int baseline=0;
  Size textSize = getTextSize(text, fontFace,
                              fontScale, thickness, &baseline);
  baseline += thickness;

  // center the text
  Point textOrg((img->cols - textSize.width)/2,
                (img->rows + textSize.height)/2);

  // draw the box
  rectangle(*img, textOrg + Point(0, baseline),
            textOrg + Point(textSize.width, -textSize.height),
            Scalar(0,0,255));
  // ... and the baseline first
  line(*img, textOrg + Point(0, thickness),
       textOrg + Point(textSize.width, thickness),
       Scalar(0, 0, 255));

  // then put the text itself
  putText(*img, text, textOrg, fontFace, fontScale,
          Scalar::all(255), thickness, 8);
  }


  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe(IMAGE_PATH, 1, 
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    //cv::namedWindow(OPENCV_WINDOW);
  }

  ~ImageConverter()
  {
    //cv::destroyWindow(OPENCV_WINDOW);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    // Draw an example circle on the video stream
    //if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
    //  cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));
    Mat src, src_gray;

    //TEST AF BLOB DETECTION
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.4;
    int thickness = 1;
    int baseline = 0;
    src = cv_ptr->image;
    cv::flip(src,src,1);

   // Set up the detector with default parameters.
    SimpleBlobDetector detector;
         
    // Detect blobs.
    std::vector<KeyPoint> keypoints;
    detector.detect( src, keypoints);
         
    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    Mat im_with_keypoints;
    drawKeypoints( src, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
         
    // Show blobs
     

    if (keypoints.size() > 0){
      ROS_INFO("Blob at (%g, %g)", keypoints[0].pt.x-320, keypoints[0].pt.y-240);
      std::stringstream ss;
      ss << "(" <<  (keypoints[0].pt.x-320) << ", " << (keypoints[0].pt.y-240) << ")";
      string tekst = ss.str();
      cv::Size siz = cv::getTextSize(tekst, fontface, scale, thickness, &baseline);
      cv::Point2f pt(keypoints[0].pt.x - (siz.width / 2), keypoints[0].pt.y - siz.height*2);
      cv::putText(im_with_keypoints, tekst, pt, fontface, scale, CV_RGB(255,0,0), thickness, 8);
    }

    imshow("keypoints", im_with_keypoints ); 

    //SLUT TEST




    // Update GUI Window
    //cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);
    
    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}