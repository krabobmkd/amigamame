/*
 * mui_bitmap_dt.h - loads a (optionally transparent) picture file via
 * picture.datatype and wraps it as a ready-to-embed MUI object.
 *
 * Replaces the former MUIC_GIFAnim/"5:<file>" MUIC_Image usage for the
 * corner logo: those paths were seen writing past allocated memory under
 * a memory-hit tool. This module does the decode via datatypes.library /
 * picture.datatype -- same technique already used in
 * ../../FriendSh3ep/bmimage.c -- and hands the resulting struct BitMap*
 * plus its PDTA_MaskPlane mask, both unmodified, to a small MUI custom
 * class that blits them with BltMaskBitMapRastPort() (see mui_bitmap_dt.c
 * for why: MUI's own Bitmap.mui class has no tag to accept an existing
 * mask plane, and its MUIA_Bitmap_Transparent single-pixel-value approach
 * doesn't work reliably once the source colour has gone through
 * screen/palette remapping).
 *
 * Lifecycle:
 *   MUIBitmapDT_OpenLibrary() -- once at startup, before any Load() call.
 *   MUIBitmapDT_Load()        -- any time after Open, once MUIMasterBase is
 *                                 open too; may be called several times
 *                                 (one retained dtObject per loaded image).
 *   MUIBitmapDT_CloseLibrary()-- once at final teardown, AFTER MUI has
 *                                 disposed of every object returned by
 *                                 Load() (that object only points at the
 *                                 struct BitMap/mask, it never copies them
 *                                 -- disposing the datatype object first
 *                                 would hand MUI dangling pointers while it
 *                                 tears down the window/app tree).
 */

#ifndef MUI_BITMAP_DT_H
#define MUI_BITMAP_DT_H

#include <exec/types.h>
#include <intuition/classusr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opens datatypes.library if not already open. Returns FALSE if it could
 * not be opened -- MUIBitmapDT_Load() then always returns NULL. */
BOOL MUIBitmapDT_OpenLibrary(void);

/* Disposes every picture.datatype object retained by MUIBitmapDT_Load()
 * calls so far and deletes the internal MUI custom class, then closes
 * datatypes.library. Call once, at final teardown, only after MUI has
 * already disposed of (or will never dispose of) every object
 * MUIBitmapDT_Load() returned. */
void MUIBitmapDT_CloseLibrary(void);

/*
 * Loads path via picture.datatype, remapped to the current default public
 * screen's bitmap format/depth, and returns a MUI Object* sized to the
 * image, ready to use as a MUI Child. If the source declares a transparent
 * colour (e.g. a GIF palette entry flagged transparent), picture.datatype's
 * own generated mask plane (PDTA_MaskPlane) is used as-is to blit the
 * image, background shining through untouched pixels -- no per-pixel work
 * on this module's part.
 *
 * Returns NULL -- nothing left allocated -- if the file doesn't exist,
 * datatypes.library isn't open, MUIMasterBase isn't open yet, or decoding
 * fails.
 *
 * The returned object's struct BitMap/mask are owned by an internal
 * picture.datatype object retained past this call; see
 * MUIBitmapDT_CloseLibrary() for the disposal rule.
 */
Object *MUIBitmapDT_Load(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* MUI_BITMAP_DT_H */
