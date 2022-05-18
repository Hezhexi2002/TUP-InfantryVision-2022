#pragma once

#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>
#include <yaml-cpp/yaml.h>

#include "../general/general.h"

using namespace std;
using namespace cv;

struct PnPInfo
{
    Eigen::Vector3d armor_cam;
    Eigen::Vector3d armor_world;
    Eigen::Vector3d R_cam;
    Eigen::Vector3d R_world;
    Eigen::Vector3d euler;
};

class CoordSolver
{
public:
    CoordSolver();
    ~CoordSolver();
    
    bool loadParam(string coord_path,string param_name);

    double dynamicCalcPitchOffset(Eigen::Vector3d &xyz);
    
    PnPInfo pnp(const std::vector<Point2f> &points_pic, const Eigen::Matrix3d &rmat_imu, int method);
    
    Eigen::Vector3d camToWorld(const Eigen::Vector3d &point_camera,const Eigen::Matrix3d &rmat);
    Eigen::Vector3d worldToCam(const Eigen::Vector3d &point_world,const Eigen::Matrix3d &rmat);

    Eigen::Vector3d staticCoordOffset(Eigen::Vector3d &xyz);
    Eigen::Vector2d staticAngleOffset(Eigen::Vector2d &angle);
    Eigen::Vector2d getAngle(Eigen::Vector3d &xyz_cam, Eigen::Matrix3d &rmat);

    inline double calcYaw(Eigen::Vector3d &xyz);
    inline double calcPitch(Eigen::Vector3d &xyz);
    Eigen::Vector2d calcYawPitch(Eigen::Vector3d &xyz);

    cv::Point2f reproject(Eigen::Vector3d &xyz);
private:
    int max_iter;
    float stop_error;
    int R_K_iter;
    Mat intrinsic;
    Mat dis_coeff;
    Eigen::Vector3d xyz_offset;
    Eigen::Vector3d t_iw;
    Eigen::Vector2d angle_offset;
    Eigen::Matrix4d transform_ic;
    Eigen::Matrix4d transform_ci;

    YAML::Node param_node;

    const int armor_type_wh_thres = 7;      //大小装甲板长宽比阈值
    const int bullet_speed = 28;            //TODO:弹速可变
    // const int bullet_speed = 16;            //TODO:弹速可变
    const double k = 0.0389;                //25°C,1atm,小弹丸
    // const double k = 0.0111;                //25°C,1atm,大弹丸
    const double g = 9.801;
};