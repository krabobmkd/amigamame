;
; Date: 24-Sep-1997			Mikael Kalms (Scout/C-Lous & more)
;
; 1x1 8bpl cpu5 C2P for arbitrary BitMaps
;
; Features:
; Performs CPU-only C2P conversion using rather state-of-the-art (as of
; the creation date, anyway) techniques
; Different routines for non-modulo and modulo C2P conversions
; Handles bitmaps of virtually any size (>4096x4096)
;
; Restrictions:
; Chunky-buffer must be an even multiple of 32 pixels wide
; X-Offset must be set to an even multiple of 8
; If these conditions not are met, the routine will abort.
; If incorrect/invalid parameters are specified, the routine will
; most probably crash.
;
; c2p1x1_8_c5_bm
;
; Changes by Mats Eirik Hansen (mats.hansen@triumph.no)
;
; - Supports modulo on the chunky source buffer. This was needed by
;   MAME 0.31 and higher.
; - Supports depths from 1 to 8. For depths from 1 to 4 I've removed
;   the whole second stage, but otherwise I've only removed the
;   write for each unneeded bitplane.
;


	XDEF	_c2p

	incdir	include:
	include	graphics/gfx.i

	section	code,code

; d0.w	chunkyx [chunky-pixels]
; d1.w	chunkyy [chunky-pixels]
; d2.w	offsx [screen-pixels]
; d3.w	offsy [screen-pixels]
; d4.l  chunky bytes per row
; a0	chunkyscreen
; a1	BitMap

_c2p:
c2p:
	movem.l	d2-d7/a2-a6,-(sp)

	move.w	d1,c2p_chunkyy
	beq	.exit

	move.l	d4,c2p_srcbpr
	sub.l	d0,d4
	move.l	d4,c2p_srcmod
					; A few sanity checks
	move.w	d0,d4
	move.w	d2,d5
	andi.w	#$1f,d4			; Even 32-pixel width?
	bne	.exit
	andi.w	#$7,d5			; Even 8-pixel xoffset?
	bne	.exit
	moveq	#0,d4
	move.w	bm_BytesPerRow(a1),d4

	move.w	d0,c2p_chunkyx ; Skip if 0 pixels to convert
	beq	.exit

	ext.l	d2			; Offs to first pixel to draw in bpl
	mulu.w	d4,d3
	lsr.l	#3,d2
	add.l	d2,d3

	lsl.l	#3,d4
	sub.l	d0,d4
	lsr.l	#3,d4
	move.l	d4,c2p_rowmod ; Modulo between two rows

	move.l	a0,a2			; Ptr to end of line + 1 iter
	add.w	c2p_chunkyx(pc),a2

	cmpi.b	#8,bm_Depth(a1)
	beq	.8start
	cmpi.b	#7,bm_Depth(a1)
	beq	.7start
	cmpi.b	#6,bm_Depth(a1)
	beq	.6start
	cmpi.b	#5,bm_Depth(a1)
	beq	.5start
	cmpi.b	#4,bm_Depth(a1)
	beq	.4start
	cmpi.b	#3,bm_Depth(a1)
	beq	.3start
	cmpi.b	#2,bm_Depth(a1)
	beq	.2start
	cmpi.b	#1,bm_Depth(a1)
	blo	.exit

.1start
	movem.l	a0-a2/d1/d3,-(sp)

	movem.l	bm_Planes(a1),a3-a6	; Setup ptrs to bpl0-3
	add.l	d3,a3

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.1modx1start
.1modx1y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
.1modx1
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
.1modx1start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.1modx1

	subq.w	#1,c2p_chunkyy
	beq	.4exit

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.1modx1y

.2start
	movem.l	a0-a2/d1/d3,-(sp)

	movem.l	bm_Planes(a1),a3-a6	; Setup ptrs to bpl0-3
	add.l	d3,a3
	add.l	d3,a4

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.2modx1start
.2modx1y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
.2modx1
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
.2modx1start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.2modx1

	subq.w	#1,c2p_chunkyy
	beq	.4exit

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.2modx1y

.3start
	movem.l	a0-a2/d1/d3,-(sp)

	movem.l	bm_Planes(a1),a3-a6	; Setup ptrs to bpl0-3
	add.l	d3,a3
	add.l	d3,a4
	add.l	d3,a5

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.3modx1start
.3modx1y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
	add.l	d0,a5
.3modx1
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
.3modx1start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.3modx1

	subq.w	#1,c2p_chunkyy
	beq	.4exit

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.3modx1y

.4start
	movem.l	a0-a2/d1/d3,-(sp)

	movem.l	bm_Planes(a1),a3-a6	; Setup ptrs to bpl0-3
	add.l	d3,a3
	add.l	d3,a4
	add.l	d3,a5
	add.l	d3,a6

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.4modx1start
.4modx1y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
	add.l	d0,a5
	add.l	d0,a6
.4modx1
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+
.4modx1start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.4modx1

	subq.w	#1,c2p_chunkyy
	beq	.4exit

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.4modx1y

.5start
	movem.l	a0-a2/d1/d3,-(sp)

	movem.l	bm_Planes(a1),a3-a6	; Setup ptrs to bpl0-3
	add.l	d3,a3
	add.l	d3,a4
	add.l	d3,a5
	add.l	d3,a6

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.5modx1start
.5modx1y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
	add.l	d0,a5
	add.l	d0,a6
.5modx1
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+
.5modx1start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.5modx1

	subq.w	#1,c2p_chunkyy
	beq	.5modx2init

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.5modx1y

.5modx2init
	movem.l	(sp)+,a0-a2/d1/d3

	move.w	d1,c2p_chunkyy

	movem.l	bm_Planes+4*4(a1),a3-a6	; Setup ptrs to bpl4-7
	add.l	d3,a3

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.5modx2start

.5modx2y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
.5modx2
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
.5modx2start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.5modx2

	subq.w	#1,c2p_chunkyy
	beq	.exit

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.5modx2y

.6start
	movem.l	a0-a2/d1/d3,-(sp)

	movem.l	bm_Planes(a1),a3-a6	; Setup ptrs to bpl0-3
	add.l	d3,a3
	add.l	d3,a4
	add.l	d3,a5
	add.l	d3,a6

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.6modx1start
.6modx1y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
	add.l	d0,a5
	add.l	d0,a6
.6modx1
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+
.6modx1start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.6modx1

	subq.w	#1,c2p_chunkyy
	beq	.6modx2init

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.6modx1y

.6modx2init
	movem.l	(sp)+,a0-a2/d1/d3

	move.w	d1,c2p_chunkyy

	movem.l	bm_Planes+4*4(a1),a3-a6	; Setup ptrs to bpl4-7
	add.l	d3,a3
	add.l	d3,a4

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.6modx2start

.6modx2y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
.6modx2
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
.6modx2start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.6modx2

	subq.w	#1,c2p_chunkyy
	beq	.exit

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.6modx2y

.7start
	movem.l	a0-a2/d1/d3,-(sp)

	movem.l	bm_Planes(a1),a3-a6	; Setup ptrs to bpl0-3
	add.l	d3,a3
	add.l	d3,a4
	add.l	d3,a5
	add.l	d3,a6

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.7modx1start
.7modx1y
	add.l	c2p_srcbpr(pc),a2 	; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 	; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
	add.l	d0,a5
	add.l	d0,a6
.7modx1
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+
.7modx1start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.7modx1

	subq.w	#1,c2p_chunkyy
	beq	.7modx2init

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.7modx1y

.7modx2init
	movem.l	(sp)+,a0-a2/d1/d3

	move.w	d1,c2p_chunkyy

	movem.l	bm_Planes+4*4(a1),a3-a6	; Setup ptrs to bpl4-7
	add.l	d3,a3
	add.l	d3,a4
	add.l	d3,a5
	add.l	d3,a6

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.7modx2start

.7modx2y
	add.l	c2p_srcbpr(pc),a2 	; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 	; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
	add.l	d0,a5
	add.l	d0,a6
.7modx2
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
.7modx2start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.7modx2

	subq.w	#1,c2p_chunkyy
	beq	.exit

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.7modx2y

.8start
	movem.l	a0-a2/d1/d3,-(sp)

	movem.l	bm_Planes(a1),a3-a6	; Setup ptrs to bpl0-3
	add.l	d3,a3
	add.l	d3,a4
	add.l	d3,a5
	add.l	d3,a6

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.8modx1start
.8modx1y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
	add.l	d0,a5
	add.l	d0,a6
.8modx1
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+
.8modx1start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.8modx1

	subq.w	#1,c2p_chunkyy
	beq	.8modx2init

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$0f0f0f0f,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsl.l	#4,d0
	lsl.l	#4,d1
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$0f0f0f0f,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsl.l	#4,d2
	lsl.l	#4,d3
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.8modx1y

.8modx2init
	movem.l	(sp)+,a0-a2/d1/d3

	move.w	d1,c2p_chunkyy

	movem.l	bm_Planes+4*4(a1),a3-a6	; Setup ptrs to bpl4-7
	add.l	d3,a3
	add.l	d3,a4
	add.l	d3,a5
	add.l	d3,a6

	move.l	(a0)+,d0		; Convert lower 4 bpls
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	bra	.8modx2start

.8modx2y
	add.l	c2p_srcbpr(pc),a2 ; Skip to end of next line
	move.l	c2p_rowmod(pc),d0 ; Skip to beginning of next line
	add.l	d0,a3
	add.l	d0,a4
	add.l	d0,a5
	add.l	d0,a6
.8modx2
	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+
.8modx2start
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	cmpa.l	a0,a2
	bne	.8modx2

	subq.w	#1,c2p_chunkyy
	beq	.exit

	add.l	c2p_srcmod(pc),a0

	move.l	(a0)+,d0
	move.l	(a0)+,d2
	move.l	(a0)+,d1
	move.l	(a0)+,d3
	move.l	d7,(a3)+

	move.l	#$f0f0f0f0,d6		; Merge 4x1, part 1
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	and.l	d6,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	or.l	d2,d0
	or.l	d3,d1

	move.l	(a0)+,d2
	move.l	(a0)+,d6
	move.l	(a0)+,d3
	move.l	(a0)+,d7
	move.l	d4,(a4)+

	move.l	#$f0f0f0f0,d4		; Merge 4x1, part 2
	and.l	d4,d2
	and.l	d4,d6
	and.l	d4,d3
	and.l	d4,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	or.l	d6,d2
	or.l	d7,d3

	move.w	d2,d6			; Swap 16x2
	move.w	d3,d7
	move.w	d0,d2
	move.w	d1,d3
	swap	d2
	swap	d3
	move.w	d2,d0
	move.w	d3,d1
	move.w	d6,d2
	move.w	d7,d3
	move.l	d5,(a5)+

	move.l	#$33333333,d4
	move.l	d2,d6			; Swap 2x2
	move.l	d3,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	#$00ff00ff,d4
	move.l	d1,d6			; Swap 8x1
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d2,d7
	move.l	a1,(a6)+

	and.l	d4,d6
	and.l	d4,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	#$55555555,d4
	move.l	d1,d5			; Swap 1x1
	move.l	d3,d7
	lsr.l	#1,d5
	lsr.l	#1,d7
	eor.l	d0,d5
	eor.l	d2,d7
	and.l	d4,d5
	and.l	d4,d7
	eor.l	d5,d0
	eor.l	d7,d2
	add.l	d5,d5
	add.l	d7,d7
	eor.l	d1,d5
	eor.l	d3,d7

	move.l	d0,a1
	move.l	d2,d4

	bra	.8modx2y

.4exit:
	movem.l	(sp)+,a0-a2/d1/d3
.exit:
	movem.l	(sp)+,d2-d7/a2-a6
.earlyexit:
	rts

c2p_chunkyx	ds.w	1
c2p_chunkyy	ds.w	1
c2p_rowmod	ds.l	1
c2p_srcmod	ds.l	1
c2p_srcbpr	ds.l	1
