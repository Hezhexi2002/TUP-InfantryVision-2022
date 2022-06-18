#include <future>
#include <vector>

#include <Eigen/Core>

#include "armor_tracker.h"
#include "./detector/inference.h"
#include "./predictor/predictor.h"
#include "../coordsolver/coordsolver.h"
#include "../general/general.h"
#include "../serial/serialport.h"

enum SpinHeading {UNKNOWN, CLOCKWISE, COUNTER_CLOCKWISE};

class Autoaim
{
public:
    Autoaim();
    ~Autoaim();

    bool run(TaskData &src,VisionData &data);       // 自瞄主函数
private:
    const string network_path = "../model/opt-0527-001.xml";
    const string camera_param_path = "../params/coord_param.yaml";
    const string predict_param_path = "../params/filter/filter_param.yaml";

    bool is_last_target_exists;
    bool is_target_switched;
    int lost_cnt;
    int prev_timestamp;
    double last_target_area;
    double last_bullet_speed;
    Point2i last_roi_center;
    Eigen::Vector3d last_aiming_point;
    Point2i roi_offset;
    Size2i input_size;
    std::vector<ArmorTracker> trackers;
    std::vector<Armor>last_armors;
    std::multimap<string, ArmorTracker> trackers_map;      //预测器Map
    std::map<string,int> new_armors_cnt_map;    //装甲板计数map，记录新增装甲板数
    std::map<string,SpinHeading> spin_status_map;    //反小陀螺，记录该车小陀螺状态
    std::map<string,double> spin_score_map;     //反小陀螺，记录各装甲板小陀螺可能性分数，大于0为逆时针旋转，小于0为顺时针旋转
    
    float high_threshold_conf;          //高置信度阈值
    float low_threshold_conf;           //低置信度阈值
    vector<Armor> last_middle_armors;   //上次介于上下置信度阈值之间的装甲板
    vector<Armor> curr_middle_armors;   //本次介于上下置信度阈值之间的装甲板
    float jump_threshold_conf;          //跳变置信度阈值 
    float dis_threshhold;               //两帧之间同一块装甲板的距离差阈值

    const int armor_type_wh_thres = 3;      //大小装甲板长宽比阈值

    const double armor_roi_expand_ratio_width = 1.1;
    const double armor_roi_expand_ratio_height = 1.5;

    const int max_lost_cnt = 5;                 //最大丢失目标帧数
    const int max_armors = 8;                   //视野中最多装甲板数
    const double max_delta_dist = 0.2;                  //两次预测间最大速度(m/s)
    const armor_conf_high_thres = 0.8;            //置信度大于该值的装甲板直接采用
    // const int max_delta_t = 75;                //使用同一预测器的最大时间间隔(ms)
    const int max_delta_t = 50;                //使用同一预测器的最大时间间隔(ms)

    int anti_spin_judge_high_thres = 2e4;//大于该阈值认为该车已开启陀螺
    int anti_spin_judge_low_thres = 1e3;//小于该阈值认为该车已关闭陀螺
    int anti_spin_max_r_multiple = 4;

    const double no_crop_ratio = 5e-3;      //禁用ROI裁剪的装甲板占图像面积最大面积比值
    const double full_crop_ratio = 1e-4;     //最大ROI比例，大于此比例ROI大小为网络输入比例                   

    const int hero_danger_zone = 4;       //英雄危险距离阈值，检测到有小于该距离的英雄直接开始攻击

    Armor last_armor;
    CoordSolver coordsolver;
    ArmorDetector detector;
    ArmorPredictor predictor_param_loader;
    ArmorPredictor predictor;

    bool updateSpinScore();
    string chooseTargetID(vector<Armor> &armors, int timestamp);
    ArmorTracker* chooseTargetTracker(vector<ArmorTracker*> trackers, int timestamp);
    Point2i cropImageByROI(Mat &img);
};