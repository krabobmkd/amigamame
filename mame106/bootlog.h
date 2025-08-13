#ifndef BOOTLOG_H_
#define BOOTLOG_H_

#ifdef __cplusplus
extern "C" {
#endif

    void bootlog_setromload(int i, int nbroms);
    void bootlog_setvideodecode(int i, int nbgfx);
    void bootlog_setDecrypt(int  dec, int nbdec);

    typedef enum {
        ebStart=0,
        ebInput,
        ebRomLoad,
        ebMemoryAndCpu,
        ebHighScoreSaveLoad,
        eDriver,
        eSoundVideo,
        eCheat, // OPTIONAL
        eCpuDecrypt,
        eProgressEnd
    } eProgress;

    void bootlog_setprogress(eProgress e);

#ifdef __cplusplus
}
#endif


#endif
