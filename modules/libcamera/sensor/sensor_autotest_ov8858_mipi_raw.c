/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"
#include "sensor_ov8858_raw_param.c"




#define ov8858_I2C_ADDR_W        (0x6c>>1)
#define ov8858_I2C_ADDR_R         (0x6c>>1)

#define ov8858_RAW_PARAM_COM  0x0000


#define ov8858_4_LANES
LOCAL unsigned long _ov8858_GetResolutionTrimTab(uint32_t param);
LOCAL uint32_t _ov8858_PowerOn(uint32_t power_on);
LOCAL uint32_t _ov8858_Identify(uint32_t param);

LOCAL uint32_t _ov8858_StreamOn(uint32_t param);
LOCAL uint32_t _ov8858_StreamOff(uint32_t param);

LOCAL uint32_t _ov8858_com_Identify_otp(void* param_ptr);


LOCAL const struct raw_param_info_tab s_ov8858_raw_param_tab[]={
	{ov8858_RAW_PARAM_COM, &s_ov8858_mipi_raw_info, _ov8858_com_Identify_otp, PNULL},
	{RAW_INFO_END_ID, PNULL, PNULL, PNULL}
};

struct sensor_raw_info* s_at_ov8858_mipi_raw_info_ptr=NULL;

static uint32_t g_module_id = 0;


LOCAL const SENSOR_REG_T ov8858_common_init[] = {
	//@@ SIZE_3264X2448_15FPS_MIPI_2LANE
	//100 99 3264 2448 ; Resolution
	//102 80 1
	//102 3601 5dc ;15fps
	//102 40 0 ; HDR Mode off
	//;FPGA set-up
	//c8 01 f2 ;MIPI FPGA;
	//CL 100 100;delay

	{0x103,0x01},
	//{0x303f,0x01},
	//{0x3012,0x6c},
	{SENSOR_WRITE_DELAY, 0x0a},  //;delay 5ms
	{0x100,0x00},
	{0x302,0x1e},//mipi colock
	{0x303,0x00},
	{0x304,0x03},
	{0x30e,0x02},
	{0x30d,0x1e},//mipi
	{0x30f,0x04},
	{0x312,0x03},
	{0x31e,0x0c},
	{0x3600,0x00},
	{0x3601,0x00},
	{0x3602,0x00},
	{0x3603,0x00},
	{0x3604,0x22},
	{0x3605,0x20},
	{0x3606,0x00},
	{0x3607,0x20},
	{0x3608,0x11},
	{0x3609,0x28},
	{0x360a,0x00},
	{0x360b,0x05},
	{0x360c,0xd4},
	{0x360d,0x40},
	{0x360e,0x0c},
	{0x360f,0x20},
	{0x3610,0x07},
	{0x3611,0x20},
	{0x3612,0x88},
	{0x3613,0x80},
	{0x3614,0x58},
	{0x3615,0x00},
	{0x3616,0x4a},
	{0x3617,0x40},
	{0x3618,0x5a},
	{0x3619,0x70},
	{0x361a,0x99},
	{0x361b,0x0a},
	{0x361c,0x07},
	{0x361d,0x00},
	{0x361e,0x00},
	{0x361f,0x00},
	{0x3638,0xff},
	{0x3633,0x0f},
	{0x3634,0x0f},
	{0x3635,0x0f},
	{0x3636,0x12},
	{0x3645,0x13},
	{0x3646,0x83},
	{0x364a,0x07},
	{0x3015,0x00},
#if defined(ov8858_2_LANES)
	{0x3018,0x32},
#elif defined(ov8858_4_LANES)
	{0x3018,0x72},
#endif
	{0x3020,0x93},
	{0x3022,0x01},
	{0x3031,0x0a},
	{0x3034,0x00},
	{0x3106,0x01},
	{0x3305,0xf1},
	{0x3308,0x00},
	{0x3309,0x28},
	{0x330a,0x00},
	{0x330b,0x20},
	{0x330c,0x00},
	{0x330d,0x00},
	{0x330e,0x00},
	{0x330f,0x40},
	{0x3307,0x04},
	{0x3500,0x00},
	{0x3501,0x9a},
	{0x3502,0x20},
	{0x3503,0x80},
	{0x3505,0x80},
	{0x3508,0x02},
	{0x3509,0x00},
	{0x350c,0x00},
	{0x350d,0x80},
	{0x3510,0x00},
	{0x3511,0x02},
	{0x3512,0x00},
	{0x3700,0x18},
	{0x3701,0x0c},
	{0x3702,0x28},
	{0x3703,0x19},
	{0x3704,0x14},
	{0x3705,0x00},
	{0x3706,0x82},
	{0x3707,0x04},
	{0x3708,0x24},
	{0x3709,0x33},
	{0x370a,0x01},
	{0x370b,0x82},
	{0x370c,0x04},
	{0x3718,0x12},
	{0x3719,0x31},
	{0x3712,0x42},
	{0x3714,0x24},
	{0x371e,0x19},
	{0x371f,0x40},
	{0x3720,0x05},
	{0x3721,0x05},
	{0x3724,0x06},
	{0x3725,0x01},
	{0x3726,0x06},
	{0x3728,0x05},
	{0x3729,0x02},
	{0x372a,0x03},
	{0x372b,0x53},
	{0x372c,0xa3},
	{0x372d,0x53},
	{0x372e,0x06},
	{0x372f,0x10},
	{0x3730,0x01},
	{0x3731,0x06},
	{0x3732,0x14},
	{0x3733,0x10},
	{0x3734,0x40},
	{0x3736,0x20},
	{0x373a,0x05},
	{0x373b,0x06},
	{0x373c,0x0a},
	{0x373e,0x03},
	{0x3750,0x0a},
	{0x3751,0x0e},
	{0x3755,0x10},
	{0x3758,0x00},
	{0x3759,0x4c},
	{0x375a,0x06},
	{0x375b,0x13},
	{0x375c,0x20},
	{0x375d,0x02},
	{0x375e,0x00},
	{0x375f,0x14},
	{0x3768,0x22},
	{0x3769,0x44},
	{0x376a,0x44},
	{0x3761,0x00},
	{0x3762,0x00},
	{0x3763,0x00},
	{0x3766,0xff},
	{0x376b,0x00},
	{0x3772,0x23},
	{0x3773,0x02},
	{0x3774,0x16},
	{0x3775,0x12},
	{0x3776,0x04},
	{0x3777,0x00},
	{0x3778,0x1a},
	{0x37a0,0x44},
	{0x37a1,0x3d},
	{0x37a2,0x3d},
	{0x37a3,0x00},
	{0x37a4,0x00},
	{0x37a5,0x00},
	{0x37a6,0x00},
	{0x37a7,0x44},
	{0x37a8,0x4c},
	{0x37a9,0x4c},
	{0x3760,0x00},
	{0x376f,0x01},
	{0x37aa,0x44},
	{0x37ab,0x2e},
	{0x37ac,0x2e},
	{0x37ad,0x33},
	{0x37ae,0x0d},
	{0x37af,0x0d},
	{0x37b0,0x00},
	{0x37b1,0x00},
	{0x37b2,0x00},
	{0x37b3,0x42},
	{0x37b4,0x42},
	{0x37b5,0x31},
	{0x37b6,0x00},
	{0x37b7,0x00},
	{0x37b8,0x00},
	{0x37b9,0xff},
	{0x3800,0x00},
	{0x3801,0x0c},
	{0x3802,0x00},
	{0x3803,0x0c},
	{0x3804,0x0c},
	{0x3805,0xd3},
	{0x3806,0x09},
	{0x3807,0xa3},
	{0x3808,0x0c},
	{0x3809,0xc0},
	{0x380a,0x09},
	{0x380b,0x90},
	{0x380c,0x07},
	{0x380d,0x94},
	{0x380e,0x09},
	{0x380f,0xaa},
	{0x3810,0x00},
	{0x3811,0x04},
	{0x3813,0x02},
	{0x3814,0x01},
	{0x3815,0x01},
	//{0x3820,0x06},//0x00->0x06
	//{0x3821,0x40},//0x46->0x40
	{0x3820,0x00},
    {0x3821,0x46},
	{0x382a,0x01},
	{0x382b,0x01},
	{0x3830,0x06},
	{0x3836,0x01},
	{0x3837,0x18},
	{0x3841,0xff},
	{0x3846,0x48},
	{0x3d85,0x16},
	{0x3d8c,0x73},
	{0x3d8d,0xde},
	{0x3f08,0x08},
	{0x4000,0xf1},
	{0x4001,0x00},
	{0x4005,0x10},
	{0x4002,0x27},
	{0x4009,0x81},
	{0x400b,0x0c},
	{0x4011,0x20},
	{0x401b,0x00},
	{0x401d,0x00},
	{0x4020,0x00},
	{0x4021,0x04},
	{0x4022,0x0c},
	{0x4023,0x60},
	{0x4024,0x0f},
	{0x4025,0x36},
	{0x4026,0x0f},
	{0x4027,0x37},
	{0x4028,0x00},
	{0x4029,0x02},
	{0x402a,0x04},
	{0x402b,0x08},
	{0x402c,0x00},
	{0x402d,0x02},
	{0x402e,0x04},
	{0x402f,0x08},
	{0x401f,0x00},
	{0x4034,0x3f},
	{0x403d,0x04},
	{0x4300,0xff},
	{0x4301,0x00},
	{0x4302,0x0f},
	{0x4316,0x00},
	{0x4503,0x18},
	{0x4600,0x01},
	{0x4601,0x97},
	{0x481f,0x32},
	{0x4837,0x16},//16
	{0x4850,0x10},
	{0x4851,0x32},
	{0x4b00,0x2a},
	{0x4b0d,0x00},
	{0x4d00,0x04},
	{0x4d01,0x18},
	{0x4d02,0xc3},
	{0x4d03,0xff},
	{0x4d04,0xff},
	{0x4d05,0xff},
	{0x5000,0x7e},
	{0x5001,0x01},
	{0x5002,0x08},
	{0x5003,0x20},
	{0x5046,0x12},
	{0x5780,0x3e},
	{0x5781,0x0f},
	{0x5782,0x44},
	{0x5783,0x02},
	{0x5784,0x01},
	{0x5785,0x00},
	{0x5786,0x00},
	{0x5787,0x04},
	{0x5788,0x02},
	{0x5789,0x0f},
	{0x578a,0xfd},
	{0x578b,0xf5},
	{0x578c,0xf5},
	{0x578d,0x03},
	{0x578e,0x08},
	{0x578f,0x0c},
	{0x5790,0x08},
	{0x5791,0x04},
	{0x5792,0x00},
	{0x5793,0x52},
	{0x5794,0xa3},
	{0x5871,0x0d},
	{0x5870,0x18},
	{0x586e,0x10},
	{0x586f,0x08},
	{0x58f8,0x3d},
	{0x5901,0x00},
	{0x5b00,0x02},
	{0x5b01,0x10},
	{0x5b02,0x03},
	{0x5b03,0xcf},
	{0x5b05,0x6c},
	{0x5e00,0x00},
	{0x5e01,0x41},
	{0x4825,0x3a},
	{0x4826,0x40},
	{0x4808,0x25},
	{0x3763,0x18},
	{0x3768,0xcc},
	{0x470b,0x28},
	{0x4202,0x00},
	{0x400d,0x10},
	{0x4040,0x07},
	{0x403e,0x08},
	{0x4041,0xc6},
	{0x3007,0x80},
	{0x400a,0x01},
	{0x3015,0x01},
	{0x0100, 0x00}
};

LOCAL const SENSOR_REG_T ov8858_640x480_setting[] = {
		//@@ SIZE_640X480_30FPS_MIPI_4LANE(Binning+subsampling)
	{0x0103,0x01},
	//{0x0100,0x00},
	{0x0302,0x1e},
	{0x0303,0x00},
	{0x0304,0x03},
	{0x030e,0x00},
	{0x030f,0x04},
	{0x0312,0x01},
	{0x031e,0x0c},
	{0x3600,0x00},
	{0x3601,0x00},
	{0x3602,0x00},
	{0x3603,0x00},
	{0x3604,0x22},
	{0x3605,0x20},
	{0x3606,0x00},
	{0x3607,0x20},
	{0x3608,0x11},
	{0x3609,0x28},
	{0x360a,0x00},
	{0x360b,0x05},
	{0x360c,0xd4},
	{0x360d,0x40},
	{0x360e,0x0c},
	{0x360f,0x20},
	{0x3610,0x07},
	{0x3611,0x20},
	{0x3612,0x88},
	{0x3613,0x80},
	{0x3614,0x58},
	{0x3615,0x00},
	{0x3616,0x4a},
	{0x3617,0x40},
	{0x3618,0x5a},
	{0x3619,0x70},
	{0x361a,0x99},
	{0x361b,0x0a},
	{0x361c,0x07},
	{0x361d,0x00},
	{0x361e,0x00},
	{0x361f,0x00},
	{0x3638,0xff},
	{0x3633,0x0f},
	{0x3634,0x0f},
	{0x3635,0x0f},
	{0x3636,0x12},
	{0x3645,0x13},
	{0x3646,0x83},
	{0x364a,0x07},
	{0x3015,0x01},
	{0x3018,0x72},
	{0x3020,0x93},
	{0x3022,0x01},
	{0x3031,0x0a},
	{0x3034,0x00},
	{0x3106,0x01},
	{0x3305,0xf1},
	{0x3308,0x00},
	{0x3309,0x28},
	{0x330a,0x00},
	{0x330b,0x20},
	{0x330c,0x00},
	{0x330d,0x00},
	{0x330e,0x00},
	{0x330f,0x40},
	{0x3307,0x04},
	{0x3500,0x00},
	{0x3501,0x27},
	 {0x3502,0x80},
	 {0x3503,0x80},
	 {0x3505,0x80},
	 {0x3508,0x04},
	 {0x3509,0x00},
	 {0x350c,0x00},
	 {0x350d,0x80},
	 {0x3510,0x00},
	 {0x3511,0x02},
	 {0x3512,0x00},
	 {0x3700,0x30},
	 {0x3701,0x18},
	 {0x3702,0x50},
	 {0x3703,0x32},
	 {0x3704,0x28},
	 {0x3705,0x00},
	 {0x3706,0x82},
	 {0x3707,0x08},
	 {0x3708,0x48},
	 {0x3709,0x66},
	 {0x370a,0x01},
	 {0x370b,0x82},
	 {0x370c,0x07},
	 {0x3718,0x14},
	 {0x3719,0x31},
	 {0x3712,0x44},
	 {0x3714,0x24},
	 {0x371e,0x31},
	 {0x371f,0x7f},
	 {0x3720,0x0a},
	 {0x3721,0x0a},
	 {0x3724,0x0c},
	 {0x3725,0x02},
	 {0x3726,0x0c},
	 {0x3728,0x0a},
	 {0x3729,0x03},
	 {0x372a,0x06},
	 {0x372b,0xa6},
	 {0x372c,0xa6},
	 {0x372d,0xa6},
	 {0x372e,0x0c},
	 {0x372f,0x20},
	 {0x3730,0x02},
	 {0x3731,0x0c},
	 {0x3732,0x28},
	 {0x3733,0x10},
	 {0x3734,0x40},
	 {0x3736,0x30},
	 {0x373a,0x0a},
	 {0x373b,0x0b},
	 {0x373c,0x14},
	 {0x373e,0x06},
	 {0x3750,0x0a},
	 {0x3751,0x0e},
	 {0x3755,0x10},
	 {0x3758,0x00},
	 {0x3759,0x4c},
	 {0x375a,0x0c},
	 {0x375b,0x26},
	 {0x375c,0x20},
	 {0x375d,0x04},
	 {0x375e,0x00},
	 {0x375f,0x28},
	 {0x3768,0x00},
	 {0x3769,0xc0},
	 {0x376a,0x42},
	 {0x3761,0x00},
	 {0x3762,0x00},
	 {0x3763,0x00},
	 {0x3766,0xff},
	 {0x376b,0x00},
	 {0x3772,0x46},
	 {0x3773,0x04},
	 {0x3774,0x2c},
	 {0x3775,0x13},
	 {0x3776,0x08},
	 {0x3777,0x00},
	 {0x3778,0x17},
	 {0x37a0,0x88},
	 {0x37a1,0x7a},
	 {0x37a2,0x7a},
	 {0x37a3,0x00},
	 {0x37a4,0x00},
	 {0x37a5,0x00},
	 {0x37a6,0x00},
	 {0x37a7,0x88},
	 {0x37a8,0x98},
	 {0x37a9,0x98},
	 {0x3760,0x00},
	 {0x376f,0x01},
	 {0x37aa,0x88},
	 {0x37ab,0x5c},
	 {0x37ac,0x5c},
	 {0x37ad,0x55},
	 {0x37ae,0x19},
	 {0x37af,0x19},
	 {0x37b0,0x00},
	 {0x37b1,0x00},
	 {0x37b2,0x00},
	 {0x37b3,0x84},
	 {0x37b4,0x84},
	 {0x37b5,0x60},
	 {0x37b6,0x00},
	 {0x37b7,0x00},
	 {0x37b8,0x00},
	 {0x37b9,0xff},
	 {0x3800,0x00},
	 {0x3801,0x0c},
	 {0x3802,0x00},
	 {0x3803,0x0c},
	 {0x3804,0x0c},
	 {0x3805,0xd3},
	 {0x3806,0x09},
	 {0x3807,0xa3},
	 {0x3808,0x02},
	 {0x3809,0x80},
	 {0x380a,0x01},
	 {0x380b,0xe0},
	 {0x380c,0x09},
	 {0x380d,0xc4},
	 {0x380e,0x07},
	 {0x380f,0x80},
	 {0x3810,0x00},
	 {0x3811,0x04},
	 {0x3813,0x02},
	 {0x3814,0x03},
	 {0x3815,0x01},
	 {0x3820,0x00},
	 {0x3821,0x6f},
	 {0x382a,0x05},
	 {0x382b,0x03},
	 {0x3830,0x0c},
	 {0x3836,0x02},
	 {0x3837,0x18},
	 {0x3841,0xff},
	 {0x3846,0x48},
	 {0x3d85,0x16},
	 {0x3d8c,0x73},
	 {0x3d8d,0xde},
	 {0x3f08,0x10},
	 {0x3f0a,0x00},
	 {0x4000,0xf1},
	 {0x4001,0x10},
	 {0x4005,0x10},
	 {0x4002,0x27},
	 {0x4009,0x81},
	 {0x400b,0x0c},
	 {0x4011,0x20},
	 {0x401b,0x00},
	 {0x401d,0x00},
	 {0x4020,0x00},
	 {0x4021,0x04},
	 {0x4022,0x02},
	 {0x4023,0xc0},
	 {0x4024,0x0f},
	 {0x4025,0x4e},
	 {0x4026,0x0f},
	 {0x4027,0x4f},
	 {0x4028,0x00},
	 {0x4029,0x02},
	 {0x402a,0x02},
	 {0x402b,0x04},
	 {0x402c,0x00},
	 {0x402d,0x02},
	 {0x402e,0x02},
	 {0x402f,0x04},
	 {0x401f,0x00},
	 {0x4034,0x3f},
	 {0x403d,0x04},
	 {0x4300,0xff},
	 {0x4301,0x00},
	 {0x4302,0x0f},
	 {0x4316,0x00},
	 {0x4500,0x58},
	 {0x4503,0x18},
	 {0x4600,0x00},
	 {0x4601,0x50},
	 {0x481f,0x32},
	 {0x4837,0x16},
	 {0x4850,0x10},
	 {0x4851,0x32},
	 {0x4b00,0x2a},
	 {0x4b0d,0x00},
	 {0x4d00,0x04},
	 {0x4d01,0x18},
	 {0x4d02,0xc3},
	 {0x4d03,0xff},
	 {0x4d04,0xff},
	 {0x4d05,0xff},
	 {0x5000,0x7e},
	 {0x5001,0x01},
	 {0x5002,0x08},
	 {0x5003,0x20},
	 {0x5046,0x12},
	 {0x5780,0x3e},
	 {0x5781,0x0f},
	 {0x5782,0x44},
	 {0x5783,0x02},
	 {0x5784,0x01},
	 {0x5785,0x00},
	 {0x5786,0x00},
	 {0x5787,0x04},
	 {0x5788,0x02},
	 {0x5789,0x0f},
	 {0x578a,0xfd},
	 {0x578b,0xf5},
	 {0x578c,0xf5},
	 {0x578d,0x03},
	 {0x578e,0x08},
	 {0x578f,0x0c},
	 {0x5790,0x08},
	 {0x5791,0x04},
	 {0x5792,0x00},
	 {0x5793,0x52},
	 {0x5794,0xa3},
	 {0x5871,0x0d},
	 {0x5870,0x18},
	 {0x586e,0x10},
	 {0x586f,0x08},
	 {0x58f8,0x3d},
	 {0x5901,0x04},
	 {0x5b00,0x02},
	 {0x5b01,0x10},
	 {0x5b02,0x03},
	 {0x5b03,0xcf},
	 {0x5b05,0x6c},
	 {0x5e00,0x00},
	 {0x5e01,0x41},
	 {0x382d,0x7f},
	 {0x4825,0x3a},
	 {0x4826,0x40},
	 {0x4808,0x25},
	 {0x3763,0x18},
	 {0x3768,0xcc},
	 {0x470b,0x28},
	 {0x4202,0x00},
	 {0x400d,0x10},
	 {0x4040,0x07},
	 {0x403e,0x08},
	 {0x4041,0xc6},
	 {0x3007,0x80},
	 {0x400a,0x01},
	 //{0x0100,0x01},
	 {0x5e00,0x80},
};

LOCAL SENSOR_REG_TAB_INFO_T s_ov8858_resolution_Tab_RAW[] = {
	{ADDR_AND_LEN_OF_ARRAY(ov8858_common_init), 0, 0, 24, SENSOR_IMAGE_FORMAT_RAW},
	//{ADDR_AND_LEN_OF_ARRAY(ov8858_1408X792_setting), 1408, 792, 24, SENSOR_IMAGE_FORMAT_RAW},
	//{ADDR_AND_LEN_OF_ARRAY(ov8858_3264X1836_setting), 3264, 1836, 24, SENSOR_IMAGE_FORMAT_RAW},
	{ADDR_AND_LEN_OF_ARRAY(ov8858_640x480_setting), 640, 480, 24, SENSOR_IMAGE_FORMAT_RAW},
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},

	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0}
};

LOCAL SENSOR_TRIM_T s_ov8858_Resolution_Trim_Tab[] = {
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	//{0, 0, 1408, 792, 70, 408, 2366},
	//{0, 0, 3264, 1836, 89, 528, 1872},
	{0, 0, 640, 480, 173, 720, 1920, {0, 0, 640, 480}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}}
};



LOCAL SENSOR_IOCTL_FUNC_TAB_T s_ov8858_ioctl_func_tab = {
	PNULL,
	_ov8858_PowerOn,
	PNULL,
	_ov8858_Identify,

	PNULL,			// write register
	PNULL,			// read  register
	PNULL,
	_ov8858_GetResolutionTrimTab,

	// External
	PNULL,
	PNULL,
	PNULL,

	PNULL, //_ov8858_set_brightness,
	PNULL, // _ov8858_set_contrast,
	PNULL,
	PNULL,			//_ov8858_set_saturation,

	PNULL, //_ov8858_set_work_mode,
	PNULL, //_ov8858_set_image_effect,

	PNULL, //_ov8858_BeforeSnapshot,
	PNULL, //_ov8858_after_snapshot,
	PNULL, //_ov8858_flash,
	PNULL,
	PNULL, //_ov8858_write_exposure,
	PNULL,
	PNULL, //_ov8858_write_gain,
	PNULL,
	PNULL,
	PNULL, //_ov8858_write_af,
	PNULL,
	PNULL, //_ov8858_set_awb,
	PNULL,
	PNULL,
	PNULL, //_ov8858_set_ev,
	PNULL,
	PNULL,
	PNULL,
	PNULL, //_ov8858_GetExifInfo,
	PNULL, //_ov8858_ExtFunc,
	PNULL, //_ov8858_set_anti_flicker,
	PNULL, //_ov8858_set_video_mode,
	PNULL, //pick_jpeg_stream
	PNULL,  //meter_mode
	PNULL, //get_status
	_ov8858_StreamOn,
	_ov8858_StreamOff,
	PNULL,
};


SENSOR_INFO_T g_at_ov8858_mipi_raw_info = {
	ov8858_I2C_ADDR_W,	// salve i2c write address
	ov8858_I2C_ADDR_R,	// salve i2c read address

	SENSOR_I2C_REG_16BIT | SENSOR_I2C_REG_8BIT | SENSOR_I2C_FREQ_100,	// bit0: 0: i2c register value is 8 bit, 1: i2c register value is 16 bit
	// bit1: 0: i2c register addr  is 8 bit, 1: i2c register addr  is 16 bit
	// other bit: reseved
	SENSOR_HW_SIGNAL_PCLK_N | SENSOR_HW_SIGNAL_VSYNC_N | SENSOR_HW_SIGNAL_HSYNC_P,	// bit0: 0:negative; 1:positive -> polarily of pixel clock
	// bit2: 0:negative; 1:positive -> polarily of horizontal synchronization signal
	// bit4: 0:negative; 1:positive -> polarily of vertical synchronization signal
	// other bit: reseved

	// preview mode
	SENSOR_ENVIROMENT_NORMAL | SENSOR_ENVIROMENT_NIGHT,

	// image effect
	SENSOR_IMAGE_EFFECT_NORMAL |
	    SENSOR_IMAGE_EFFECT_BLACKWHITE |
	    SENSOR_IMAGE_EFFECT_RED |
	    SENSOR_IMAGE_EFFECT_GREEN |
	    SENSOR_IMAGE_EFFECT_BLUE |
	    SENSOR_IMAGE_EFFECT_YELLOW |
	    SENSOR_IMAGE_EFFECT_NEGATIVE | SENSOR_IMAGE_EFFECT_CANVAS,

	// while balance mode
	0,

	7,			// bit[0:7]: count of step in brightness, contrast, sharpness, saturation
	// bit[8:31] reseved

	SENSOR_LOW_PULSE_RESET,	// reset pulse level
	5,			// reset pulse width(ms)

	SENSOR_LOW_LEVEL_PWDN,	// 1: high level valid; 0: low level valid

	1,			// count of identify code
	{{0x300A, 0x88},		// supply two code to identify sensor.
	 {0x300B, 0x58}},		// for Example: index = 0-> Device id, index = 1 -> version id

	SENSOR_AVDD_2800MV,	// voltage of avdd

	640,			// max width of source image
	480,			// max height of source image
	"ov8858",		// name of sensor

	SENSOR_IMAGE_FORMAT_RAW,	// define in SENSOR_IMAGE_FORMAT_E enum,SENSOR_IMAGE_FORMAT_MAX
	// if set to SENSOR_IMAGE_FORMAT_MAX here, image format depent on SENSOR_REG_TAB_INFO_T

	SENSOR_IMAGE_PATTERN_RAWRGB_B,// pattern of input image form sensor;
	s_ov8858_resolution_Tab_RAW,	// point to resolution table information structure
	&s_ov8858_ioctl_func_tab,	// point to ioctl function table
	&s_at_ov8858_mipi_raw_info_ptr,		// information and table about Rawrgb sensor
	NULL,			//&g_ov8858_ext_info,                // extend information about sensor
	SENSOR_AVDD_1800MV,	// iovdd
	SENSOR_AVDD_1500MV,	// dvdd
	1,			// skip frame num before preview
	3,			// skip frame num before capture
	0,			// deci frame num during preview
	0,			// deci frame num during video preview

	0,
	0,
	0,
	0,
	0,
#if defined(ov8858_2_LANES)
	{SENSOR_INTERFACE_TYPE_CSI2, 2, 10, 0},
#elif defined(ov8858_4_LANES)
	{SENSOR_INTERFACE_TYPE_CSI2, 4, 10, 0},
#endif

	NULL,//s_ov8858_video_info,
	3,			// skip frame num while change setting
};

LOCAL struct sensor_raw_info* Sensor_GetContext(void)
{
	return s_at_ov8858_mipi_raw_info_ptr;
}

LOCAL uint32_t Sensor_ov8858_InitRawTuneInfo(void)
{
	uint32_t rtn=0x00;
	//struct sensor_raw_info* raw_sensor_ptr=Sensor_GetContext();
	//struct sensor_raw_tune_info* sensor_ptr=raw_sensor_ptr->tune_ptr;
	//struct sensor_raw_cali_info* cali_ptr=raw_sensor_ptr->cali_ptr;

	return rtn;
}


LOCAL unsigned long _ov8858_GetResolutionTrimTab(uint32_t param)
{
	SENSOR_PRINT("0x%x",  (unsigned long)s_ov8858_Resolution_Trim_Tab);
	return (unsigned long) s_ov8858_Resolution_Trim_Tab;
}
LOCAL uint32_t _ov8858_PowerOn(uint32_t power_on)
{
	SENSOR_AVDD_VAL_E dvdd_val = g_at_ov8858_mipi_raw_info.dvdd_val;
	SENSOR_AVDD_VAL_E avdd_val = g_at_ov8858_mipi_raw_info.avdd_val;
	SENSOR_AVDD_VAL_E iovdd_val = g_at_ov8858_mipi_raw_info.iovdd_val;
	BOOLEAN power_down = g_at_ov8858_mipi_raw_info.power_down_level;
	BOOLEAN reset_level = g_at_ov8858_mipi_raw_info.reset_pulse_level;
	//uint32_t reset_width=g_ov8858_yuv_info.reset_pulse_width;

	if (SENSOR_TRUE == power_on) {
		Sensor_PowerDown(power_down);
		// Open power
		Sensor_SetMonitorVoltage(SENSOR_AVDD_2800MV);
		//Sensor_SetVoltage(dvdd_val, avdd_val, iovdd_val);
		Sensor_SetAvddVoltage(avdd_val);
		usleep(1000);
		Sensor_SetIovddVoltage(iovdd_val);
		usleep(1000);
		Sensor_SetDvddVoltage(dvdd_val);
		usleep(2*1000);
		//_ov8858_dw9714_SRCInit(2);
		Sensor_SetMCLK(SENSOR_DEFALUT_MCLK);
		usleep(2*1000);
		Sensor_PowerDown(!power_down);
		usleep(2*1000);
		// Reset sensor
		Sensor_Reset(reset_level);
		usleep(5*1000);
	} else {
		Sensor_PowerDown(power_down);
		Sensor_SetMCLK(SENSOR_DISABLE_MCLK);
		//Sensor_SetVoltage(SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED);
		Sensor_SetDvddVoltage(SENSOR_AVDD_CLOSED);
		Sensor_SetIovddVoltage(SENSOR_AVDD_CLOSED);
		Sensor_SetAvddVoltage(SENSOR_AVDD_CLOSED);
		Sensor_SetMonitorVoltage(SENSOR_AVDD_CLOSED);
	}
	SENSOR_PRINT("SENSOR_ov8858: _ov8858_Power_On(1:on, 0:off): %d", power_on);
	return SENSOR_SUCCESS;
}


LOCAL uint32_t _ov8858_com_Identify_otp(void* param_ptr)
{
	uint32_t rtn=SENSOR_FAIL;
	uint32_t param_id;

	SENSOR_PRINT("SENSOR_ov8858: _ov8858_com_Identify_otp");

	/*read param id from sensor omap*/
	param_id=ov8858_RAW_PARAM_COM;

	if(ov8858_RAW_PARAM_COM==param_id){
		rtn=SENSOR_SUCCESS;
	}

	return rtn;
}

LOCAL uint32_t _ov8858_GetRawInof(void)
{
	uint32_t rtn=SENSOR_SUCCESS;
	struct raw_param_info_tab* tab_ptr = (struct raw_param_info_tab*)s_ov8858_raw_param_tab;
	uint32_t param_id;
	uint32_t i=0x00;

	/*read param id from sensor omap*/
	param_id=ov8858_RAW_PARAM_COM;

	for(i=0x00; ; i++)
	{
		g_module_id = i;
		if(RAW_INFO_END_ID==tab_ptr[i].param_id){
			if(NULL==s_at_ov8858_mipi_raw_info_ptr){
				SENSOR_PRINT("SENSOR_ov8858: ov5647_GetRawInof no param error");
				rtn=SENSOR_FAIL;
			}
			SENSOR_PRINT("SENSOR_ov8858: ov8858_GetRawInof end");
			break;
		}
		else if(PNULL!=tab_ptr[i].identify_otp){
			if(SENSOR_SUCCESS==tab_ptr[i].identify_otp(0))
			{
				s_at_ov8858_mipi_raw_info_ptr = tab_ptr[i].info_ptr;
				SENSOR_PRINT("SENSOR_ov8858: ov8858_GetRawInof success");
				break;
			}
		}
	}

	return rtn;
}




LOCAL uint32_t _ov8858_Identify(uint32_t param)
{
#define ov8858_PID_VALUE_0    0x00
#define ov8858_PID_ADDR_0     0x300A
#define ov8858_PID_VALUE_1    0x88
#define ov8858_PID_ADDR_1     0x300B
#define ov8858_VER_VALUE    0x58
#define ov8858_VER_ADDR     0x300C

	uint8_t pid_value_0 = 0x01;
	uint8_t pid_value_1 = 0x00;
	uint8_t ver_value = 0x00;
	uint32_t ret_value = SENSOR_FAIL;

	SENSOR_PRINT("SENSOR_ov8858: mipi raw identify\n");

	pid_value_0 = Sensor_ReadReg(ov8858_PID_ADDR_0);
	if (ov8858_PID_VALUE_0 == pid_value_0) {
		pid_value_1 = Sensor_ReadReg(ov8858_PID_ADDR_1);
		if (ov8858_PID_VALUE_1 == pid_value_1) {
			ver_value = Sensor_ReadReg(ov8858_VER_ADDR);
			SENSOR_PRINT("SENSOR_ov8858: Identify: PID = 0x%x, VER = 0x%x", pid_value_1, ver_value);
			if (ov8858_VER_VALUE == ver_value) {
				SENSOR_PRINT("SENSOR_ov8858: this is ov8858 sensor !");
				ret_value=_ov8858_GetRawInof();
				if(SENSOR_SUCCESS != ret_value)
				{
					SENSOR_PRINT("SENSOR_ov8858: the module is unknow error !");
				}
				Sensor_ov8858_InitRawTuneInfo();
			} else {
				SENSOR_PRINT("SENSOR_ov8858: Identify this is OV%x%x sensor !", pid_value_1, ver_value);
			}
		} else {
			SENSOR_PRINT("SENSOR_ov8858: identify fail, PID_ADDR = 0x%x,  pid_value= 0x%d", ov8858_PID_ADDR_1, pid_value_1);
		}
	} else {
		SENSOR_PRINT("SENSOR_ov8858: identify fail, PID_ADDR = 0x%x, pid_value= 0x%d", ov8858_PID_ADDR_0, pid_value_0);
		
	}

	return ret_value;
}



LOCAL uint32_t _ov8858_StreamOn(uint32_t param)
{
	SENSOR_PRINT("SENSOR_ov8858: StreamOn");

	Sensor_WriteReg(0x0100, 0x01);

	return 0;
}

LOCAL uint32_t _ov8858_StreamOff(uint32_t param)
{
	SENSOR_PRINT("SENSOR_ov8858: StreamOff");

	Sensor_WriteReg(0x0100, 0x00);
	usleep(100*1000);

	return 0;
}
