#include<iostream>
#include<Eigen/Dense>
#include <fstream>
#include <pcl-1.11/pcl/io/pcd_io.h>
#include <pcl-1.11/pcl/point_types.h>
#include "LasOperator.h"

#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/radius_outlier_removal.h>
using namespace std;

using namespace pcl;
using namespace io;

void te() {

    string outfilename = "D:\\SPL100_noise\\data\\test\\Out_Cloud_pcl.laz";

    string testFilename = "D:\\SPL100_noise\\data\\test\\test_Cloud.laz";
    LasOperate lo(testFilename);
    CloudStruct myCloudStruct = lo.pointRead();

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);


    *cloud = myCloudStruct.ptsxyz;
    pcl::RadiusOutlierRemoval<pcl::PointXYZ> sor;   //创建滤波器对象
    sor.setInputCloud(cloud);                           //设置待滤波的点云
    sor.setRadiusSearch(0.8);                               //设置在进行统计时考虑的临近点个数
    sor.setMinNeighborsInRadius(2);                      //设置判断是否为离群点的阀值，用来倍乘标准差，也就是上面的std_mul
    sor.filter(*cloud_filtered);                    //滤波结果存储到cloud_filtered

    CloudStruct myOutCloudStruct;
    myOutCloudStruct.ptsxyz = *cloud_filtered;
    cout <<"main中的:" <<myOutCloudStruct.ptsxyz.points.size()<<endl;

    lo.MyPcSave(outfilename, myOutCloudStruct);



}


int main(int argc, char** argv) {
   
    te();


    return 0;
}
