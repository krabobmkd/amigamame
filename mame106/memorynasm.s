
	include exec/types.i


	STRUCTURE directDrawScreen,0
		APTR	dsc_base
		ULONG	dsc_bpr
		WORD	dsc_clipX1
		WORD	dsc_clipY1
		WORD	dsc_clipX2
		WORD	dsc_clipY2
	LABEL	dsc_sizeof

	XDEF	memory_readlong_d16_be
	XDEF	_memory_readlong_d16_be

	section	code,code

memory_readlong_d16_be:
_memory_readlong_d16_be:
;	movem.l a2/d3/d2,-(sp)
.LCFI2:
	and.l _active_address_space,d0
	move.l 4+_active_address_space,a0
	move.l d0,a1
	moveq #14,d1
	lsr.l d1,d0
	move.b (a0,d0.l),d1 ; d1 entry   a1 adress
;	move.b d1,d2
	cmp.l #67,d1
;	jls .L244
	bge.b .aft
;		addq.l #1,(_entriesDirectReadA,d1.l*4)
;		and.w #255,d1
		mulu.w #20,d1
		sub.l ([12+_active_address_space],d1.l,4),d0
		move.l (_bank_ptr,d2.l*4),a0
		and.l ([12+_active_address_space],d1.l,12),d0
		move.l (a0,d0.l),d0
	rts
.aft
	cmp.l #191,d2
	blt .L241
	add.l #-192,d2
	lsl.l #8,d2
	lsl.l #6,d2
	move.l #16383,d1
	and.l d0,d1
	add.l a0,d2
	move.l d1,a0
	clr.w d1
	move.b (262144,a0,d2.l),d1
	clr.l d2
	move.b d1,d2
	mulu.w #20,d1
	add.l 12+_active_address_space,d1
	move.l d1,a0
	sub.l (4,a0),d0
	and.l (12,a0),d0
	cmp.l #67,d2
	jhi .L242
	addq.l #1,(_entriesDirectReadB,d2.l*4)
	move.l (_bank_ptr,d2.l*4),a0
	move.l (a0,d0.l),d0
;	jra .L238
;.L244:
;	addq.l #1,(_entriesDirectReadA,d2.l*4)
;	and.w #255,d1
;	mulu.w #20,d1
;	sub.l ([12+_active_address_space],d1.l,4),d0
;	move.l (_bank_ptr,d2.l*4),a0
;	and.l ([12+_active_address_space],d1.l,12),d0
;	move.l (a0,d0.l),d0
.L238:
	movem.l (sp)+,d2/d3/a2
	rts
.L241:
	and.w #255,d1
	mulu.w #20,d1
	add.l 12+_active_address_space,d1
	move.l d1,a0
	sub.l (4,a0),d0
	and.l (12,a0),d0
.L242:
	addq.l #1,(_entriesOverread,d2.l*4)
	move.l d0,d2
	move.l (a0),a2
	lsr.l #1,d2
	clr.l d1
	move.l d2,d0
	jsr (a2)
	move.w d0,d3
	clr.l d1
	move.l d2,d0
	addq.l #1,d0
	jsr (a2)
	lsl.l #8,d3
	lsl.l #8,d3
	or.w d0,d3
	move.l d3,d0
	jra .L238
