#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int, char**){

    //FIRST CAMERA FLUX
    VideoCapture cap1(0); // open camera
    //VideoCapture cap1("/opt/lampp/htdocs/_video.mp4");
    // iPod screen size : 1136x640 = 568*2x320*2;
    //cap1.set(3, 320);
    //cap1.set(4, 320);

    int frame_width1 = cap1.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height1 = cap1.get(CV_CAP_PROP_FRAME_HEIGHT);

    printf("taille du fichier originel (WxH) : %d, %d\n", frame_width1, frame_height1);

    if(!cap1.isOpened()){  // check succeeded
        printf("Error Opennig the file\n");
        return -1;
    }

    double fps = cap1.get(CV_CAP_PROP_FPS);

    //printf("frame1.size : %d, %d\n", frame_width1, frame_height1);

    //CREATING ONE FRAME TO CONTAIN 2 VIDEOS
    Mat totalFrame = Mat::zeros(frame_height1, frame_width1*2, cap1.get(CAP_PROP_FORMAT));
    //printf("totalFrame.size : %d, %d\n", totalFrame.rows, totalFrame.cols);

    //CREATING VIDEO WRITER
    //VideoWriter video("/opt/lampp/htdocs/test.avi", CV_FOURCC('M','J','P','G'), fps, CvSize(totalFrame.cols, totalFrame.rows), true);
    //const string filename = "video";
    VideoWriter video("/opt/lampp/htdocs/video", CV_FOURCC('H','2','6','4'), fps, CvSize(totalFrame.cols, totalFrame.rows));


    //MAIN LOOP
    while(true){

        //FIRST VIDEO FRAME EXTRACTION
        Mat frame1;
        cap1 >> frame1; // get a new frame from camera
            
        //SECOND VIDEO FRAME EXTRACTION
        //Mat frame2;
        //cap2 >> frame2;

        //FRAME EMPTY = BREAK
        if(frame1.empty()){
            printf("frame1 ou frame2 vide\n");
            break;
        }

        hconcat(frame1, frame1, totalFrame);
        video.write(totalFrame);

        //FRAME ROTATION
        //transpose(edges, edges);
        //flip(edges, edges, +1);

        //MAIN FRAME DISPLAY
        imshow("MAIN FRAME", totalFrame);

        //REQUIRED IF IMSHOW() => HUI NEEDS A DELAY TIME TO DISPLAY THE FRAME 
        //waitKey(1000);
    }
    return 0;
}



/* BACKUP DU CODE AU CAS OU xD

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int, char**)
{
    VideoCapture cap(1); // open camera
    if(!cap.isOpened()){  // check succeeded
        printf("Error Opennig the file\n");
        return -1;
    }
    Mat edges;
    namedWindow("edges", 1);

    while(true){
        Mat frame;
        cap >> frame; // get a new frame from camera
        if(frame.empty()){
            printf("Image vide\n");
            break;
        }
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        waitKey(30);
    }
    cap.release();
    return 0;
}
*/