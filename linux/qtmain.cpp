#include <QApplication>
#include <QString>
#include <QList>
#include <QVector>
#include <iostream>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <vector>
#include <cmath>
#include <QThread>
#include <QMutex>
#include "lconfig.h"
#include "qtmain.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <stdio.h>
#include <sstream>
#include <fstream>

extern "C" {
    #include "osdepend.h"
    #include "mamecore.h"
    #include "mame.h"
    #include "driver.h"
    #include "input.h"
}
#include <chrono>
#define STATCPUINSTR 1
#ifdef STATCPUINSTR
extern "C" {
    #include "cpustats.h"
}
#endif


using namespace std;




extern "C" {
int soundMixerIsOn() {
    return 1;
}

}

// // conv tool here because fine.
// void imageToMigabm()
// {
//     QImage imgb;
//      imgb.load("mamelogo128.gif");
//     QImage img = imgb.convertToFormat(QImage::Format_Indexed8);
//  QImage::Format f =img.format();
//  cout << "f:"<< (int)f;
//     int fwidth = img.width()>>3;
//     vector<uint8_t> planar(fwidth *img.height() );

//     for (int y = 0; y < img.height(); y++) {
//         const uchar* scanLine = img.constScanLine(y);
//         for(int w=0;w<fwidth ;w++)
//         {
//             uint8_t c=0;
//             c|= (*scanLine++ >10)<<7;
//             c|= (*scanLine++>10)<<6;
//             c|= (*scanLine++>10)<<5;
//             c|= (*scanLine++>10)<<4;
//             c|= (*scanLine++>10)<<3;
//             c|= (*scanLine++>10)<<2;
//             c|= (*scanLine++>10)<<1;
//             c|= (*scanLine++>10);
//             planar[y*fwidth + w]=c;
//         }

//         //memcpy(pixelData + (img.width() * y), scanLine, img.width());
//     }
// //    stringstream ss;
//     ofstream fileimgd("imgdata.c");
//     int nbc=0;
//     for(size_t i=0 ; i<planar.size()  ; i++)
//     {
//         uint8_t c = planar[i];
//         fileimgd << (uint32_t) c;
//         if(i<planar.size()-1) fileimgd << ",";
//         nbc++;
//         if(nbc>= 32) {
//             fileimgd << "\n";
//             nbc=0;
//         }

//     }



//    // Format_Indexed8 QImage convertedImage = image.convertToFormat(QImage::Format_RGB32);
// }

void StartGame(int idriver)
{

    if(!drivers[idriver]) return;
    MameConfig &conf = getMainConfig();

    conf.applyToMameOptions(options);

    // - - games can have has rotation + we add our explicit rotation ,
    // and we must rotate the tab setting interface the other way to compensate.
    // silly isn't it ?
    int gameorientation = drivers[idriver]->flags & ORIENTATION_MASK;

    UINT32 uiapplied = 0;
//    int uiorientation = ROT0;
    if(gameorientation == ROT90) uiapplied=ROT270;
    if(gameorientation == ROT270) uiapplied=ROT90;
    if(gameorientation == ROT180) uiapplied=ROT180;
    if(gameorientation == ORIENTATION_FLIP_X) uiapplied =ORIENTATION_FLIP_X;
    if(gameorientation == ORIENTATION_FLIP_Y) uiapplied =ORIENTATION_FLIP_Y;

    // done here
//    if(drivers[idriver])
    options.ui_orientation = uiapplied & ORIENTATION_MASK;  // uiorientation;
    options.skip_disclaimer = 1;
    options.skip_gameinfo = 1;
    options.skip_warnings = 1;
    options.use_samples = 1;
    options.pause_bright = 1.0f;
options.samplerate = 22050;

    // vector things
    options.beam = 0x0001c000;               /* vector beam width */
    options.vector_flicker = 0.0f;     /* float vector beam flicker effect control */
    options.vector_intensity = 1.5f;  /* float vector beam intensity 1.5f defaulty */
    options.translucency = 1;  /* 1 to enable translucency on vectors */
    options.antialias = 1;  /* 1 to enable antialias on vectors */
//    vector_width
    options.vector_glow = 0;

    /* Clear the zip filename caches. */

    osd_set_mastervolume(0);
    printf("before run_game\n");
        run_game(idriver);
    printf("after run_game\n");

 //re?   unzip_cache_clear();
    mame_pause(0);// remove pause that could be set while quiting previous game.

//todo ?
//  if(options.playback)
//    osd_fclose(options.playback);

//  if(options.record)
//    osd_fclose(options.record);
}
int nbframe=0;
int w = 320;
int h = 224;
vector<uint8_t> bm(w * h * 3, 128);
QProc *proc=nullptr;
QThread *qthread = nullptr;
struct _mame_display *_display=nullptr;
QImage _image; //(bm.data(), w, h, QImage::Format_RGB888);
QMutex _imageMutex;
bool imageok=false;
bool isinexit=false;
QMutex m_mutex;
bool m_bIs15b = false;
bool m_bIs32b = false;
int m_nbtest=0;
QProc::QProc() : QObject()
{}

extern "C" { extern int isoth; }

void QProc::process()
{
    getMainConfig().init(0,nullptr);
//    getMainConfig().load();

    int idriver = -1;
    m_nbtest = 0;
    // test if just "mame romname".
    {
        int idriver = getMainConfig().driverIndex().index(

       //"nightstr"
      //  "bigrun"
     // "cischeat"
    // "sci"
      //"sharrier"
//      "arkanoid"
 "starwars"
//"cchasm"
//"startrek"
//"tacscan"
 //     "othunder"
      // "thndrbld"
      // "chasehq"
//"gforce2"
        );
        if(idriver>0)
        {
            getMainConfig().setActiveDriver(idriver);
            StartGame(idriver);
        }
    }
   //  nbframe = 0;
   //  m_nbtest = 1;
   //  isoth =1;
   //  {
   //      int idriver = getMainConfig().driverIndex().index("othunder");

   //      //  if game was explicit, no GUI
   //      if(idriver>0)
   //      {
   //          getMainConfig().setActiveDriver(idriver);
   //          StartGame(idriver);
   //      }
   // }
}

QWin::QWin() : QLabel()
{
 //   m_mutex.lock(); // reverse mutex

    qthread = new QThread();
    qthread->start();

    proc = new  QProc();
    proc->moveToThread(qthread);
    connect(this,&QWin::startproc, proc,&QProc::process);

    emit startproc();
    connect(&m_timer,&QTimer::timeout,this,&QWin::updateWin);
    m_timer.setSingleShot(false);
    m_timer.start(1000/40);
    show();
    setMouseTracking(true);

}
extern "C" {
unsigned int GetDisplayGoodiesFlags()
{
 return 3;
}
}
// extern "C" {
// extern int dbg_nbt;
// }
void QWin::mouseMoveEvent(QMouseEvent* event)
{
  //  dbg_nbt = event->x()<<1;

}
void QWin::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter p(this);
// dbg_nbt
    p.setPen(QPen(Qt::white));
    p.drawText(60,60,QString("woot:")+QString::number(nbframe));

//    p.drawText(60,60,QString("woot:")+QString::number(nbframe));
}
void QWin::updateWin()
{

    if(!_display || !_display->game_bitmap ) return;
   // struct _mame_display *display=_display;

    if(imageok)
    {
    _imageMutex.lock();
        int w = _image.width();
        int h = _image.height();
        int izoom = 1;
    QPixmap qpx = QPixmap::fromImage(_image).scaled(QSize(w * izoom, h * izoom));
    _imageMutex.unlock();



        static bool resizedone = false;
        if (!resizedone && w > 0 && h > 0)
        {
            this->setFixedSize(w * izoom, h * izoom);
            resizedone = true;
        }
        this->setPixmap(qpx);
        imageok = false;
    }
   // QThread::msleep(40);


	//lbl.show();
//    m_mutex.unlock();
//    m_mutex.lock();
}
extern "C" {
void logEntries();
}
int main(int argc, char* argv[])
{
    // imageToMigabm();
    // return 0;

	QApplication a(argc, argv);
    QWin w;
        int r =  a.exec();
    isinexit = true;
 //   logEntries();


    cpustats_log();

	return r;
}


int osd_init()
{
  return(0);
}

int osd_create_display(const osd_create_params *params, UINT32 *rgb_components)
{
    if((params->video_attributes & VIDEO_RGB_DIRECT) && (rgb_components))
    {
        m_bIs15b = true;
        if (params->video_attributes & VIDEO_NEEDS_6BITS_PER_GUN)
        {
            rgb_components[0] = 0x00ff0000;
            rgb_components[1] = 0x0000ff00;
            rgb_components[2] = 0x000000ff;
            m_bIs32b = true;
        }
        else
        {
            rgb_components[0] = 0x00007c00;
            rgb_components[1] = 0x000003e0;
            rgb_components[2] = 0x0000001f;
        }
    }


    return 0;
}
void osd_close_display(void)
{

}
int osd_skip_this_frame(void)
{
    return 0;
}

void osd_update_video_and_audio(struct _mame_display *display)
{
    if(isinexit) return;
    _display = display;



    uint16_t *p = (uint16_t *) _display->game_bitmap->base;
    int x1 = _display->game_visible_area.min_x;
    int y1 = _display->game_visible_area.min_y;
    int realHeight = (_display->game_visible_area.max_y - _display->game_visible_area.min_y)+1;
    int realWidth = (_display->game_visible_area.max_x - _display->game_visible_area.min_x)+1;

    if(w != realWidth || h != realHeight)
    {
        w = realWidth;
        h = realHeight;
        bm.resize(w*h*4);
    }

    if (m_bIs32b)
    {
        for (int y = 0; y < realHeight; y++)
        {
            uint32_t* pline = (uint32_t*)display->game_bitmap->line[y + y1];
            pline += x1;
            for (int x = 0; x < realWidth; x++)
            {
                uint32_t rgb = *pline++;

                int i = (x + y * w) * 3;
                bm[i] = (rgb >> 16)  & 0xff;
                bm[i + 1] = (rgb >> 8) & 0xff;
                bm[i + 2] = (rgb) & 0xff;
            }
        }
    } else
    if(m_bIs15b)
    {

        for(int y=0;y<realHeight;y++)
        {
            uint16_t *pline = (uint16_t *)display->game_bitmap->line[y+y1];
            pline += x1;
            for(int x=0;x<realWidth;x++)
            {
                uint16_t rgb = *pline++;

                int i = (x+y*w)*3;
                bm[i]= ((rgb>>10)<<3) & 0xf8;
                bm[i+1]=  ((rgb>>5)<<3) & 0xf8;
                bm[i+2]=((rgb)<<3) & 0xf8;

            }
        }
    } 

    for(int y=0;y<realHeight;y++)
    {
        uint16_t *pline = (uint16_t *)display->game_bitmap->line[y+y1];
        pline += x1;
        for(int x=0;x<realWidth;x++)
        {
            uint16_t c = *pline++;
            uint32_t rgb=0;
            if(c<display->game_palette_entries)
            rgb = display->game_palette[c];

            int i = (x+y*w)*3;
            bm[i]= rgb>>16;
            bm[i+1]= rgb>>8;
            bm[i+2]= rgb;

        }
    }

   _imageMutex.lock();

	QImage image(bm.data(), w, h, QImage::Format_RGB888);

        _image = image;
        imageok = true;

    _imageMutex.unlock();

          // if(nbframe>300)min0 max255 sens100 delta40 cend40  rev0 res:0
    //  while(imageok)
    //  {
       //  QThread::msleep(40);
    // }

nbframe++;
    // logo
//if(nbframe == 60*20+60-4-4-4) mame_pause(1);

// chasehq sprite prio bug:
// if(nbframe == 60*100 && m_nbtest == 0)
// {
//  mame_schedule_exit();
// }
//if(nbframe == 658) mame_pause(1); // ninjaw scene
//if(nbframe == 950) mame_pause(1);

//if(nbframe == 60*25) mame_pause(1);
// if(nbframe==1200) exit(1);
 // static int j=0;
 // if(nbframe==60*5 && j==0) {
 // j=1; mame_schedule_exit();

 // }
//    m_mutex.lock();
//    m_mutex.unlock();

}
mame_bitmap *osd_override_snapshot(mame_bitmap *bitmap, rectangle *bounds)
{


    return bitmap;
}
const char *osd_get_fps_text(const performance_info *performance)
{
    return "";
}
extern "C" { int amigamame_audio_forcemono=0;  }
int osd_start_audio_stream(int stereo)
{
    return 800;
}
int osd_update_audio_stream(INT16 *buffer)
{
    return 800;
}
void osd_stop_audio_stream(void)
{

}
void osd_set_mastervolume(int attenuation)
{

}
int osd_get_mastervolume(void)
{
    return 0;
}
void osd_sound_enable(int enable)
{

}

const os_code_info *osd_get_code_list(void)
{
    static os_code_info l[]={
        {"5",34,KEYCODE_5},
        {"1",35,KEYCODE_1},
        {"A",32,KEYCODE_A},
        {"TAB",33,KEYCODE_TAB},
        {"SPACE",65,KEYCODE_SPACE},
        {"M1AX",1024,MOUSECODE_1_ANALOG_X},
        {"M1AY",1025,MOUSECODE_1_ANALOG_Y},
        {"M1ABT1",1026,MOUSECODE_1_BUTTON1},
        {"M1ABT2",1027,MOUSECODE_1_BUTTON2},
        {"bt4",100,JOYCODE_1_BUTTON4},
        {NULL,0,0},
    };
    return &l[0];
}
int opened=0;
INT32 osd_get_code_value(os_code oscode)
{
// to open menu
    //if(oscode == 33 && nbframe>2*60)
    //{
    //    if(opened==0)
    //    {
    //        opened=1;
    //        return 1;
    //    }
    //    return 0;
    //}
    // if(oscode == 35 && nbframe>4*60)
    // {
    //     //if(opened>0)
    //     {
    //     opened++;
    //         //opened ;
    //         return (opened>>6)&1;
    //     }
    //     //return 0;
    // }
    // if(oscode == 100)
    // {   // bt4

    //     return (opened>>6)&1;
    // }

//    if(oscode==1024)
//    {
//        return 1<<10;
//    }
    return 0;
}
int osd_readkey_unicode(int flush)
{
    return 0;
}
void osd_customize_inputport_list(input_port_default_entry *defaults)
{

}
int osd_joystick_needs_calibration(void)
{
    return 0;
}
void osd_joystick_start_calibration(void)
{

}
const char *osd_joystick_calibrate_next(void)
{
    return NULL;
}
void osd_joystick_calibrate(void)
{

}
void osd_joystick_end_calibration(void)
{

}



cycles_t osd_cycles(void)
{
    using namespace std::chrono;

    system_clock::time_point now = system_clock::now();
    microseconds micros_since_epoch = duration_cast<microseconds>(now.time_since_epoch());
    return micros_since_epoch.count();
//#ifdef __linux__
//    struct timeval tv;
//    gettimeofday(&tv,NULL);
//    return tv.tv_sec * 1000000L + tv.tv_usec;
//    #else
//    static cycles_t o=0;
//    o+= 5465;
//return o;
//    #endif
}
cycles_t osd_cycles_per_second(void)
{
    return 1000000LL;
}
cycles_t osd_profiling_ticks(void)
{
    return osd_cycles();
}


void *osd_alloc_executable(size_t size)
{
    return NULL;
}
void osd_free_executable(void *ptr)
{

}
//int osd_display_loading_rom_message(const char *name,rom_load_data *romdata)
//{
//    return 0;
//}
void osd_update_boot_progress(int per256, int enm)
{

}


int osd_is_bad_read_ptr(const void *ptr, size_t size)
{
    return 0;
}


