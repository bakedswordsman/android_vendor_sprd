#ifndef MENUTITLE

#define __MAKE_MENUTITLE_ENUM__
#define MENUTITLE(num,title, func)	A_PHONE_##title,
enum {
#endif
	MENUTITLE(CASE_TEST_LCD,TEST_LCD, test_lcd_start)
	MENUTITLE(CASE_TEST_TP,TEST_TP, test_tp_start)
	MENUTITLE(CASE_TEST_MULTITOUCH,TEST_MULTI_TOUCH, test_multi_touch_start)
	MENUTITLE(CASE_TEST_BACKLIGHT,TEST_VB_BL, test_vb_bl_start)
	MENUTITLE(CASE_TEST_LED,TEST_LED, test_led_start)
	MENUTITLE(CASE_TEST_KEY,TEST_KEY, test_key_start)
	MENUTITLE(CASE_TEST_FCAMERA,TEST_FCAMERA, test_fcamera_start)
	MENUTITLE(CASE_TEST_BCAMERA,TEST_BCAMERA_FLASH, test_bcamera_start)
	MENUTITLE(CASE_TEST_MAINLOOP,TEST_MAINLOOP, test_mainloopback_start)
	MENUTITLE(CASE_TEST_ASSISLOOP,TEST_ASSISLOOP, test_assisloopback_start)
	MENUTITLE(CASE_TEST_RECEIVER,TEST_RECEIVER, test_receiver_start)
	MENUTITLE(CASE_TEST_CHARGE,TEST_CHARGE, test_charge_start)
	MENUTITLE(CASE_TEST_SDCARD,TEST_SDCARD, test_sdcard_start)
	MENUTITLE(CASE_TEST_SIMCARD,TEST_SIMCARD, test_sim_start)
	MENUTITLE(CASE_TEST_HEADSET,TEST_HEADSET, test_headset_start)
	MENUTITLE(CASE_TEST_FM,TEST_FM, test_fm_start)
	MENUTITLE(CASE_TEST_GSENSOR,TEST_GSENSOR, test_gsensor_start)
	MENUTITLE(CASE_TEST_LPSOR,TEST_LSENSOR, test_lsensor_start)
	MENUTITLE(CASE_TEST_BT,TEST_BT, test_bt_start)
	MENUTITLE(CASE_TEST_WIFI,TEST_WIFI, test_wifi_start)
	MENUTITLE(CASE_TEST_GPS,TEST_GPS, test_gps_start)
#ifdef __MAKE_MENUTITLE_ENUM__
	K_MENU_AUTO_TEST_CNT,
};
#undef __MAKE_MENUTITLE_ENUM__
#undef MENUTITLE
#endif
