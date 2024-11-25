	include exec/types.i

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
   ; rmoved a2
   movem.l d5/d4/d3/d2,-(sp)

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
   beq.b .L1854
   move.b d1,d4
   and.w #15,d4
   beq.b .L1874
   add.w d0,d4
   move.w d4,(12,a0)
.L1874:
   bfextu d1{#24:#4},d4
   beq.b .L1855
   add.w d0,d4
   move.w d4,(14,a0)
.L1855:
   bfextu d1{#20:#4},d4
   beq.b .L1856
   add.w d0,d4
   move.w d4,(8,a0)
.L1856:
   bfextu d1{#16:#4},d4
   beq.b .L1857
   add.w d0,d4
   move.w d4,(10,a0)
.L1857:
   bfextu d1{#12:#4},d4
   beq.b .L1858
   add.w d0,d4
   move.w d4,(4,a0)
.L1858:
   bfextu d1{#8:#4},d4
   beq.b .L1859
   add.w d0,d4
   move.w d4,(6,a0)
.L1859:
   bfextu d1{#4:#4},d4
   beq.b .L1860
   add.w d0,d4
   move.w d4,(a0)
.L1860:
   moveq #28,d4
   lsr.l d4,d1
   beq.b .L1854
   move.w d0,d4
   add.w d1,d4
   move.w d4,(2,a0)
.L1854:
   move.l (4,a2),d1
   beq.b .L1863
   move.b d1,d4
   and.w #15,d4
   beq.b .L1876
   add.w d0,d4
   move.w d4,(28,a0)
.L1876:
   bfextu d1{#24:#4},d4
   beq.b .L1864
   add.w d0,d4
   move.w d4,(30,a0)
.L1864:
   bfextu d1{#20:#4},d4
   beq.b .L1865
   add.w d0,d4
   move.w d4,(24,a0)
.L1865:
   bfextu d1{#16:#4},d4
   beq.b .L1866
   add.w d0,d4
   move.w d4,(26,a0)
.L1866:
   bfextu d1{#12:#4},d4
   beq.b .L1867
   add.w d0,d4
   move.w d4,(20,a0)
.L1867:
   bfextu d1{#8:#4},d4
   beq.b .L1868
   add.w d0,d4
   move.w d4,(22,a0)
.L1868:
   bfextu d1{#4:#4},d4
   beq.b .L1869
   add.w d0,d4
   move.w d4,(16,a0)
.L1869:
   moveq #28,d4
   lsr.l d4,d1
   beq.b .L1863
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
   movem.l (sp)+,d2/d3/d4/d5
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
   ; removed a2 because return code doesn't use the pointer again.
   movem.l d5/d4/d3,-(sp)
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
   beq.b .L1852
   move.b d1,d4
   and.w #15,d4
   beq.b .L1872
   add.w d0,d4
   move.w d4,(18,a0)
.L1872:
   bfextu d1{#24:#4},d4
   beq.b .L1853
   add.w d0,d4
   move.w d4,(16,a0)
.L1853:
   bfextu d1{#20:#4},d4
   beq.b .L1854
   add.w d0,d4
   move.w d4,(22,a0)
.L1854:
   bfextu d1{#16:#4},d4
   beq.b .L1855
   add.w d0,d4
   move.w d4,(20,a0)
.L1855:
   bfextu d1{#12:#4},d4
   beq.b .L1856
   add.w d0,d4
   move.w d4,(26,a0)
.L1856:
   bfextu d1{#8:#4},d4
   beq.b .L1857
   add.w d0,d4
   move.w d4,(24,a0)
.L1857:
   bfextu d1{#4:#4},d4
   beq.b .L1858
   add.w d0,d4
   move.w d4,(30,a0)
.L1858:
   moveq #28,d4
   lsr.l d4,d1
   beq.b .L1852
   move.w d0,d4
   add.w d1,d4
   move.w d4,(28,a0)
.L1852:
   move.l (4,a2),d1
   beq.b .L1861
   move.b d1,d4
   and.w #15,d4
   beq.b .L1874
   add.w d0,d4
   move.w d4,(2,a0)
.L1874:
   bfextu d1{#24:#4},d4
   beq.b .L1862
   add.w d0,d4
   move.w d4,(a0)
.L1862:
   bfextu d1{#20:#4},d4
   beq.b .L1863
   add.w d0,d4
   move.w d4,(6,a0)
.L1863:
   bfextu d1{#16:#4},d4
   beq.b .L1864
   add.w d0,d4
   move.w d4,(4,a0)
.L1864:
   bfextu d1{#12:#4},d4
   beq.b .L1865
   add.w d0,d4
   move.w d4,(10,a0)
.L1865:
   bfextu d1{#8:#4},d4
   beq.b .L1866
   add.w d0,d4
   move.w d4,(8,a0)
.L1866:
   bfextu d1{#4:#4},d4
   beq.b .L1867
   add.w d0,d4
   move.w d4,(14,a0)
.L1867:
   moveq #28,d4
   lsr.l d4,d1
   beq.b .L1861
   move.w d0,d4
   add.w d1,d4
   move.w d4,(12,a0)
.L1861:
   add.l d5,a2
   move.l d3,a0
   cmp.l a1,d3
   ble .L1903
.L1849:
   movem.l (sp)+,d3/d4/d5
   rts


; ----------------- lines

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE_4_FUNES_S1_S3_:
.LFB74:
   move.b (4,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   rts
.LFE74:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE0_4_FUNES_S1_S3_:
.LFB77:
   move.b (2,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (4,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   rts
.LFE77:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE1_4_FUNES_S1_S3_:
.LFB80:
   move.b (2,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (4,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (6,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   rts
.LFE80:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE2_4_FUNES_S1_S3_:
.LFB83:
   move.b (1,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (2,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (4,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (6,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   rts
.LFE83:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE3_4_FUNES_S1_S3_:
.LFB86:
   move.b (1,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (2,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (4,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (6,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (7,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   rts
.LFE86:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE4_4_FUNES_S1_S3_:
.LFB89:
   move.b (1,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (2,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (3,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (4,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (6,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (7,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   rts
.LFE89:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE5_4_FUNES_S1_S3_:
.LFB92:
   move.b (1,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (2,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (3,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (4,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (5,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (6,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (7,a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   rts
.LFE92:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE6_4_FUNES_S1_S3_:
.LFB95:
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   rts
.LFE95:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE7_4_FUNES_S1_S3_:
.LFB98:
   move.l d2,-(sp)
.LCFI0:
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   move.l (sp)+,d2
   rts
.LFE98:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE8_4_FUNES_S1_S3_:
.LFB101:
   move.l d2,-(sp)
.LCFI1:
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1),d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   move.l (sp)+,d2
   rts
.LFE101:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE9_4_FUNES_S1_S3_:
.LFB104:
   move.l d2,-(sp)
.LCFI2:
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1),d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE104:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE10_4_FUNES_S1_S3_:
.LFB107:
   move.l d2,-(sp)
.LCFI3:
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1),d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE107:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE11_4_FUNES_S1_S3_:
.LFB110:
   move.l d2,-(sp)
.LCFI4:
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1),d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE110:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE12_4_FUNES_S1_S3_:
.LFB113:
   move.l d2,-(sp)
.LCFI5:
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1),d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE113:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE13_4_FUNES_S1_S3_:
.LFB116:
   move.l d2,-(sp)
.LCFI6:
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1),d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE116:

; mostly used
__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE14_4_FUNES_S1_S3_:
.LFB119:
   move.l d2,-(sp)
.LCFI7:
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1)+,d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1),d1
   move.w d1,d2
   and.w #15,d2
   add.w d0,d2
   move.w d2,(a0)+
   bfextu d1{#24:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE119:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE15_4_FUNES_S1_S3_:
.LFB122:
   move.b (4,a1),d1
   and.w #15,d1
   beq.b .L17
   add.w d0,d1
   move.w d1,(a0)
.L17:
   rts
.LFE122:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE16_4_FUNES_S1_S3_:
.LFB125:
   move.b (2,a1),d1
   and.w #15,d1
   beq.b .L25

   add.w d0,d1
   move.w d1,(a0)
.L25:
   move.b (4,a1),d1
   and.w #15,d1
   beq.b .L24
   add.w d0,d1
   move.w d1,(2,a0)
.L24:
   rts
.LFE125:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE17_4_FUNES_S1_S3_:
.LFB128:
   move.b (2,a1),d1
   and.w #15,d1
   beq.b .L36
   add.w d0,d1
   move.w d1,(a0)
.L36:
   move.b (4,a1),d1
   and.w #15,d1
   beq.b .L37

   add.w d0,d1
   move.w d1,(2,a0)
.L37:
   move.b (6,a1),d1
   and.w #15,d1
   beq.b .L35

   add.w d0,d1
   move.w d1,(4,a0)
.L35:
   rts
.LFE128:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE18_4_FUNES_S1_S3_:
.LFB131:
   move.b (1,a1),d1
   and.w #15,d1
   beq.b .L51

   add.w d0,d1
   move.w d1,(a0)
.L51:
   move.b (2,a1),d1
   and.w #15,d1
   beq.b .L52

   add.w d0,d1
   move.w d1,(2,a0)
.L52:
   move.b (4,a1),d1
   and.w #15,d1
   beq.b .L53

   add.w d0,d1
   move.w d1,(4,a0)
.L53:
   move.b (6,a1),d1
   and.w #15,d1
   beq.b .L50

   add.w d0,d1
   move.w d1,(6,a0)
.L50:
   rts
.LFE131:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE19_4_FUNES_S1_S3_:
.LFB134:
   move.b (1,a1),d1
   and.w #15,d1
   beq.b .L70
   add.w d0,d1
   move.w d1,(a0)
.L70:
   move.b (2,a1),d1
   and.w #15,d1
   beq.b .L71
   add.w d0,d1
   move.w d1,(2,a0)
.L71:
   move.b (4,a1),d1
   and.w #15,d1
   beq.b .L72
   add.w d0,d1
   move.w d1,(4,a0)
.L72:
   move.b (6,a1),d1
   and.w #15,d1
   beq.b .L73
   add.w d0,d1
   move.w d1,(6,a0)
.L73:
   move.b (7,a1),d1
   and.w #15,d1
   beq.b .L69
   add.w d0,d1
   move.w d1,(8,a0)
.L69:
   rts
.LFE134:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE20_4_FUNES_S1_S3_:
.LFB137:
   move.b (1,a1),d1
   and.w #15,d1
   beq.b .L93

   add.w d0,d1
   move.w d1,(a0)
.L93:
   move.b (2,a1),d1
   and.w #15,d1
   beq.b .L94

   add.w d0,d1
   move.w d1,(2,a0)
.L94:
   move.b (3,a1),d1
   and.w #15,d1
   beq.b .L95

   add.w d0,d1
   move.w d1,(4,a0)
.L95:
   move.b (4,a1),d1
   and.w #15,d1
   beq.b .L96

   add.w d0,d1
   move.w d1,(6,a0)
.L96:
   move.b (6,a1),d1
   and.w #15,d1
   beq.b .L97

   add.w d0,d1
   move.w d1,(8,a0)
.L97:
   move.b (7,a1),d1
   and.w #15,d1
   beq.b .L92

   add.w d0,d1
   move.w d1,(10,a0)
.L92:
   rts
.LFE137:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE21_4_FUNES_S1_S3_:
.LFB140:
   move.b (1,a1),d1
   and.w #15,d1
   beq.b .L120
   add.w d0,d1
   move.w d1,(a0)
.L120:
   move.b (2,a1),d1
   and.w #15,d1
   beq.b .L121
   add.w d0,d1
   move.w d1,(2,a0)
.L121:
   move.b (3,a1),d1
   and.w #15,d1
   beq.b .L122
   add.w d0,d1
   move.w d1,(4,a0)
.L122:
   move.b (4,a1),d1
   and.w #15,d1
   beq.b .L123
   add.w d0,d1
   move.w d1,(6,a0)
.L123:
   move.b (5,a1),d1
   and.w #15,d1
   beq.b .L124
   add.w d0,d1
   move.w d1,(8,a0)
.L124:
   move.b (6,a1),d1
   and.w #15,d1
   beq.b .L125
   add.w d0,d1
   move.w d1,(10,a0)
.L125:
   move.b (7,a1),d1
   and.w #15,d1
   beq.b .L119
   add.w d0,d1
   move.w d1,(12,a0)
.L119:
   rts
.LFE140:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE22_4_FUNES_S1_S3_:
.LFB143:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L151
   add.w d0,d1
   move.w d1,(a0)
.L151:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L152
   add.w d0,d1
   move.w d1,(2,a0)
.L152:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L153
   add.w d0,d1
   move.w d1,(4,a0)
.L153:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L154
   add.w d0,d1
   move.w d1,(6,a0)
.L154:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L155
   add.w d0,d1
   move.w d1,(8,a0)
.L155:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L156
   add.w d0,d1
   move.w d1,(10,a0)
.L156:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L157
   add.w d0,d1
   move.w d1,(12,a0)
.L157:
   move.b (a1),d1
   and.w #15,d1
   beq.b .L150
   add.w d0,d1
   move.w d1,(14,a0)
.L150:
   rts
.LFE143:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE23_4_FUNES_S1_S3_:
.LFB146:
   move.l d2,-(sp)
.LCFI8:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L186
   add.w d0,d1
   move.w d1,(a0)
.L186:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L187
   add.w d0,d1
   move.w d1,(2,a0)
.L187:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L188

   add.w d0,d1
   move.w d1,(4,a0)
.L188:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L189

   add.w d0,d1
   move.w d1,(6,a0)
.L189:
   clr.l d2
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L190

   add.w d0,d1
   move.w d1,(8,a0)
.L190:
   lsr.l #4,d2
   beq.b .L191
   move.w d0,d1
   add.w d2,d1
   move.w d1,(10,a0)
.L191:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L192

   add.w d0,d1
   move.w d1,(12,a0)
.L192:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L193

   add.w d0,d1
   move.w d1,(14,a0)
.L193:
   move.b (a1),d1
   and.w #15,d1
   beq.b .L185

   add.w d0,d1
   move.w d1,(16,a0)
.L185:
   move.l (sp)+,d2
   rts
.LFE146:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE24_4_FUNES_S1_S3_:
.LFB149:
   move.l d2,-(sp)
.LCFI9:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L225
   add.w d0,d1
   move.w d1,(a0)
.L225:
   clr.l d2
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L226
   add.w d0,d1
   move.w d1,(2,a0)
.L226:
   lsr.l #4,d2
   beq.b .L227
   move.w d0,d1
   add.w d2,d1
   move.w d1,(4,a0)
.L227:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L228
   add.w d0,d1
   move.w d1,(6,a0)
.L228:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L229
   add.w d0,d1
   move.w d1,(8,a0)
.L229:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L230
   add.w d0,d1
   move.w d1,(10,a0)
.L230:
   lsr.l #4,d2
   beq.b .L231
   move.w d0,d1
   add.w d2,d1
   move.w d1,(12,a0)
.L231:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L232
   add.w d0,d1
   move.w d1,(14,a0)
.L232:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L233
   add.w d0,d1
   move.w d1,(16,a0)
.L233:
   move.b (a1),d1
   and.w #15,d1
   beq.b .L224
   add.w d0,d1
   move.w d1,(18,a0)
.L224:
   move.l (sp)+,d2
   rts
.LFE149:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE25_4_FUNES_S1_S3_:
.LFB152:
   move.l d2,-(sp)
.LCFI10:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L268
   add.w d0,d1
   move.w d1,(a0)
.L268:
   clr.l d2
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L269
   add.w d0,d1
   move.w d1,(2,a0)
.L269:
   lsr.l #4,d2
   beq.b .L270
   move.w d0,d1
   add.w d2,d1
   move.w d1,(4,a0)
.L270:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L271
   add.w d0,d1
   move.w d1,(6,a0)
.L271:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L272
   add.w d0,d1
   move.w d1,(8,a0)
.L272:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L273
   add.w d0,d1
   move.w d1,(10,a0)
.L273:
   lsr.l #4,d2
   beq.b .L274
   move.w d0,d1
   add.w d2,d1
   move.w d1,(12,a0)
.L274:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L275
   add.w d0,d1
   move.w d1,(14,a0)
.L275:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L276
   add.w d0,d1
   move.w d1,(16,a0)
.L276:
   move.b (a1),d1
   move.b d1,d2
   and.w #15,d1
   beq.b .L277
   add.w d0,d1
   move.w d1,(18,a0)
.L277:
   lsr.l #4,d2
   beq.b .L267
   add.w d2,d0
   move.w d0,(20,a0)
.L267:
   move.l (sp)+,d2
   rts
.LFE152:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE26_4_FUNES_S1_S3_:
.LFB155:
   move.l d2,-(sp)
.LCFI11:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L315
   add.w d0,d1
   move.w d1,(a0)
.L315:
   clr.l d2
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L316
   add.w d0,d1
   move.w d1,(2,a0)
.L316:
   lsr.l #4,d2
   beq.b .L317
   move.w d0,d1
   add.w d2,d1
   move.w d1,(4,a0)
.L317:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L318
   add.w d0,d1
   move.w d1,(6,a0)
.L318:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L319
   add.w d0,d1
   move.w d1,(8,a0)
.L319:
   lsr.l #4,d2
   beq.b .L320
   move.w d0,d1
   add.w d2,d1
   move.w d1,(10,a0)
.L320:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L321
   add.w d0,d1
   move.w d1,(12,a0)
.L321:
   lsr.l #4,d2
   beq.b .L322
   move.w d0,d1
   add.w d2,d1
   move.w d1,(14,a0)
.L322:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L323
   add.w d0,d1
   move.w d1,(16,a0)
.L323:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L324
   add.w d0,d1
   move.w d1,(18,a0)
.L324:
   move.b (a1),d1
   move.b d1,d2
   and.w #15,d1
   beq.b .L325
   add.w d0,d1
   move.w d1,(20,a0)
.L325:
   lsr.l #4,d2
   beq.b .L314
   add.w d2,d0
   move.w d0,(22,a0)
.L314:
   move.l (sp)+,d2
   rts
.LFE155:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE27_4_FUNES_S1_S3_:
.LFB158:
   move.l d2,-(sp)
.LCFI12:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L366
   add.w d0,d1
   move.w d1,(a0)
.L366:
   clr.l d2
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L367
   add.w d0,d1
   move.w d1,(2,a0)
.L367:
   lsr.l #4,d2
   beq.b .L368
   move.w d0,d1
   add.w d2,d1
   move.w d1,(4,a0)
.L368:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L369
   add.w d0,d1
   move.w d1,(6,a0)
.L369:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L370
   add.w d0,d1
   move.w d1,(8,a0)
.L370:
   lsr.l #4,d2
   beq.b .L371
   move.w d0,d1
   add.w d2,d1
   move.w d1,(10,a0)
.L371:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L372
   add.w d0,d1
   move.w d1,(12,a0)
.L372:
   lsr.l #4,d2
   beq.b .L373
   move.w d0,d1
   add.w d2,d1
   move.w d1,(14,a0)
.L373:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L374
   add.w d0,d1
   move.w d1,(16,a0)
.L374:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L375
   add.w d0,d1
   move.w d1,(18,a0)
.L375:
   lsr.l #4,d2
   beq.b .L376
   move.w d0,d1
   add.w d2,d1
   move.w d1,(20,a0)
.L376:
   move.b (a1),d1
   move.b d1,d2
   and.w #15,d1
   beq.b .L377
   add.w d0,d1
   move.w d1,(22,a0)
.L377:
   lsr.l #4,d2
   beq.b .L365
   add.w d2,d0
   move.w d0,(24,a0)
.L365:
   move.l (sp)+,d2
   rts
.LFE158:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE28_4_FUNES_S1_S3_:
.LFB161:
   move.l d2,-(sp)
.LCFI13:
   clr.l d2
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L421
   add.w d0,d1
   move.w d1,(a0)
.L421:
   lsr.l #4,d2
   beq.b .L422
   move.w d0,d1
   add.w d2,d1
   move.w d1,(2,a0)
.L422:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L423
   add.w d0,d1
   move.w d1,(4,a0)
.L423:
   lsr.l #4,d2
   beq.b .L424
   move.w d0,d1
   add.w d2,d1
   move.w d1,(6,a0)
.L424:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L425
   add.w d0,d1
   move.w d1,(8,a0)
.L425:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L426
   add.w d0,d1
   move.w d1,(10,a0)
.L426:
   lsr.l #4,d2
   beq.b .L427
   move.w d0,d1
   add.w d2,d1
   move.w d1,(12,a0)
.L427:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L428
   add.w d0,d1
   move.w d1,(14,a0)
.L428:
   lsr.l #4,d2
   beq.b .L429
   move.w d0,d1
   add.w d2,d1
   move.w d1,(16,a0)
.L429:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L430
   add.w d0,d1
   move.w d1,(18,a0)
.L430:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L431
   add.w d0,d1
   move.w d1,(20,a0)
.L431:
   lsr.l #4,d2
   beq.b .L432
   move.w d0,d1
   add.w d2,d1
   move.w d1,(22,a0)
.L432:
   move.b (a1),d1
   move.b d1,d2
   and.w #15,d1
   beq.b .L433
   add.w d0,d1
   move.w d1,(24,a0)
.L433:
   lsr.l #4,d2
   beq.b .L420
   add.w d2,d0
   move.w d0,(26,a0)
.L420:
   move.l (sp)+,d2
   rts
.LFE161:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE29_4_FUNES_S1_S3_:
.LFB164:
   move.l d2,-(sp)
.LCFI14:
   clr.l d2
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L480
   add.w d0,d1
   move.w d1,(a0)
.L480:
   lsr.l #4,d2
   beq.b .L481
   move.w d0,d1
   add.w d2,d1
   move.w d1,(2,a0)
.L481:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L482
   add.w d0,d1
   move.w d1,(4,a0)
.L482:
   lsr.l #4,d2
   beq.b .L483
   move.w d0,d1
   add.w d2,d1
   move.w d1,(6,a0)
.L483:
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L484
   add.w d0,d1
   move.w d1,(8,a0)
.L484:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L485
   add.w d0,d1
   move.w d1,(10,a0)
.L485:
   lsr.l #4,d2
   beq.b .L486
   move.w d0,d1
   add.w d2,d1
   move.w d1,(12,a0)
.L486:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L487
   add.w d0,d1
   move.w d1,(14,a0)
.L487:
   lsr.l #4,d2
   beq.b .L488
   move.w d0,d1
   add.w d2,d1
   move.w d1,(16,a0)
.L488:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L489
   add.w d0,d1
   move.w d1,(18,a0)
.L489:
   lsr.l #4,d2
   beq.b .L490
   move.w d0,d1
   add.w d2,d1
   move.w d1,(20,a0)
.L490:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L491
   add.w d0,d1
   move.w d1,(22,a0)
.L491:
   lsr.l #4,d2
   beq.b .L492
   move.w d0,d1
   add.w d2,d1
   move.w d1,(24,a0)
.L492:
   move.b (a1),d1
   move.b d1,d2
   and.w #15,d1
   beq.b .L493
   add.w d0,d1
   move.w d1,(26,a0)
.L493:
   lsr.l #4,d2
   beq.b .L479
   add.w d2,d0
   move.w d0,(28,a0)
.L479:
   move.l (sp)+,d2
   rts
.LFE164:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE30_4_FUNES_S1_S3_:
.LFB167:
   move.l d2,-(sp)
.LCFI15:
   clr.l d2
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L543
   add.w d0,d1
   move.w d1,(a0)
.L543:
   lsr.l #4,d2
   beq.b .L544
   move.w d0,d1
   add.w d2,d1
   move.w d1,(2,a0)
.L544:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L545
   add.w d0,d1
   move.w d1,(4,a0)
.L545:
   lsr.l #4,d2
   beq.b .L546
   move.w d0,d1
   add.w d2,d1
   move.w d1,(6,a0)
.L546:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L547
   add.w d0,d1
   move.w d1,(8,a0)
.L547:
   lsr.l #4,d2
   beq.b .L548
   move.w d0,d1
   add.w d2,d1
   move.w d1,(10,a0)
.L548:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L549
   add.w d0,d1
   move.w d1,(12,a0)
.L549:
   lsr.l #4,d2
   beq.b .L550
   move.w d0,d1
   add.w d2,d1
   move.w d1,(14,a0)
.L550:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L551
   add.w d0,d1
   move.w d1,(16,a0)
.L551:
   lsr.l #4,d2
   beq.b .L552
   move.w d0,d1
   add.w d2,d1
   move.w d1,(18,a0)
.L552:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L553
   add.w d0,d1
   move.w d1,(20,a0)
.L553:
   lsr.l #4,d2
   beq.b .L554
   move.w d0,d1
   add.w d2,d1
   move.w d1,(22,a0)
.L554:
   move.b (a1),d2
   move.b (a1)+,d1
   and.w #15,d1
   beq.b .L555
   add.w d0,d1
   move.w d1,(24,a0)
.L555:
   lsr.l #4,d2
   beq.b .L556
   move.w d0,d1
   add.w d2,d1
   move.w d1,(26,a0)
.L556:
   move.b (a1),d1
   move.b d1,d2
   and.w #15,d1
   beq.b .L557
   add.w d0,d1
   move.w d1,(28,a0)
.L557:
   lsr.l #4,d2
   beq.b .L542
   add.w d2,d0
   move.w d0,(30,a0)
.L542:
   move.l (sp)+,d2
   rts
.LFE167:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE31_4_FUNES_S1_S3_:
.LFB170:
   bfextu (3,a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   rts
.LFE170:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE32_4_FUNES_S1_S3_:
.LFB173:
   move.l d2,-(sp)
.LCFI16:
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (3,a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE173:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE33_4_FUNES_S1_S3_:
.LFB176:
   move.l d2,-(sp)
.LCFI17:
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (3,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE176:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE34_4_FUNES_S1_S3_:
.LFB179:
   move.l d2,-(sp)
.LCFI18:
   bfextu (6,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (3,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE179:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE35_4_FUNES_S1_S3_:
.LFB182:
   move.l d2,-(sp)
.LCFI19:
   bfextu (6,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (3,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE182:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE36_4_FUNES_S1_S3_:
.LFB185:
   move.l d2,-(sp)
.LCFI20:
   bfextu (6,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (4,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (3,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE185:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE37_4_FUNES_S1_S3_:
.LFB188:
   move.l d2,-(sp)
.LCFI21:
   bfextu (6,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (4,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (3,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (2,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE188:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE38_4_FUNES_S1_S3_:
.LFB191:
   move.l d2,-(sp)
.LCFI22:
   bfextu (7,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (6,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (4,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (3,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (2,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   rts
.LFE191:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE39_4_FUNES_S1_S3_:
.LFB194:
   move.l d3,-(sp)
.LCFI23:
   move.l d2,-(sp)
.LCFI24:
   bfextu (7,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (6,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (4,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (2,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE194:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE40_4_FUNES_S1_S3_:
.LFB197:
   move.l d3,-(sp)
.LCFI25:
   move.l d2,-(sp)
.LCFI26:
   bfextu (7,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (4,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (2,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (a1){#0:#4},d1
   add.w d1,d0
   move.w d0,(a0)
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE197:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE41_4_FUNES_S1_S3_:
.LFB200:
   move.l d3,-(sp)
.LCFI27:
   move.l d2,-(sp)
.LCFI28:
   bfextu (7,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (4,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (2,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1),d1
   move.w d0,a1
   bfextu d1{#24:#4},d2
   add.w d2,a1
   move.w a1,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE200:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE42_4_FUNES_S1_S3_:
.LFB203:
   move.l d3,-(sp)
.LCFI29:
   move.l d2,-(sp)
.LCFI30:
   bfextu (7,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (2,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   bfextu (1,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (a1),d1
   move.w d0,a1
   bfextu d1{#24:#4},d2
   add.w d2,a1
   move.w a1,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE203:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE43_4_FUNES_S1_S3_:
.LFB206:
   move.l d3,-(sp)
.LCFI31:
   move.l d2,-(sp)
.LCFI32:
   bfextu (7,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (2,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (1,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1),d1
   move.w d0,a1
   bfextu d1{#24:#4},d2
   add.w d2,a1
   move.w a1,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE206:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE44_4_FUNES_S1_S3_:
.LFB209:
   move.l d3,-(sp)
.LCFI33:
   move.l d2,-(sp)
.LCFI34:
   move.b (7,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (2,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (1,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1),d1
   move.w d0,a1
   bfextu d1{#24:#4},d2
   add.w d2,a1
   move.w a1,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE209:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE45_4_FUNES_S1_S3_:
.LFB212:
   move.l d3,-(sp)
.LCFI35:
   move.l d2,-(sp)
.LCFI36:
   move.b (7,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   bfextu (5,a1){#0:#4},d1
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)+
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (2,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (1,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1),d1
   move.w d0,a1
   bfextu d1{#24:#4},d2
   add.w d2,a1
   move.w a1,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE212:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE46_4_FUNES_S1_S3_:
.LFB215:
   move.l d3,-(sp)
.LCFI37:
   move.l d2,-(sp)
.LCFI38:
   move.b (7,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (5,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (2,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (1,a1),d1
   bfextu d1{#24:#4},d2
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)+
   move.b (a1),d1
   move.w d0,a1
   bfextu d1{#24:#4},d2
   add.w d2,a1
   move.w a1,(a0)+
   and.w #15,d1
   add.w d0,d1
   move.w d1,(a0)
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE215:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE47_4_FUNES_S1_S3_:
.LFB218:
   bfextu (3,a1){#0:#4},d1
   beq.b .L625
   add.w d1,d0
   move.w d0,(a0)
.L625:
   rts
.LFE218:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE48_4_FUNES_S1_S3_:
.LFB221:
   move.l d2,-(sp)
.LCFI39:
   bfextu (5,a1){#0:#4},d1
   beq.b .L633
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L633:
   bfextu (3,a1){#0:#4},d1
   beq.b .L632
   add.w d1,d0
   move.w d0,(2,a0)
.L632:
   move.l (sp)+,d2
   rts
.LFE221:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE49_4_FUNES_S1_S3_:
.LFB224:
   move.l d2,-(sp)
.LCFI40:
   bfextu (5,a1){#0:#4},d1
   beq.b .L644
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L644:
   bfextu (3,a1){#0:#4},d1
   beq.b .L645
   move.w d0,d2
   add.w d1,d2
   move.w d2,(2,a0)
.L645:
   bfextu (1,a1){#0:#4},d1
   beq.b .L643
   add.w d1,d0
   move.w d0,(4,a0)
.L643:
   move.l (sp)+,d2
   rts
.LFE224:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE50_4_FUNES_S1_S3_:
.LFB227:
   move.l d2,-(sp)
.LCFI41:
   bfextu (6,a1){#0:#4},d1
   beq.b .L659
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L659:
   bfextu (5,a1){#0:#4},d1
   beq.b .L660
   move.w d0,d2
   add.w d1,d2
   move.w d2,(2,a0)
.L660:
   bfextu (3,a1){#0:#4},d1
   beq.b .L661
   move.w d0,d2
   add.w d1,d2
   move.w d2,(4,a0)
.L661:
   bfextu (1,a1){#0:#4},d1
   beq.b .L658
   add.w d1,d0
   move.w d0,(6,a0)
.L658:
   move.l (sp)+,d2
   rts
.LFE227:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE51_4_FUNES_S1_S3_:
.LFB230:
   move.l d2,-(sp)
.LCFI42:
   bfextu (6,a1){#0:#4},d1
   beq.b .L678
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L678:
   bfextu (5,a1){#0:#4},d1
   beq.b .L679
   move.w d0,d2
   add.w d1,d2
   move.w d2,(2,a0)
.L679:
   bfextu (3,a1){#0:#4},d1
   beq.b .L680
   move.w d0,d2
   add.w d1,d2
   move.w d2,(4,a0)
.L680:
   bfextu (1,a1){#0:#4},d1
   beq.b .L681
   move.w d0,d2
   add.w d1,d2
   move.w d2,(6,a0)
.L681:
   bfextu (a1){#0:#4},d1
   beq.b .L677
   add.w d1,d0
   move.w d0,(8,a0)
.L677:
   move.l (sp)+,d2
   rts
.LFE230:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE52_4_FUNES_S1_S3_:
.LFB233:
   move.l d2,-(sp)
.LCFI43:
   bfextu (6,a1){#0:#4},d1
   beq.b .L701
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L701:
   bfextu (5,a1){#0:#4},d1
   beq.b .L702
   move.w d0,d2
   add.w d1,d2
   move.w d2,(2,a0)
.L702:
   bfextu (4,a1){#0:#4},d1
   beq.b .L703
   move.w d0,d2
   add.w d1,d2
   move.w d2,(4,a0)
.L703:
   bfextu (3,a1){#0:#4},d1
   beq.b .L704
   move.w d0,d2
   add.w d1,d2
   move.w d2,(6,a0)
.L704:
   bfextu (1,a1){#0:#4},d1
   beq.b .L705
   move.w d0,d2
   add.w d1,d2
   move.w d2,(8,a0)
.L705:
   bfextu (a1){#0:#4},d1
   beq.b .L700
   add.w d1,d0
   move.w d0,(10,a0)
.L700:
   move.l (sp)+,d2
   rts
.LFE233:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE53_4_FUNES_S1_S3_:
.LFB236:
   move.l d2,-(sp)
.LCFI44:
   bfextu (6,a1){#0:#4},d1
   beq.b .L728
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L728:
   bfextu (5,a1){#0:#4},d1
   beq.b .L729
   move.w d0,d2
   add.w d1,d2
   move.w d2,(2,a0)
.L729:
   bfextu (4,a1){#0:#4},d1
   beq.b .L730
   move.w d0,d2
   add.w d1,d2
   move.w d2,(4,a0)
.L730:
   bfextu (3,a1){#0:#4},d1
   beq.b .L731
   move.w d0,d2
   add.w d1,d2
   move.w d2,(6,a0)
.L731:
   bfextu (2,a1){#0:#4},d1
   beq.b .L732
   move.w d0,d2
   add.w d1,d2
   move.w d2,(8,a0)
.L732:
   bfextu (1,a1){#0:#4},d1
   beq.b .L733
   move.w d0,d2
   add.w d1,d2
   move.w d2,(10,a0)
.L733:
   bfextu (a1){#0:#4},d1
   beq.b .L727
   add.w d1,d0
   move.w d0,(12,a0)
.L727:
   move.l (sp)+,d2
   rts
.LFE236:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE54_4_FUNES_S1_S3_:
.LFB239:
   move.l d2,-(sp)
.LCFI45:
   bfextu (7,a1){#0:#4},d1
   beq.b .L759
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L759:
   bfextu (6,a1){#0:#4},d1
   beq.b .L760
   move.w d0,d2
   add.w d1,d2
   move.w d2,(2,a0)
.L760:
   bfextu (5,a1){#0:#4},d1
   beq.b .L761
   move.w d0,d2
   add.w d1,d2
   move.w d2,(4,a0)
.L761:
   bfextu (4,a1){#0:#4},d1
   beq.b .L762
   move.w d0,d2
   add.w d1,d2
   move.w d2,(6,a0)
.L762:
   bfextu (3,a1){#0:#4},d1
   beq.b .L763
   move.w d0,d2
   add.w d1,d2
   move.w d2,(8,a0)
.L763:
   bfextu (2,a1){#0:#4},d1
   beq.b .L764
   move.w d0,d2
   add.w d1,d2
   move.w d2,(10,a0)
.L764:
   bfextu (1,a1){#0:#4},d1
   beq.b .L765
   move.w d0,d2
   add.w d1,d2
   move.w d2,(12,a0)
.L765:
   bfextu (a1){#0:#4},d1
   beq.b .L758
   add.w d1,d0
   move.w d0,(14,a0)
.L758:
   move.l (sp)+,d2
   rts
.LFE239:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE55_4_FUNES_S1_S3_:
.LFB242:
   move.l d3,-(sp)
.LCFI46:
   move.l d2,-(sp)
.LCFI47:
   bfextu (7,a1){#0:#4},d1
   beq.b .L794
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L794:
   bfextu (6,a1){#0:#4},d1
   beq.b .L795
   move.w d0,d2
   add.w d1,d2
   move.w d2,(2,a0)
.L795:
   bfextu (5,a1){#0:#4},d1
   beq.b .L796
   move.w d0,d2
   add.w d1,d2
   move.w d2,(4,a0)
.L796:
   bfextu (4,a1){#0:#4},d1
   beq.b .L797
   move.w d0,d2
   add.w d1,d2
   move.w d2,(6,a0)
.L797:
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L798
   move.w d0,d3
   add.w d2,d3
   move.w d3,(8,a0)
.L798:
   and.w #15,d1
   beq.b .L799
   add.w d0,d1
   move.w d1,(10,a0)
.L799:
   bfextu (2,a1){#0:#4},d1
   beq.b .L800
   move.w d0,d2
   add.w d1,d2
   move.w d2,(12,a0)
.L800:
   bfextu (1,a1){#0:#4},d1
   beq.b .L801
   move.w d0,d2
   add.w d1,d2
   move.w d2,(14,a0)
.L801:
   bfextu (a1){#0:#4},d1
   beq.b .L793
   add.w d1,d0
   move.w d0,(16,a0)
.L793:
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE242:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE56_4_FUNES_S1_S3_:
.LFB245:
   move.l d3,-(sp)
.LCFI48:
   move.l d2,-(sp)
.LCFI49:
   bfextu (7,a1){#0:#4},d1
   beq.b .L833
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L833:
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L834
   move.w d0,d3
   add.w d2,d3
   move.w d3,(2,a0)
.L834:
   and.w #15,d1
   beq.b .L835
   add.w d0,d1
   move.w d1,(4,a0)
.L835:
   bfextu (5,a1){#0:#4},d1
   beq.b .L836
   move.w d0,d2
   add.w d1,d2
   move.w d2,(6,a0)
.L836:
   bfextu (4,a1){#0:#4},d1
   beq.b .L837
   move.w d0,d2
   add.w d1,d2
   move.w d2,(8,a0)
.L837:
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L838
   move.w d0,d3
   add.w d2,d3
   move.w d3,(10,a0)
.L838:
   and.w #15,d1
   beq.b .L839
   add.w d0,d1
   move.w d1,(12,a0)
.L839:
   bfextu (2,a1){#0:#4},d1
   beq.b .L840
   move.w d0,d2
   add.w d1,d2
   move.w d2,(14,a0)
.L840:
   bfextu (1,a1){#0:#4},d1
   beq.b .L841
   move.w d0,d2
   add.w d1,d2
   move.w d2,(16,a0)
.L841:
   bfextu (a1){#0:#4},d1
   beq.b .L832
   add.w d1,d0
   move.w d0,(18,a0)
.L832:
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE245:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE57_4_FUNES_S1_S3_:
.LFB248:
   move.l d3,-(sp)
.LCFI50:
   move.l d2,-(sp)
.LCFI51:
   bfextu (7,a1){#0:#4},d1
   beq.b .L876
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L876:
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L877
   move.w d0,d3
   add.w d2,d3
   move.w d3,(2,a0)
.L877:
   and.w #15,d1
   beq.b .L878
   add.w d0,d1
   move.w d1,(4,a0)
.L878:
   bfextu (5,a1){#0:#4},d1
   beq.b .L879
   move.w d0,d2
   add.w d1,d2
   move.w d2,(6,a0)
.L879:
   bfextu (4,a1){#0:#4},d1
   beq.b .L880
   move.w d0,d2
   add.w d1,d2
   move.w d2,(8,a0)
.L880:
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L881
   move.w d0,d3
   add.w d2,d3
   move.w d3,(10,a0)
.L881:
   and.w #15,d1
   beq.b .L882
   add.w d0,d1
   move.w d1,(12,a0)
.L882:
   bfextu (2,a1){#0:#4},d1
   beq.b .L883
   move.w d0,d2
   add.w d1,d2
   move.w d2,(14,a0)
.L883:
   bfextu (1,a1){#0:#4},d1
   beq.b .L884
   move.w d0,d2
   add.w d1,d2
   move.w d2,(16,a0)
.L884:
   move.b (a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L885
   move.w d0,a1
   add.w d2,a1
   move.w a1,(18,a0)
.L885:
   and.w #15,d1
   beq.b .L875
   add.w d0,d1
   move.w d1,(20,a0)
.L875:
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE248:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE58_4_FUNES_S1_S3_:
.LFB251:
   move.l d3,-(sp)
.LCFI52:
   move.l d2,-(sp)
.LCFI53:
   bfextu (7,a1){#0:#4},d1
   beq.b .L923
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L923:
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L924
   move.w d0,d3
   add.w d2,d3
   move.w d3,(2,a0)
.L924:
   and.w #15,d1
   beq.b .L925
   add.w d0,d1
   move.w d1,(4,a0)
.L925:
   bfextu (5,a1){#0:#4},d1
   beq.b .L926
   move.w d0,d2
   add.w d1,d2
   move.w d2,(6,a0)
.L926:
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L927
   move.w d0,d3
   add.w d2,d3
   move.w d3,(8,a0)
.L927:
   and.w #15,d1
   beq.b .L928
   add.w d0,d1
   move.w d1,(10,a0)
.L928:
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L929
   move.w d0,d3
   add.w d2,d3
   move.w d3,(12,a0)
.L929:
   and.w #15,d1
   beq.b .L930
   add.w d0,d1
   move.w d1,(14,a0)
.L930:
   bfextu (2,a1){#0:#4},d1
   beq.b .L931
   move.w d0,d2
   add.w d1,d2
   move.w d2,(16,a0)
.L931:
   bfextu (1,a1){#0:#4},d1
   beq.b .L932
   move.w d0,d2
   add.w d1,d2
   move.w d2,(18,a0)
.L932:
   move.b (a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L933
   move.w d0,a1
   add.w d2,a1
   move.w a1,(20,a0)
.L933:
   and.w #15,d1
   beq.b .L922
   add.w d0,d1
   move.w d1,(22,a0)
.L922:
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE251:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE59_4_FUNES_S1_S3_:
.LFB254:
   move.l d3,-(sp)
.LCFI54:
   move.l d2,-(sp)
.LCFI55:
   bfextu (7,a1){#0:#4},d1
   beq.b .L974
   move.w d0,d2
   add.w d1,d2
   move.w d2,(a0)
.L974:
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L975
   move.w d0,d3
   add.w d2,d3
   move.w d3,(2,a0)
.L975:
   and.w #15,d1
   beq.b .L976
   add.w d0,d1
   move.w d1,(4,a0)
.L976:
   bfextu (5,a1){#0:#4},d1
   beq.b .L977
   move.w d0,d2
   add.w d1,d2
   move.w d2,(6,a0)
.L977:
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L978
   move.w d0,d3
   add.w d2,d3
   move.w d3,(8,a0)
.L978:
   and.w #15,d1
   beq.b .L979
   add.w d0,d1
   move.w d1,(10,a0)
.L979:
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L980
   move.w d0,d3
   add.w d2,d3
   move.w d3,(12,a0)
.L980:
   and.w #15,d1
   beq.b .L981
   add.w d0,d1
   move.w d1,(14,a0)
.L981:
   bfextu (2,a1){#0:#4},d1
   beq.b .L982
   move.w d0,d2
   add.w d1,d2
   move.w d2,(16,a0)
.L982:
   move.b (1,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L983
   move.w d0,d3
   add.w d2,d3
   move.w d3,(18,a0)
.L983:
   and.w #15,d1
   beq.b .L984
   add.w d0,d1
   move.w d1,(20,a0)
.L984:
   move.b (a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L985
   move.w d0,a1
   add.w d2,a1
   move.w a1,(22,a0)
.L985:
   and.w #15,d1
   beq.b .L973
   add.w d0,d1
   move.w d1,(24,a0)
.L973:
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE254:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE60_4_FUNES_S1_S3_:
.LFB257:
   move.l d3,-(sp)
.LCFI56:
   move.l d2,-(sp)
.LCFI57:
   move.b (7,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1029
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)
.L1029:
   and.w #15,d1
   beq.b .L1030
   add.w d0,d1
   move.w d1,(2,a0)
.L1030:
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1031
   move.w d0,d3
   add.w d2,d3
   move.w d3,(4,a0)
.L1031:
   and.w #15,d1
   beq.b .L1032
   add.w d0,d1
   move.w d1,(6,a0)
.L1032:
   bfextu (5,a1){#0:#4},d1
   beq.b .L1033
   move.w d0,d2
   add.w d1,d2
   move.w d2,(8,a0)
.L1033:
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1034
   move.w d0,d3
   add.w d2,d3
   move.w d3,(10,a0)
.L1034:
   and.w #15,d1
   beq.b .L1035
   add.w d0,d1
   move.w d1,(12,a0)
.L1035:
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1036
   move.w d0,d3
   add.w d2,d3
   move.w d3,(14,a0)
.L1036:
   and.w #15,d1
   beq.b .L1037
   add.w d0,d1
   move.w d1,(16,a0)
.L1037:
   bfextu (2,a1){#0:#4},d1
   beq.b .L1038
   move.w d0,d2
   add.w d1,d2
   move.w d2,(18,a0)
.L1038:
   move.b (1,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1039
   move.w d0,d3
   add.w d2,d3
   move.w d3,(20,a0)
.L1039:
   and.w #15,d1
   beq.b .L1040
   add.w d0,d1
   move.w d1,(22,a0)
.L1040:
   move.b (a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1041
   move.w d0,a1
   add.w d2,a1
   move.w a1,(24,a0)
.L1041:
   and.w #15,d1
   beq.b .L1028
   add.w d0,d1
   move.w d1,(26,a0)
.L1028:
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE257:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE61_4_FUNES_S1_S3_:
.LFB260:
   move.l d3,-(sp)
.LCFI58:
   move.l d2,-(sp)
.LCFI59:
   move.b (7,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1088
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)
.L1088:
   and.w #15,d1
   beq.b .L1089
   add.w d0,d1
   move.w d1,(2,a0)
.L1089:
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1090
   move.w d0,d3
   add.w d2,d3
   move.w d3,(4,a0)
.L1090:
   and.w #15,d1
   beq.b .L1091
   add.w d0,d1
   move.w d1,(6,a0)
.L1091:
   bfextu (5,a1){#0:#4},d1
   beq.b .L1092
   move.w d0,d2
   add.w d1,d2
   move.w d2,(8,a0)
.L1092:
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1093
   move.w d0,d3
   add.w d2,d3
   move.w d3,(10,a0)
.L1093:
   and.w #15,d1
   beq.b .L1094
   add.w d0,d1
   move.w d1,(12,a0)
.L1094:
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1095
   move.w d0,d3
   add.w d2,d3
   move.w d3,(14,a0)
.L1095:
   and.w #15,d1
   beq.b .L1096

   add.w d0,d1
   move.w d1,(16,a0)
.L1096:
   move.b (2,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1097
   move.w d0,d3
   add.w d2,d3
   move.w d3,(18,a0)
.L1097:
   and.w #15,d1
   beq.w .L1098
   add.w d0,d1
   move.w d1,(20,a0)
.L1098:
   move.b (1,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1099
   move.w d0,d3
   add.w d2,d3
   move.w d3,(22,a0)
.L1099:
   and.w #15,d1
   beq.b .L1100
   add.w d0,d1
   move.w d1,(24,a0)
.L1100:
   move.b (a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1101
   move.w d0,a1
   add.w d2,a1
   move.w a1,(26,a0)
.L1101:
   and.w #15,d1
   beq.b .L1087

   add.w d0,d1
   move.w d1,(28,a0)
.L1087:
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts
.LFE260:

__ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE62_4_FUNES_S1_S3_:
.LFB263:
   move.l d3,-(sp)
.LCFI60:
   move.l d2,-(sp)
.LCFI61:
   move.b (7,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1151
   move.w d0,d3
   add.w d2,d3
   move.w d3,(a0)
.L1151:
   and.w #15,d1
   beq.b .L1152
   add.w d0,d1
   move.w d1,(2,a0)
.L1152:
   move.b (6,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1153
   move.w d0,d3
   add.w d2,d3
   move.w d3,(4,a0)
.L1153:
   and.w #15,d1
   beq.b .L1154

   add.w d0,d1
   move.w d1,(6,a0)
.L1154:
   move.b (5,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1155
   move.w d0,d3
   add.w d2,d3
   move.w d3,(8,a0)
.L1155:
   and.w #15,d1
   beq.b .L1156

   add.w d0,d1
   move.w d1,(10,a0)
.L1156:
   move.b (4,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1157
   move.w d0,d3
   add.w d2,d3
   move.w d3,(12,a0)
.L1157:
   and.w #15,d1
   beq.b .L1158

   add.w d0,d1
   move.w d1,(14,a0)
.L1158:
   move.b (3,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1159
   move.w d0,d3
   add.w d2,d3
   move.w d3,(16,a0)
.L1159:
   and.w #15,d1
   beq.b .L1160

   add.w d0,d1
   move.w d1,(18,a0)
.L1160:
   move.b (2,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1161
   move.w d0,d3
   add.w d2,d3
   move.w d3,(20,a0)
.L1161:
   and.w #15,d1
   beq.b .L1162

   add.w d0,d1
   move.w d1,(22,a0)
.L1162:
   move.b (1,a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1163
   move.w d0,d3
   add.w d2,d3
   move.w d3,(24,a0)
.L1163:
   and.w #15,d1
   beq.b .L1164

   add.w d0,d1
   move.w d1,(26,a0)
.L1164:
   move.b (a1),d1
   bfextu d1{#24:#4},d2
   beq.b .L1165
   move.w d0,a1
   add.w d2,a1
   move.w a1,(28,a0)
.L1165:
   and.w #15,d1
   beq.b .L1150

   add.w d0,d1
   move.w d1,(30,a0)
.L1150:
   move.l (sp)+,d2
   move.l (sp)+,d3
   rts


   xdef _neogeo_lineWriters
_neogeo_lineWriters:

; - - - -- - - - -- - - - - no flipx, Opaque  , zx 0->15
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE0_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE1_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE2_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE3_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE4_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE5_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE6_4_FUNES_S1_S3_

   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE7_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE8_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE9_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE10_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE11_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE12_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE13_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE14_4_FUNES_S1_S3_

; - - - -- - - - -- - - - - no flipx, Transparent0

   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE15_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE16_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE17_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE18_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE19_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE20_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE21_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE22_4_FUNES_S1_S3_

   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE23_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE24_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE25_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE26_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE27_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE28_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE29_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE30_4_FUNES_S1_S3_

; - - - -- - - - -- - - - - flipx, Opaque  , zx 0->15

   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE31_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE32_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE33_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE34_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE35_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE36_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE37_4_FUNES_S1_S3_

   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE38_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE39_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE40_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE41_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE42_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE43_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE44_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE45_4_FUNES_S1_S3_

; - - - -- - - - -- - - - - flipx, Transparent0

   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE46_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE47_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE48_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE49_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE50_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE51_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE52_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE53_4_FUNES_S1_S3_

   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE54_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE55_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE56_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE57_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE58_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE59_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE60_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE61_4_FUNES_S1_S3_
   dc.l __ZNUlU6asmregILi8EEPtU6asmregILi9EEPhU6asmregILi0EEtE62_4_FUNES_S1_S3_

