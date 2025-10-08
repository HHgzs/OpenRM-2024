#include "utils/serial.h"
#include "utils/timer.h"
#include "uniterm/uniterm.h"
#include <thread>
#include <iostream>  // 用于标准输入输出
#include <sstream>   // 用于字符串流
#include <fcntl.h>   // 用于文件控制
#include <termios.h> // 用于串口操作
#include <unistd.h>  // 用于POSIX标准的系统调用
#include <cstring>   // 用于处理字符串和内存操作
#include <cerrno>    // 用于错误处理
using namespace rm;
using namespace std;



SerialStatus rm::getSerialPortList(std::vector<std::string>& port_list, SerialType type) {
    std::string cmd;
    switch(type) {
        case SERIAL_TYPE_TTY_USB:
            cmd = "ls /dev/ttyUSB* --color=never";
            break;
        case SERIAL_TYPE_TTY_CH343USB:
            cmd = "ls /dev/ttyCH343USB* --color=never";
            break;
        case SERIAL_TYPE_TTY_THS:
            cmd = "ls /dev/ttyTHS* --color=never";
            break;
        case SERIAL_TYPE_TTY_ACM:
            cmd = "ls /dev/ttyACM* --color=never";
            break;
    }

    // 测试串口是否存在
    int exitCode = std::system(cmd.c_str());
    if (exitCode != 0) {
        rm::message("Serial port not found", rm::MSG_ERROR);
        return SERIAL_STATUS_NOT_FOUND;
    }

    // 执行命令并获取输出
    std::string command_output;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        rm::message("Serial port popen failed", rm::MSG_ERROR);
        return SERIAL_STATUS_NOT_FOUND;
    }

    // 设置缓冲区读取输出
    const int bufferSize = 128;
    char buffer[bufferSize];
    while (!feof(pipe)) {
        if (fgets(buffer, bufferSize, pipe) != nullptr) {
            command_output += buffer;
        }
    }
    pclose(pipe);

    // 将输出按行分割
    port_list.clear();
    std::istringstream iss(command_output);
    std::string line;
    while (std::getline(iss, line)) {
        port_list.push_back(line);
    }

    rm::message("Serial port found", rm::MSG_OK);
    return SERIAL_STATUS_OK;
}

SerialStatus rm::openSerialPort(
    int& file_descriptor, 
    std::string name, 
    int baudrate, 
    char parity_bit,
    int data_bit,
    int stop_bit
) {
    // 修改串口权限
    // string chmod_cmd = "chmod 777 " + name;
    // int exitCode = std::system(chmod_cmd.c_str());
    // if (exitCode != 0) {
    //     rm::message("Serial port chmod failed", rm::MSG_ERROR);
    //     return SERIAL_STATUS_CHMOD_FAILED;
    // }

    // 获取串口文件描述符
    file_descriptor = open(name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (file_descriptor == -1) {
        rm::message("Serial port open failed", rm::MSG_ERROR);
        return SERIAL_STATUS_OPEN_FAILED;
    }

    // 配置串口文件描述符
    if (fcntl(file_descriptor, F_SETFL, 0) < 0) {
        rm::message("Serial port set file status failed", rm::MSG_ERROR);
        return SERIAL_STATUS_SET_FILE_STATUS_FAILED;
    }
    
    // 配置串口属性
    termios newtio {}, oldtio {};
    if (tcgetattr(file_descriptor, &oldtio) != 0) {
        return SERIAL_STATUS_GET_OLDTIO_FAILED;
    }
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    // 设置字符大小
    switch (data_bit) {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
        default:
            break;
    }

    switch (parity_bit) {
        case 'O': // 奇校验
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E': // 偶校验
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'N': // 无校验
            newtio.c_cflag &= ~PARENB;
            break;
        default:
            break;
    }

    // 设置波特率
    switch (baudrate) {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
    }

    // 设置停止位
    if (stop_bit == 1) {
        newtio.c_cflag &= ~CSTOPB;
    } else if (stop_bit == 2) {
        newtio.c_cflag |= CSTOPB;
    }

    // 设置等待时间，最小字符
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(file_descriptor, TCIFLUSH);
    
    // 重新激活
    if ((tcsetattr(file_descriptor, TCSANOW, &newtio)) != 0) {
        rm::message("Serial port set newtio failed", rm::MSG_ERROR);
        return SERIAL_STATUS_SET_NEWTIO_FAILED;
    }

    return SERIAL_STATUS_OK;
}

SerialStatus rm::closeSerialPort(int file_descriptor) {
    close(file_descriptor);
    rm::message("Serial port closed", rm::MSG_WARNING);
    return SERIAL_STATUS_OK;
}

SerialStatus rm::restartSerialPort(
    int& file_descriptor, 
    std::string name, 
    int baudrate, 
    char parity_bit,
    int data_bit,
    int stop_bit
) {
    int status;
    rm::message("Serial port trying to restart", rm::MSG_WARNING);
    while(1) {
        if (access(name.c_str(), F_OK) < 0) {
            rm::message("Serial port restart failed : name error", rm::MSG_ERROR);
            return SERIAL_STATUS_NAME_ERROR;
        }
        closeSerialPort(file_descriptor);
        status = (int)openSerialPort(file_descriptor, name, baudrate, parity_bit, data_bit, stop_bit);
        if(status == SERIAL_STATUS_OK) {
            rm::message("Serial port restart success", rm::MSG_OK);
            break;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        
    }
    rm::message("Serial port restart success", rm::MSG_OK);
    return SERIAL_STATUS_OK;
}

SerialStatus rm::readFromSerialPort(
    unsigned char* data,
    unsigned int length,
    int& file_descriptor,
    bool restart,
    std::string name, 
    int baudrate, 
    char parity_bit,
    int data_bit,
    int stop_bit
) {
    if(file_descriptor <= 0){
        rm::message("Serial port read failed : invalid file descriptor", rm::MSG_ERROR);
       
        if(restart) {
            restartSerialPort(file_descriptor, name, baudrate, parity_bit, data_bit, stop_bit);
        } else {
            return SERIAL_STATUS_READ_FAILED;
        }
    }

    TimePoint tp0 = getTime();
    int cnt = 0, curr = 0;
    while (true) {
        
        curr = read(file_descriptor, data + cnt, length - cnt);

        TimePoint tp1 = getTime();
        double delay = getDoubleOfS(tp0, tp1);

        if (curr < 0 || delay > 0.2) {
            rm::message("Serial port read failed : read error", rm::MSG_ERROR);
            perror("read");
            if(restart) {
                openSerialPort(file_descriptor, name, baudrate, parity_bit, data_bit, stop_bit);
                continue;
            } else {
                closeSerialPort(file_descriptor);
                return SERIAL_STATUS_READ_FAILED;
            }
        }
        
        cnt += curr;
        if (cnt >= (int)length) {
            break;
        }
    }
    return SERIAL_STATUS_OK;
}

SerialStatus rm::writeToSerialPort(
    const unsigned char* data,
    unsigned int length,
    int& file_descriptor,
    bool restart,
    std::string name, 
    int baudrate, 
    char parity_bit,
    int data_bit,
    int stop_bit
) {
    if(file_descriptor <= 0){
        rm::message("Serial port write failed : invalid file descriptor", rm::MSG_ERROR);
        if(restart) {
            restartSerialPort(file_descriptor, name, baudrate, parity_bit, data_bit, stop_bit);
        } else {
            return SERIAL_STATUS_WRITE_FAILED;
        }
    }

    TimePoint tp0 = getTime();
    int cnt = 0, curr = 0;
    while (true) {
        curr = write(file_descriptor, data + cnt, length - cnt);

        TimePoint tp1 = getTime();
        double delay = getDoubleOfS(tp0, tp1);

        if (curr < 0 || delay > 0.2) {
            rm::message("Serial port write failed : write error", rm::MSG_ERROR);
            if(restart) {
                openSerialPort(file_descriptor, name, baudrate, parity_bit, data_bit, stop_bit);
                continue;
            } else {
                closeSerialPort(file_descriptor);
                return SERIAL_STATUS_WRITE_FAILED;
            }
        }
        cnt += curr;
        if (cnt >= (int)length) {
            break;
        }
    }
    return SERIAL_STATUS_OK;
}

SerialStatus rm::initSerialHead(int& file_descriptor, size_t struct_size, const unsigned char sof) {
    
    int input_size = struct_size + 1;
    char header[2 * input_size];

    if (readFromSerialPort((unsigned char *)header, 2 * input_size, file_descriptor) != SERIAL_STATUS_OK) {
        rm::message("Serial port head failed to find", rm::MSG_ERROR);
        return SERIAL_STATUS_INIT_HEAD_FAILED;
    }

    bool found = false;
    int start_index = 0;
    for (int i = 0; i < input_size; i++) {
        if (header[i] == sof && header[i + input_size] == sof) {
            found = true;
            start_index = i;
        }
    }
    if(!found) {
        rm::message("Serial port init head failed : header not found", rm::MSG_ERROR);
        return SERIAL_STATUS_INIT_HEAD_FAILED;
    }

    if(readFromSerialPort((unsigned char *)header, start_index, file_descriptor) != SERIAL_STATUS_OK) {
        rm::message("Serial port init head failed : read error while aheading", rm::MSG_ERROR);
        return SERIAL_STATUS_INIT_HEAD_FAILED;
    }
    
    return SERIAL_STATUS_OK;
}

