/*
 * Copyright (c) 2015-2016, Renesas Electronics Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Renesas nor the names of its contributors may be
 *     used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

//=========================================================
//===== Setting for R-CarM3 ===============================
//=========================================================


#include "common.h"
#include "reg_rcarh3.h"
#include "boot_init_port_M3.h"

 /* Product Register */
#define PRR					(0xFFF00044U)
#define PRR_PRODUCT_MASK	(0x00007F00U)
#define PRR_CUT_MASK		(0x000000FFU)
#define PRR_PRODUCT_H3		(0x00004F00U)           /* R-Car H3 */
#define PRR_PRODUCT_M3		(0x00005200U)           /* R-Car M3 */
#define PRR_PRODUCT_10		(0x00U)



#define PFC_WR(m,n)   *((volatile uint32_t*)PFC_PMMR)=~(n);*((volatile uint32_t*)(m))=(n);


/* SCIF3 Registers for Dummy write */
#define SCIF3_BASE				(0xE6C50000U)
#define SCIF3_SCFCR				(SCIF3_BASE + 0x0018U)
#define SCIF3_SCFDR				(SCIF3_BASE + 0x001CU)
#define SCFCR_DATA				(0x0000U)

/* Realtime module stop control */
#define RMSTPCR0_RTDMAC			(0x00200000U)

/* RT-DMAC Registers */
#define RTDMAC_CH				(0U)		/* choose 0 to 15 */

#define RTDMAC_BASE				(0xFFC10000U)
#define RTDMAC_RDMOR			(RTDMAC_BASE + 0x0060U)
#define RTDMAC_RDMCHCLR			(RTDMAC_BASE + 0x0080U)
#define RTDMAC_RDMSAR(x)		(RTDMAC_BASE + 0x8000U + (0x80U * (x)))
#define RTDMAC_RDMDAR(x)		(RTDMAC_BASE + 0x8004U + (0x80U * (x)))
#define RTDMAC_RDMTCR(x)		(RTDMAC_BASE + 0x8008U + (0x80U * (x)))
#define RTDMAC_RDMCHCR(x)		(RTDMAC_BASE + 0x800CU + (0x80U * (x)))
#define RTDMAC_RDMCHCRB(x)		(RTDMAC_BASE + 0x801CU + (0x80U * (x)))
#define RTDMAC_RDMDPBASE(x)		(RTDMAC_BASE + 0x8050U + (0x80U * (x)))
#define RTDMAC_DESC_BASE		(RTDMAC_BASE + 0xA000U)
#define RTDMAC_DESC_RDMSAR		(RTDMAC_DESC_BASE + 0x0000U)
#define RTDMAC_DESC_RDMDAR		(RTDMAC_DESC_BASE + 0x0004U)
#define RTDMAC_DESC_RDMTCR		(RTDMAC_DESC_BASE + 0x0008U)

#define RDMOR_DME				(0x0001U)		/* DMA Master Enable */
#define RDMCHCR_DPM_INFINITE	(0x30000000U)	/* Infinite repeat mode */
#define RDMCHCR_RPT_TCR			(0x02000000U)	/* enable to update TCR */
#define RDMCHCR_TS_2			(0x00000008U)	/* Word(2byte) units transfer */
#define RDMCHCR_RS_AUTO			(0x00000400U)	/* Auto request */
#define RDMCHCR_DE				(0x00000001U)	/* DMA Enable */
#define RDMCHCRB_DRST			(0x00008000U)	/* Descriptor reset */
#define RDMCHCRB_SLM_256		(0x00000080U)	/* once in 256 clock cycle */
#define RDMDPBASE_SEL_EXT		(0x00000001U)	/* External memory use */


static void StartRtDma0_Descriptor(void);


void InitPORT(void)
{
	uint32_t product;

	product = *((volatile uint32_t*)PRR) & PRR_PRODUCT_MASK;

	switch (product) {
	case PRR_PRODUCT_H3:
		InitMODSEL();
		InitIPSR_H3();
		InitGPSR_H3();
		InitPOCCTRL();
		InitDRVCTRL();
		InitPUD();
		InitPUEN();
		break;

	case PRR_PRODUCT_M3:
		StartRtDma0_Descriptor();
		InitMODSEL();
		InitIPSR_M3();
		InitGPSR_M3();
		InitPOCCTRL();
		InitDRVCTRL();
		InitPUD();
		InitPUEN();
		break;

	default:
		break;
	}

}


void InitMODSEL(void)
{
	PFC_WR(PFC_MOD_SEL0,0x00000000);
	PFC_WR(PFC_MOD_SEL1,0x00000000);
	PFC_WR(PFC_MOD_SEL2,0x00000000);
}

void InitIPSR_H3(void)
{
	PFC_WR(PFC_IPSR0, 0x00000000);

#ifdef Area0Boot
	PFC_WR(PFC_IPSR1, 0x02222220);
	PFC_WR(PFC_IPSR2, 0x00000000);
	PFC_WR(PFC_IPSR3, 0x00000000);
	PFC_WR(PFC_IPSR4, 0x00000000);
	PFC_WR(PFC_IPSR5, 0x00000000);
	PFC_WR(PFC_IPSR6, 0x00000000);
	PFC_WR(PFC_IPSR7, 0x00000000);
#else			
	PFC_WR(PFC_IPSR1, 0x60003333);
	PFC_WR(PFC_IPSR2, 0x06666666);
	PFC_WR(PFC_IPSR3, 0x66666000);
	PFC_WR(PFC_IPSR4, 0x00000666);
	PFC_WR(PFC_IPSR5, 0x00000600);
	PFC_WR(PFC_IPSR6, 0x66666000);
	PFC_WR(PFC_IPSR7, 0x00000666);
#endif

	PFC_WR(PFC_IPSR8, 0x11110000);
	PFC_WR(PFC_IPSR9, 0x00000000);
	PFC_WR(PFC_IPSR10,0x04000011);
	PFC_WR(PFC_IPSR11,0x00000400);
	PFC_WR(PFC_IPSR12,0x80000300);
	PFC_WR(PFC_IPSR13,0x00000038);
	PFC_WR(PFC_IPSR14,0x00000000);
	PFC_WR(PFC_IPSR15,0x00000011);
	PFC_WR(PFC_IPSR16,0x00000010);
	PFC_WR(PFC_IPSR17,0x00000000);
}

void InitIPSR_M3(void)
{
	PFC_WR(PFC_IPSR0, 0x00000000);

#ifdef Area0Boot
	PFC_WR(PFC_IPSR1, 0x02222220);
	PFC_WR(PFC_IPSR2, 0x00000000);
	PFC_WR(PFC_IPSR3, 0x00000000);
	PFC_WR(PFC_IPSR4, 0x00000000);
	PFC_WR(PFC_IPSR5, 0x00000000);
	PFC_WR(PFC_IPSR6, 0x00000000);
	PFC_WR(PFC_IPSR7, 0x00000000);
#else			
	PFC_WR(PFC_IPSR1, 0x60003333);
	PFC_WR(PFC_IPSR2, 0x06666666);
	PFC_WR(PFC_IPSR3, 0x66666000);
	PFC_WR(PFC_IPSR4, 0x00000666);
	PFC_WR(PFC_IPSR5, 0x00000600);
	PFC_WR(PFC_IPSR6, 0x66666000);
	PFC_WR(PFC_IPSR7, 0x00000666);
#endif

	PFC_WR(PFC_IPSR8, 0x11110000);
	PFC_WR(PFC_IPSR9, 0x00000000);
	PFC_WR(PFC_IPSR10,0x10000000);
	PFC_WR(PFC_IPSR11,0x04000001);
	PFC_WR(PFC_IPSR12,0x00000400);
	PFC_WR(PFC_IPSR13,0x80000300);
	PFC_WR(PFC_IPSR14,0x00000038);
	PFC_WR(PFC_IPSR15,0x00000000);
	PFC_WR(PFC_IPSR16,0x00000000);
	PFC_WR(PFC_IPSR17,0x00000010);
}

void InitGPSR_H3(void)
{
#ifdef Area0Boot
	PFC_WR(PFC_GPSR0, 0x0000FFFF);
	PFC_WR(PFC_GPSR1, 0x0EFFFFFF);		//H3
	PFC_WR(PFC_GPSR2, 0x00007BFF);
#else			
	PFC_WR(PFC_GPSR0, 0x0000FF00);
	PFC_WR(PFC_GPSR1, 0x080FF0FF);		//H3
	PFC_WR(PFC_GPSR2, 0x00007BBF);
#endif
	PFC_WR(PFC_GPSR3, 0x00003F3F);
	PFC_WR(PFC_GPSR4, 0x00019FFF);
	PFC_WR(PFC_GPSR5, 0x002DFDF1);
	PFC_WR(PFC_GPSR6, 0x3FDEC70F);		//H3
	PFC_WR(PFC_GPSR7, 0x0000000F);
}

void InitGPSR_M3(void)
{
#ifdef Area0Boot
	PFC_WR(PFC_GPSR0, 0x0000FFFF);
	PFC_WR(PFC_GPSR1, 0x1EFFFFFF);		//M3
	PFC_WR(PFC_GPSR2, 0x00007BFF);
#else			
	PFC_WR(PFC_GPSR0, 0x0000FF00);
	PFC_WR(PFC_GPSR1, 0x180FF0FF);		//M3
	PFC_WR(PFC_GPSR2, 0x00007BBF);
#endif
	PFC_WR(PFC_GPSR3, 0x00003F3F);
	PFC_WR(PFC_GPSR4, 0x00019FFF);
	PFC_WR(PFC_GPSR5, 0x002DFDF1);
	PFC_WR(PFC_GPSR6, 0x3FDE070F);		//M3
	PFC_WR(PFC_GPSR7, 0x0000000F);
}

void InitPOCCTRL(void)
{
	PFC_WR(PFC_POCCTRL0, 0x3FF8003F);
}

void InitDRVCTRL(void)
{
	PFC_WR(PFC_DRVCTRL0, 0x33333333);
	PFC_WR(PFC_DRVCTRL1, 0x33333337);
	PFC_WR(PFC_DRVCTRL2, 0x77777777);
	PFC_WR(PFC_DRVCTRL3, 0x77777777);
	PFC_WR(PFC_DRVCTRL4, 0x77777777);
	PFC_WR(PFC_DRVCTRL5, 0x77777777);
	PFC_WR(PFC_DRVCTRL6, 0x77777777);
	PFC_WR(PFC_DRVCTRL7, 0x77777777);
	PFC_WR(PFC_DRVCTRL8, 0x77777777);
	PFC_WR(PFC_DRVCTRL9, 0x77777777);
	PFC_WR(PFC_DRVCTRL10,0x77777777);
	PFC_WR(PFC_DRVCTRL11,0x77777733);
	PFC_WR(PFC_DRVCTRL12,0x33300030);
	PFC_WR(PFC_DRVCTRL13,0x33777777);
	PFC_WR(PFC_DRVCTRL14,0x77777777);
	PFC_WR(PFC_DRVCTRL15,0x77777777);
	PFC_WR(PFC_DRVCTRL16,0x77777777);
	PFC_WR(PFC_DRVCTRL17,0x77777777);
	PFC_WR(PFC_DRVCTRL18,0x77777777);
	PFC_WR(PFC_DRVCTRL19,0x77777777);
	PFC_WR(PFC_DRVCTRL20,0x77777777);
	PFC_WR(PFC_DRVCTRL21,0x77777777);
	PFC_WR(PFC_DRVCTRL22,0x77777777);
	PFC_WR(PFC_DRVCTRL23,0x77777777);
	PFC_WR(PFC_DRVCTRL24,0x77777770);
}

void InitPUD(void)
{
	PFC_WR(PFC_PUD0,0x00005FBF);
	PFC_WR(PFC_PUD1,0x00200FFE);
	PFC_WR(PFC_PUD2,0x330001E6);
	PFC_WR(PFC_PUD3,0x000002E0);
	PFC_WR(PFC_PUD4,0xFFFFFF00);
	PFC_WR(PFC_PUD5,0x7F5FFF87);
	PFC_WR(PFC_PUD6,0x00000055);
}

void InitPUEN(void)
{
	PFC_WR(PFC_PUEN0,0x00000FFF);

#ifdef Area0Boot
	PFC_WR(PFC_PUEN1,0x00000034);
	PFC_WR(PFC_PUEN2,0x000004C6);
#else			
	PFC_WR(PFC_PUEN1,0x00100234);
	PFC_WR(PFC_PUEN2,0x000004C4);
#endif

	PFC_WR(PFC_PUEN3,0x00000200);
	PFC_WR(PFC_PUEN4,0x3E000000);
	PFC_WR(PFC_PUEN5,0x1F000805);
	PFC_WR(PFC_PUEN6,0x00000006);
}


static void StartRtDma0_Descriptor(void)
{
	uint32_t reg;

	reg = *((volatile uint32_t *)PRR);
	reg &= (PRR_CUT_MASK);
	if (reg == (PRR_PRODUCT_10)) {
		/* Module stop clear */
		while((*((volatile uint32_t *)CPG_RMSTPCR0) & RMSTPCR0_RTDMAC) != 0U) {
			reg = *((volatile uint32_t *)CPG_RMSTPCR0);
			reg &= ~RMSTPCR0_RTDMAC;
			*((volatile uint32_t *)CPG_CPGWPR) = ~reg;
			*((volatile uint32_t *)CPG_RMSTPCR0) = reg;
		}

		/* Initialize ch0, Reset Descriptor */
		*((volatile uint32_t *)RTDMAC_RDMCHCLR) = ((uint32_t)1U << RTDMAC_CH);
		*((volatile uint32_t *)RTDMAC_RDMCHCRB(RTDMAC_CH)) = RDMCHCRB_DRST;

		/* Enable DMA */
		*((volatile uint16_t *)RTDMAC_RDMOR) = RDMOR_DME;

		/* Set first transfer */
		*((volatile uint32_t *)RTDMAC_RDMSAR(RTDMAC_CH)) = PRR;
		*((volatile uint32_t *)RTDMAC_RDMDAR(RTDMAC_CH)) = SCIF3_SCFDR;
		*((volatile uint32_t *)RTDMAC_RDMTCR(RTDMAC_CH)) = 0x00000001U;

		/* Set descriptor */
		*((volatile uint32_t *)RTDMAC_DESC_RDMSAR) = 0x00000000U;
		*((volatile uint32_t *)RTDMAC_DESC_RDMDAR) = 0x00000000U;
		*((volatile uint32_t *)RTDMAC_DESC_RDMTCR) = 0x00200000U;
		*((volatile uint32_t *)RTDMAC_RDMCHCRB(RTDMAC_CH)) = RDMCHCRB_SLM_256;
		*((volatile uint32_t *)RTDMAC_RDMDPBASE(RTDMAC_CH)) = (RTDMAC_DESC_BASE
														     | RDMDPBASE_SEL_EXT);

		/* Set transfer parameter, Start transfer */
		*((volatile uint32_t *)RTDMAC_RDMCHCR(RTDMAC_CH)) = (RDMCHCR_DPM_INFINITE
														   | RDMCHCR_RPT_TCR
														   | RDMCHCR_TS_2
														   | RDMCHCR_RS_AUTO
														   | RDMCHCR_DE);
	}
}