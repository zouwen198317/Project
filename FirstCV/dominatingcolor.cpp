#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <dirent.h>

using namespace cv;
using namespace std;

void colorReduce(Mat &image, int div=64){
    int nl = image.rows;
    int nc = image.cols * image.channels();
    
    for(int j=0; j<nl; j++){
        uchar* data = image.ptr<uchar>(j);
        for(int i=0; i<nc; i++){
            data[i] = data[i]/div*div + div/2;
        }
    }
}
int maindc( int /*argc*/, char** /*argv*/ )
{
    DIR *dir;
    struct dirent *ent;
    std::string ext = "png";
    std::string s = "";
    string pwd = "";
    std::vector<cv::Point> polygon;
    //    polygon.push_back(Point(400,236));
    //    polygon.push_back(Point(640,390));
    //    polygon.push_back(Point(640,240));
    //    polygon.push_back(Point(365,145));
    //polygon.push_back(Point(411,348));
    // polygon.push_back(Point(552,480));
    //polygon.push_back(Point(640,352));
    //polygon.push_back(Point(400,288));
    polygon.push_back(Point(861,463));
    polygon.push_back(Point(1092,706));
    polygon.push_back(Point(1280,394));
    polygon.push_back(Point(790,267));
    //fills the roi of the visible road
    //  fillConvexPoly(polyMat, polygon, Scalar(255), 8, 0);
    //rows = 480 = height, cols = 640 = width;
    
    Rect minRect = boundingRect(polygon);
    
    
    Mat image = Mat::zeros( 480, 640, CV_8UC3 );
    Mat tpl = imread("/Users/sanjeevsingh/Desktop/edge.png",0);
    Canny(tpl, tpl, 5, 50, 3);
    namedWindow("result", CV_WINDOW_AUTOSIZE );
    String filename;
    char* path = "/Volumes/MacintoshHD2/SideEye/images/meng5/Vehicle/5/";
    if ((dir = opendir (path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            std::stringstream sstm;
            std::string fname = ent->d_name;  // filename
            // if filename's last characters are extension
            
            if (fname.find(ext, (fname.length() - ext.length())) != std::string::npos){
                sstm << path << fname;
                filename = sstm.str();
                cout<<filename<<endl;
                image = imread(filename,CV_LOAD_IMAGE_COLOR);
                colorReduce(image);
                
                
                //  Mat source = imread("/Users/sanjeevsingh/Desktop/002.png", 1 );
                Mat roi = image(Rect(minRect.x, minRect.y, minRect.width-1, minRect.height-1));
                Mat samples(roi.rows * roi.cols, 3, CV_32F);
                for( int y = 0; y < roi.rows; y++ )
                    for( int x = 0; x < roi.cols; x++ )
                        for( int z = 0; z < 3; z++)
                            samples.at<float>(y + x*roi.rows, z) = roi.at<Vec3b>(y,x)[z];
                
                
                int clusterCount = 3;
                Mat labels;
                int attempts = 5;
                Mat centers;
                kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );
                
                
                Mat new_image( roi.size(), roi.type() );
                for( int y = 0; y < roi.rows; y++ )
                    for( int x = 0; x < roi.cols; x++ )
                    {
                        int cluster_idx = labels.at<int>(y + x*roi.rows,0);
                        new_image.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
                        new_image.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
                        new_image.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
                    }
                imshow( "clustered image", new_image );
                waitKey( 0 );
                //waitKey(0);
                //if(waitKey(30) >= 0) break;
            }
        }
        
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
    }
    
}