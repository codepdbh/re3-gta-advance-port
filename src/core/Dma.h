#ifndef __GTA_DMA_H__
#define __GTA_DMA_H__

#ifdef GTA_PS2

enum DmaChannel {
	DMA_VIF0,
	DMA_VIF1,
	DMA_GIF,
	DMA_fromIPU,
	DMA_toIPU,
	DMA_SIF0,
	DMA_SIF1,
	DMA_SIF2,
	DMA_fromSPR,
	DMA_toSPR
};

struct DmaRegs
{
	uint32 chcr, pad0[3];
	uint32 madr, pad1[3];
	uint32 qwc, pad2[3];
	uint32 tadr, pad3[3];
	uint32 asr0, pad4[3];
	uint32 asr1, pad5[3];
};

class CDma
{
	static DmaRegs *pDmaRegs[10];
public:
	static void StartDmaTag(DmaChannel ch, void *pkt, uint32 flags);
	static bool SyncChannel(DmaChannel ch, bool wait);
};

#endif

#endif // __GTA_DMA_H__
