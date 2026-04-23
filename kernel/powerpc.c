#include "global.h"

#define diff_ticks(tick0,tick1)		(((u32)(tick1)<(u32)(tick0))?((u32)-1-(u32)(tick0)+(u32)(tick1)):((u32)(tick1)-(u32)(tick0)))

void hardUDelay(u32 us) {
	u32 ticks;
	u32 curTimer = read32(HW_TIMER);
	//HW_TIMER runs at 1/128th of the HW clock
	if (read32(HW_CLOCKS) & 2) { //GC clocks => 162 MHz
		ticks = (us * 81) >> 6;
	} else { //Wii clocks => 243 MHz
		ticks = (us * 243) >> 7;
	}
	
	while (diff_ticks(curTimer, read32(HW_TIMER)) < ticks);
}

void powerpc_upload_stub(u32 entry)
{
	u32 i;

	set32(HW_EXICTRL, EXICTRL_ENABLE_EXI);

	// lis r3, entry@h
	write32(EXI_BOOT_BASE + 4 * 0, 0x3c600000 | entry >> 16);
	// ori r3, r3, entry@l
	write32(EXI_BOOT_BASE + 4 * 1, 0x60630000 | (entry & 0xffff));
	// mtsrr0 r3
	write32(EXI_BOOT_BASE + 4 * 2, 0x7c7a03a6);
	// li r3, 0
	write32(EXI_BOOT_BASE + 4 * 3, 0x38600000);
	// mtsrr1 r3
	write32(EXI_BOOT_BASE + 4 * 4, 0x7c7b03a6);
	// rfi
	write32(EXI_BOOT_BASE + 4 * 5, 0x4c000064);

	for (i = 6; i < 0x10; ++i)
		write32(EXI_BOOT_BASE + 4 * i, 0);

	set32(HW_DIFLAGS, DIFLAGS_BOOT_CODE);
	set32(HW_AHBPROT, 0xFFFFFFFF);

	dbgprintf("disabling EXI now...\n");
	clear32(HW_EXICTRL, EXICTRL_ENABLE_EXI);
}

void powerpc_switch_clock(void) {
	/*write32(HW_CLOCKS, 1);
	udelay(1);
	write32(HW_RESETS, 0x7FFFFFC7);
	udelay(1);

	write32(HW_CLOCKS, 3);
	write32(HW_RESETS, 0x7FFFFFCF);
	udelay(4);

	write32(HW_CLOCKS, 2);
	udelay(1);*/

	set32(HW_CLOCKS, 1); //Set FX
	hardUDelay(100);
	clear32(HW_RESETS, 0x08); //Clear RSTB_DSKPLL
	hardUDelay(100);
	set32(HW_CLOCKS, 2); //Set SPEED
	hardUDelay(100);
	set32(HW_RESETS, 0x08); //Set RSTB_DSKPLL
	hardUDelay(100);
	clear32(HW_CLOCKS, 1); //Clear FX
	hardUDelay(100);
}

void powerpc_hang(void)
{
	clear32(HW_RESETS, 0x30);
	hardUDelay(100);
	// set32(HW_RESETS, 0x20);
	// hardUDelay(100);
}

void powerpc_reset(void)
{
	// enable the broadway IPC interrupt
	write32(HW_PPCIRQMASK, (1<<30));
	clear32(HW_RESETS, 0x30);
	hardUDelay(100);
	set32(HW_RESETS, 0x20);
	hardUDelay(200);
	set32(HW_RESETS, 0x10);
	hardUDelay(100000);
	set32(HW_EXICTRL, EXICTRL_ENABLE_EXI);
}