/**
@File      GxIAPI.h
@Brief     the interface for the GxIAPI dll module. 
@Author    Software Department
@Date      2023-03-16
@Version   1.4.2303.9161
*/

#ifndef GX_GALAXY_H
#define GX_GALAXY_H


////////////////////////////////////////////////////////////////////////////////////////////
//  Type definitions. The following types are defined in the standard C library header 
//  stdint.h. This file is not included in the previous version of Microsoft's compilation
//  platform VS 2010, so type definitions need to be redefined here.
////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
    #ifndef _STDINT_H 
        #ifdef _MSC_VER // Microsoft compiler
            #if _MSC_VER < 1600
                typedef __int8            int8_t;
                typedef __int16           int16_t;
                typedef __int32           int32_t;
                typedef __int64           int64_t;
                typedef unsigned __int8   uint8_t;
                typedef unsigned __int16  uint16_t;
                typedef unsigned __int32  uint32_t;
                typedef unsigned __int64  uint64_t;
            #else
                // In Visual Studio 2010 is stdint.h already included
                #include <stdint.h>
            #endif
        #else
            // Not a Microsoft compiler
            #include <stdint.h>
        #endif
    #endif 
#else
    // Linux
    #include <stdint.h>
#endif


//------------------------------------------------------------------------------
//  Operating System Platform Definition
//------------------------------------------------------------------------------

#include <stddef.h>

#ifdef WIN32
    #ifndef _WIN32
        #define _WIN32
    #endif
#endif

#ifdef _WIN32
    #include <Windows.h>
    #define GX_DLLIMPORT   __declspec(dllimport)
    #define GX_DLLEXPORT   __declspec(dllexport)

    #define GX_STDC __stdcall
    #define GX_CDEC __cdecl

    #if defined(__cplusplus)
        #define GX_EXTC extern "C"
    #else
        #define GX_EXTC
    #endif
#else
    // remove the None #define conflicting with GenApi
    #undef None
    #if __GNUC__>=4
        #define GX_DLLIMPORT   __attribute__((visibility("default")))
        #define GX_DLLEXPORT   __attribute__((visibility("default")))

        #if defined(__i386__)
            #define GX_STDC __attribute__((stdcall))
            #define GX_CDEC __attribute__((cdecl))
        #else
            #define GX_STDC 
            #define GX_CDEC 
        #endif

        #if defined(__cplusplus)
            #define GX_EXTC extern "C"
        #else
            #define GX_EXTC
        #endif
    #else
        #error Unknown compiler
    #endif
#endif

#ifdef GX_GALAXY_DLL
    #define GX_DLLENTRY GX_EXTC GX_DLLEXPORT
#else
    #define GX_DLLENTRY GX_EXTC GX_DLLIMPORT
#endif


//------------------------------------------------------------------------------
//  Error Code Definition
//------------------------------------------------------------------------------
typedef enum GX_STATUS_LIST
{
    GX_STATUS_SUCCESS                   =  0,          ///< Success
    GX_STATUS_ERROR                     = -1,          ///< There is an unspecified internal error that is not expected to occur
    GX_STATUS_NOT_FOUND_TL              = -2,          ///< The TL library cannot be found
    GX_STATUS_NOT_FOUND_DEVICE          = -3,          ///< The device is not found
    GX_STATUS_OFFLINE                   = -4,          ///< The current device is in an offline status
    GX_STATUS_INVALID_PARAMETER         = -5,          ///< Invalid parameter. Generally, the pointer is NULL or the input IP and other parameter formats are invalid
    GX_STATUS_INVALID_HANDLE            = -6,          ///< Invalid handle
    GX_STATUS_INVALID_CALL              = -7,          ///< The interface is invalid, which refers to software interface logic error
    GX_STATUS_INVALID_ACCESS            = -8,          ///< The function is currently inaccessible or the device access mode is incorrect
    GX_STATUS_NEED_MORE_BUFFER          = -9,          ///< The user request buffer is insufficient: the user input buffer size during the read operation is less than the actual need
    GX_STATUS_ERROR_TYPE                = -10,         ///< The type of FeatureID used by the user is incorrect, such as an integer interface using a floating-point function code
    GX_STATUS_OUT_OF_RANGE              = -11,         ///< The value written by the user is crossed
    GX_STATUS_NOT_IMPLEMENTED           = -12,         ///< This function is not currently supported
    GX_STATUS_NOT_INIT_API              = -13,         ///< There is no call to initialize the interface
    GX_STATUS_TIMEOUT                   = -14,         ///< Timeout error
}GX_STATUS_LIST;
typedef int32_t GX_STATUS;

//------------------------------------------------------------------------------
//  Frame Status Definition
//------------------------------------------------------------------------------
typedef enum GX_FRAME_STATUS_LIST
{
    GX_FRAME_STATUS_SUCCESS             = 0,           ///< Normal frame
    GX_FRAME_STATUS_INCOMPLETE          = -1,          ///< Incomplete frame
    GX_FRAME_STATUS_INVALID_IMAGE_INFO  = -2,          ///< Information Error Frame
}GX_FRAME_STATUS_LIST;
typedef  int32_t  GX_FRAME_STATUS;

//------------------------------------------------------------------------------
//  Device Type Definition
//------------------------------------------------------------------------------
typedef enum GX_DEVICE_CLASS_LIST
{
    GX_DEVICE_CLASS_UNKNOWN             = 0,           ///< Unknown device type
    GX_DEVICE_CLASS_USB2                = 1,           ///< USB2.0 Vision device
    GX_DEVICE_CLASS_GEV                 = 2,           ///< Gige Vision device
    GX_DEVICE_CLASS_U3V                 = 3,           ///< USB3 Vision device
    GX_DEVICE_CLASS_SMART               = 4,           ///< Smart camera device
}GX_DEVICE_CLASS_LIST;
typedef  int32_t GX_DEVICE_CLASS;

//------------------------------------------------------------------------------
//  Feature Mask Definition
//------------------------------------------------------------------------------
typedef enum GX_FEATURE_MASK
{
    GX_FEATURE_TYPE_MASK                = 0xF0000000,  ///Feature type mask
    GX_FEATURE_LEVEL_MASK               = 0x0F000000,  ///Feature level mask
}GX_FEATURE_MASK;

//------------------------------------------------------------------------------
//  Feature Type Definition
//------------------------------------------------------------------------------
typedef enum GX_FEATURE_TYPE
{
    GX_FEATURE_INT                      = 0x10000000,  ///< Integer type
    GX_FEATURE_FLOAT                    = 0X20000000,  ///< Floating point type
    GX_FEATURE_ENUM                     = 0x30000000,  ///< Enum type
    GX_FEATURE_BOOL                     = 0x40000000,  ///< Boolean type
    GX_FEATURE_STRING                   = 0x50000000,  ///< String type
    GX_FEATURE_BUFFER                   = 0x60000000,  ///< Block data type
    GX_FEATURE_COMMAND                  = 0x70000000,  ///< Command type
}GX_FEATURE_TYPE;

//------------------------------------------------------------------------------
//  Feature Level Definition
//------------------------------------------------------------------------------
typedef enum GX_FEATURE_LEVEL
{
    GX_FEATURE_LEVEL_REMOTE_DEV         = 0x00000000,  ///< Remote device layer
    GX_FEATURE_LEVEL_TL                 = 0x01000000,  ///< TL layer
    GX_FEATURE_LEVEL_IF                 = 0x02000000,  ///< Interface layer    
    GX_FEATURE_LEVEL_DEV                = 0x03000000,  ///< Device layer
    GX_FEATURE_LEVEL_DS                 = 0x04000000,  ///< DataStream layer
}GX_FEATURE_LEVEL;

//------------------------------------------------------------------------------
//  Access Mode of Device
//------------------------------------------------------------------------------
typedef enum GX_ACCESS_MODE
{
    GX_ACCESS_READONLY                  = 2,           ///< Open the device in read-only mode
    GX_ACCESS_CONTROL                   = 3,           ///< Open the device in controlled mode
    GX_ACCESS_EXCLUSIVE                 = 4,           ///< Open the device in exclusive mode
}GX_ACCESS_MODE;
typedef int32_t GX_ACCESS_MODE_CMD;

//------------------------------------------------------------------------------
//  Access Status of Current Device
//------------------------------------------------------------------------------
typedef enum GX_ACCESS_STATUS
{
    GX_ACCESS_STATUS_UNKNOWN            = 0,           ///< The device's current status is unknown
    GX_ACCESS_STATUS_READWRITE          = 1,           ///< The device currently supports reading and writing
    GX_ACCESS_STATUS_READONLY           = 2,           ///< The device currently only supports reading
    GX_ACCESS_STATUS_NOACCESS           = 3,           ///< The device currently does neither support reading nor support writing
}GX_ACCESS_STATUS;
typedef int32_t GX_ACCESS_STATUS_CMD;

//------------------------------------------------------------------------------
//  Open Mode Definition
//------------------------------------------------------------------------------
typedef enum GX_OPEN_MODE
{
    GX_OPEN_SN                          = 0,           ///< Opens the device via a serial number
    GX_OPEN_IP                          = 1,           ///< Opens the device via an IP address
    GX_OPEN_MAC                         = 2,           ///< Opens the device via a MAC address
    GX_OPEN_INDEX                       = 3,           ///< Opens the device via a serial number (Start from 1, such as 1, 2, 3, 4...)
    GX_OPEN_USERID                      = 4,           ///< Opens the device via user defined ID
}GX_OPEN_MODE;
typedef int32_t GX_OPEN_MODE_CMD;

//------------------------------------------------------------------------------
//  IP Configure Mode
//------------------------------------------------------------------------------
enum GX_IP_CONFIGURE_MODE_LIST
{
    GX_IP_CONFIGURE_DHCP                = 0x6,         ///< Enable the DHCP mode to allocate the IP address by the DHCP server
    GX_IP_CONFIGURE_LLA                 = 0x4,         ///< Enable the LLA mode to allocate the IP address
    GX_IP_CONFIGURE_STATIC_IP           = 0x5,         ///< Enable the static IP mode to configure the IP address
    GX_IP_CONFIGURE_DEFAULT             = 0x7,         ///< Enable the default mode to configure the IP address
};
typedef int32_t GX_IP_CONFIGURE_MODE;

typedef enum GX_FEATURE_ID
{
    //////////////////////////////////////////////////////////////////////////
    /// Remote device layer(Remote Device Feature)
    //////////////////////////////////////////////////////////////////////////

    //---------------DeviceInfomation Section--------------------------
    GX_STRING_DEVICE_VENDOR_NAME               = 0   | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Name of the manufacturer of the device.
    GX_STRING_DEVICE_MODEL_NAME                = 1   | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Model of the device.
    GX_STRING_DEVICE_FIRMWARE_VERSION          = 2   | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Version of the firmware in the device.
    GX_STRING_DEVICE_VERSION                   = 3   | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Version of the device.
    GX_STRING_DEVICE_SERIAL_NUMBER             = 4   | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Device serial number.
    GX_STRING_FACTORY_SETTING_VERSION          = 6   | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Factory parameter version
    GX_STRING_DEVICE_USERID                    = 7   | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< User-programmable device identifier.
    GX_INT_DEVICE_LINK_SELECTOR                = 8   | GX_FEATURE_INT    | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects which Link of the device to control.
    GX_ENUM_DEVICE_LINK_THROUGHPUT_LIMIT_MODE  = 9   | GX_FEATURE_ENUM   | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Controls if the DeviceLinkThroughputLimit is active.
    GX_INT_DEVICE_LINK_THROUGHPUT_LIMIT        = 10  | GX_FEATURE_INT    | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Limits the maximum bandwidth of the data that will be streamed out by the device on the selected Link.
    GX_INT_DEVICE_LINK_CURRENT_THROUGHPUT      = 11  | GX_FEATURE_INT    | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< The bandwidth of current device acquisition
    GX_COMMAND_DEVICE_RESET                    = 12  | GX_FEATURE_COMMAND| GX_FEATURE_LEVEL_REMOTE_DEV,   ///< reset device
    GX_INT_TIMESTAMP_TICK_FREQUENCY            = 13  | GX_FEATURE_INT    | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Time stamp clock frequency
    GX_COMMAND_TIMESTAMP_LATCH                 = 14  | GX_FEATURE_COMMAND| GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Timestamp latch 
    GX_COMMAND_TIMESTAMP_RESET                 = 15  | GX_FEATURE_COMMAND| GX_FEATURE_LEVEL_REMOTE_DEV,   ///< reset Timestamp
    GX_COMMAND_TIMESTAMP_LATCH_RESET           = 16  | GX_FEATURE_COMMAND| GX_FEATURE_LEVEL_REMOTE_DEV,   ///< reset Timestamp latch
    GX_INT_TIMESTAMP_LATCH_VALUE               = 17  | GX_FEATURE_INT    | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Timestamp Latch value
    GX_STRING_DEVICE_PHY_VERSION               = 18  | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Device network chip version
    GX_ENUM_DEVICE_TEMPERATURE_SELECTOR        = 19  | GX_FEATURE_ENUM   | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Device temperature selection
    GX_FLOAT_DEVICE_TEMPERATURE                = 20  | GX_FEATURE_FLOAT  | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Device temperature
    GX_STRING_DEVICE_ISP_FIRMWARE_VERSION      = 21  | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Devide isp firmware version
    GX_ENUM_LOWPOWER_MODE                      = 22  | GX_FEATURE_ENUM   | GX_FEATURE_LEVEL_REMOTE_DEV,   ///<Low power consumption mode, see also GX_LOWPOWER_MODE_ENT
	GX_ENUM_CLOSE_CCD							= 23 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Close CCD, refer to GX_CLOSE_CCD_ENTRY
	GX_STRING_PRODUCTION_CODE					= 24 | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Production code
	GX_STRING_DEVICE_ORIGINAL_NAME				= 25 | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Original name
	GX_INT_REVISION								= 26 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< CXP protocol version
	GX_INT_VERSIONS_SUPPORTED					= 27 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Supported CXP protocol versions
	GX_INT_VERSION_USED							= 28 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Use version
	GX_BOOL_TEC_ENABLE							= 29 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,		///< TEC switch
	GX_FLOAT_TEC_TARGET_TEMPERATURE				= 30 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< TEC target temperature
	GX_BOOL_FAN_ENABLE							= 31 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Fan switch
	GX_INT_TEMPERATURE_DETECTION_STATUS			= 32 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Temperature state detection
	GX_INT_FAN_SPEED							= 33 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Fan speed
	GX_FLOAT_DEVICE_HUMIDITY					= 34 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Equipment humidity 
	GX_FLOAT_DEVICE_PRESSURE					= 35 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Equipment air pressure
	GX_INT_AIR_CHANGE_DETECTION_STATUS			= 36 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Ventilation status detection
	GX_INT_AIR_TIGHTNESS_DETECTION_STATUS		= 37 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Airtightness state detection

    //---------------ImageFormat Section--------------------------------
    GX_INT_SENSOR_WIDTH               = 1000 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Effective width of the sensor in pixels.
    GX_INT_SENSOR_HEIGHT              = 1001 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Effective height of the sensor in pixels.
    GX_INT_WIDTH_MAX                  = 1002 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Maximum width of the image (in pixels).
    GX_INT_HEIGHT_MAX                 = 1003 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Maximum height of the image (in pixels).
    GX_INT_OFFSET_X                   = 1004 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Horizontal offset from the origin to the region of interest (in pixels).
    GX_INT_OFFSET_Y                   = 1005 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Vertical offset from the origin to the region of interest (in pixels).
    GX_INT_WIDTH                      = 1006 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Width of the image provided by the device (in pixels).
    GX_INT_HEIGHT                     = 1007 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Height of the image provided by the device (in pixels).
    GX_INT_BINNING_HORIZONTAL         = 1008 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Number of horizontal photo-sensitive cells to combine together.
    GX_INT_BINNING_VERTICAL           = 1009 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Number of vertical photo-sensitive cells to combine together.
    GX_INT_DECIMATION_HORIZONTAL      = 1010 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Horizontal sub-sampling of the image.
    GX_INT_DECIMATION_VERTICAL        = 1011 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Vertical sub-sampling of the image.
    GX_ENUM_PIXEL_SIZE                = 1012 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Total size in bits of a pixel of the image.
    GX_ENUM_PIXEL_COLOR_FILTER        = 1013 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Type of color filter that is applied to the image.
    GX_ENUM_PIXEL_FORMAT              = 1014 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Format of the pixels provided by the device.
    GX_BOOL_REVERSE_X                 = 1015 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Flip horizontally the image sent by the device.
    GX_BOOL_REVERSE_Y                 = 1016 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Flip vertically the image sent by the device.
    GX_ENUM_TEST_PATTERN              = 1017 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Selects the type of test pattern that is generated by the device as image source.
    GX_ENUM_TEST_PATTERN_GENERATOR_SELECTOR = 1018 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects which test pattern generator is controlled by the TestPattern feature.
    GX_ENUM_REGION_SEND_MODE          = 1019 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< ROI output mode, see also GX_REGION_SEND_MODE_ENTRY
    GX_ENUM_REGION_MODE               = 1020 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< zone switch, see also GX_REGION_MODE_ENTRY
    GX_ENUM_RREGION_SELECTOR          = 1021 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Selects the Region of interest to control.
    GX_INT_CENTER_WIDTH               = 1022 |GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,      ///< width of window
    GX_INT_CENTER_HEIGHT              = 1023 |GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,      ///< height of window
    GX_ENUM_BINNING_HORIZONTAL_MODE   = 1024 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Binning Horizontal mode, see also GX_BINNING_HORIZONTAL_MODE_ENTRY
    GX_ENUM_BINNING_VERTICAL_MODE     = 1025 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Binning vertical mode, see also GX_BINNING_VERTICAL_MODE_ENTRY
    GX_ENUM_SENSOR_SHUTTER_MODE       = 1026 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sensor shutter mode, see also GX_SENSOR_SHUTTER_MODE_ENTRY
    GX_INT_DECIMATION_LINENUMBER      = 1027 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Decimation linenumber
	GX_INT_SENSOR_DECIMATION_HORIZONTAL = 1028 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Sensor decimation horizontal
	GX_INT_SENSOR_DECIMATION_VERTICAL   = 1029 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Sensor decimation vertical
 	GX_ENUM_SENSOR_SELECTOR             = 1030 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Select the current sensor type，see also GX_SENSOR_SELECTOR_ENTRY
	GX_INT_CURRENT_SENSOR_WIDTH         = 1031 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Current sensor image width
	GX_INT_CURRENT_SENSOR_HEIGHT        = 1032 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Current sensor image height
	GX_INT_CURRENT_SENSOR_OFFSETX       = 1033 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Current sensor horizontal offset
	GX_INT_CURRENT_SENSOR_OFFSETY       = 1034 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Current sensor vertical offset
	GX_INT_CURRENT_SENSOR_WIDTHMAX      = 1035 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Maximum current sensor width
	GX_INT_CURRENT_SENSOR_HEIGHTMAX     = 1036 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Maximum current sensor height
	GX_ENUM_SENSOR_BIT_DEPTH			= 1037 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,	 ///< Sensor Bit Depth, refer to GX_SENSOR_BIT_DEPTH_ENTRY
	GX_BOOL_WATERMARK_ENABLE			= 1038 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Watermark

    //---------------TransportLayer Section-------------------------------
    GX_INT_PAYLOAD_SIZE                              = 2000 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Provides the number of bytes transferred for each image or chunk on the stream channel. 
    GX_BOOL_GEV_CURRENT_IPCONFIGURATION_LLA          = 2001 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Controls whether the Link Local Address IP configuration scheme is activated on the given logical link.
    GX_BOOL_GEV_CURRENT_IPCONFIGURATION_DHCP         = 2002 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Controls whether the DHCP IP configuration scheme is activated on the given logical link.
    GX_BOOL_GEV_CURRENT_IPCONFIGURATION_PERSISTENTIP = 2003 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Controls whether the PersistentIP configuration scheme is activated on the given logical link.
    GX_INT_ESTIMATED_BANDWIDTH                       = 2004 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< EstimatedBandwidth, Unit: Bps(Bytes per second)
    GX_INT_GEV_HEARTBEAT_TIMEOUT                     = 2005 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Controls the current heartbeat timeout in milliseconds.
    GX_INT_GEV_PACKETSIZE                            = 2006 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Specifies the stream packet size, in bytes, to send on the selected channel for a GVSP transmitter or specifies the maximum packet size supported by a GVSP receiver.
    GX_INT_GEV_PACKETDELAY                           = 2007 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Controls the delay (in timestamp counter unit) to insert between each packet for this stream channel.
    GX_INT_GEV_LINK_SPEED                            = 2008 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< It indicates the connection speed in Mbps for the selected network interface.
	GX_ENUM_DEVICE_TAP_GEOMETRY						 = 2009 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Equipment geometry

    //---------------AcquisitionTrigger Section---------------------------
    GX_ENUM_ACQUISITION_MODE          = 3000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sets the acquisition mode of the device.
    GX_COMMAND_ACQUISITION_START      = 3001 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Starts the Acquisition of the device.
    GX_COMMAND_ACQUISITION_STOP       = 3002 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Stops the Acquisition of the device at the end of the current Frame.
    GX_INT_ACQUISITION_SPEED_LEVEL    = 3003 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Setting the speed level of acquiring image.
    GX_INT_ACQUISITION_FRAME_COUNT    = 3004 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Number of frames to acquire in MultiFrame Acquisition mode.
    GX_ENUM_TRIGGER_MODE              = 3005 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Controls if the selected trigger is active.
    GX_COMMAND_TRIGGER_SOFTWARE       = 3006 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Generates an internal trigger.
    GX_ENUM_TRIGGER_ACTIVATION        = 3007 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Specifies the activation mode of the trigger.
    GX_ENUM_TRIGGER_SWITCH            = 3008 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Control external trigger signal is valid, see also GX_TRIGGER_SWITCH_ENTRY
    GX_FLOAT_EXPOSURE_TIME            = 3009 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Sets the Exposure time when ExposureMode is Timed and ExposureAuto is Off.
    GX_ENUM_EXPOSURE_AUTO             = 3010 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sets the automatic exposure mode when ExposureMode is Timed.
    GX_FLOAT_TRIGGER_FILTER_RAISING   = 3011 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Raising edge signal pulse width is smaller than this value is invalid.
    GX_FLOAT_TRIGGER_FILTER_FALLING   = 3012 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Falling edge signal pulse width is smaller than this value is invalid.
    GX_ENUM_TRIGGER_SOURCE            = 3013 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Specifies the internal signal or physical input Line to use as the trigger source.
    GX_ENUM_EXPOSURE_MODE             = 3014 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sets the operation mode of the Exposure (or shutter).
    GX_ENUM_TRIGGER_SELECTOR          = 3015 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Selects the type of trigger to configure.
    GX_FLOAT_TRIGGER_DELAY            = 3016 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Specifies the delay in microseconds (us) to apply after the trigger reception before activating it.
    GX_ENUM_TRANSFER_CONTROL_MODE     = 3017 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Selects the control method for the transfers.
    GX_ENUM_TRANSFER_OPERATION_MODE   = 3018 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Selects the operation mode of the transfer.
    GX_COMMAND_TRANSFER_START         = 3019 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Starts the streaming of data blocks out of the device.
    GX_INT_TRANSFER_BLOCK_COUNT       = 3020 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< frame number of transmission. when set GX_ENUM_TRANSFER_OPERATION_MODE as GX_ENUM_TRANSFER_OPERATION_MODE_MULTIBLOCK, this function is actived
    GX_BOOL_FRAMESTORE_COVER_ACTIVE   = 3021 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< FrameBufferOverwriteActive
    GX_ENUM_ACQUISITION_FRAME_RATE_MODE			= 3022  | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Controls if the acquisitionFrameRate is active, see also GX_ACQUISITION_FRAME_RATE_MODE_ENTRY
    GX_FLOAT_ACQUISITION_FRAME_RATE				= 3023  | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Controls the acquisition rate (in Hertz) at which the frames are captured.
    GX_FLOAT_CURRENT_ACQUISITION_FRAME_RATE		= 3024  | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Indicates the maximum allowed frame acquisition rate.
    GX_ENUM_FIXED_PATTERN_NOISE_CORRECT_MODE	= 3025  | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Controls if the FixedPatternNoise is active, see also GX_FIXED_PATTERN_NOISE_CORRECT_MODE  
    GX_INT_ACQUISITION_BURST_FRAME_COUNT		= 3030  | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< frame number of transmission.
    GX_ENUM_ACQUISITION_STATUS_SELECTOR			= 3031  | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Acquisition status selection, see also GX_ACQUISITION_STATUS_SELECTOR_ENTRY
    GX_BOOL_ACQUISITION_STATUS					= 3032  | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Acquisition status
    GX_FLOAT_EXPOSURE_DELAY						= 30300 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Delay of exposure
    GX_FLOAT_EXPOSURE_OVERLAP_TIME_MAX			= 30301 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Maximum overlap exposure time
    GX_ENUM_EXPOSURE_TIME_MODE					= 30302 | GX_FEATURE_ENUM  | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Exposure time mode, see also GX_EXPOSURE_TIME_MODE_ENTRY
    GX_ENUM_ACQUISITION_BURST_MODE				= 30303 | GX_FEATURE_ENUM  | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Burst acquisition mode，see also GX_EXPOSURE_TIME_MODE_ENTRY
	GX_ENUM_OVERLAP_MODE						= 30304 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< overlap mode,refer to GX_OVERLAP_MODE_ENTRY
	GX_ENUM_MULTISOURCE_SELECTOR				= 30305 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,	///< MultiSourceSelector to GX_MULTISOURCE_SELECTOR_ENTRY
	GX_BOOL_MULTISOURCE_ENABLE					= 30306 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< MultiSource Trigger Enable
	GX_BOOL_TRIGGER_CACHE_ENABLE				= 30307 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Trigger Cache Enable

    //----------------DigitalIO Section----------------------------------
    GX_ENUM_USER_OUTPUT_SELECTOR      = 4000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects which bit of the User Output register will be set by UserOutputValue.
    GX_BOOL_USER_OUTPUT_VALUE         = 4001 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Sets the value of the bit selected by UserOutputSelector.
    GX_ENUM_USER_OUTPUT_MODE          = 4002 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Output signal can be used for different purposes, flash or a user-defined constant level, see also GX_USER_OUTPUT_MODE_ENTRY
    GX_ENUM_STROBE_SWITCH             = 4003 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Set the flash light switch, see also GX_STROBE_SWITCH_ENTRY
    GX_ENUM_LINE_SELECTOR             = 4004 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects the physical line (or pin) of the external device connector to configure.
    GX_ENUM_LINE_MODE                 = 4005 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Controls if the physical Line is used to Input or Output a signal.
    GX_BOOL_LINE_INVERTER             = 4006 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Controls the inversion of the signal of the selected input or output Line.
    GX_ENUM_LINE_SOURCE               = 4007 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects which internal acquisition or I/O source signal to output on the selected Line.
    GX_BOOL_LINE_STATUS               = 4008 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Returns the current status of the selected input or output Line.
    GX_INT_LINE_STATUS_ALL            = 4009 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Returns the current status of all available Line signals at time of polling in a single bit field.
    GX_FLOAT_PULSE_WIDTH              = 4010 | GX_FEATURE_FLOAT| GX_FEATURE_LEVEL_REMOTE_DEV,
    GX_INT_LINE_RANGE                 = 4011 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Flash lamp pulse width
    GX_INT_LINE_DELAY                 = 4012 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Flash Delay
	GX_INT_LINE_FILTER_RAISING_EDGE		= 4013 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Pin rising edge filtering
	GX_INT_LINE_FILTER_FALLING_EDGE		= 4014 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Pin falling edge filtering
    
    //----------------AnalogControls Section----------------------------
    GX_ENUM_GAIN_AUTO                 = 5000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Sets the automatic gain control (AGC) mode.
    GX_ENUM_GAIN_SELECTOR             = 5001 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects which Gain is controlled by the various Gain features.
    GX_ENUM_BLACKLEVEL_AUTO           = 5003 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Controls the mode for automatic black level adjustment.
    GX_ENUM_BLACKLEVEL_SELECTOR       = 5004 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects which Black Level is controlled by the various Black Level features.
    GX_ENUM_BALANCE_WHITE_AUTO        = 5006 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Controls the mode for automatic white balancing between the color channels.
    GX_ENUM_BALANCE_RATIO_SELECTOR    = 5007 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects which Balance ratio to control.
    GX_FLOAT_BALANCE_RATIO            = 5008 | GX_FEATURE_FLOAT| GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Controls ratio of the selected color component to a reference color component.
    GX_ENUM_COLOR_CORRECT             = 5009 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Color correction, see also GX_COLOR_CORRECT_ENTRY
    GX_ENUM_DEAD_PIXEL_CORRECT        = 5010 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< The dead pixel correct function can eliminate dead pixels in the image, see also GX_DEAD_PIXEL_CORRECT_ENTRY
    GX_FLOAT_GAIN                     = 5011 | GX_FEATURE_FLOAT| GX_FEATURE_LEVEL_REMOTE_DEV,   ///< The value is an float value that sets the selected gain control in units specific to the camera.
    GX_FLOAT_BLACKLEVEL               = 5012 | GX_FEATURE_FLOAT| GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Controls the analog black level as an absolute physical value.
    GX_BOOL_GAMMA_ENABLE              = 5013 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Enable bit of Gamma
    GX_ENUM_GAMMA_MODE                = 5014 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Gamma select, see also GX_GAMMA_MODE_ENTRY
    GX_FLOAT_GAMMA                    = 5015 | GX_FEATURE_FLOAT| GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Gamma
    GX_INT_DIGITAL_SHIFT              = 5016 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< bit select
    GX_ENUM_LIGHT_SOURCE_PRESET       = 5017 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Light source preset, see also GX_LIGHT_SOURCE_PRESET_ENTRY
	GX_BOOL_BLACKLEVEL_CALIB_STATUS		= 5018 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,		///< BlackLevelCalibStatus
	GX_INT_BLACKLEVEL_CALIB_VALUE		= 5019 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< BlackLevelCalibValue
	GX_FLOAT_PGA_GAIN					= 5020 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,	///< PGAGain BlackLevelCalibStatus

    //---------------CustomFeature Section-------------------------
    GX_INT_ADC_LEVEL                  = 6000 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< When the pixel size is not 8bits, this function can be used to choose 8bits form 10bits or 12bit for show image.
    GX_INT_H_BLANKING                 = 6001 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Horizontal blanking
    GX_INT_V_BLANKING                 = 6002 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Vertical blanking
    GX_STRING_USER_PASSWORD           = 6003 | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV, ///< user password
    GX_STRING_VERIFY_PASSWORD         = 6004 | GX_FEATURE_STRING | GX_FEATURE_LEVEL_REMOTE_DEV, ///< verify password
    GX_BUFFER_USER_DATA               = 6005 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV, ///< user data
    GX_INT_GRAY_VALUE                 = 6006 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< ExpectedGrayValue_InqIsImplemented
    GX_ENUM_AA_LIGHT_ENVIRONMENT      = 6007 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Automatic function according to the external light conditions better for accommodation, see also GX_AA_LIGHT_ENVIRMENT_ENTRY
    GX_INT_AAROI_OFFSETX              = 6008 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< This value sets the X offset (left offset) for the rect of interest in pixels for 2A, i.e., the distance in pixels between the left side of the image area and the left side of the AAROI.
    GX_INT_AAROI_OFFSETY              = 6009 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< This value sets the Y offset (top offset) for the rect of interest for 2A, i.e., the distance in pixels between the top of the image area and the top of the AAROI.
    GX_INT_AAROI_WIDTH                = 6010 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< This value sets the width of the rect of interest in pixels for 2A.
    GX_INT_AAROI_HEIGHT               = 6011 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< This value sets the height of the rect of interest in pixels for 2A.
    GX_FLOAT_AUTO_GAIN_MIN            = 6012 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Setting up automatic gain range of minimum. When the gain is set to auto mode, this function works.
    GX_FLOAT_AUTO_GAIN_MAX            = 6013 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Setting up automatic gain range of maximum. When the gain is set to auto mode, this function works.
    GX_FLOAT_AUTO_EXPOSURE_TIME_MIN   = 6014 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Setting up automatic shutter range of minimum. When the shutter is set to auto mode, this function works.
    GX_FLOAT_AUTO_EXPOSURE_TIME_MAX   = 6015 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Setting up automatic shutter range of maximum. When the shutter is set to auto mode, this function works.
    GX_BUFFER_FRAME_INFORMATION       = 6016 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV, ///< FrameInformation
    GX_INT_CONTRAST_PARAM             = 6017 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Contrast parameter
    GX_FLOAT_GAMMA_PARAM              = 6018 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Gamma parameter
    GX_INT_COLOR_CORRECTION_PARAM     = 6019 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Color correction coefficient
    GX_ENUM_IMAGE_GRAY_RAISE_SWITCH   = 6020 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Control ImageGrayRaise is valid, see also GX_IMAGE_GRAY_RAISE_SWITCH_ENTRY
    GX_ENUM_AWB_LAMP_HOUSE            = 6021 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Refers to the AWB working environment, see also GX_AWB_LAMP_HOUSE_ENTRY
    GX_INT_AWBROI_OFFSETX             = 6022 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< This value sets the X offset (left offset) for the rect of interest in pixels for Auto WhiteBalance
    GX_INT_AWBROI_OFFSETY             = 6023 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< This value sets the Y offset (top offset) for the rect of interest for Auto WhiteBalance
    GX_INT_AWBROI_WIDTH               = 6024 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< This value sets the width of the rect of interest in pixels for Auto WhiteBalance
    GX_INT_AWBROI_HEIGHT              = 6025 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< This value sets the height of the rect of interest in pixels for Auto WhiteBalance
    GX_ENUM_SHARPNESS_MODE            = 6026 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Sharpening mode, see also GX_SHARPNESS_MODE_ENTRY
    GX_FLOAT_SHARPNESS                = 6027 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Sharpness
    GX_ENUM_USER_DATA_FILED_SELECTOR  = 6028 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< User selects the flash data field, refer to GX_USER_DATA_FILED_SELECTOR_ENTRY for area selection
    GX_BUFFER_USER_DATA_FILED_VALUE   = 6029 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV, ///< User data field content
    GX_ENUM_FLAT_FIELD_CORRECTION     = 6030 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Flat field correction switch, see also GX_FLAT_FIELD_CORRECTION_ENTRY
    GX_ENUM_NOISE_REDUCTION_MODE      = 6031 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Noise reduction mode, see also GX_NOISE_REDUCTION_MODE_ENTRY
    GX_FLOAT_NOISE_REDUCTION          = 6032 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Noise reduction
    GX_BUFFER_FFCLOAD                 = 6033 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Get flat field correction parameters
    GX_BUFFER_FFCSAVE                 = 6034 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Set flat field correction parameters
    GX_ENUM_STATIC_DEFECT_CORRECTION  = 6035 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Static defect correction, Refer to GX_ENUM_STATIC_DEFECT_CORRECTION_ENTRY
    GX_ENUM_2D_NOISE_REDUCTION_MODE   = 6036 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< 2D Noise Reduction Mode, Refer to GX_2D_NOISE_REDUCTION_MODE_ENTRY
    GX_ENUM_3D_NOISE_REDUCTION_MODE   = 6037 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< 3D Noise Reduction Mode, Refer to GX_3D_NOISE_REDUCTION_MODE_ENTRY
    GX_COMMAND_CLOSE_ISP              = 6038 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,///< Close ISP.
    GX_BUFFER_STATIC_DEFECT_CORRECTION_VALUE_ALL        = 6039 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< static defect correction value, Refer to GX_BUFFER_FFCSAVE
    GX_BUFFER_STATIC_DEFECT_CORRECTION_FLASH_VALUE      = 6040 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,	///< static defect correction flash value, Refer to GX_BUFFER_FFCSAVE
    GX_INT_STATIC_DEFECT_CORRECTION_FINISH              = 6041 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,      ///< static defect correction finish, Refer to GX_INT_AWBROI_HEIGHT
    GX_BUFFER_STATIC_DEFECT_CORRECTION_INFO             = 6042 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< static defect correction info, Refer to GX_BUFFER_FFCSAVE
	GX_COMMAND_STRIP_CALIBRATION_START					= 6043 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Starts the strip calibration
	GX_COMMAND_STRIP_CALIBRATION_STOP					= 6044 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Ready to stop the strip calibration
	GX_BUFFER_USER_DATA_FILED_VALUE_ALL					= 6045 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,	///< Continuous user area content 
	GX_ENUM_SHADING_CORRECTION_MODE						= 6046 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Shade correction mode
	GX_COMMAND_FFC_GENERATE								= 6047 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Generate flat field correction factor
	GX_ENUM_FFC_GENERATE_STATUS							= 6048 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Level-field correction status
	GX_ENUM_FFC_EXPECTED_GRAY_VALUE_ENABLE				= 6049 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Level-field correction expected gray value enable
	GX_INT_FFC_EXPECTED_GRAY							= 6050 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Flat-field correction expected gray value
	GX_INT_FFC_COEFFICIENTS_SIZE						= 6051 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Level-field correction factor size
	GX_BUFFER_FFC_VALUE_ALL								= 6052 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,	///< Level-field correction value
	GX_ENUM_DSNU_SELECTOR								= 6053 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Selection of dark field correction coefficient
	GX_COMMAND_DSNU_GENERATE							= 6054 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Generate dark field correction factor
	GX_ENUM_DSNU_GENERATE_STATUS						= 6055 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Dark field correction status
	GX_COMMAND_DSNU_SAVE								= 6056 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Save dark-field correction factor
	GX_COMMAND_DSNU_LOAD								= 6057 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Load dark-field correction factor
	GX_ENUM_PRNU_SELECTOR								= 6058 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Selection of bright field correction coefficient
	GX_COMMAND_PRNU_GENERATE							= 6059 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Generate bright field correction factor
	GX_ENUM_PRNU_GENERATE_STATUS						= 6060 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Bright-field correction status
	GX_COMMAND_PRNU_SAVE								= 6061 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Save the bright field correction factor
	GX_COMMAND_PRNU_LOAD								= 6062 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Loaded open field correction factor
	GX_BUFFER_DATA_FIELD_VALUE_ALL						= 6063 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,	///< Data field value
	GX_INT_STATIC_DEFECT_CORRECTION_CALIB_STATUS		= 6064 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Static bad point calibration status
	GX_INT_FFC_FACTORY_STATUS							= 6065 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Level-field correction status detection
	GX_INT_DSNU_FACTORY_STATUS							= 6066 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Detection of dark-field correction state
	GX_INT_PRNU_FACTORY_STATUS							= 6067 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Open field correction state detection
	GX_BUFFER_DETECT									= 6068 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,	///< Buffer detection（CXP）
	GX_ENUM_FFC_COEFFICIENT								= 6069 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Selection of flat field correction coefficient

    //---------------UserSetControl Section-------------------------
    GX_ENUM_USER_SET_SELECTOR         = 7000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Selects the feature User Set to load, save or configure.
    GX_COMMAND_USER_SET_LOAD          = 7001 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Loads the User Set specified by UserSetSelector to the device and makes it active.
    GX_COMMAND_USER_SET_SAVE          = 7002 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Save the User Set specified by UserSetSelector to the non-volatile memory of the device.
    GX_ENUM_USER_SET_DEFAULT          = 7003 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Selects the feature User Set to load and make active by default when the device is reset.
	GX_INT_DATA_FIELD_VALUE_ALL_USED_STATUS = 7004 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,	   ///< Factory status of user data area

    //---------------Event Section-------------------------
    GX_ENUM_EVENT_SELECTOR             = 8000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects which Event to signal to the host application.
    GX_ENUM_EVENT_NOTIFICATION         = 8001 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Activate or deactivate the notification to the host application of the occurrence of the selected Event.
    GX_INT_EVENT_EXPOSUREEND           = 8002 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Returns the unique identifier of the ExposureEnd type of Event.
    GX_INT_EVENT_EXPOSUREEND_TIMESTAMP = 8003 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Returns the Timestamp of the ExposureEnd Event.
    GX_INT_EVENT_EXPOSUREEND_FRAMEID   = 8004 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Returns the unique Identifier of the Frame (or image) that generated the ExposureEnd Event.
    GX_INT_EVENT_BLOCK_DISCARD         = 8005 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< This enumeration value indicates the BlockDiscard event ID.
    GX_INT_EVENT_BLOCK_DISCARD_TIMESTAMP = 8006 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Indicates the time stamp for the BlockDiscard event
    GX_INT_EVENT_OVERRUN                 = 8007 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< This enumeration value indicates the EventOverrun event ID.
    GX_INT_EVENT_OVERRUN_TIMESTAMP       = 8008 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Indicates the time stamp of the EventOverrun event
    GX_INT_EVENT_FRAMESTART_OVERTRIGGER  = 8009 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< This enumeration value indicates the FrameStartOverTrigger event ID.
    GX_INT_EVENT_FRAMESTART_OVERTRIGGER_TIMESTAMP = 8010 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Indicates the time stamp of the FrameStartOverTrigger event
    GX_INT_EVENT_BLOCK_NOT_EMPTY                  = 8011 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< This enumeration value indicates the BlockNotEmpty event.
    GX_INT_EVENT_BLOCK_NOT_EMPTY_TIMESTAMP        = 8012 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Indicates the time stamp of the BlockNotEmpty event
    GX_INT_EVENT_INTERNAL_ERROR                   = 8013 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< This enumeration value indicates the InternalError event.
    GX_INT_EVENT_INTERNAL_ERROR_TIMESTAMP         = 8014 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Indicates the time stamp of the InternalError event
    GX_INT_EVENT_FRAMEBURSTSTART_OVERTRIGGER      = 8015 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Frame burst start overtrigger event ID
    GX_INT_EVENT_FRAMEBURSTSTART_OVERTRIGGER_FRAMEID      = 8016 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Frame burst start overtrigger event frame ID
    GX_INT_EVENT_FRAMEBURSTSTART_OVERTRIGGER_TIMESTAMP    = 8017 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Frame burst start overtrigger event timestamp
    GX_INT_EVENT_FRAMESTART_WAIT                          = 8018 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Frame start wait event ID
    GX_INT_EVENT_FRAMESTART_WAIT_TIMESTAMP                = 8019 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Frame start wait event timestamp
    GX_INT_EVENT_FRAMEBURSTSTART_WAIT                     = 8020 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Frame burst start wait event ID
    GX_INT_EVENT_FRAMEBURSTSTART_WAIT_TIMESTAMP           = 8021 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Frame burst start wait event timestamp
    GX_INT_EVENT_BLOCK_DISCARD_FRAMEID                    = 8022 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Data block discard event frame ID
    GX_INT_EVENT_FRAMESTART_OVERTRIGGER_FRAMEID           = 8023 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Frame start wait overtrigger event frame ID
    GX_INT_EVENT_BLOCK_NOT_EMPTY_FRAMEID                  = 8024 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Data block not empty event frame ID
    GX_INT_EVENT_FRAMESTART_WAIT_FRAMEID                  = 8025 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Frame start wait event frame ID
    GX_INT_EVENT_FRAMEBURSTSTART_WAIT_FRAMEID             = 8026 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Frame burst start wait event frame ID
	GX_ENUM_EVENT_SIMPLE_MODE							  = 8027 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< event block ID enable,refer to GX_EVENT_SIMPLE_MODE_ENTRY

    //---------------LUT Section-------------------------
    GX_ENUM_LUT_SELECTOR             = 9000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Selects which LUT to control.
    GX_BUFFER_LUT_VALUEALL           = 9001 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Accesses all the LUT coefficients in a single access without using individual LUTIndex.
    GX_BOOL_LUT_ENABLE               = 9002 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Activates the selected LUT.
    GX_INT_LUT_INDEX                 = 9003 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Control the index (offset) of the coefficient to access in the selected LUT.
    GX_INT_LUT_VALUE                 = 9004 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Returns the Value at entry LUTIndex of the LUT selected by LUTSelector.
	GX_INT_LUT_FACTORY_STATUS		= 9005 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Lookup table factory status

    //---------------ChunkData Section-------------------------
    GX_BOOL_CHUNKMODE_ACTIVE         = 10001 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Activates the inclusion of Chunk data in the payload of the image.
    GX_ENUM_CHUNK_SELECTOR           = 10002 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Selects which Chunk to enable or control.
    GX_BOOL_CHUNK_ENABLE             = 10003 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Enables the inclusion of the selected Chunk data in the payload of the image.
    
    //---------------Color Transformation Control-------------------------
    GX_ENUM_COLOR_TRANSFORMATION_MODE       = 11000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Color conversion selection, see also GX_COLOR_TRANSFORMATION_MODE_ENTRY
    GX_BOOL_COLOR_TRANSFORMATION_ENABLE     = 11001 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Activates the selected Color Transformation module.
    GX_ENUM_COLOR_TRANSFORMATION_VALUE_SELECTOR = 11002 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Selects the Gain factor or Offset of the Transformation matrix to access in the selected Color Transformation module.
    GX_FLOAT_COLOR_TRANSFORMATION_VALUE     = 11003 | GX_FEATURE_FLOAT| GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Represents the value of the selected Gain factor or Offset inside the Transformation matrix.
    GX_ENUM_SATURATION_MODE                 = 11004 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Saturation mode, Refer to GX_ENUM_SATURATION_MODE_ENTRY
    GX_INT_SATURATION                       = 11005 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Saturation	

    //---------------CounterAndTimerControl Section-------------------------
    GX_ENUM_TIMER_SELECTOR                  = 12000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Selects which Counter to configure, Refer to GX_TIMER_SELECTOR_ENTRY
    GX_FLOAT_TIMER_DURATION                 = 12001 | GX_FEATURE_FLOAT| GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Sets the duration (in microseconds) of the Timer pulse.
    GX_FLOAT_TIMER_DELAY                    = 12002 | GX_FEATURE_FLOAT| GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Sets the duration (in microseconds) of the delay to apply at the reception of a trigger before starting the Timer.
    GX_ENUM_TIMER_TRIGGER_SOURCE            = 12003 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Selects the source of the trigger to start the Timer, Refer to GX_TIMER_TRIGGER_SOURCE_ENTRY
    GX_ENUM_COUNTER_SELECTOR                = 12004 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Selects which Counter to configure, Refer to GX_COUNTER_SELECTOR_ENTRY
    GX_ENUM_COUNTER_EVENT_SOURCE            = 12005 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Select the events that will be the source to increment the Counter, Refer to GX_COUNTER_EVENT_SOURCE_ENTRY
    GX_ENUM_COUNTER_RESET_SOURCE            = 12006 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Selects the signals that will be the source to reset the Counter, Refer to GX_COUNTER_RESET_SOURCE_ENTRY
    GX_ENUM_COUNTER_RESET_ACTIVATION        = 12007 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Selects the Activation mode of the Counter Reset Source signal, Refer to GX_COUNTER_RESET_ACTIVATION_ENTRY
    GX_COMMAND_COUNTER_RESET                = 12008 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Does a software reset of the selected Counter and starts it.
    GX_ENUM_COUNTER_TRIGGER_SOURCE          = 12009 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Counter trigger source, see also GX_COUNTER_TRIGGER_SOURCE_ENTRY
    GX_INT_COUNTER_DURATION                 = 12010 | GX_FEATURE_INT  | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Counter duration
    GX_ENUM_TIMER_TRIGGER_ACTIVATION		= 12011 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Timer Trigger Activation see also GX_TIMER_TRIGGER_ACTIVATION_ENTRY
	GX_INT_COUNTER_VALUE					= 12012 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,      ///< counter value

    //---------------RemoveParameterLimitControl Section-------------------------
    GX_ENUM_REMOVE_PARAMETER_LIMIT          = 13000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Remove paremeter range restriction, see also GX_REMOVE_PARAMETER_LIMIT_ENTRY
    
    //---------------HDRControl Section-------------------------
    GX_ENUM_HDR_MODE                        = 14000 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< HDR Mode, Refer to GX_HDR_MODE_ENTRY
    GX_INT_HDR_TARGET_LONG_VALUE            = 14001 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Set the target gray value of the long-exposure image.
    GX_INT_HDR_TARGET_SHORT_VALUE           = 14002 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Set the target gray value of the short-exposure image.
    GX_INT_HDR_TARGET_MAIN_VALUE            = 14003 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Set the target gray value of the HDR image.

    //---------------MultiGrayControl Section-------------------------
    GX_ENUM_MGC_MODE                        = 15001 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Multi-gray Control mode, Refer to GX_MGC_MODE_ENTRY
    GX_INT_MGC_SELECTOR                     = 15002 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< select Multi-gray Control.
    GX_FLOAT_MGC_EXPOSURE_TIME              = 15003 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Multi-gray Control ExposureTime.
    GX_FLOAT_MGC_GAIN                       = 15004 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Multi-gray Control Gain.
    
    //---------------ImageQualityControl Section-------------------------
    GX_BUFFER_STRIPED_CALIBRATION_INFO                     = 16001 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< striped calibration info, Refer to GX_BUFFER_STATIC_DEFECT_CORRECTION_INFO
    GX_FLOAT_CONTRAST                                      = 16002 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< contrast
    
    //---------------GyroControl Section-------------------------
    GX_BUFFER_IMU_DATA                                     = 17001 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,  ///< Gyro data
    GX_ENUM_IMU_CONFIG_ACC_RANGE                           = 17002 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Accelerometer measurement range，see also GX_IMU_CONFIG_ACC_RANGE_ENTRY
    GX_ENUM_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_SWITCH      = 17003 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Accelerometer low-pass filter switch, see also GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_SWITCH_ENTRY
    GX_ENUM_IMU_CONFIG_ACC_ODR                             = 17004 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Accelerometer output data rate，see also GX_IMU_CONFIG_ACC_ODR_ENTRY
    GX_ENUM_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_FREQUENCY   = 17005 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Accelerometer Accelerometer Low Pass Cutoff Frequency，see also GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_FREQUENCY_ENTRY
    GX_ENUM_IMU_CONFIG_GYRO_XRANGE                         = 17006 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Measuring range of gyroscope in X direction，see also GX_IMU_CONFIG_GYRO_RANGE_ENTRY
    GX_ENUM_IMU_CONFIG_GYRO_YRANGE                         = 17007 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Measuring range of gyroscope in Y direction，see also GX_IMU_CONFIG_GYRO_RANGE_ENTRY
    GX_ENUM_IMU_CONFIG_GYRO_ZRANGE                         = 17008 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Measuring range of gyroscope in Z direction，see also GX_IMU_CONFIG_GYRO_RANGE_ENTRY
    GX_ENUM_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_SWITCH     = 17009 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Gyroscope low-pass filter switch，see also GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_SWITCH_ENTRY
    GX_ENUM_IMU_CONFIG_GYRO_ODR                            = 17010 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Gyroscope output data rate，see also GX_IMU_CONFIG_GYRO_ODR_ENTRY
    GX_ENUM_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY  = 17011 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Accelerometer Low Pass Cutoff Frequency，see also GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_ENTRY
    GX_FLOAT_IMU_ROOM_TEMPERATURE                          = 17012 | GX_FEATURE_FLOAT | GX_FEATURE_LEVEL_REMOTE_DEV,   ///< Gyroscope temperature room temperature setting
    GX_ENUM_IMU_TEMPERATURE_ODR                            = 17013 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Thermometer output data rate setting，see also GX_IMU_TEMPERATURE_ODR_ENTRY
    
   	//---------------FrameBufferControl Section-------------------------
	GX_INT_FRAME_BUFFER_COUNT         = 18001 | GX_FEATURE_INT     | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Frame storage depth
	GX_COMMAND_FRAME_BUFFER_FLUSH     = 18002 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Clear frame memory  

	//----------------SerialPortControl Section----------------------------------
	GX_ENUM_SERIALPORT_SELECTOR						= 19001 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,				///< Serial port selection
	GX_ENUM_SERIALPORT_SOURCE						= 19002 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,				///< Serial port input source
	GX_ENUM_SERIALPORT_BAUDRATE						= 19003 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,				///< Serial baud rate
	GX_INT_SERIALPORT_DATA_BITS						= 19004 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,					///< Serial port data bit
	GX_ENUM_SERIALPORT_STOP_BITS					= 19005 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,				///< Serial port stop bit
	GX_ENUM_SERIALPORT_PARITY						= 19006 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,				///< Serial port parity
	GX_INT_TRANSMIT_QUEUE_MAX_CHARACTER_COUNT		= 19007 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,					///< Maximum number of characters in transmission queue
	GX_INT_TRANSMIT_QUEUE_CURRENT_CHARACTER_COUNT	= 19008 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,					///< Current number of characters in the transmission queue
	GX_INT_RECEIVE_QUEUE_MAX_CHARACTER_COUNT		= 19009 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,					///< Maximum number of characters in receive queue
	GX_INT_RECEIVE_QUEUE_CURRENT_CHARACTER_COUNT	= 19010 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,					///< Current number of characters in the receive queue
	GX_INT_RECEIVE_FRAMING_ERROR_COUNT				= 19011 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,				///< Received frame error count
	GX_INT_RECEIVE_PARITY_ERROR_COUNT				= 19012 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,				///< Receive parity error count
	GX_COMMAND_RECEIVE_QUEUE_CLEAR					= 19013 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV,			///< Queue Clear
	GX_BUFFER_SERIALPORT_DATA						= 19014 | GX_FEATURE_BUFFER | GX_FEATURE_LEVEL_REMOTE_DEV,			///< serial data
	GX_INT_SERIALPORT_DATA_LENGTH					= 19015 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,				///< Serial port data length 
	GX_INT_SERIAL_PORT_DETECTION_STATUS				= 19016 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,				///< Serial port status detection	

	//---------------CoaXPress Section-------------------------
	GX_ENUM_CXP_LINK_CONFIGURATION					= 20001 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Connection configuration
	GX_ENUM_CXP_LINK_CONFIGURATION_PREFERRED		= 20002 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Preset connection configuration
	GX_ENUM_CXP_LINK_CONFIGURATION_STATUS			= 20003 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< CXP connection configuration status
	GX_INT_IMAGE1_STREAM_ID							= 20004 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< First image flow ID
	GX_ENUM_CXP_CONNECTION_SELECTOR					= 20005 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Connection selection
	GX_ENUM_CXP_CONNECTION_TEST_MODE				= 20006 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Connection test mode
	GX_INT_CXP_CONNECTION_TEST_ERROR_COUNT			= 20007 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Connection test error count
	GX_INT_CXP_CONNECTION_TEST_PACKET_RX_COUNT		= 20008 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,     ///< Number of connection test packets received
	GX_INT_CXP_CONNECTION_TEST_PACKET_TX_COUNT		= 20009 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Number of connection test packets sent

	//--------------SequencerControl Section-------------------------
	GX_ENUM_SEQUENCER_MODE							= 21001 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sequencer mode
	GX_ENUM_SEQUENCER_CONFIGURATION_MODE			= 21002 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sequencer configuration mode
	GX_ENUM_SEQUENCER_FEATURE_SELECTOR				= 21003 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sequencer function selector
	GX_BOOL_SEQUENCER_FEATURE_ENABLE				= 21004 | GX_FEATURE_BOOL | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sequencer function enabled
	GX_INT_SEQUENCER_SET_SELECTOR					= 21005 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sequencer setting selector
	GX_INT_SEQUENCER_SET_COUNT						= 21006 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sequencer count
	GX_INT_SEQUENCER_SET_ACTIVE						= 21007 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Sequencer settings active
	GX_COMMAND_SEQUENCER_SET_RESET					= 21008 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Sequencer setting reset
	GX_INT_SEQUENCER_PATH_SELECTOR					= 21009 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Sequencer payh selection
	GX_INT_SEQUENCER_SET_NEXT						= 21010 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,		///< Sequencer Next
	GX_ENUM_SEQUENCER_TRIGGER_SOURCE				= 21011 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,	///< Sequencer Trigger
	GX_COMMAND_SEQUENCER_SET_SAVE					= 21012 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Sequencer Save
	GX_COMMAND_SEQUENCER_SET_LOAD					= 21013 | GX_FEATURE_COMMAND | GX_FEATURE_LEVEL_REMOTE_DEV, ///< Sequencer Load

	//--------------EnoderControl Section-------------------------
	GX_ENUM_ENCODER_SELECTOR						= 22001 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Encoder selector
	GX_ENUM_ENCODER_DIRECTION						= 22002 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Encoder direction
	GX_INT_ENCODER_VALUE							= 22003 | GX_FEATURE_INT | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Decoder value
	GX_ENUM_ENCODER_SOURCEA							= 22004 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,	///< Encoder phase A input
	GX_ENUM_ENCODER_SOURCEB							= 22005 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,	///< Encoder phase B input
	GX_ENUM_ENCODER_MODE							= 22006 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_REMOTE_DEV,    ///< Encoder Mode
      
    //////////////////////////////////////////////////////////////////////////
    /// Local device layer(Device Feature)
    //////////////////////////////////////////////////////////////////////////
    GX_DEV_INT_COMMAND_TIMEOUT     = 0 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DEV, ///< Indicates the current command timeout of the specific Link.
    GX_DEV_INT_COMMAND_RETRY_COUNT = 1 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DEV, ///< Command retry times

    //////////////////////////////////////////////////////////////////////////
    /// Flow layer(DataStream Feature)
    //////////////////////////////////////////////////////////////////////////
    GX_DS_INT_ANNOUNCED_BUFFER_COUNT          = 0 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Number of Buffers declared
    GX_DS_INT_DELIVERED_FRAME_COUNT           = 1 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Number of received frames (including residual frames)
    GX_DS_INT_LOST_FRAME_COUNT                = 2 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Number of frames lost due to insufficient buffers
    GX_DS_INT_INCOMPLETE_FRAME_COUNT          = 3 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Number of residual frames received
    GX_DS_INT_DELIVERED_PACKET_COUNT          = 4 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Number of packets received
    GX_DS_INT_RESEND_PACKET_COUNT             = 5 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Number of retransmission packets
    GX_DS_INT_RESCUED_PACKED_COUNT            = 6 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Number of successful retransmitted packets
    GX_DS_INT_RESEND_COMMAND_COUNT            = 7 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Repeat command times
    GX_DS_INT_UNEXPECTED_PACKED_COUNT         = 8 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Exception packet number
    GX_DS_INT_MAX_PACKET_COUNT_IN_ONE_BLOCK   = 9 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,   ///< Maximum number of retransmissions of data blocks
    GX_DS_INT_MAX_PACKET_COUNT_IN_ONE_COMMAND = 10 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< Maximum number of packets contained in a retransmit command
    GX_DS_INT_RESEND_TIMEOUT                  = 11 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< Retransmission timeout time
    GX_DS_INT_MAX_WAIT_PACKET_COUNT           = 12 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< Maximum waiting packet number
    GX_DS_ENUM_RESEND_MODE                    = 13 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_DS, ///< Retransmission, see also GX_DS_RESEND_MODE_ENTRY
    GX_DS_INT_MISSING_BLOCKID_COUNT           = 14 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< Missing number of BlockID
    GX_DS_INT_BLOCK_TIMEOUT                   = 15 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< Data block timeout
    GX_DS_INT_STREAM_TRANSFER_SIZE            = 16 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< size of transfer block
    GX_DS_INT_STREAM_TRANSFER_NUMBER_URB      = 17 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< Number of data blocks transmitted
    GX_DS_INT_MAX_NUM_QUEUE_BUFFER            = 18 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< Maximum Buffer Number of Collection Queues
    GX_DS_INT_PACKET_TIMEOUT                  = 19 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< time of package timeout
    GX_DS_INT_SOCKET_BUFFER_SIZE              = 20 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< Socket buffer size in kilobytes
    GX_DS_ENUM_STOP_ACQUISITION_MODE          = 21 | GX_FEATURE_ENUM | GX_FEATURE_LEVEL_DS, ///< stop acquisition mode
    GX_DS_ENUM_STREAM_BUFFER_HANDLING_MODE    = 22 | GX_FEATURE_ENUM| GX_FEATURE_LEVEL_DS,  ///< Buffer handling mode, see also GX_DS_STREAM_BUFFER_HANDLING_MODE_ENTRY
    GX_DS_INT_ACQUISITION_BUFFER_CACHE_PREC   = 23 | GX_FEATURE_INT | GX_FEATURE_LEVEL_DS,  ///< Acquisition Buffer Cache Num Prec
}GX_FEATURE_ID;
typedef int32_t GX_FEATURE_ID_CMD;

//------------------------------------------------------------------------------
//  Handle Definition
//------------------------------------------------------------------------------
typedef void* GX_DEV_HANDLE;               ///< Device handle. It can be obtained through the GXOpenDevice interface and can be used to achieve control and acquisition
typedef void* GX_EVENT_CALLBACK_HANDLE;    ///< Device callback handle. It can be used to register callback functions for related events, such as a device offline callback function
typedef void* GX_FEATURE_CALLBACK_HANDLE;  ///< Device attributes update callback handle. It can be used to register device attribute and update callback function

//------------------------------------------------------------------------------------
typedef enum GX_PIXEL_SIZE_ENTRY
{
    GX_PIXEL_SIZE_BPP8  = 8,
    GX_PIXEL_SIZE_BPP10 = 10,
    GX_PIXEL_SIZE_BPP12 = 12,
    GX_PIXEL_SIZE_BPP16 = 16,
    GX_PIXEL_SIZE_BPP24 = 24,
    GX_PIXEL_SIZE_BPP30 = 30,
    GX_PIXEL_SIZE_BPP32 = 32,
    GX_PIXEL_SIZE_BPP36 = 36,
    GX_PIXEL_SIZE_BPP48 = 48,
    GX_PIXEL_SIZE_BPP64 = 64,
}GX_PIXEL_SIZE_ENTRY;

typedef enum GX_PIXEL_COLOR_FILTER_ENTRY
{
    GX_COLOR_FILTER_NONE     = 0,    ///< None
    GX_COLOR_FILTER_BAYER_RG = 1,    ///< RG format
    GX_COLOR_FILTER_BAYER_GB = 2,    ///< GB format
    GX_COLOR_FILTER_BAYER_GR = 3,    ///< GR format
    GX_COLOR_FILTER_BAYER_BG = 4,    ///< BG format
}GX_PIXEL_COLOR_FILTER_ENTRY;

#define GX_PIXEL_MONO                  ( 0x01000000 )
#define GX_PIXEL_COLOR                 ( 0x02000000 )

#define GX_PIXEL_8BIT                  ( 0x00080000 )
#define GX_PIXEL_10BIT                 ( 0x000A0000 )
#define GX_PIXEL_12BIT                 ( 0x000C0000 )
#define GX_PIXEL_16BIT                 ( 0x00100000 )
#define GX_PIXEL_24BIT                 ( 0x00180000 )
#define GX_PIXEL_30BIT                 ( 0x001E0000 )
#define GX_PIXEL_32BIT                 ( 0x00200000 )
#define GX_PIXEL_36BIT                 ( 0x00240000 )
#define GX_PIXEL_48BIT                 ( 0x00300000 )
#define GX_PIXEL_64BIT                 ( 0x00400000 )

typedef enum GX_PIXEL_FORMAT_ENTRY
{
    GX_PIXEL_FORMAT_UNDEFINED          = (0),
    GX_PIXEL_FORMAT_MONO8              = (GX_PIXEL_MONO  | GX_PIXEL_8BIT  | 0x0001),//0x1080001,
    GX_PIXEL_FORMAT_MONO8_SIGNED       = (GX_PIXEL_MONO  | GX_PIXEL_8BIT  | 0x0002),//0x1080002,
    GX_PIXEL_FORMAT_MONO10             = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0003),//0x1100003,
    GX_PIXEL_FORMAT_MONO12             = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0005),//0x1100005,
    GX_PIXEL_FORMAT_MONO14             = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0025),//0x1100025,
    GX_PIXEL_FORMAT_MONO16             = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0007),//0x1100007,
    GX_PIXEL_FORMAT_BAYER_GR8          = (GX_PIXEL_MONO  | GX_PIXEL_8BIT  | 0x0008),//0x1080008,               
    GX_PIXEL_FORMAT_BAYER_RG8          = (GX_PIXEL_MONO  | GX_PIXEL_8BIT  | 0x0009),//0x1080009,                
    GX_PIXEL_FORMAT_BAYER_GB8          = (GX_PIXEL_MONO  | GX_PIXEL_8BIT  | 0x000A),//0x108000A,
    GX_PIXEL_FORMAT_BAYER_BG8          = (GX_PIXEL_MONO  | GX_PIXEL_8BIT  | 0x000B),//0x108000B,
    GX_PIXEL_FORMAT_BAYER_GR10         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x000C),//0x110000C,                
    GX_PIXEL_FORMAT_BAYER_RG10         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x000D),//0x110000D,
    GX_PIXEL_FORMAT_BAYER_GB10         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x000E),//0x110000E,
    GX_PIXEL_FORMAT_BAYER_BG10         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x000F),//0x110000F,
    GX_PIXEL_FORMAT_BAYER_GR12         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0010),//0x1100010,              
    GX_PIXEL_FORMAT_BAYER_RG12         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0011),//0x1100011,
    GX_PIXEL_FORMAT_BAYER_GB12         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0012),//0x1100012,
    GX_PIXEL_FORMAT_BAYER_BG12         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0013),//0x1100013,
    GX_PIXEL_FORMAT_BAYER_GR16         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x002E),//0x110002E,                
    GX_PIXEL_FORMAT_BAYER_RG16         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x002F),//0x110002F,
    GX_PIXEL_FORMAT_BAYER_GB16         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0030),//0x1100030,
    GX_PIXEL_FORMAT_BAYER_BG16         = (GX_PIXEL_MONO  | GX_PIXEL_16BIT | 0x0031),//0x1100031,
    GX_PIXEL_FORMAT_RGB8_PLANAR        = (GX_PIXEL_COLOR | GX_PIXEL_24BIT | 0x0021),//0x2180021,
    GX_PIXEL_FORMAT_RGB10_PLANAR       = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x0022),//0x2300022,
    GX_PIXEL_FORMAT_RGB12_PLANAR       = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x0023),//0x2300023,
    GX_PIXEL_FORMAT_RGB16_PLANAR       = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x0024),//0x2300024,

    GX_PIXEL_FORMAT_RGB8               = (GX_PIXEL_COLOR | GX_PIXEL_24BIT | 0x0014),//0x2180014,
    GX_PIXEL_FORMAT_RGB10              = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x0018),//0x2300018,
    GX_PIXEL_FORMAT_RGB12              = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x001A),//0x230001A,
    GX_PIXEL_FORMAT_RGB14              = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x005E),//0x230005E,
    GX_PIXEL_FORMAT_RGB16              = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x0033),//0x2300033,

    GX_PIXEL_FORMAT_BGR8               = (GX_PIXEL_COLOR | GX_PIXEL_24BIT | 0x0015),//0x2180015,
    GX_PIXEL_FORMAT_BGR10              = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x0019),//0x2300019,
    GX_PIXEL_FORMAT_BGR12              = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x001B),//0x230001B,
    GX_PIXEL_FORMAT_BGR14              = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x004A),//0x230004A,
    GX_PIXEL_FORMAT_BGR16              = (GX_PIXEL_COLOR | GX_PIXEL_48BIT | 0x004B),//0x230004B,

    GX_PIXEL_FORMAT_RGBA8              = (GX_PIXEL_COLOR | GX_PIXEL_32BIT | 0x0016),//0x2200016,
    GX_PIXEL_FORMAT_BGRA8              = (GX_PIXEL_COLOR | GX_PIXEL_32BIT | 0x0017),//0x2200017,
    GX_PIXEL_FORMAT_ARGB8              = (GX_PIXEL_COLOR | GX_PIXEL_32BIT | 0x0018),//0x2200018,
    GX_PIXEL_FORMAT_ABGR8              = (GX_PIXEL_COLOR | GX_PIXEL_32BIT | 0x0019),//0x2200019,

    GX_PIXEL_FORMAT_YUV444_8           = (GX_PIXEL_COLOR | GX_PIXEL_24BIT | 0x0020),//0x2180020,
    GX_PIXEL_FORMAT_YUV422_8           = (GX_PIXEL_COLOR | GX_PIXEL_16BIT | 0x0032),//0x2100032,
    GX_PIXEL_FORMAT_YUV411_8           = (GX_PIXEL_COLOR | GX_PIXEL_12BIT | 0x001E),//0x20C001E,
    GX_PIXEL_FORMAT_YUV420_8_PLANAR    = (GX_PIXEL_COLOR | GX_PIXEL_12BIT | 0x0040),//0x20C0040,

    GX_PIXEL_FORMAT_YCBCR444_8         = (GX_PIXEL_COLOR | GX_PIXEL_24BIT | 0x005B),//0x218005B,
    GX_PIXEL_FORMAT_YCBCR422_8         = (GX_PIXEL_COLOR | GX_PIXEL_16BIT | 0x003B),//0x210003B,
    GX_PIXEL_FORMAT_YCBCR411_8         = (GX_PIXEL_COLOR | GX_PIXEL_12BIT | 0x005A),//0x20C005A,

    GX_PIXEL_FORMAT_YCBCR601_444_8     = (GX_PIXEL_COLOR | GX_PIXEL_24BIT | 0x003D),//0x218003D,
    GX_PIXEL_FORMAT_YCBCR601_422_8     = (GX_PIXEL_COLOR | GX_PIXEL_16BIT | 0x003E),//0x210003E,
    GX_PIXEL_FORMAT_YCBCR601_411_8     = (GX_PIXEL_COLOR | GX_PIXEL_12BIT | 0x003F),//0x20C003F,

    GX_PIXEL_FORMAT_YCBCR709_444_8     = (GX_PIXEL_COLOR | GX_PIXEL_24BIT | 0x0040),//0x2180040,
    GX_PIXEL_FORMAT_YCBCR709_422_8     = (GX_PIXEL_COLOR | GX_PIXEL_16BIT | 0x0041),//0x2100041,
    GX_PIXEL_FORMAT_YCBCR709_411_8     = (GX_PIXEL_COLOR | GX_PIXEL_12BIT | 0x0042),//0x20C0042,
    
	GX_PIXEL_FORMAT_MONO10_PACKED      = (GX_PIXEL_MONO  | GX_PIXEL_12BIT | 0x0004),//0x010C0004,GigE Vision specific format
	GX_PIXEL_FORMAT_MONO12_PACKED      = (GX_PIXEL_MONO  | GX_PIXEL_12BIT | 0x0006),//0x010C0006,GigE Vision specific format
}GX_PIXEL_FORMAT_ENTRY;

typedef enum GX_SENSOR_BIT_DEPTH_ENTRY
{
	GX_SENSOR_BIT_DEPTH_BPP8						= 8,
	GX_SENSOR_BIT_DEPTH_BPP10						= 10,
	GX_SENSOR_BIT_DEPTH_BPP12						= 12,
}GX_SENSOR_BIT_DEPTH_ENTRY;

typedef enum GX_ACQUISITION_MODE_ENTRY
{
    GX_ACQ_MODE_SINGLE_FRAME                       = 0,           ///< Single frame mode
    GX_ACQ_MODE_MULITI_FRAME                       = 1,           ///< Multi frame mode
    GX_ACQ_MODE_CONTINUOUS                         = 2,           ///< Continuous mode
}GX_ACQUISITION_MODE_ENTRY;

typedef enum GX_TRIGGER_MODE_ENTRY
{
    GX_TRIGGER_MODE_OFF                            = 0,           ///< Switch off the trigger mode
    GX_TRIGGER_MODE_ON                             = 1,           ///< Switch on the trigger mode
}GX_TRIGGER_MODE_ENTRY;

typedef enum GX_OVERLAP_MODE_ENTRY
{
	GX_OVERLAP_MODE_OFF								= 0,          ///< Switch off the overlap mode
	GX_OVERLAP_MODE_ON								= 1,          ///< Switch on the overlap mode
}GX_OVERLAP_MODE_ENTRY;

typedef enum GX_TRIGGER_SOURCE_ENTRY
{
    GX_TRIGGER_SOURCE_SOFTWARE                     = 0,           ///< Software trigger
    GX_TRIGGER_SOURCE_LINE0                        = 1,           ///< Trigger source 0
    GX_TRIGGER_SOURCE_LINE1                        = 2,           ///< Trigger source 1
    GX_TRIGGER_SOURCE_LINE2                        = 3,           ///< Trigger source 2
    GX_TRIGGER_SOURCE_LINE3                        = 4,           ///< Trigger source 3
    GX_TRIGGER_SOURCE_COUNTER2END                  = 5,           ///< Counter 2 end trigger
    GX_TRIGGER_SOURCE_TRIGGER                      = 6,           ///< Trigger signal
	GX_TRIGGER_SOURCE_MULTISOURCE					= 7,          ///< MultiSource
	GX_TRIGGER_SOURCE_CXPTRIGGER0					= 8,          ///< CXPTrigger0
	GX_TRIGGER_SOURCE_CXPTRIGGER1					= 9,          ///< CXPTrigger1
}GX_TRIGGER_SOURCE_ENTRY;

typedef enum GX_MULTISOURCE_SELECTOR_ENTRY
{
	GX_MULTISOURCE_SELECTOR_SOFTWARE				= 0,			///< Software MultiSource
	GX_MULTISOURCE_SELECTOR_LINE0					= 1,			///< MultiSource source 0
	GX_MULTISOURCE_SELECTOR_LINE2					= 3,			///< MultiSource source 2
	GX_MULTISOURCE_SELECTOR_LINE3					= 4,			///< MultiSource source 3
}GX_MULTISOURCE_SELECTOR_ENTRY;

typedef enum GX_TRIGGER_ACTIVATION_ENTRY
{
    GX_TRIGGER_ACTIVATION_FALLINGEDGE              = 0,           ///< Falling edge trigger
    GX_TRIGGER_ACTIVATION_RISINGEDGE               = 1,           ///< Rising edge trigger
    GX_TRIGGER_ACTIVATION_ANYEDGE     			   = 2,           ///< Trigger activation anyedge
    GX_TRIGGER_ACTIVATION_LEVELHIGH   			   = 3,           ///< Trigger activation level high
    GX_TRIGGER_ACTIVATION_LEVELLOW    			   = 4,           ///< Trigger activation level low
}GX_TRIGGER_ACTIVATION_ENTRY;

typedef enum GX_TRIGGER_SWITCH_ENTRY
{
    GX_TRIGGER_SWITCH_OFF                          = 0,           ///< Switch off the external trigger
    GX_TRIGGER_SWITCH_ON                           = 1,           ///< Switch on the external trigger
}GX_TRIGGER_SWITCH_ENTRY;

typedef enum GX_EXPOSURE_MODE_ENTRY
{
    GX_EXPOSURE_MODE_TIMED                         = 1,           ///< Control exposure time through exposure time register
    GX_EXPOSURE_MODE_TRIGGERWIDTH                  = 2,           ///< Control exposure time through trigger signal width
}GX_EXPOSURE_MODE_ENTRY;

typedef enum GX_EXPOSURE_AUTO_ENTRY
{
    GX_EXPOSURE_AUTO_OFF                           = 0,           ///< Switch off automatic exposure
    GX_EXPOSURE_AUTO_CONTINUOUS                    = 1,           ///< Continuous automatic exposure
    GX_EXPOSURE_AUTO_ONCE                          = 2,           ///< Single automatic exposure
}GX_EXPOSURE_AUTO_ENTRY;

typedef enum GX_USER_OUTPUT_SELECTOR_ENTRY
{
    GX_USER_OUTPUT_SELECTOR_OUTPUT0                = 1,           ///< Output 0
    GX_USER_OUTPUT_SELECTOR_OUTPUT1                = 2,           ///< Output 1
    GX_USER_OUTPUT_SELECTOR_OUTPUT2                = 4,           ///< Output 2
    GX_USER_OUTPUT_SELECTOR_OUTPUT3 			   = 5,           ///< Output 3
    GX_USER_OUTPUT_SELECTOR_OUTPUT4 			   = 6,           ///< Output 4
    GX_USER_OUTPUT_SELECTOR_OUTPUT5 			   = 7,           ///< Output 5
    GX_USER_OUTPUT_SELECTOR_OUTPUT6 			   = 8,           ///< Output 6
}GX_USER_OUTPUT_SELECTOR_ENTRY;

typedef enum GX_USER_OUTPUT_MODE_ENTRY
{
    GX_USER_OUTPUT_MODE_STROBE                     = 0,           ///< Strobe light
    GX_USER_OUTPUT_MODE_USERDEFINED                = 1,           ///< User defined
}GX_USER_OUTPUT_MODE_ENTRY;

typedef enum GX_STROBE_SWITCH_ENTRY
{
    GX_STROBE_SWITCH_OFF                           = 0,           ///< Switch off the strobe light
    GX_STROBE_SWITCH_ON                            = 1,           ///< Switch on the strobe light
}GX_STROBE_SWITCH_ENTRY;

typedef enum GX_GAIN_AUTO_ENTRY
{
    GX_GAIN_AUTO_OFF                               = 0,           ///< Switch off automatic gain
    GX_GAIN_AUTO_CONTINUOUS                        = 1,           ///< Continuous automatic gain
    GX_GAIN_AUTO_ONCE                              = 2,           ///< Single automatic gain
}GX_GAIN_AUTO_ENTRY;

typedef enum GX_GAIN_SELECTOR_ENTRY
{
    GX_GAIN_SELECTOR_ALL                           = 0,           ///< All gain channels 
    GX_GAIN_SELECTOR_RED                           = 1,           ///< Red channel gain
    GX_GAIN_SELECTOR_GREEN                         = 2,           ///< Green channel gain
    GX_GAIN_SELECTOR_BLUE                          = 3,           ///< Blue channel gain
	GX_GAIN_SELECTOR_DIGITAL_ALL					= 4,          ///< Digital gain
}GX_GAIN_SELECTOR_ENTRY;

typedef enum GX_BLACKLEVEL_AUTO_ENTRY
{
    GX_BLACKLEVEL_AUTO_OFF                         = 0,           ///< Switch off automatic black level
    GX_BLACKLEVEL_AUTO_CONTINUOUS                  = 1,           ///< Continuous automatic black level
    GX_BLACKLEVEL_AUTO_ONCE                        = 2,           ///< Single automatic black level
}GX_BLACKLEVEL_AUTO_ENTRY;

typedef enum GX_BLACKLEVEL_SELECTOR_ENTRY
{
    GX_BLACKLEVEL_SELECTOR_ALL                     = 0,           ///< All black level channels
    GX_BLACKLEVEL_SELECTOR_RED                     = 1,           ///< Red channel black level
    GX_BLACKLEVEL_SELECTOR_GREEN                   = 2,           ///< Green channel black level
    GX_BLACKLEVEL_SELECTOR_BLUE                    = 3,           ///< Blue channel black level
}GX_BLACKLEVEL_SELECTOR_ENTRY;

typedef enum GX_BALANCE_WHITE_AUTO_ENTRY
{
    GX_BALANCE_WHITE_AUTO_OFF                      = 0,           ///< Switch off automatic white balance
    GX_BALANCE_WHITE_AUTO_CONTINUOUS               = 1,           ///< Continuous automatic white balance
    GX_BALANCE_WHITE_AUTO_ONCE                     = 2,           ///< Single automatic white balance
}GX_BALANCE_WHITE_AUTO_ENTRY;

typedef enum GX_BALANCE_RATIO_SELECTOR_ENTRY
{
    GX_BALANCE_RATIO_SELECTOR_RED                  = 0,           ///< Red channel
    GX_BALANCE_RATIO_SELECTOR_GREEN                = 1,           ///< Green channel
    GX_BALANCE_RATIO_SELECTOR_BLUE                 = 2,           ///< Blue channel
}GX_BALANCE_RATIO_SELECTOR_ENTRY;

typedef enum GX_COLOR_CORRECT_ENTRY
{
    GX_COLOR_CORRECT_OFF                           = 0,           ///< Switch off automatic color correction
    GX_COLOR_CORRECT_ON                            = 1,           ///< Switch on automatic color correction
}GX_COLOR_CORRECT_ENTRY;

typedef enum GX_DEAD_PIXEL_CORRECT_ENTRY
{
    GX_DEAD_PIXEL_CORRECT_OFF                      = 0,           ///< Switch off automatic bad point correction
    GX_DEAD_PIXEL_CORRECT_ON                       = 1,           ///< Switch on automatic bad point correction
}GX_DEAD_PIXEL_CORRECT_ENTRY;

typedef enum GX_AA_LIGHT_ENVIRMENT_ENTRY
{
    GX_AA_LIGHT_ENVIRMENT_NATURELIGHT              = 0,           ///< Natural light
    GX_AA_LIGHT_ENVIRMENT_AC50HZ                   = 1,           ///< 50 Hz fluorescent light
    GX_AA_LIGHT_ENVIRMENT_AC60HZ                   = 2,           ///< 60 Hz fluorescent light
}GX_AA_LIGHT_ENVIRMENT_ENTRY;

typedef enum GX_DEVICE_TAP_GEOMETRY_ENTRY
{
	GX_DEVICE_TAP_GEOMETRY_GEOMETRY_1X_1Y			= 0,           ///<Geometry_1X_1Y
	GX_DEVICE_TAP_GEOMETRY_GEOMETRY_1X_1Y2			= 9,           ///<Geometry_1X_1Y2
	GX_DEVICE_TAP_GEOMETRY_GEOMETRY_1X_2YE			= 10,           ///<Geometry_1X_2YE
}GX_DEVICE_TAP_GEOMETRY_ENTRY;

typedef enum GX_USER_SET_SELECTOR_ENTRY
{
    GX_ENUM_USER_SET_SELECTOR_DEFAULT              = 0,           ///< Default parameter group
    GX_ENUM_USER_SET_SELECTOR_USERSET0             = 1,           ///< User parameter group 0
    GX_ENUM_USER_SET_SELECTOR_USERSET1 			   = 2,           ///< User parameter group 1
}GX_USER_SET_SELECTOR_ENTRY;

typedef enum GX_IMAGE_GRAY_RAISE_SWITCH_ENTRY
{
    GX_IMAGE_GRAY_RAISE_SWITCH_OFF                 = 0,           ///< Switch off the image brightness stretch
    GX_IMAGE_GRAY_RAISE_SWITCH_ON                  = 1,           ///< Switch on the image brightness stretch
}GX_IMAGE_GRAY_RAISE_SWITCH_ENTRY;

typedef enum GX_FIXED_PATTERN_NOISE_CORRECT_MODE
{
    GX_FIXEDPATTERNNOISECORRECT_OFF                = 0,           ///< Switch off the pattern noise correction
    GX_FIXEDPATTERNNOISECORRECT_ON                 = 1,           ///< Switch on the pattern noise correction

}GX_FIXED_PATTERN_NOISE_CORRECT_MODE;

typedef enum GX_AWB_LAMP_HOUSE_ENTRY
{
    GX_AWB_LAMP_HOUSE_ADAPTIVE                     = 0,           ///< Adaptive light source
    GX_AWB_LAMP_HOUSE_D65                          = 1,           ///< Color temperature 6500k
    GX_AWB_LAMP_HOUSE_FLUORESCENCE                 = 2,           ///< Fluorescent
    GX_AWB_LAMP_HOUSE_INCANDESCENT                 = 3,           ///< Incandescent
    GX_AWB_LAMP_HOUSE_D75                          = 4,           ///< Color temperature 7500k
    GX_AWB_LAMP_HOUSE_D50                          = 5,           ///< Color temperature 5000k
    GX_AWB_LAMP_HOUSE_U30                          = 6,           ///< Color temperature 3000k
}GX_AWB_LAMP_HOUSE_ENTRY;

typedef enum GX_TEST_PATTERN_ENTRY
{
    GX_ENUM_TEST_PATTERN_OFF                       = 0,           ///< Off
    GX_ENUM_TEST_PATTERN_GRAY_FRAME_RAMP_MOVING    = 1,           ///< Static grey increment
    GX_ENUM_TEST_PATTERN_SLANT_LINE_MOVING         = 2,           ///< Rolling slant line
    GX_ENUM_TEST_PATTERN_VERTICAL_LINE_MOVING      = 3,           ///< Rolling vertical line
    GX_ENUM_TEST_PATTERN_HORIZONTAL_LINE_MOVING    = 4,           ///< Rolling horizontal line
    GX_ENUM_TEST_PATTERN_GREY_VERTICAL_RAMP        = 5,           ///< Vertical grey increment
    GX_ENUM_TEST_PATTERN_SLANT_LINE                = 6,           ///< Static slant line
}GX_TEST_PATTERN_ENTRY;

typedef enum GX_TRIGGER_SELECTOR_ENTRY
{
    GX_ENUM_TRIGGER_SELECTOR_FRAME_START           = 1,           ///< Capture single frame
    GX_ENUM_TRIGGER_SELECTOR_FRAME_BURST_START     = 2,           ///< High speed continuous shooting
} GX_TRIGGER_SELECTOR_ENTRY;

typedef enum GX_LINE_SELECTOR_ENTRY
{
    GX_ENUM_LINE_SELECTOR_LINE0                    = 0,           ///< Line 0
    GX_ENUM_LINE_SELECTOR_LINE1                    = 1,           ///< Line 1
    GX_ENUM_LINE_SELECTOR_LINE2                    = 2,           ///< Line 2
    GX_ENUM_LINE_SELECTOR_LINE3                    = 3,           ///< Line 3
    GX_ENUM_LINE_SELECTOR_LINE4                    = 4,           ///< Line 4
    GX_ENUM_LINE_SELECTOR_LINE5                    = 5,           ///< Line 5
    GX_ENUM_LINE_SELECTOR_LINE6                    = 6,           ///< Line 6
    GX_ENUM_LINE_SELECTOR_LINE7                    = 7,           ///< Line 7
    GX_ENUM_LINE_SELECTOR_LINE8                    = 8,           ///< Line 8
    GX_ENUM_LINE_SELECTOR_LINE9                    = 9,           ///< Line 9
    GX_ENUM_LINE_SELECTOR_LINE10                   = 10,          ///< Line 10
    GX_ENUM_LINE_SELECTOR_LINE_STROBE              = 11,          ///< Line strobe
    GX_ENUM_LINE_SELECTOR_LINE11          		   = 12,          ///< Line 11
    GX_ENUM_LINE_SELECTOR_LINE12         		   = 13,          ///< Line 12
    GX_ENUM_LINE_SELECTOR_LINE13        		   = 14,          ///< Line 13
    GX_ENUM_LINE_SELECTOR_LINE14          		   = 15,          ///< Line 14
	GX_ENUM_LINE_SELECTOR_TRIGGER                  = 16,          ///<Hardware trigger input
	GX_ENUM_LINE_SELECTOR_IO1                      = 17,          ///<GPIO input
	GX_ENUM_LINE_SELECTOR_IO2                      = 18,          ///<GPIO input
	GX_ENUM_LINE_SELECTOR_FLASH_P                  = 19,          ///<Flash_ B transmission
	GX_ENUM_LINE_SELECTOR_FLASH_W                  = 20,          ///<Flash_ B transmission
} GX_LINE_SELECTOR_ENTRY;

typedef enum GX_LINE_MODE_ENTRY
{
    GX_ENUM_LINE_MODE_INPUT                        = 0,            ///< Input
    GX_ENUM_LINE_MODE_OUTPUT                       = 1,            ///< Output
} GX_LINE_MODE_ENTRY;

typedef enum GX_LINE_SOURCE_ENTRY
{
    GX_ENUM_LINE_SOURCE_OFF                        = 0,            ///< Off
    GX_ENUM_LINE_SOURCE_STROBE                     = 1,            ///< Strobe light
    GX_ENUM_LINE_SOURCE_USEROUTPUT0                = 2,            ///< UserDefined output 0
    GX_ENUM_LINE_SOURCE_USEROUTPUT1                = 3,            ///< UserDefined output 1
    GX_ENUM_LINE_SOURCE_USEROUTPUT2                = 4,            ///< UserDefined output 2
    GX_ENUM_LINE_SOURCE_EXPOSURE_ACTIVE            = 5,            ///< Exposure active
    GX_ENUM_LINE_SOURCE_FRAME_TRIGGER_WAIT         = 6,            ///< Frame trigger wait
    GX_ENUM_LINE_SOURCE_ACQUISITION_TRIGGER_WAIT   = 7,            ///< Acquisition trigger wait
    GX_ENUM_LINE_SOURCE_TIMER1_ACTIVE              = 8,            ///< Timer 1 active
    GX_ENUM_LINE_SOURCE_USEROUTPUT3                = 9,            ///< UserDefined output 3
    GX_ENUM_LINE_SOURCE_USEROUTPUT4                = 10,           ///< UserDefined output 4
    GX_ENUM_LINE_SOURCE_USEROUTPUT5                = 11,           ///< UserDefined output 5
    GX_ENUM_LINE_SOURCE_USEROUTPUT6                = 12,           ///< UserDefined output 6
    GX_ENUM_LINE_SOURCE_TIMER2_ACTIVE              = 13,           ///< Timer 2 active
    GX_ENUM_LINE_SOURCE_TIMER3_ACTIVE              = 14,           ///< Timer 3 active
    GX_ENUM_LINE_SOURCE_FRAME_TRIGGER              = 15,       	   ///< Frame trigger
	GX_ENUM_LINE_SOURCE_Flash_W                    = 16,           ///< Flash_w
	GX_ENUM_LINE_SOURCE_Flash_P                    = 17,           ///< Flash_P
	GX_ENUM_LINE_SOURCE_SERIAL_PORT_0			   = 18,		   ///< SerialPort0
} GX_LINE_SOURCE_ENTRY; 

typedef enum GX_ENCODER_SELECTOR_ENTRY
{
	GX_ENUM_ENCODER0							= 0,             ///< Encoder selector 0
	GX_ENUM_ENCODER1							= 1,             ///< Encoder selector 1
	GX_ENUM_ENCODER2							= 2,             ///< Encoder selector 2
} GX_ENCODER_SELECTOR_ENTRY;

typedef enum GX_ENCODER_SOURCEA_ENTRY
{
	GX_ENUM_SOURCEA_OFF							= 0,               ///< Encoder A related closed input
	GX_ENUM_SOURCEA_LINE0						= 1,               ///< Encoder phase A input Line0
	GX_ENUM_SOURCEA_LINE1						= 2,               ///< Encoder phase A input Line1
	GX_ENUM_SOURCEA_LINE2						= 3,               ///< Encoder phase A input Line2
	GX_ENUM_SOURCEA_LINE3						= 4,               ///< Encoder phase A input Line3
	GX_ENUM_SOURCEA_LINE4						= 5,               ///< Encoder phase A input Line4
	GX_ENUM_SOURCEA_LINE5						= 6,               ///< Encoder phase A input Line5
} GX_ENCODER_SOURCEA_ENTRY;

typedef enum GX_ENCODER_SOURCEB_ENTRY
{
	GX_ENUM_SOURCEB_OFF							= 0,               ///< Encoder phase B close input
	GX_ENUM_SOURCEB_LINE0						= 1,               ///< Encoder phase B input Line0
	GX_ENUM_SOURCEB_LINE1						= 2,               ///< Encoder phase B input Line1
	GX_ENUM_SOURCEB_LINE2						= 3,               ///< Encoder phase B input Line2
	GX_ENUM_SOURCEB_LINE3						= 4,               ///< Encoder phase B input Line3
	GX_ENUM_SOURCEB_LINE4						= 5,               ///< Encoder phase B input Line4
	GX_ENUM_SOURCEB_LINE5						= 6,               ///< Encoder phase B input Line5
} GX_ENCODER_SOURCEB_ENTRY;

typedef enum GX_ENCODER_MODE_ENTRY
{
	GX_ENUM_HIGH_RESOLUTION						= 0,				///< Encoder Mode
} GX_ENCODER_MODE_ENTRY;


typedef enum GX_ENCODER_DIRECTION_ENTRY
{
	GX_ENUM_FORWARD								= 0,               ///< Encoder direction forward
	GX_ENUM_BACKWARD							= 1,               ///< Encoder direction backward
} GX_ENCODER_DIRECTION_ENTRY;

typedef enum GX_EVENT_SELECTOR_ENTRY
{
    GX_ENUM_EVENT_SELECTOR_EXPOSUREEND					= 0x0004,       ///< End of exposure
    GX_ENUM_EVENT_SELECTOR_BLOCK_DISCARD				= 0x9000,       ///< Image frame discarding
    GX_ENUM_EVENT_SELECTOR_EVENT_OVERRUN				= 0x9001,       ///< Event queue overflow
    GX_ENUM_EVENT_SELECTOR_FRAMESTART_OVERTRIGGER		= 0x9002,       ///< Trigger signal overflow
    GX_ENUM_EVENT_SELECTOR_BLOCK_NOT_EMPTY				= 0x9003,       ///< Image frame memory is not empty
    GX_ENUM_EVENT_SELECTOR_INTERNAL_ERROR				= 0x9004,       ///< Internal error events
    GX_ENUM_EVENT_SELECTOR_FRAMEBURSTSTART_OVERTRIGGER	= 0x9005,   ///< Frame burst start overtrigger event
    GX_ENUM_EVENT_SELECTOR_FRAMESTART_WAIT				= 0x9006,   ///< Frame start wait event
    GX_ENUM_EVENT_SELECTOR_FRAMEBURSTSTART_WAIT			= 0x9007,   ///< Frame burst start wait event
} GX_EVENT_SELECTOR_ENTRY;

typedef enum GX_EVENT_NOTIFICATION_ENTRY
{
    GX_ENUM_EVENT_NOTIFICATION_OFF                 = 0,            ///< Turn off event
    GX_ENUM_EVENT_NOTIFICATION_ON                  = 1,            ///< Turn on event
} GX_EVENT_NOTIFICATION_ENTRY;

typedef enum GX_EVENT_SIMPLE_MODE_ENTRY
{
	GX_EVENT_SIMPLE_MODE_OFF						= 0,			///< Turn off block ID enable
	GX_EVENT_SIMPLE_MODE_ON							= 1,			///< Turn on block ID enable
}GX_EVENT_SIMPLE_MODE_ENTRY;

typedef enum GX_LUT_SELECTOR_ENTRY
{
    GX_ENUM_LUT_SELECTOR_LUMINANCE                 = 0,            ///< Luminance
} GX_LUT_SELECTOR_ENTRY;

typedef enum GX_TRANSFERDELAY_MODE_ENTRY
{
    GX_ENUM_TRANSFERDELAY_MODE_OFF                 = 0,            ///< Disable transmission delay
    GX_ENUM_TRANSFERDELAY_MODE_ON                  = 1,            ///< Enable transmission delay
} GX_TRANSFERDELAY_MODE_ENTRY;

typedef enum GX_COVER_FRAMESTORE_MODE_ENTRY
{
    GX_ENUM_COVER_FRAMESTORE_MODE_OFF              = 0,            ///< Disable Frame Memory Coverage
    GX_ENUM_COVER_FRAMESTORE_MODE_ON               = 1,            ///< Enable Frame Memory Coverage
} GX_COVER_FRAMESTORE_MODE_ENTRY;

typedef enum GX_USER_SET_DEFAULT_ENTRY
{
    GX_ENUM_USER_SET_DEFAULT_DEFAULT               = 0,            ///< Default parameter group
    GX_ENUM_USER_SET_DEFAULT_USERSET0              = 1,            ///< User parameter group 0
} GX_USER_SET_DEFAULT_ENTRY;

typedef enum GX_TRANSFER_CONTROL_MODE_ENTRY
{
    GX_ENUM_TRANSFER_CONTROL_MODE_BASIC            = 0,            ///< Turn off transmission control
    GX_ENUM_TRANSFER_CONTROL_MODE_USERCONTROLED    = 1,            ///< User-controlled transmission control mode
} GX_TRANSFER_CONTROL_MODE_ENTRY;

typedef enum GX_TRANSFER_OPERATION_MODE_ENTRY
{
    GX_ENUM_TRANSFER_OPERATION_MODE_MULTIBLOCK     = 0,            ///< Specify the number of frames to be sent
} GX_TRANSFER_OPERATION_MODE_ENTRY;

typedef enum GX_DS_RESEND_MODE_ENTRY
{
    GX_DS_RESEND_MODE_OFF                          = 0,            ///< Turn off resend mode     
    GX_DS_RESEND_MODE_ON                           = 1,            ///< Turn on resend mode
} GX_DS_RESEND_MODE_ENTRY;

typedef enum GX_DS_STREAM_BUFFER_HANDLING_MODE_ENTRY
{
    GX_DS_STREAM_BUFFER_HANDLING_MODE_OLDEST_FIRST              = 1,        ///< OldestFirst mode
    GX_DS_STREAM_BUFFER_HANDLING_MODE_OLDEST_FIRST_OVERWRITE    = 2,        ///< OldestFirstOverwrite mode
    GX_DS_STREAM_BUFFER_HANDLING_MODE_NEWEST_ONLY               = 3,        ///< NewestOnly mode
}GX_DS_STREAM_BUFFER_HANDLING_MODE_ENTRY;

typedef enum GX_DEVICE_LINK_THROUGHPUT_LIMIT_MODE_ENTRY
{
    GX_DEVICE_LINK_THROUGHPUT_LIMIT_MODE_OFF       = 0,            ///< Turn off bandwidth limitation mode
    GX_DEVICE_LINK_THROUGHPUT_LIMIT_MODE_ON        = 1             ///< Turn on bandwidth limitation mode
}GX_DEVICE_LINK_THROUGHPUT_LIMIT_MODE_ENTRY;

typedef enum GX_TEST_PATTERN_GENERATOR_SELECTOR_ENTRY
{
    GX_TEST_PATTERN_GENERATOR_SELECTOR_SENSOR      = 0,            ///< Test pattern generator of sensor
    GX_TEST_PATTERN_GENERATOR_SELECTOR_REGION0     = 1,            ///< Test pattern generator of FPGA
}GX_TEST_PATTERN_GENERATOR_SELECTOR_ENTRY;


typedef enum GX_CHUNK_SELECTOR_ENTRY
{
    GX_CHUNK_SELECTOR_CHUNK_FRAME_ID               = 1,            ///< Frame ID
    GX_CHUNK_SELECTOR_CHUNK_TIME_STAMP             = 2,            ///< Timestamp
    GX_CHUNK_SELECTOR_CHUNK_COUNTER_VALUE          = 3             ///< Counter value
}GX_CHUNK_SELECTOR_ENTRY;

typedef enum GX_ACQUISITION_FRAME_RATE_MODE_ENTRY
{
    GX_ACQUISITION_FRAME_RATE_MODE_OFF             = 0,            ///< Turn off frame rate control mode
    GX_ACQUISITION_FRAME_RATE_MODE_ON              = 1             ///< Turn on frame rate control mode
}GX_ACQUISITION_FRAME_RATE_MODE_ENTRY;

typedef enum GX_REGION_SEND_MODE
{
    GX_REGION_SEND_SINGLE_ROI_MODE                 = 0,            ///< Single ROI
    GX_REGION_SEND_MULTI_ROI_MODE                  = 1             ///< Multi ROI
}GX_REGION_SEND_MODE;

typedef enum GX_REGION_MODE
{
    GX_REGION_MODE_OFF                             = 0,            ///< Close currently selected region
    GX_REGION_MODE_ON                              = 1             ///< Open currently selected region
}GX_REGION_MODE;

typedef enum GX_REGION_SELECTOR_ENTRY
{
    GX_REGION_SELECTOR_REGION0                     = 0,            ///< Region 0
    GX_REGION_SELECTOR_REGION1                     = 1,            ///< Region 1
    GX_REGION_SELECTOR_REGION2                     = 2,            ///< Region 2
    GX_REGION_SELECTOR_REGION3                     = 3,            ///< Region 3
    GX_REGION_SELECTOR_REGION4                     = 4,            ///< Region 4
    GX_REGION_SELECTOR_REGION5                     = 5,            ///< Region 5
    GX_REGION_SELECTOR_REGION6                     = 6,            ///< Region 6
    GX_REGION_SELECTOR_REGION7                     = 7             ///< Region 7
}GX_REGION_SELECTOR_ENTRY;

typedef enum GX_SHARPNESS_MODE_ENTRY
{
    GX_SHARPNESS_MODE_OFF                          = 0,            ///< Turn off sharpness mode
    GX_SHARPNESS_MODE_ON                           = 1             ///< Turn on sharpness mode
}GX_SHARPNESS_MODE_ENTRY;

typedef enum GX_NOISE_REDUCTION_MODE_ENTRY
{
    GX_NOISE_REDUCTION_MODE_OFF                    = 0,            ///< Noise reduction mode off
    GX_NOISE_REDUCTION_MODE_ON                     = 1             ///< Noise reduction mode on
}GX_NOISE_REDUCTION_MODE_ENTRY;

typedef enum GX_SHADING_CORRECTION_MODE_ENTRY
{
	GX_SHADING_CORRECTION_MODE_FLAT_FIELD_CORRECTION		= 0,	///< Flat field correction
	GX_SHADING_CORRECTION_MODE_PARALLAX_CORRECTION			= 1,	///< Parallax correction
	GX_SHADING_CORRECTION_MODE_TAILOR_FLAT_FIELD_CORRECTION = 2,	///< Custom flat field correction
	GX_SHADING_CORRECTION_MODE_DEVICE_FLAT_FIELD_CORRECTION = 3		///< Equipment flat field correction
}GX_SHADING_CORRECTION_MODE_ENTRY;

typedef enum GX_FFC_GENERATE_STATUS_ENTRY
{
	GX_FFC_GENERATE_STATUS_IDLE						= 0,	///< idle
	GX_FFC_GENERATE_STATUS_WAITING_IMAGE			= 1,	///< Waiting image
	GX_FFC_GENERATE_STATUS_FINISH					= 2,	///< complete
}GX_FFC_GENERATE_STATUS_ENTRY;

typedef enum GX_FFC_COEFFICIENT_ENTRY
{
	GX_FFC_COEFFICIENT_SET0							= 0,	///< Flat field correction coefficient Set0
	GX_FFC_COEFFICIENT_SET1							= 1, 	///< Flat field correction coefficient Set1
	GX_FFC_COEFFICIENT_SET2							= 2,	///< Flat field correction coefficient Set2
	GX_FFC_COEFFICIENT_SET3							= 3, 	///< Flat field correction coefficient Set3
	GX_FFC_COEFFICIENT_SET4							= 4,	///< Flat field correction coefficient Set4
	GX_FFC_COEFFICIENT_SET5							= 5, 	///< Flat field correction coefficient Set5
	GX_FFC_COEFFICIENT_SET6							= 6,	///< Flat field correction coefficient Set6
	GX_FFC_COEFFICIENT_SET7							= 7, 	///< Flat field correction coefficient Set7
	GX_FFC_COEFFICIENT_SET8							= 8,	///< Flat field correction coefficient Set8
	GX_FFC_COEFFICIENT_SET9							= 9, 	///< Flat field correction coefficient Set9
	GX_FFC_COEFFICIENT_SET10						= 10,	///< Flat field correction coefficient Set10
	GX_FFC_COEFFICIENT_SET11						= 11, 	///< Flat field correction coefficient Set11
	GX_FFC_COEFFICIENT_SET12						= 12,	///< Flat field correction coefficient Set12
	GX_FFC_COEFFICIENT_SET13						= 13,	///< Flat field correction coefficient Set13
	GX_FFC_COEFFICIENT_SET14						= 14,	///< Flat field correction coefficient Set14
	GX_FFC_COEFFICIENT_SET15						= 15,	///< Flat field correction coefficient Set15
}GX_FFC_COEFFICIENT_ENTRY;

typedef enum GX_FFC_EXPECTED_GRAY_VALUE_ENABLE_ENTRY
{
	GX_FFC_EXPECTED_GRAY_VALUE_ENABLE_OFF			= 0,	///< Disable flat field correction expected gray value
	GX_FFC_EXPECTED_GRAY_VALUE_ENABLE_ON			= 1,	///< Enable flat field correction expected gray value
}GX_FFC_EXPECTED_GRAY_VALUE_ENABLE_ENTRY;

typedef enum GX_DSNU_SELECTOR_ENTRY
{
	GX_DSNU_SELECTOR_DEFAULT						= 0,	///< Dark field correction coefficient Default
	GX_DSNU_SELECTOR_SET0							= 1,	///< Dark field correction coefficient Set0
	GX_DSNU_SELECTOR_SET1							= 2, 	///< Dark field correction coefficient Set1
	GX_DSNU_SELECTOR_SET2							= 3,	///< Dark field correction coefficient Set2
	GX_DSNU_SELECTOR_SET3							= 4, 	///< Dark field correction coefficient Set3
	GX_DSNU_SELECTOR_SET4							= 5,	///< Dark field correction coefficient Set4
	GX_DSNU_SELECTOR_SET5							= 6, 	///< Dark field correction coefficient Set5
	GX_DSNU_SELECTOR_SET6							= 7,	///< Dark field correction coefficient Set6
	GX_DSNU_SELECTOR_SET7							= 8, 	///< Dark field correction coefficient Set7
	GX_DSNU_SELECTOR_SET8							= 9,	///< Dark field correction coefficient Set8
	GX_DSNU_SELECTOR_SET9							= 10, 	///< Dark field correction coefficient Set9
	GX_DSNU_SELECTOR_SET10							= 11,	///< Dark field correction coefficient Set10
	GX_DSNU_SELECTOR_SET11							= 12, 	///< Dark field correction coefficient Set11
	GX_DSNU_SELECTOR_SET12							= 13,	///< Dark field correction coefficient Set12
	GX_DSNU_SELECTOR_SET13							= 14,	///< Dark field correction coefficient Set13
	GX_DSNU_SELECTOR_SET14							= 15,	///< Dark field correction coefficient Set14
	GX_DSNU_SELECTOR_SET15							= 16,	///< Dark field correction coefficient Set15
}GX_DSNU_SELECTOR_ENTRY;

typedef enum GX_DSNU_GENERATE_STATUS_ENTRY
{
	GX_DSNU_GENERATE_STATUS_IDLE					= 0,	///< leave unused
	GX_DSNU_GENERATE_STATUS_WAITING_IMAGE			= 1,	///< Wait for image
	GX_DSNU_GENERATE_STATUS_FINISH					= 2,	///< accomplish
}GX_DSNU_GENERATE_STATUS_ENTRY;

typedef enum GX_PRNU_SELECTOR_ENTRY
{
	GX_PRNU_SELECTOR_DEFAULT						= 0,	///< Bright field correction coefficient Default
	GX_PRNU_SELECTOR_SET0							= 1,	///< Bright field correction coefficient Set0
	GX_PRNU_SELECTOR_SET1							= 2, 	///< Bright field correction coefficient Set1
	GX_PRNU_SELECTOR_SET2							= 3,	///< Bright field correction coefficient Set2
	GX_PRNU_SELECTOR_SET3							= 4, 	///< Bright field correction coefficient Set3
	GX_PRNU_SELECTOR_SET4							= 5,	///< Bright field correction coefficient Set4
	GX_PRNU_SELECTOR_SET5							= 6, 	///< Bright field correction coefficient Set5
	GX_PRNU_SELECTOR_SET6							= 7,	///< Bright field correction coefficient Set6
	GX_PRNU_SELECTOR_SET7							= 8, 	///< Bright field correction coefficient Set7
	GX_PRNU_SELECTOR_SET8							= 9,	///< Bright field correction coefficient Set8
	GX_PRNU_SELECTOR_SET9							= 10, 	///< Bright field correction coefficient Set9
	GX_PRNU_SELECTOR_SET10							= 11,	///< Bright field correction coefficient Set10
	GX_PRNU_SELECTOR_SET11							= 12, 	///< Bright field correction coefficient Set11
	GX_PRNU_SELECTOR_SET12							= 13,	///< Bright field correction coefficient Set12
	GX_PRNU_SELECTOR_SET13							= 14,	///< Bright field correction coefficient Set13
	GX_PRNU_SELECTOR_SET14							= 15,	///< Bright field correction coefficient Set14
	GX_PRNU_SELECTOR_SET15							= 16,	///< Bright field correction coefficient Set15
}GX_PRNU_SELECTOR_ENTRY;

typedef enum GX_PRNU_GENERATE_STATUS_ENTRY
{
	GX_PRNU_GENERATE_STATUS_IDLE					= 0,	///< leave unused
	GX_PRNU_GENERATE_STATUS_WAITING_IMAGE			= 1,	///< Wait for image
	GX_PRNU_GENERATE_STATUS_FINISH					= 2,	///< accomplish
}GX_PRNU_GENERATE_STATUS_ENTRY;

typedef enum GX_CXP_LINK_CONFIGURATION_ENTRY
{
	GX_CXP_LINK_CONFIGURATION_CXP6_X1				= 0x00010048,	///< Connection configuration status CXP6_X1
	GX_CXP_LINK_CONFIGURATION_CXP12_X1				= 0x00010058,	///< Connection configuration status CXP12_X1
	GX_CXP_LINK_CONFIGURATION_CXP6_X2				= 0x00020048,	///< Connection configuration status CXP6_X2
	GX_CXP_LINK_CONFIGURATION_CXP12_X2				= 0x00020058,	///< Connection configuration status CXP12_X2
	GX_CXP_LINK_CONFIGURATION_CXP6_X4				= 0x00040048,	///< Connection configuration status CXP6_X4
	GX_CXP_LINK_CONFIGURATION_CXP12_X4				= 0x00040058,	///< Connection configuration status CXP12_X4
	GX_CXP_LINK_CONFIGURATION_CXP3_X1				= 0x00010038,	///< CXP Connection configuration status CXP3_X1
	GX_CXP_LINK_CONFIGURATION_CXP3_X2				= 0x00020038,	///< CXP Connection configuration status CXP3_X2
	GX_CXP_LINK_CONFIGURATION_CXP3_X4				= 0x00040038,	///< CXP Connection configuration status CXP3_X4

}GX_CXP_LINK_CONFIGURATION_ENTRY;

typedef enum GX_CXP_LINK_CONFIGURATION_PREFERRED_ENTRY
{
	GX_CXP_LINK_CONFIGURATION_PREFERRED_CXP12_X4	= 0x00040058,	///< Preset connection configuration CXP12_X4
}GX_CXP_LINK_CONFIGURATION_PREFERRED_ENTRY;

typedef enum GX_CXP_LINK_CONFIGURATION_STATUS_ENTRY
{
	GX_CXP_LINK_CONFIGURATION_STATUS_CXP6_X1		= 0x00010048,	///< CXP Connection configuration status CXP6_X1
	GX_CXP_LINK_CONFIGURATION_STATUS_CXP12_X1		= 0x00010058,	///< CXP Connection configuration status CXP12_X1
	GX_CXP_LINK_CONFIGURATION_STATUS_CXP6_X2		= 0x00020048,	///< CXP Connection configuration status CXP6_X2
	GX_CXP_LINK_CONFIGURATION_STATUS_CXP12_X2		= 0x00020058,	///< CXP Connection configuration status CXP12_X2
	GX_CXP_LINK_CONFIGURATION_STATUS_CXP6_X4		= 0x00040048,	///< CXP Connection configuration status CXP6_X4
	GX_CXP_LINK_CONFIGURATION_STATUS_CXP12_X4		= 0x00040058,	///< CXP Connection configuration status CXP12_X4
	GX_CXP_LINK_CONFIGURATION_STATUS_CXP3_X1		= 0x00010038,	///< CXP Connection configuration status CXP3_X1
	GX_CXP_LINK_CONFIGURATION_STATUS_CXP3_X2		= 0x00020038,	///< CXP Connection configuration status CXP3_X2
	GX_CXP_LINK_CONFIGURATION_STATUS_CXP3_X4		= 0x00040038,	///< CXP Connection configuration status CXP3_X4
}GX_CXP_LINK_CONFIGURATION_STATUS_ENTRY;

typedef enum GX_CXP_CONNECTION_SELECTOR_ENTRY
{
	GX_CXP_CONNECTION_SELECTOR_0					= 0,	///< Connection selection 0
	GX_CXP_CONNECTION_SELECTOR_1					= 1,	///< Connection selection 1
	GX_CXP_CONNECTION_SELECTOR_2					= 2,	///< Connection selection 2
	GX_CXP_CONNECTION_SELECTOR_3					= 3,	///< Connection selection 3
}GX_CXP_CONNECTION_SELECTOR_ENTRY;

typedef enum GX_CXP_CONNECTION_TEST_MODE_ENTRY
{
	GX_CXP_CONNECTION_TEST_MODE_OFF					= 0,     ///< Turn off connection test mode
	GX_CXP_CONNECTION_TEST_MODE_MODE1				= 1,     ///< Trigger connection test mode
}GX_CXP_CONNECTION_TEST_MODE_ENTRY;

typedef enum GX_SEQUENCER_MODE_ENTRY
{
	GX_SEQUENCER_MODE_OFF							= 0,    ///< Close sequencer mode
	GX_SEQUENCER_MODE_ON							= 1,    ///< Open sequencer mode
}GX_SEQUENCER_MODE_ENTRY;

typedef enum GX_SEQUENCER_CONFIGURATION_MODE_ENTRY
{
	GX_SEQUENCER_CONFIGURATION_MODE_OFF				= 0,    ///< Turn off sequencer configuration mode
	GX_SEQUENCER_CONFIGURATION_MODE_ON				= 1,    ///< Open sequencer configuration mode
}GX_SEQUENCER_CONFIGURATION_MODE_ENTRY;

typedef enum GX_SEQUENCER_FEATURE_SELECTOR_ENTRY
{
	GX_SEQUENCER_FEATURE_SELECTOR_FLAT_FIELD_CORRECTION = 0,    ///< Sequence function selection
}GX_SEQUENCER_FEATURE_SELECTOR_ENTRY;

typedef enum GX_SEQUENCER_TRIGGER_SOURCE_ENTRY
{
	GX_SEQUENCER_TRIGGER_SOURCE_FRAME_START				= 7,		///< Sequence Trigger FrameStart

}GX_SEQUENCER_TRIGGER_SOURCE_ENTRY;

typedef enum GX_BINNING_HORIZONTAL_MODE_ENTRY
{
    GX_BINNING_HORIZONTAL_MODE_SUM                 = 0,            ///< Horizontal value sum of BINNING
    GX_BINNING_HORIZONTAL_MODE_AVERAGE             = 1,            ///< Average horizontal value of BINNING
}GX_BINNING_HORIZONTAL_MODE_ENTRY;

typedef enum GX_BINNING_VERTICAL_MODE_ENTRY
{
    GX_BINNING_VERTICAL_MODE_SUM                   = 0,            ///< Vertical value sum of BINNING
    GX_BINNING_VERTICAL_MODE_AVERAGE               = 1,            ///< Average Vertical value of BINNING
}GX_BINNING_VERTICAL_MODE_ENTRY;

typedef enum GX_SENSOR_SHUTTER_MODE_ENTRY
{
    GX_SENSOR_SHUTTER_MODE_GLOBAL                  = 0,            ///< All pixels are exposed simultaneously with same exposure time
    GX_SENSOR_SHUTTER_MODE_ROLLING                 = 1,            ///< All pixels have the same exposure time, but exposure start at different time
    GX_SENSOR_SHUTTER_MODE_GLOBALRESET             = 2,            ///< All pixels start exposure at same time, but exposure time are different
}GX_SENSOR_SHUTTER_MODE_ENTRY;

typedef enum GX_ACQUISITION_STATUS_SELECTOR_ENTRY
{
    GX_ACQUISITION_STATUS_SELECTOR_ACQUISITION_TRIGGER_WAIT   = 0, ///< Acquisition trigger waiting
    GX_ACQUISITION_STATUS_SELECTOR_FRAME_TRIGGER_WAIT         = 1, ///< Frame trigger waiting
}GX_ACQUISITION_STATUS_SELECTOR_ENTRY;

typedef enum GX_GAMMA_MODE_ENTRY
{
    GX_GAMMA_SELECTOR_SRGB                         = 0,            ///< Default gamma correction
    GX_GAMMA_SELECTOR_USER                         = 1,            ///< User-defined gamma correction
}GX_GAMMA_MODE_ENTRY;

typedef enum GX_LIGHT_SOURCE_PRESET_ENTRY
{
    GX_LIGHT_SOURCE_PRESET_OFF                     = 0,            ///< Light house preset OFF
    GX_LIGHT_SOURCE_PRESET_CUSTOM                  = 1,            ///< Light house preset CUSTOM
    GX_LIGHT_SOURCE_PRESET_DAYLIGHT_6500K          = 2,            ///< Light house preset DAYLIGHT_6500K
    GX_LIGHT_SOURCE_PRESET_DAYLIGHT_5000K          = 3,            ///< Light house preset DAYLIGHT_5000K
    GX_LIGHT_SOURCE_PRESET_COOL_WHITE_FLUORESCENCE = 4,            ///< Light house preset COOL_WHITE_FLUORESCENCE
    GX_LIGHT_SOURCE_PRESET_INCA                    = 5,            ///< Light house preset INCANDESCENT_A
}GX_LIGHT_SOURCE_PRESET_ENTRY;

typedef enum GX_COLOR_TRANSFORMATION_MODE_ENTRY
{
    GX_COLOR_TRANSFORMATION_SELECTOR_RGB_TO_RGB    = 0,            ///< Default color transformation
    GX_COLOR_TRANSFORMATION_SELECTOR_USER          = 1,            ///< User-defined color transformation
}GX_COLOR_TRANSFORMATION_MODE_ENTRY;

typedef enum GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_ENTRY
{
    GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_GAIN00  = 0,
    GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_GAIN01  = 1,
    GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_GAIN02  = 2,
    GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_GAIN10  = 3,
    GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_GAIN11  = 4,
    GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_GAIN12  = 5,
    GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_GAIN20  = 6,
    GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_GAIN21  = 7,
    GX_COLOR_TRANSFORMATION_VALUE_SELECTOR_GAIN22  = 8,
}GX_COLOR_TRANSFORMATION_VALUE_ENTRY;

/* Reset Device Mode */
typedef enum GX_RESET_DEVICE_MODE
{
    GX_MANUFACTURER_SPECIFIC_RECONNECT             = 0x1,          ///< Reconnect Device
    GX_MANUFACTURER_SPECIFIC_RESET                 = 0x2           ///< Reset Device 
}GX_RESET_DEVICE_MODE;

typedef enum GX_TIMER_SELECTOR_ENTRY
{
    GX_TIMER_SELECTOR_TIMER1                       = 1,            ///< Timer 1
    GX_TIMER_SELECTOR_TIMER2   					   = 2,            ///< Timer2
    GX_TIMER_SELECTOR_TIMER3   					   = 3,            ///< Timer3
}GX_TIMER_SELECTOR_ENTRY;

typedef enum GX_TIMER_TRIGGER_SOURCE_ENTRY
{
    GX_TIMER_TRIGGER_SOURCE_EXPOSURE_START         = 1,            ///< Exposure start
    GX_TIMER_TRIGGER_SOURCE_LINE10           	   = 10,      	   ///< Trigger source line 10
    GX_TIMER_TRIGGER_SOURCE_LINE14           	   = 14,           ///< Trigger source line 14
    GX_TIMER_TRIGGER_SOURCE_STROBE           	   = 16,           ///< Trigger source strobe
}GX_TIMER_TRIGGER_SOURCE_ENTRY;

typedef enum GX_COUNTER_SELECTOR_ENTRY
{
    GX_COUNTER_SELECTOR_COUNTER1                   = 1,            ///< Counter 1
    GX_COUNTER_SELECTOR_COUNTER2                   = 2,            ///< Counter 2
}GX_COUNTER_SELECTOR_ENTRY;

typedef enum GX_COUNTER_EVENT_SOURCE_ENTRY
{
    GX_COUNTER_EVENT_SOURCE_FRAME_START            = 1,            ///< Count the number of "FRAME_START" events
    GX_COUNTER_EVENT_SOURCE_FRAME_TRIGGER          = 2,            ///< Count the number of "FRAME_TRIGGER" events
    GX_COUNTER_EVENT_SOURCE_ACQUISITION_TRIGGER    = 3,            ///< Count the number of "ACQUISITION_TRIGGER" events
    GX_COUNTER_EVENT_SOURCE_OFF                    = 4,            ///< Off
    GX_COUNTER_EVENT_SOURCE_SOFTWARE               = 5,            ///< Count the number of "SOFTWARE_TRIGGER" events
    GX_COUNTER_EVENT_SOURCE_LINE0                  = 6,            ///< Count the number of "LINE0_TRIGGER" events
    GX_COUNTER_EVENT_SOURCE_LINE1                  = 7,            ///< Count the number of "LINE1_TRIGGER" events
    GX_COUNTER_EVENT_SOURCE_LINE2                  = 8,            ///< Count the number of "LINE2_TRIGGER" events
    GX_COUNTER_EVENT_SOURCE_LINE3                  = 9,            ///< Count the number of "LINE3_TRIGGER" events
}GX_COUNTER_EVENT_SOURCE_ENTRY;

typedef enum GX_COUNTER_RESET_SOURCE_ENTRY
{
    GX_COUNTER_RESET_SOURCE_OFF                    = 0,            ///< Counter reset off
    GX_COUNTER_RESET_SOURCE_SOFTWARE               = 1,            ///< Software
    GX_COUNTER_RESET_SOURCE_LINE0                  = 2,            ///< Line 0
    GX_COUNTER_RESET_SOURCE_LINE1                  = 3,            ///< Line 1
    GX_COUNTER_RESET_SOURCE_LINE2                  = 4,            ///< Line 2
    GX_COUNTER_RESET_SOURCE_LINE3                  = 5,            ///< Line 3
    GX_COUNTER_RESET_SOURCE_COUNTER2END            = 6,            ///< Counter2End
	GX_COUNTER_RESET_SOURCE_CXPTRIGGER0			   = 8,			  ///< CxpTrigger0
	GX_COUNTER_RESET_SOURCE_CXPTRIGGER1			   = 9,			  ///< CxpTrigger1
}GX_COUNTER_RESET_SOURCE_ENTRY;

typedef enum GX_COUNTER_TRIGGER_SOURCE_ENTRY
{
    GX_COUNTER_TRIGGER_SOURCE_OFF                  = 0,            ///< Off
    GX_COUNTER_TRIGGER_SOURCE_SOFTWARE             = 1,            ///< Software
    GX_COUNTER_TRIGGER_SOURCE_LINE0                = 2,            ///< Line 0
    GX_COUNTER_TRIGGER_SOURCE_LINE1                = 3,            ///< Line 1
    GX_COUNTER_TRIGGER_SOURCE_LINE2                = 4,            ///< Line 2
    GX_COUNTER_TRIGGER_SOURCE_LINE3                = 5,            ///< Line 3
}GX_COUNTER_TRIGGER_SOURCE_ENTRY;

typedef enum GX_COUNTER_RESET_ACTIVATION_ENTRY
{
    GX_COUNTER_RESET_ACTIVATION_RISING_EDGE        = 1,            ///< Rising edge counter reset.
}GX_COUNTER_RESET_ACTIVATION_ENTRY;

typedef enum GX_USER_DATA_FILED_SELECTOR_ENTRY
{
    GX_USER_DATA_FILED_0                           = 0,            ///< Flash data field 0
    GX_USER_DATA_FILED_1                           = 1,            ///< Flash data field 1
    GX_USER_DATA_FILED_2                           = 2,            ///< Flash data field 2
    GX_USER_DATA_FILED_3                           = 3,            ///< Flash data field 3
}GX_USER_DATA_FILED_SELECTOR_ENTRY;

typedef enum GX_REMOVE_PARAMETER_LIMIT_ENTRY
{
    GX_ENUM_REMOVE_PARAMETER_LIMIT_OFF             = 0,            ///< Off
    GX_ENUM_REMOVE_PARAMETER_LIMIT_ON              = 1,            ///< On
}GX_REMOVE_PARAMETER_LIMIT_ENTRY;

typedef enum GX_FLAT_FIELD_CORRECTION_ENTRY
{
    GX_ENUM_FLAT_FIELD_CORRECTION_OFF              = 0,            ///< Off
    GX_ENUM_FLAT_FIELD_CORRECTION_ON               = 1,            ///< On
}GX_FLAT_FIELD_CORRECTION_ENTRY;

typedef enum GX_DEVICE_TEMPERATURE_SELECTOR_ENTRY
{
    GX_DEVICE_TEMPERATURE_SELECTOR_SENSOR          = 1,            ///< Sensor
    GX_DEVICE_TEMPERATURE_SELECTOR_MAINBOARD       = 2,            ///< Mainboard
}GX_DEVICE_TEMPERATURE_SELECTOR_ENTRY;

typedef enum GX_STOP_ACQUISITION_MODE_ENTRY
{
    GX_STOP_ACQUISITION_MODE_GENERAL               = 0,            ///< General stop acquisition mode
    GX_STOP_ACQUISITION_MODE_LIGHT                 = 1,            ///< Light stop acquisition mode
} GX_STOP_ACQUISITION_MODE_ENTRY;

typedef enum GX_EXPOSURE_TIME_MODE_ENTRY
{
    GX_EXPOSURE_TIME_MODE_ULTRASHORT               = 0,            ///< Ultra short mode
    GX_EXPOSURE_TIME_MODE_STANDARD                 = 1,            ///< Standard mode
} GX_EXPOSURE_TIME_MODE_ENTRY;

typedef enum GX_ENUM_SATURATION_MODE_ENTRY
{
    GX_ENUM_SATURATION_OFF = 0,    ///< Off
    GX_ENUM_SATURATION_ON  = 1,    ///< On
}GX_ENUM_SATURATION_MODE_ENTRY;

typedef enum GX_ENUM_STATIC_DEFECT_CORRECTION_ENTRY
{
    GX_ENUM_STATIC_DEFECT_CORRECTION_OFF = 0,    ///< Off
    GX_ENUM_STATIC_DEFECT_CORRECTION_ON  = 1,    ///< On
}GX_ENUM_STATIC_DEFECT_CORRECTION_ENTRY;
	
typedef enum GX_2D_NOISE_REDUCTION_MODE_ENTRY
{
    GX_2D_NOISE_REDUCTION_MODE_OFF = 0,     ///< Close 2D Noise Reduction Mode.
    GX_2D_NOISE_REDUCTION_MODE_LOW = 1,     ///< 2D Noise Reduction Mode: Low.
    GX_2D_NOISE_REDUCTION_MODE_MIDDLE = 2,  ///< 2D Noise Reduction Mode: Middle.
    GX_2D_NOISE_REDUCTION_MODE_HIGH = 3,    ///< 2D Noise Reduction Mode: High.
}GX_2D_NOISE_REDUCTION_MODE_ENTRY;

typedef enum GX_3D_NOISE_REDUCTION_MODE_ENTRY
{
    GX_3D_NOISE_REDUCTION_MODE_OFF = 0,     ///< Close 3D Noise Reduction Mode.
    GX_3D_NOISE_REDUCTION_MODE_LOW = 1,     ///< 3D Noise Reduction Mode: Low.
    GX_3D_NOISE_REDUCTION_MODE_MIDDLE = 2,  ///< 3D Noise Reduction Mode: Middle.
    GX_3D_NOISE_REDUCTION_MODE_HIGH = 3,    ///< 3D Noise Reduction Mode: High.
}GX_3D_NOISE_REDUCTION_MODE_ENTRY;

typedef enum GX_HDR_MODE_ENTRY
{
    GX_HDR_MODE_OFF = 0,            ///< Close HDR Mode.
    GX_HDR_MODE_CONTINUOUS = 1,     ///< Enable continuous mode.
}GX_HDR_MODE_ENTRY;

typedef enum GX_MGC_MODE_ENTRY
{
    GX_MGC_MODE_OFF = 0,            ///< Close MGC Mode.
    GX_MGC_MODE_TWO_FRAME = 1,      ///< Sets the first frame MGC ExposureTime and MGC Gain.
    GX_MGC_MODE_FOUR_FRAME = 2,     ///< Sets the second frame MGC ExposureTime and MGC Gain.
}GX_MGC_CONTROL_MODE_ENTRY;

typedef enum GX_TIMER_TRIGGER_ACTIVATION_ENTRY
{
	GX_TIMER_TRIGGER_ACTIVATION_RISINGEDGE = 0,            ///< Rising edge Timer Trigger Activation
}GX_TIMER_TRIGGER_ACTIVATION_ENTRY;

typedef enum GX_ACQUISITION_BURST_MODE_ENTRY
{
	GX_ENUM_ACQUISITION_BURST_MODE_STANDARD    = 0,    ///< Standard mode   
	GX_ENUM_ACQUISITION_BURST_MODE_HIGH_SPEED  = 1,    ///< High speed mode
}GX_ACQUISITION_BURST_MODE_ENTRY;

typedef enum GX_LOWPOWER_MODE_ENTRY
{
    GX_LOWPOWER_MODE_OFF            = 0,    ///< Not in low power consumption mode
    GX_LOWPOWER_MODE_ON             = 1,    ///< Enter low power consumption mode
}GX_LOWPOWER_MODE_ENTRY;

typedef enum GX_CLOSE_CCD_ENTRY
{
	GX_CLOSE_CCD_OFF				= 0,	///< Normal mode
	GX_CLOSE_CCD_ON					= 1,	///< Enter CCD low power consumption mode
}GX_CLOSE_CCD_ENTRY;

typedef enum GX_SENSOR_SELECTOR_ENTRY
{
    GX_SENSOR_SELECTOR_CMOS1        = 0,    ///< Select CMOS 1 Sensor
    GX_SENSOR_SELECTOR_CCD1         = 1,    ///< Select CCD1 sensor
}GX_SENSOR_SELECTOR_ENTRY;

typedef enum GX_IMU_CONFIG_ACC_RANGE_ENTRY
{
    GX_IMU_CONFIG_ACC_RANGE_16G     = 2,    ///< The accelerometer has a measuring range of 16g
    GX_IMU_CONFIG_ACC_RANGE_8G      = 3,    ///< The accelerometer has a measuring range of 8g
    GX_IMU_CONFIG_ACC_RANGE_4G      = 4,    ///< The accelerometer has a measuring range of 4g
    GX_IMU_CONFIG_ACC_RANGE_2G      = 5,    ///< The accelerometer has a measuring range of 5g
}GX_IMU_CONFIG_ACC_RANGE_ENTRY;

typedef enum GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_SWITCH_ENTRY
{
    GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_ON                    = 0,    ///< Open the accelerometer low-pass filter switch
    GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_OFF                   = 1,    ///< Close the accelerometer low-pass filter switch
}GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_SWITCH_ENTRY;

typedef enum GX_IMU_CONFIG_ACC_ODR_ENTRY
{
    GX_IMU_CONFIG_ACC_ODR_1000HZ    = 0,    ///< The accelerometer output data rate is 1000Hz
    GX_IMU_CONFIG_ACC_ODR_500HZ     = 1,    ///< The accelerometer output data rate is 500Hz
    GX_IMU_CONFIG_ACC_ODR_250Hz     = 2,    ///< The accelerometer output data rate is 250Hz
    GX_IMU_CONFIG_ACC_ODR_125Hz     = 3,    ///< The accelerometer output data rate is 125Hz
    GX_IMU_CONFIG_ACC_ODR_63Hz      = 4,    ///< The accelerometer output data rate is 63Hz
    GX_IMU_CONFIG_ACC_ODR_31Hz      = 5,    ///< The accelerometer output data rate is 31Hz
    GX_IMU_CONFIG_ACC_ODR_16Hz      = 6,    ///< The accelerometer output data rate is 16Hz
    GX_IMU_CONFIG_ACC_ODR_2000Hz    = 8,    ///< The accelerometer output data rate is 2000Hz
    GX_IMU_CONFIG_ACC_ODR_4000Hz    = 9,    ///< The accelerometer output data rate is 4000Hz
    GX_IMU_CONFIG_ACC_ODR_8000Hz    = 10,   ///< The accelerometer output data rate is 8000Hz
}GX_IMU_CONFIG_ACC_ODR_ENTRY;

typedef enum GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_FREQUENCY_ENTRY
{
    GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_FREQUENCY_ODR040      = 0,    ///< Accelerometer The accelerometer low-pass cutoff frequency isODR×0.40
    GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_FREQUENCY_ODR025      = 1,    ///< Accelerometer The accelerometer low-pass cutoff frequency isODR×0.25
    GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_FREQUENCY_ODR011      = 2,    ///< Accelerometer The accelerometer low-pass cutoff frequency isODR×0.11
    GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_FREQUENCY_ODR004      = 3,    ///< Accelerometer The accelerometer low-pass cutoff frequency isODR×0.04
    GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_FREQUENCY_ODR002      = 4,    ///< Accelerometer The accelerometer low-pass cutoff frequency isODR×0.02
}GX_IMU_CONFIG_ACC_ODR_LOW_PASS_FILTER_FREQUENCY_ENTRY;

typedef enum GX_IMU_CONFIG_GYRO_RANGE_ENTRY
{
    GX_IMU_CONFIG_GYRO_RANGE_125DPS     = 2,    ///< The measuring range of gyroscope in X direction is125dps
    GX_IMU_CONFIG_GYRO_RANGE_250DPS     = 3,    ///< The measuring range of gyroscope in X direction is250dps
    GX_IMU_CONFIG_GYRO_RANGE_500DPS     = 4,    ///< The measuring range of gyroscope in X direction is500dps
    GX_IMU_CONFIG_GYRO_RANGE_1000DPS    = 5,    ///< The measuring range of gyroscope in X direction is1000dps
    GX_IMU_CONFIG_GYRO_RANGE_2000DPS    = 6,    ///< 陀The measuring range of gyroscope in X direction is2000dps
}GX_IMU_CONFIG_GYRO_RANGE_ENTRY;

typedef enum GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_SWITCH_ENTRY
{
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_ON                   = 0,    ///< Turn on the gyro low-pass filter
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_OFF                  = 1,    ///< Turn off the gyro low-pass filter
}GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_SWITCH_ENTRY;

typedef enum GX_IMU_CONFIG_GYRO_ODR_ENTRY
{
    GX_IMU_CONFIG_GYRO_ODR_1000HZ       = 0,    ///< Gyroscope output data rate is1000Hz
    GX_IMU_CONFIG_GYRO_ODR_500HZ        = 1,    ///< Gyroscope output data rate is500Hz
    GX_IMU_CONFIG_GYRO_ODR_250HZ        = 2,    ///< Gyroscope output data rate is250Hz
    GX_IMU_CONFIG_GYRO_ODR_125HZ        = 3,    ///< Gyroscope output data rate is125Hz
    GX_IMU_CONFIG_GYRO_ODR_63HZ         = 4,    ///< Gyroscope output data rate is63Hz
    GX_IMU_CONFIG_GYRO_ODR_31HZ         = 5,    ///< Gyroscope output data rate is31Hz
    GX_IMU_CONFIG_GYRO_ODR_4KHZ         = 9,    ///< Gyroscope output data rate is4000Hz
    GX_IMU_CONFIG_GYRO_ODR_8KHZ         = 10,   ///< Gyroscope output data rate is8000Hz
    GX_IMU_CONFIG_GYRO_ODR_16KHZ        = 11,   ///< Gyroscope output data rate is16Hz
    GX_IMU_CONFIG_GYRO_ODR_32KHZ        = 12,   ///< Gyroscope output data rate is32Hz
}GX_IMU_CONFIG_GYRO_ODR_ENTRY;

typedef enum GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_ENTRY
{
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF2000HZ     = 2000,    ///< Accelerometer The accelerometer low-pass cutoff frequency is2000Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF1600HZ     = 1600,    ///< Accelerometer The accelerometer low-pass cutoff frequency is1600Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF1525HZ     = 1525,    ///< Accelerometer The accelerometer low-pass cutoff frequency is1525Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF1313HZ     = 1313,    ///< Accelerometer The accelerometer low-pass cutoff frequency is1313Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF1138HZ     = 1138,    ///< Accelerometer The accelerometer low-pass cutoff frequency is1138Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF1000HZ     = 1000,    ///< Accelerometer The accelerometer low-pass cutoff frequency is1000Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF863HZ      = 863,     ///< Accelerometer The accelerometer low-pass cutoff frequency is863Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF638HZ      = 638,     ///< Accelerometer The accelerometer low-pass cutoff frequency is638Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF438HZ      = 438,     ///< Accelerometer The accelerometer low-pass cutoff frequency is438Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF313HZ      = 313,     ///< Accelerometer The accelerometer low-pass cutoff frequency is313Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF213HZ      = 213,     ///< Accelerometer The accelerometer low-pass cutoff frequency is213Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF219HZ      = 219,     ///< Accelerometer The accelerometer low-pass cutoff frequency is219Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF363HZ      = 363,     ///< Accelerometer The accelerometer low-pass cutoff frequency is363Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF320HZ      = 320,     ///< Accelerometer The accelerometer low-pass cutoff frequency is320Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF250HZ      = 250,     ///< Accelerometer The accelerometer low-pass cutoff frequency is250Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF200HZ      = 200,     ///< Accelerometer The accelerometer low-pass cutoff frequency is200Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF181HZ      = 181,     ///< Accelerometer The accelerometer low-pass cutoff frequency is181Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF160HZ      = 160,     ///< Accelerometer The accelerometer low-pass cutoff frequency is160Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF125HZ      = 125,     ///< Accelerometer The accelerometer low-pass cutoff frequency is125Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF100HZ      = 100,     ///< Accelerometer The accelerometer low-pass cutoff frequency is100Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF90HZ       = 90,      ///< Accelerometer The accelerometer low-pass cutoff frequency is90Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF80HZ       = 80,      ///< Accelerometer The accelerometer low-pass cutoff frequency is80Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF63HZ       = 63,      ///< Accelerometer The accelerometer low-pass cutoff frequency is63Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF50HZ       = 50,      ///< Accelerometer The accelerometer low-pass cutoff frequency is50Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF45HZ       = 45,      ///< Accelerometer The accelerometer low-pass cutoff frequency is45Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF40HZ       = 40,      ///< Accelerometer The accelerometer low-pass cutoff frequency is40Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF31HZ       = 31,      ///< Accelerometer The accelerometer low-pass cutoff frequency is31Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF25HZ       = 25,      ///< Accelerometer The accelerometer low-pass cutoff frequency is25Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF23HZ       = 23,      ///< Accelerometer The accelerometer low-pass cutoff frequency is23Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF20HZ       = 20,      ///< Accelerometer The accelerometer low-pass cutoff frequency is20Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF15HZ       = 15,      ///< Accelerometer The accelerometer low-pass cutoff frequency is15Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF13HZ       = 13,      ///< Accelerometer The accelerometer low-pass cutoff frequency is13Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF11HZ       = 11,      ///< Accelerometer The accelerometer low-pass cutoff frequency is11Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF10HZ       = 10,      ///< Accelerometer The accelerometer low-pass cutoff frequency is10Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF8HZ        = 8,       ///< Accelerometer The accelerometer low-pass cutoff frequency is8Hz
    GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_GYROLPF6HZ        = 6,       ///< Accelerometer The accelerometer low-pass cutoff frequency is6Hz
}GX_IMU_CONFIG_GYRO_ODR_LOW_PASS_FILTER_FREQUENCY_ENTRY;

typedef enum GX_IMU_TEMPERATURE_ODR_ENTRY
{
    GX_IMU_TEMPERATURE_ODR_500HZ						= 0,    ///< The output data rate of the thermometer is500Hz
    GX_IMU_TEMPERATURE_ODR_250HZ						= 1,    ///< The output data rate of the thermometer is250Hz 
    GX_IMU_TEMPERATURE_ODR_125HZ						= 2,    ///< The output data rate of the thermometer is125Hz
    GX_IMU_TEMPERATURE_ODR_63HZ							= 3,    ///< The output data rate of the thermometer is63Hz
}GX_IMU_TEMPERATURE_ODR_ENTRY;

typedef enum GX_SERIALPORT_SELECTOR_ENTRY
{
	GX_SERIALPORT_SERIALPORT_0							= 0,    ///< Serial port 0

}GX_SERIALPORT_SELECTOR_ENTRY;

typedef enum GX_SERIALPORT_SOURCE_ENTRY
{
	GX_SERIALPORT_SERIALPORT_SOURCE_OFF					= 0,		///< Serial port input source switch
	GX_SERIALPORT_SERIALPORT_SOURCE_LINE_0				= 1,		///< Serial port input source 0
	GX_SERIALPORT_SERIALPORT_SOURCE_LINE_1				= 2,		///< Serial port input source 1
	GX_SERIALPORT_SERIALPORT_SOURCE_LINE_2				= 3,		///< Serial port input source 2
	GX_SERIALPORT_SERIALPORT_SOURCE_LINE_3				= 4,		///< Serial port input source 3

}GX_SERIALPORT_SOURCE_ENTRY;

typedef enum GX_SERIALPORT_BAUNDRATE_ENTRY
{
	GX_SERIALPORT_BAUNDRATE_9600						= 5,    ///< Serial port baud rate is 9600Hz
	GX_SERIALPORT_BAUNDRATE_19200						= 6,    ///< Serial port baud rate is 19200Hz
	GX_SERIALPORT_BAUNDRATE_38400						= 7,    ///< Serial port baud rate is 38400Hz
	GX_SERIALPORT_BAUNDRATE_76800						= 8,    ///< Serial port baud rate is 76800Hz
	GX_SERIALPORT_BAUNDRATE_115200						= 9,    ///< Serial port baud rate is 115200Hz
}GX_SERIALPORT_BAUNDRATE_ENTRY;

typedef enum GX_SERIALPORT_STOP_BITS_ENTRY
{
	GX_SERIALPORT_STOP_BITS_ONE							= 0,    ///< Bit1
	GX_SERIALPORT_STOP_BITS_ONEANDHALF					= 1,    ///< Bit1AndHalf
	GX_SERIALPORT_STOP_BITS_TWO							= 2,    ///< Bit2
}GX_SERIALPORT_STOP_BITS_ENTRY;

typedef enum GX_SERIALPORT_PARITY_ENTRY
{
	GX_SERIALPORT_PARITY_NONE							= 0,    ///< None
	GX_SERIALPORT_PARITY_ODD							= 1,    ///< Odd number
	GX_SERIALPORT_PARITY_EVEN							= 2,    ///< even numbers
	GX_SERIALPORT_PARITY_MARK							= 3,    ///< sign
	GX_SERIALPORT_PARITY_SPACE							= 4,    ///< blank
}GX_SERIALPORT_PARITY_ENTRY;

//------------------------------------------------------------------------------
//  Structural Type Definition
//------------------------------------------------------------------------------

#define GX_INFO_LENGTH_8_BYTE   (8)  ///< 8 bytes
#define GX_INFO_LENGTH_32_BYTE  (32) ///< 32 bytes
#define GX_INFO_LENGTH_64_BYTE  (64) ///< 64 bytes
#define GX_INFO_LENGTH_128_BYTE (128)///< 128 bytes

typedef struct GX_DEVICE_IP_INFO 
{
    char szDeviceID[GX_INFO_LENGTH_64_BYTE + 4];         ///< the unique identifier of the device, 64+4 bytes
    char szMAC[GX_INFO_LENGTH_32_BYTE];                  ///< MAC address, 32 bytes,
    char szIP[GX_INFO_LENGTH_32_BYTE];                   ///< IP address, 32 bytes,
    char szSubNetMask[GX_INFO_LENGTH_32_BYTE];           ///< subnet mask, 32 bytes
    char szGateWay[GX_INFO_LENGTH_32_BYTE];              ///< gateway, 32 bytes
    char szNICMAC[GX_INFO_LENGTH_32_BYTE];               ///< the MAC address of the corresponding NIC(Network Interface Card), 32 bytes
    char szNICIP[GX_INFO_LENGTH_32_BYTE];                ///< the IP address of the corresponding NIC, 32 bytes
    char szNICSubNetMask[GX_INFO_LENGTH_32_BYTE];        ///< the subnet mask of the corresponding NIC, 32 bytes
    char szNICGateWay[GX_INFO_LENGTH_32_BYTE];           ///< the gateway of the corresponding NIC, 32 bytes
    char szNICDescription[GX_INFO_LENGTH_128_BYTE + 4];  ///< the description of the corresponding NIC, 128+4 bytes
    char reserved[512];                                  ///< reserved, 512 bytes
}GX_DEVICE_IP_INFO;

typedef struct GX_DEVICE_BASE_INFO 
{
    char szVendorName[GX_INFO_LENGTH_32_BYTE];           ///< vendor name, 32 bytes
    char szModelName[GX_INFO_LENGTH_32_BYTE];            ///< model name, 32 bytes
    char szSN[GX_INFO_LENGTH_32_BYTE];                   ///< device serial number, 32 bytes
    char szDisplayName[GX_INFO_LENGTH_128_BYTE + 4];     ///< device display name, 128+4 bytes
    char szDeviceID[GX_INFO_LENGTH_64_BYTE + 4];         ///< the unique identifier of the device, 64+4 bytes
    char szUserID[GX_INFO_LENGTH_64_BYTE + 4];           ///< user-defined name, 64+4 bytes
    GX_ACCESS_STATUS_CMD accessStatus;                   ///< access status that is currently supported by the device. Refer to GX_ACCESS_STATUS, 4 bytes
    GX_DEVICE_CLASS      deviceClass;                    ///< device type, such as USB2.0, GEV, 4 bytes
    char reserved[300];                                  ///< reserved, 300 bytes
}GX_DEVICE_BASE_INFO;

typedef struct GX_OPEN_PARAM 
{
    char              *pszContent;                       ///< Standard C string that is decided by openMode. It could be an IP address, a camera serial number, and so on
    GX_OPEN_MODE_CMD   openMode;                         ///< Device open mode. The device can be open via the SN, IP, MAC, etc. Please refer to GX_OPEN_MODE
    GX_ACCESS_MODE_CMD accessMode;                       ///< Device access mode, such as read-only, control, exclusive, etc. Please refer to GX_ACCESS_MODE
}GX_OPEN_PARAM;




// void            *pUserParam;  ———— 用户私有数据指针，可以用于在回调函数中传递自定义的额外参数
// GX_FRAME_STATUS status;       ———— 回调函数返回的图像状态，请参考 GX_FRAME_STATUS
// const void      *pImgBuf;     ———— 图像数据地址（在启用帧信息后，pImgBuf 包含图像数据和帧信息数据）
// int32_t         nImgSize;     ———— 数据大小，以字节为单位（在启用帧信息后，nImgSize 是图像数据大小和帧信息大小的总和）
// int32_t         nWidth;       ———— 图像宽度
// int32_t         nHeight;      ———— 图像高度
// int32_t         nPixelFormat; ———— 图像的像素格式
// uint64_t        nFrameID;     ———— 图像的帧标识
// uint64_t        nTimestamp;   ———— 图像的时间戳
// int32_t         nOffsetX;     ———— 图像的X偏移量
// int32_t         nOffsetY;     ———— 图像的Y偏移量
// int32_t         reserved[1];  ———— 4字节，保留字段
typedef struct GX_FRAME_CALLBACK_PARAM
{
    void           *pUserParam;                          ///< User's private data pointer
    GX_FRAME_STATUS status;                              ///< The image state returned by the callback function. Please refer to GX_FRAME_STATUS
    const void     *pImgBuf;                             ///< The image data address (After the frame information is enabled, the pImgBuf contains image data and frame information data)
    int32_t         nImgSize;                            ///< Data size, in bytes (After the frame information is enabled, nImgSize is the sum of the size of the image data and the size of the frame information)
    int32_t         nWidth;                              ///< Image width
    int32_t         nHeight;                             ///< Image height
    int32_t         nPixelFormat;                        ///< PixelFormat of image
    uint64_t        nFrameID;                            ///< Frame identification of image
    uint64_t        nTimestamp;                          ///< Timestamp of image
    int32_t         nOffsetX;                            ///< X-direction offset of the image
    int32_t         nOffsetY;                            ///< Y-direction offset of the image
    int32_t         reserved[1];                         ///< 4 bytes,reserved
}GX_FRAME_CALLBACK_PARAM;

typedef struct GX_FRAME_DATA
{
    GX_FRAME_STATUS nStatus;                             ///< The state of the acquired image. Please refer to GX_FRAME_STATUS
    void           *pImgBuf;                             ///< The image data address (After the frame information is enabled, the pImgBuf contains image data and frame information data)
    int32_t         nWidth;                              ///< Image width
    int32_t         nHeight;                             ///< Image height
    int32_t         nPixelFormat;                        ///< Pixel format of image
    int32_t         nImgSize;                            ///< Data size (After the frame information is enabled, nImgSize is the sum of the size of the image data and the size of the frame information)
    uint64_t        nFrameID;                            ///< Frame identification of image
    uint64_t        nTimestamp;                          ///< Timestamp of image
    int32_t         nOffsetX;                            ///< X-direction offset of the image
    int32_t         nOffsetY;                            ///< Y-direction offset of the image
    int32_t         reserved[1];                         ///< 4 bytes,reserved
}GX_FRAME_DATA;

typedef struct GX_FRAME_BUFFER
{
    GX_FRAME_STATUS nStatus;                             ///< The state of the acquired image. Please refer to GX_FRAME_STATUS
    void           *pImgBuf;                             ///< The image data pointer (After the frame information is enabled, the pImgBuf contains image data and frame information data)
    int32_t         nWidth;                              ///< Image width
    int32_t         nHeight;                             ///< Image height
    int32_t         nPixelFormat;                        ///< Pixel format of image
    int32_t         nImgSize;                            ///< Data size, in bytes (After the frame information is enabled, nImgSize is the sum of the size of the image data and the size of the frame information)
    uint64_t        nFrameID;                            ///< Frame identification of image
    uint64_t        nTimestamp;                          ///< Timestamp of image
    uint64_t        nBufID;                              ///< BufID
    int32_t         nOffsetX;                            ///< X-direction offset of the image
    int32_t         nOffsetY;                            ///< Y-direction offset of the image
    int32_t         reserved[16];                        ///< 64 bytes,reserved
}GX_FRAME_BUFFER;

typedef struct GX_INT_RANGE
{
    int64_t nMin;                                        ///< Minimum value
    int64_t nMax;                                        ///< Maximum value
    int64_t nInc;                                        ///< Step size
    int32_t reserved[8];                                 ///< 32 bytes,reserved
}GX_INT_RANGE;

typedef struct GX_FLOAT_RANGE
{
    double dMin;                                         ///< Minimum value
    double dMax;                                         ///< Maximum value
    double dInc;                                         ///< Step size
    char   szUnit[GX_INFO_LENGTH_8_BYTE];                ///< Unit. 8 bytes
    bool   bIncIsValid;                                  ///< Indicates whether the step size is supported, 1 byte
    int8_t reserved[31];                                 ///< 31 bytes,reserved
}GX_FLOAT_RANGE;

typedef struct GX_ENUM_DESCRIPTION
{
    int64_t nValue;                                      ///< The value of the enumeration item
    char    szSymbolic[GX_INFO_LENGTH_64_BYTE];          ///< The character description information of the enumeration item, 64 bytes
    int32_t reserved[8];                                 ///< 32 bytes,reserved
}GX_ENUM_DESCRIPTION;

//Frame buffer pointer type
typedef GX_FRAME_BUFFER* PGX_FRAME_BUFFER;

//------------------------------------------------------------------------------
//  Callback function type definition
//------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
\brief     Capture callback function type
\param     pFrameData    The pointer to the address that the user introduced to receive the image data
\return    void
*/
//----------------------------------------------------------------------------------
typedef void (GX_STDC *GXCaptureCallBack) (GX_FRAME_CALLBACK_PARAM *pFrameData);
//----------------------------------------------------------------------------------
/**
\brief     Device offline callback function type
\param     pUserParam    User private parameter
\return    void
*/
//----------------------------------------------------------------------------------
typedef void (GX_STDC *GXDeviceOfflineCallBack) (void *pUserParam);
//----------------------------------------------------------------------------------
/**
\brief     Device attribute update callback function type
\param     nFeatureID    FeatureID
\param     pUserParam    User private parameter
\return    void
*/
//----------------------------------------------------------------------------------
typedef void (GX_STDC *GXFeatureCallBack) (GX_FEATURE_ID_CMD nFeatureID, void *pUserParam);

//------------------------------------------------------------------------------
//  Standard C API Function Definition
//------------------------------------------------------------------------------
#define GX_API GX_EXTC GX_STATUS GX_STDC GX_DLLEXPORT

//----------------------------------------------------------------------------------
/**
\brief      Gets the library version number.
\return     const char*   Library version number of string type.
*/
//----------------------------------------------------------------------------------
GX_EXTC GX_DLLEXPORT const char *GX_STDC GXGetLibVersion ();

//------------------------------------------------------------------------
/**
\brief      Initialize the device library for some resource application operations. This interface must be called
            before using the GxIAPI to interact with the camera, and the GXCloseLib must be called to release all
            the resources when the GxIAPI is stopped for all control of the device.
\attention  Before calling the other interfaces (except GXCloseLib/ GXGetLastError), you must call the GXInitLib
            interface for initialization first, otherwise the error GX_STATUS_NOT_INIT_API will return.
\return     GX_STATUS_SUCCESS             The operation is successful, no error occurs.
            GX_STATUS_NOT_FOUND_TL        Can not found the library.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//------------------------------------------------------------------------
GX_API GXInitLib ();

//----------------------------------------------------------------------------------
/**
\brief      Close the device library to release resources. You must to call this interface to release resources
            when the GxIAPI stopped all the controls of the device. Corresponding to the GXInitLib.
\return     GX_STATUS_SUCCESS             The operation is successful and no error occurs.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXCloseLib ();

//------------------------------------------------------------------------
/**
\brief      To get the latest error descriptions information of the program.
\param      [out] pErrorCode      Return the last error code. You could set the parameter to NULL if you don't need this value.
\param      [out] pszErrText      Return the address of the buffer allocated for error information.
\param      [in,out] pSize        The address size of the buffer allocated for error information. Unit: byte.
                                  If pszErrText is NULL:
                                  [out]pnSize   Return the actual required buffer size.
                                  If pszErrText is not NULL:
                                  [in]pnSize    It is the actual allocated buffer size.
                                  [out]pnSize   Return the actual allocated buffer size.
\return     GX_STATUS_SUCCESS              The operation is successful and no error occurs.
            GX_STATUS_INVALID_PARAMETER    The pointer that the user input is NULL.
            GX_STATUS_NEED_MORE_BUFFER     The buffer that the user filled is too small.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//------------------------------------------------------------------------
GX_API GXGetLastError (GX_STATUS *pErrorCode, char *pszErrText, size_t *pSize);

//----------------------------------------------------------------------------------
/**
\brief      Enumerating currently all available devices in subnet and gets the number of devices.
\param      [out] punNumDevices   The address pointer used to return the number of devices, and the pointer can not be NULL.
\param      [in] unTimeOut        The timeout time of enumeration (unit: ms). If the device is successfully
                                  enumerated within the specified timeout time, the value returns immediately.
                                  If the device is not enumerated within the specified timeout time, then it
                                  waits until the specified timeout time is over and then it returns.
\return     GX_STATUS_SUCCESS              The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API         The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER    The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXUpdateDeviceList (uint32_t *punNumDevices, uint32_t nTimeOut);

//----------------------------------------------------------------------------------
/**
\brief      Enumerating currently all available devices in entire network and gets the number of devices.
\param      [out] punNumDevices   The address pointer used to return the number of devices, and the pointer can not be NULL.
\param      [in] unTimeOut        The timeout time of enumeration (unit: ms). If the device is successfully
                                  enumerated within the specified timeout time, the value returns immediately.
                                  If the device is not enumerated within the specified timeout time, then it
                                  waits until the specified timeout time is over and then it returns.
\return     GX_STATUS_SUCCESS             The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API        The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER   The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXUpdateAllDeviceList (uint32_t *punNumDevices, uint32_t nTimeOut);

//----------------------------------------------------------------------------------
/**
\brief      To get the basic information of all devices.
\attention  You should call the GxUpdateDeviceList() interface for an enumeration before calling the function to
            get the device information. Otherwise, the device information that the user gets is inconsistent with
            the device that is currently connected.
\param      [out] pDeviceInfo     The structure pointer of the device information.
\param      [in,out]pBufferSize   The buffer size of device information structure, unit: byte.
                                  If pDeviceInfo is NULL:
                                  [out]pnBufferSize  Return the actual size of the device information.
                                  If pDeviceInfo is not NULL:
                                  [in]pnBufferSize   The size of the buffer that the user allocated.
                                  [out]pnBufferSize  Return the actual allocated buffer size.
\return     GX_STATUS_SUCCESS             The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API        The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER   The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetAllDeviceBaseInfo (GX_DEVICE_BASE_INFO *pDeviceInfo, size_t *pBufferSize);

//----------------------------------------------------------------------------------
/**
\brief      To get the network information of all devices.
\attention  You should call the GxUpdateDeviceList() interface for an enumeration before calling the function to
            get the device information. Otherwise, the device information that the user gets is inconsistent with
            the device that is currently connected.
\param      [in]  nIndex          The serial number of the device.
\param      [out] pstDeviceIPInfo The structure pointer of the device information.
\return     GX_STATUS_SUCCESS             The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API        The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER   The index that the user input is cross the border.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetDeviceIPInfo (uint32_t nIndex, GX_DEVICE_IP_INFO *pstDeviceIPInfo);

//----------------------------------------------------------------------------------
/**
\brief      Open the device by index, starting from 1.
\param      nDeviceIndex          The index of the device starts from 1, for example: 1, 2, 3, 4...
\param      phDevice              Device handle returned by the interface.
\return     GX_STATUS_SUCCESS             The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API        The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER   The pointer that the user input is NULL.
            GX_STATUS_OUT_OF_RANGE        The index of the user input is bigger than the available devices number.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXOpenDeviceByIndex (uint32_t nDeviceIndex, GX_DEV_HANDLE *phDevice);

//----------------------------------------------------------------------------------
/**
\brief      Open the device by a specific unique identification, such as: SN, IP, MAC, Index etc.
\attention  It is recommended that you call the GxUpdateDeviceList() interface to make an enumeration before calling
            the function. To ensure that device list within the library is consistent with the current device.
\param      [in]pOpenParam          The open device parameter which is configurated by user. Ref:GX_OPEN_PARAM.
\param      [out]phDevice           The device handle returned by the interface.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_NOT_FOUND_DEVICE      Not found the device that matches the specific information.
            GX_STATUS_INVALID_ACCESS        The device can not be opened under the current access mode.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXOpenDevice (GX_OPEN_PARAM *pOpenParam, GX_DEV_HANDLE *phDevice);

//----------------------------------------------------------------------------------
/**
\brief      Specify the device handle to close the device.
\attention  Close the device handle that has been closed, return the GX_STATUS_INVALID_HANDLE error.
\param      [in]hDevice             The device handle that the user specified to close. The hDevice can be get by GXOpenDevice interface.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The illegal handle that the user introduces, or reclose the device.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXCloseDevice (GX_DEV_HANDLE hDevice);

//----------------------------------------------------------------------------------
/**
\brief      Get the string description for the feature code.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pszName            The character string buffer address that the user inputs. The character string
                                    length includes the end terminator '\0'.
\param      [in,out]pnSize          The length of the character string buffer address that the user inputs. Unit: byte.
                                    If pszName is NULL:
                                    [out]pnSize   Return the actual size of the character string.
                                    If pszName is not NULL:
                                    [in]pnSize    The size of the buffer that the user allocated.
                                    [out]pnSize   Return the actual filled buffer size.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_NEED_MORE_BUFFER      The buffer that the user allocated is too small.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetFeatureName (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, char *pszName, size_t *pnSize);

//----------------------------------------------------------------------------------
/**
\brief      Inquire the current camera whether support a special feature. Usually the camera does not support a
            feature means that:
            1) By inquiring the camera register, the current camera really does not support this feature.
            2) There is no description of this feature in the current camera description file.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pbIsImplemented    To return the result whether is support this feature. If support, then returns
                                    true, if not support, false will return.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXIsImplemented (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, bool *pbIsImplemented);

//----------------------------------------------------------------------------------
/**
\brief      Inquire if a feature code is currently readable.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pbIsReadable       To return the result whether the feature code ID is readable. If readable,
                                    then will return true, if not readable, false will return.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXIsReadable (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, bool *pbIsReadable);

//----------------------------------------------------------------------------------
/**
\brief      Inquire if a feature code is currently writable.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pbIsWritable       To return the result whether the feature code ID is writable. If writable,
                                    then will return true, if not writable, false will return.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXIsWritable (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, bool *pbIsWritable);

//----------------------------------------------------------------------------------
/**
\brief      To get the minimum value, maximum value and steps of the int type.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pIntRange          The structure of range description. Reference GX_INT_RANGE.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not read the int range.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetIntRange (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, GX_INT_RANGE *pIntRange);

//----------------------------------------------------------------------------------
/**
\brief      Get the current value of the int type.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pnValue            Point to the pointer of the current value returned.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not read.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetInt (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, int64_t *pnValue);

//----------------------------------------------------------------------------------
/**
\brief      Set the value of int type.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [in]pnValue             The value that the user will set.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_OUT_OF_RANGE          The value that the user introduces is across the border, smaller
                                            than the minimum, or larger than the maximum, or is not an integer
                                            multiple of the step.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXSetInt (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, int64_t nValue);

//----------------------------------------------------------------------------------
/**
\brief      To get the minimum value, maximum value, steps and unit of the float type.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pFloatRange        The description structure pointer of float type. Reference the GX_FLOAT_RANGE.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not read the range of the float type.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetFloatRange (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, GX_FLOAT_RANGE *pFloatRange);

//----------------------------------------------------------------------------------
/**
\brief      Set the value of float type.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [in]dValue              The float value that the user will set.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_OUT_OF_RANGE          The value that the user introduces is across the border, smaller
                                            than the minimum, or larger than the maximum, or is not an integer
                                            multiple of the step.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXSetFloat (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, double dValue);

//----------------------------------------------------------------------------------
/**
\brief      Get the value of float type.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pdValue            Point to the pointer of the float value returned.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetFloat (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, double *pdValue);

//----------------------------------------------------------------------------------
/**
\brief      Get the number of the options for the enumeration item.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pnEntryNums        The pointer that point to the number returned.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetEnumEntryNums (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, uint32_t *pnEntryNums);

//----------------------------------------------------------------------------------
/**
\brief      To get the description information of the enumerated type values: the number of enumerated items
            and the value and descriptions of each item, please reference GX_ENUM_DESCRIPTION.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pEnumDescription   The array pointer, used for the enumeration description information returned.
\param      [in,out]pBufferSize     The size of the GX_ENUM_DESCRIPTION array that the user introduces, unit: byte.
                                    If pEnumDescription is NULL:
                                    [out]pnBufferSize   The actual size of the buffer needed.
                                    If pEnumDescription is not NULL:
                                    [in]pnBufferSize   The size of the buffer that the user allocated.
                                    [out]pnBufferSize  Return the actual filled buffer size.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_NEED_MORE_BUFFER      The buffer that the user allocates is too small.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetEnumDescription (GX_DEV_HANDLE hDevice, 
                             GX_FEATURE_ID_CMD featureID, 
                             GX_ENUM_DESCRIPTION *pEnumDescription,
                             size_t *pBufferSize);

//----------------------------------------------------------------------------------
/**
\brief      To get the current enumeration value.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pnValue            The pointer that point to the return values.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetEnum (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, int64_t *pnValue);

//----------------------------------------------------------------------------------
/**
\brief      Set the enumeration value.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [in]nValue              The enumeration values that the user will set. The value range can be got
                                    by the nValue of the GX_ENUM_DESCRIPTION.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXSetEnum (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, int64_t nValue);

//----------------------------------------------------------------------------------
/**
\brief      Get the value of bool type.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pbValue            The pointer that point to the bool value returned.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetBool (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, bool *pbValue);

//----------------------------------------------------------------------------------
/**
\brief      Set the value of bool type.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [in]bValue              The bool value that the user will set.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXSetBool (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, bool bValue);

//----------------------------------------------------------------------------------
/**
\brief      Get the current value length of the character string type. Unit: byte. The user can allocate the buffer size
            according to the length information that is get from the function, and then call the GXGetString to get the
            character string information.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pnSize             The pointer that point to the length value returned. The length value is end with '\0', unit: byte.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetStringLength (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, size_t *pnSize);

//----------------------------------------------------------------------------------
/**
\brief      Get the maximum length of the string type value. Unit: byte. The user allocates buffer according to
            the length information obtained, then call the GXGetString to get the string information. This interface
            can get the maximum possible length of the string (including the terminator '\ 0'), but the actual length
            of the string might not be that long, if the user wants to allocate buffer according to the actual string
            length, the user can call the GXGetStringLength interface to get the actual string length.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pnSize             The pointer that point to the length value returned. The length value is end with '\0', unit: byte.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetStringMaxLength (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, size_t *pnSize);

//----------------------------------------------------------------------------------
/**
\brief      Get the content of the string type value.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pszContent         Point to the string buffer address that the user allocated.
\param      [in,out]pnSize          The length of the string buffer address that the user inputs.
                                    If pszContent is NULL:
                                    [out]pnSize  Return the actual size of the buffer needed.
                                    If pszContent is not NULL:
                                    [in]pnSize  The size of the buffer that the user allocated.
                                    [out]pnSize Return the actual filled buffer size.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            GX_STATUS_NEED_MORE_BUFFER      The buffer that the user allocates is too small.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetString (GX_DEV_HANDLE hDevice, 
                    GX_FEATURE_ID_CMD featureID, 
                    char *pszContent,
                    size_t *pnSize);

//----------------------------------------------------------------------------------
/**
\brief      Set the content of the string value.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [in]pszContent          The string address that the user will set. The string is end with '\0'.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user introduces is NULL.
            GX_STATUS_OUT_OF_RANGE          The maximum length that the content the user writes
                                            exceeds the string size.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXSetString (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, char *pszContent);

//----------------------------------------------------------------------------------
/**
\brief      Get the length of the chunk data and the unit is byte, the user can apply the buffer based on the
            length obtained, and then call the GXGetBuffer to get the chunk data.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pnSize             The pointer that points to the length value returned. Unit: byte.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetBufferLength (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID, size_t *pnSize);

//----------------------------------------------------------------------------------
/**
\brief      Get the chunk data.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pBuffer            The pointer that point to the chunk data buffer address that the user applied.
\param      [in,out]pnSize          The length of the buffer address that the user inputs.
                                    If pBuffer is NULL:
                                    [out]pnSize  Return the actual size of the buffer needed.
                                    If pBuffer is not NULL:
                                    [in]pnSize   The size of the buffer that the user allocated.
                                    [out]pnSize  Return the actual filled buffer size.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            GX_STATUS_NEED_MORE_BUFFER      The buffer that the user allocates is too small.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetBuffer (GX_DEV_HANDLE hDevice, 
                    GX_FEATURE_ID_CMD featureID, 
                    uint8_t *pBuffer,
                    size_t *pnSize);

//----------------------------------------------------------------------------------
/**
\brief      Set the chunk data.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [in]pBuffer             The pointer that point to the chunk data buffer address that the user will set.
\param      [in]nSize               The length of the buffer address that the user inputs.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user introduces is NULL.
            GX_STATUS_OUT_OF_RANGE          The maximum length that the content the user writes exceeds
                                            the string size.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXSetBuffer (GX_DEV_HANDLE hDevice, 
                    GX_FEATURE_ID_CMD featureID, 
                    uint8_t *pBuffer,
                    size_t nSize);

//----------------------------------------------------------------------------------
/**
\brief      Send the command.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_NOT_IMPLEMENTED       The feature that is not support currently.
            GX_STATUS_ERROR_TYPE            The featureID type that the user introduces is error.
            GX_STATUS_INVALID_ACCESS        Currently inaccessible, can not write.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXSendCommand (GX_DEV_HANDLE hDevice, GX_FEATURE_ID_CMD featureID);

//----------------------------------------------------------------------------------
/**
\brief      Set the number of the acquisition buffers.
\param      [in]hDevice             The handle of the device.
\param      [in]nBufferNum          The number of the acquisition buffers that the user sets.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The input parameter that the user introduces is invalid.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXSetAcqusitionBufferNumber (GX_DEV_HANDLE hDevice, uint64_t nBufferNum);

//----------------------------------------------------------------------------------
/**
\brief      Get the number of the acquisition buffers.
\param      [in]hDevice             The handle of the device.
\param      [out]pBufferNum         Current number of the acquisition buffers.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The input parameter that the user introduces is invalid.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetAcqusitionBufferNumber (GX_DEV_HANDLE hDevice, uint64_t *pBufferNum);

//----------------------------------------------------------------------------------
/**
\brief      Start acquisition, including stream acquisition and device acquisition.
\param      [in]hDevice             The handle of the device.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_ACCESS        Device access mode error.
            GX_STATUS_ERROR                 Unspecified internal errors that are not expected to occur.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXStreamOn (GX_DEV_HANDLE hDevice);

//----------------------------------------------------------------------------------
/**
\brief      Stop acquisition, including stop stream acquisition and stop device acquisition.
\param      [in]hDevice             The handle of the device.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_ACCESS        Device access mode error.
            GX_STATUS_INVALID_CALL          Acquisition is not started or the callback is registered. It
                                            is not allowed to call the interface.
            GX_STATUS_ERROR                 Unspecified internal errors that are not expected to occur.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXStreamOff (GX_DEV_HANDLE hDevice);

//----------------------------------------------------------------------------------
/**
\brief      After starting the acquisition, an image (zero copy) can be acquired through this interface.
\param      [in]hDevice             The handle of the device.
\param      [out]ppFrameBuffer      Address pointer of image data output by the interface.
\param      [in]nTimeOut            Take timeout time (unit: ms).
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user introduces is NULL.
            GX_STATUS_INVALID_CALL          Acquisition is not started or the callback is registered. It
                                            is not allowed to call the interface.
            GX_STATUS_TIMEOUT               Acquire image timeout error.
            GX_STATUS_ERROR                 Unspecified internal errors that are not expected to occur.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXDQBuf (GX_DEV_HANDLE hDevice, 
                PGX_FRAME_BUFFER *ppFrameBuffer,
                uint32_t nTimeOut);

//----------------------------------------------------------------------------------
/**
\brief      After the acquisition is started, the image data buffer can be placed back into the GxIAPI
            library through this interface and continue to be used for acquisition.
\param      [in]hDevice             The handle of the device.
\param      [in]pFrameBuffer        Image data buffer pointer to be placed back into the GxIAPI library.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user introduces is NULL.
            GX_STATUS_INVALID_CALL          Acquisition is not started or the callback is registered. It
                                           is not allowed to call the interface.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXQBuf (GX_DEV_HANDLE hDevice, PGX_FRAME_BUFFER pFrameBuffer);

//----------------------------------------------------------------------------------
/**
\brief      After starting the acquisition, all the buffers (zero copies) of the acquired images can be
            obtained through this interface. The order of the stored images in the image data array is
            from old to new, that is, ppFrameBufferArray[0] stores the oldest image, and
            ppFrameBufferArray[nFrameCount - 1] stores the latest image.
\param      [in]hDevice                     The handle of the device.
\param      [out]ppFrameBufferArray         Array of image data pointers.
\param      [in]nFrameBufferArraySize       The number of applications for image arrays.
\param      [out]pnFrameCount               Returns the number of actual filled images.
\param      [in]nTimeOut                    Take timeout time (unit: ms).
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user introduces is NULL.
            GX_STATUS_INVALID_CALL          Acquisition is not started or the callback is registered. It
                                            is not allowed to call the interface.
            GX_STATUS_NEED_MORE_BUFFER      Insufficient buffer requested by the user: When reading,the user
                                            input buffer size is smaller than the actual need.
            GX_STATUS_TIMEOUT               Acquire image timeout error.
            GX_STATUS_ERROR                 Unspecified internal errors that are not expected to occur.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXDQAllBufs (GX_DEV_HANDLE hDevice, 
                    PGX_FRAME_BUFFER *ppFrameBufferArray, 
                    uint32_t nFrameBufferArraySize, 
                    uint32_t *pnFrameCount,
                    uint32_t nTimeOut);

//----------------------------------------------------------------------------------
/**
\brief      After the acquisition is started, all the acquired image data buffers can be put back into
            the GxIAPI library through this interface, and continue to be used for acquisition.
\param      [in]hDevice             The handle of the device.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_CALL          Invalid interface call.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXQAllBufs (GX_DEV_HANDLE hDevice);

//----------------------------------------------------------------------------------
/**
\brief      Register the capture callback function, corresponding to GXUnregisterCaptureCallback.
\attention  After sending the start acquisition command, the user can not register the callback function.
            Otherwise, it will return GX_STATUS_INVALID_CALL.
\param      [in]hDevice             The handle of the device.
\param      [in]pUserParam          The private data pointer that the user will use in the callback function.
\param      [in]callBackFun         The callback function that the user will register, for the function type,
                                    see GXCaptureCallBack.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user introduces is NULL.
            GX_STATUS_INVALID_CALL          After sending the start acquisition command, the user can
                                            not register the capture callback function.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXRegisterCaptureCallback (GX_DEV_HANDLE hDevice, void *pUserParam, GXCaptureCallBack callBackFun);

//----------------------------------------------------------------------------------
/**
\brief      Unregister the capture callback function, corresponding to GXRegisterCaptureCallback.
\attention  Before sending the stop acquisition command, the user can not unregister the callback function.
            Otherwise, it will return GX_STATUS_INVALID_CALL.
\param      [in]hDevice             The handle of the device.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_CALL          After sending the stop acquisition command, the user can not unregister
                                            the capture callback function.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXUnregisterCaptureCallback (GX_DEV_HANDLE hDevice);

//----------------------------------------------------------------------------------
/**
\brief      After starting acquisition, you can call this function to get images directly. Noting that the
            interface can not be mixed with the callback capture mode.
\attention  The GXGetImage interface is not allowed to be called after the capture callback function is registered,
            and the call will return GX_STATUS_INVALID_CALL error. When using high resolution cameras for high-speed
            acquisition, because there is a buffer copy within the GXGetImage interface, it will affect the transport
            performance. It is recommended that users use the capture callback mode in this case.
\param      [in]hDevice             The handle of the device.
\param      [in,out]pFrameData      The pointer to the address that the user introduced to receive the image data.
\param      [in]nTimeout            The timeout time of capture image (unit: ms).
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_CALL          After registering the capture callback function, the user calls the GXGetImage
                                            to get image.
            GX_STATUS_INVALID_PARAMETER     User incoming image address pointer is NULL.
            GX_STATUS_NEED_MORE_BUFFER      Insufficient buffer requested by the user: When reading,the user
                                            input buffer size is smaller than the actual need.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetImage (GX_DEV_HANDLE hDevice, GX_FRAME_DATA *pFrameData, uint32_t nTimeout);

//----------------------------------------------------------------------------------
/**
\brief      Empty the cache image in the image output queue.
\attention  If the user processes the images too slow, the image of last acquisition may be remained in the queue.
            Especially in the trigger mode, after the user send the trigger signal, and get the old image (last
            image). If you want to get the current image that corresponding to trigger signal, you should call the
            GXFlushQueue interface before sending the trigger signal to empty the image output queue.
\param      [in]hDevice             The handle of the device.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXFlushQueue (GX_DEV_HANDLE hDevice);

//----------------------------------------------------------------------------------
/**
\brief      At present, the Mercury Gigabit camera provides the device offline notification event mechanism, the
            user can call this interface to register the event handle callback function.
\param      [in]hDevice             The handle of the device.
\param      [in]pUserParam          User private parameter.
\param      [in]callBackFun         The user event handle callback function, for the function type, see GXDeviceOfflineCallBack.
\param      [in]pHCallBack          The handle of offline callback function, the handle is used for unregistering the callback function.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The unsupported event ID or the callback function is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXRegisterDeviceOfflineCallback (GX_DEV_HANDLE hDevice, 
                                        void *pUserParam,
                                        GXDeviceOfflineCallBack callBackFun, 
                                        GX_EVENT_CALLBACK_HANDLE *pHCallBack);

//----------------------------------------------------------------------------------
/**
\brief      Unregister event handle callback function.
\param      [in]hDevice             The handle of the device.
\param      [in]hCallBack           The handle of device offline callback function.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXUnregisterDeviceOfflineCallback (GX_DEV_HANDLE hDevice, GX_EVENT_CALLBACK_HANDLE hCallBack);

//----------------------------------------------------------------------------------
/**
\brief      Empty the device event, such as the frame exposure to end the event data queue.
\attention  The library internal event data receiving and processing using caching mechanism, if the user
            receiving, processing event speed is slower than the event generates, then the event data will be
            accumulated in the library, it will affect the the user to get real-time event data. If you want
            to get the real-time event data, you need to call the GXFlushEvent interface to clear the event
            cache data. This interface empties all the event data at once.
\param      [in]hDevice             The handle of the device.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXFlushEvent (GX_DEV_HANDLE hDevice);

//----------------------------------------------------------------------------------
/**
\brief      Get the number of the events in the current remote device event queue cache.
\param      [in]hDevice             The handle of the device.
\param      [in]pnEventNum          The pointer of event number.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetEventNumInQueue (GX_DEV_HANDLE hDevice, uint32_t *pnEventNum);

//----------------------------------------------------------------------------------
/**
\brief      Register device attribute update callback function. When the current value of the device property
            has updated, or the accessible property is changed, call this callback function.
\param      [in]hDevice             The handle of the device.
\param      [in]pUserParam          User private parameter.
\param      [in]callBackFun         The user event handle callback function, for function type, see GXFeatureCallBack.
\param      [in]featureID           The feature code ID.
\param      [out]pHCallBack         The handle of property update callback function, to unregister the callback function.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            GX_STATUS_INVALID_PARAMETER     The unsupported event ID or the callback function is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXRegisterFeatureCallback (GX_DEV_HANDLE hDevice, 
                                  void *pUserParam,
                                  GXFeatureCallBack callBackFun, 
                                  GX_FEATURE_ID_CMD featureID,
                                  GX_FEATURE_CALLBACK_HANDLE *pHCallBack);

//----------------------------------------------------------------------------------
/**
\brief      Unregister device attribute update callback function.
\param      [in]hDevice             The handle of the device.
\param      [in]featureID           The feature code ID.
\param      [out]pHCallBack         The attribute update callback function handle.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXUnregisterFeatureCallback (GX_DEV_HANDLE hDevice, 
                                    GX_FEATURE_ID_CMD featureID, 
                                    GX_FEATURE_CALLBACK_HANDLE hCallBack);

//----------------------------------------------------------------------------------
/**
\brief      Export the current parameter of the camera to the configuration file.(ANSI)
\param      [in]hDevice             The handle of the device.
\param      [in]pszFilePath         The path of the configuration file that to be generated.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXExportConfigFile (GX_DEV_HANDLE hDevice, const char *pszFilePath);

//----------------------------------------------------------------------------------
/**
\brief      Import the configuration file for the camera.(ANSI)
\param      [in]hDevice             The handle of the device.
\param      [in]pszFilePath         The path of the configuration file.
\param      [in]bVerify             If bVerify is true, all imported values will be read out to verify consistency.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
#ifndef __cplusplus
GX_API GXImportConfigFile(GX_DEV_HANDLE hDevice, const char *pszFilePath, bool bVerify);
#else
GX_API GXImportConfigFile(GX_DEV_HANDLE hDevice, const char *pszFilePath, bool bVerify = false);
#endif

//----------------------------------------------------------------------------------
/**
\brief      Read the value of the specified register
\param      [in]hDevice             The handle of the device.
\param      [in]ui64Address         Register address.
\param      [out]pBuffer            Return the value of the register, can not be NULL.
\param      [in, out]piSize         [in]The size of the buffer that the user allocated.
                                    [out]Return the actual filled buffer size.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXReadRemoteDevicePort (GX_DEV_HANDLE hDevice, 
                               uint64_t ui64Address, 
                               void *pBuffer,
                               size_t *piSize);

//----------------------------------------------------------------------------------
/**
\brief      Write the given data to the specified register.
\param      [in]hDevice             The handle of the device.
\param      [in]ui64Address         Register address.
\param      [in]pBuffer             The value to be written to the register, cannot be NULL.
\param      [in, out]piSize         [in]Buffer size to be written to the register.
                                    [out]Returns the size actually written to the register.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXWriteRemoteDevicePort (GX_DEV_HANDLE hDevice, 
                                uint64_t ui64Address, 
                                const void *pBuffer,
                                size_t *piSize);

//----------------------------------------------------------------------------------
/**
\brief      Get the persistent IP information of the device.
\param      [in]       hDevice                  The handle of the device.
\param      [in]       pszIP                    The character string address of the device persistent IP.
\param      [in, out]  pnIPLength               The character string length of the device persistent IP address.
\param      [in]       pnIPLength:              The user buffer size.
\param      [out]      pnIPLength:              The actual filled buffer size.
\param      [in]       pszSubNetMask            The device persistent subnet mask character string address.
\param      [in, out]  pnSubNetMaskLength       The character string length of the device persistent subnet mask.
\param      [in]       pnSubNetMaskLength:      The user buffer size.
\param      [out]      pnSubNetMaskLength:      The actual filled buffer size.
\param      [in]       pszDefaultGateWay        The character string address of the device persistent gateway.
\param      [in, out]  pnDefaultGateWayLength   The character string length of the device persistent gateway.
\param      [in]       pnDefaultGateWayLength:  The user buffer size.
\param      [out]      pnDefaultGateWayLength:  The actual filled buffer size.
\return     GX_STATUS_SUCCESS                   The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API              The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER         The pointer that the user input is NULL.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXGetDevicePersistentIpAddress (GX_DEV_HANDLE hDevice, 
                                       char   *pszIP,
                                       size_t *pnIPLength,
                                       char   *pszSubNetMask,
                                       size_t *pnSubNetMaskLength,
                                       char   *pszDefaultGateWay,
                                       size_t *pnDefaultGateWayLength);

//----------------------------------------------------------------------------------
/**
\brief      Set the persistent IP information of the device.
\param      [in]hDevice             The handle of the device.
\param      [in]pszIP               The persistent IP character string of the device. End with'\0'.
\param      [in]pszSubNetMask       The persistent subnet mask character string of the device. End with'\0'.
\param      [in]pszDefaultGateWay   The persistent gateway character string of the device. End with'\0'.
\return     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_HANDLE        The handle that the user introduces is illegal.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
//----------------------------------------------------------------------------------
GX_API GXSetDevicePersistentIpAddress (GX_DEV_HANDLE hDevice,
                                       const char *pszIP,
                                       const char *pszSubNetMask,
                                       const char *pszDefaultGateWay);

// ---------------------------------------------------------------------------
/**
\brief      Configure the static IP address of the camera.
\param      [in]pszDeviceMacAddress The MAC address of the device.
\param      [in]ui32IpConfigFlag    IP Configuration.
\param      [in]pszIPAddress        The IP address to be set.
\param      [in]pszSubnetMask       The subnet mask to be set.
\param      [in]pszDefaultGateway   The default gateway to be set.
\param      [in]pszUserID           The user-defined name to be set.
\retrun     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER     The parameter is invalid.
            GX_STATUS_NOT_FOUND_DEVICE      Can not found the device.
            GX_STATUS_ERROR                 The operation is failed.
            GX_STATUS_INVALID_ACCESS        Access denied.
            GX_STATUS_TIMEOUT               The operation is timed out.
            GC_ERR_IO                       IO error.
            GC_ERR_INVALID_ID               Invalid ID.
            The errors that are not covered above please reference GX_STATUS_LIST.
*/
// ---------------------------------------------------------------------------
GX_API GXGigEIpConfiguration (const char *pszDeviceMacAddress,
                              GX_IP_CONFIGURE_MODE emIpConfigMode,
                              const char *pszIpAddress,
                              const char *pszSubnetMask,
                              const char *pszDefaultGateway,
                              const char *pszUserID);

// ---------------------------------------------------------------------------
/**
\brief      Execute the Force IP.
\param      [in]pszDeviceMacAddress The MAC address of the device.
\param      [in]pszIPAddress        The IP address to be set.
\param      [in]pszSubnetMask       The subnet mask to be set.
\param      [in]pszDefaultGateway   The default gateway to be set.
\retrun     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER     The parameter is invalid.
            GX_STATUS_NOT_FOUND_DEVICE      Can not found the device.
            GX_STATUS_ERROR                 The operation is failed.
            GX_STATUS_INVALID_ACCESS        Access denied.
            GX_STATUS_TIMEOUT               The operation is timed out.
            GC_ERR_IO                       IO error.
            GC_ERR_INVALID_ID               Invalid ID.
*/
// ---------------------------------------------------------------------------
GX_API GXGigEForceIp (const char *pszDeviceMacAddress,
                      const char *pszIpAddress,
                      const char *pszSubnetMask,
                      const char *pszDefaultGateway);
                                   

// ---------------------------------------------------------------------------
/**
\brief      Reconnection/Reset
\param      [in]pszDeviceMacAddress The MAC address of the device.
\param      [in]ui32FeatureInfo     Reconnection mode.
\retrun     GX_STATUS_SUCCESS               The operation is successful and no error occurs.
            GX_STATUS_NOT_INIT_API          The GXInitLib initialization library is not called.
            GX_STATUS_INVALID_PARAMETER     The parameter is invalid.
            GX_STATUS_NOT_FOUND_DEVICE      Can not found the device.
            GX_STATUS_ERROR                 The operation is failed.
            GX_STATUS_INVALID_ACCESS        Access denied.
            GX_STATUS_TIMEOUT               The operation is timed out.
            GC_ERR_IO                       IO error.
            GC_ERR_INVALID_ID               Invalid ID.
*/
// ---------------------------------------------------------------------------
GX_API GXGigEResetDevice (const char *pszDeviceMacAddress, GX_RESET_DEVICE_MODE ui32FeatureInfo);

#endif  //GX_GALAXY_H
