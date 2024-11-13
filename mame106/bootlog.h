#ifndef BOOTLOG_H_
#define BOOTLOG_H_

#ifdef __cplusplus
extern "C" {
#endif

    void bootlog_setromload(int i, int nbroms);

    typedef enum {
        ebStart=0,
        ebInput,
        ebRomLoad,
        ebMemoryAndCpu,
        ebHighScoreSaveLoad,
        eDriver, //  decryption is done and memory maps altered
        eSoundVideo,
        eCheat,
        eProgressSize


    } eProgress;

    void bootlog_setprogress(eProgress e);

#ifdef __cplusplus
}
#endif


#endif
