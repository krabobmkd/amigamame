#include "bootlog.h"


void bootlog_setprogress(eProgress e)
{
    // enum to percent.
    int romloadweight=40;
    int initvidweight=25;
    int restweigth = 100-(romloadweight+initvidweight);

    int enumpercent = ((int)restweigth * 100) / (int)eProgressSize;

    if(enumpercent>100) enumpercent=100;


}
