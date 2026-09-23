#include "common.h"
#ifdef GTA_PS2
#include "Dma.h"

#include <eeregs.h>

DmaRegs *CDma::pDmaRegs[10] = {
	(DmaRegs*)D0_CHCR,
	(DmaRegs*)D1_CHCR,
	(DmaRegs*)D2_CHCR,
	(DmaRegs*)D3_CHCR,
	(DmaRegs*)D4_CHCR,
	(DmaRegs*)D5_CHCR,
	(DmaRegs*)D6_CHCR,
	(DmaRegs*)D7_CHCR,
	(DmaRegs*)D8_CHCR,
	(DmaRegs*)D9_CHCR,
};

void
CDma::StartDmaTag(DmaChannel ch, void *pkt, uint32 flags)
{
	volatile DmaRegs *regs = pDmaRegs[ch];
	uint32 addr = (uint32)pkt;

	if(addr & 0xF){
		printf("ERROR: DMA memory 0x%x is not aligned to a quad word\n", addr);
		exit(-1);
	}
	regs->qwc = 0;
	// 80000000 selects SPR
	regs->tadr = (addr&~0xF) | (flags&0x80000000);
	// clear interrupt status
	*D_STAT = 1<<ch;
	*D_PCR |= 1<<ch;
	// start, chain, 0xC0 select tag transfer and tag interrupt
	regs->chcr = 0x104 | flags&0xC0;
}

inline void sync_dma_bc0(void)
{
#ifdef __MWERKS__
label_0b:
	asm __volatile__("bc0t label_0f" );
	asm __volatile__("nop");
	asm __volatile__(" bc0t label_0f" );
	asm __volatile__("nop");
	asm __volatile__(" bc0t label_0f" );
	asm __volatile__("nop");
	asm __volatile__("bc0f label_0b");
	asm __volatile__("nop");
label_0f:;
#else
	asm __volatile__("0:");
	asm __volatile__("bc0t 0f" );
	asm __volatile__("nop");
	asm __volatile__(" bc0t 0f" );
	asm __volatile__("nop");
	asm __volatile__(" bc0t 0f" );
	asm __volatile__("nop");
	asm __volatile__("bc0f 0b");
	asm __volatile__("nop");
	asm __volatile__("0:" );
#endif
}

bool
CDma::SyncChannel(DmaChannel ch, bool wait)
{
	volatile DmaRegs *regs = pDmaRegs[ch];

	if(wait){
		while(regs->chcr & 0x100)
			sync_dma_bc0();
		return true;
	}else{
		return (regs->chcr & 0x100) == 0;
	}
}
#endif
