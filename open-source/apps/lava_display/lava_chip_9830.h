/******************************************************************************
 ** File Name:    lava_chip_9830.h                                *
 ** Author:       billy.zhang billy.zhang@spreadtrum.com          *
 ** DATE:         2015-09-09                                        *
 ** Copyright:    2015 Spreatrum, Incoporated. All Rights Reserved. *
 ** Description:                                                    *
 ******************************************************************************/

/*dispc regs*/
#define SPRD_DISPC0_BASE		0x20800000


#define DISPC_SIZE_XY 				(SPRD_DISPC0_BASE + 0x0004)
#define DISPC_DPI_H_TIMING 			(SPRD_DISPC0_BASE + 0x0084)
#define DISPC_DPI_V_TIMING 			(SPRD_DISPC0_BASE + 0x0088)

/*dsi regs*/
#define SPRD_DSI_BASE		0x21800000

#define R_DSI_HOST_DPI_COLOR_CODE		(SPRD_DSI_BASE + 0x0010)
#define R_DSI_HOST_MODE_CFG			(SPRD_DSI_BASE + 0x0034)
#define R_DSI_HOST_PHY_IF_CFG			(SPRD_DSI_BASE + 0x00a4)


/*global res*/
#define AON_APB_RF_BASE		0x402e0000
#define DISP_EMC_EB				(AON_APB_RF_BASE + 0x0004)

#define CTL_BASE_DISPCKG	0x71200000
#define DISPC_PLL_CFG				(CTL_BASE_DISPCKG + 0x002c)

#define REGS_DISP_AHB_BASE	0x20d00000
#define REG_DISP_AHB_AHB_EB			(REGS_DISP_AHB_BASE + 0x0000)

#define REGS_AP_AHB_BASE	0X20D00000
#define REG_AP_AHB_MISC_CKG_EN			(REGS_AP_AHB_BASE + 0x3008)
