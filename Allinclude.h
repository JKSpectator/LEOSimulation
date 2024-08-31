#pragma once
//所有项目都需要包含的头文件，用于存储公用库
// 定义 _USE_MATH_DEFINES 宏，使得 M_PI 等数学常量可用
#define _USE_MATH_DEFINES

// 定义 ATTACK_K 宏，可能用于控制某些功能或条件编译
#define ATTACK_K

// 包含标准输入输出流库
#include <iostream>

// 包含文件流库，用于读写文件
#include <fstream>

// 包含字符串库，用于字符串操作
#include <string>

// 包含字符串流库，用于字符串与其它类型之间的转换
#include <sstream>

// 包含随机数库，用于生成随机数
#include <random>

// 包含数学库，提供各种数学函数
#include <cmath>

// 包含向量库，用于处理动态数组
#include <vector>

// 包含自定义头文件，定义轨道元素相关操作
#include "KeplerOrbits/OrbitalElements.h"

// 包含自定义头文件，定义坐标系统转换相关操作
#include "KeplerOrbits/CoordinateSystems.h"

// 包含自定义头文件，定义轨道体相关操作
#include "KeplerOrbits/OrbitBody.h"

// 包含函数库，用于定义和使用函数对象
#include <functional>

// 包含 Windows 头文件，提供 Windows 系统相关操作
#include<Windows.h>

// 包含时间头文件，提供时间获取和转换功能
#include<ctime>

// 包含时间头文件，提供更多时间处理功能
#include<time.h>

// 包含 nlohmann/json 库，用于 JSON 数据的解析和序列化
#include "nlohmann/json.hpp"

// 使用 nlohmann::json 命名空间，简化代码
using json = nlohmann::json;

// 使用标准命名空间，简化代码
using namespace std;