#ifndef SERIAL_H
#define SERIAL_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <exception>
#include <stdexcept>
#include <stdint.h>

//__FILE__ 指示本行语句所在源文件的文件名
//__LINE__ 指示本行语句所在源文件的行数
#define THROW(exceptionClass,message) throw exceptionClass((__FILE__),(__LINE__),(message))

// 定义串口可能的字节大小
typedef enum {
    fivebits = 5,
    sixbits = 6,
    sevenbits = 7,
    eightbits = 8
} bytesize_t;

// 定义串口可能的奇偶校验大小
typedef enum {
    parity_none = 0,
    parity_odd = 1,
    parity_even = 2,
    parity_mark = 3,
    parity_space =4
} parity_t;

// 定义串口可能的停止位
typedef enum {
    stopbits_one = 1,
    stopbits_two = 2,
    stopbits_one_point_five
} stopbits_t;

// 定义串口可能的流量控制值
typedef enum {
    flowcontrol_none = 0,
    flowcontrol_software,
    flowcontrol_hardware
} flowcontrol_t;

//串口的超时结构，时间单位为微秒
class Timeout {
public:
    static uint32_t max() {
        return std::numeric_limits<uint32_t>::max();
    }
    static Timeout simpleTimeout(uint32_t timeout) {
        return Timeout(max(), timeout, 0, timeout , 0);
    }

    // 接收到的字节与超时之间的毫秒数
    uint32_t inter_byte_timeout;
    // 调用read后恒定等待的毫秒数
    uint32_t read_timeout_constant;
    // 调用read后等待的请求字节数的倍数
    uint32_t read_timeout_multiplier;
    // 调用write后恒定等待的毫秒数
    uint32_t write_timeout_constant;
    // 调用write后等待的请求字节数的倍数
    uint32_t write_timeout_multiplier;

    // 构造函数，禁用单参数使用隐式转换构造函数构造
    explicit Timeout (uint32_t inter_byte_timeout_=0,
                      uint32_t read_timeout_constant_=0,
                      uint32_t read_timeout_multiplier_=0,
                      uint32_t write_timeout_constant_=0,
                      uint32_t write_timeout_multiplier_=0)
    : inter_byte_timeout(inter_byte_timeout_),
      read_timeout_constant(read_timeout_constant_),
      read_timeout_multiplier(read_timeout_multiplier_),
      write_timeout_constant(write_timeout_constant_),
      write_timeout_multiplier(write_timeout_multiplier_)
    {}
};

// 提供便携式串行端口接口的类
class Serial {
public:
    Serial (const std::string &port="",
            uint32_t baudrate = 115200,
            Timeout timeout = Timeout(),
            bytesize_t bytesize = eightbits,
            parity_t parity = parity_none,
            stopbits_t stopbits = stopbits_one,
            flowcontrol_t flowcontrol = flowcontrol_none);
    virtual ~Serial();
    // 当串口指定且未打开时，打开串口
    void open();
    // 获得串口状态
    /* C++ 静态方法
     * 函数前面使用const 修饰表示返回值为const
     * 后面使用 const修饰表示函数不可以修改class的成员
     */
    bool isOpen() const;
    // 关闭串口
    void close();
    // 返回缓冲区的字符数
    size_t available();
    /* 阻塞，直到有串行数据要读取或read_timeout_constant毫秒数已过去。
       当函数退出且端口处于可读状态时，返回值为true，否则为false
       （由于超时或选择中断）
     */
    bool waitReadable();
    /* 在当前串口设置下，阻塞与传输字符数量所需等待的相对应的时间
       该函数可以与waitReadable结合使用，以从端口读取更大的数据块。
     */
    void waitByteTimes(size_t count);
    /* 从串口中读取给定数量的字符到缓冲区，有且只有以下三种情况
       1. 读取完成
       2. 出现超时，在此情况下读取字符数与请求读取的字符数可能不相同
       3. 抛出异常

       参数如下
       @param buffer 记录所请求的内容的列表
       @param size 定义需要读取的字符数
     */
    size_t read(uint8_t *buffer, size_t size);
    size_t read(std::vector<uint8_t> &buffer, size_t size = 1);
    size_t read(std::string &buffer, size_t size = 1);
    std::string read(size_t size = 1);
    /* eol for end of line.

     */
    size_t readline(std::string &buffer, size_t size = 65536, std::string eol = "\n");
    std::string readline(size_t size = 65536, std::string eol ="\n");
    std::vector<std::string> readlines (size_t size = 65536, std::string eol = "\n");
    size_t write(const uint8_t *data, size_t size);
    size_t write(const std::vector<uint8_t> &data);
    size_t write(const std::string &data);
    void setPort(const std::string &port);
    std::string getPort () const;
    void setTimeout(uint32_t inter_byte_timeout,
                    uint32_t read_timeout_constant,
                    uint32_t read_timeout_multiplier,
                    uint32_t write_time_out_constant,
                    uint32_t write_timeout_multiplier); 
    // {
    //     Timeout timeout(inter_byte_timeout,
    //                     read_timeout_constant,
    //                     read_timeout_multiplier,
    //                     write_time_out_constant,
    //                     write_timeout_multiplier);
    //     return setTimeout(timeout);
    // }
    void setTimeout(Timeout &timeout);
    Timeout getTimeout() const;
    void setBaudrate(uint32_t baudrate);
    uint32_t getBaudrate () const;
    void setBytesize(bytesize_t bytesize);
    bytesize_t getBytesize() const;
    void setParity(parity_t parity);
    parity_t getParity() const;
    void setStopbits(stopbits_t stopbits);
    stopbits_t getStopbits() const;
    void setFlowcontrol(flowcontrol_t flowcontrol);
    flowcontrol_t getFlowcontrol() const;
    void flush();
    void flushInput();
    void flushOutput();
    void sendBreak(int duration);
    void setBreak(bool level = true);
    void setRTS(bool level = false);
    void setDTR(bool level = false);
    bool waitForChange();
    bool getCTS();
    bool getDSR();
    bool getRI();
    bool getCD();
private:
    // 禁用复制构造函数
    Serial(const Serial&);
    // 重载操作符等号，返回值是本类的引用类型
    Serial& operator=(const Serial&);

    // 串口实现类
    class SerialImpl;
    SerialImpl *pimpl_;

    // 锁
    class ScopedReadLock;
    class ScopedWriteLock;

    // 读函数
    size_t read_ (uint8_t *buffer, size_t size);
    // 写函数
    size_t write_ (const uint8_t *data, size_t length);
};

class SerialException : public std::exception{
    // 禁用复制构造函数
    SerialException& operator=(const SerialException&);
    std::string e_what_;
public:
    SerialException(const char *description){
        std::stringstream ss;
        ss << "SerialException " << description << "failed.";
        e_what_ = ss.str();
    }
    SerialException(const SerialException& other) : e_what_(other.e_what_) {}
    virtual ~SerialException() throw() {}
    virtual const char* what() const throw(){
        return e_what_.c_str();
    }
};

class IOException : public std::exception{
    // 禁用复制构造函数
    IOException& operator=(const IOException&);
    std::string file_;
    int line_;
    std::string e_what_;
    int errno_;
public:
    explicit IOException(std::string file, int line, int errnum) : file_(file),line_(line),errno_(errnum){
        std::stringstream ss;
        char * error_str = strerror(errnum);
        ss << "IO Exception (" << errno_ << "):" << error_str;
        ss << ", file " << file_ << ", line " << line_ << ".";
        e_what_ = ss.str();
    }
    explicit IOException (std::string file, int line, const char * description)
        : file_(file), line_(line), errno_(0) {
        std::stringstream ss;
        ss << "IO Exception: " << description;
        ss << ", file " << file_ << ", line " << line_ << ".";
        e_what_ = ss.str();
    }

    virtual ~IOException() throw() {}
    IOException (const IOException& other) : line_(other.line_), e_what_(other.e_what_), errno_(other.errno_) {}

    int getErrorNumber () const{
        return errno_;
    }

    virtual const char* what() const throw() {
        return e_what_.c_str();
    }
};

class PortNotOpenedException : public std::exception{
    const PortNotOpenedException& operator=(PortNotOpenedException);
    std::string e_what_;
public:
    PortNotOpenedException (const char * description)  {
        std::stringstream ss;
        ss << "PortNotOpenedException " << description << " failed.";
        e_what_ = ss.str();
    }
    PortNotOpenedException (const PortNotOpenedException& other) : e_what_(other.e_what_) {}
    virtual ~PortNotOpenedException() throw() {}
    virtual const char* what () const throw() {
        return e_what_.c_str();
    }
};

struct PortInfo {
    std::string port;
    std::string description;
    std::string hardware_id;
};

std::vector<PortInfo> list_ports();
std::vector<std::string> get_sysfs_info(const std::string& device_path);
#endif
