#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include <fstream>
#include <math.h>
#include <vector>
//vector<char> test;
#define SIZE 100
#define foldor 50
cv::Mat input;
cv::Mat dataImg;
cv::Mat comparation;
cv::Mat kernal(2, 2, CV_8U, cv::Scalar(1));

void medianFilter(cv::Mat& input);
void imageOutput(char outputName[], cv::Mat& output);
void HSV(cv::Mat& input);
void preprocess(cv::Mat& input);
void detect(cv::Mat& undetect);
char line[SIZE];
using namespace std;
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		auto color = comparation.at<cv::Vec3b>(y, x);
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		cout << static_cast<int>(color[0]) << endl;
		cout << static_cast<int>(color[1]) << endl;
		cout << static_cast<int>(color[2]) << endl;
	}
	else if (event == cv::EVENT_RBUTTONDOWN)
	{
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == cv::EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	/*else if (event == cv::EVENT_MOUSEMOVE)
	{
		cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}*/
}
int main()
{
	char PathTwo[200];
	std::fstream fin;//宣告fin檔案
	std::fstream fout;
	fin.open("Test.txt", std::ios::in);//開啟檔案fin(檔名 input.txt)
	fout.open("result1.txt", std::ios::out);//開啟檔案fout(檔名 answer.txt)
	fout << "40" << endl;//寫入字串
	/*if (!fin){//如果fin開啟檔案失敗
		std::cout << "Fail to open file: " << "input.txt" << std::endl;
	}
	else{
		do {
			fin.getline(line, sizeof(line));//一行一行輸出(line=每一行的資料)
			int a = strlen(line); //計算字串長度
			if (a < 3){ //第一行為兩個數字 字串長度為2
				continue;
			}
			cout << line << "\n";//把剩下的資料都丟到fout(測試)(之後要寫到輸出偵測結果)
			comparation = cv::imread(line, cv::IMREAD_GRAYSCALE);
			preprocess(comparation);
			detect(comparation);
		} while (!fin.eof());
		fin.close();
	}*/
	for (int i = 0; i < SIZE; i++){
		sprintf_s(PathTwo, sizeof(PathTwo), "CharModels/1/1_Resized_%04d.bmp", i);
		//sprintf_s(PathTwo, sizeof(PathTwo), "CharModelsAns/%03d.bmp", i);
		//comparation = cv::imread(PathTwo, cv::IMREAD_COLOR);
		comparation = cv::imread(PathTwo, cv::IMREAD_GRAYSCALE);
		cout << PathTwo << endl;
		if (comparation.empty()) { break; }
		//HSV(comparation);
		
		preprocess(comparation);
		//detect(comparation);
		imageOutput(PathTwo, comparation);
	}
	system("pause");
	cv::waitKey(0);
	return 0;
}

void HSV(cv::Mat& input){
	cv::cvtColor(input, input, CV_BGR2HSV);
	inRange(input, cv::Scalar(0, 0, 75), cv::Scalar(0, 0, 255), input);
	
}

void preprocess(cv::Mat& input){
	//input = input.rowRange(0,256);
	//cv::threshold(input, input, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::adaptiveThreshold(input, input, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 255, 25);
	cv::dilate(input, input, kernal, cv::Point(-1, -1), 2);
	cv::erode(input, input, kernal, cv::Point(-1, -1), 4);
}

void detect(cv::Mat& undetect){
	char inputName[50] = { 0 };
	char totalPath[50] = { 0 };
	char inputFormat[] = ".bmp";
	char number[3] = { 0 };
	char totalStr[3] = { 0 };
	int totalNum = 2;
	int totalSum[foldor] = { 0 };
	int totalPepperSum[foldor] = { 0 };
	int sum = 0;
	int modelPoint = 0;
	int dPoint = 0;
	int temp = 0;
	int reproduceArr[foldor] = { 0 };
	fstream result;
	result.open("result.txt", ios::out | ios::app);
	if (!result){//如果開啟檔案失敗，fp為0；成功，fp為非0
		cout << "Fail to open file: " << "result.txt" << endl;
	}
	char *pattern[]{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
		"k", "l", "m", "n", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
	char *fileTagName[]{"0", "1", "1_1", "1_2", "2", "3", "3_1", "4", "5", "6", "6_1", "7", "7_1", "8", "9", "9_1", "a", "b", "c", "c_1", "d", "d_1", "e", "f", "g", "h", "h_1", "i", "j", "j_1",
		"k", "l", "m", "n", "p", "q", "r", "r_1", "s", "t", "t_1", "u", "v", "v_1", "w", "w_1", "x", "y", "y_1", "z"};
	for (int fileTag = 0; fileTag < foldor; fileTag++){
		strcpy(inputName, "CharModels/");
		strcat(inputName, fileTagName[fileTag]);
		strcat(inputName, "/");

		strcpy(totalPath, inputName);
		strcat(totalPath, "total.txt");
		ifstream drill(totalPath);
		while (drill.getline(line, sizeof(line), '\n')){
			strcpy(totalStr, line);
			totalNum = atoi(totalStr);
		}
		totalNum++;
		for (int i = 0; i < totalNum; i++){
			sum = 0;
			strcpy(inputName, "CharModels/");
			strcat(inputName, fileTagName[fileTag]);
			strcat(inputName, "/");

			strcat(inputName, fileTagName[fileTag]);
			strcat(inputName, "_Resized_00");
			if (i < 10) {
				strcat(inputName, "0");
				_itoa(i, number, 10);
				strcat(inputName, number);
			}
			else{
				_itoa(i, number, 10);
				strcat(inputName, number);
			}
			strcat(inputName, inputFormat);

			input = cv::imread(inputName, cv::IMREAD_GRAYSCALE);
			dataImg = input;
			preprocess(dataImg);
			for (int row = 0; row < dataImg.rows; ++row){
				for (int col = 0; col < dataImg.cols; ++col){
					modelPoint = dataImg.at<unsigned char>(row, col);
					dPoint = abs(undetect.at<unsigned char>(row, col) - modelPoint);
					sum += dPoint;
				}
			}
			//imageOutput(inputName, dataImg);
			totalSum[fileTag] += sum;
		}
		totalSum[fileTag] /= totalNum;
	}
	for (int i = 0; i < foldor; i++){
		reproduceArr[i] = totalSum[i];
	}
	for (int i = 0; i < foldor; i++) {
		for (int j = i; j < foldor; j++) {
			if (totalSum[j] < totalSum[i]) {
				temp = totalSum[j];
				totalSum[j] = totalSum[i];
				totalSum[i] = temp;
			}
		}
	}
	for (int i = 0; i < foldor; i++){
		if (totalSum[0] == reproduceArr[i]){	
			char test = fileTagName[i][0];
			cout << "辨識結果為:" << fileTagName[i][0] << endl;
			if (i==0){
				result << "0" << endl;//寫入字串
			}
			else if (i == 1){
				result << "1" << endl;//寫入字串
			}
			else if (i == 2){
				result << "1" << endl;//寫入字串
			}
			else if (i == 3){
				result << "1" << endl;//寫入字串
			}
			else if (i == 4){
				result << "2" << endl;//寫入字串
			}
			else if (i == 5){
				result << "3" << endl;//寫入字串
			}
			else if (i == 6){
				result << "3" << endl;//寫入字串
			}
			else if (i == 7){
				result << "4" << endl;//寫入字串
			}
			else if (i == 8){
				result << "5" << endl;//寫入字串
			}
			else if (i == 9){
				result << "6" << endl;//寫入字串
			}
			else if (i == 10){
				result << "6" << endl;//寫入字串
			}
			else if (i == 11){
				result << "7" << endl;//寫入字串
			}
			else if (i == 12){
				result << "7" << endl;//寫入字串
			}
			else if (i == 13){
				result << "8" << endl;//寫入字串
			}
			else if (i == 14){
				result << "9" << endl;//寫入字串
			}
			else if (i == 15){
				result << "9" << endl;//寫入字串
			}
			else if (i == 16){
				result << "10" << endl;//寫入字串
			}
			else if (i == 17){
				result << "11" << endl;//寫入字串
			}
			else if (i == 18){
				result << "12" << endl;//寫入字串
			}
			else if (i == 19){
				result << "12" << endl;//寫入字串
			}
			else if (i == 20){
				result << "13" << endl;//寫入字串
			}
			else if (i == 21){
				result << "13" << endl;//寫入字串
			}
			else if (i == 22){
				result << "14" << endl;//寫入字串
			}
			else if (i == 23){
				result << "15" << endl;//寫入字串
			}
			else if (i == 24){
				result << "16" << endl;//寫入字串
			}
			else if (i == 25){
				result << "17" << endl;//寫入字串
			}
			else if (i == 26){
				result << "17" << endl;//寫入字串
			}
			else if (i == 27){
				result << "18" << endl;//寫入字串
			}
			else if (i == 28){
				result << "19" << endl;//寫入字串
			}
			else if (i == 29){
				result << "19" << endl;//寫入字串
			}
			else if (i == 30){
				result << "20" << endl;//寫入字串
			}
			else if (i == 31){
				result << "21" << endl;//寫入字串
			}
			else if (i == 32){
				result << "22" << endl;//寫入字串
			}
			else if (i == 33){
				result << "23" << endl;//寫入字串
			}
			else if (i == 34){
				result << "25" << endl;//寫入字串
			}
			else if (i == 35){
				result << "26" << endl;//寫入字串
			}
			else if (i == 36){
				result << "27" << endl;//寫入字串
			}
			else if (i == 37){
				result << "27" << endl;//寫入字串
			}
			else if (i == 38){
				result << "28" << endl;//寫入字串
			}
			else if (i == 39){
				result << "29" << endl;//寫入字串
			}
			else if (i == 40){
				result << "29" << endl;//寫入字串
			}
			else if (i == 41){
				result << "30" << endl;//寫入字串
			}
			else if (i == 42){
				result << "31" << endl;//寫入字串
			}
			else if (i == 43){
				result << "31" << endl;//寫入字串
			}
			else if (i == 44){
				result << "32" << endl;//寫入字串
			}
			else if (i == 45){
				result << "32" << endl;//寫入字串
			}
			else if (i == 46){
				result << "33" << endl;//寫入字串
			}
			else if (i == 47){
				result << "34" << endl;//寫入字串
			}
			else if (i == 48){
				result << "34" << endl;//寫入字串
			}
			else if (i == 49){
				result << "35" << endl;//寫入字串
			}			
		}
	}
}

void imageOutput(char outputName[], cv::Mat& output){
	cv::namedWindow(outputName, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(outputName, CallBackFunc, NULL);
	cv::imshow(outputName, output);
}
