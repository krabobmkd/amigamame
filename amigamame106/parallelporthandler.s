    include "exec/types.i"
    include "hardware/custom.i"
    include "hardware/intbits.i"

    include "lvo/exec_lib.i"

        XDEF    _RBFHandler
        XDEF    RBFHandler

JSRLIB MACRO
       JSR  _LVO\1(A6)
       ENDM

BUFLEN    EQU    256

       STRUCTURE RBFDATA,0
        APTR   rd_task
        ULONG  rd_signal
        UWORD  rd_buffercount
        STRUCT rd_charbuffer,BUFLEN+2
        STRUCT rd_flagbuffer,BUFLEN+2
        STRUCT rd_name,32
        LABEL RBFDATA_SIZEOF

* Entered with:
*  D0 == scratch
*  D1 == INTENAT & INTREQR (scratch)
*  A0 == custom chips (scratch)
*  A1 == is_Data which is RBFDATA structure (scratch)
*  A5 == vector to our code (scratch)
*  A6 == pointer to ExecBase (scratch)
*
* Note - This simple handler just receives one buffer full of serial
* input data, signals main, then ignores all subsequent serial data.
*

;parallel port hardware addresses (from amiga.lib)

	xref	_ciaaprb			; the actual port address
	xref	_ciaaddrb		; data direction register

	xref	_ciabpra			; control lines are here
	xref	_ciabddra		; data direction register


	section	code,code
RBFHandler:
_RBFHandler:                            ;entry to our interrupt handler


	;set up parallel port for reading
	move.b	#0,_ciaaddrb		; all lines read
	andi.b	#$FF,_ciabddra		; busy, pout, and sel. to read

;  --  -serial example:
;	MOVE.W  serdatr(A0),D1          ;get the input word (flags and char)

;	MOVE.W  rd_buffercount(A1),D0   ;get our buffer index
;	CMPI.W  #BUFLEN,D0              ;no more room in our buffer ?
;	BEQ.S   ExitHandler             ;yes - just exit (ignore new char)
;	LEA.L   rd_charbuffer(A1),A5    ;else get our character buffer address
;	MOVE.B  D1,0(A5,D0.W)           ;store character in our character buffer
;	LEA.L   rd_flagbuffer(A1),A5    ;get our flag buffer address
;	LSR.W   #8,d1                   ;shift flags down
;	MOVE.B  D1,0(A5,D0.W)           ;store flags in flagbuffer

;	ADDQ.W  #1,D0                   ;increment our buffer index
;	MOVE.W  D0,rd_buffercount(A1)   ;   and replace it
;	CMPI.W  #BUFLEN,D0              ;did our buffer just become full ?
;	BNE.S   ExitHandler             ;no - we can exit
;	MOVE.L  A0,-(SP)                ;yes - save custom
;	MOVE.L  rd_signal(A1),D0        ;get signal allocated in main()
;	MOVE.L  rd_task(A1),A1          ;and pointer to main task
;	JSRLIB  Signal                  ;tell main we are full
;	MOVE.L  (SP)+,A0                ;restore custom
									;Note: system call trashed D0-D1/A0-A1
ExitHandler:
	MOVE.W  #INTF_RBF,intreq(A0)    ;clear the interrupt
	RTS                             ;return to exec
