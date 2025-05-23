/***************************************************************************

    Midway DCS Audio Board

****************************************************************************

    There are several variations of this board, which was in use by
    Midway and eventually Atari for almost 10 years.

    DCS ROM-based mono:
        * ADSP-2105 @ 10MHz
        * single channel output
        * 2k external shared program/data RAM
        * ROM-based, up to 8MB total
        * used in:
            Mortal Kombat 2 (1993)
            Cruisin' USA (1994)
            Revolution X (1994)
            Killer Instinct (1994)
            Killer Instinct 2 (1995)
            Cruisin' World (1996)
            Offroad Challenge (1997)

        * 8k external shared program/data RAM
        * used in:
            Mortal Kombat 3 (1994)
            Ultimate Mortal Kombat 3 (1994)
            2 On 2 Open Ice Challenge (1995)
            WWF Wrestlemania (1995)
            NBA Hangtime (1996)
            NBA Maximum Hangtime (1996)
            Rampage World Tour (1997)

    DCS2 RAM-based stereo (Seattle):
        * ADSP-2115 @ 16MHz
        * dual channel output (stereo)
        * SDRC ASIC for RAM/ROM access
        * RAM-based, 2MB total
        * used in:
            War Gods (1995)
            Wayne Gretzky's 3D Hockey (1996)
            Mace: The Dark Age (1996)
            Biofreaks (1997)
            NFL Blitz (1997)
            California Speed (1998)
            Vapor TRX (1998)
            NFL Blitz '99 (1998)
            CarnEvil (1998)
            Hyperdrive (1998)
            NFL Blitz 2000 Gold (1999)

    DCS2 ROM-based stereo (Zeus):
        * ADSP-2104 @ 16MHz
        * dual channel output (stereo)
        * SDRC ASIC for RAM/ROM access
        * ROM-based, up to 16MB total
        * used in:
            Mortal Kombat 4 (1997)
            Invasion (1999)
            Cruisin' Exotica (1999)
            The Grid (2001)

    DCS2 RAM-based stereo (Vegas):
        * ADSP-2104 @ 16MHz
        * dual channel output (stereo)
        * SDRC ASIC for RAM/ROM access
        * RAM-based, 4MB total
        * used in:
            Gauntlet Legends (1998)
            Tenth Degree (1998)
            Gauntlet Dark Legacy (1999)
            War: The Final Assault (1999)

    DCS2 RAM-based stereo (DSIO):
        * ADSP-2181 @ 16.667MHz
        * dual channel output (stereo)
        * custom ASIC for RAM/ROM access
        * RAM-based, 4MB total
        * used in:
            Road Burners (1999)

    DCS2 RAM-based multi-channel (Denver):
        * ADSP-2181 @ 16.667MHz
        * 2-6 channel output
        * custom ASIC for RAM/ROM access
        * RAM-based, 4MB total
        * used in:
            San Francisco Rush: 2049 (1998)

    Unknown other DCS boards:
        * NBA Showtime
        * NBA Showtime / NFL Blitz 2000 Gold
        * Cart Fury

*****************************************************************************

    SDRC (Sound DRAM Control) ASIC
        * Boot ROM = 32k x 8
        * Data ROM = Up to 16MB ROM (4 chip selects)
        * SRAM = 32k x 24 or 8k x 24
            * common map:
                 PGM 0800-0fff -> RAM 4800-4fff
                 PGM 1000-1fff -> RAM 5000-5fff
                 PGM 2000-2fff -> RAM 6000-6fff
                 PGM 3000-3fff -> RAM 7000-7fff
            * bank = 0:
                DATA 0800-0fff -> RAM 0800-0fff
                DATA 1000-17ff -> RAM 0000-07ff
                DATA 1800-1fff -> RAM 1800-1fff
                DATA 2000-27ff -> RAM 1000-17ff
                DATA 2800-2fff -> RAM 2800-2fff
                DATA 3000-37ff -> RAM 2000-27ff
            * bank = 1:
                DATA 0800-0fff -> unmapped
                DATA 1000-17ff -> unmapped
                DATA 1800-1fff -> RAM 3800-3fff
                DATA 2000-27ff -> RAM 3000-37ff
                DATA 2800-2fff -> RAM 2800-2fff
                DATA 3000-37ff -> RAM 2000-27ff

    0480 (reset = XXX0 0X00 0X00 XX00)
        15:13 = SMODE (write only)
          12  = SM_BK (SRAM bank: 0 or 1)
          11  = SM_EN (SRAM enable: 0=disabled, 1=enabled)
         9:7  = ROM_PG (ROM page select: 0-7)
          5   = ROM_MS (ROM memory select: 0=boot memory, 1=data memory)
          4   = ROM_SZ (ROM area size: 0=4k words, 1=1k words)
         1:0  = ROM_ST (ROM memory start: 0=0000, 1=3000, 2=3400, 3=none)

    0481 (reset = 000X 00X0 0X00 XX00)
          15  = AREF_ACT (read only, 1=DRAM auto refresh in progress)
          14  = /MUTE (mute output)
          13  = /LED (LED output)
        11:10 = /RES_TFS (Reset TFS outputs: low bit = channel 1&2, high = channel 3&4)
          8   = TFS_INV (TFS output polarity: 0=same, 1=inverted)
          7   = DM_3WS (DRAM wait states: 0=2, 1=3)
         5:4  = DM_REF (DRAM refresh: 0=disabled, 1=manual, 2=auto, 3=auto 2x)
         1:0  = DM_ST (DRAM memory start: 0=none, 1=0000, 2=3000, 3=3400)

    0482 (reset = XXX0 0000 0000 0000)
        10:0  = DM_PG[10..0] (DRAM page)
        12:0  = EPM_PG[12..0] (EPROM page [low 10 bits used for 4k pages])

    0483 (reset = 1010 0000 1000 0001)
        15:8  = SDRC_ID[7..0] (revision: 5A = ASIC version, A0 = FPGA version)
          7   = SEC_D7
          6   = SEC_D[6..1]
          0   = SEC_D0

****************************************************************************/

#include "driver.h"
#include "cpu/adsp2100/adsp2100.h"
#include "dcs.h"
#include "sound/dmadac.h"
#include "machine/midwayic.h"

#include <math.h>


#define LOG_DCS_TRANSFERS			(0)
#define LOG_DCS_IO					(0)
#define LOG_BUFFER_FILLING			(0)

#define HLE_TRANSFERS				(1)



/*************************************
 *
 *  Constants
 *
 *************************************/

#define LCTRL_OUTPUT_EMPTY			0x400
#define LCTRL_INPUT_EMPTY			0x800

#define IS_OUTPUT_EMPTY()			(dcs.latch_control & LCTRL_OUTPUT_EMPTY)
#define IS_OUTPUT_FULL()			(!(dcs.latch_control & LCTRL_OUTPUT_EMPTY))
#define SET_OUTPUT_EMPTY()			(dcs.latch_control |= LCTRL_OUTPUT_EMPTY)
#define SET_OUTPUT_FULL()			(dcs.latch_control &= ~LCTRL_OUTPUT_EMPTY)

#define IS_INPUT_EMPTY()			(dcs.latch_control & LCTRL_INPUT_EMPTY)
#define IS_INPUT_FULL()				(!(dcs.latch_control & LCTRL_INPUT_EMPTY))
#define SET_INPUT_EMPTY()			(dcs.latch_control |= LCTRL_INPUT_EMPTY)
#define SET_INPUT_FULL()			(dcs.latch_control &= ~LCTRL_INPUT_EMPTY)


/* These are the some of the control register, we dont use them all */
enum
{
	IDMA_CONTROL_REG = 0,	/* 3fe0 */
	BDMA_INT_ADDR_REG,		/* 3fe1 */
	BDMA_EXT_ADDR_REG,		/* 3fe2 */
	BDMA_CONTROL_REG,		/* 3fe3 */
	BDMA_WORD_COUNT_REG,	/* 3fe4 */
	PROG_FLAG_DATA_REG,		/* 3fe5 */
	PROG_FLAG_CONTROL_REG,	/* 3fe6 */

	S1_AUTOBUF_REG = 15,	/* 3fef */
	S1_RFSDIV_REG,			/* 3ff0 */
	S1_SCLKDIV_REG,			/* 3ff1 */
	S1_CONTROL_REG,			/* 3ff2 */
	S0_AUTOBUF_REG,			/* 3ff3 */
	S0_RFSDIV_REG,			/* 3ff4 */
	S0_SCLKDIV_REG,			/* 3ff5 */
	S0_CONTROL_REG,			/* 3ff6 */
	S0_MCTXLO_REG,			/* 3ff7 */
	S0_MCTXHI_REG,			/* 3ff8 */
	S0_MCRXLO_REG,			/* 3ff9 */
	S0_MCRXHI_REG,			/* 3ffa */
	TIMER_SCALE_REG,		/* 3ffb */
	TIMER_COUNT_REG,		/* 3ffc */
	TIMER_PERIOD_REG,		/* 3ffd */
	WAITSTATES_REG,			/* 3ffe */
	SYSCONTROL_REG			/* 3fff */
};


/* these macros are used to reference the SDRC ASIC */
#define SDRC_ROM_ST		((sdrc.reg[0] >> 0) & 3)	/* 0=0000, 1=3000, 2=3400, 3=none */
#define SDRC_ROM_SZ		((sdrc.reg[0] >> 4) & 1)	/* 0=4k, 1=1k */
#define SDRC_ROM_MS		((sdrc.reg[0] >> 5) & 1)	/* 0=/BMS, 1=/DMS */
#define SDRC_ROM_PG		((sdrc.reg[0] >> 7) & 7)
#define SDRC_SM_EN		((sdrc.reg[0] >> 11) & 1)
#define SDRC_SM_BK		((sdrc.reg[0] >> 12) & 1)
#define SDRC_SMODE		((sdrc.reg[0] >> 13) & 7)

#define SDRC_DM_ST		((sdrc.reg[1] >> 0) & 3)	/* 0=none, 1=0000, 2=3000, 3=3400 */
#define SDRC_DM_REF		((sdrc.reg[1] >> 4) & 3)
#define SDRC_DM_3WS		((sdrc.reg[1] >> 7) & 1)
#define SDRC_TFS_INV	((sdrc.reg[1] >> 8) & 1)
#define SDRC_RES_TFS	((sdrc.reg[1] >> 10) & 3)
#define SDRC_LED		((sdrc.reg[1] >> 13) & 1)
#define SDRC_MUTE		((sdrc.reg[1] >> 14) & 1)
#define SDRC_AREF_ACT	((sdrc.reg[1] >> 15) & 1)

#define SDRC_DM_PG		((sdrc.reg[2] >> 0) & 0x7ff)
#define SDRC_EPM_PG		((sdrc.reg[2] >> 0) & 0x1fff)


/* these macros are used to reference the DSIO ASIC */
#define DSIO_EMPTY_FIFO	((dsio.reg[1] >> 0) & 1)
#define DSIO_CUR_OUTPUT	((dsio.reg[1] >> 4) & 1)
#define DSIO_RES_TFS	((dsio.reg[1] >> 10) & 1)
#define DSIO_LED		((dsio.reg[1] >> 13) & 1)
#define DSIO_MUTE		((dsio.reg[1] >> 14) & 1)

#define DSIO_DM_PG		((dsio.reg[2] >> 0) & 0x7ff)


/* these macros are used to reference the DENVER ASIC */
#define DENV_DSP_SPEED	((dsio.reg[1] >> 2) & 3)	/* read only: 1=33.33MHz */
#define DENV_RES_TFS	((dsio.reg[1] >> 10) & 1)
#define DENV_CHANNELS	((dsio.reg[1] >> 11) & 3)	/* 0=2ch, 1=4ch, 2=6ch */
#define DENV_LED		((dsio.reg[1] >> 13) & 1)
#define DENV_MUTE		((dsio.reg[1] >> 14) & 1)

#define DENV_DM_PG		((dsio.reg[2] >> 0) & 0x7ff)



/*************************************
 *
 *  Type definitions
 *
 *************************************/

struct _sdrc_state
{
	UINT16		reg[4];
	UINT8		seed;
};
typedef struct _sdrc_state sdrc_state;


struct _dsio_denver_state
{
	UINT16		reg[4];
	UINT8		start_on_next_write;
};
typedef struct _dsio_denver_state dsio_state;


struct _dcs_state
{
	UINT8		cpunum;
	UINT8		rev;

	/* sound output */
	UINT8		channels;
	UINT16		size;
	UINT16		incs;
	mame_timer *reg_timer;
	mame_timer *sport_timer;
	mame_timer *internal_timer;
	int			ireg;
	UINT16		ireg_base;
	UINT16		control_regs[32];

	/* memory access/booting */
	UINT16 *	bootrom;
	UINT32		bootrom_words;
	UINT16 *	sounddata;
	UINT32		sounddata_words;
	UINT16		sounddata_bank;

	/* I/O with the host */
	UINT8		auto_ack;
	UINT16		latch_control;
	UINT16		input_data;
	UINT16		output_data;
	UINT16		output_control;
	UINT32		output_control_cycles;
	UINT8		last_output_full;
	UINT8		last_input_empty;
	void		(*output_full_cb)(int);
	void		(*input_empty_cb)(int);
	UINT16		(*fifo_data_r)(void);
	UINT16		(*fifo_status_r)(void);

	/* timers */
	UINT8		timer_enable;
	UINT8		timer_ignore;
	UINT64		timer_start_cycles;
	UINT32		timer_start_count;
	UINT32		timer_scale;
	UINT32		timer_period;
	UINT32		timers_fired;
};
typedef struct _dcs_state dcs_state;


struct _hle_transfer_state
{
	int			dcs_state;
	int			state;
	int			start;
	int			stop;
	int			type;
	int			temp;
	int			writes_left;
	UINT16		sum;
	int			fifo_entries;
	mame_timer *watchdog;
};
typedef struct _hle_transfer_state hle_transfer_state;



/*************************************
 *
 *  Statics
 *
 *************************************/

static dcs_state dcs;
static sdrc_state sdrc;
static dsio_state dsio;
static hle_transfer_state transfer;

static UINT16 *dcs_sram;

static UINT16 *dcs_polling_base;

static UINT32 *dcs_internal_program_ram;
static UINT32 *dcs_external_program_ram;




/*************************************
 *
 *  Prototypes
 *
 *************************************/

static READ16_HANDLER( dcs_dataram_r );
static WRITE16_HANDLER( dcs_dataram_w );
static WRITE16_HANDLER( dcs_data_bank_select_w );

static void sdrc_reset(void);
static READ16_HANDLER( sdrc_r );
static WRITE16_HANDLER( sdrc_w );

static void dsio_reset(void);
static READ16_HANDLER( dsio_r );
static WRITE16_HANDLER( dsio_w );

static void denver_reset(void);
static READ16_HANDLER( denver_r );
static WRITE16_HANDLER( denver_w );

static READ16_HANDLER( adsp_control_r );
static WRITE16_HANDLER( adsp_control_w );

static READ16_HANDLER( latch_status_r );
static READ16_HANDLER( fifo_input_r );
static READ16_HANDLER( input_latch_r );
static WRITE16_HANDLER( input_latch_ack_w );
static WRITE16_HANDLER( output_latch_w );
static READ16_HANDLER( output_control_r );
static WRITE16_HANDLER( output_control_w );

static void timer_enable_callback(int enable);
static void internal_timer_callback(int param);
static void dcs_irq(int state);
static void sport0_irq(int state);
static void recompute_sample_rate(void);
static void sound_tx_callback(int port, INT32 data);

static READ16_HANDLER( dcs_polling_r );

static void transfer_watchdog_callback(int param);
static int preprocess_write(UINT16 data);



/*************************************
 *
 *  Original DCS Memory Maps
 *
 *************************************/

/* DCS 2k memory map */
ADDRESS_MAP_START( dcs_2k_program_map, ADDRESS_SPACE_PROGRAM, 32 )
	AM_RANGE(0x0000, 0x03ff) AM_RAM AM_BASE(&dcs_internal_program_ram)
	AM_RANGE(0x0800, 0x0fff) AM_RAM AM_SHARE(1) AM_BASE(&dcs_external_program_ram)
	AM_RANGE(0x1000, 0x17ff) AM_RAM AM_SHARE(1)
	AM_RANGE(0x1800, 0x1fff) AM_RAM AM_SHARE(1)
ADDRESS_MAP_END

ADDRESS_MAP_START( dcs_2k_data_map, ADDRESS_SPACE_DATA, 16 )
	AM_RANGE(0x0000, 0x07ff) AM_MIRROR(0x1800) AM_READWRITE(dcs_dataram_r, dcs_dataram_w)
	AM_RANGE(0x2000, 0x2fff) AM_ROMBANK(20)
	AM_RANGE(0x3000, 0x33ff) AM_WRITE(dcs_data_bank_select_w)
	AM_RANGE(0x3400, 0x37ff) AM_READWRITE(input_latch_r, output_latch_w)
	AM_RANGE(0x3800, 0x39ff) AM_RAM
	AM_RANGE(0x3fe0, 0x3fff) AM_READWRITE(adsp_control_r, adsp_control_w)
ADDRESS_MAP_END


/* DCS 2k with UART memory map */
ADDRESS_MAP_START( dcs_2k_uart_data_map, ADDRESS_SPACE_DATA, 16 )
	AM_RANGE(0x0000, 0x07ff) AM_MIRROR(0x1800) AM_READWRITE(dcs_dataram_r, dcs_dataram_w)
	AM_RANGE(0x2000, 0x2fff) AM_ROMBANK(20)
	AM_RANGE(0x3000, 0x33ff) AM_WRITE(dcs_data_bank_select_w)
	AM_RANGE(0x3400, 0x3402) AM_NOP								/* UART (ignored) */
	AM_RANGE(0x3403, 0x3403) AM_READWRITE(input_latch_r, output_latch_w)
	AM_RANGE(0x3404, 0x3405) AM_NOP								/* UART (ignored) */
	AM_RANGE(0x3800, 0x39ff) AM_RAM
	AM_RANGE(0x3fe0, 0x3fff) AM_READWRITE(adsp_control_r, adsp_control_w)
ADDRESS_MAP_END


/* DCS 8k memory map */
ADDRESS_MAP_START( dcs_8k_program_map, ADDRESS_SPACE_PROGRAM, 32 )
	AM_RANGE(0x0000, 0x03ff) AM_RAM AM_BASE(&dcs_internal_program_ram)
	AM_RANGE(0x0800, 0x1fff) AM_RAM AM_BASE(&dcs_external_program_ram)
ADDRESS_MAP_END

ADDRESS_MAP_START( dcs_8k_data_map, ADDRESS_SPACE_DATA, 16 )
	AM_RANGE(0x0000, 0x07ff) AM_RAM
	AM_RANGE(0x0800, 0x1fff) AM_READWRITE(dcs_dataram_r, dcs_dataram_w)
	AM_RANGE(0x2000, 0x2fff) AM_ROMBANK(20)
	AM_RANGE(0x3000, 0x33ff) AM_WRITE(dcs_data_bank_select_w)
	AM_RANGE(0x3400, 0x37ff) AM_READWRITE(input_latch_r, output_latch_w)
	AM_RANGE(0x3800, 0x39ff) AM_RAM
	AM_RANGE(0x3fe0, 0x3fff) AM_READWRITE(adsp_control_r, adsp_control_w)
ADDRESS_MAP_END



/*************************************
 *
 *  DCS2 Memory Maps
 *
 *************************************/

ADDRESS_MAP_START( dcs2_program_map, ADDRESS_SPACE_PROGRAM, 32 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x0000, 0x03ff) AM_RAM	AM_BASE(&dcs_internal_program_ram)
ADDRESS_MAP_END


ADDRESS_MAP_START( dcs2_data_map, ADDRESS_SPACE_DATA, 16 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x0400, 0x0400) AM_READWRITE(input_latch_r, input_latch_ack_w)
	AM_RANGE(0x0401, 0x0401) AM_WRITE(output_latch_w)
	AM_RANGE(0x0402, 0x0402) AM_READWRITE(output_control_r, output_control_w)
	AM_RANGE(0x0403, 0x0403) AM_READ(latch_status_r)
	AM_RANGE(0x0404, 0x0407) AM_READ(fifo_input_r)
	AM_RANGE(0x0480, 0x0483) AM_READWRITE(sdrc_r, sdrc_w)
	AM_RANGE(0x3800, 0x39ff) AM_RAM
	AM_RANGE(0x3fe0, 0x3fff) AM_READWRITE(adsp_control_r, adsp_control_w)
ADDRESS_MAP_END



/*************************************
 *
 *  DSIO Memory Maps
 *
 *************************************/

ADDRESS_MAP_START( dsio_program_map, ADDRESS_SPACE_PROGRAM, 32 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x0000, 0x3fff) AM_RAM	AM_BASE(&dcs_internal_program_ram)
ADDRESS_MAP_END


ADDRESS_MAP_START( dsio_data_map, ADDRESS_SPACE_DATA, 16 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x0000, 0x03ff) AM_RAMBANK(20)
	AM_RANGE(0x0400, 0x3fdf) AM_RAM
	AM_RANGE(0x3fe0, 0x3fff) AM_READWRITE(adsp_control_r, adsp_control_w)
ADDRESS_MAP_END


ADDRESS_MAP_START( dsio_io_map, ADDRESS_SPACE_IO, 16 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x0400, 0x0400) AM_READWRITE(input_latch_r, input_latch_ack_w)
	AM_RANGE(0x0401, 0x0401) AM_WRITE(output_latch_w)
	AM_RANGE(0x0402, 0x0402) AM_READWRITE(output_control_r, output_control_w)
	AM_RANGE(0x0403, 0x0403) AM_READ(latch_status_r)
	AM_RANGE(0x0404, 0x0407) AM_READ(fifo_input_r)
	AM_RANGE(0x0480, 0x0483) AM_READWRITE(dsio_r, dsio_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Denver Memory Maps
 *
 *************************************/

ADDRESS_MAP_START( denver_program_map, ADDRESS_SPACE_PROGRAM, 32 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x0000, 0x3fff) AM_RAM	AM_BASE(&dcs_internal_program_ram)
ADDRESS_MAP_END


ADDRESS_MAP_START( denver_data_map, ADDRESS_SPACE_DATA, 16 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x0000, 0x07ff) AM_RAMBANK(20)
	AM_RANGE(0x0800, 0x3fdf) AM_RAM
	AM_RANGE(0x3fe0, 0x3fff) AM_READWRITE(adsp_control_r, adsp_control_w)
ADDRESS_MAP_END


ADDRESS_MAP_START( denver_io_map, ADDRESS_SPACE_IO, 16 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x0400, 0x0400) AM_READWRITE(input_latch_r, input_latch_ack_w)
	AM_RANGE(0x0401, 0x0401) AM_WRITE(output_latch_w)
	AM_RANGE(0x0402, 0x0402) AM_READWRITE(output_control_r, output_control_w)
	AM_RANGE(0x0403, 0x0403) AM_READ(latch_status_r)
	AM_RANGE(0x0404, 0x0407) AM_READ(fifo_input_r)
	AM_RANGE(0x0480, 0x0483) AM_READWRITE(denver_r, denver_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Original DCS Machine Drivers
 *
 *************************************/

/* Basic DCS system with ADSP-2105 and 2k of SRAM (T-unit, V-unit, Killer Instinct) */
MACHINE_DRIVER_START( dcs_audio_2k )
	MDRV_CPU_ADD_TAG("dcs", ADSP2105, 10000000)
	MDRV_CPU_PROGRAM_MAP(dcs_2k_program_map,0)
	MDRV_CPU_DATA_MAP(dcs_2k_data_map,0)

	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


/* Basic DCS system with ADSP-2105 and 2k of SRAM, using a UART for communications (X-unit) */
MACHINE_DRIVER_START( dcs_audio_2k_uart )
	MDRV_IMPORT_FROM(dcs_audio_2k)

	MDRV_CPU_MODIFY("dcs")
	MDRV_CPU_DATA_MAP(dcs_2k_uart_data_map,0)
MACHINE_DRIVER_END


/* Basic DCS system with ADSP-2105 and 8k of SRAM (Wolf-unit) */
MACHINE_DRIVER_START( dcs_audio_8k )
	MDRV_IMPORT_FROM(dcs_audio_2k)

	MDRV_CPU_MODIFY("dcs")
	MDRV_CPU_PROGRAM_MAP(dcs_8k_program_map,0)
	MDRV_CPU_DATA_MAP(dcs_8k_data_map,0)
MACHINE_DRIVER_END



/*************************************
 *
 *  DCS2 Machine Drivers
 *
 *************************************/

MACHINE_DRIVER_START( dcs2_audio_2115 )
	MDRV_CPU_ADD_TAG("dcs2", ADSP2115, 16000000)
	MDRV_CPU_PROGRAM_MAP(dcs2_program_map,0)
	MDRV_CPU_DATA_MAP(dcs2_data_map,0)

	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)
MACHINE_DRIVER_END


MACHINE_DRIVER_START( dcs2_audio_2104 )
	MDRV_IMPORT_FROM(dcs2_audio_2115)
	MDRV_CPU_REPLACE("dcs2", ADSP2104, 16000000)
MACHINE_DRIVER_END



/*************************************
 *
 *  DSIO Machine Drivers
 *
 *************************************/

MACHINE_DRIVER_START( dcs2_audio_dsio )
	MDRV_CPU_ADD_TAG("dsio", ADSP2181, 32000000)
	MDRV_CPU_PROGRAM_MAP(dsio_program_map,0)
	MDRV_CPU_DATA_MAP(dsio_data_map,0)
	MDRV_CPU_IO_MAP(dsio_io_map,0)

	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)
MACHINE_DRIVER_END



/*************************************
 *
 *  Denver Machine Drivers
 *
 *************************************/

MACHINE_DRIVER_START( dcs2_audio_denver )
	MDRV_CPU_ADD_TAG("denver", ADSP2181, 33333333)
	MDRV_CPU_PROGRAM_MAP(denver_program_map,0)
	MDRV_CPU_DATA_MAP(denver_data_map,0)
	MDRV_CPU_IO_MAP(denver_io_map,0)

	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(DMADAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)
MACHINE_DRIVER_END



/*************************************
 *
 *  ADSP booting
 *
 *************************************/

static void dcs_boot(void)
{
	UINT8 buffer[0x1000];
	UINT32 max_banks;
	UINT16 *base;
	int i;

	switch (dcs.rev)
	{
		/* rev 1: use the last set data bank to boot from */
		case 1:

			/* determine the base */
			max_banks = dcs.bootrom_words / 0x1000;
			base = dcs.bootrom + ((dcs.sounddata_bank * 0x1000) % dcs.bootrom_words);

			/* convert from 16-bit data to 8-bit data and boot */
			for (i = 0; i < 0x1000; i++)
				buffer[i] = base[i];
			adsp2105_load_boot_data(buffer, dcs_internal_program_ram);
			break;

		/* rev 2: use the ROM page in the SDRC to boot from */
		case 2:

			/* determine the base */
			if (dcs.bootrom == dcs.sounddata)
			{
				/* EPROM case: page is selected from the page register */
				base = dcs.bootrom + ((SDRC_EPM_PG * 0x1000) % dcs.bootrom_words);
			}
			else
			{
				/* DRAM case: page is selected from the ROM page register */
				base = dcs.bootrom + ((SDRC_ROM_PG * 0x1000) % dcs.bootrom_words);
			}

			/* convert from 16-bit data to 8-bit data and boot */
			for (i = 0; i < 0x1000; i++)
				buffer[i] = base[i];
			adsp2115_load_boot_data(buffer, dcs_internal_program_ram);
			break;

		/* rev 3/4: HALT the ADSP-2181 until program is downloaded via IDMA */
		case 3:
		case 4:
			cpunum_set_input_line(dcs.cpunum, INPUT_LINE_HALT, ASSERT_LINE);
			dsio.start_on_next_write = 0;
			break;
	}
}



/*************************************
 *
 *  System reset
 *
 *************************************/

static void dcs_reset(int param)
{
	/* reset the memory banking */
	switch (dcs.rev)
	{
		/* rev 1: just reset the bank to 0 */
		case 1:
			dcs.sounddata_bank = 0;
			memory_set_bankptr(20, dcs.sounddata);
			break;

		/* rev 2: reset the SDRC ASIC */
		case 2:
			sdrc_reset();
			break;

		/* rev 3: reset the DSIO ASIC */
		case 3:
			dsio_reset();
			break;

		/* rev 4: reset the Denver ASIC */
		case 4:
			denver_reset();
			break;
	}

	/* initialize our state structure and install the transmit callback */
	dcs.size = 0;
	dcs.incs = 0;
	dcs.ireg = 0;

	/* initialize the ADSP Tx and timer callbacks */
	cpunum_set_info_fct(dcs.cpunum, CPUINFO_PTR_ADSP2100_TX_HANDLER, (genf *)sound_tx_callback);
	cpunum_set_info_fct(dcs.cpunum, CPUINFO_PTR_ADSP2100_TIMER_HANDLER, (genf *)timer_enable_callback);

	/* initialize the ADSP control regs */
	memset(dcs.control_regs, 0, sizeof(dcs.control_regs));

	/* clear all interrupts */
	cpunum_set_input_line(dcs.cpunum, ADSP2105_IRQ0, CLEAR_LINE);
	cpunum_set_input_line(dcs.cpunum, ADSP2105_IRQ1, CLEAR_LINE);
	cpunum_set_input_line(dcs.cpunum, ADSP2105_IRQ2, CLEAR_LINE);

	/* initialize the comm bits */
	SET_INPUT_EMPTY();
	SET_OUTPUT_EMPTY();
	if (!dcs.last_input_empty && dcs.input_empty_cb)
		(*dcs.input_empty_cb)(dcs.last_input_empty = 1);
	if (dcs.last_output_full && dcs.output_full_cb)
		(*dcs.output_full_cb)(dcs.last_output_full = 0);

	/* boot */
	dcs_boot();

	/* reset timers */
	dcs.timer_enable = 0;
	dcs.timer_scale = 1;
	timer_adjust(dcs.internal_timer, TIME_NEVER, 0, 0);

	/* start the SPORT0 timer */
	if (dcs.sport_timer)
		timer_adjust(dcs.sport_timer, TIME_IN_HZ(1000), 0, TIME_IN_HZ(1000));

	/* reset the HLE transfer states */
	transfer.dcs_state = transfer.state = 0;
}



/*************************************
 *
 *  System setup
 *
 *************************************/

void dcs_init(void)
{
	memset(&dcs, 0, sizeof(dcs));

	/* find the DCS CPU and the sound ROMs */
	dcs.cpunum = mame_find_cpu_index("dcs");
	dcs.rev = 1;
	dcs.channels = 1;

	/* configure boot and sound ROMs */
	dcs.bootrom = (UINT16 *)memory_region(REGION_SOUND1);
	dcs.bootrom_words = memory_region_length(REGION_SOUND1) / 2;
	dcs.sounddata = dcs.bootrom;
	dcs.sounddata_words = dcs.bootrom_words;

	/* create the timers */
	dcs.internal_timer = timer_alloc(internal_timer_callback);
	dcs.reg_timer = timer_alloc(dcs_irq);

	/* non-RAM based automatically acks */
	dcs.auto_ack = TRUE;

	/* reset the system */
	dcs_reset(0);
}


void dcs2_init(int dram_in_mb, offs_t polling_offset)
{
	memset(&dcs, 0, sizeof(dcs));

	/* find the DCS CPU and the sound ROMs */
	dcs.cpunum = mame_find_cpu_index("dcs2");
	dcs.rev = 2;
	if ((INT8)dcs.cpunum == -1)
	{
		dcs.cpunum = mame_find_cpu_index("dsio");
		dcs.rev = 3;
	}
	if ((INT8)dcs.cpunum == -1)
	{
		dcs.cpunum = mame_find_cpu_index("denver");
		dcs.rev = 4;
	}
	dcs.channels = 2;

	/* always boot from the base of REGION_SOUND1 */
	dcs.bootrom = (UINT16 *)memory_region(REGION_SOUND1);
	dcs.bootrom_words = memory_region_length(REGION_SOUND1) / 2;

	/* supports both RAM and ROM variants */
	if (dram_in_mb != 0)
	{
		dcs.sounddata = auto_malloc(dram_in_mb << 20);
		dcs.sounddata_words = (dram_in_mb << 20) / 2;
	}
	else
	{
		dcs.sounddata = dcs.bootrom;
		dcs.sounddata_words = dcs.bootrom_words;
	}

	/* allocate memory for the SRAM */
	dcs_sram = auto_malloc(0x8000*4);

	/* create the timers */
	dcs.internal_timer = timer_alloc(internal_timer_callback);
	dcs.reg_timer = timer_alloc(dcs_irq);
	dcs.sport_timer = timer_alloc(sport0_irq);

	/* we don't do auto-ack by default */
	dcs.auto_ack = FALSE;

	/* install the speedup handler */
	if (polling_offset)
		dcs_polling_base = memory_install_read16_handler(dcs.cpunum, ADDRESS_SPACE_DATA, polling_offset, polling_offset, 0, 0, dcs_polling_r);

	/* allocate a watchdog timer for HLE transfers */
	if (HLE_TRANSFERS)
		transfer.watchdog = timer_alloc(transfer_watchdog_callback);

	/* reset the system */
	dcs_reset(0);
}


void dsio_init(int dram_in_mb, offs_t polling_offset)
{
	dcs2_init(dram_in_mb, polling_offset);
}


void dcs_set_auto_ack(int state)
{
	dcs.auto_ack = state;
}



/*************************************
 *
 *  Original DCS read/write handlers
 *
 *************************************/

static READ16_HANDLER( dcs_dataram_r )
{
	return dcs_external_program_ram[offset] >> 8;
}


static WRITE16_HANDLER( dcs_dataram_w )
{
	UINT16 newdata = dcs_external_program_ram[offset] >> 8;
	COMBINE_DATA(&newdata);
	dcs_external_program_ram[offset] = (newdata << 8) | (dcs_external_program_ram[offset] & 0xff);
}


static WRITE16_HANDLER( dcs_data_bank_select_w )
{
	dcs.sounddata_bank = data & 0x7ff;
	memory_set_bankptr(20, &dcs.sounddata[(dcs.sounddata_bank * 0x1000) % dcs.sounddata_words]);

	/* bit 11 = sound board led */
#if 0
	set_led_status(2, data & 0x800);
#endif
}



/*************************************
 *
 *  SDRC ASIC Memory handling
 *
 *************************************/

INLINE void sdrc_update_bank_pointers(void)
{
	if (SDRC_SM_EN != 0)
	{
		int pagesize = (SDRC_ROM_SZ == 0 && SDRC_ROM_ST != 0) ? 4096 : 1024;

		/* update the bank pointer based on whether we are ROM-based or RAM-based */
		if (dcs.bootrom == dcs.sounddata)
		{
			/* ROM-based; use the memory page to select from ROM */
			if (SDRC_ROM_MS == 1 && SDRC_ROM_ST != 3)
				memory_set_bankptr(25, &dcs.sounddata[(SDRC_EPM_PG * pagesize) % dcs.sounddata_words]);
		}
		else
		{
			/* RAM-based; use the ROM page to select from ROM, and the memory page to select from RAM */
			if (SDRC_ROM_MS == 1 && SDRC_ROM_ST != 3)
				memory_set_bankptr(25, &dcs.bootrom[(SDRC_ROM_PG * 4096 /*pagesize*/) % dcs.bootrom_words]);
			if (SDRC_DM_ST != 0)
				memory_set_bankptr(26, &dcs.sounddata[(SDRC_DM_PG * 1024) % dcs.sounddata_words]);
		}
	}
}


static void sdrc_remap_memory(void)
{
	/* if SRAM disabled, clean it out */
	if (SDRC_SM_EN == 0)
	{
		memory_install_read32_handler (dcs.cpunum, ADDRESS_SPACE_PROGRAM, 0x0800, 0x3fff, 0, 0, MRA32_UNMAP);
		memory_install_write32_handler(dcs.cpunum, ADDRESS_SPACE_PROGRAM, 0x0800, 0x3fff, 0, 0, MWA32_UNMAP);
		memory_install_read16_handler (dcs.cpunum, ADDRESS_SPACE_DATA, 0x0800, 0x37ff, 0, 0, MRA16_UNMAP);
		memory_install_write16_handler(dcs.cpunum, ADDRESS_SPACE_DATA, 0x0800, 0x37ff, 0, 0, MWA16_UNMAP);
	}

	/* otherwise, map the SRAM */
	else
	{
		/* first start with a clean program map */
		memory_install_read32_handler (dcs.cpunum, ADDRESS_SPACE_PROGRAM, 0x0800, 0x3fff, 0, 0, MRA32_BANK21);
		memory_install_write32_handler(dcs.cpunum, ADDRESS_SPACE_PROGRAM, 0x0800, 0x3fff, 0, 0, MWA32_BANK21);
		memory_set_bankptr(21, dcs_sram + 0x4800);

		/* set up the data map based on the SRAM banking */
		/* map 0: ram from 0800-37ff */
		if (SDRC_SM_BK == 0)
		{
			memory_install_read16_handler (dcs.cpunum, ADDRESS_SPACE_DATA, 0x0800, 0x17ff, 0, 0, MRA16_BANK22);
			memory_install_write16_handler(dcs.cpunum, ADDRESS_SPACE_DATA, 0x0800, 0x17ff, 0, 0, MWA16_BANK22);
			memory_install_read16_handler (dcs.cpunum, ADDRESS_SPACE_DATA, 0x1800, 0x27ff, 0, 0, MRA16_BANK23);
			memory_install_write16_handler(dcs.cpunum, ADDRESS_SPACE_DATA, 0x1800, 0x27ff, 0, 0, MWA16_BANK23);
			memory_install_read16_handler (dcs.cpunum, ADDRESS_SPACE_DATA, 0x2800, 0x37ff, 0, 0, MRA16_BANK24);
			memory_install_write16_handler(dcs.cpunum, ADDRESS_SPACE_DATA, 0x2800, 0x37ff, 0, 0, MWA16_BANK24);
			memory_set_bankptr(22, dcs_sram + 0x0000);
			memory_set_bankptr(23, dcs_sram + 0x1000);
			memory_set_bankptr(24, dcs_sram + 0x2000);
		}

		/* map 1: nothing from 0800-17ff, alternate RAM at 1800-27ff, same RAM at 2800-37ff */
		else
		{
			memory_install_read16_handler (dcs.cpunum, ADDRESS_SPACE_DATA, 0x0800, 0x17ff, 0, 0, MRA16_UNMAP);
			memory_install_write16_handler(dcs.cpunum, ADDRESS_SPACE_DATA, 0x0800, 0x17ff, 0, 0, MWA16_UNMAP);
			memory_install_read16_handler (dcs.cpunum, ADDRESS_SPACE_DATA, 0x1800, 0x27ff, 0, 0, MRA16_BANK23);
			memory_install_write16_handler(dcs.cpunum, ADDRESS_SPACE_DATA, 0x1800, 0x27ff, 0, 0, MWA16_BANK23);
			memory_install_read16_handler (dcs.cpunum, ADDRESS_SPACE_DATA, 0x2800, 0x37ff, 0, 0, MRA16_BANK24);
			memory_install_write16_handler(dcs.cpunum, ADDRESS_SPACE_DATA, 0x2800, 0x37ff, 0, 0, MWA16_BANK24);
			memory_set_bankptr(23, dcs_sram + 0x3000);
			memory_set_bankptr(24, dcs_sram + 0x2000);
		}
	}

	/* map the ROM page as bank 25 */
	if (SDRC_ROM_MS == 1 && SDRC_ROM_ST != 3)
	{
		int baseaddr = (SDRC_ROM_ST == 0) ? 0x0000 : (SDRC_ROM_ST == 1) ? 0x3000 : 0x3400;
		int pagesize = (SDRC_ROM_SZ == 0 && SDRC_ROM_ST != 0) ? 4096 : 1024;
		memory_install_read16_handler (dcs.cpunum, ADDRESS_SPACE_DATA, baseaddr, baseaddr + pagesize - 1, 0, 0, MRA16_BANK25);
	}

	/* map the DRAM page as bank 26 */
	if (SDRC_DM_ST != 0)
	{
		int baseaddr = (SDRC_DM_ST == 1) ? 0x0000 : (SDRC_DM_ST == 2) ? 0x3000 : 0x3400;
		memory_install_read16_handler (dcs.cpunum, ADDRESS_SPACE_DATA, baseaddr, baseaddr + 0x3ff, 0, 0, MRA16_BANK26);
		memory_install_write16_handler(dcs.cpunum, ADDRESS_SPACE_DATA, baseaddr, baseaddr + 0x3ff, 0, 0, MWA16_BANK26);
	}

	/* update the bank pointers */
	sdrc_update_bank_pointers();
}


static void sdrc_reset(void)
{
	memset(sdrc.reg, 0, sizeof(sdrc.reg));
	sdrc_remap_memory();
}



/*************************************
 *
 *  SDRC ASIC read/write
 *
 *************************************/

static READ16_HANDLER( sdrc_r )
{
	UINT16 result = sdrc.reg[offset];

	/* offset 3 is for security */
	if (offset == 3)
	{
		switch (SDRC_SMODE)
		{
			default:
			case 0:	/* no-op */
				result = 0x5a81;
				break;

			case 1:	/* write seed */
				result = 0x5aa4;
				break;

			case 2:	/* read data */
				result = 0x5a00 | ((sdrc.seed & 0x3f) << 1);
				break;

			case 3:	/* shift left */
				result = 0x5ab9;
				break;

			case 4:	/* add */
				result = 0x5a03;
				break;

			case 5:	/* xor */
				result = 0x5a69;
				break;

			case 6:	/* prg */
				result = 0x5a20;
				break;

			case 7:	/* invert */
				result = 0x5aff;
				break;
		}
	}

	return result;
}


static WRITE16_HANDLER( sdrc_w )
{
	UINT16 diff = sdrc.reg[offset] ^ data;

	switch (offset)
	{
		/* offset 0 controls ROM mapping */
		case 0:
			sdrc.reg[0] = data;
			if (diff & 0x1833)
				sdrc_remap_memory();
			if (diff & 0x0380)
				sdrc_update_bank_pointers();
			break;

		/* offset 1 controls RAM mapping */
		case 1:
			sdrc.reg[1] = data;
//          dmadac_enable(0, dcs.channels, SDRC_MUTE);
			if (diff & 0x0003)
				sdrc_remap_memory();
			break;

		/* offset 2 controls paging */
		case 2:
			sdrc.reg[2] = data;
			if (diff & 0x1fff)
				sdrc_update_bank_pointers();
			break;

		/* offset 3 controls security */
		case 3:
			switch (SDRC_SMODE)
			{
				case 0:	/* no-op */
				case 2:	/* read data */
					break;

				case 1:	/* write seed */
					sdrc.seed = data & 0xff;
					break;

				case 3:	/* shift left */
					sdrc.seed = (sdrc.seed << 1) | 1;
					break;

				case 4:	/* add */
					sdrc.seed += sdrc.seed >> 1;
					break;

				case 5:	/* xor */
					sdrc.seed ^= (sdrc.seed << 1) | 1;
					break;

				case 6:	/* prg */
					sdrc.seed = (((sdrc.seed << 7) ^ (sdrc.seed << 5) ^ (sdrc.seed << 4) ^ (sdrc.seed << 3)) & 0x80) | (sdrc.seed >> 1);
					break;

				case 7:	/* invert */
					sdrc.seed = ~sdrc.seed;
					break;
			}
			break;
	}
}



/*************************************
 *
 *  DSIO ASIC read/write
 *
 *************************************/

static void dsio_reset(void)
{
	memset(&dsio, 0, sizeof(dsio));
}


static READ16_HANDLER( dsio_r )
{
	UINT16 result = dsio.reg[offset];

	if (offset == 1)
	{
		static UINT16 bits;

		/* bit 4 specifies which channel is being output */
		bits ^= 0x0010;
		result = (result & ~0x0010) | bits;
	}
	return result;
}


static WRITE16_HANDLER( dsio_w )
{
	switch (offset)
	{
		/* offset 1 controls I/O */
		case 1:
			dsio.reg[1] = data;

			/* determine /MUTE and number of channels */
			dmadac_enable(0, dcs.channels, DSIO_MUTE);

			/* bit 0 resets the FIFO */
			midway_ioasic_fifo_reset_w(DSIO_EMPTY_FIFO ^ 1);
			break;

		/* offset 2 controls RAM pages */
		case 2:
			dsio.reg[2] = data;
			memory_set_bankptr(20, &dcs.sounddata[(DSIO_DM_PG * 1024) % dcs.sounddata_words]);
			break;
	}
}



/*************************************
 *
 *  Denver ASIC read/write
 *
 *************************************/

static void denver_reset(void)
{
	memset(&dsio, 0, sizeof(dsio));
}


static READ16_HANDLER( denver_r )
{
	UINT16 result = dsio.reg[offset];

	if (offset == 3)
	{
		/* returns 1 for DRAM, 2 for EPROM-based */
		result = 0x0001;
	}
	return result;
}


static WRITE16_HANDLER( denver_w )
{
	int enable, channels;

	switch (offset)
	{
		/* offset 1 controls I/O */
		case 1:
			dsio.reg[1] = data;

			/* determine /MUTE and number of channels */
			enable = DENV_MUTE;
			channels = 2 + 2 * DENV_CHANNELS;

			/* if the number of channels has changed, adjust */
			if (channels != dcs.channels)
			{
				dcs.channels = channels;
				dmadac_enable(0, dcs.channels, enable);
				if (dcs.channels < 6)
					dmadac_enable(dcs.channels, 6 - dcs.channels, FALSE);
				recompute_sample_rate();
			}
			break;

		/* offset 2 controls RAM pages */
		case 2:
			dsio.reg[2] = data;
			memory_set_bankptr(20, &dcs.sounddata[(DENV_DM_PG * 2048) % dcs.sounddata_words]);
			break;

		/* offset 3 controls FIFO reset */
		case 3:
			midway_ioasic_fifo_reset_w(1);
			break;
	}
}



/*************************************
 *
 *  DSIO/Denver IDMA access
 *
 *************************************/

WRITE32_HANDLER( dsio_idma_addr_w )
{
	if (LOG_DCS_TRANSFERS)
		logerror("%08X:IDMA_addr = %04X\n", activecpu_get_pc(), data);
	cpuintrf_push_context(dcs.cpunum);
	adsp2181_idma_addr_w(data);
	if (data == 0)
		dsio.start_on_next_write = 2;
	cpuintrf_pop_context();
}


WRITE32_HANDLER( dsio_idma_data_w )
{
	UINT32 pc = activecpu_get_pc();
	cpuintrf_push_context(dcs.cpunum);
	if ((mem_mask & 0x0000ffff) != 0x0000ffff)
	{
		if (LOG_DCS_TRANSFERS)
			logerror("%08X:IDMA_data_w(%04X) = %04X\n", pc, adsp2181_idma_addr_r(), data & 0xffff);
		adsp2181_idma_data_w(data & 0xffff);
	}
	if ((mem_mask & 0xffff0000) != 0xffff0000)
	{
		if (LOG_DCS_TRANSFERS)
			logerror("%08X:IDMA_data_w(%04X) = %04X\n", pc, adsp2181_idma_addr_r(), data >> 16);
		adsp2181_idma_data_w(data >> 16);
	}
	cpuintrf_pop_context();
	if (dsio.start_on_next_write && --dsio.start_on_next_write == 0)
	{
		logerror("Starting DSIO CPU\n");
		cpunum_set_input_line(dcs.cpunum, INPUT_LINE_HALT, CLEAR_LINE);
	}
}


READ32_HANDLER( dsio_idma_data_r )
{
	UINT32 result;
	cpuintrf_push_context(dcs.cpunum);
	result = adsp2181_idma_data_r();
	cpuintrf_pop_context();
	if (LOG_DCS_TRANSFERS)
		logerror("%08X:IDMA_data_r(%04X) = %04X\n", activecpu_get_pc(), adsp2181_idma_addr_r(), result);
	return result;
}



/***************************************************************************
    DCS COMMUNICATIONS
****************************************************************************/

void dcs_set_io_callbacks(void (*output_full_cb)(int), void (*input_empty_cb)(int))
{
	dcs.input_empty_cb = input_empty_cb;
	dcs.output_full_cb = output_full_cb;
}


void dcs_set_fifo_callbacks(UINT16 (*fifo_data_r)(void), UINT16 (*fifo_status_r)(void))
{
	dcs.fifo_data_r = fifo_data_r;
	dcs.fifo_status_r = fifo_status_r;
}


int dcs_control_r(void)
{
	/* only boost for DCS2 boards */
	if (!dcs.auto_ack && !HLE_TRANSFERS)
		cpu_boost_interleave(TIME_IN_USEC(0.5), TIME_IN_USEC(5));
	return dcs.latch_control;
}


void dcs_reset_w(int state)
{
	/* going high halts the CPU */
	if (state)
	{
		logerror("%08x: DCS reset = %d\n", safe_activecpu_get_pc(), state);

		/* just run through the init code again */
		timer_set(TIME_NOW, 0, dcs_reset);
		cpunum_set_input_line(dcs.cpunum, INPUT_LINE_RESET, ASSERT_LINE);
	}

	/* going low resets and reactivates the CPU */
	else
		cpunum_set_input_line(dcs.cpunum, INPUT_LINE_RESET, CLEAR_LINE);
}


static READ16_HANDLER( latch_status_r )
{
	int result = 0;
	if (IS_INPUT_FULL())
		result |= 0x80;
	if (IS_OUTPUT_EMPTY())
		result |= 0x40;
	if (dcs.fifo_status_r && (!HLE_TRANSFERS || transfer.state == 0))
		result |= (*dcs.fifo_status_r)() & 0x38;
	if (HLE_TRANSFERS && transfer.state != 0)
		result |= 0x08;
	return result;
}


static READ16_HANDLER( fifo_input_r )
{
	if (dcs.fifo_data_r)
		return (*dcs.fifo_data_r)();
	else
		return 0xffff;
}



/***************************************************************************
    INPUT LATCH (data from host to DCS)
****************************************************************************/

static void dcs_delayed_data_w(int data)
{
	if (LOG_DCS_IO)
		logerror("%08X:dcs_data_w(%04X)\n", activecpu_get_pc(), data);

	/* boost the interleave temporarily */
	cpu_boost_interleave(TIME_IN_USEC(0.5), TIME_IN_USEC(5));

	/* set the IRQ line on the ADSP */
	cpunum_set_input_line(dcs.cpunum, ADSP2105_IRQ2, ASSERT_LINE);

	/* indicate we are no longer empty */
	if (dcs.last_input_empty && dcs.input_empty_cb)
		(*dcs.input_empty_cb)(dcs.last_input_empty = 0);
	SET_INPUT_FULL();

	/* set the data */
	dcs.input_data = data;
}


void dcs_data_w(int data)
{
	/* preprocess the write */
	if (preprocess_write(data))
		return;

	/* if we are DCS1, set a timer to latch the data */
	if (!dcs.sport_timer)
		timer_set(TIME_NOW, data, dcs_delayed_data_w);
	else
	 	dcs_delayed_data_w(data);
}


static WRITE16_HANDLER( input_latch_ack_w )
{
	if (!dcs.last_input_empty && dcs.input_empty_cb)
		(*dcs.input_empty_cb)(dcs.last_input_empty = 1);
	SET_INPUT_EMPTY();
	cpunum_set_input_line(dcs.cpunum, ADSP2105_IRQ2, CLEAR_LINE);
}


static READ16_HANDLER( input_latch_r )
{
	if (dcs.auto_ack)
		input_latch_ack_w(0,0,0);
	if (LOG_DCS_IO)
		logerror("%08X:input_latch_r(%04X)\n", activecpu_get_pc(), dcs.input_data);
	return dcs.input_data;
}



/***************************************************************************
    OUTPUT LATCH (data from DCS to host)
****************************************************************************/

static void latch_delayed_w(int data)
{
	if (!dcs.last_output_full && dcs.output_full_cb)
		(*dcs.output_full_cb)(dcs.last_output_full = 1);
	SET_OUTPUT_FULL();
	dcs.output_data = data;
}


static WRITE16_HANDLER( output_latch_w )
{
	if (LOG_DCS_IO)
		logerror("%08X:output_latch_w(%04X) (empty=%d)\n", activecpu_get_pc(), data, IS_OUTPUT_EMPTY());
	timer_set(TIME_NOW, data, latch_delayed_w);
}


static void delayed_ack_w(int param)
{
	SET_OUTPUT_EMPTY();
}


void dcs_ack_w(void)
{
	timer_set(TIME_NOW, 0, delayed_ack_w);
}


int dcs_data_r(void)
{
	/* data is actually only 8 bit (read from d8-d15) */
	if (dcs.last_output_full && dcs.output_full_cb)
		(*dcs.output_full_cb)(dcs.last_output_full = 0);
	if (dcs.auto_ack)
		delayed_ack_w(0);

	if (LOG_DCS_IO)
		logerror("%08X:dcs_data_r(%04X)\n", activecpu_get_pc(), dcs.output_data);
	return dcs.output_data;
}



/***************************************************************************
    OUTPUT CONTROL BITS (has 3 additional lines to the host)
****************************************************************************/

static void output_control_delayed_w(int data)
{
	if (LOG_DCS_IO)
		logerror("output_control = %04X\n", data);
	dcs.output_control = data;
	dcs.output_control_cycles = 0;
}


static WRITE16_HANDLER( output_control_w )
{
	if (LOG_DCS_IO)
		logerror("%04X:output_control = %04X\n", activecpu_get_pc(), data);
	timer_set(TIME_NOW, data, output_control_delayed_w);
}


static READ16_HANDLER( output_control_r )
{
	dcs.output_control_cycles = activecpu_gettotalcycles();
	return dcs.output_control;
}


int dcs_data2_r(void)
{
	return dcs.output_control;
}



/*************************************
 *
 *  Timer management
 *
 *************************************/

static void update_timer_count(void)
{
	UINT64 periods_since_start;
	UINT64 elapsed_cycles;
	UINT64 elapsed_clocks;

	/* if not enabled, skip */
	if (!dcs.timer_enable)
		return;

	/* count cycles */
	elapsed_cycles = cpunum_gettotalcycles64(dcs.cpunum) - dcs.timer_start_cycles;
	elapsed_clocks = elapsed_cycles / dcs.timer_scale;

	/* if we haven't counted past the initial count yet, just do that */
	if (elapsed_clocks < dcs.timer_start_count + 1)
		dcs.control_regs[TIMER_COUNT_REG] = dcs.timer_start_count - elapsed_clocks;

	/* otherwise, count how many periods */
	else
	{
		elapsed_clocks -= dcs.timer_start_count + 1;
		periods_since_start = elapsed_clocks / (dcs.timer_period + 1);
		elapsed_clocks -= periods_since_start * (dcs.timer_period + 1);
		dcs.control_regs[TIMER_COUNT_REG] = dcs.timer_period - elapsed_clocks;
	}
}


static void internal_timer_callback(int param)
{
	INT64 target_cycles;

	/* compute the absolute cycle when the next one should fire */
	/* we do this to avoid drifting */
	dcs.timers_fired++;
	target_cycles = dcs.timer_start_cycles + dcs.timer_scale * (dcs.timer_start_count + 1 + dcs.timers_fired * (dcs.timer_period + 1));
	target_cycles -= cpunum_gettotalcycles64(dcs.cpunum);

	/* set the next timer, but only if it's for a reasonable number */
	if (!dcs.timer_ignore && (dcs.timer_period > 10 || dcs.timer_scale > 1))
		timer_adjust(dcs.internal_timer, TIME_IN_CYCLES(target_cycles, dcs.cpunum), 0, 0);
	cpunum_set_input_line(dcs.cpunum, ADSP2105_TIMER, PULSE_LINE);
}


static void reset_timer(void)
{
	/* if not enabled, skip */
	if (!dcs.timer_enable)
		return;

	/* compute the time until the first firing */
	dcs.timer_start_cycles = cpunum_gettotalcycles64(dcs.cpunum);
	dcs.timers_fired = 0;

	/* if this is the first timer, check the IRQ routine for the DRAM refresh stub */
	/* if that's all the timer does, we don't really need to fire */
	if (!dcs.timer_ignore)
	{
		/* Road Burners: @ 28: JMP $0032  18032F, same code at $32 */

		cpuintrf_push_context(dcs.cpunum);
		if (program_read_dword(0x18*4) == 0x0c0030 &&		/* ENA SEC_REG */
			program_read_dword(0x19*4) == 0x804828 &&		/* SI = DM($0482) */
			program_read_dword(0x1a*4) == 0x904828 &&		/* DM($0482) = SI */
			program_read_dword(0x1b*4) == 0x0C0020 &&		/* DIS SEC_REG */
			program_read_dword(0x1c*4) == 0x0A001F)			/* RTI */
		{
			dcs.timer_ignore = TRUE;
		}
		cpuintrf_pop_context();
	}

	/* adjust the timer if not optimized */
	if (!dcs.timer_ignore)
		timer_adjust(dcs.internal_timer, TIME_IN_CYCLES(dcs.timer_scale * (dcs.timer_start_count + 1), dcs.cpunum), 0, 0);
}


static void timer_enable_callback(int enable)
{
	dcs.timer_enable = enable;
	dcs.timer_ignore = 0;
	if (enable)
	{
//      printf("Timer enabled @ %d cycles/int, or %f Hz\n", dcs.timer_scale * (dcs.timer_period + 1), 1.0 / TIME_IN_CYCLES(dcs.timer_scale * (dcs.timer_period + 1), dcs.cpunum));
		reset_timer();
	}
	else
	{
//      printf("Timer disabled\n");
		timer_adjust(dcs.internal_timer, TIME_NEVER, 0, 0);
	}
}



/***************************************************************************
    ADSP CONTROL & TRANSMIT CALLBACK
****************************************************************************/

/*
    The ADSP2105 memory map when in boot rom mode is as follows:

    Program Memory:
    0x0000-0x03ff = Internal Program Ram (contents of boot rom gets copied here)
    0x0400-0x07ff = Reserved
    0x0800-0x3fff = External Program Ram

    Data Memory:
    0x0000-0x03ff = External Data - 0 Waitstates
    0x0400-0x07ff = External Data - 1 Waitstates
    0x0800-0x2fff = External Data - 2 Waitstates
    0x3000-0x33ff = External Data - 3 Waitstates
    0x3400-0x37ff = External Data - 4 Waitstates
    0x3800-0x39ff = Internal Data Ram
    0x3a00-0x3bff = Reserved (extra internal ram space on ADSP2101, etc)
    0x3c00-0x3fff = Memory Mapped control registers & reserved.
*/

static READ16_HANDLER( adsp_control_r )
{
	static UINT16 progflags = 0;
	UINT16 result = 0xffff;

	switch (offset)
	{
		case PROG_FLAG_DATA_REG:
			/* Denver waits for this & 0x000e == 0x0000 */
			/* Denver waits for this & 0x000e == 0x0006 */
			result = progflags ^= 0x0006;
			break;

		case IDMA_CONTROL_REG:
			result = adsp2181_idma_addr_r();
			break;

		case TIMER_COUNT_REG:
			update_timer_count();
			result = dcs.control_regs[offset];
			break;

		default:
			result = dcs.control_regs[offset];
			break;
	}
	return result;
}


static WRITE16_HANDLER( adsp_control_w )
{
	dcs.control_regs[offset] = data;

	switch (offset)
	{
		case SYSCONTROL_REG:
			/* bit 9 forces a reset */
			if (data & 0x0200)
			{
				cpunum_set_input_line(dcs.cpunum, INPUT_LINE_RESET, PULSE_LINE);
				dcs_boot();
				dcs.control_regs[SYSCONTROL_REG] = 0;
			}

			/* see if SPORT1 got disabled */
			if ((data & 0x0800) == 0)
			{
				dmadac_enable(0, dcs.channels, 0);
				timer_adjust(dcs.reg_timer, TIME_NEVER, 0, 0);
			}
			break;

		case S1_AUTOBUF_REG:
			/* autobuffer off: nuke the timer, and disable the DAC */
			if ((data & 0x0002) == 0)
			{
				dmadac_enable(0, dcs.channels, 0);
				timer_adjust(dcs.reg_timer, TIME_NEVER, 0, 0);
			}
			break;

		case S1_CONTROL_REG:
            // krb: ... mortal kombat too much
			// if (((data >> 4) & 3) == 2)
			// 	logerror("DCS: Oh no!, the data is compresed with u-law encoding\n");
			// if (((data >> 4) & 3) == 3)
			// 	logerror("DCS: Oh no!, the data is compresed with A-law encoding\n");
			break;

		case TIMER_SCALE_REG:
			data = (data & 0xff) + 1;
			if (data != dcs.timer_scale)
			{
				update_timer_count();
				dcs.timer_scale = data;
				reset_timer();
			}
			break;

		case TIMER_COUNT_REG:
			dcs.timer_start_count = data;
			reset_timer();
			break;

		case TIMER_PERIOD_REG:
			if (data != dcs.timer_period)
			{
				update_timer_count();
				dcs.timer_period = data;
				reset_timer();
			}
			break;

		case IDMA_CONTROL_REG:
			adsp2181_idma_addr_w(data);
			break;
	}
}


/***************************************************************************
    DCS IRQ GENERATION CALLBACKS
****************************************************************************/

static void dcs_irq(int state)
{
	/* get the index register */
	int reg = cpunum_get_reg(dcs.cpunum, ADSP2100_I0 + dcs.ireg);

	/* copy the current data into the buffer */
	{
		int count = dcs.size / 2;
		INT16 buffer[0x400];
		int i;

		cpuintrf_push_context(dcs.cpunum);
		for (i = 0; i < count; i++)
		{
			buffer[i] = data_read_word_16le(reg * 2);
			reg += dcs.incs;
		}
		cpuintrf_pop_context();

		if (dcs.channels)
			dmadac_transfer(0, dcs.channels, 1, dcs.channels, (dcs.size / 2) / dcs.channels, buffer);
	}

	/* check for wrapping */
	if (reg >= dcs.ireg_base + dcs.size)
	{
		/* reset the base pointer */
		reg = dcs.ireg_base;

		/* generate the (internal, thats why the pulse) irq */
		cpunum_set_input_line(dcs.cpunum, ADSP2105_IRQ1, PULSE_LINE);
	}

	/* store it */
	cpunum_set_reg(dcs.cpunum, ADSP2100_I0 + dcs.ireg, reg);
}


static void sport0_irq(int state)
{
	/* this latches internally, so we just pulse */
	/* note that there is non-interrupt code that reads/modifies/writes the output_control */
	/* register; if we don't interlock it, we will eventually lose sound (see CarnEvil) */
	/* so we skip the SPORT interrupt if we read with output_control within the last 5 cycles */
	if ((cpunum_gettotalcycles(dcs.cpunum) - dcs.output_control_cycles) > 5)
		cpunum_set_input_line(dcs.cpunum, ADSP2115_SPORT0_RX, PULSE_LINE);
}


static void recompute_sample_rate(void)
{
	/* calculate how long until we generate an interrupt */

	/* frequency in Hz per each bit sent */
	double sample_rate = (double)Machine->drv->cpu[dcs.cpunum].cpu_clock / (double)(2 * (dcs.control_regs[S1_SCLKDIV_REG] + 1));

	/* now put it down to samples, so we know what the channel frequency has to be */
	sample_rate /= 16 * dcs.channels;
	dmadac_set_frequency(0, dcs.channels, sample_rate);
	dmadac_enable(0, dcs.channels, 1);

	/* fire off a timer wich will hit every half-buffer */
	if (dcs.incs)
		timer_adjust(dcs.reg_timer, TIME_IN_HZ(sample_rate) * (dcs.size / (2 * dcs.channels * dcs.incs)), 0, TIME_IN_HZ(sample_rate) * (dcs.size / (2 * dcs.channels * dcs.incs)));
}


static void sound_tx_callback(int port, INT32 data)
{
	/* check if it's for SPORT1 */
	if (port != 1)
		return;

	/* check if SPORT1 is enabled */
	if (dcs.control_regs[SYSCONTROL_REG] & 0x0800) /* bit 11 */
	{
		/* we only support autobuffer here (wich is what this thing uses), bail if not enabled */
		if (dcs.control_regs[S1_AUTOBUF_REG] & 0x0002) /* bit 1 */
		{
			/* get the autobuffer registers */
			int		mreg, lreg;
			UINT16	source;

			dcs.ireg = (dcs.control_regs[S1_AUTOBUF_REG] >> 9) & 7;
			mreg = (dcs.control_regs[S1_AUTOBUF_REG] >> 7) & 3;
			mreg |= dcs.ireg & 0x04; /* msb comes from ireg */
			lreg = dcs.ireg;

			/* now get the register contents in a more legible format */
			/* we depend on register indexes to be continuous (wich is the case in our core) */
			source = cpunum_get_reg(dcs.cpunum, ADSP2100_I0 + dcs.ireg);
			dcs.incs = cpunum_get_reg(dcs.cpunum, ADSP2100_M0 + mreg);
			dcs.size = cpunum_get_reg(dcs.cpunum, ADSP2100_L0 + lreg);

			/* get the base value, since we need to keep it around for wrapping */
			source -= dcs.incs;

			/* make it go back one so we dont lose the first sample */
			cpunum_set_reg(dcs.cpunum, ADSP2100_I0 + dcs.ireg, source);

			/* save it as it is now */
			dcs.ireg_base = source;

			/* recompute the sample rate and timer */
			recompute_sample_rate();
			return;
		}
		else
			logerror( "ADSP SPORT1: trying to transmit and autobuffer not enabled!\n" );
	}

	/* if we get there, something went wrong. Disable playing */
	dmadac_enable(0, dcs.channels, 0);

	/* remove timer */
	timer_adjust(dcs.reg_timer, TIME_NEVER, 0, 0);
}



/***************************************************************************
    VERY BASIC & SAFE OPTIMIZATIONS
****************************************************************************/

static READ16_HANDLER( dcs_polling_r )
{
	activecpu_eat_cycles(1000);
	return *dcs_polling_base;
}



/***************************************************************************
    DATA TRANSFER HLE MECHANISM
****************************************************************************/

void dcs_fifo_notify(int count, int max)
{
	/* skip if not in mid-transfer */
	if (!HLE_TRANSFERS || transfer.state == 0 || !dcs.fifo_data_r)
	{
		transfer.fifo_entries = 0;
		return;
	}

	/* preprocess a word */
	transfer.fifo_entries = count;
	if (transfer.state != 5 || transfer.fifo_entries == transfer.writes_left || transfer.fifo_entries >= 256)
	{
		for ( ; transfer.fifo_entries; transfer.fifo_entries--)
			preprocess_write((*dcs.fifo_data_r)());
	}
}


static void transfer_watchdog_callback(int starting_writes_left)
{
	if (transfer.fifo_entries && starting_writes_left == transfer.writes_left)
	{
		for ( ; transfer.fifo_entries; transfer.fifo_entries--)
			preprocess_write((*dcs.fifo_data_r)());
	}
	timer_adjust(transfer.watchdog, TIME_IN_MSEC(1), transfer.writes_left, 0);
}


static void s1_ack_callback2(int data)
{
	/* if the output is full, stall for a usec */
	if (IS_OUTPUT_FULL())
	{
		timer_set(TIME_IN_USEC(1), data, s1_ack_callback2);
		return;
	}
	output_latch_w(0, 0x000a, 0);
}


static void s1_ack_callback1(int data)
{
	/* if the output is full, stall for a usec */
	if (IS_OUTPUT_FULL())
	{
		timer_set(TIME_IN_USEC(1), data, s1_ack_callback1);
		return;
	}
	output_latch_w(0, data, 0);

	/* chain to the next word we need to write back */
	timer_set(TIME_IN_USEC(1), 0, s1_ack_callback2);
}


static int preprocess_stage_1(UINT16 data)
{
	switch (transfer.state)
	{
		case 0:
			/* look for command 0x001a to transfer chunks of data */
			if (data == 0x001a)
			{
				if (LOG_DCS_TRANSFERS) logerror("%08X:DCS Transfer command %04X\n", activecpu_get_pc(), data);
				transfer.state++;
				if (HLE_TRANSFERS) return 1;
			}

			/* look for command 0x002a to start booting the uploaded program */
			else if (data == 0x002a)
			{
				if (LOG_DCS_TRANSFERS) logerror("%08X:DCS State change %04X\n", activecpu_get_pc(), data);
				transfer.dcs_state = 1;
			}

			/* anything else is ignored */
			else
			{
				if (LOG_DCS_TRANSFERS) logerror("Command: %04X\n", data);
			}
			break;

		case 1:
			/* first word is the start address */
			transfer.start = data;
			transfer.state++;
			if (LOG_DCS_TRANSFERS) logerror("Start address = %04X\n", transfer.start);
			if (HLE_TRANSFERS) return 1;
			break;

		case 2:
			/* second word is the stop address */
			transfer.stop = data;
			transfer.state++;
			if (LOG_DCS_TRANSFERS) logerror("Stop address = %04X\n", transfer.stop);
			if (HLE_TRANSFERS) return 1;
			break;

		case 3:
			/* third word is the transfer type */
			/* transfer type 0 = program memory */
			/* transfer type 1 = SRAM bank 0 */
			/* transfer type 2 = SRAM bank 1 */
			transfer.type = data;
			transfer.state++;
			if (LOG_DCS_TRANSFERS) logerror("Transfer type = %04X\n", transfer.type);

			/* at this point, we can compute how many words to expect for the transfer */
			transfer.writes_left = transfer.stop - transfer.start + 1;
			if (transfer.type == 0)
				transfer.writes_left *= 2;

			/* reset the checksum */
			transfer.sum = 0;

			/* handle the HLE case */
			if (HLE_TRANSFERS)
			{
				if (transfer.type == 1 && SDRC_SM_BK == 1)
				{
					sdrc.reg[0] &= ~0x1000;
					sdrc_remap_memory();
				}
				if (transfer.type == 2 && SDRC_SM_BK == 0)
				{
					sdrc.reg[0] |= 0x1000;
					sdrc_remap_memory();
				}
				return 1;
			}
			break;

		case 4:
			/* accumulate the sum over all data */
			transfer.sum += data;

			/* if we're out, stop the transfer */
			if (--transfer.writes_left == 0)
			{
				if (LOG_DCS_TRANSFERS) logerror("Transfer done, sum = %04X\n", transfer.sum);
				transfer.state = 0;
			}

			/* handle the HLE case */
			if (HLE_TRANSFERS)
			{
				/* write the new data to memory */
				cpuintrf_push_context(dcs.cpunum);
				if (transfer.type == 0)
				{
					if (transfer.writes_left & 1)
						transfer.temp = data;
					else
						program_write_dword(transfer.start++ * 4, (transfer.temp << 8) | (data & 0xff));
				}
				else
					data_write_word(transfer.start++ * 2, data);
				cpuintrf_pop_context();

				/* if we're done, start a timer to send the response words */
				if (transfer.state == 0)
					timer_set(TIME_IN_USEC(1), transfer.sum, s1_ack_callback1);
				return 1;
			}
			break;
	}
	return 0;
}


static void s2_ack_callback(int data)
{
	/* if the output is full, stall for a usec */
	if (IS_OUTPUT_FULL())
	{
		timer_set(TIME_IN_USEC(1), data, s2_ack_callback);
		return;
	}
	output_latch_w(0, data, 0);
	output_control_w(0, (dcs.output_control & ~0xff00) | 0x0300, 0);
}


static int preprocess_stage_2(UINT16 data)
{
	switch (transfer.state)
	{
		case 0:
			/* look for command 0x55d0 or 0x55d1 to transfer chunks of data */
			if (data == 0x55d0 || data == 0x55d1)
			{
				if (LOG_DCS_TRANSFERS) logerror("%08X:DCS Transfer command %04X\n", activecpu_get_pc(), data);
				transfer.state++;
				if (HLE_TRANSFERS) return 1;
			}

			/* anything else is ignored */
			else
			{
				if (LOG_DCS_TRANSFERS) logerror("%08X:Command: %04X\n", activecpu_get_pc(), data);
			}
			break;

		case 1:
			/* first word is the upper bits of the start address */
			transfer.start = data << 16;
			transfer.state++;
			if (HLE_TRANSFERS) return 1;
			break;

		case 2:
			/* second word is the lower bits of the start address */
			transfer.start |= data;
			transfer.state++;
			if (LOG_DCS_TRANSFERS) logerror("Start address = %08X\n", transfer.start);
			if (HLE_TRANSFERS) return 1;
			break;

		case 3:
			/* third word is the upper bits of the stop address */
			transfer.stop = data << 16;
			transfer.state++;
			if (HLE_TRANSFERS) return 1;
			break;

		case 4:
			/* fourth word is the lower bits of the stop address */
			transfer.stop |= data;
			transfer.state++;
			if (LOG_DCS_TRANSFERS) logerror("Stop address = %08X\n", transfer.stop);

			/* at this point, we can compute how many words to expect for the transfer */
			transfer.writes_left = transfer.stop - transfer.start + 1;

			/* reset the checksum */
			transfer.sum = 0;
			if (HLE_TRANSFERS)
			{
				timer_adjust(transfer.watchdog, TIME_IN_MSEC(1), transfer.writes_left, 0);
				return 1;
			}
			break;

		case 5:
			/* accumulate the sum over all data */
			transfer.sum += data;

			/* if we're out, stop the transfer */
			if (--transfer.writes_left == 0)
			{
				if (LOG_DCS_TRANSFERS) logerror("Transfer done, sum = %04X\n", transfer.sum);
				transfer.state = 0;
			}

			/* handle the HLE case */
			if (HLE_TRANSFERS)
			{
				/* write the new data to memory */
				dcs.sounddata[transfer.start++] = data;

				/* if we're done, start a timer to send the response words */
				if (transfer.state == 0)
				{
					timer_set(TIME_IN_USEC(1), transfer.sum, s2_ack_callback);
					timer_adjust(transfer.watchdog, TIME_NEVER, 0, 0);
				}
				return 1;
			}
			break;
	}
	return 0;
}


static int preprocess_write(UINT16 data)
{
	int result;

	/* if we're not DCS2, skip */
	if (!dcs.sport_timer)
		return 0;

	/* state 0 - initialization phase */
	if (transfer.dcs_state == 0)
		result = preprocess_stage_1(data);
	else
		result = preprocess_stage_2(data);

	/* if we did the write, toggle the full/not full state so interrupts are generated */
	if (result && dcs.input_empty_cb)
	{
		if (dcs.last_input_empty)
			(*dcs.input_empty_cb)(dcs.last_input_empty = 0);
		if (!dcs.last_input_empty)
			(*dcs.input_empty_cb)(dcs.last_input_empty = 1);
	}
	return result;
}
