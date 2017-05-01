#include <time.h>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <string.h>

#include "Thirdparty/DBoW2/DBoW2/FORB.h"
#include "Thirdparty/DBoW2/DBoW2/TemplatedVocabulary.h"

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/features2d/features2d.hpp>
//#include "opencv2/xfeatures2d.hpp"

#include "ORBVocabulary.h"
#include "ORBextractor.h"

using namespace std;
using namespace DBoW2;
using namespace ORB_SLAM2;
using namespace DUtils;

void extractORBFeatures(cv::Mat &image, vector<vector<cv::Mat> > &features, ORBextractor* extractor);
void changeStructureORB(const cv::Mat &descriptor, vector<bool> &mask, vector<cv::Mat> &out);
void isInImage(vector<cv::KeyPoint> &keys, float &cx, float &cy, float &rMin, float &rMax, vector<bool> &mask);
void createVocabularyFile(ORBVocabulary &voc, std::string &fileName, const vector<vector<cv::Mat> > &features);

std::string folder = "/mnt/hgfs/database/turtlebot_datas/1130/rgb/";
std::string vociName = "self_trained_voc.txt";

int main(int argc, char **argv) {
    cout << "***** Train Vocabulary *****" << endl;
    //vector<std::string> filenames;
    vector<cv::String> filenames;
    cv::glob(folder, filenames);
    //cv::glob(folder, filenames, false);

    // init ORBextractor
    int nLevels = 6;
    ORBextractor* extractor = new ORBextractor(1000, 1.2, nLevels, 1, 20);
    int nImages = filenames.size();
    
    vector<vector<cv::Mat> > features;
    features.clear();
    features.reserve(nImages);

    // cv_bridge::CvImageConstPtr cv_ptr;
    cv::Mat image;
    // cout << "> Using bag file: " << bagFile << endl;
    cout << "> Extracting Features from " << nImages << " images..." << endl;
    for (int i = 0; i < nImages; i+=5)
    {
        // sensor_msgs::Image::ConstPtr i = m.instantiate<sensor_msgs::Image>();
        std::cout << "Processing the " << i <<" image " << std::endl;
        cv::Mat src = cv::imread(filenames[i]);
        imshow("View", src);
        cv::waitKey(1);
        if (!src.empty()) {
            // cv_ptr = cv_bridge::toCvShare(i);
            cv::cvtColor(src, image, CV_RGB2GRAY);
            extractORBFeatures(image, features, extractor);
        }
    }
    // bag.close();
    cout << "... Extraction done!" << endl;
    
    // Creating the Vocabulary
    // define vocabulary
    const int k = 10; // branching factor
    const WeightingType weight = TF_IDF;
    const ScoringType score = L1_NORM;
    ORBVocabulary voc(k, nLevels, weight, score);
    createVocabularyFile(voc, vociName, features);
    cout << "***** THE END *****" << endl;

    return 0;
}


/*
 * define functions
*/
void extractORBFeatures(cv::Mat &image, vector<vector<cv::Mat> > &features, ORBextractor* extractor)
{
    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptorORB;
    // extract
    (*extractor)(image, cv::Mat(), keypoints, descriptorORB);

    // reject features outside region of interest
    vector<bool> mask;
    float cx = 0; float cy = 0;
    float rMin = 0; float rMax = 0;
    isInImage(keypoints, cx, cy, rMin, rMax, mask);
    // create descriptor vector for the vocabulary
    features.push_back(vector<cv::Mat>());
    changeStructureORB(descriptorORB, mask, features.back());
    //imshow("ORBFeature", features.back().back());
}

void changeStructureORB(const cv::Mat &descriptor, vector<bool> &mask, vector<cv::Mat> &out) 
{
    for (int i = 0; i < descriptor.rows; i++) {
        if (mask[i]) {
            out.push_back(descriptor.row(i));
        }
    }
}

void isInImage(vector<cv::KeyPoint> &keys, float &cx, float &cy, float &rMin, float &rMax, vector<bool> &mask) 
{
    int N = keys.size();
    mask = vector<bool>(N, false);
    int num = 0;
    for (int i = 0; i < N; i++) {
        cv::KeyPoint kp = keys[i];
        float u = kp.pt.x;
        float v = kp.pt.y;
        // 320*240
        //if (u > 20 && u < 320-20 && v > 20 && v < 240-20) {
        // 640*480
        if (u > 20 && u < 640-20 && v > 20 && v < 480-20) {
            mask[i] = true;
            num ++;
        }
    }
    std::cout << "In image number " << num << std::endl;
}

void createVocabularyFile(ORBVocabulary &voc, std::string &fileName, const vector<vector<cv::Mat> > &features)
{
    cout << "> Creating vocabulary. May take some time ..." << endl;
    voc.create(features);
    cout << "... done!" << endl;
    
    cout << "> Vocabulary information: " << endl << voc << endl << endl;
    // save the vocabulary to disk
    cout << endl << "> Saving vocabulary..." << endl;
    voc.saveToTextFile(fileName);
    cout << "... saved to file: " << fileName << endl;
}
