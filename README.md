# OpenRM-2024



**This is the vision algorithm library for Beiyang Mecha 2024**


**Compatible with the open-source auto-aiming framework [TJURM-2024](https://github.com/HHgzs/TJURM-2024)**

**Welcome to check the Wiki for more information [TJURM Auto-Aiming Algorithm Wiki](https://github.com/HHgzs/TJURM-2024/wiki/TJURM%E8%87%AA%E7%9E%84%E7%AE%97%E6%B3%95Wiki)**





## 🌟 Code Highlights 🌟



### 🎖️ Full Lineup

Integrating **auto-aiming, rune activation, and outpost targeting** in one, compatible with all unit types: **Infantry, Sentry, Hero, and Drone**

**Bilibili: [【RM Auto-Aiming】Beiyang Mecha 2024 Season Auto-Aiming Open Source!](https://www.bilibili.com/video/BV1jApUe1EPT/)** 


### 🚀 Dynamic Link Library Design

This auto-aiming algorithm library adopts dynamic link library design for **quick start and convenient invocation**

- Supports cmake find_package invocation
- Full dynamic link library design

🎉🎉🎉 **User experience similar to OpenCV** 🎉🎉🎉





### 🙌 Algorithm and Framework Separation

**Algorithm Library**

- The algorithm library adopts dynamic link library form, making it easy to adapt to more vision projects and facilitate code reuse

- https://github.com/HHgzs/OpenRM-2024



**Auto-Aiming Framework**

- The auto-aiming framework has no specific algorithm implementation. It builds a pipeline architecture by calling algorithms implemented in the algorithm library, achieving high efficiency and scalability

- https://github.com/HHgzs/TJURM-2024







## 🦺 Environment Configuration 🦺

Below is a comprehensive tutorial for environment configuration. If you have any questions, please leave a message in the discussion area or communication group

**Note**:

- The project uses **OpenCV4.5.4**, your version should be as consistent with the project as possible

- You can install multiple versions coexisting according to the environment configuration process below

**Tips**

- **✅ AMD GPU / Non-NVIDIA Systems**: OpenRM now fully supports systems without NVIDIA hardware (AMD GPUs, Intel GPUs, macOS). TensorRT module automatically disabled. See [AMD_GPU_FIX.md](AMD_GPU_FIX.md) for details.
- **Without Nvidia hardware, OpenRM can still compile normally, the tensorrt module will automatically not participate in compilation**
- **Without Daheng camera driver, the repository can still compile normally, the industrial camera module will not participate in compilation**

### System Compatibility

| Platform | GPU | Status | Notes |
|----------|-----|--------|-------|
| Linux | NVIDIA | ✅ Full support | All features including TensorRT |
| Linux | AMD | ✅ Supported | All features except TensorRT |
| Linux | Intel | ✅ Supported | All features except TensorRT |
| macOS | Any | ✅ Supported | Development only, no TensorRT |



### Basic Environment

---



#### gcc/g++

The `gcc/g++` version used in this project is `8.4.0`, please ensure your `gcc/g++` version can compile this project normally

---



#### cmake

The `cmake` version used in this project is `3.22.1`, if your local `cmake` version is lower than `3.12`, you need to update the `cmake` version

Check the compatibility relationship between `cmake` version and `gcc/g++`:

[CXX_STANDARD](https://cmake.org/cmake/help/v3.12/prop_tgt/CXX_STANDARD.html)


First download the required version of `cmake`

[cmake](https://cmake.org/files/)

The version downloaded in this project is `cmake-3.22.1.tar.gz`,


Extract and enter the folder

```bash
tar -xzvf  cmake-3.22.1.tar.gz
cd cmake-3.22.1
```

If there is no bin directory, compile and install

```bash
./bootstrap
make -j6
sudo make install
```

If there is a bin directory, you can directly rename the folder and copy it to the software directory

```bash
# Return to parent directory
cd ..
mv cmake-3.22.1 cmake
sudo cp -r ./cmake /usr/local
```

Modify environment variables

```bash
vim ~/.bashrc
```

Add `export PATH=/usr/local/cmake/bin:$PATH` at the end of the file

```bash
source ~/.bashrc
cmake --version
```

At this point, it should correctly print version information. If cmake still cannot be used later, you need to add a symbolic link

```bash
sudo ln -s /usr/local/cmake/bin/cmake /usr/local/bin/camke
```

---



#### Eigen

You can install using apt

```bash
sudo apt install libeigen3-dev
```

You can also choose to compile from source
[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
[eigen-git-mirror](https://github.com/eigenteam/eigen-git-mirror)


Compile and install

```bash
cd eigen-git-mirror
mkdir build
cd build
cmake ..
sudo make install
```

---





#### Ceres

First install necessary dependencies

```bash
sudo apt-get install liblapack-dev libsuitesparse-dev libcxsparse3 libgflags-dev libgoogle-glog-dev libgtest-dev
```

Get ceres source code from Github
[ceres-solver](https://github.com/ceres-solver/ceres-solver)

Version `1.14.0` is used here, download the tar.gz compressed package


Decompress and compile

```bash
tar -zxvf ceres-solver-1.14.0.tar.gz 
cd ceres-solver-1.14.0/
mkdir build
cd build/

cmake ..
make -j6
sudo make install
```

---





#### Ncurses

Install with one command

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

Note that due to conflicts between the `OK` macro definition in ncurses and OpenCV, it needs to be modified

First modify the permissions

```bash
sudo chmod 770 /usr/include/curses.h
```

Use VSCode global replace to change `OK` to `KO`

Finally change the permissions back

```bash
sudo chmod 644 /usr/include/curses.h
```

---





#### Cuda, cudnn, TensorRT

Due to different device properties, please install by yourself. The **Nvidia NX** and **AGX** series system environments used in this project come with **CUDA** suite

**Without Nvidia hardware, without CUDA, OpenRM can still compile normally, tensorrt module will not participate in compilation**

---





#### OpenCV 4.5.4 Multiple Version Coexistence

First install necessary dependencies

```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config 
sudo apt-get install libgtk2.0-dev libavcodec-dev libavformat-dev  libtiff4-dev  libswscale-dev libjasper-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
```

**If dependencies are incomplete, please search online by yourself**



Go to the github repositories of `OpenCV` and `opencv_contrib` respectively to download the source code

Note that the versions of OpenCV and opencv_contrib must correspond correctly

- [opencv](https://github.com/opencv/opencv/releases)
- [opencv_contrib](https://github.com/opencv/opencv_contrib/tags)


Place `opencv-4.5.4.zip` and `opencv_contrib-4.5.4.zip` in the same folder and decompress them separately

```bash
unzip opencv-4.5.4.zip
unzip opencv_contrib-4.5.4.zip
cd opencv-4.5.4
mkdir build
cd build
```

Create a new folder under /usr/local/ to store the opencv version

```bash
mkdir /usr/local/opencv4.5.4
```

If you choose to install opencv's cuda functionality and opencv extension package, you can use the following command

```bash
cmake \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=/usr/local/opencv4.5.4 \
-DOPENCV_ENABLE_NONFREE=1 \
-DBUILD_opencv_python2=1 \
-DBUILD_opencv_python3=1 \
-DWITH_FFMPEG=1 \
-DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda \
-DCUDA_ARCH_BIN=7.2 \
-DCUDA_ARCH_PTX=7.2 \
-DWITH_CUDA=1 \
-DENABLE_FAST_MATH=1 \
-DCUDA_FAST_MATH=1 \
-DWITH_CUBLAS=1 \
-DOPENCV_GENERATE_PKGCONFIG=1 \
-DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-4.5.4/modules ..
```



The **CUDA_ARCH** version should be consistent with your own **CUDA**. Here we only introduce the installation method of OpenCV with CUDA. The installation without CUDA is similar



Then execute compile and install

```bash
make -j6
sudo make install
```

---





#### Daheng Driver

Go to Daheng official website to download c++ driver

https://www.daheng-imaging.com/downloads/

The one used in this project is 【Daheng Camera】【USB3.0】【ARM Linux】

**Galaxy Linux-armhf-Gige-U3 SDK**

If the camera driver is not installed, the repository can still compile normally

---







### OpenRM Installation

Use `run.sh` for automatic installation

```bash
cd OpenRM-2024
sudo ./run.sh -t
```



`run.sh` has multiple functions:

- **-t** After compiling and installing **OpenRM** dynamic link library, compile and install a parameter panel program named **openrm**
- **-r** Delete compilation and installation results, and recompile
- **-d** Completely delete OpenRM
- **-i** Reinstall
- **-g \<arg>** Call git, need to add commit
- Without parameters, only compile and install **OpenRM** dynamic link library





## 🧩 Module Introduction 🧩



### cudatools

CUDA programming section, compiled with **NVCC**, currently implements image resize functionality

```c++
void rm::resize(
    uint8_t* src,
    int src_width,
    int src_height,
    float* dst,
    int dst_width,
    int dst_height,
    void* cuda_stream
);
```



---



### tensorrt

Call tensorrt to accelerate inference, and yolo-series nms algorithms

```c++
bool rm::initTrtOnnx(
    const std::string& onnx_file,
    const std::string& engine_file,
    nvinfer1::IExecutionContext** context,
    unsigned int batch_size = 1U
);

bool rm::initTrtEngine(
    const std::string& engine_file,
    nvinfer1::IExecutionContext** context
);

bool rm::initCudaStream(
    cudaStream_t* stream
);

void rm::detectEnqueue(
    float* input_device_buffer,
    float* output_device_buffer,
    nvinfer1::IExecutionContext** context,
    cudaStream_t* stream
);

void rm::detectOutput(
    float* output_host_buffer,
    const float* output_device_buffer,
    cudaStream_t* stream,
    size_t output_struct_size,
    int bboxes_num,
    int batch_size = 1
); 

void rm::detectOutputClassify(
    float* output_host_buffer,
    const float* output_device_buffer,
    cudaStream_t* stream,
    int class_num
); 

void rm::mallocYoloCameraBuffer(
    uint8_t** rgb_host_buffer,
    uint8_t** rgb_device_buffer,
    int rgb_width,
    int rgb_height,
    int batch_size = 1,
    int channels = 3
);


void rm::mallocYoloDetectBuffer(
    float** input_device_buffer,
    float** output_device_buffer,
    float** output_host_buffer,
    int input_width,
    int input_height,
    size_t output_struct_size,
    int bboxes_num,
    int batch_size = 1,
    int channels = 3
);

void rm::mallocClassifyBuffer(
    float** input_host_buffer,
    float** input_device_buffer,
    float** output_device_buffer,
    float** output_host_buffer,
    int input_width,
    int input_height,
    int class_num,
    int channels = 3
);

void rm::freeYoloCameraBuffer(
    uint8_t* rgb_host_buffer,
    uint8_t* rgb_device_buffer
);


void rm::freeYoloDetectBuffer(
    float* input_device_buffer,
    float* output_device_buffer,
    float* output_host_buffer
);

void rm::freeClassifyBuffer(
    float* input_host_buffer,
    float* input_device_buffer,
    float* output_device_buffer,
    float* output_host_buffer
);

void rm::memcpyYoloCameraBuffer(
    uint8_t* rgb_mat_data,
    uint8_t* rgb_host_buffer,
    uint8_t* rgb_device_buffer,
    int rgb_width,
    int rgb_height,
    int channels = 3
);

void rm::memcpyClassifyBuffer(
    uint8_t* mat_data,
    float* input_host_buffer,
    float* input_device_buffer,
    int input_width,
    int input_height,
    int channels = 3
);

std::vector<YoloRect> rm::yoloArmorNMS_V5C36(
    float* output_host_buffer,
    int output_bboxes_num,
    int armor_classes_num,
    float confidence_threshold,
    float nms_threshold,
    int input_width,
    int input_height,
    int infer_width,
    int infer_height
);

std::vector<YoloRect> rm::yoloArmorNMS_V5(
    float* output_host_buffer,
    int output_bboxes_num,
    int armor_classes_num,
    float confidence_threshold,
    float nms_threshold,
    int input_width,
    int input_height,
    int infer_width,
    int infer_height
);

std::vector<YoloRect> rm::yoloArmorNMS_FP(
    float* output_host_buffer,
    int output_bboxes_num,
    int classes_num,
    float confidence_threshold,
    float nms_threshold,
    int input_width,
    int input_height,
    int infer_width,
    int infer_height
);

std::vector<YoloRect> rm::yoloArmorNMS_FPX(
    float* output_host_buffer,
    int output_bboxes_num,
    int classes_num,
    float confidence_threshold,
    float nms_threshold,
    int input_width,
    int input_height,
    int infer_width,
    int infer_height
);
```

---



### attach

Attack target selection and switching module

```c++
class AttackInterface;
```

```c++
bool rm::isValidArmorID(ArmorID armor_id, char valid_byte);
double rm::getAngleOffsetTargetToReferee(
    const double yaw,
    const double pitch,
    const double target_x,
    const double target_y,
    const double target_z,
    const double referee_x,
    const double referee_y,
    const double referee_z,
    const double referee_yaw = 0.0,
    const double referee_pitch = 0.0,
    const double axis_x = 0.0,
    const double axis_y = 0.0,
    const double axis_z = 0.0);
```



### kalman

Motion prediction models based on KF and EKF

```c++
class rm::AntitopV1;
class rm::AntitopV2;
class rm::AntitopV3;
```

```c++
class rm::OutpostV1;
class rm::OutpostV2;
```

```c++
class rm::RuneV1;
class rm::RuneV2;
```


```c++
class rm::TQstateV1;
class rm::TQstateV2;
class rm::TQstateV3;
class rm::TQstateV4;
```

```c++
class rm::trajectoryV1;
```



### uniterm

Library for openrm parameter panel program, can be called to add LOG

```c++
void rm::message(const std::string& name, int msg);
void rm::message(const std::string& name, float msg);
void rm::message(const std::string& name, double msg);
void rm::message(const std::string& name, char msg);
void rm::message(const std::string& name, MsgNum msg);
void rm::message(const std::string& msg, MSG type = MSG_NOTE);
void rm::message(const std::string& info, int img_width, int img_height, cv::Rect rect);
void rm::message(const std::string& info, int img_width, int img_height, std::vector<cv::Point2f> four_points);
void rm::message(const std::string& info, int img_width, int img_height, cv::Point2f pointX);
```



### pointer

Module for traditional vision to find light bars and four-point positioning



### solver

Implements PnP solving based on ternary method proposed by Shanghai Jiao Tong University Jiao Long team



### video

Camera driver interface, supports UVC camera and Daheng camera



### serial

Serial communication, supports usb to ttl and virtual serial port



### tf

Coordinate transformation, implements specific coordinate transformation used in auto-aiming



### delay

Flight delay calculation



### print

Parameter tuning printing tool



### timer

Time module









## 💡 Extension Method 💡




A total of five places need to be modified. Below is an example of adding `openrm_timer`, whose source code is `src/utils/timer.cpp`

`include/openrm.h`

```c++
#include "utils/timer.h"
```

`src/utils/CMakeLists.txt`

```cmake
add_library(
    openrm_timer
        SHARED
)
target_sources(
    openrm_timer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src/utils/timer.cpp
)
target_include_directories(
    openrm_timer
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/openrm>
)
```


`CMakeLists.txt`

```cmake
add_subdirectory(src/utils)
```


`CMakeLists.txt`

```cmake
if (CUDA_FOUND)
    set(
        TARGETS_LIST
            openrm_timer
    )
else()
    set(
        TARGETS_LIST
            openrm_timer
    )
endif()
```

`cmake/OpenRMCopfig.cmake.in`

```cmake
set(
    OPENRM_LIBS
        openrm::openrm_timer
)
```

`cmake/OpenRMConfig.cmake.in.nocuda`

```cmake
set(
    OPENRM_LIBS
        openrm::openrm_timer
)
```



