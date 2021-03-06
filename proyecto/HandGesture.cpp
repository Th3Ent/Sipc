#include "HandGesture.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

HandGesture::HandGesture() {

}


double HandGesture::getAngle(Point s, Point e, Point f) {

	double v1[2],v2[2];
	v1[0] = s.x - f.x;
	v1[1] = s.y - f.y;

	v2[0] = e.x - f.x;
	v2[1] = e.y - f.y;

	double ang1 = atan2(v1[1],v1[0]);
	double ang2 = atan2(v2[1],v2[0]);

	double angle = ang1 - ang2;
	if (angle > CV_PI) angle -= 2 * CV_PI;
	if (angle < -CV_PI) angle += 2 * CV_PI;
	return (angle * 180.0/CV_PI);
}
void HandGesture::FeaturesDetection(Mat mask, Mat output_img) {
	int contdedos=0;
	vector<vector<Point> > contours;
	vector<Point> contoursmax;
	Mat temp_mask ;
	mask.copyTo(temp_mask);//copia la mascara
	int index = -1;

        // CODIGO 3.1
        // detección del contorno de la mano y selección del contorno más largo
        findContours(temp_mask,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
				for(int aux = 0; aux <contours.size(); aux++){
					if(contours[aux].size() > contoursmax.size()){
						contoursmax = contours[aux];
						index = aux;
					}
				}
        // pintar el contorno
        //...
				if(contours.size()==0)
					return;


				drawContours(output_img,contours,index,Scalar(255,0,0),2,8,vector<Vec4i>(),0,Point());

	//obtener el convex hull
	vector<int> hull;
	convexHull(contours[index],hull);

	// pintar el convex hull
	Point pt0 = contours[index][hull[hull.size()-1]];
	for (int i = 0; i < hull.size(); i++)
	{
		Point pt = contours[index][hull[i]];
		line(output_img, pt0, pt, Scalar(0, 0, 255), 2, CV_AA);
		pt0 = pt;
	}

        //obtener los defectos de convexidad
	vector<Vec4i> defects;
	convexityDefects(contours[index], hull, defects);

        double area = contourArea(contoursmax,false);
	std::ostringstream strs;
	strs << area;
	std::string str = strs.str();
	int cont = 0;
		for (int i = 0; i < defects.size(); i++) {
			Point s = contours[index][defects[i][0]];
			Point e = contours[index][defects[i][1]];
			Point f = contours[index][defects[i][2]];
			float depth = (float)defects[i][3] / 256.0;//se divide la distancia entre 256 para que este entre 0 255
			double angle = getAngle(s, e, f);
                        // CODIGO 3.2
                        // filtrar y mostrar los defectos de convexidad
			if(depth > 50 && angle < 90){
				circle(output_img,f,5,Scalar(0,255,0),3);
				contdedos++;
			}
    }
		string result;
		switch(contdedos){
			case 1: 
				result = "DOS dedos levantados";
				break;
			case 2: 
				result = "TRES dedos levantados";
				break;
			case 3: 
				result = "CUATRO dedos levantados";
				break;
			case 4: 
				result = "CINCO dedos levantados";
				break;
			case 0: 
				
					if( area > 34000  && area < 37000)
						result = "CERO dedos levantados";
					else if(area > 31000 && area < 34000)
						result = "UN dedo levantado";
					else
					result = "CERO o UN dedo levantados";
				break;
			default:
				result = "ERROR";
				break;
		}

		cv::putText(output_img, result, Point(10, 70), FONT_HERSHEY_SIMPLEX, 1.0,CV_RGB(0, 255, 0), 2);

		rectangle(output_img, boundingRect(contoursmax).tl(), boundingRect(contoursmax).br(), CV_RGB(12, 183, 142), 2, 8, 0);

}
