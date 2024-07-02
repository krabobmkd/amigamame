; amiga: phxass I=ii: M=68020 cgxhooks_re.s

; cross compile:
; vasmm68k_mot -devpac -m68020 -Fhunk -I/opt/amiga/m68k-amigaos/ndk-include cgxhooks_re.s -o cgxhooks_re.s.o

;_LVOAllocMem     EQU   -198   -c6
;_LVOAllocAbs     EQU   -204  -cc
;_LVOFreeMem     EQU   -210    -d2
;_LVOOldOpenLibrary     EQU   -408    -$198
;_LVOCloseLibrary     EQU   -414   -$19e
;_LVOOpenLibrary   -552    -$228

; graphics
; -$03B4   -948
;_LVOReleasePen     EQU   -948
;_LVOObtainPen     EQU   -954

	include exec/types.i
	include utility/hooks.i

	include lvo/exec_lib.i
	include lvo/graphics_lib.i


_LVOGetCyberMapAttr equ -$0060

;STRUCTURE HOOK,MLN_SIZE
; APTR h_Entry		; assembler entry point
; APTR h_SubEntry		; optional HLL entry point
; APTR h_Data		; owner specific
;LABEL h_SIZEOF

; corrected " version
;struct CGXHook
;{
;	struct Hook			Hook;  // sizeof=20=$14
;	ULONG				Type;   // $14
;	struct Library		*SysBase; // $18
;	struct Library		*GfxBase; // $1c
;	struct Library		*LayersBase; // $20
;	struct Library		*CyberGfxBase; // $24
;	ULONG				BytesPerPixel; // $28  filled by _AllocCGXHook
;	ULONG				PixFmt; // $2c  filled by _AllocCGXHook
;	struct Screen		*Screen; // $30
;    // looks OK til here

;    // to be verified:
;    UBYTE				*Source; //$34
;	ULONG				Data; // $38
;	ULONG				SrcMod; // $3c
;	ULONG				SrcX; // $40
;	ULONG				SrcY; // $44
;	ULONG				DstX; // $48
;	ULONG				DstY; // $4c
;    // changed by krb against asm code:
;    // $50
;    // obvious missing 24 bytes
;    UBYTE               COMPLETEME[20];
;    APTR                TABLEPOINTER; // $64, point $68 at start

;    // $68
;	ULONG				Remap[256]; // $50   struct size: $450

;    // libs does alloc #$00000468
;    // so there is $18 bytes more.

;};



 STRUCTURE CGXHook,h_SIZEOF ; extends Hooks
	ULONG	cgh_Type			; $14
	APTR	cgh_SysBase			; $18
	APTR	cgh_GfxBase			; $1c
	APTR	cgh_LayersBase		; $20
	APTR	cgh_CyberGfxBase	; $24
	ULONG	cgh_BytesPerPixel	; $28
	ULONG	cgh_PixFmt			; $2c
	APTR	cgh_Screen  ; struct Screen   $30
	APTR	cgh_Source ; UBYTE *
	; following is to be validated
	ULONG	cgh_Data
	ULONG	cgh_SrcMod
	ULONG	cgh_SrcX
	ULONG	cgh_SrcY
	ULONG	cgh_DstX
	ULONG	cgh_DstY
	; absolutely wrong on the C header side...
	STRUCT	cgh_xxxxx,20
	APTR	cgh_guessme ; ptr filled with cgh_Remap.
	STRUCT	cgh_Remap,4*256

 ; should be #$00000468 -> verified
 LABEL	cgh_SIZEOF

	SECTION "",CODE

	XDEF	_AllocCLUT8RemapHook
	XDEF	_FreeCGXHook
	XDEF	_CustomRemapCLUT8RemapHook
	XDEF	_DoCLUT8RemapHook


; test to check real values of defines...
;    XDEF v_cgh_SIZEOF
;v_cgh_SIZEOF:
;    dc.l cgh_SIZEOF

	; used by _AllocCLUT8RemapHook
_AllocCGXHook:
	; a0: probably screen->Bitmap $54(screen)
	MOVEM.L	A3/A5/A6,-(A7)
	MOVEA.L	A0,A5
	MOVEA.W	#$0004,A0
	MOVEA.L	(A0),A1  ; a1 execbase
	MOVE.L	#$00000468,D0   ; size cgxhooks struct + clut table at end.
	MOVEQ	#$01,D1
	SWAP	D1		; memf_clear
	MOVEA.L	A1,A6
	JSR	_LVOAllocMem(A6)	; alloc, prob. Alloc() not AllocVec()
	MOVEA.L	D0,A3  ; struct in A3
	MOVE.L	A3,D0
	BEQ.W	L00000E

	MOVEA.W	#$0004,A0
	MOVEA.L	(A0),A1
	MOVE.L	A1,cgh_SysBase(A3) ; sysbase in struct

	LEA	graphicsname(PC),A1	; "graphics.library"
	MOVEA.L	cgh_SysBase(A3),A6
	MOVEQ	#$27,D0  ; ver 39
	JSR	_LVOOpenLibrary(A6)
	MOVE.L	D0,cgh_GfxBase(A3)  ; GfxBase in struct
	BEQ.W	L00000D

	LEA	layersname(PC),A1
	MOVEA.L	cgh_SysBase(A3),A6
	MOVEQ	#$27,D0
	JSR	_LVOOpenLibrary(A6)
	MOVE.L	D0,cgh_LayersBase(A3)
	BEQ.B	L00000C

	LEA	cgxname(PC),A1
	MOVEA.L	cgh_SysBase(A3),A6
	MOVEQ	#$28,D0
	JSR	_LVOOpenLibrary(A6)
	MOVE.L	D0,cgh_CyberGfxBase(A3)
	BEQ.B	L00000B   ; cybergfx fail

	MOVEA.L	$0004(A5),A0    ; a5 screen rastport or bitmap
	MOVE.L	#$80000008,D0
	MOVEA.L	cgh_CyberGfxBase(A3),A6   ; cybergfx base
	JSR	_LVOGetCyberMapAttr(A6)   ; value = GetCyberMapAttr( a0 BitMap, d0 Attribute );
	; #define CYBRMATTR_ISCYBERGFX  (0x80000008) /* returns -1 if supplied bitmap is a cybergfx one */
	TST.L	D0
	BEQ.B	L00000A ; error if screen not cgx

	MOVEA.L	$0004(A5),A0
	MOVE.L	#$80000004,D0  ; CYBRMATTR_PIXFMT return the pixel format
	JSR	_LVOGetCyberMapAttr(A6)
	MOVE.L	D0,cgh_PixFmt(A3)

	MOVEA.L	$0004(A5),A0
	MOVEA.L	cgh_CyberGfxBase(A3),A6
	MOVE.L	#$80000002,D0 ; CYBRMATTR_BPPIX    bytes per pixel
	JSR	_LVOGetCyberMapAttr(A6)
	MOVE.L	D0,cgh_BytesPerPixel(A3)
	MOVE.L	A3,D0
	BRA.B	L00000F  ; ok
L00000A:
	MOVEA.L	cgh_CyberGfxBase(A3),A1
	MOVEA.L	cgh_SysBase(A3),A6
	JSR	_LVOCloseLibrary(A6)
L00000B:   ; cybergraphics fail
	MOVEA.L	cgh_LayersBase(A3),A1
	MOVEA.L	cgh_SysBase(A3),A6
	JSR	_LVOCloseLibrary(A6)
L00000C:
	MOVEA.L	cgh_GfxBase(A3),A1
	MOVEA.L	cgh_SysBase(A3),A6
	JSR	_LVOCloseLibrary(A6)
L00000D:
	MOVEA.L	A3,A1
	MOVEA.L	cgh_SysBase(A1),A6
	MOVE.L	#$00000468,D0
	JSR	_LVOFreeMem(A6)
L00000E:
	MOVEQ	#$00,D0
L00000F:
	MOVEM.L	(A7)+,A3/A5/A6
	RTS

graphicsname:
	dc.b	"graphics.library",0
layersname:
	dc.b	"layers.library",0
cgxname:
	dc.b	"cybergraphics.library",0
	even

_FreeCGXHook:
	MOVEM.L	D7/A3/A5/A6,-(A7)
	MOVEA.L	A0,A5
	MOVE.L	cgh_Type(A5),D0
	TST.L	D0
	BNE.B	L000014
	TST.L	$002C(A5)
	BNE.B	L000014
	LEA	$0068(A5),A3
	MOVEQ	#$00,D7
L000013:
	CMP.L	$0060(A5),D7
	BGE.B	L000014
	MOVEA.L	cgh_Screen(A5),A0
	MOVEQ	#$00,D0
	MOVE.B	(A3)+,D0
	MOVEA.L	$0030(A0),A0
	MOVEA.L	cgh_GfxBase(A5),A6
	JSR	_LVOReleasePen(A6)
	ADDQ.L	#1,D7
	BRA.B	L000013
L000014:
	MOVEA.L	cgh_CyberGfxBase(A5),A1
	MOVEA.L	cgh_SysBase(A5),A6
	JSR	_LVOCloseLibrary(A6)
	MOVEA.L	cgh_LayersBase(A5),A1
	JSR	_LVOCloseLibrary(A6)
	MOVEA.L	cgh_GfxBase(A5),A1
	JSR	_LVOCloseLibrary(A6)
	MOVEA.L	A5,A1
	MOVE.L	#$00000468,D0
	JSR	_LVOFreeMem(A6)
	MOVEM.L	(A7)+,D7/A3/A5/A6
	RTS


_WriteCLUT8Hook:
	SUBA.W	#$000C,A7
	MOVEM.L	D2-D7/A2/A3/A5/A6,-(A7)
	MOVEA.L	A1,A3
	MOVEA.L	A0,A5
	MOVE.L	$000C(A3),D0
	SUB.L	$0058(A5),D0
	MOVE.L	D0,D7
	MOVE.L	$0010(A3),D0
	SUB.L	$005C(A5),D0
	MOVE.L	D0,D6
	CLR.L	-(A7)
	PEA	$002C(A7)
	MOVE.L	#$84001006,-(A7)
	PEA	$0038(A7)
	MOVE.L	#$84001007,-(A7)
	MOVEA.L	$0004(A2),A0
	MOVEA.L	$0024(A5),A6
	MOVEA.L	A7,A1
	JSR	-$00A8(A6)
	LEA	$0014(A7),A7
	MOVE.L	D0,$0030(A7)
	BEQ.W	L000015
	MOVEA.L	$000C(A5),A0
	MOVEA.L	$0038(A5),A1
	ADDA.L	D7,A1
	ADDA.L	$0048(A5),A1
	MOVE.L	D6,D0
	ADD.L	$004C(A5),D0
	MOVE.L	$0040(A5),D1
	MULU.L	D1,D0
	ADDA.L	D0,A1
	MOVE.W	$0004(A3),D0
	EXT.L	D0
	MULU.L	$0028(A5),D0
	MOVE.L	$002C(A7),D2
	ADD.L	D0,D2
	MOVE.W	$0006(A3),D0
	EXT.L	D0
	MOVE.L	$0028(A7),D3
	MULU.L	D3,D0
	ADD.L	D0,D2
	MOVE.W	$0008(A3),D0
	EXT.L	D0
	MOVE.W	$0004(A3),D4
	EXT.L	D4
	SUB.L	D4,D0
	ADDQ.L	#1,D0
	MOVE.W	$000A(A3),D4
	EXT.L	D4
	MOVE.W	$0006(A3),D5
	EXT.L	D5
	SUB.L	D5,D4
	ADDQ.L	#1,D4
	MOVEA.L	A0,A6
	MOVE.L	A2,-(A7)
	MOVE.L	D4,D1
	MOVEA.L	A1,A0
	MOVEA.L	D2,A1
	MOVE.L	$0040(A5),D2
	MOVEA.L	$0064(A5),A2
	JSR	(A6)
	MOVEA.L	(A7)+,A2
	MOVEA.L	$0030(A7),A0
	MOVEA.L	$0024(A5),A6
	JSR	-$00AE(A6)
L000015:
	MOVEM.L	(A7)+,D2-D7/A2/A3/A5/A6
	ADDA.W	#$000C,A7
	RTS

_AllocCLUT8RemapHook:
	; a0 screen
	; a1 palette (ULONG *) -> or NULL ?
	MOVEM.L	A2/A3/A5/A6,-(A7)
	MOVEA.L	A1,A3
	MOVEA.L	A0,A5
	LEA	$0054(A5),A0  ; a tout les coups rasport du screen  -> sc_RastPort ou sc_Bitmap -> plutot bitmap
	;BSR.W	L000010   ; foireux, erreur d68k, mais doit etre _AllocCGXHook
	bsr.w _AllocCGXHook

	MOVEA.L	D0,A2   ; d0: retour struct CGXHooks alloked with lib bases and bitmap infos.
	MOVE.L	A2,D0
	BEQ.W	L00001E  ; null if cgx not supported or screen not cgx.
	CLR.L	cgh_Type(A2)
	MOVE.L	A5,cgh_Screen(A2)
	LEA	_WriteCLUT8Hook(PC),A0
	MOVE.L	A0,h_Entry(A2)  ; struct Hook h_Entry function pointer
	; palette start at $50 ? at $68 ?
	LEA	$0068(A2),A0 ; oO palette start at $50 -> no.
	MOVE.L	A0,$0064(A2)

	MOVE.L	cgh_BytesPerPixel(A2),D0 ; BytesPerPixel (1,2,3,4)
	SUBQ.L	#1,D0
	BEQ.B	L000017
	SUBQ.L	#1,D0
L000016:
	BEQ.B	L000018
	SUBQ.L	#1,D0
	BEQ.B	L000019
	SUBQ.L	#1,D0
	BEQ.B	L00001A
	BRA.B	L00001B
L000017:	; one byte per pixel case
	;FOIREUX LEA	L00005B(PC),A0
	lea	mapclut8b(pc),a0
	MOVE.L	A0,h_SubEntry(A2)  ; hook h_SubEntry

	BRA.B	L00001B
L000018:	; 2 bytes per pixel case
	;LEA	L000054(PC),A0

	lea	mapclut16b(pc),a0
	MOVE.L	A0,h_SubEntry(A2) ; hook h_SubEntry

	BRA.B	L00001B
L000019: ; 3 bytes pixel ??? cgx modes doesn't get that.

	LEA	mapclut24b(PC),A0
	MOVE.L	A0,h_SubEntry(A2) ; hook h_SubEntry

	BRA.B	L00001B
L00001A: ; 4 bytes per pixel

	LEA	mapclut32b(PC),A0
	MOVE.L	A0,h_SubEntry(A2) ; hook h_SubEntry

L00001B: ; more then 4 bytes per pixels (???) -> no
	TST.L	h_SubEntry(A2)
	BEQ.B	L00001D
	MOVE.L	A3,D0		; Palette pointer second arg.
	BEQ.B	L00001C

	LEA	$0068(A2),A0
	MOVE.L	A2,-(A7)
		MOVEA.L	A0,A1
		MOVE.L	cgh_PixFmt(A2),D0
		MOVEA.L	A3,A0 ; palette ptr
		MOVEA.L	cgh_GfxBase(A2),A6
		MOVEA.L	$0030(A5),A2   ; a5 screen -> a2 rastport ?
		BSR.W		IDONTKNOWWHERE
	MOVEA.L	(A7)+,A2
	MOVE.L	D0,$0060(A2)
	BLT.B	L00001D
L00001C: ; when palette pointer NULL.
	MOVE.L	A2,D0
	BRA.B	L00001F
L00001D:
	MOVEA.L	A2,A0
	BSR.W	L000016
L00001E:
	MOVEQ	#$00,D0
L00001F:
	MOVEM.L	(A7)+,A2/A3/A5/A6
	RTS


IDONTKNOWWHERE:
	rts

mapclut8b:
mapclut16b:
mapclut24b:
mapclut32b:
	rts


_CustomRemapCLUT8RemapHook:
	rts

_DoCLUT8RemapHook:
	rts


