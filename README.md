## LEOSimulation
### 功能
1. 模拟LEO卫星运动
2. 模拟两个地面站间借助LEO卫星的通信
3. 模拟地面攻击站点导致的延时效果
### 使用方法
1. 下载代码使用VS打开
2. 配置ConfigTxt文件夹下的配置文件（详细配置请于[配置详情](#config)查看）
>1. city_data.txt——从第二行开始，每一行为一个地面站点的经纬度信息
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
1. city_data.txt：

| 元素 |     描述     |  类型   |         限制         |    示例    |
| ---- | ------------ | ------ | ------------------- | ---------- |
| name | 地面站的名字 | string | 英文名字             | Haikou     |
| lat  | 纬度         | double | 精细到小数点后5或6位 | 19.808054  |
| lon  | 经度         | double | 精细到小数点后5或6位 | 110.302734 |
>参考示例文件，文件内第一行为描述，从第二行开始每一行均为一个地面站的配置信息
>经纬度精细到小数点后5或6位是要将模拟精细到米级
2. communication_config.txt：

|        元素        |            描述            |       类型       |                                  限制                                  |      默认值/示例      |
| ------------------ | -------------------------- | --------------- | ---------------------------------------------------------------------- | -------------------- |
| bandwidth          | 带宽大小                   | int             | 单位为bps，通信节点带宽大小，这里限制int大小，可以通过修改为长整型增大范围 | 100000000            |
| velocity           | 电磁波速度                 | int             | 一般为300000000m/s                                                     | 300000000            |
| packet_size        | 模拟发送的包的大小          | int             | 单位是b，通信包大小，这里限制int大小，可以通过修改为长整型增大范围         | 12000                |
| noisy              | 噪声生成的分布              | int             | 1=高斯分布，2=随机分布                                                  | 1                    |
| distribution_left  | 噪声分布的区间左端点        | double          | 0=<左端点<右端点                                                        | 0.5                  |
| distribution_right | 噪声分布的区间右端点        | double          | 右端点<=1                                                              | 1                    |
| Source             | 信息发送地面节点的纬度 经度 | <double,double> | 地面发送站的经纬度，取小数点后6位即可精确到米级                           | 19.808054 110.302734 |
| Target             | 信息接收地面节点的纬度 经度 | <double,double> | 地面目标站的经纬度，取小数点后6位即可精确到米级                           | 45.675482 126.738281 |
| Attack             | 攻击地面节点的纬度 经度     | <double,double> | 攻击中心点的经纬度，取小数点后6位即可精确到米级                           | 18.808054 110.302734 |
>攻击模拟为一个地面站点，所以影响范围与其他地面站点通信范围相同
3. config.txt
*注：对于每个轨道都要手动设置一遍偏心率到近点位置

|       元素       |             描述              |  类型   |              限制              | 默认值/示例 |
| --------------- | ---------------------------- | ------ | ------------------------------ | ----------- |
| Mode            | 轨道生成模式                  | string | 只可设置为Auto或Manual          | Auto        |
| planesNum       | 自动生成的轨道数量             | int    | 受限于仿真设备，一般设置<20     | 12          |
| nodesNum        | 自动生成的每个轨道上卫星的数量 | int    | 受限于仿真设备，一般设置<20     | 12          |
| sma             | 卫星距地高度，用于计算长半轴   | double | 单位为千米km                   | 500         |
| ManualPlanesNum | 手动设置轨道的数量             | int    | 受限于仿真设备，一般设置<20     | 12          |
| ManualNodesNum  | 手动设置的每个轨道上卫星的数量 | int    | 受限于仿真设备，一般设置<20     | 12          |
| Ecc             | 偏心率                        | double | 0=<偏心率<1以保证轨道为封闭曲线 | 0           |
| Sma             | 长半轴                        | double | 长半轴为x千米时，填入x*10       | 68781.37    |
| Inc             | 倾角（相对于赤道）             | double | 0=<倾角<=90，以度为单位         | 70          |
| Lta             | 升交点位置（赤经/黄经）        | double | 0=<升交点位置<=360，以度为单位  | 0           |
| Ltp             | 近点位置（赤经/黄经）          | double | 0=<近点位置<=360，以度为单位    | 0           |