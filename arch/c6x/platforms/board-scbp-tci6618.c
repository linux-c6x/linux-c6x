/*
 *  linux/arch/c6x/platforms/board-scbp-tci6618.c
 *
 *  Port on Texas Instruments TMS320C6x architecture
 *
 *  Copyright (C) 2011 Texas Instruments Incorporated
 *  Author: Aurelien Jacquiot <a-jacquiot@ti.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/i2c/at24.h>
#include <linux/clk.h>
#include <linux/kernel_stat.h>
#include <linux/platform_device.h>

#include <asm/setup.h>
#include <asm/irq.h>
#include <asm/machdep.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/timer.h>
#include <asm/percpu.h>
#include <asm/clock.h>

#include <mach/board.h>

SOC_CLK_DEF(122880000); /* SYSCLK is a 122.88 MHz clock */

static struct clk_lookup evm_clks[] = {
        SOC_CLK(),
	CLK("", NULL, NULL)
};


#ifdef CONFIG_EDMA3
#include <asm/edma.h>

static const s8
queue_tc_mapping0[][2] = {
	/* {event queue no, TC no} */
	{0, 0},
	{1, 1},
	{-1, -1},
};

static const s8
queue_priority_mapping0[][2] = {
	/* {event queue no, Priority} */
	{0, 0},
	{1, 1},
	{-1, -1},
};

static const s8
queue_tc_mapping1[][2] = {
	/* {event queue no, TC no} */
	{0, 0},
	{1, 1},
	{2, 2},
	{3, 3},
	{-1, -1},
};

static const s8
queue_priority_mapping1[][2] = {
	/* {event queue no, Priority} */
	{0, 0},
	{1, 1},
	{2, 2},
	{3, 3},
	{-1, -1},
};

static const s8
queue_tc_mapping2[][2] = {
	/* {event queue no, TC no} */
	{0, 0},
	{1, 1},
	{2, 2},
	{3, 3},
	{-1, -1},
};

static const s8
queue_priority_mapping2[][2] = {
	/* {event queue no, Priority} */
	{0, 0},
	{1, 1},
	{2, 2},
	{3, 3},
	{-1, -1},
};

static struct edma_soc_info edma_cc0_info = {
	.n_channel		= EDMA_NUM_DMACH,
	.n_region		= EDMA_NUM_REGIONS,
	.n_slot			= EDMA0_NUM_PARAMENTRY,
	.n_tc			= EDMA0_NUM_EVQUE,
	.n_cc			= 3,
	.queue_tc_mapping	= queue_tc_mapping0,
	.queue_priority_mapping	= queue_priority_mapping0,
};

static struct edma_soc_info edma_cc1_info = {
	.n_channel		= EDMA_NUM_DMACH,
	.n_region		= EDMA_NUM_REGIONS,
	.n_slot			= EDMA1_NUM_PARAMENTRY,
	.n_tc			= EDMA1_NUM_EVQUE,
	.n_cc			= 3,
	.queue_tc_mapping	= queue_tc_mapping1,
	.queue_priority_mapping	= queue_priority_mapping1,
};

static struct edma_soc_info edma_cc2_info = {
	.n_channel		= EDMA_NUM_DMACH,
	.n_region		= EDMA_NUM_REGIONS,
	.n_slot			= EDMA2_NUM_PARAMENTRY,
	.n_tc			= EDMA2_NUM_EVQUE,
	.n_cc			= 3,
	.queue_tc_mapping	= queue_tc_mapping2,
	.queue_priority_mapping	= queue_priority_mapping2,
};

static struct edma_soc_info *edma_info[] = {
	&edma_cc0_info,
	&edma_cc1_info,
	&edma_cc2_info,
};

static struct resource edma_resources[] = {
	{
		.name	= "edma0",
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma1",
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma2",
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma0_err",
		.start	= EDMA0_IRQ_CCERRINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma1_err",
		.start	= EDMA1_IRQ_CCERRINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma2_err",
		.start	= EDMA2_IRQ_CCERRINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma_cc0",
		.start	= EDMA0_REGISTER_BASE,
		.end	= EDMA0_REGISTER_BASE + 0x7fff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_cc1",
		.start	= EDMA1_REGISTER_BASE,
		.end	= EDMA1_REGISTER_BASE + 0x7fff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_cc2",
		.start	= EDMA2_REGISTER_BASE,
		.end	= EDMA2_REGISTER_BASE + 0x7fff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma0_tc0",
		.start	= EDMA0_TC0_BASE,
		.end	= EDMA0_TC0_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma0_tc1",
		.start	= EDMA0_TC1_BASE,
		.end	= EDMA0_TC1_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma1_tc0",
		.start	= EDMA1_TC0_BASE,
		.end	= EDMA1_TC0_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma1_tc1",
		.start	= EDMA1_TC1_BASE,
		.end	= EDMA1_TC1_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma1_tc2",
		.start	= EDMA1_TC2_BASE,
		.end	= EDMA1_TC2_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma1_tc3",
		.start	= EDMA1_TC3_BASE,
		.end	= EDMA1_TC3_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma2_tc0",
		.start	= EDMA2_TC0_BASE,
		.end	= EDMA2_TC0_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma2_tc1",
		.start	= EDMA2_TC1_BASE,
		.end	= EDMA2_TC1_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma2_tc2",
		.start	= EDMA2_TC2_BASE,
		.end	= EDMA2_TC2_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma2_tc3",
		.start	= EDMA2_TC3_BASE,
		.end	= EDMA2_TC3_BASE + 0x03ff,
		.flags	= IORESOURCE_MEM,
	},
	/* not using TC*_ERR */
};

static struct platform_device edma_device = {
	.name			= "edma",
	.id			= 0,
	.dev.platform_data	= edma_info,
	.num_resources		= ARRAY_SIZE(edma_resources),
	.resource		= edma_resources,
};

static int __init evm_init_edma(void)
{
	int status;

	/* This is based on coreid, so must be calculated at runtime */
	edma_resources[0].start = EDMA0_IRQ_CCINT;
	edma_resources[1].start = EDMA1_IRQ_CCINT;
	edma_resources[2].start = EDMA2_IRQ_CCINT;

	status = platform_device_register(&edma_device);
	if (status != 0)
		pr_debug("setup_edma --> %d\n", status);

	return status;

}
arch_initcall(evm_init_edma);
#endif /* CONFIG_EDMA3 */

#if defined(CONFIG_SERIAL_8250) || defined(CONFIG_SERIAL_8250_MODULE)
#include <linux/serial_8250.h>
#include <linux/serial.h>
/*
 * Configuration for 16550-like UART
 */
static struct plat_serial8250_port serial8250_platform_data [] = {
        {
                .membase  = (void *) UART_BASE_ADDR,
                .mapbase  = UART_BASE_ADDR,
                .irq      = IRQ_UARTINT,
                .flags    = UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
                .iotype   = UPIO_MEM32,
                .regshift = 2,
        },
        {
                .flags          = 0
        },
};

static struct platform_device serial8250_device = {
        .id             = PLAT8250_DEV_PLATFORM,
        .name           = "serial8250",
        .dev            = {
                .platform_data = serial8250_platform_data,
        },
};

#include <linux/serial_reg.h>
#define SERIAL_OUT(offset, value) writel(value, UART_BASE_ADDR + ((offset) << 2))

static int __init evm_init_uart(void)
{
	struct clk *clk;

	/* 
	 * Reset and enable the UART peripheral
	 */
	SERIAL_OUT(0xc, 0x0000);    /* Reset the UART */
	udelay(100);
	SERIAL_OUT(0xc, 0x6001);    /* UTRST | URRST | FREE */
	SERIAL_OUT(UART_FCR, 0x8);  /* DMAMODE1 */

	/*
	 *  Retrieve the UART clock
	 */
	clk = clk_get(NULL, "uart");
	if (IS_ERR(clk))
		return -ENODEV;
	else
		serial8250_platform_data[0].uartclk = clk_get_rate(clk);

	return platform_device_register(&serial8250_device);
}

core_initcall(evm_init_uart);
#endif

static void dummy_print_dummy(char *s, unsigned long hex) {}
static void dummy_progress(unsigned int step, char *s) {}

/* Called from arch/kernel/setup.c */
void c6x_board_setup_arch(void)
{   
	printk("Designed for the SCBP board, Texas Instruments.\n");

	mach_progress      = dummy_progress;
	mach_print_value   = dummy_print_dummy;

	c6x_clk_init(evm_clks);

	mach_progress(1, "End of SCBP specific initialization");
}

__init int evm_init(void)
{
	return 0;
}

arch_initcall(evm_init);
