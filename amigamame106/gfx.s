***************************************************************************
*
* Copyright (C) 1999 Mats Eirik Hansen (mats.hansen@triumph.no)
*
* CUSTOMDRAWGFX and CUSTOMMEM aren't working with MAME 0.31 and later.
* I haven't bothered fixing them as they apparently gave no speed increase.
*
* $Id: gfx.s,v 1.1 1999/04/28 18:52:25 meh Exp $
*
* $Log: gfx.s,v $
* Revision 1.1  1999/04/28 18:52:25  meh
* Initial revision
*
*
***************************************************************************

	INCDIR	include:,myinclude:

	INCLUDE	"exec/types.i"

	IFD DEBUG
	INCLUDE	"system.gs"
	INCLUDE	"macros.i"
	ENDC

	IFD CUSTOMDRAWGFX
	XDEF	_drawgfx_core8
	XDEF	_clearbitmap
	ENDC

	XDEF	_RemapPixels

	IFD CUSTOMMEM
	XDEF	_mrh_ram
	XDEF	_mrh_error
	XDEF	_mrh_bank1
	XDEF	_mrh_bank2
	XDEF	_mrh_bank3
	XDEF	_mrh_bank4
	XDEF	_mrh_bank5
	XDEF	_mrh_nop
	XDEF	_mrh_error_sparse
	XDEF	_mwh_ram
	XDEF	_mwh_error
	XDEF	_mwh_bank1
	XDEF	_mwh_bank2
	XDEF	_mwh_bank3
	XDEF	_mwh_bank4
	XDEF	_mwh_bank5
	XDEF	_mwh_nop
	XDEF	_mwh_error_sparse
	XDEF	_mwh_rom
	XDEF	_mwh_ramrom
	XDEF	_cpu_readmem16
	XDEF	_cpu_writemem16
	XDEF	_cpu_readmem20
	XDEF	_cpu_writemem20
	XDEF	_cpu_readmem24
	XDEF	_cpu_writemem24
	ENDC

;	XDEF	break

	XREF	_Machine	; -> RunningMachine structure.
	XREF	_remappedtable

	IFD CUSTOMMEM
	XREF	_RAM
	XREF	_ROM
	XREF	_cpu_bankbase
	XREF	_cur_mrhard
	XREF	_readhardware
	XREF	_memoryreadoffset
	XREF	_memoryreadhandler
	XREF	_cur_mwhard
	XREF	_writehardware
	XREF	_memorywriteoffset
	XREF	_memorywritehandler
	ENDC

;	XREF	_DeltaW
;	XREF	_DeltaH
;	XREF	_DeltaWShift
;	XREF	_DeltaHShift
;	XREF	_DeltaBuf

	XREF	_DOSBase

MAX_GFX_ELEMENTS	= 20
MAX_MEMORY_REGIONS	= 10
MAX_PENS		= 256
MAX_LAYERS		= 4

TRANS_NONE	= 0
TRANS_PEN	= 1
TRANS_COLOR	= 2
TRANS_THROUGH	= 3

	BITDEF	ORIENT,FLIP_X,0
	BITDEF	ORIENT,FLIP_Y,1
	BITDEF	ORIENT,SWAP_XY,2

ABITS2_16	= 4
ABITS_MIN_16	= 0
ABITS2_20	= 8
ABITS_MIN_20	= 0
ABITS2_24	= 8
ABITS_MIN_24	= 2
MH_SBITS	= 8
MH_HARDMAX	= 64

	STRUCTURE DrawGfxArgs,0
		APTR	dga_BitMap
		APTR	dga_GfxElement
		LONG	dga_Code
		LONG	dga_Color
		LONG	dga_FlipX
		LONG	dga_FlipY
		LONG	dga_StartX
		LONG	dga_StartY
		APTR	dga_ClipRect
		LONG	dga_Trans
		LONG	dga_TransColor
		LONG	dga_Dirty		; _drawgfx_core only.

	STRUCTURE FillBitMapArgs,0
		APTR	fbma_BitMap
		LONG	fbma_Pen
		APTR	fbma_ClipRect

	STRUCTURE OsdBitMap,0
		LONG	obm_Width
		LONG	obm_Height
		LONG	obm_Depth
		APTR	obm_Private
		APTR	obm_Line

	STRUCTURE GfxElement,0
		LONG	ge_Width
		LONG	ge_Height
		APTR	ge_BitMap
		LONG	ge_TotalElements
		LONG	ge_ColorGranuality
		APTR	ge_ColorTable
		LONG	ge_TotalColors
		ULONG	ge_PenUsage

	STRUCTURE Rect,0
		LONG	r_MinX
		LONG	r_MaxX
		LONG	r_MinY
		LONG	r_MaxY

	STRUCTURE RunningMachine,0
		STRUCT	rm_MemoryRegion,4*MAX_MEMORY_REGIONS
		STRUCT	rm_Gfx,4*MAX_GFX_ELEMENTS
		APTR	rm_ScreenBitMap
		APTR	rm_DirtyLayer
		STRUCT	rm_Layer,4*MAX_LAYERS
		APTR	rm_Pens
		APTR	rm_ColorTable
		APTR	rm_GameDriver
		APTR	rm_MachineDriver
		LONG	rm_SampleRate
		LONG	rm_SampleBits
		APTR	rm_Samples
		APTR	rm_InputPorts
		LONG	rm_Orientation
		APTR	rm_UIFont
		LONG	rm_UIWidth
		LONG	rm_UIHeight
		LONG	rm_UIXMin
		LONG	rm_UIYMin

DUMP:	MACRO
	IFD	DEBUG
	movem.l	d0-d2/a0-a1/a6,-(sp)
	move.l	\1,-(sp)
	move.l	_DOSBase,a6
	move.l	#DumpFormat,d1
	move.l	sp,d2
	CALL	VPrintf
	addq.l	#4,sp
	movem.l	(sp)+,d0-d2/a0-a1/a6
	ENDC
	ENDM

	IFD CUSTOMDRAWGFX
_drawgfx_core8:
	lea	4(sp),a0			; a0.l -> DrawGfxArgs.

	tst.l	dga_GfxElement(a0)
	req

	movem.l	d2-d7/a2-a6,-(sp)

	move.l	_Machine,a1
	move.l	rm_Orientation(a1),d0

	move.l	dga_BitMap(a0),a2		; a2.l -> destination OsdBitMap.
	move.l	dga_GfxElement(a0),a3		; a3.l -> source GfxElement.

	btst	#ORIENTB_SWAP_XY,d0
	beq	dg_NoXYSwap

	move.l	dga_FlipX(a0),d1
	move.l	dga_FlipY(a0),dga_FlipX(a0)
	move.l	d1,dga_FlipY(a0)

	move.l	dga_StartX(a0),d3
	move.l	dga_StartY(a0),d2

	move.l	dga_ClipRect(a0),d1
	beq	dg_NoClip
	move.l	d1,a1

	move.l	r_MinX(a1),d6
	move.l	r_MaxX(a1),d7
	move.l	r_MinY(a1),d4
	move.l	r_MaxY(a1),d5

	bra	dg_FlipX

dg_NoXYSwap:
	move.l	dga_StartX(a0),d2
	move.l	dga_StartY(a0),d3

	move.l	dga_ClipRect(a0),d1
	beq	dg_NoClip
	move.l	d1,a1

	move.l	r_MinX(a1),d4
	move.l	r_MaxX(a1),d5
	move.l	r_MinY(a1),d6
	move.l	r_MaxY(a1),d7

	bra	dg_FlipX

dg_NoClip:
	moveq.l	#0,d4
	add.l	obm_Width(a2),d5
	subq.l	#1,d5
	moveq.l	#0,d6
	move.l	obm_Height(a2),d7
	subq.l	#1,d7

;-- Flip coordinates: -----------------------------------------------------

dg_FlipX:
	btst	#ORIENTB_FLIP_X,d0
	beq	dg_NoXFlip
	neg.l	d2
	add.l	obm_Width(a2),d2
	sub.l	ge_Width(a3),d2
	move.l	d4,d1
	moveq	#-1,d4
	add.l	obm_Width(a2),d4
	sub.l	d5,d4
	moveq	#-1,d5
	add.l	obm_Width(a2),d5
	sub.l	d1,d5

dg_NoXFlip:
	btst	#ORIENTB_FLIP_Y,d0
	beq	dg_NoYFlip
	neg.l	d3
	add.l	obm_Height(a2),d3
	sub.l	ge_Height(a3),d3
	move.l	d6,d1
	moveq	#-1,d6
	add.l	obm_Height(a2),d6
	sub.l	d7,d6
	moveq	#-1,d7
	add.l	obm_Height(a2),d7
	sub.l	d1,d7

dg_NoYFlip:

;-- Clip coordinates: -----------------------------------------------------

	tst.l	d4
	bpl	dg_NoClipClipStartX
	moveq	#0,d4
dg_NoClipClipStartX:
	cmp.l	obm_Width(a2),d5
	bmi	dg_NoClipClipEndX
	move.l	obm_Width(a2),d5
	subq.l	#1,d5
dg_NoClipClipEndX:

	tst.l	d6
	bpl	dg_NoClipClipStartY
	moveq	#0,d6
dg_NoClipClipStartY:
	cmp.l	obm_Height(a2),d7
	bmi	dg_NoClipClipEndY
	move.l	obm_Height(a2),d7
	subq.l	#1,d7
dg_NoClipClipEndY:

	cmp.l	d4,d5
	bmi	dg_Done
	cmp.l	d6,d7
	bmi	dg_Done

	cmp.l	d2,d5
	bmi	dg_Done
	move.l	d2,d1
	add.l	ge_Width(a3),d1
	subq.l	#1,d1
	cmp.l	d4,d1
	bmi	dg_Done
	cmp.l	d4,d2
	bpl	dg_NoClipStartX
	sub.l	d4,d2
	neg.l	d2
	bra	dg_ClipEndX
dg_NoClipStartX:
	move.l	d2,d4
	moveq	#0,d2
dg_ClipEndX:
	cmp.l	d5,d1
	bpl	dg_DoClipEndX
	move.l	d1,d5
dg_DoClipEndX:

	cmp.l	d3,d7
	bmi	dg_Done
	move.l	d3,d1
	add.l	ge_Height(a3),d1
	subq.l	#1,d1
	cmp.l	d6,d1
	bmi	dg_Done
	cmp.l	d6,d3
	bpl	dg_NoClipStartY
	sub.l	d6,d3
	neg.l	d3
	bra	dg_ClipEndY
dg_NoClipStartY:
	move.l	d3,d6
	moveq	#0,d3
dg_ClipEndY:
	cmp.l	d7,d1
	bpl	dg_DoClipEndY
	move.l	d1,d7
dg_DoClipEndY:

;-- Prepare for loops: ----------------------------------------------------

	lsl.l	#2,d6
	move.l	obm_Line(a2),a5
	lea	(a5,d6.l),a5			; a5.l -> destination line pointer array.
	lsr.l	#2,d6

	sub.l	d6,d7				; d7.l =  y loop counter.

	move.w	#4,a2

	move.l	ge_TotalElements(a3),d0
	move.l	dga_Code(a0),d1
	divu	d0,d1
	swap	d1
	move.l	ge_Height(a3),d0
	mulu	d0,d1
	add.l	d3,d1
	tst.l	dga_FlipY(a0)
	beq	dg_NoFlipY
	add.l	d7,d1
	move.w	#-4,a2
dg_NoFlipY:
	lsl.l	#2,d1
	move.l	ge_BitMap(a3),a4
	move.l	obm_Line(a4),a4
	lea	(a4,d1.l),a4			; a4.l -> source line pointer array.

	sub.l	d4,d5				; d5.l =  x loop counter.

	tst.l	dga_FlipX(a0)
	beq	dg_NoFlipX
	neg.l	d2
	add.l	ge_Width(a3),d2
dg_NoFlipX:

;-- Update delta buffer: --------------------------------------------------

;	move.l	_DeltaBuf,d0
;	beq	dg_NoDeltaBuf
;
;	movem.l	d2/d4,-(sp)
;
;	move.l	d0,a1
;
;	move.l	_DeltaHShift,d0
;	lsr.l	d0,d6
;	mulu	_DeltaW,d6
;	move.l	d7,d2
;	add.l	_DeltaH,d2
;	add.l	d6,a1
;	lsr.l	d0,d2
;
;	move.l	_DeltaWShift,d0
;	lsr.l	d0,d4
;	move.l	d5,d1
;	add.l	_DeltaW,d1
;	add.l	d4,a1
;	lsr.l	d0,d1
;
;	moveq	#1,d0
;	move.l	_DeltaW,d6
;	sub.l	d1,d6
;	subq.l	#1,d1
;	subq.l	#1,d2
;
;dg_DeltaYLoop:
;	move.l	d1,d4
;dg_DeltaXLoop:
;	move.b	d0,(a1)+
;	dbf	d4,dg_DeltaXLoop
;	add.l	d6,a1
;	dbf	d2,dg_DeltaYLoop
;
;	movem.l	(sp)+,d2/d4
;dg_NoDeltaBuf:

;-- Color parameters: -----------------------------------------------------

	move.l	ge_ColorTable(a3),d6
	beq	dg_NoColorOffset
	move.l	dga_Color(a0),d0
	move.l	ge_TotalColors(a3),d3
	divu	d3,d0
	swap	d0
	move.l	ge_ColorGranuality(a3),d3
	mulu	d0,d3
	add.l	d3,d6
	move.l	d6,a6
dg_NoColorOffset:

	move.l	dga_Trans(a0),d3
	beq	dg_TransNone
	move.l	dga_TransColor(a0),d0
	cmp.l	#TRANS_PEN,d3
	beq	dg_TransPen
	move.l	_Machine,a1
	move.l	rm_Pens(a1),a1
	move.b	(a1,d0.w),d0
	cmp.l	#TRANS_THROUGH,d3
	beq	dg_TransThrough

;-- Transparent color: ----------------------------------------------------

dg_TransColor:
	moveq	#0,d3

	tst.l	d6
	beq	dg_TPCNoRemap

	tst.l	dga_FlipX(a0)
	bne	dg_TCYLoopFlip

dg_TCYLoop:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TCXLoop:
	move.b	(a0)+,d3
	move.b	(a6,d3.w),d3
	cmp.b	d0,d3
	beq	dg_TCTrans
	move.b	d3,(a1)
dg_TCTrans:
	addq.l	#1,a1
	dbf	d1,dg_TCXLoop
	add.l	a2,a4
	dbf	d7,dg_TCYLoop

	bra	dg_Done

dg_TCYLoopFlip:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TCXLoopFlip:
	move.b	-(a0),d3
	move.b	(a6,d3.w),d3
	cmp.b	d0,d3
	beq	dg_TCTransFlip
	move.b	d3,(a1)
dg_TCTransFlip:
	addq.l	#1,a1
	dbf	d1,dg_TCXLoopFlip
	add.l	a2,a4
	dbf	d7,dg_TCYLoopFlip

	bra	dg_Done

;-- Transparent pen: ------------------------------------------------------

dg_TransPen:
	moveq	#0,d3

	tst.l	d6
	beq	dg_TPCNoRemap

	tst.l	dga_FlipX(a0)
	bne	dg_TPYLoopFlip

dg_TPYLoop:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TPXLoop:
	move.b	(a0)+,d3
	cmp.b	d0,d3
	beq	dg_TPTrans
	move.b	(a6,d3.w),d3
	move.b	d3,(a1)
dg_TPTrans:
	addq.l	#1,a1
	dbf	d1,dg_TPXLoop
	add.l	a2,a4
	dbf	d7,dg_TPYLoop

	bra	dg_Done

dg_TPYLoopFlip:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TPXLoopFlip:
	move.b	-(a0),d3
	cmp.b	d0,d3
	beq	dg_TPTransFlip
	move.b	(a6,d3.w),d3
	move.b	d3,(a1)
dg_TPTransFlip:
	addq.l	#1,a1
	dbf	d1,dg_TPXLoopFlip
	add.l	a2,a4
	dbf	d7,dg_TPYLoopFlip

	bra	dg_Done

;-- Transparent pen or color with no remapping: ---------------------------

dg_TPCNoRemap:
	tst.l	dga_FlipX(a0)
	bne	dg_TPCYLoopFlipNR

dg_TPCYLoopNR:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TPCXLoopNR:
	move.b	(a0)+,d3
	cmp.b	d0,d3
	beq	dg_TPCTransNR
	move.b	d3,(a1)
dg_TPCTransNR:
	addq.l	#1,a1
	dbf	d1,dg_TPCXLoopNR
	add.l	a2,a4
	dbf	d7,dg_TPCYLoopNR

	bra	dg_Done

dg_TPCYLoopFlipNR:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TPCXLoopFlipNR:
	move.b	-(a0),d3
	cmp.b	d0,d3
	beq	dg_TPCTransFlipNR
	move.b	d3,(a1)
dg_TPCTransFlipNR:
	addq.l	#1,a1
	dbf	d1,dg_TPCXLoopFlipNR
	add.l	a2,a4
	dbf	d7,dg_TPCYLoopFlipNR

	bra	dg_Done

;-- No transparenncy: -----------------------------------------------------

dg_TransNone:
	moveq	#0,d3

	tst.l	d6
	beq	dg_TNNoRemap

	tst.l	dga_FlipX(a0)
	bne	dg_TNYLoopFlip

dg_TNYLoop:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TNXLoop:
	move.b	(a0)+,d3
	move.b	(a6,d3.w),d3
	move.b	d3,(a1)+
	dbf	d1,dg_TNXLoop
	add.l	a2,a4
	dbf	d7,dg_TNYLoop

	bra	dg_Done

dg_TNYLoopFlip:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TNXLoopFlip:
	move.b	-(a0),d3
	move.b	(a6,d3.w),d3
	move.b	d3,(a1)+
	dbf	d1,dg_TNXLoopFlip
	add.l	a2,a4
	dbf	d7,dg_TNYLoopFlip

	bra	dg_Done

dg_TNNoRemap:
	moveq	#3,d3

	tst.l	dga_FlipX(a0)
	bne	dg_TNYLoopFlipNR

	move.l	d2,d1
	eor.l	d4,d1
	and.l	d3,d1
	bne	dg_TNYLoopNR
	move.l	d2,d1
	or.l	d4,d1
	and.l	d3,d1
	beq	dg_TNAlignedNR

	movem.l	a4/a5/d7,-(sp)

	move.l	d4,d1
	and.l	d3,d1
	subq.l	#1,d1
	beq	dg_TN1LoopNR
	subq.l	#1,d1
	beq	dg_TN2LoopNR

dg_TN3LoopNR:
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
	move.b	(a0)+,(a1)+
	move.b	(a0)+,(a1)+
	move.b	(a0)+,(a1)+
	add.l	a2,a4
	dbf	d7,dg_TN3LoopNR

	bra	dg_TNDoAlignedNR

dg_TN2LoopNR:
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
	move.b	(a0)+,(a1)+
	move.b	(a0)+,(a1)+
	add.l	a2,a4
	dbf	d7,dg_TN3LoopNR

	bra	dg_TNDoAlignedNR

dg_TN1LoopNR:
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
	move.b	(a0),(a1)
	add.l	a2,a4
	dbf	d7,dg_TN3LoopNR

dg_TNDoAlignedNR:
	movem.l	(sp)+,a4/a5/d7
	move.l	d4,d1
	and.l	d3,d1
	add.l	d1,d2
	add.l	d1,d4
	sub.l	d1,d5

dg_TNAlignedNR:
	move.l	d5,d1
	lsr.l	#2,d1
	beq	dg_TNYLoopNR

	movem.l	a4/a5/d5/d7,-(sp)

dg_TNYLoopANR:
	move.l	d1,d5
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TNXLoopANR:
	move.l	(a0)+,(a1)+
	dbf	d5,dg_TNXLoopANR
	add.l	a2,a4
	dbf	d7,dg_TNYLoopANR

	movem.l	(sp)+,a4/a5/d5/d7
	lsl.l	#2,d1
	add.l	d1,d2
	add.l	d1,d4
	sub.l	d1,d5

dg_TNYLoopNR:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TNXLoopNR:
	move.b	(a0)+,(a1)+
	dbf	d1,dg_TNXLoopNR
	add.l	a2,a4
	dbf	d7,dg_TNYLoopNR

	bra	dg_Done

dg_TNYLoopFlipNR:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TNXLoopFlipNR:
	move.b	-(a0),(a1)+
	dbf	d1,dg_TNXLoopFlipNR
	add.l	a2,a4
	dbf	d7,dg_TNYLoopFlipNR

	bra	dg_Done

;-- Write though transparent destination: ---------------------------------

dg_TransThrough:
	moveq	#0,d3

	tst.l	d6
	beq	dg_TTNoRemap

	tst.l	dga_FlipX(a0)
	bne	dg_TTYLoopFlip

dg_TTYLoop:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TTXLoop:
	cmp.b	(a1),d0
	bne	dg_TTTrans
	move.b	(a0),d3
	move.b	(a6,d3.w),d3
	move.b	d3,(a1)
dg_TTTrans:
	addq.l	#1,a0
	addq.l	#1,a1
	dbf	d1,dg_TTXLoop
	add.l	a2,a4
	dbf	d7,dg_TTYLoop

	bra	dg_Done

dg_TTYLoopFlip:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TTXLoopFlip:
	subq.l	#1,a0
	cmp.b	(a1),d0
	bne	dg_TTTransFlip
	move.b	(a0),d3
	move.b	(a6,d3.w),d3
	move.b	d3,(a1)
dg_TTTransFlip:
	addq.l	#1,a1
	dbf	d1,dg_TTXLoopFlip
	add.l	a2,a4
	dbf	d7,dg_TTYLoopFlip

	bra	dg_Done

dg_TTNoRemap:
	tst.l	dga_FlipX(a0)
	bne	dg_TTYLoopFlipNR

dg_TTYLoopNR:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TTXLoopNR:
	cmp.b	(a1),d0
	bne	dg_TTTransNR
	move.b	(a0),(a1)
dg_TTTransNR:
	addq.l	#1,a0
	addq.l	#1,a1
	dbf	d1,dg_TTXLoopNR
	add.l	a2,a4
	dbf	d7,dg_TTYLoopNR

	bra	dg_Done

dg_TTYLoopFlipNR:
	move.l	d5,d1
	move.l	(a4),a0
	move.l	(a5)+,a1
	add.l	d2,a0
	add.l	d4,a1
dg_TTXLoopFlipNR:
	subq.l	#1,a0
	cmp.b	(a1),d0
	bne	dg_TTTransFlipNR
	move.b	(a0),(a1)
dg_TTTransFlipNR:
	addq.l	#1,a1
	dbf	d1,dg_TTXLoopFlipNR
	add.l	a2,a4
	dbf	d7,dg_TTYLoopFlipNR

dg_Done:
	movem.l	(sp)+,d2-d7/a2-a6
	rts
	ENDC

	IFD	DEBUG
DumpFormat:
	dc.b	"%08lx",10,0
	even
	ENDC

***************************************************************************

	IFD CUSTOMDRAWGFX
_clearbitmap:
	move.l	4(sp),a0
;	move.l	_DeltaBuf,d0
;	beq	cbm_ClearBitMap
;
;	move.l	d0,a1
;	move.l	_DeltaW,d0
;	move.l	_DeltaH,d1
;	mulu	d0,d1
;	subq.l	#1,d1
;	moveq	#1,d0
;cbm_DeltaLoop:
;	move.b	d0,(a1)+
;	dbf	d1,cbm_DeltaLoop

cbm_ClearBitMap:
	move.l	d2,-(sp)
	move.l	obm_Width(a0),d0
	beq	cbm_Done
	move.l	obm_Height(a0),d1
	beq	cbm_Done
	move.l	obm_Line(a0),a0
	subq.l	#1,d0
	subq.l	#1,d1
cbm_ClearYLoop:
	move.l	(a0)+,a1
	move.l	d0,d2
cbm_ClearXLoop:
	clr.b	(a1)+
	dbf	d2,cbm_ClearXLoop
	dbf	d1,cbm_ClearYLoop

cbm_Done:
	move.l	(sp)+,d2
	rts
	ENDC

***************************************************************************

; a0.l -> source buffer.
; a1.l -> destination buffer.
; a2.l -> remap table.
; d0.l =  source width.
; d1.l =  destination bytes per row.
; d2.l =  height.
; d3.l =  source bytes per row.

_RemapPixels:
	movem.l	a3/d2-d7,-(sp)
	sub.l	d0,d1
	sub.l	d0,d3
	move.l	d3,a3
	move.l	d0,d3
	lsr.l	#2,d0
	lsl.l	#2,d0
	sub.l	d0,d3
	lsr.l	#2,d0
	subq.l	#1,d0
	subq.l	#1,d2
	subq.l	#1,d3
	moveq	#0,d7
r_YLoop:
	move.l	d0,d4
r_X1Loop:
	move.l	(a0)+,d5
	rol.l	#8,d5
	move.b	d5,d7
	move.b	(a2,d7.w),d6
	lsl.l	#8,d6
	rol.l	#8,d5
	move.b	d5,d7
	move.b	(a2,d7.w),d6
	lsl.l	#8,d6
	rol.l	#8,d5
	move.b	d5,d7
	move.b	(a2,d7.w),d6
	lsl.l	#8,d6
	rol.l	#8,d5
	move.b	d5,d7
	move.b	(a2,d7.w),d6
	move.l	d6,(a1)+
	dbf	d4,r_X1Loop
	move.l	d3,d4
	bmi	r_X2Done
r_X2Loop:
	move.b	(a0)+,d7
	move.b	(a2,d7.w),(a1)+
	dbf	d4,r_X2Loop
	addq.l	#3,a1
	sub.l	d3,a1
r_X2Done:
	add.l	d1,a1
	add.l	a3,a0

	dbf	d2,r_YLoop
	movem.l	(sp)+,a3/d2-d7
	rts


	IFD CUSTOMMEM
_mrh_ram:
_mrh_error:
	move.l	4(sp),a0
	add.l	_RAM,a0
	moveq	#0,d0
	move.b	(a0),d0
	rts

_mrh_bank1:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+4,a0
	moveq	#0,d0
	move.b	(a0),d0
	rts

_mrh_bank2:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+8,a0
	moveq	#0,d0
	move.b	(a0),d0
	rts

_mrh_bank3:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+12,a0
	moveq	#0,d0
	move.b	(a0),d0
	rts

_mrh_bank4:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+16,a0
	moveq	#0,d0
	move.b	(a0),d0
	rts

_mrh_bank5:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+20,a0
	moveq	#0,d0
	move.b	(a0),d0
	rts

_mrh_nop:
_mrh_error_sparse:
	moveq	#0,d0
	rts

_mwh_ram:
_mwh_error:
	move.l	4(sp),a0
	add.l	_RAM,a0
	move.b	11(sp),(a0)
	rts

_mwh_bank1:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+4,a0
	move.b	11(sp),(a0)
	rts

_mwh_bank2:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+8,a0
	move.b	11(sp),(a0)
	rts

_mwh_bank3:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+12,a0
	move.b	11(sp),(a0)
	rts

_mwh_bank4:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+16,a0
	move.b	11(sp),(a0)
	rts

_mwh_bank5:
	move.l	4(sp),a0
	add.l	_cpu_bankbase+20,a0
	move.b	11(sp),(a0)
	rts

_mwh_nop:
_mwh_error_sparse:
_mwh_rom:
	rts

_mwh_ramrom:
	move.l	4(sp),a0
	move.l	_RAM,a1
	move.b	11(sp),(a0,a1.l)
	move.l	_ROM,a1
	move.b	11(sp),(a0,a1.l)
	rts

_cpu_readmem16:
	move.l	4(sp),d0
	move.l	d0,a1
	lsr.l	#ABITS2_16+ABITS_MIN_16,d0
	move.l	_cur_mrhard,a0
	move.b	(a0,d0.l),d0
	bne	crm16_NotRAM
	add.l	_RAM,a1
	moveq	#0,d0
	move.b	(a1),d0
	rts
crm16_NotRAM:
	cmp.b	#MH_HARDMAX,d0
	bmi	crm16_NotRAM2
	sub.b	#MH_HARDMAX,d0
	lsl.w	#MH_SBITS,d0			; MH_SBITS = 8.
	move.l	a1,d1
;	lsr.l	#ABITS_MIN_16,d1		; ABITS_MIN_16 = 0.
	and.l	#$f,d1				; $ffffffff >> (32 - ABITS2_16).
	lea	_readhardware,a0
	add.w	d0,a0
	move.b	(a0,d1.l),d0
	bne	crm16_NotRAM2
	add.l	_RAM,a1
	moveq	#0,d0
	move.b	(a1),d0
	rts
crm16_NotRAM2:
	moveq	#0,d1
	move.b	d0,d1
	lsl.l	#2,d1
	lea	_memoryreadoffset,a0
	sub.l	(a0,d1.l),a1
	move.l	a1,4(sp)
	lea	_memoryreadhandler,a0
	move.l	(a0,d1.l),a0
	jmp	(a0)

_cpu_writemem16:
	move.l	4(sp),d0
	move.l	d0,a1
	lsr.l	#ABITS2_16+ABITS_MIN_16,d0
	move.l	_cur_mwhard,a0
	move.b	(a0,d0.l),d0
	bne	cwm16_NotRAM
	add.l	_RAM,a1
	move.b	11(sp),(a1)
	rts
cwm16_NotRAM:
	cmp.b	#MH_HARDMAX,d0
	bmi	cwm16_NotRAM2
	sub.b	#MH_HARDMAX,d0
	lsl.w	#MH_SBITS,d0
	move.l	a1,d1
;	lsr.l	#ABITS_MIN_16,d1		; ABITS_MIN_16 = 0.
	and.l	#$f,d1				; $ffffffff >> (32 - ABITS2_16).
	lea	_writehardware,a0
	add.w	d0,a0
	move.b	(a0,d1.l),d0
	bne	cwm16_NotRAM2
	add.l	_RAM,a1
	move.b	11(sp),(a1)
	rts
cwm16_NotRAM2:
	moveq	#0,d1
	move.b	d0,d1
	lsl.l	#2,d1
	lea	_memorywriteoffset,a0
	sub.l	(a0,d1.l),a1
	move.l	a1,4(sp)
	lea	_memorywritehandler,a0
	move.l	(a0,d1.l),a0
	jmp	(a0)

_cpu_readmem20:
	move.l	4(sp),d0
	move.l	d0,a1
	lsr.l	#ABITS2_20+ABITS_MIN_20,d0
	move.l	_cur_mrhard,a0
	move.b	(a0,d0.l),d0
	bne	crm20_NotRAM
	add.l	_RAM,a1
	moveq	#0,d0
	move.b	(a1),d0
	rts
crm20_NotRAM:
	cmp.b	#MH_HARDMAX,d0
	bmi	crm20_NotRAM2
	sub.b	#MH_HARDMAX,d0
	lsl.w	#MH_SBITS,d0			; MH_SBITS = 8.
	move.l	a1,d1
;	lsr.l	#ABITS_MIN_20,d1		; ABITS_MIN_20 = 0.
	and.l	#$ff,d1				; $ffffffff >> (32 - ABITS2_20).
	lea	_readhardware,a0
	add.w	d0,a0
	move.b	(a0,d1.l),d0
	bne	crm20_NotRAM2
	add.l	_RAM,a1
	moveq	#0,d0
	move.b	(a1),d0
	rts
crm20_NotRAM2:
	moveq	#0,d1
	move.b	d0,d1
	lsl.l	#2,d1
	lea	_memoryreadoffset,a0
	sub.l	(a0,d1.l),a1
	move.l	a1,4(sp)
	lea	_memoryreadhandler,a0
	move.l	(a0,d1.l),a0
	jmp	(a0)

_cpu_writemem20:
	move.l	4(sp),d0
	move.l	d0,a1
	lsr.l	#ABITS2_20+ABITS_MIN_20,d0
	move.l	_cur_mwhard,a0
	move.b	(a0,d0.l),d0
	bne	cwm20_NotRAM
	add.l	_RAM,a1
	move.b	11(sp),(a1)
	rts
cwm20_NotRAM:
	cmp.b	#MH_HARDMAX,d0
	bmi	cwm20_NotRAM2
	sub.b	#MH_HARDMAX,d0
	lsl.w	#MH_SBITS,d0
	move.l	a1,d1
;	lsr.l	#ABITS_MIN_20,d1		; ABITS_MIN_20 = 0.
	and.l	#$ff,d1				; $ffffffff >> (32 - ABITS2_20).
	lea	_writehardware,a0
	add.w	d0,a0
	move.b	(a0,d1.l),d0
	bne	cwm20_NotRAM2
	add.l	_RAM,a1
	move.b	11(sp),(a1)
	rts
cwm20_NotRAM2:
	moveq	#0,d1
	move.b	d0,d1
	lsl.l	#2,d1
	lea	_memorywriteoffset,a0
	sub.l	(a0,d1.l),a1
	move.l	a1,4(sp)
	lea	_memorywritehandler,a0
	move.l	(a0,d1.l),a0
	jmp	(a0)

_cpu_readmem24:
	move.l	4(sp),d0
	move.l	d0,a1
	lsr.l	#ABITS2_24,d0
	lsr.l	#ABITS_MIN_24,d0
	move.l	_cur_mrhard,a0
	move.b	(a0,d0.l),d0
	bne	crm24_NotRAM
	add.l	_RAM,a1
	moveq	#0,d0
	move.b	(a1),d0
	rts
crm24_NotRAM:
	cmp.b	#MH_HARDMAX,d0
	bmi	crm24_NotRAM2
	sub.b	#MH_HARDMAX,d0
	lsl.w	#MH_SBITS,d0			; MH_SBITS = 8.
	move.l	a1,d1
	lsr.l	#ABITS_MIN_24,d1		; ABITS_MIN_24 = 0.
	and.l	#$ff,d1				; $ffffffff >> (32 - ABITS2_24).
	lea	_readhardware,a0
	add.w	d0,a0
	move.b	(a0,d1.l),d0
	bne	crm24_NotRAM2
	add.l	_RAM,a1
	moveq	#0,d0
	move.b	(a1),d0
	rts
crm24_NotRAM2:
	moveq	#0,d1
	move.b	d0,d1
	lsl.l	#2,d1
	lea	_memoryreadoffset,a0
	sub.l	(a0,d1.l),a1
	move.l	a1,4(sp)
	lea	_memoryreadhandler,a0
	move.l	(a0,d1.l),a0
	jmp	(a0)

_cpu_writemem24:
	move.l	4(sp),d0
	move.l	d0,a1
	lsr.l	#ABITS2_24,d0
	lsr.l	#ABITS_MIN_24,d0
	move.l	_cur_mwhard,a0
	move.b	(a0,d0.l),d0
	bne	cwm24_NotRAM
	add.l	_RAM,a1
	move.b	11(sp),(a1)
	rts
cwm24_NotRAM:
	cmp.b	#MH_HARDMAX,d0
	bmi	cwm24_NotRAM2
	sub.b	#MH_HARDMAX,d0
	lsl.w	#MH_SBITS,d0
	move.l	a1,d1
	lsr.l	#ABITS_MIN_24,d1		; ABITS_MIN_24 = 2.
	and.l	#$ff,d1				; $ffffffff >> (32 - ABITS2_24).
	lea	_writehardware,a0
	add.w	d0,a0
	move.b	(a0,d1.l),d0
	bne	cwm24_NotRAM2
	add.l	_RAM,a1
	move.b	11(sp),(a1)
	rts
cwm24_NotRAM2:
	moveq	#0,d1
	move.b	d0,d1
	lsl.l	#2,d1
	lea	_memorywriteoffset,a0
	sub.l	(a0,d1.l),a1
	move.l	a1,4(sp)
	lea	_memorywritehandler,a0
	move.l	(a0,d1.l),a0
	jmp	(a0)

	ENDC
