 include exec/types.i

	STRUCTURE directDrawScreen,0
		APTR	dsc_base
		ULONG	dsc_bpr
		WORD	dsc_clipX1
		WORD	dsc_clipY1
		WORD	dsc_clipX2
		WORD	dsc_clipY2
	LABEL	dsc_sizeof

;a0 screen
;a1 end screen
;a2 sprite mem

;d0 palette
;d1 dy -8 or +8
; d2 screenmodx very constant

;typedef void (*neoTileWriter)(UINT16 ipalette REGNG(d0),INT32 dy REGNG(d1),INT32 screenmodx REGNG(d2),
 ;                       UINT16 *bmy REGNG(a0),UINT16 *bmyend REGNG(a1),UINT8 *fspr REGNG(a2) );

	section	code,code

	XDEF	_NeoDrawTile16_Opaque_z15
_NeoDrawTile16_Opaque_z15:

;	movem.l a2/d4/d3/d2,-(sp)
	movem.l d4/d3,-(sp)
.LCFI102:
;	moveq #-2,d3
;	and.l d3,d2
	;cmp.l a0,a1
	;jcs .L1844

.L1848:
	move.l (a2),d3
	move.w d3,d4
	and.w #15,d4
	add.w d0,d4
	move.w d4,(12,a0)

	bfextu d3{#24:#4},d4
	add.w d0,d4
	move.w d4,(14,a0)

	bfextu d3{#20:#4},d4
	add.w d0,d4
	move.w d4,(8,a0)
	bfextu d3{#16:#4},d4
	add.w d0,d4
	move.w d4,(10,a0)
	bfextu d3{#12:#4},d4
	add.w d0,d4
	move.w d4,(4,a0)
	bfextu d3{#8:#4},d4
	add.w d0,d4
	move.w d4,(6,a0)
	bfextu d3{#4:#4},d4
	add.w d0,d4
	move.w d4,(a0)

	;moveq #28,d4
	;lsr.l d4,d3
	;move.w d0,d4
	;add.w d3,d4
	;move.w d4,(2,a0)
	  rol.l	#4,d3
	  and.w	#$000f,d3
	  add.w	d0,d3
	  move.w	d3,(2,a0)
;test move.w	#15,(2,a0)

	move.l (4,a2),d3
	move.w d3,d4
	and.w #15,d4
	add.w d0,d4
	move.w d4,(28,a0)

	bfextu d3{#24:#4},d4
	add.w d0,d4
	move.w d4,(30,a0)
	bfextu d3{#20:#4},d4
	add.w d0,d4
	move.w d4,(24,a0)
	bfextu d3{#16:#4},d4
	add.w d0,d4
	move.w d4,(26,a0)
	bfextu d3{#12:#4},d4
	add.w d0,d4
	move.w d4,(20,a0)
	bfextu d3{#8:#4},d4
	add.w d0,d4
	move.w d4,(22,a0)
	bfextu d3{#4:#4},d4
	add.w d0,d4
	move.w d4,(16,a0)

		moveq #28,d4
		lsr.l d4,d3
		move.w d0,d4
	add.w d3,d4
	move.w d4,(18,a0)

	add.l d1,a2	; sprite next line up or down


	add.l d2,a0 // screen next down.
	cmp.l a1,a0
	ble	.L1848

.L1844:
;	movem.l (sp)+,d2/d3/d4/a2
	movem.l (sp)+,d3/d4
	rts


;a0 screen
;a1 end screen
;a2 sprite mem

;d0 palette
;d1 dy -8 or +8
; d2 screenmodx very constant




	XDEF	_NeoDrawTile16_Transp0_z15
_NeoDrawTile16_Transp0_z15:
.LFB522:
	movem.l a2/d5/d4/d3/d2,-(sp)
.LCFI103:
	move.l d1,d5
;	moveq #-2,d1
;	and.l d1,d2
;	cmp.l a0,a1
;	jcs .L1851
.L1905:
	move.l a0,d3
	add.l d2,d3
	move.l (a2),d1
	beq .L1854
	move.b d1,d4
	and.b #15,d4
	beq .L1874
	and.w #255,d4
	add.w d0,d4
	move.w d4,(12,a0)
.L1874:
	bfextu d1{#24:#4},d4
	beq .L1855
	add.w d0,d4
	move.w d4,(14,a0)
.L1855:
	bfextu d1{#20:#4},d4
	beq .L1856
	add.w d0,d4
	move.w d4,(8,a0)
.L1856:
	bfextu d1{#16:#4},d4
	beq .L1857
	add.w d0,d4
	move.w d4,(10,a0)
.L1857:
	bfextu d1{#12:#4},d4
	beq .L1858
	add.w d0,d4
	move.w d4,(4,a0)
.L1858:
	bfextu d1{#8:#4},d4
	beq .L1859
	add.w d0,d4
	move.w d4,(6,a0)
.L1859:
	bfextu d1{#4:#4},d4
	beq .L1860
	add.w d0,d4
	move.w d4,(a0)
.L1860:
	moveq #28,d4
	lsr.l d4,d1
	beq .L1854
	move.w d0,d4
	add.w d1,d4
	move.w d4,(2,a0)
.L1854:
	move.l (4,a2),d1
	beq .L1863
	move.b d1,d4
	and.b #15,d4
	beq .L1876
	and.w #255,d4
	add.w d0,d4
	move.w d4,(28,a0)
.L1876:
	bfextu d1{#24:#4},d4
	beq .L1864
	add.w d0,d4
	move.w d4,(30,a0)
.L1864:
	bfextu d1{#20:#4},d4
	beq .L1865
	add.w d0,d4
	move.w d4,(24,a0)
.L1865:
	bfextu d1{#16:#4},d4
	beq .L1866
	add.w d0,d4
	move.w d4,(26,a0)
.L1866:
	bfextu d1{#12:#4},d4
	beq .L1867
	add.w d0,d4
	move.w d4,(20,a0)
.L1867:
	bfextu d1{#8:#4},d4
	beq .L1868
	add.w d0,d4
	move.w d4,(22,a0)
.L1868:
	bfextu d1{#4:#4},d4
	beq .L1869
	add.w d0,d4
	move.w d4,(16,a0)
.L1869:
	moveq #28,d4
	lsr.l d4,d1
	beq .L1863
	move.w d0,d4
	add.w d1,d4
	move.w d4,(18,a0)
.L1863:
	add.l d5,a2
	move.l d3,a0
	cmp.l a1,d3
;	jls .L1905
	ble .L1905
.L1851:
	movem.l (sp)+,d2/d3/d4/d5/a2
	rts
	XDEF	_NeoDrawTile16_Opaque_flipx_z15
_NeoDrawTile16_Opaque_flipx_z15:

.LFB519:
	movem.l a2/d4/d3,-(sp)
.LCFI102:
;	moveq #-2,d3
;	and.l d3,d2
;	cmp.l a0,a1
;	jcs .L1842
.L1846:
	move.l (a2),d3
	move.w d3,d4
	and.w #15,d4
	add.w d0,d4
	move.w d4,(18,a0)
	bfextu d3{#24:#4},d4
	add.w d0,d4
	move.w d4,(16,a0)
	bfextu d3{#20:#4},d4
	add.w d0,d4
	move.w d4,(22,a0)
	bfextu d3{#16:#4},d4
	add.w d0,d4
	move.w d4,(20,a0)
	bfextu d3{#12:#4},d4
	add.w d0,d4
	move.w d4,(26,a0)
	bfextu d3{#8:#4},d4
	add.w d0,d4
	move.w d4,(24,a0)
	bfextu d3{#4:#4},d4
	add.w d0,d4
	move.w d4,(30,a0)
	moveq #28,d4
	lsr.l d4,d3
	move.w d0,d4
	add.w d3,d4
	move.w d4,(28,a0)
	move.l (4,a2),d3
	move.w d3,d4
	and.w #15,d4
	add.w d0,d4
	move.w d4,(2,a0)
	bfextu d3{#24:#4},d4
	add.w d0,d4
	move.w d4,(a0)
	bfextu d3{#20:#4},d4
	add.w d0,d4
	move.w d4,(6,a0)
	bfextu d3{#16:#4},d4
	add.w d0,d4
	move.w d4,(4,a0)
	bfextu d3{#12:#4},d4
	add.w d0,d4
	move.w d4,(10,a0)
	bfextu d3{#8:#4},d4
	add.w d0,d4
	move.w d4,(8,a0)
	bfextu d3{#4:#4},d4
	add.w d0,d4
	move.w d4,(14,a0)
	moveq #28,d4
	lsr.l d4,d3
	move.w d0,d4
	add.w d3,d4
	move.w d4,(12,a0)
	add.l d1,a2
	add.l d2,a0
	cmp.l a1,a0
	ble .L1846
.L1842:
	movem.l (sp)+,d3/d4/a2

	rts
	XDEF	_NeoDrawTile16_Transp0_flipx_z15
_NeoDrawTile16_Transp0_flipx_z15:

.LFB520:
	movem.l a2/d5/d4/d3,-(sp)
.LCFI103:
	move.l d1,d5
;	moveq #-2,d1
;	and.l d1,d2
;	cmp.l a0,a1
;	jcs .L1849
.L1903:
	move.l a0,d3
	add.l d2,d3
	move.l (a2),d1
	beq .L1852
	move.b d1,d4
	and.b #15,d4
	beq .L1872
	and.w #255,d4
	add.w d0,d4
	move.w d4,(18,a0)
.L1872:
	bfextu d1{#24:#4},d4
	beq .L1853
	add.w d0,d4
	move.w d4,(16,a0)
.L1853:
	bfextu d1{#20:#4},d4
	beq .L1854
	add.w d0,d4
	move.w d4,(22,a0)
.L1854:
	bfextu d1{#16:#4},d4
	beq .L1855
	add.w d0,d4
	move.w d4,(20,a0)
.L1855:
	bfextu d1{#12:#4},d4
	beq .L1856
	add.w d0,d4
	move.w d4,(26,a0)
.L1856:
	bfextu d1{#8:#4},d4
	beq .L1857
	add.w d0,d4
	move.w d4,(24,a0)
.L1857:
	bfextu d1{#4:#4},d4
	beq .L1858
	add.w d0,d4
	move.w d4,(30,a0)
.L1858:
	moveq #28,d4
	lsr.l d4,d1
	beq .L1852
	move.w d0,d4
	add.w d1,d4
	move.w d4,(28,a0)
.L1852:
	move.l (4,a2),d1
	beq .L1861
	move.b d1,d4
	and.b #15,d4
	beq .L1874
	and.w #255,d4
	add.w d0,d4
	move.w d4,(2,a0)
.L1874:
	bfextu d1{#24:#4},d4
	beq .L1862
	add.w d0,d4
	move.w d4,(a0)
.L1862:
	bfextu d1{#20:#4},d4
	beq .L1863
	add.w d0,d4
	move.w d4,(6,a0)
.L1863:
	bfextu d1{#16:#4},d4
	beq .L1864
	add.w d0,d4
	move.w d4,(4,a0)
.L1864:
	bfextu d1{#12:#4},d4
	beq .L1865
	add.w d0,d4
	move.w d4,(10,a0)
.L1865:
	bfextu d1{#8:#4},d4
	beq .L1866
	add.w d0,d4
	move.w d4,(8,a0)
.L1866:
	bfextu d1{#4:#4},d4
	beq .L1867
	add.w d0,d4
	move.w d4,(14,a0)
.L1867:
	moveq #28,d4
	lsr.l d4,d1
	beq .L1861
	move.w d0,d4
	add.w d1,d4
	move.w d4,(12,a0)
.L1861:
	add.l d5,a2
	move.l d3,a0
	cmp.l a1,d3
	ble .L1903
.L1849:
	movem.l (sp)+,d3/d4/d5/a2
	rts
