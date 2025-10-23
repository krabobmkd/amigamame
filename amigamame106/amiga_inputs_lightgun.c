#include "amiga_inputs_lightgun.h"
#include "amiga_inputs_interface.h"


#include <proto/exec.h>

#include <hardware/custom.h>
#include <hardware/intbits.h>

struct LPInteruptData {
    ULONG vposr,vposr2;
    UWORD joy0dat,joy1dat;
};


struct sLightGunsPrivate
{
    struct sLightGuns _s;
    struct Interrupt *_interupt;
    struct LPInteruptData *_pintdata;
    int _interuptok;

    UWORD _miny;
    UWORD _maxy;
};

static int lightpeninterupt( register struct LPInteruptData *plpid __asm("a1") )
{
    // "read it twice"
    plpid->vposr2 = *((ULONG *)(0x00DFF004));
    plpid->vposr = *((ULONG *)(0x00DFF004));

//JOY0DAT/JOY1DAT
    plpid->joy1dat = *((UWORD *)(0x00DFF00C));

    // custom.vposr;
    return 0; // should set z flag
}

struct sLightGuns *LightGun_create()
{

    struct sLightGunsPrivate *plg = (struct sLightGuns *)AllocVec(sizeof(struct sLightGunsPrivate),MEMF_CLEAR);
    if(!plg) return NULL;


    struct LPInteruptData *pintdata;
    plg->_pintdata = pintdata = AllocVec(sizeof(struct LPInteruptData), MEMF_PUBLIC|MEMF_CLEAR);
    if(!pintdata)
    {
        LightGun_close(&plg->_s);
        //if(preturncode) *preturncode = PROPJOYRET_ALLOC;
        return NULL;
    }

    struct Interrupt *itr;
    plg->_interupt = itr = AllocVec(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR);

    if(!itr)
    {
        LightGun_close(&plg->_s);
       // if(preturncode) *preturncode = PROPJOYRET_ALLOC;
        return NULL;
    }
    plg->_miny = 0xffff;
    //    pintdata->_Task = pprops->_Task;
    //    pintdata->_Signal = 1L << signr;
    itr->is_Node.ln_Name = (char *)"lgun";
    itr->is_Data = (APTR)pintdata;
    itr->is_Code = &lightpeninterupt;
    AddIntServer(INTB_VERTB,itr);
    plg->_interuptok = 1;


    return &plg->_s;
}

void LightGun_close(struct sLightGuns *plgp)
{
    if(!plgp) return;

    struct sLightGunsPrivate *plg = (struct sLightGunsPrivate *)plgp;

    if(plg->_interuptok) {
        RemIntServer(INTB_VERTB,plg->_interupt);
    }

    if(plg->_pintdata) FreeVec(plg->_pintdata);
    if(plg->_interupt) FreeVec(plg->_interupt);

    FreeVec(plg);
}

void LightGun_update(struct sLightGuns *plgp)
{
    if(!plgp) return;
    struct sLightGunsPrivate *plg = (struct sLightGunsPrivate *)plgp;
    if(!plg->_pintdata) return;
    ULONG vposr = plg->_pintdata->vposr;
    plgp->_x = vposr & 0x0ff;
    plgp->_y = (vposr>>8) & 0x01ff; // would vary with resolution (200 ntsc 256 pal)
    plgp->_longFrame =  vposr>>30;
    plgp->_joy0dat = plg->_pintdata->joy0dat;
    plgp->_joy1dat = plg->_pintdata->joy1dat;
    //-> calibrate y.
    if(plgp->_y > plg->_maxy) plg->_maxy = plgp->_y;
    if(plgp->_y < plg->_miny) plg->_miny = plgp->_y;
    int ydelta = plg->_maxy - plg->_miny;
    if(ydelta>0)
    {
        plgp->_calibrated_y = (UWORD)
        ((((int)plgp->_y-(int)plg->_miny)*255)/ydelta);
    }



//    UWORD   vposr;
//    UWORD   vhposr;
// VPOSR    ($DFF004)  Read  light pen position  (high order bits)
//   VHPOSR   ($DFF006)  Read  light pen position  (low order bits)
//The light pen register is at the same address as the beam counters. The
//bits are as follows:

//   VPOSR:     Bit 15       Long frame/short frame.  0=short frame ->
//              Bits 14-1    Chip ID code.  Do not depend on value!
//              Bit 0        V8 (most significant bit of vertical position)

//   VHPOSR:    Bits 15-8    V7-V0 (vertical position)
//              Bits 7-0     H8-H1 (horizontal position)
}



//#include <exec/types.h>
//#include "intuition/intuition.h"
//#include <exec/ports.h>
//#include <exec/io.h>
//#include <exec/memory.h>
//#include <devices/input.h>
//#include <exec/devices.h>
//#include <devices/inputevent.h>
//#include "stdio.h"
//#include "stdlib.h"
//#include "graphics/gfxbase.h"
//#include "graphics/view.h"
//#include "proto/dos.h"
//#include "proto/exec.h"
//#include "proto/intuition.h"
//#include "proto/graphics.h"
//#include "hardware/custom.h"

///***************** CONSTANTS ********************/
//#define LP_ENABLE 0x08
//#define XCOMPENSATE 42 /* This is to compensate some timing problems.
//			  You might need to change this. Uh! Again the
//			  answer is 42! What was the question!? */

//#define BANNER "\x9B\x30;33mLightpenhandler Version 1.1\x9B\x30m (c) by Andreas Klingler 1988,1991\n"
//#define ACTIVATED " Handler installed\n"
//#define TERMINATING " Handler removed\n"
//#define PORTNAME "lightpen_AKK.port"

///****************** GLOBAL VARIABLES ************/
//extern struct Custom __far custom;

//struct MsgPort *inputDevPort;
//struct IOStdReq *inputRequestBlock;
//struct Interrupt handlerStuff;
//struct OURMSG {
//    struct Message msgpart;
//    short dummy;
//    } msg;

//struct LpInfo { short *ViewX, *ViewY;  } handlerdata;

///* Prototypes for functions defined in lp.c */
//struct InputEvent * __regargs __saveds myhandler(struct InputEvent *,
//                                                 struct LpInfo *);
//void main(void);
//void chkabort(void);


///* Declarations for CBACK */
//extern BPTR _Backstdout;         /* standard output when run in background */
//long _BackGroundIO = 1;          /* Flag to tell it we want to do I/O      */
//long _stack = 4000;              /* Amount of stack space our task needs   */
//char *_procname = "Lightpenhandler";  /* The name of the task to create    */
//long _priority = 20;             /* The priority to run us at              */

///******************* Here goes the Handler ****************/
//struct InputEvent *
//__regargs __saveds myhandler(ev, mydata)
//	struct InputEvent *ev;  /* and a pointer to a list of events */
//	struct LpInfo *mydata;  /* system will pass me a pointer to my
//				      own data space. */

//{
//	static struct InputEvent lpev;
//	struct InputEvent *retwert;
//	BOOL lpen_invalid = FALSE,button_pressed = FALSE;
//	static BOOL left_down,right_down;
//	ULONG pos;
//	static ULONG *lpen = (ULONG *)&custom.vposr;
//	register short x,y;

//	pos = *lpen;	/* read pen position (we read two
//			   registers here: VPOSR,VHPOSR) */

//	Forbid();
//	  if ((pos != *lpen) || /* Test, if Lpen has been
//				   triggered since VBLANK */
//	     ((pos & 0x1ffff) > 0x12a00)) {  retwert=ev; lpen_invalid=TRUE; }
//	   else retwert = &lpev;

//	/* Translate in HIRES INLACE coordinates (depending of position of
//	   the View). That`s what mouse coordinates are in.
//	   Since we have only 8 bits for the horizonal position it gets
//	   a little bit tricky (there are 640 pixel in one line in hires!) */
//	  x = (short)((pos <<1) & 0x1ff);
//	  if (x < 100) x += 454;
// 	  y = ((short) (pos >> 8) &0x1ff) - *(mydata->ViewY);
// 	  x -= (*(mydata->ViewX) + XCOMPENSATE);

//	  lpev.ie_NextEvent = ev;  /* link lpen ahead of event stream */
//	  lpev.ie_Class = IECLASS_POINTERPOS;
//	  lpev.ie_Code = IECODE_NOBUTTON;
//	  lpev.ie_Qualifier = 0;
//	  lpev.ie_TimeStamp.tv_secs = 0;
//	  lpev.ie_TimeStamp.tv_micro = 0;
//	  lpev.ie_X = x << 1;
//	  lpev.ie_Y = y << 1;

//	/* Button on lightpen pressed ? */
//	  if (right_down) {
//	  	if (!(custom.joy1dat & 1)) { lpev.ie_Code = IECODE_RBUTTON |
//							      IECODE_UP_PREFIX;
//				  	     right_down = FALSE;
//					     button_pressed = TRUE;
//					   }
//		        }
//	  else if (custom.joy1dat & 1) { lpev.ie_Code = IECODE_RBUTTON;
//				         right_down = TRUE;
//					 button_pressed = TRUE;
//					}
//	  if (left_down) {
//	  		if (!(custom.joy1dat & 0x100)){
//					lpev.ie_Code = IECODE_LBUTTON |
//						       IECODE_UP_PREFIX;
//					left_down = FALSE;
//					button_pressed = TRUE;
//					}
//		        }
//		else    if (custom.joy1dat & 0x100) {
//					lpev.ie_Code = IECODE_LBUTTON;
//					left_down = TRUE;
//					button_pressed = TRUE;
//					}

//	/* Append positiondata only if it is valid */
//	  if (button_pressed && lpen_invalid) {
//			lpev.ie_Class = IECLASS_RAWMOUSE;
//			lpev.ie_Qualifier = IEQUALIFIER_RELATIVEMOUSE;
//			lpev.ie_X = lpev.ie_Y = 0;
//			retwert = &lpev;
//			}
//	Permit();
//	return (retwert);
//}


//void main()
//{
//	struct MsgPort *port;
//	SHORT error;


//	if (_Backstdout) Write(_Backstdout, BANNER, sizeof(BANNER));

//	/* now see if we are allready installed */
//      if (!(port = FindPort(PORTNAME)))  {
//        /* if not, install ourself : */
//	if ((port = CreatePort(PORTNAME,0)) == NULL) _exit (19);

//	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
//	if (!GfxBase) _exit(20);

//	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
//	if (!IntuitionBase) _exit(21);

//	/* Tell the OS about the lightpen and enable the hardware */
//	GfxBase->system_bplcon0 |= LP_ENABLE;
//	RemakeDisplay();

//	/* Determine position of the View. Attention: if someone pokes
//	   around with Preferences while we are running, we can`t handle
//	   this! */
//	handlerdata.ViewX = (short *)&(GfxBase->ActiView->DxOffset);
//	handlerdata.ViewY = (short *)&(GfxBase->ActiView->DyOffset);

//	inputDevPort = CreatePort(0,0);		/* for input device */
//	if(inputDevPort == NULL) _exit(1);	/* error during createport */
//	inputRequestBlock=CreateStdIO(inputDevPort);
//	if(inputRequestBlock == 0) { DeletePort(inputDevPort); _exit(2); }
//					/* error during createstdio */


//	handlerStuff.is_Data = (APTR)&handlerdata;
//			/* address of its data area */
//	handlerStuff.is_Code = (void *)myhandler;
//			/* address of entry point to handler */
//	handlerStuff.is_Node.ln_Pri = 51;
//			/* set the priority one step higher than
//		 	 * Intuition, so that our handler enters
//			 * the chain ahead of Intuition.
//			 */
//	error = OpenDevice("input.device",0,(struct IORequest *)inputRequestBlock,0);
//	if(error) _exit(23);

//	inputRequestBlock->io_Command = IND_ADDHANDLER;
//	inputRequestBlock->io_Data = (APTR)&handlerStuff;

//	DoIO((struct IORequest *)inputRequestBlock);  /* Activate the Handler */

//	/* The Handler is running. Close everything we don`t need
//	   anymore and wait until it is time to uninstall the handler.
//	   All the dirty work is done by the handler */

//	CloseLibrary((struct Library *)IntuitionBase);
//	CloseLibrary((struct Library *)GfxBase);

//	if (_Backstdout) { Write(_Backstdout,ACTIVATED, sizeof(ACTIVATED));
//			   Close(_Backstdout);
//			   }
//	WaitPort(port);  		/* Wait until we get message */
//	GetMsg(port);			/* to exit, then remove it */
//					/* and get out		   */

//	inputRequestBlock->io_Command = IND_REMHANDLER;
//	inputRequestBlock->io_Data = (APTR)&handlerStuff;
//	DoIO((struct IORequest *)inputRequestBlock);

//	/* close the input device */
//	CloseDevice((struct IORequest *)inputRequestBlock);

//	/* delete the IO request */
//	DeleteStdIO(inputRequestBlock);

//	/* free other system stuff */
//	DeletePort(inputDevPort);
//	DeletePort(port);
//      }   /* this is the end of the BIG-IF (and of the program */

//      else  /* in this case the handler is already installed, so we
//	       send a message to it to make it deinstall itself */
//	{
//	msg.msgpart.mn_Length = sizeof(struct OURMSG);
//	PutMsg(port,(struct Message *)&msg);
//	if (_Backstdout) { Write(_Backstdout, TERMINATING, sizeof(TERMINATING));
//			   Close(_Backstdout);
//			   }
//       }
//}					/* end of main */

//void MemCleanup() {}	/* We don`t want this out of lc.lib */

//void chkabort() {}


