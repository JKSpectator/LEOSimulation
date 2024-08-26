## LEOSimulation
### 功能
1. 模拟LEO卫星运动
2. 模拟两个地面站间借助LEO卫星的通信
3. 模拟地面攻击站点导致的延时效果
### 使用方法
1. 下载代码使用VS打开
2. 配置ConfigTxt文件夹下的配置文件（详细配置请于[配置详情](#config)查看）
>1. city_data.txt——地面站点的经纬度信息
>2. communication_config.txt——通信模拟参数
>3. config.txt——LEO卫星信息
3. 选择x64/x86方式运行（x64直接进行数据模拟，x86使用Glut可视化辅助模拟）
>注意，选择x86可视化时，需要确保如下几个步骤：
>1. glut32.dll放置于LEOSimulation.cpp即main函数文件同级目录
>2. 配置项目属性：C/C++——附加包含目录为OpenGL的文件夹
>3. 配置项目属性：链接器——附加库目录为OpenGL文件夹
### 代码示例解析
1. 模拟器初始化
```c++
Constellation cs;
Communication comm;

void SInit()
{
	cs = Constellation();
	comm = Communication();
	cs.attack[0].SetLatAndLon(comm.attack.latitude(), comm.attack.longitude());
	cs.attack[0].x = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[0];
	cs.attack[0].y = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[1];
	cs.attack[0].z = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[2];
	cs.AddGroundSandT(comm.source, comm.target);
}
```
2. 获取模拟延时
```c++
//更新卫星数据
cs.updateSatellites();
//判断是否找到可行通信路径
if (cs.pathId.size() != 0)
{
	cout << "通信延迟：" << comm.communication_stt(cs.pathDistance, cs.pathState) << endl;
	cout << "无噪声通信延迟：" << comm.communication_stt_no_noisy(cs.pathDistance, cs.pathState) << endl;
	cout << "最理想通信延迟：" << comm.communication_stt_ideal(cs.pathDistance) << endl;
}
```
### 文件目录解析
1. ConfigTxt文件夹——存储配置文件
2. KeplerOrbits文件夹——轨道计算辅助代码文件
3. OpenGL文件夹——Glut可视化库
4. Allinclude.h——所有使用到的库
5. Constellayion.cpp——星座模拟器实现代码
6. Constellayion.h——星座模拟器头文件
7. DelaySimulation.cpp——通信模拟器实现代码
8. DelaySimulation.h——通信模拟器头文件
9. LEOSimulation.cpp——卫星通信**示例**代码
10. LEOSimulation.h——示例代码头文件
### 配置详情 <a id="config">
1. city_data.txt：名字 纬度 经度
2. communication_config.txt：
* bandwidth 带宽大小 (bps)
* velocity 电磁波速度 (m/s)
* packet_size 模拟发送的包的大小
* noisy 噪声生成的分布 (1gauss 2random)
* distribution_left 噪声分布的区间左端点
* distribution_right 噪声分布的区间右端点
* Source 信息发送地面节点的纬度 经度
* Target 信息接收地面节点的纬度 经度
* Attack 攻击地面节点的纬度 经度

3. config.txt
* Mode 自动生成轨道 //手动设置轨道
* planesNum 自动生成的轨道数量
* nodesNum 自动生成的每个轨道上卫星的数量
* sma 卫星距地高度，用于计算长半轴 
* ManualPlanesNum 手动设置轨道的数量
* ManualNodesNum 手动设置的每个轨道上卫星的数量
* Ecc 偏心率
* Sma 长半轴
* Inc 倾角（相对于赤道）
* Lta 升交点位置（赤经/黄经）
* Ltp 近点位置（赤经/黄经）
* *注：对于每个轨道都要手动设置一遍偏心率到近点位置