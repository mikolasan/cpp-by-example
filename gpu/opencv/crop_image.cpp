// Include Libraries
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>

// Namespace nullifies the use of cv::function();
using namespace std;
using namespace cv;

CascadeClassifier face_cascade;

void detectAndDisplay( Mat frame );

int main()
{
  String face_cascade_name = samples::findFile("haarcascade_frontalcatface_extended.xml");
  if( !face_cascade.load( face_cascade_name ) ) {
    cout << "--(!)Error loading face cascade\n";
    return -1;
  }
  // Read image
  Mat img = imread("test.png");
  detectAndDisplay(img);

  // cout << "Width : " << img.size().width << endl;
  // cout << "Height: " << img.size().height << endl;
  // cout << "Channels: :" << img.channels() << endl;
  // // Crop image
  // Mat cropped_image = img(Range(80,280), Range(150,330));
 
  // //display image
  // imshow(" Original Image", img);
  // imshow("Cropped Image", cropped_image);
 
  // //Save the cropped Image
  // imwrite("Cropped Image.jpg", cropped_image);
 
  // 0 means loop infinitely
  waitKey(0);
  destroyAllWindows();
  return 0;
}

void detectAndDisplay( Mat frame )
{
  Mat frame_gray;
  cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );
  
  std::vector<Rect> faces;
  face_cascade.detectMultiScale( frame_gray, faces );
  cout << "faces found: " << faces.size() << endl;
  for ( size_t i = 0; i < faces.size(); i++ )
  {
    Point center( 
      faces[i].x + faces[i].width/2,
      faces[i].y + faces[i].height/2 );
    ellipse( frame, center, 
      Size( faces[i].width/2, faces[i].height/2 ), 
      0, 0, 360, Scalar( 255, 0, 255 ), 4 );

    Mat faceROI = frame_gray( faces[i] );
  }
  //-- Show what you got
  imshow( "Capture - Face detection", frame );
}