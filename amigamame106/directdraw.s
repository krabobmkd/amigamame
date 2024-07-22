;struct directDrawScreen {
;    void *_base;
;    ULONG _bpr;
;    WORD _clipX1,_clipY1,_clipX2,_clipY2;
;};
;struct directDrawSource {
;    void *_base;
;    ULONG _bpr;
;    WORD _x1,_y1,_width,_height; // to be drawn.
;};
;// for any of the 8 16b target mode?
;    void directDrawClut16(register directDrawScreen *screen __asm("a0"),
;                    register directDrawSource *source __asm("a1"),
;                    register LONG x1 __asm("d0"),
;                    register LONG y1 __asm("d1"),
;                    register UBYTE *lut __asm("a2") // actually UWORD* or anywhat.
;                );
;	incdir	include:
;	include	graphics/gfx.i
	include exec/types.i

; from Mame driver.h
ORIENTATION_FLIP_X equ 1
ORIENTATION_FLIP_Y equ 2
ORIENTATION_SWAP_XY equ 4

	STRUCTURE directDrawScreen,0
		APTR	dsc_base
		ULONG	dsc_bpr
		WORD	dsc_clipX1
		WORD	dsc_clipY1
		WORD	dsc_clipX2
		WORD	dsc_clipY2
	LABEL	dsc_sizeof

	STRUCTURE directDrawSource,0
		APTR	dso_base
		ULONG	dso_bpr
		WORD	dso_x1
		WORD	dso_y1
		WORD	dso_x2
		WORD	dso_y2
	LABEL	dso_sizeof

	XDEF	directDrawClut16
	XDEF	_directDrawClut16

	XDEF directDrawClut32
	XDEF _directDrawClut32

	section	code,code

_directDrawClut16:
directDrawClut16:
	;a0 dsc_
	;a1 dso_
	;a2 lut .w
	;d0 xstart
	;d1 ystart
	; d2 swap flags
 	movem.l	d2-d7/a2-a6,-(sp)

	; - - - - - y
	; on source
	move.w	dso_y1(a1),d3  ; source start
	move.w	dso_y2(a1),d4 ; source end

	move.w	dsc_clipY1(a0),d2

; move.w	d2,asmval+2
; move.w	d4,asmval2+2


	; test complete y exit
	move.w	d1,d5
	add.w	d4,d5
	sub.w	d3,d5 ; d5=y1+height
	cmp.w	d5,d2
	bge		.endfunc

	cmp.w	d1,d2
	ble.b	.noclipy1
		add.w	d2,d3
		sub.w	d1,d3 ; ystartsource += dif

		move.w	d2,d1 ; screen start
.noclipy1

	move.w	dsc_clipY2(a0),d2
	; test complete y exit
	cmp.w	d1,d2
	ble		.endfunc

	move.w	d1,d5
	add.w	d4,d5
	sub.w	d3,d5 ; d5 end draw y2
	sub.w	d5,d2 ; d2 +clip dif
	bge.b	.noclipy2
		add.w	d2,d4 ; cut length
.noclipy2

	;d0 x1
	;d1 y1
	;d3 srcy1
	;d4 srcy2
	sub.w	d3,d4 ; d4 height to draw
	ble	.endfunc

	; - - - -  - -manage pointers
	; manage from source
	move.l	dso_base(a1),a3 ; ptr bm source
	move.l	dsc_base(a0),a4 ; ptr bm dest
	; shift source x1
	move.l	dsc_bpr(a0),d6	; need
	move.l	dso_bpr(a1),d7	; need
	mulu.w	d7,d3
	adda.l	d3,a3
	mulu.w	d6,d1
	adda.l	d1,a4
	; d1 d2 d3 free, d4 height
	; - -- - - - x
	move.w	dso_x1(a1),d1  ; source start
	move.w	dso_x2(a1),d3  ; source end

	move.w	dsc_clipX1(a0),d2

	; test complete x1 exit
	move.w	d0,d5
	add.w	d3,d5
	sub.w	d1,d5 ; d5=x1+width
	cmp.w	d5,d2
	bge		.endfunc

	cmp.w	d0,d2
	ble.b	.noclipx1
		add.w	d2,d1
		sub.w	d0,d1 ; xstartsource += dif

		move.w	d2,d0 ; screen start
.noclipx1

	move.w	dsc_clipX2(a0),d2
	; test complete x exit
	cmp.w	d0,d2
	ble		.endfunc

	move.w	d0,d5
	add.w	d3,d5
	sub.w	d1,d5 ; d5 end draw y2
	sub.w	d5,d2 ; d2 +clip dif
	bge.b	.noclipx2
		add.w	d2,d3 ; cut length
.noclipx2

	sub.w	d1,d3 ; width
	lea		(a3,d1.w*2),a3	; src
	lea		(a4,d0.w*2),a4	; dest

	sub.w	#1,d4
	sub.w	#1,d3 ; width

	; - - - -draw loops
	clr.l	d1	; to extend .w->.l a unsigned way.
.lpy
	move.l	a3,a0	;src
	move.l	a4,a1	;dest
	move.w	d3,d0
.lpx
		move.w (a0)+,d1
		move.w (a2,d1.l*2),(a1)+  ; d1 used as signed, a2 point +32k center of 64k
	dbf	d0,.lpx

	adda.l	d6,a4
	adda.l	d7,a3
	dbf	d4,.lpy

.endfunc
	movem.l	(sp)+,d2-d7/a2-a6
	rts

; could size optimise, almost same:
_directDrawClut32:
directDrawClut32:
	;a0 dsc_
	;a1 dso_
	;a2 lut .w
	;d0 xstart
	;d1 ystart
	; d2 swap flags
 	movem.l	d2-d7/a2-a6,-(sp)

	; - - - - - y
	; on source
	move.w	dso_y1(a1),d3  ; source start
	move.w	dso_y2(a1),d4 ; source end

	move.w	dsc_clipY1(a0),d2

; move.w	d2,asmval+2
; move.w	d4,asmval2+2


	; test complete y exit
	move.w	d1,d5
	add.w	d4,d5
	sub.w	d3,d5 ; d5=y1+height
	cmp.w	d5,d2
	bge		.endfunc

	cmp.w	d1,d2
	ble.b	.noclipy1
		add.w	d2,d3
		sub.w	d1,d3 ; ystartsource += dif

		move.w	d2,d1 ; screen start
.noclipy1

	move.w	dsc_clipY2(a0),d2
	; test complete y exit
	cmp.w	d1,d2
	ble		.endfunc

	move.w	d1,d5
	add.w	d4,d5
	sub.w	d3,d5 ; d5 end draw y2
	sub.w	d5,d2 ; d2 +clip dif
	bge.b	.noclipy2
		add.w	d2,d4 ; cut length
.noclipy2

	;d0 x1
	;d1 y1
	;d3 srcy1
	;d4 srcy2
	sub.w	d3,d4 ; d4 height to draw
	ble	.endfunc

	; - - - -  - -manage pointers
	; manage from source
	move.l	dso_base(a1),a3 ; ptr bm source
	move.l	dsc_base(a0),a4 ; ptr bm dest
	; shift source x1
	move.l	dsc_bpr(a0),d6	; need
	move.l	dso_bpr(a1),d7	; need
	mulu.w	d7,d3
	adda.l	d3,a3
	mulu.w	d6,d1
	adda.l	d1,a4
	; d1 d2 d3 free, d4 height
	; - -- - - - x
	move.w	dso_x1(a1),d1  ; source start
	move.w	dso_x2(a1),d3  ; source end

	move.w	dsc_clipX1(a0),d2

	; test complete x1 exit
	move.w	d0,d5
	add.w	d3,d5
	sub.w	d1,d5 ; d5=x1+width
	cmp.w	d5,d2
	bge		.endfunc

	cmp.w	d0,d2
	ble.b	.noclipx1
		add.w	d2,d1
		sub.w	d0,d1 ; xstartsource += dif

		move.w	d2,d0 ; screen start
.noclipx1

	move.w	dsc_clipX2(a0),d2
	; test complete x exit
	cmp.w	d0,d2
	ble		.endfunc

	move.w	d0,d5
	add.w	d3,d5
	sub.w	d1,d5 ; d5 end draw y2
	sub.w	d5,d2 ; d2 +clip dif
	bge.b	.noclipx2
		add.w	d2,d3 ; cut length
.noclipx2

	sub.w	d1,d3 ; width
	lea		(a3,d1.w*2),a3	; src , still .w
	lea		(a4,d0.w*4),a4	; dest

	sub.w	#1,d4
	sub.w	#1,d3 ; width

	; - - - -draw loops
	clr.l	d1	; to extend .w->.l a unsigned way.
.lpy
	move.l	a3,a0	;src
	move.l	a4,a1	;dest
	move.w	d3,d0
.lpx
		move.w (a0)+,d1
		move.l (a2,d1.l*4),(a1)+  ; d1 used as signed, a2 point +32k center of 64k
	dbf	d0,.lpx

	adda.l	d6,a4
	adda.l	d7,a3
	dbf	d4,.lpy

.endfunc
	movem.l	(sp)+,d2-d7/a2-a6
	rts



;asmval:
;_asmval:
;		dc.l	0

;		XDEF asmval2
;		XDEF _asmval2

;asmval2:
;_asmval2:
;			dc.l	0
