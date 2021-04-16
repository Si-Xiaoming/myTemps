#pragma once
#include<iostream>
#include<laszip/laszip_api.h>
#include<vector>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <cstdlib>
#include <ctime>
#include<Eigen/Dense>
using namespace Eigen;
using namespace std;

struct LasHeader {
	int num_points;
	Vector3d offset;
};

struct CloudStruct {
	pcl::PointCloud<pcl::PointXYZ> ptsxyz;
	//vector<int> classifcation;
	//inline size_t ptsNum()const { return ptsxyz.width * ptsxyz.height; }
};


//template <typename num_t>
//struct PointCloud
//{
//	struct Point
//	{
//		num_t  x, y, z;
//		int classifcation;
//	};
//
//	std::vector<Point>  pts;
//
//	// Must return the number of data points
//	inline size_t kdtree_get_point_count() const { return pts.size(); }
//
//	// Returns the dim'th component of the idx'th point in the class:
//	// Since this is inlined and the "dim" argument is typically an immediate value, the
//	//  "if/else's" are actually solved at compile time.
//	inline num_t kdtree_get_pt(const size_t idx, const size_t dim) const
//	{
//		if (dim == 0) return pts[idx].x;
//		else if (dim == 1) return pts[idx].y;
//		else return pts[idx].z;
//	}
//
//	// Optional bounding-box computation: return false to default to a standard bbox computation loop.
//	//   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
//	//   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
//	template <class BBOX>
//	bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }
//};


class LasOperate {
public:
	string path;
	LasOperate(string path_) :path(path_) {}
	~LasOperate() {}

	LasHeader pointReadHeader();

	CloudStruct pointRead();

	void MyPcSave(const std::string& outpath, const CloudStruct& outCloud);
};