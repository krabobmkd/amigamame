/*
 * mui_bitmap_dt.c - see mui_bitmap_dt.h for the lifecycle/rationale.
 *
 * Picture decode/remap pipeline is the same shape as
 * ../../FriendSh3ep/bmimage.c's bmimage_open_file_to_screen(): remap to the
 * screen when it's a CLUT (<=8bpp) screen, PDTA_FreeSourceBitMap TRUE so
 * the intermediate decode is discarded; for a truecolor/RTG screen, decode
 * without remap (PDTA_DestMode=PMODE_V43) to keep full colour. Either way
 * PDTA_MaskPlane gives the transparency mask, if the source has one -- read
 * once and carried through as-is, no processing, exactly like bmimage.c
 * hands its img->mask straight to the caller.
 *
 * MUI's Bitmap.mui class has no tag to accept that mask plane -- its own
 * MUIA_Bitmap_Transparent only recognises a single fixed pixel VALUE, and
 * which raw pixel value a given source colour remaps to (index or RGB
 * chunky) depends on the screen's palette/mode, not something worth
 * chasing after the fact. So instead a tiny MUI custom class
 * (TransBitmapClass below) blits bm+mask straight with
 * BltMaskBitMapRastPort() in MUIM_Draw -- one blitter call, mask consumed
 * unmodified, same primitive fs3estyle.c's BITMAP_MaskPlane usage rides on
 * (there via the images/bitmap.image BOOPSI class; this MUI app has no
 * dependency on that library, so its own minimal class does the same job).
 */

#include "mui_bitmap_dt.h"

#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>
#include <hardware/blit.h>
#include <intuition/screens.h>
#include <intuition/classusr.h>
#include <intuition/classes.h>
#include <libraries/mui.h>
#include <utility/tagitem.h>

#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/alib.h>
#include <proto/datatypes.h>
#include <proto/utility.h>
#include <proto/muimaster.h>

#ifdef __GNUC__
#define REG(r) __asm(#r)
#else
#define REG(r)
#endif

/* Same base-pointer-by-name convention as GfxBase/IntuitionBase/etc. in
 * main.cpp: proto/datatypes.h's inline stubs call through this symbol. */
struct Library *DataTypesBase = NULL;

/* MUIMasterBase/UtilityBase themselves are opened/owned by AllocGUI()/
 * FreeGUI() (gui_mui.cpp) and main.cpp respectively; proto/muimaster.h and
 * proto/utility.h need them in scope under these exact names. */
extern struct Library *MUIMasterBase;
extern struct Library *UtilityBase;

/* ---- TransBitmapClass: minimal MUI Area subclass that blits a
 * datatype-decoded struct BitMap + its PDTA_MaskPlane mask, unmodified. ---- */

struct TransBitmapData
{
    struct BitMap *bm;
    PLANEPTR        mask;   /* NULL if the source had no transparency */
    UWORD           width;
    UWORD           height;
};

/* Private creation-time tags, only ever used to build this class's own
 * instances below -- never exposed outside this file. */
#define MUIA_TransBitmap_BitMap  (TAG_USER | 0x1001)
#define MUIA_TransBitmap_Mask    (TAG_USER | 0x1002)
#define MUIA_TransBitmap_Width   (TAG_USER | 0x1003)
#define MUIA_TransBitmap_Height  (TAG_USER | 0x1004)

static struct MUI_CustomClass *TransBitmapClass = NULL;

static ULONG TransBitmapDispatcher(struct IClass *cl REG(a0), Object *obj REG(a2), Msg msg REG(a1))
{
    switch (msg->MethodID)
    {
        case OM_NEW:
        {
            struct opSet *ops = (struct opSet *)msg;

            obj = (Object *)DoSuperMethodA(cl, obj, msg);
            if (obj)
            {
                struct TransBitmapData *data = (struct TransBitmapData *)INST_DATA(cl, obj);

                data->bm     = (struct BitMap *)GetTagData(MUIA_TransBitmap_BitMap, 0, ops->ops_AttrList);
                data->mask   = (PLANEPTR)GetTagData(MUIA_TransBitmap_Mask,   0, ops->ops_AttrList);
                data->width  = (UWORD)GetTagData(MUIA_TransBitmap_Width,  0, ops->ops_AttrList);
                data->height = (UWORD)GetTagData(MUIA_TransBitmap_Height, 0, ops->ops_AttrList);
            }
            return (ULONG)obj;
        }

        case MUIM_AskMinMax:
        {
            struct TransBitmapData *data = (struct TransBitmapData *)INST_DATA(cl, obj);
            struct MUIP_AskMinMax  *ammsg = (struct MUIP_AskMinMax *)msg;

            DoSuperMethodA(cl, obj, msg);

            ammsg->MinMaxInfo->MinWidth  += data->width;
            ammsg->MinMaxInfo->DefWidth  += data->width;
            ammsg->MinMaxInfo->MaxWidth  += data->width;
            ammsg->MinMaxInfo->MinHeight += data->height;
            ammsg->MinMaxInfo->DefHeight += data->height;
            ammsg->MinMaxInfo->MaxHeight += data->height;
            return 0;
        }

        case MUIM_Draw:
        {
            struct TransBitmapData *data = (struct TransBitmapData *)INST_DATA(cl, obj);
            struct MUIP_Draw       *dmsg = (struct MUIP_Draw *)msg;

            DoSuperMethodA(cl, obj, msg);

            if (data->bm && (dmsg->flags & MADF_DRAWOBJECT))
            {
                /* The layout may hand this object more room than the image
                 * (fixed-size children can still end up in an oversized
                 * box depending on the surrounding group) -- center it
                 * rather than always blitting flush to the top-left. */
                WORD destX = _left(obj) + (_width(obj)  - (WORD)data->width)  / 2;
                WORD destY = _top(obj)  + (_height(obj) - (WORD)data->height) / 2;

                if (data->mask)
                {
                    BltMaskBitMapRastPort(data->bm, 0, 0, _rp(obj),
                        destX, destY, data->width, data->height,
                        (ABC | ABNC | ANBC), data->mask);
                }
                else
                {
                    BltBitMapRastPort(data->bm, 0, 0, _rp(obj),
                        destX, destY, data->width, data->height,
                        (ABC | ABNC));
                }
            }
            return 0;
        }
    }

    return DoSuperMethodA(cl, obj, msg);
}

static BOOL ensure_class(void)
{
    if (TransBitmapClass) return TRUE;
    if (!MUIMasterBase) return FALSE;

    TransBitmapClass = MUI_CreateCustomClass(NULL, MUIC_Rectangle, NULL,
                                              sizeof(struct TransBitmapData),
                                              (APTR)TransBitmapDispatcher);
    return (BOOL)(TransBitmapClass != NULL);
}

/* ---- retained picture.datatype objects (own the struct BitMap/mask every
 * TransBitmapClass instance points at) ---- */

struct RetainedDTObject
{
    struct RetainedDTObject *next;
    Object                  *dtObject;
};

static struct RetainedDTObject *retainedList = NULL;

BOOL MUIBitmapDT_OpenLibrary(void)
{
    if (DataTypesBase) return TRUE;
    DataTypesBase = OpenLibrary((STRPTR)"datatypes.library", 39);
    return (BOOL)(DataTypesBase != NULL);
}

void MUIBitmapDT_CloseLibrary(void)
{
    struct RetainedDTObject *node = retainedList;

    if (TransBitmapClass)
    {
        MUI_DeleteCustomClass(TransBitmapClass);
        TransBitmapClass = NULL;
    }

    while (node)
    {
        struct RetainedDTObject *next = node->next;
        if (node->dtObject) DisposeDTObject(node->dtObject);
        FreeVec(node);
        node = next;
    }
    retainedList = NULL;

    if (DataTypesBase)
    {
        CloseLibrary(DataTypesBase);
        DataTypesBase = NULL;
    }
}

static BOOL retain_dtobject(Object *dtObject)
{
    struct RetainedDTObject *node =
        (struct RetainedDTObject *)AllocVec(sizeof(struct RetainedDTObject), MEMF_ANY);

    if (!node) return FALSE;

    node->dtObject = dtObject;
    node->next     = retainedList;
    retainedList   = node;
    return TRUE;
}

static Object *open_file_as_bitmap_object(const char *path, struct Screen *screen)
{
    Object               *dto  = NULL;
    struct BitMapHeader   *bmhd = NULL;
    struct BitMap         *bm   = NULL;
    PLANEPTR                mask = NULL;
    UWORD                   width = 0, height = 0;
    Object                 *bitmapObj = NULL;

    if (!ensure_class()) return NULL;

    if (GetBitMapAttr(screen->RastPort.BitMap, BMA_DEPTH) <= 8)
    {
        dto = NewDTObject((APTR)path,
            DTA_GroupID,           GID_PICTURE,
            PDTA_Screen,           (ULONG)screen,
            PDTA_Remap,            TRUE,
            PDTA_FreeSourceBitMap, TRUE,
            TAG_DONE);
    }
    else
    {
        dto = NewDTObject((APTR)path,
            DTA_GroupID,             GID_PICTURE,
            PDTA_Screen,             (ULONG)screen,
            PDTA_Remap,              FALSE,
            PDTA_DestMode,           PMODE_V43,
            PDTA_SubClassRendersAll, TRUE,
            TAG_DONE);
    }
    if (!dto) return NULL;

    /* alib's DoMethod(), not DoDTMethod() -- see bmimage.c's file header
     * comment: DoDTMethod()/DoDTMethodA() are known to freeze for at least
     * one picture.datatype method on this codebase's toolchain, no reason
     * to trust the tag-call path here either. */
    DoMethod(dto, DTM_PROCLAYOUT, (ULONG)NULL, (ULONG)TRUE);

    GetDTAttrs(dto, PDTA_BitMapHeader, (ULONG)&bmhd, TAG_DONE);
    if (bmhd)
    {
        width  = bmhd->bmh_Width;
        height = bmhd->bmh_Height;
    }

    GetDTAttrs(dto, PDTA_DestBitMap, (ULONG)&bm, TAG_DONE);
    if (!bm) GetDTAttrs(dto, PDTA_BitMap, (ULONG)&bm, TAG_DONE);

    if (!bm || width < 1 || height < 1)
    {
        DisposeDTObject(dto);
        return NULL;
    }

    /* Carried through unmodified -- see file header comment. */
    GetDTAttrs(dto, PDTA_MaskPlane, (ULONG)&mask, TAG_DONE);

    bitmapObj = NewObject(TransBitmapClass->mcc_Class, NULL,
        MUIA_TransBitmap_BitMap, (ULONG)bm,
        MUIA_TransBitmap_Mask,   (ULONG)mask,
        MUIA_TransBitmap_Width,  (ULONG)width,
        MUIA_TransBitmap_Height, (ULONG)height,
        TAG_DONE);
    if (!bitmapObj)
    {
        DisposeDTObject(dto);
        return NULL;
    }

    if (!retain_dtobject(dto))
    {
        /* Can't track it for later cleanup -- rather leak the small
         * bookkeeping node's absence than hand MUI a live object whose
         * backing BitMap we're about to free out from under it. */
        MUI_DisposeObject(bitmapObj);
        DisposeDTObject(dto);
        return NULL;
    }

    return bitmapObj;
}

Object *MUIBitmapDT_Load(const char *path)
{
    struct Screen *screen;
    Object        *bitmapObj;
    BPTR           hdl;

    if (!path || !path[0] || !DataTypesBase) return NULL;

    hdl = Open((STRPTR)path, MODE_OLDFILE);
    if (!hdl) return NULL;
    Close(hdl);

    screen = LockPubScreen(NULL);
    if (!screen) return NULL;

    bitmapObj = open_file_as_bitmap_object(path, screen);

    UnlockPubScreen(NULL, screen);

    return bitmapObj;
}
