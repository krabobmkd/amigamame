/***************************************************************************

    Sega Out Run hardware

****************************************************************************

    Known bugs:
        * LED connected to stop lights no longer working

    To do for each game:
        * verify memory test
        * verify inputs
        * verify DIP switches
        * verify protection
        * check playability

***************************************************************************/

#include "driver.h"
#include "system16.h"
#include "machine/segaic16.h"
#include "machine/8255ppi.h"
#include "cpu/m68000/m68000.h"
#include "sound/2151intf.h"
#include "sound/segapcm.h"

#include "cpu/m68000/m68kops.h"

#include "drawCtrl.h"
#include "ui_text.h"
#define MASTER_CLOCK			50000000
#define SOUND_CLOCK				16000000


/*************************************
 *
 *  Statics
 *
 *************************************/

static UINT16 *workram;
static UINT16 *cpu1ram, *cpu1rom;

static UINT8 adc_select=0;

static UINT8 irq2_state;
static UINT8 vblank_irq_state;

static read16_handler custom_io_r;
static write16_handler custom_io_w;

static const UINT8 *custom_map;



/*************************************
 *
 *  Prototypes
 *
 *************************************/

extern void fd1094_machine_init(void);
extern void fd1094_driver_init(void);

static READ16_HANDLER( misc_io_r );
static WRITE16_HANDLER( misc_io_w );

static WRITE8_HANDLER( unknown_porta_w );
static WRITE8_HANDLER( unknown_portb_w );
static WRITE8_HANDLER( video_control_w );



/*************************************
 *
 *  PPI interfaces
 *
 *************************************/

static ppi8255_interface single_ppi_intf =
{
	1,
	{ NULL },
	{ NULL },
	{ NULL },
	{ unknown_porta_w },
	{ unknown_portb_w },
	{ video_control_w }
};



/*************************************
 *
 *  Memory mapping tables
 *
 *************************************/

static const struct segaic16_memory_map_entry outrun_info[] =
{
	{ 0x35/2, 0x90000, 0x10000, 0xf00000,      ~0, segaic16_road_control_0_r, segaic16_road_control_0_w, NULL,                  "road control" },
	{ 0x35/2, 0x80000, 0x01000, 0xf0f000,      ~0, MRA16_BANK10,              MWA16_BANK10,              &segaic16_roadram_0,   "road RAM" },
	{ 0x35/2, 0x60000, 0x08000, 0xf18000,      ~0, MRA16_BANK11,              MWA16_BANK11,              &cpu1ram,              "CPU 1 RAM" },
	{ 0x35/2, 0x00000, 0x60000, 0xf00000,      ~0, MRA16_BANK12,              MWA16_ROM,                 &cpu1rom,              "CPU 1 ROM" },
	{ 0x31/2, 0x00000, 0x04000, 0xffc000,      ~0, misc_io_r,                 misc_io_w,                 NULL,                  "I/O space" },
	{ 0x2d/2, 0x00000, 0x01000, 0xfff000,      ~0, MRA16_BANK13,              MWA16_BANK13,              &segaic16_spriteram_0, "object RAM" },
	{ 0x29/2, 0x00000, 0x02000, 0xffe000,      ~0, MRA16_BANK14,              segaic16_paletteram_w,     &paletteram16,         "color RAM" },
	{ 0x25/2, 0x00000, 0x10000, 0xfe0000,      ~0, MRA16_BANK15,              segaic16_tileram_0_w,      &segaic16_tileram_0,   "tile RAM" },
	{ 0x25/2, 0x10000, 0x01000, 0xfef000,      ~0, MRA16_BANK16,              segaic16_textram_0_w,      &segaic16_textram_0,   "text RAM" },
	{ 0x21/2, 0x60000, 0x08000, 0xf98000,      ~0, MRA16_BANK17,              MWA16_BANK17,              &workram,              "CPU 0 RAM" },
	{ 0x21/2, 0x00000, 0x60000, 0xf80000, 0x00000, MRA16_BANK18,              MWA16_ROM,                 NULL,                  "CPU 0 ROM" },
	{ 0 }
};



/*************************************
 *
 *  Configuration
 *
 *************************************/

static void sound_w(UINT8 data)
{
	soundlatch_w(0, data);
	cpunum_set_input_line(2, INPUT_LINE_NMI, PULSE_LINE);
}


/*
krb cpustats
 ----- cpu 0 -----
 adr:00007de0 nbr:0044e9b4 regir:00006cf8
 adr:00007dda nbr:0044e9b4 regir:00004a79
 adr:00007964 nbr:000e4051 regir:00000c79
 adr:0000796c nbr:000e4051 regir:000067f6
 adr:000078ec nbr:0004c77f regir:000067f8
 adr:000078ea nbr:0004c77f regir:00004a50
 adr:000078e6 nbr:0004c77f regir:000041e8
 adr:00007c68 nbr:0001940c regir:000067f0
 adr:00007c60 nbr:0001940c regir:000008b9
 adr:00007c5a nbr:0001940c regir:00004a79
 adr:00007bf2 nbr:00012b50 regir:00006700
 adr:00007bec nbr:00012b50 regir:0000082d
 -------
 ----- cpu 1 -----
 adr:00001182 nbr:003f83cf regir:00004a6d
 adr:00001186 nbr:003f83ce regir:00006cfa
 adr:00001066 nbr:000645c7 regir:00006cf8
 adr:00001060 nbr:000645c7 regir:00000c6d
 adr:0000208e nbr:0004e200 regir:00003100
 adr:0000208c nbr:0004e200 regir:00004840
 adr:0000208a nbr:0004e200 regir:0000e980
 adr:00002088 nbr:0004e200 regir:00002003
 adr:00002090 nbr:0004e200 regir:000051c9
 adr:00002086 nbr:0004e200 regir:0000d682
 adr:00001342 nbr:0004ddae regir:00006f00
 adr:00001340 nbr:0004ddae regir:00005343
 -------

free inst slot:00000008
free inst slot:00000009
free inst slot:0000000a
free inst slot:0000000b
*/
void krb_outrun_m68k_op_tst_16_al(M68KOPT_PARAMS);
void krb_outrun_m68k_op_tst_16_di(M68KOPT_PARAMS);

//{
//	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);

//	FLAG_N = NFLAG_16(res);
//	FLAG_Z = res;
//	FLAG_V = VFLAG_CLEAR;
//	FLAG_C = CFLAG_CLEAR;
//}

static void krb_outrun_patch_cpu_synchro()
{
	UINT16 *pcodemain = (UINT16 *)memory_region(REGION_CPU1);
    if(pcodemain[0x00007dda>>1] == 0x4a79) // inst. 0x4a79 triggers m68k_op_tst_16_al()
    {   // replace that tst.w call by a patch, on an unused opcode.
        m68ki_instruction_jump_table[8] = krb_outrun_m68k_op_tst_16_al;
        pcodemain[0x00007dda>>1] = 8;
        //printf("orp1 done\n");
    }

	UINT16 *pcodesub = (UINT16 *)memory_region(REGION_CPU2);
    if(pcodesub[0x00001182>>1] == 0x4a6d) // inst. 0x4a6d  void m68k_op_tst_16_di(M68KOPT_PARAMS)
    {
        m68ki_instruction_jump_table[9] = krb_outrun_m68k_op_tst_16_di; // &krb_outrun_m68k_op_tst_16_di;
        pcodesub[0x00001182>>1] = 9;
        //printf("orp2 done\n");
    }

}
static void krb_shangon_patch_cpu_synchro()
{
	UINT16 *pcodemain = (UINT16 *)memory_region(REGION_CPU1);
    if(pcodemain[0x0000149c>>1] == 0x4a79) // inst. 0x4a79 triggers m68k_op_tst_16_al()
    {   // replace that tst.w call by a patch, on an unused opcode.
        m68ki_instruction_jump_table[8] = krb_outrun_m68k_op_tst_16_al;
        pcodemain[0x0000149c>>1] = 8;
    }

	 UINT16 *pcodesub = (UINT16 *)memory_region(REGION_CPU2);
     if(pcodesub[0x000010ca>>1] == 0x4a79)
     {
         m68ki_instruction_jump_table[8] = krb_outrun_m68k_op_tst_16_al;
         pcodesub[0x000010ca>>1] = 8;
     }

}


static void outrun_generic_init(void)
{
	/* allocate memory for regions not autmatically assigned */
	segaic16_spriteram_0 = auto_malloc(0x01000);
	paletteram16         = auto_malloc(0x02000);
	segaic16_tileram_0   = auto_malloc(0x10000);
	segaic16_textram_0   = auto_malloc(0x01000);
	workram              = auto_malloc(0x08000);

	/* init the memory mapper */
	segaic16_memory_mapper_init(0, outrun_info, sound_w, NULL);

	/* init the FD1094 */
	fd1094_driver_init();

	/* configure the 8255 interface */
	ppi8255_init(&single_ppi_intf);

	/* reset the custom handlers and other pointers */
	custom_io_r = NULL;
	custom_io_w = NULL;
	custom_map = NULL;

}



/*************************************
 *
 *  Initialization & interrupts
 *
 *************************************/

static void update_main_irqs(void)
{
	int irq = 0;

	/* the IRQs are effectively ORed together */
	if (vblank_irq_state)
		irq |= 4;
	if (irq2_state)
		irq |= 2;

	/* assert the lines that are live, or clear everything if nothing is live */
	if (irq != 0)
	{
		cpunum_set_input_line(0, irq, ASSERT_LINE);
		cpu_boost_interleave(0, TIME_IN_USEC(100));
	}
	else
		cpunum_set_input_line(0, 7, CLEAR_LINE);
}


static void irq2_gen(int param)
{
	/* set the IRQ2 line */
	irq2_state = 1;
	update_main_irqs();
}


static void scanline_callback(int scanline)
{
	int next_scanline = scanline;

	/* trigger IRQs on certain scanlines */
	switch (scanline)
	{
		/* IRQ2 triggers on HBLANK of scanlines 65, 129, 193 */
		case 65:
		case 129:
		case 193:
			timer_set(cpu_getscanlineperiod() * 0.9, 0, irq2_gen);
			next_scanline = scanline + 1;
			break;

		/* IRQ2 turns off at the start of scanlines 66, 130, 194 */
		case 66:
		case 130:
		case 194:
			irq2_state = 0;
			next_scanline = (scanline == 194) ? 223 : (scanline + 63);
			break;

		/* VBLANK triggers on scanline 223 */
		case 223:
			vblank_irq_state = 1;
			next_scanline = scanline + 1;
			cpunum_set_input_line(1, 4, ASSERT_LINE);
			break;

		/* VBLANK turns off at the start of scanline 224 */
		case 224:
			vblank_irq_state = 0;
			next_scanline = 65;
			cpunum_set_input_line(1, 4, CLEAR_LINE);
			break;
	}

	/* update IRQs on the main CPU */
	update_main_irqs();

	/* come back at the next targeted scanline */
	mame_timer_set(cpu_getscanlinetime_mt(next_scanline), next_scanline, scanline_callback);
}



/*************************************
 *
 *  Basic machine setup
 *
 *************************************/

static void outrun_reset(void)
{
	cpunum_set_input_line(1, INPUT_LINE_RESET, PULSE_LINE);
	//krb test, copy from xbd
	cpu_boost_interleave(0, TIME_IN_USEC(100));
}


static MACHINE_RESET( outrun )
{
	fd1094_machine_init();

	/* reset misc components */
	segaic16_memory_mapper_reset();
	if (custom_map)
		segaic16_memory_mapper_config(custom_map);
	segaic16_tilemap_reset();

	/* hook the RESET line, which resets CPU #1 */
	cpunum_set_info_fct(0, CPUINFO_PTR_M68K_RESET_CALLBACK, (genf *)outrun_reset);

	/* start timers to track interrupts */
	mame_timer_set(cpu_getscanlinetime_mt(223), 223, scanline_callback);
}



/*************************************
 *
 *  8255 handlers
 *
 *************************************/

static WRITE8_HANDLER( unknown_porta_w )
{
//	loginfo(2,"8255 port A = %02X\n", data);
}


static WRITE8_HANDLER( unknown_portb_w )
{
//	loginfo(2,"8255 port B = %02X\n", data);
}


static WRITE8_HANDLER( video_control_w )
{
	/* Output port:
        D7: SG1 -- connects to sprite chip
        D6: SG0 -- connects to mixing
        D5: Screen display (1= blanked, 0= displayed)
        D4-D2: (ADC2-0)
        D1: (CONT) - affects sprite hardware
        D0: Sound section reset (1= normal operation, 0= reset)
    */
	segaic16_set_display_enable(data & 0x20);
	adc_select = (data >> 2) & 7;
	cpunum_set_input_line(2, INPUT_LINE_RESET, (data & 0x01) ? CLEAR_LINE : ASSERT_LINE);
}



/*************************************
 *
 *  I/O space
 *
 *************************************/

static READ16_HANDLER( misc_io_r )
{
	if (custom_io_r)
		return custom_io_r(offset, mem_mask);
	//loginfo(2,"%06X:misc_io_r - unknown read access to address %04X\n", activecpu_get_pc(), offset * 2);
	return segaic16_open_bus_r(0,0);
}


static WRITE16_HANDLER( misc_io_w )
{
	if (custom_io_w)
	{
		custom_io_w(offset, data, mem_mask);
		return;
	}
//	loginfo(2,"%06X:misc_io_w - unknown write access to address %04X = %04X & %04X\n", activecpu_get_pc(), offset * 2, data, mem_mask ^ 0xffff);
}
//krb: remember analog values that were tested this frame.
// 0,1,2 are steering, gas, brakes.
/*

static READ16_HANDLER( outrun_custom_io_r )
{
	offset &= 0x7f/2;
	switch (offset & 0x70/2)
	{
		case 0x00/2:
			return ppi8255_0_r(offset & 3);

		case 0x10/2:
			return readinputport(offset & 3);

		case 0x30/2:
		{
			static const char *ports[] = { "ADC0", "ADC1", "ADC2", "ADC3", "ADC4", "ADC5", "ADC6", "ADC7" };
			return readinputportbytag_safe(ports[adc_select], 0x0010);
		}

		case 0x60/2:
			return watchdog_reset_r(0);
	}

	logerror("%06X:outrun_custom_io_r - unknown read access to address %04X\n", activecpu_get_pc(), offset * 2);
	return segaic16_open_bus_r(0,0);
}

*/
// int levervt[4]={0};

static READ16_HANDLER( outrun_custom_io_r )
{
	offset &= 0x7f/2;
	switch (offset & 0x70/2)
	{
		case 0x00/2:
			return ppi8255_0_r(offset & 3);

		case 0x10/2:
		{
            UINT16 lv = readinputport(offset & 3);
            if((offset & 3) == 0)
            {
                // note: this is read all along game
                commonControlsValues._lever = (int) (lv & 16); // this is the bit for the gear position
               // commonControlsValues._leverCount++;
            }
			return lv;
        }
		case 0x30/2:
		{
            // krb note:
            // adc_select=0 steering wheel, 2 -> 127(center) ->254
            // pressing left right arrow then get back to middle, using mouse let at last pos.
            // ADC1: brake. bt b -> 2 to 254 in like 0.4 sec
			static const char *ports[] = { "ADC0", "ADC1", "ADC2", "ADC3", "ADC4", "ADC5", "ADC6", "ADC7" };
			UINT16 vread = readinputportbytag_safe(ports[adc_select], 0x0010);
			if(adc_select == 0)
            {
                commonControlsValues.analogValues[0] = (INT16) vread;
               // commonControlsValues.analogValuesReadCount[0]++;
            }
			return vread;
		}

		case 0x60/2:
			return watchdog_reset_r(0);
	}

//	loginfo(2,"%06X:outrun_custom_io_r - unknown read access to address %04X\n", activecpu_get_pc(), offset * 2);
	return segaic16_open_bus_r(0,0);

	// offset &= 0x7f/2;
	// switch (offset & 0x70/2)
	// {
	// 	case 0x00/2:
	// 		return ppi8255_0_r(offset & 3);

	// 	case 0x10/2:
	// 		return readinputport(offset & 3);

	// 	case 0x30/2:
	// 	{
	// 		static const char *ports[] = { "ADC0", "ADC1", "ADC2", "ADC3", "ADC4", "ADC5", "ADC6", "ADC7" };
	// 		return readinputportbytag_safe(ports[adc_select], 0x0010);
	// 	}

	// 	case 0x60/2:
	// 		return watchdog_reset_r(0);
	// }

	// return segaic16_open_bus_r(0,0);
}

extern void segaic16_sprites_draw_0_w_fast();
static WRITE16_HANDLER( outrun_custom_io_w )
{

	switch (offset & 0x70/2)
	{
		case 0x00/2:
			if (ACCESSING_LSB)
			{
                offset &= 0x7f/2;
				ppi8255_0_w(offset & 3, data);
            }
			return;

		case 0x20/2:
			if (ACCESSING_LSB)
			{
				/* Output port:
                    D7: /MUTE
                    D6-D0: unknown
                */
				sound_global_enable(data & 0x80);
			}
			return;

		case 0x30/2:
			/* ADC trigger */
			return;

		case 0x60/2:
			watchdog_reset_w(0,0);
			return;

		case 0x70/2:
		{
    		//not used in that case: offset &= 0x7f/2;
    		//optim this horror -> not much, copy 2kb each 2 frames.
			//test
			//segaic16_sprites_draw_0_w(offset, data, mem_mask);

             segaic16_sprites_draw_0_w_fast();

		}
			return;
	}
//	loginfo(2,"%06X:misc_io_w - unknown write access to address %04X = %04X & %04X\n", activecpu_get_pc(), offset * 2, data, mem_mask ^ 0xffff);
}


static READ16_HANDLER( shangon_custom_io_r )
{
	offset &= 0x303f/2;
	switch (offset)
	{
		case 0x1000/2:
		case 0x1002/2:
		case 0x1004/2:
		case 0x1006/2:
			return readinputport(offset & 3);

		case 0x3020/2:
		{
			static const char *ports[] = { "ADC0", "ADC1", "ADC2", "ADC3" };
			UINT16 vread = readinputportbytag_safe(ports[adc_select], 0x0010);
			if(adc_select == 0)
            {
                commonControlsValues.analogValues[0] = 256-(INT16) vread;
               // commonControlsValues.analogValuesReadCount[0]++;
            }
			return vread;

		}
	}
//	loginfo(2,"%06X:misc_io_r - unknown read access to address %04X\n", activecpu_get_pc(), offset * 2);
	return segaic16_open_bus_r(0,0);
}


static WRITE16_HANDLER( shangon_custom_io_w )
{
	offset &= 0x303f/2;
	switch (offset)
	{
		case 0x0000/2:
			/* Output port:
                D7-D6: (ADC1-0)
                D5: Screen display
            */
			adc_select = (data >> 6) & 3;
			segaic16_set_display_enable((data >> 5) & 1);
			return;

		case 0x0020/2:
			/* Output port:
                D0: Sound section reset (1= normal operation, 0= reset)
            */
			cpunum_set_input_line(2, INPUT_LINE_RESET, (data & 1) ? CLEAR_LINE : ASSERT_LINE);
			return;

		case 0x3000/2:
			watchdog_reset_w(0,0);
			return;

		case 0x3020/2:
			/* ADC trigger */
			return;
	}
//	loginfo(2,"%06X:misc_io_w - unknown write access to address %04X = %04X & %04X\n", activecpu_get_pc(), offset * 2, data, mem_mask ^ 0xffff);
}



/*************************************
 *
 *  Capacitor-backed RAM
 *
 *************************************/

static NVRAM_HANDLER( outrun )
{
	if (read_or_write)
		mame_fwrite(file, workram, 0x8000);
	else if (file)
		mame_fread(file, workram, 0x8000);
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( outrun_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x000000, 0xffffff) AM_READWRITE(segaic16_memory_mapper_lsb_r, segaic16_memory_mapper_lsb_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Second CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( sub_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) | AMEF_ABITS(20) )
	AM_RANGE(0x000000, 0x05ffff) AM_ROM AM_BASE(&cpu1rom)
	AM_RANGE(0x060000, 0x067fff) AM_MIRROR(0x018000) AM_RAM AM_BASE(&cpu1ram)
	AM_RANGE(0x080000, 0x080fff) AM_MIRROR(0x00f000) AM_RAM AM_BASE(&segaic16_roadram_0)
	AM_RANGE(0x090000, 0x09ffff) AM_READWRITE(segaic16_road_control_0_r, segaic16_road_control_0_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Sound CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x0000, 0xefff) AM_ROM
	AM_RANGE(0xf000, 0xf0ff) AM_MIRROR(0x0700) AM_READWRITE(SegaPCM_r, SegaPCM_w)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) | AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_MIRROR(0x3e) AM_WRITE(YM2151_register_port_0_w)
	AM_RANGE(0x01, 0x01) AM_MIRROR(0x3e) AM_READWRITE(YM2151_status_port_0_r, YM2151_data_port_0_w)
	AM_RANGE(0x40, 0x40) AM_MIRROR(0x3f) AM_READ(soundlatch_r)
ADDRESS_MAP_END



/*************************************
 *
 *  Generic port definitions
 *
 *************************************/

static INPUT_PORTS_START( outrun_generic )
	PORT_START_TAG("SERVICE")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_SERVICE_NO_TOGGLE( 0x02, IP_ACTIVE_LOW )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START_TAG("UNKNOWN")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("COINAGE")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x05, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x04, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x02, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x03, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin B too) or 1/1" )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x50, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x40, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x10, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x20, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x30, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin A too) or 1/1" )

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END



/*************************************
 *
 *  Game-specific port definitions
 *
 *************************************/

static INPUT_PORTS_START( outrun )
	PORT_INCLUDE( outrun_generic )

	PORT_MODIFY("SERVICE")
	// OH ITS GEAR SHIFT !!
//	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_TOGGLE PORT_CODE(KEYCODE_SPACE)
// todo have  PORT_CODE(KEYCODE_SPACE) back and test it
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_TOGGLE PORT_NAME(ui_getstring(UI_GearShift))
	// PORT_NAME("Gear Shift")

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x02, "Up Cockpit" )
	PORT_DIPSETTING(    0x01, "Mini Up" )
	PORT_DIPSETTING(    0x03, "Moving" )
//  PORT_DIPSETTING(    0x00, "No Use" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, "Time" )
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xc0, 0xc0, "Enemies" )
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START_TAG("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x7f, IPT_PADDLE ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)
    //PORT_NAME("Steering Wheel")
    PORT_NAME(ui_getstring(UI_SteeringWheel))

	PORT_START_TAG("ADC1")	/* gas pedal */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_START_TAG("ADC2")	/* brake */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(40)
INPUT_PORTS_END


static INPUT_PORTS_START( outrundx )
	PORT_INCLUDE( outrun )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, "Not Moving" )
	PORT_DIPSETTING(    0x01, "Moving" )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, "Time" )
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xc0, 0xc0, "Enemies" )
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START_TAG("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x7f, IPT_PADDLE ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)

	PORT_START_TAG("ADC1")	/* gas pedal */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_START_TAG("ADC2")	/* brake */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(40)
INPUT_PORTS_END


static INPUT_PORTS_START( toutrun )
	PORT_INCLUDE( outrun_generic )

	PORT_MODIFY("SERVICE")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_TOGGLE PORT_NAME(ui_getstring(UI_GearShift))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME(ui_getstring(UI_Turbo))

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x02, "Cockpit Conversion" )
	PORT_DIPSETTING(    0x01, "Mini Up" )
	PORT_DIPSETTING(    0x03, "Moving" )
	PORT_DIPSETTING(    0x00, "Cockpit" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Turbo" )
	PORT_DIPSETTING(    0x00, "Start Button" )
	PORT_DIPSETTING(    0x08, "Use Turbo Shifter" )
	PORT_DIPNAME( 0x30, 0x10, "Credits" )
	PORT_DIPSETTING(    0x20, "3 to Start/2 to Continue" )
	PORT_DIPSETTING(    0x30, "2 to Start/1 to Continue" )
	PORT_DIPSETTING(    0x10, "1 to Start/1 to Continue" )
	PORT_DIPSETTING(    0x00, "2 to Start/2 to Continue" )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START_TAG("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x7f, IPT_PADDLE ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)

	PORT_START_TAG("ADC1")	/* gas pedal */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_START_TAG("ADC2")	/* brake */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(40)
INPUT_PORTS_END


static INPUT_PORTS_START( shangon )
	PORT_INCLUDE( outrun_generic )

	PORT_MODIFY("SERVICE")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("UNKNOWN")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE( 0x04, IP_ACTIVE_LOW )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON3 )	/* S.C. SW? */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x06, 0x06, "Time" )
	PORT_DIPSETTING(    0x04, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x06, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x18, 0x18, "Enemies" )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x18, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START_TAG("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x7f, IPT_PADDLE ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_REVERSE

	PORT_START_TAG("ADC1")	/* gas pedal */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_START_TAG("ADC2")	/* brake */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(40)
INPUT_PORTS_END



/*************************************
 *
 *  Sound definitions
 *
 *************************************/

static struct SEGAPCMinterface segapcm_interface =
{
	BANK_512,
	REGION_SOUND1
};



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,	0, 1024 },
	{ -1 }
};



/*************************************
 *
 *  Generic machine drivers
 *
 *************************************/

static MACHINE_DRIVER_START( outrun )

	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", M68000, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(outrun_map,0)

	MDRV_CPU_ADD_TAG("sub", M68000, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(sub_map,0)

	MDRV_CPU_ADD_TAG("sound", Z80, SOUND_CLOCK/4)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_map,0)
	MDRV_CPU_IO_MAP(sound_portmap,0)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(1000000 * (262 - 224) / (262 * 60))

	MDRV_MACHINE_RESET(outrun)
	MDRV_NVRAM_HANDLER(outrun)
	MDRV_INTERLEAVE(100)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(40*8, 28*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 0*8, 28*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4096*3)

	MDRV_VIDEO_START(outrun)
	MDRV_VIDEO_UPDATE(outrun)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM2151, SOUND_CLOCK/4)
	MDRV_SOUND_ROUTE(0, "left", 0.43)
	MDRV_SOUND_ROUTE(1, "right", 0.43)

	MDRV_SOUND_ADD_TAG("pcm", SEGAPCM, SOUND_CLOCK/4)
	MDRV_SOUND_CONFIG(segapcm_interface)
	MDRV_SOUND_ROUTE(0, "left", 1.0)
	MDRV_SOUND_ROUTE(1, "right", 1.0)
MACHINE_DRIVER_END


MACHINE_DRIVER_START( shangon )
	MDRV_IMPORT_FROM(outrun)
	MDRV_VIDEO_START(shangon)
	MDRV_VIDEO_UPDATE(shangon)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Outrun
    CPU: 68000
*/
ROM_START( outrun )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr10380b.133", 0x000000, 0x10000, CRC(1f6cadad) SHA1(31e870f307f44eb4f293b607123b623beee2bc3c) )
	ROM_LOAD16_BYTE( "epr10382b.118", 0x000001, 0x10000, CRC(c4c3fa1a) SHA1(69236cf9f27691dee290c79db1fc9b5e73ea77d7) )
	ROM_LOAD16_BYTE( "epr10381a.132", 0x020000, 0x10000, CRC(be8c412b) SHA1(bf3ff05bbf81bdd44567f3b9bb4919ed4a499624) )
	ROM_LOAD16_BYTE( "epr10383b.117", 0x020001, 0x10000, CRC(10a2014a) SHA1(1970895145ad8b5735f66ed8c837d9d453ce9b23) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr10327a.76", 0x00000, 0x10000, CRC(e28a5baf) SHA1(f715bde96c73ed47035acf5a41630fdeb41bb2f9) )
	ROM_LOAD16_BYTE( "epr10329a.58", 0x00001, 0x10000, CRC(da131c81) SHA1(57d5219bd0e2fd886217e37e8773fd76be9b40eb) )
	ROM_LOAD16_BYTE( "epr10328a.75", 0x20000, 0x10000, CRC(d5ec5e5d) SHA1(a4e3cfca4d803e72bc4fcf91ab00e21bf3f8959f) )
	ROM_LOAD16_BYTE( "epr10330a.57", 0x20001, 0x10000, CRC(ba9ec82a) SHA1(2136c9572e26b7ae6de402c0cd53174407cc6018) )

	ROM_REGION( 0x30000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr10268.99",  0x00000, 0x08000, CRC(95344b04) SHA1(b3480714b11fc49b449660431f85d4ba92f799ba) )
	ROM_LOAD( "opr10232.102", 0x08000, 0x08000, CRC(776ba1eb) SHA1(e3477961d19e694c97643066534a1f720e0c4327) )
	ROM_LOAD( "opr10267.100", 0x10000, 0x08000, CRC(a85bb823) SHA1(a7e0143dee5a47e679fd5155e58e717813912692) )
	ROM_LOAD( "opr10231.103", 0x18000, 0x08000, CRC(8908bcbf) SHA1(8e1237b640a6f26bdcbfd5e201dadb2687c4febb) )
	ROM_LOAD( "opr10266.101", 0x20000, 0x08000, CRC(9f6f1a74) SHA1(09164e858ebeedcff4d389524ddf89e7c216dcae) )
	ROM_LOAD( "opr10230.104", 0x28000, 0x08000, CRC(686f5e50) SHA1(03697b892f911177968aa40de6c5f464eb0258e7) )

	ROM_REGION32_LE( 0x100000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr10371.9",  0x00000, 0x20000, CRC(7cc86208) SHA1(21320f945f7c8e990c97c9b1232a0f4b6bd00f8f) )
	ROM_LOAD32_BYTE( "mpr10373.10", 0x00001, 0x20000, CRC(b0d26ac9) SHA1(3a9ce8547cd43b7b04abddf9a9ab5634e0bbfaba) )
	ROM_LOAD32_BYTE( "mpr10375.11", 0x00002, 0x20000, CRC(59b60bd7) SHA1(e5d8c67e020608edd24ba87b7687b2ac2483ee7f) )
	ROM_LOAD32_BYTE( "mpr10377.12", 0x00003, 0x20000, CRC(17a1b04a) SHA1(9f7210cb4153ac9029a785dcd4b45f4513a4b008) )
	ROM_LOAD32_BYTE( "mpr10372.13", 0x80000, 0x20000, CRC(b557078c) SHA1(a3746a2da077a8df4932348f650a061f413e8430) )
	ROM_LOAD32_BYTE( "mpr10374.14", 0x80001, 0x20000, CRC(8051e517) SHA1(9c8509fbed170b4ac74c169da573393e54774f49) )
	ROM_LOAD32_BYTE( "mpr10376.15", 0x80002, 0x20000, CRC(f3b8f318) SHA1(a5f2532613f33a64441e0f75443c10ba78dccc6e) )
	ROM_LOAD32_BYTE( "mpr10378.16", 0x80003, 0x20000, CRC(a1062984) SHA1(4399030a155caf71f2dec7f75c4b65531ab53576) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx (2 identical roms, 1 for each road?) */
	ROM_LOAD( "opr10185.11", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )
	ROM_LOAD( "opr10186.47", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "epr10187.88",       0x00000, 0x8000, CRC(a10abaa9) SHA1(01c8a819587a66d2ee4d255656e36fa0904377b0) )

	ROM_REGION( 0x60000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "opr10193.66",       0x00000, 0x8000, CRC(bcd10dde) SHA1(417ce1d7242884640c5b14f4db8ee57cde7d085d) )
	ROM_LOAD( "opr10192.67",       0x10000, 0x8000, CRC(770f1270) SHA1(686bdf44d45c1d6002622f6658f037735382f3e0) )
	ROM_LOAD( "opr10191.68",       0x20000, 0x8000, CRC(20a284ab) SHA1(7c9027416d4122791ba53782fe2230cf02b7d506) )
	ROM_LOAD( "opr10190.69",       0x30000, 0x8000, CRC(7cab70e2) SHA1(a3c581d2b438630d0d4c39481dcfd85681c9f889) )
	ROM_LOAD( "opr10189.70",       0x40000, 0x8000, CRC(01366b54) SHA1(f467a6b807694d5832a985f5381c170d24aaee4e) )
	ROM_LOAD( "opr10188.71",       0x50000, 0x8000, CRC(bad30ad9) SHA1(f70dd3a6362c314adef313b064102f7a250401c8) )
ROM_END

/**************************************************************************************************************************
    Outrun
    CPU: 68000
*/
ROM_START( outrun2 )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr10380a.133", 0x000000, 0x10000, CRC(434fadbc) SHA1(83c861d331e69ef4f2452c313ae4b5ea9d8b7948) )
	ROM_LOAD16_BYTE( "epr10382a.118", 0x000001, 0x10000, CRC(1ddcc04e) SHA1(945d207d8d602d7fdb6d25f6b93c9c0b995e8d5a) )
	ROM_LOAD16_BYTE( "epr10381a.132", 0x020000, 0x10000, CRC(be8c412b) SHA1(bf3ff05bbf81bdd44567f3b9bb4919ed4a499624) )
	ROM_LOAD16_BYTE( "epr10383a.117", 0x020001, 0x10000, CRC(dcc586e7) SHA1(d6e1de6b562359574d94b88ce6101646c506e701) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr10327a.76", 0x00000, 0x10000, CRC(e28a5baf) SHA1(f715bde96c73ed47035acf5a41630fdeb41bb2f9) )
	ROM_LOAD16_BYTE( "epr10329a.58", 0x00001, 0x10000, CRC(da131c81) SHA1(57d5219bd0e2fd886217e37e8773fd76be9b40eb) )
	ROM_LOAD16_BYTE( "epr10328a.75", 0x20000, 0x10000, CRC(d5ec5e5d) SHA1(a4e3cfca4d803e72bc4fcf91ab00e21bf3f8959f) )
	ROM_LOAD16_BYTE( "epr10330a.57", 0x20001, 0x10000, CRC(ba9ec82a) SHA1(2136c9572e26b7ae6de402c0cd53174407cc6018) )

	ROM_REGION( 0x30000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr10268.99",  0x00000, 0x08000, CRC(95344b04) SHA1(b3480714b11fc49b449660431f85d4ba92f799ba) )
	ROM_LOAD( "opr10232.102", 0x08000, 0x08000, CRC(776ba1eb) SHA1(e3477961d19e694c97643066534a1f720e0c4327) )
	ROM_LOAD( "opr10267.100", 0x10000, 0x08000, CRC(a85bb823) SHA1(a7e0143dee5a47e679fd5155e58e717813912692) )
	ROM_LOAD( "opr10231.103", 0x18000, 0x08000, CRC(8908bcbf) SHA1(8e1237b640a6f26bdcbfd5e201dadb2687c4febb) )
	ROM_LOAD( "opr10266.101", 0x20000, 0x08000, CRC(9f6f1a74) SHA1(09164e858ebeedcff4d389524ddf89e7c216dcae) )
	ROM_LOAD( "opr10230.104", 0x28000, 0x08000, CRC(686f5e50) SHA1(03697b892f911177968aa40de6c5f464eb0258e7) )

	ROM_REGION32_LE( 0x100000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr10371.9",  0x00000, 0x20000, CRC(7cc86208) SHA1(21320f945f7c8e990c97c9b1232a0f4b6bd00f8f) )
	ROM_LOAD32_BYTE( "mpr10373.10", 0x00001, 0x20000, CRC(b0d26ac9) SHA1(3a9ce8547cd43b7b04abddf9a9ab5634e0bbfaba) )
	ROM_LOAD32_BYTE( "mpr10375.11", 0x00002, 0x20000, CRC(59b60bd7) SHA1(e5d8c67e020608edd24ba87b7687b2ac2483ee7f) )
	ROM_LOAD32_BYTE( "mpr10377.12", 0x00003, 0x20000, CRC(17a1b04a) SHA1(9f7210cb4153ac9029a785dcd4b45f4513a4b008) )
	ROM_LOAD32_BYTE( "mpr10372.13", 0x80000, 0x20000, CRC(b557078c) SHA1(a3746a2da077a8df4932348f650a061f413e8430) )
	ROM_LOAD32_BYTE( "mpr10374.14", 0x80001, 0x20000, CRC(8051e517) SHA1(9c8509fbed170b4ac74c169da573393e54774f49) )
	ROM_LOAD32_BYTE( "mpr10376.15", 0x80002, 0x20000, CRC(f3b8f318) SHA1(a5f2532613f33a64441e0f75443c10ba78dccc6e) )
	ROM_LOAD32_BYTE( "mpr10378.16", 0x80003, 0x20000, CRC(a1062984) SHA1(4399030a155caf71f2dec7f75c4b65531ab53576) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx (2 identical roms, 1 for each road?) */
	ROM_LOAD( "opr10185.11", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )
	ROM_LOAD( "opr10186.47", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "epr10187.88",       0x00000, 0x8000, CRC(a10abaa9) SHA1(01c8a819587a66d2ee4d255656e36fa0904377b0) )

	ROM_REGION( 0x60000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "opr10193.66",       0x00000, 0x8000, CRC(bcd10dde) SHA1(417ce1d7242884640c5b14f4db8ee57cde7d085d) )
	ROM_LOAD( "opr10192.67",       0x10000, 0x8000, CRC(770f1270) SHA1(686bdf44d45c1d6002622f6658f037735382f3e0) )
	ROM_LOAD( "opr10191.68",       0x20000, 0x8000, CRC(20a284ab) SHA1(7c9027416d4122791ba53782fe2230cf02b7d506) )
	ROM_LOAD( "opr10190.69",       0x30000, 0x8000, CRC(7cab70e2) SHA1(a3c581d2b438630d0d4c39481dcfd85681c9f889) )
	ROM_LOAD( "opr10189.70",       0x40000, 0x8000, CRC(01366b54) SHA1(f467a6b807694d5832a985f5381c170d24aaee4e) )
	ROM_LOAD( "opr10188.71",       0x50000, 0x8000, CRC(bad30ad9) SHA1(f70dd3a6362c314adef313b064102f7a250401c8) )
ROM_END

/**************************************************************************************************************************
    Outrun
    CPU: 68000 (317-????)

    Sega Outrun Japan version
    -------------------------

    CPU Board (837-6063)
    ---------
    EPR10173 - IC66 - 5826
    EPR10174 - IC67 - 1817
    EPR10175 - IC68 - EAE0
    EPR10176 - IC69 - 05F3
    EPR10178 - IC86 - 5494
    EPR10179 - IC87 - E63D
    EPR10180 - IC88 - 14C5
    EPR10181 - IC89 - 999E

    EPR10183 - IC115 - 089E
    EPR10184 - IC116 - 1CD2
    EPR10258 - IC117 - 40FE
    EPR10259 - IC118 - 9CBF
    EPR10261 - IC130 - 7DCE
    EPR10262 - IC131 - 43C1
    EPR10263 - IC132 - 905E
    EPR10264 - IC133 - 8498

    Video Board (834-6065 Revision A)
    -----------
    EPR10194 - IC26 - 8C35
    EPR10195 - IC27 - 4012
    EPR10196 - IC28 - C4D8
    EPR10197 - IC29 - FD47
    EPR10198 - IC30 - BF34
    EPR10199 - IC31 - DD89
    EPR10200 - IC32 - A707
    EPR10201 - IC33 - E288

    EPR10203 - IC38 - 3703
    EPR10204 - IC39 - 861B
    EPR10205 - IC40 - 36C5
    EPR10206 - IC41 - 5F40
    EPR10207 - IC42 - F500
    EPR10208 - IC43 - D932
    EPR10209 - IC44 - D464
    EPR10210 - IC45 - 4D74

    EPR10212 - IC52 - 707D
    EPR10213 - IC53 - 8204
    EPR10214 - IC54 - 79C4
    EPR10215 - IC55 - 0236
    EPR10216 - IC56 - 5738
    EPR10217 - IC57 - E265
    EPR10218 - IC58 - 9571
    EPR10219 - IC59 - A8C9

    EPR10221 - IC66 - 224E
    EPR10222 - IC67 - 4677
    EPR10223 - IC68 - D3BF
    EPR10224 - IC69 - 03A7
    EPR10225 - IC70 - 1215
    EPR10226 - IC71 - C3B8
    EPR10227 - IC72 - 5595
    EPR10228 - IC73 - 934B
*/

ROM_START( outrun1 )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr10183.115", 0x000000, 0x8000, CRC(3d992396) SHA1(8cef43799b71cfd36d3fea140afff7fe0bafcfc1) )
	ROM_LOAD16_BYTE( "epr10261.130", 0x000001, 0x8000, CRC(1d034847) SHA1(664b24c13f7885403328906682213e38c1ad994e) )
	ROM_LOAD16_BYTE( "epr10184.116", 0x010000, 0x8000, CRC(1a73dc46) SHA1(70f31619e80eb3d70747e7006e135c8bc0a31675) )
	ROM_LOAD16_BYTE( "epr10262.131", 0x010001, 0x8000, CRC(5386b6b3) SHA1(a554ed1b4e07811c4accc59c063baa42949b6670) )
	ROM_LOAD16_BYTE( "epr10258.117", 0x020000, 0x8000, CRC(39408e4f) SHA1(4f7f8b393dfb1e1935d595ae55a6913a27b02f80) )
	ROM_LOAD16_BYTE( "epr10263.132", 0x020001, 0x8000, CRC(eda65fd6) SHA1(dd9c072856edffff3e73423f22ab40c5893bd26f) )
	ROM_LOAD16_BYTE( "epr10259.118", 0x030000, 0x8000, CRC(95100b1a) SHA1(d2a5eb97623321b6c943bc435de26bf5d39ea312) )
	ROM_LOAD16_BYTE( "epr10264.133", 0x030001, 0x8000, CRC(cc94b102) SHA1(29dc7e2a8509d0b5d30e2fb9404e0517b97f64e8) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr10173.66", 0x000000, 0x8000, CRC(6c2775c0) SHA1(2dd3a4e7f7b8808da74fbd53423a83775afff5d5) )
	ROM_LOAD16_BYTE( "epr10178.86", 0x000001, 0x8000, CRC(6d36be05) SHA1(02527701451bbdfa14280ef4db6f4d540e6ee470) )
	ROM_LOAD16_BYTE( "epr10174.67", 0x010000, 0x8000, CRC(aae7efad) SHA1(bbc68daafc8bb61d0b065baa3a3583e95de4d9ad) )
	ROM_LOAD16_BYTE( "epr10179.87", 0x010001, 0x8000, CRC(180fd041) SHA1(87f1566cff1bded7642e260b8337a278052727bb) )
	ROM_LOAD16_BYTE( "epr10175.68", 0x020000, 0x8000, CRC(31c76063) SHA1(a3069c5443e7f87c38a69530f00ccc6e9a8eac42) )
	ROM_LOAD16_BYTE( "epr10180.88", 0x020001, 0x8000, CRC(4713b264) SHA1(ab498b5232520657bae841927ee74994a6fb1c4e) )
	ROM_LOAD16_BYTE( "epr10176.69", 0x030000, 0x8000, CRC(a7811f90) SHA1(a2ac49f0947ddddbbdaa90ebdefd232fdbf27c35) )
	ROM_LOAD16_BYTE( "epr10181.89", 0x030001, 0x8000, CRC(e009a04d) SHA1(f3253a0feb6acd08238e025e7ab8b5cb175d1c67) )

	ROM_REGION( 0x30000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr10268.99", 0x00000, 0x08000, CRC(95344b04) SHA1(b3480714b11fc49b449660431f85d4ba92f799ba) )
	ROM_LOAD( "opr10232.102", 0x08000, 0x08000, CRC(776ba1eb) SHA1(e3477961d19e694c97643066534a1f720e0c4327) )
	ROM_LOAD( "opr10267.100", 0x10000, 0x08000, CRC(a85bb823) SHA1(a7e0143dee5a47e679fd5155e58e717813912692) )
	ROM_LOAD( "opr10231.103", 0x18000, 0x08000, CRC(8908bcbf) SHA1(8e1237b640a6f26bdcbfd5e201dadb2687c4febb) )
	ROM_LOAD( "opr10266.101", 0x20000, 0x08000, CRC(9f6f1a74) SHA1(09164e858ebeedcff4d389524ddf89e7c216dcae) )
	ROM_LOAD( "opr10230.104", 0x28000, 0x08000, CRC(686f5e50) SHA1(03697b892f911177968aa40de6c5f464eb0258e7) )

	ROM_REGION32_LE( 0x100000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD32_BYTE( "epr10194.26", 0x00000, 0x08000, CRC(f0eda3bd) SHA1(173e10a10372d42da81e6eb48c3e23a117638c0c) )
	ROM_LOAD32_BYTE( "epr10203.38", 0x00001, 0x08000, CRC(8445a622) SHA1(1187dee7db09a42446fc75872d49936310141eb8) )
	ROM_LOAD32_BYTE( "epr10212.52", 0x00002, 0x08000, CRC(dee7e731) SHA1(f09d18f8d8405025b87dd01488ad2098e28410b0) )
	ROM_LOAD32_BYTE( "epr10221.66", 0x00003, 0x08000, CRC(43431387) SHA1(a28896e888bc4d4f67babd49003d663c1ceabb71) )
	ROM_LOAD32_BYTE( "epr10195.27", 0x20000, 0x08000, CRC(0de75cdd) SHA1(a97faea76aca663ccbbde327f3d1d8ae256649d3) )
	ROM_LOAD32_BYTE( "epr10204.39", 0x20001, 0x08000, CRC(5f4b5abb) SHA1(f81637b2eb6a4bde76c43eedfad7e5375594c7bd) )
	ROM_LOAD32_BYTE( "epr10213.53", 0x20002, 0x08000, CRC(1d1b22f0) SHA1(d3b1c36d08c4b7b08f9969a521e62eebd5b2238d) )
	ROM_LOAD32_BYTE( "epr10222.67", 0x20003, 0x08000, CRC(a254c706) SHA1(e2801a0a7fd5546a48cd53ad7e4743d821d985ff) )
	ROM_LOAD32_BYTE( "epr10196.28", 0x40000, 0x08000, CRC(8688bb59) SHA1(0aaa90c5101aa1db00db776a15a0a525587dfc43) )
	ROM_LOAD32_BYTE( "epr10205.40", 0x40001, 0x08000, CRC(74bd93ca) SHA1(6a02ea3b977e56cfd61302afa2abf6c2dc766ba7) )
	ROM_LOAD32_BYTE( "epr10214.54", 0x40002, 0x08000, CRC(57527e18) SHA1(4cc95c4b741f495e5b9c3b9d4d9ab9a6fded9aeb) )
	ROM_LOAD32_BYTE( "epr10223.68", 0x40003, 0x08000, CRC(3850690e) SHA1(0f92743f848edc8deaeeef3afca5f662ceba61e7) )
	ROM_LOAD32_BYTE( "epr10197.29", 0x60000, 0x08000, CRC(009165a6) SHA1(987b91e8c5c54bb7c4520b13a72f1f47c34278f4) )
	ROM_LOAD32_BYTE( "epr10206.41", 0x60001, 0x08000, CRC(954542c5) SHA1(3c67e3568c04ba083f4aacad2e8857cdd16b3b2f) )
	ROM_LOAD32_BYTE( "epr10215.55", 0x60002, 0x08000, CRC(69be5a6c) SHA1(2daac5877a71de04878f231f03361f697552431f) )
	ROM_LOAD32_BYTE( "epr10224.69", 0x60003, 0x08000, CRC(5cffc346) SHA1(0481f864bb584c96cd92c260a62c0c1d4030bde8) )
	ROM_LOAD32_BYTE( "epr10198.30", 0x80000, 0x08000, CRC(d894992e) SHA1(451469f743a0019b8797d16ba7b26a267d13fe06) )
	ROM_LOAD32_BYTE( "epr10207.42", 0x80001, 0x08000, CRC(ca61cea4) SHA1(7c39e2863f5c7be290522acdaf046b1dab7a3542) )
	ROM_LOAD32_BYTE( "epr10216.56", 0x80002, 0x08000, CRC(d394134d) SHA1(42f768a9c9eb9f556d197548c35b3a0cd5414734) )
	ROM_LOAD32_BYTE( "epr10225.70", 0x80003, 0x08000, CRC(0a5d1f2b) SHA1(43d9c7539b6cebbac3395a4ba71a702300c9e644) )
	ROM_LOAD32_BYTE( "epr10199.31", 0xa0000, 0x08000, CRC(86376af6) SHA1(971f4b0d9a01ca7ffb50cefbe1ab41b703a4a41a) )
	ROM_LOAD32_BYTE( "epr10208.43", 0xa0001, 0x08000, CRC(6830b7fa) SHA1(3ece1971a4f025104ebd026da6751caea9aa8a64) )
	ROM_LOAD32_BYTE( "epr10217.57", 0xa0002, 0x08000, CRC(bf2c9b76) SHA1(248e273255968115a60855b1fffcce1dbeacc3d4) )
	ROM_LOAD32_BYTE( "epr10226.71", 0xa0003, 0x08000, CRC(5a452474) SHA1(6789a33b55a1693ec9cc196b3ebd220b14169e08) )
	ROM_LOAD32_BYTE( "epr10200.32", 0xc0000, 0x08000, CRC(1e5d4f73) SHA1(79deddf4461dad5784441c2839894207b7d2ecac) )
	ROM_LOAD32_BYTE( "epr10209.44", 0xc0001, 0x08000, CRC(5c15419e) SHA1(7b4e9c0cb430afae7f927c0224021add0a627251) )
	ROM_LOAD32_BYTE( "epr10218.58", 0xc0002, 0x08000, CRC(db4bdb39) SHA1(b4661611b28e7ff1c721565175038cfd1e99d383) )
	ROM_LOAD32_BYTE( "epr10227.72", 0xc0003, 0x08000, CRC(c7def392) SHA1(fa7d1245eefdc3abb9520118bbb0d025ca62901e) )
	ROM_LOAD32_BYTE( "epr10201.33", 0xe0000, 0x08000, CRC(1d9d4b9c) SHA1(3264b66c87aa7de4c140450b96adbe3071231d4a) )
	ROM_LOAD32_BYTE( "epr10210.45", 0xe0001, 0x08000, CRC(39422931) SHA1(8d8a3f4597945c92aebd20c0784180696b6c9c1c) )
	ROM_LOAD32_BYTE( "epr10219.59", 0xe0002, 0x08000, CRC(e73b9224) SHA1(1904a71a0c18ab2a3a5929e72b1c215dbb0fa213) )
	ROM_LOAD32_BYTE( "epr10228.73", 0xe0003, 0x08000, CRC(25803978) SHA1(1a18922aeb516e8deb026d52e3cdcc4e69385af5) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx (2 identical roms, 1 for each road?) */
	ROM_LOAD( "opr10185.11", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )
	ROM_LOAD( "opr10186.47", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "epr10187.88",       0x00000, 0x8000, CRC(a10abaa9) SHA1(01c8a819587a66d2ee4d255656e36fa0904377b0) )

	ROM_REGION( 0x60000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "opr10193.66",       0x00000, 0x8000, CRC(bcd10dde) SHA1(417ce1d7242884640c5b14f4db8ee57cde7d085d) )
	ROM_LOAD( "opr10192.67",       0x10000, 0x8000, CRC(770f1270) SHA1(686bdf44d45c1d6002622f6658f037735382f3e0) )
	ROM_LOAD( "opr10191.68",       0x20000, 0x8000, CRC(20a284ab) SHA1(7c9027416d4122791ba53782fe2230cf02b7d506) )
	ROM_LOAD( "opr10190.69",       0x30000, 0x8000, CRC(7cab70e2) SHA1(a3c581d2b438630d0d4c39481dcfd85681c9f889) )
	ROM_LOAD( "opr10189.70",       0x40000, 0x8000, CRC(01366b54) SHA1(f467a6b807694d5832a985f5381c170d24aaee4e) )
	ROM_LOAD( "opr10188.71",       0x50000, 0x8000, CRC(bad30ad9) SHA1(f70dd3a6362c314adef313b064102f7a250401c8) )
ROM_END

/**************************************************************************************************************************
    Outrun (bootleg)

    Outrun bootleg made by PHILKO

    It is composed of 3 boards.

    The upper board contains:
    2x 68000 cpus
    1x z80 cpu
    1x ym2151
    1x 20mhz osc (near 68k)
    1x 16mhz osc (near z80)
    2x pots
    1x PHILKO custom chip quad package soldered (gfx chip?). it's marked "Philko PK8702 8717".
    eproms from a-1 to a-14

    Mid board contains:
    many TTLs and rams
    1x NEC D8255AC-2 (I/O chip?)
    5x Philko custom chips dip package (little as a ttl). They are all marked "Philko PK8701 8720"
    eproms a-15, a-16 and a-17

    Lower board contains:
    lots of rams
    1x custom Philko chip the size of a z80. it's marked "Philko PK8703"
    1x custom Philko chip the size of a z80. it's marked "Philko PK8704"
    1x custom Philko chip the size of a z80. it's marked "Philko PK8705"
    eproms from a-18 to a-33

    Dumped by Corrado Tomaselli
*/
ROM_START( outrunb )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "a-10.bin", 0x000000, 0x10000, CRC(cddceea2) SHA1(34cb4ca61c941e96e585f3cd2aed79bdde67f8eb) )
	ROM_LOAD16_BYTE( "a-9.bin",  0x000001, 0x10000, CRC(14e97a67) SHA1(a86ccb719ad695ed814bedfe02dbafa435fc65da) )
	ROM_LOAD16_BYTE( "a-14.bin", 0x020000, 0x10000, CRC(3092d857) SHA1(8ebfeab9217b80a7983a4f8eb7bb7d3387d791b3) )
	ROM_LOAD16_BYTE( "a-13.bin", 0x020001, 0x10000, CRC(30a1c496) SHA1(734c82930197e6e8cd2bea145aedda6b3c1145d0) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "a-8.bin",  0x00000, 0x10000, CRC(d7f5aae0) SHA1(0f9b693f078cdbbfeade5a373a94a20110d586ca) )
	ROM_LOAD16_BYTE( "a-7.bin",  0x00001, 0x10000, CRC(88c2e78f) SHA1(198cab9133345e4529f7fb52c29974c9a1a84933) )
	ROM_LOAD16_BYTE( "a-12.bin", 0x20000, 0x10000, CRC(d5ec5e5d) SHA1(a4e3cfca4d803e72bc4fcf91ab00e21bf3f8959f) )
	ROM_LOAD16_BYTE( "a-11.bin", 0x20001, 0x10000, CRC(74c5fbec) SHA1(a44f1477d830fdb4d6c29351da94776843e5d3e1) )

	ROM_REGION( 0x30000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "a-15.bin",	0x00000, 0x10000, CRC(4c489133) SHA1(db97de9d84ca5916e69972ee19ccb4c15fa98bf9) )
	ROM_LOAD( "a-17.bin",	0x10000, 0x10000, CRC(899c781d) SHA1(4f759c316a57a1e42838375525290425d25b53e1) )
	ROM_LOAD( "a-16.bin",	0x20000, 0x10000, CRC(98dd4d15) SHA1(914ebcb330455ab35968b4add4d94be123a185a5) )

	ROM_REGION32_LE( 0x100000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD32_BYTE( "a-18.bin", 	0x00000, 0x10000, CRC(77377e00) SHA1(4f376b05692f33d529f4c058dac989136c808ca1) )
	ROM_LOAD32_BYTE( "a-20.bin", 	0x00001, 0x10000, CRC(69ecc975) SHA1(3560e9a31fc71e263a6ff61224b8db2b17836075) )
	ROM_LOAD32_BYTE( "a-22.bin", 	0x00002, 0x10000, CRC(b6a8d0e2) SHA1(6184700dbe2c8c9c91f220e246501b7a865e4a05) )
	ROM_LOAD32_BYTE( "a-24.bin", 	0x00003, 0x10000, CRC(d632d8a2) SHA1(27ca6faaa073bd01b2be959dba0359f93e8c1ec1) )
	ROM_LOAD32_BYTE( "a-26.bin",	0x40000, 0x10000, CRC(4f784236) SHA1(1fb610fd29d3ddd8c5d4892ae215386b18552e6f) )
	ROM_LOAD32_BYTE( "a-28.bin",	0x40001, 0x10000, CRC(ee4f7154) SHA1(3a84c1b19d9dfcd5310e9cee90c0d4562a4a7786) )
	ROM_LOAD32_BYTE( "a-30.bin",	0x40002, 0x10000, CRC(e9880aa3) SHA1(cc47f631e758bd856bbc6d010fe230f9b1ed29de) )
	ROM_LOAD32_BYTE( "a-32.bin",	0x40003, 0x10000, CRC(dc286dc2) SHA1(eaa245b81f8a324988f617467fc3134a39b59c65) )
	ROM_LOAD32_BYTE( "a-19.bin", 	0x80000, 0x10000, CRC(2c0e7277) SHA1(cf14d1ca1fba2e2687998c04ad2ab8c629917412) )
	ROM_LOAD32_BYTE( "a-21.bin", 	0x80001, 0x10000, CRC(54761e57) SHA1(dc0fc645eb998675ab9fe683d63d4ee57ae23693) )
	ROM_LOAD32_BYTE( "a-23.bin", 	0x80002, 0x10000, CRC(a00d0676) SHA1(c2ab29a7489c6f774ce26ef023758215ea3f7050) )
	ROM_LOAD32_BYTE( "a-25.bin", 	0x80003, 0x10000, CRC(da398368) SHA1(115b2881d2d5ddeda2ce82bb209a2c0b4acfcae4) )
	ROM_LOAD32_BYTE( "a-27.bin",	0xc0000, 0x10000, CRC(8d459356) SHA1(143914b1ac074708fed1d89072f915424aeb841e) )
	ROM_LOAD32_BYTE( "a-29.bin",	0xc0001, 0x10000, CRC(a8245727) SHA1(13c1d417078d91b8c97e35d632e1ac4bc9bd64e3) )
	ROM_LOAD32_BYTE( "a-31.bin",	0xc0002, 0x10000, CRC(ef7d06fe) SHA1(541b5ba45f4140e2cc29a9da2592b476d414af5d) )
	ROM_LOAD32_BYTE( "a-33.bin",	0xc0003, 0x10000, CRC(1222af9f) SHA1(2364bd54cbe21dd688efff32e93bf154546c93d6) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx */
	ROM_LOAD( "a-2.bin", 0x0000, 0x8000, CRC(ed5bda9c) SHA1(f09a34caf1f9f6b119700a00635ab8fa8244362d) )
	ROM_LOAD( "a-3.bin", 0x0000, 0x8000, CRC(666fe754) SHA1(606090db53d658d7b04dca4748014a411e12f259) ) // we only use htis one for now

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "a-1.bin", 0x00000, 0x8000, CRC(209bb53a) SHA1(4ec9ca7532354f05f06295a01c4fa4982268e1d5) )

	ROM_REGION( 0x60000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "a-6.bin", 0x00000, 0x08000, CRC(191f98f4) SHA1(a6ac6feeeeed8e08a19bfa280c5f5f8bc69833e2) )
	ROM_CONTINUE(        0x10000, 0x08000 )
	ROM_LOAD( "a-5.bin", 0x20000, 0x08000, CRC(374466d0) SHA1(c648bcb17ed0501bb3e94994716b4a7b81ec75e4) )
	ROM_CONTINUE(        0x30000, 0x08000 )
	ROM_LOAD( "a-4.bin", 0x40000, 0x08000, CRC(2a27d0b0) SHA1(018db0e80af37c22c4eb57747093ac3b9faf8931) )
	ROM_CONTINUE(        0x50000, 0x08000 )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Turbo Outrun
    CPU: FD1094 (317-0118)
*/
ROM_START( toutrun )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12513.133", 0x000000, 0x10000, CRC(ae8835a5) SHA1(09573964d4f42ac0f08be3682b73e3420df27c6d) )
	ROM_LOAD16_BYTE( "epr12512.118", 0x000001, 0x10000, CRC(f90372ad) SHA1(b42dd8c580421b4d7ffacf8d3baa7b9fc9e392ef) )
	ROM_LOAD16_BYTE( "epr12515.132", 0x020000, 0x10000, CRC(1f047df4) SHA1(c1c67847f1390e671c19f0b90c3cbfbc237d960b) )
	ROM_LOAD16_BYTE( "epr12514.117", 0x020001, 0x10000, CRC(5539e9c3) SHA1(01046e3b836f66ba6d5c4be1611de48197aca67f) )
	ROM_LOAD16_BYTE( "epr12293.131", 0x040000, 0x10000, CRC(f4321eea) SHA1(64334acc82c14bb58b7d51719f34fd81cfb9fc6b) )
	ROM_LOAD16_BYTE( "epr12292.116", 0x040001, 0x10000, CRC(51d98af0) SHA1(6e7115706bfafb687faa23d55d4a8c8e498a4df2) )

	ROM_REGION( 0x2000, REGION_USER1, 0 )	/* decryption key */
	ROM_LOAD( "317-0118.key", 0x0000, 0x2000, CRC(083d7d56) SHA1(3153e44479986859f60a26fe9264ecea07e6e469) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "opr12295.76", 0x000000, 0x10000, CRC(d43a3a84) SHA1(362c98f62c205b6b40b7e8a4ba107745b547b984) )
	ROM_LOAD16_BYTE( "opr12294.58", 0x000001, 0x10000, CRC(27cdcfd3) SHA1(4fe57db95b109ab1bb1326789e06a3d3aac311cc) )
	ROM_LOAD16_BYTE( "opr12297.75", 0x020000, 0x10000, CRC(1d9b5677) SHA1(fb6e33acc43fbc7a8d7ac44045439ecdf794fdeb) )
	ROM_LOAD16_BYTE( "opr12296.57", 0x020001, 0x10000, CRC(0a513671) SHA1(4c13ca3a6f0aa9d06ed80798b466cca0c966a265) )

	ROM_REGION( 0x30000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr12323.102", 0x00000, 0x10000, CRC(4de43a6f) SHA1(68909338e1f192ac2699c8a8d24c3f46502dd019) )
	ROM_LOAD( "opr12324.103", 0x10000, 0x10000, CRC(24607a55) SHA1(69033f2281cd42e88233c23d809b73607fe54853) )
	ROM_LOAD( "opr12325.104", 0x20000, 0x10000, CRC(1405137a) SHA1(367db88d36852e35c5e839f692be5ea8c8e072d2) )

	ROM_REGION32_LE( 0x100000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12336.9",   0x00000, 0x20000, CRC(dda465c7) SHA1(83acc12a387b004986f084f25964c15a9f88a41a) )
	ROM_LOAD32_BYTE( "mpr12337.10",  0x00001, 0x20000, CRC(828233d1) SHA1(d73a200af4245d590e1fd3ac436723f99cc50452) )
	ROM_LOAD32_BYTE( "mpr12338.11",  0x00002, 0x20000, CRC(46b4b5f4) SHA1(afeb2e5ac6792edafe7328993fe8dfcd4bce1924) )
	ROM_LOAD32_BYTE( "mpr12339.12",  0x00003, 0x20000, CRC(0d7e3bab) SHA1(fdb603df55785ded593daf591ddd90f8f24e0d47) )
	ROM_LOAD32_BYTE( "mpr12364.13",  0x80000, 0x20000, CRC(a4b83e65) SHA1(966d8c163cef0842abff54e1dba3f15248e73f68) )
	ROM_LOAD32_BYTE( "mpr12365.14",  0x80001, 0x20000, CRC(4a80b2a9) SHA1(14b4fe71e102622a73c8dc0dbd0013cbbe6fcf9d) )
	ROM_LOAD32_BYTE( "mpr12366.15",  0x80002, 0x20000, CRC(385cb3ab) SHA1(fec6d80d488bfe26524fa3a48b195a45a073e481) )
	ROM_LOAD32_BYTE( "mpr12367.16",  0x80003, 0x20000, CRC(4930254a) SHA1(00f24be3bf02b143fa554f4d32e283bdac79af6a) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx */
	ROM_LOAD( "epr12298.11", 0x00000, 0x08000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "epr12300.88",	0x00000, 0x10000, CRC(e8ff7011) SHA1(6eaf3aea507007ea31d507ed7825d905f4b8e7ab) )

	ROM_REGION( 0x60000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "opr12301.66",    0x00000, 0x10000, CRC(6e78ad15) SHA1(c31ddf434b459cd1a381d2a028beabddd4ed10d2) )
	ROM_LOAD( "opr12302.67",    0x10000, 0x10000, CRC(e72928af) SHA1(40e0b178958cfe97c097fe9d82b5de54bc27a29f) )
	ROM_LOAD( "opr12303.68",    0x20000, 0x10000, CRC(8384205c) SHA1(c1f9d52bc587eab5a97867198e9aa7c19e973429) )
	ROM_LOAD( "opr12304.69",    0x30000, 0x10000, CRC(e1762ac3) SHA1(855f06c082a17d90857e6efa3cf95b0eda0e634d) )
	ROM_LOAD( "opr12305.70",    0x40000, 0x10000, CRC(ba9ce677) SHA1(056781f92450c902e1d279a02bda28337815cba9) )
	ROM_LOAD( "opr12306.71",    0x50000, 0x10000, CRC(e49249fd) SHA1(ff36e4dba4e9d3d354e3dd528edeb50ad9c18ee4) )
ROM_END

/**************************************************************************************************************************
    Turbo Outrun
    CPU: FD1094 (317-????)
*/
ROM_START( toutrun2 )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12410.133", 0x000000, 0x10000, CRC(aa74f3e9) SHA1(2daf6b17317542063c0a40beea5b45c797192591) )
	ROM_LOAD16_BYTE( "epr12409.118", 0x000001, 0x10000, CRC(c11c8ef7) SHA1(4c1c5100d7fd728642d58e4bf45fba48695841e3) )
	ROM_LOAD16_BYTE( "epr12412.132", 0x020000, 0x10000, CRC(b0534647) SHA1(40f2260ff0d0ac662d118cc7280bb26006ee75e9) )
	ROM_LOAD16_BYTE( "epr12411.117", 0x020001, 0x10000, CRC(12bb0d83) SHA1(4aa1b724b2a7258fff7aa1971582950b3163c0db) )
	ROM_LOAD16_BYTE( "epr12293.131", 0x040000, 0x10000, CRC(f4321eea) SHA1(64334acc82c14bb58b7d51719f34fd81cfb9fc6b) )
	ROM_LOAD16_BYTE( "epr12292.116", 0x040001, 0x10000, CRC(51d98af0) SHA1(6e7115706bfafb687faa23d55d4a8c8e498a4df2) )

	ROM_REGION( 0x2000, REGION_USER1, 0 )	/* decryption key */
	ROM_LOAD( "317-unknown.key", 0x0000, 0x2000, CRC(33e632ae) SHA1(9fd8bd11d0a87ec4dfc4dc386012ab7992cb2bd7) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "opr12295.76", 0x000000, 0x10000, CRC(d43a3a84) SHA1(362c98f62c205b6b40b7e8a4ba107745b547b984) )
	ROM_LOAD16_BYTE( "opr12294.58", 0x000001, 0x10000, CRC(27cdcfd3) SHA1(4fe57db95b109ab1bb1326789e06a3d3aac311cc) )
	ROM_LOAD16_BYTE( "opr12297.75", 0x020000, 0x10000, CRC(1d9b5677) SHA1(fb6e33acc43fbc7a8d7ac44045439ecdf794fdeb) )
	ROM_LOAD16_BYTE( "opr12296.57", 0x020001, 0x10000, CRC(0a513671) SHA1(4c13ca3a6f0aa9d06ed80798b466cca0c966a265) )

	ROM_REGION( 0x30000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr12323.102", 0x00000, 0x10000, CRC(4de43a6f) SHA1(68909338e1f192ac2699c8a8d24c3f46502dd019) )
	ROM_LOAD( "opr12324.103", 0x10000, 0x10000, CRC(24607a55) SHA1(69033f2281cd42e88233c23d809b73607fe54853) )
	ROM_LOAD( "opr12325.104", 0x20000, 0x10000, CRC(1405137a) SHA1(367db88d36852e35c5e839f692be5ea8c8e072d2) )

	ROM_REGION32_LE( 0x100000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD32_BYTE( "opr12307.9",  0x00000, 0x10000, CRC(437dcf09) SHA1(0022ee4d1c3698f77271e570cef98a8a1e5c5d6a) )
	ROM_LOAD32_BYTE( "opr12308.10", 0x00001, 0x10000, CRC(0de70cc2) SHA1(c03f8f8cda72daf64af2878bf254840ac6dd17eb) )
	ROM_LOAD32_BYTE( "opr12309.11", 0x00002, 0x10000, CRC(deb8c242) SHA1(c05d8ced4eafae52c4795fb1471cd66f5903d1aa) )
	ROM_LOAD32_BYTE( "opr12310.12", 0x00003, 0x10000, CRC(45cf157e) SHA1(5d0be2a374a53ea1fe0ba2bf9b2173e96de1eb51) )
	ROM_LOAD32_BYTE( "opr12311.13", 0x40000, 0x10000, CRC(ae2bd639) SHA1(64bb60ae7e3f87fbbce00106ba65c4e6fc1af0e4) )
	ROM_LOAD32_BYTE( "opr12312.14", 0x40001, 0x10000, CRC(626000e7) SHA1(4a7f9e76dd76a3dc56b8257149bc94be3f4f2e87) )
	ROM_LOAD32_BYTE( "opr12313.15", 0x40002, 0x10000, CRC(52870c37) SHA1(3a6836a46d94c0f9115800d206410252a1134c57) )
	ROM_LOAD32_BYTE( "opr12314.16", 0x40003, 0x10000, CRC(40c461ea) SHA1(7bed8f24112dc3c827fd087138fcf2700092aa59) )
	ROM_LOAD32_BYTE( "opr12315.17", 0x80000, 0x10000, CRC(3ff9a3a3) SHA1(0d90fe2669d03bd07a0d3b05934201778e28d54c) )
	ROM_LOAD32_BYTE( "opr12316.18", 0x80001, 0x10000, CRC(8a1e6dc8) SHA1(32f09ec504c2b6772815bad7380a2f738f11746a) )
	ROM_LOAD32_BYTE( "opr12317.19", 0x80002, 0x10000, CRC(77e382d4) SHA1(5b7912069a46043b7be989d82436add85497d318) )
	ROM_LOAD32_BYTE( "opr12318.20", 0x80003, 0x10000, CRC(d1afdea9) SHA1(813eccc88d5046992be5b5a0618d32127d16e30b) )
	ROM_LOAD32_BYTE( "opr12319.25", 0xc0000, 0x10000, CRC(df23baf9) SHA1(f9611391bb3b3b92203fa9f6dd461e3a6e863622) )
	ROM_LOAD32_BYTE( "opr12320.22", 0xc0001, 0x10000, CRC(7931e446) SHA1(9f2161a689ebad61f6653942e23d9c2bc6170d4a) )
	ROM_LOAD32_BYTE( "opr12321.23", 0xc0002, 0x10000, CRC(830bacd4) SHA1(5a4816969437ee1edca5845006c0b8e9ba365491) )
	ROM_LOAD32_BYTE( "opr12322.24", 0xc0003, 0x10000, CRC(8b812492) SHA1(bf1f9e059c093c0991c7caf1b01c739ed54b8357) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx */
	ROM_LOAD( "epr12298.11", 0x00000, 0x08000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "epr12300.88",	0x00000, 0x10000, CRC(e8ff7011) SHA1(6eaf3aea507007ea31d507ed7825d905f4b8e7ab) )

	ROM_REGION( 0x60000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "opr12301.66",    0x00000, 0x10000, CRC(6e78ad15) SHA1(c31ddf434b459cd1a381d2a028beabddd4ed10d2) )
	ROM_LOAD( "opr12302.67",    0x10000, 0x10000, CRC(e72928af) SHA1(40e0b178958cfe97c097fe9d82b5de54bc27a29f) )
	ROM_LOAD( "opr12303.68",    0x20000, 0x10000, CRC(8384205c) SHA1(c1f9d52bc587eab5a97867198e9aa7c19e973429) )
	ROM_LOAD( "opr12304.69",    0x30000, 0x10000, CRC(e1762ac3) SHA1(855f06c082a17d90857e6efa3cf95b0eda0e634d) )
	ROM_LOAD( "opr12305.70",    0x40000, 0x10000, CRC(ba9ce677) SHA1(056781f92450c902e1d279a02bda28337815cba9) )
	ROM_LOAD( "opr12306.71",    0x50000, 0x10000, CRC(e49249fd) SHA1(ff36e4dba4e9d3d354e3dd528edeb50ad9c18ee4) )
ROM_END

/**************************************************************************************************************************
    Turbo Outrun
    CPU: FD1094 (317-????)
*/
ROM_START( toutrun1 )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12397.133", 0x000000, 0x10000, CRC(e4b57d7d) SHA1(62be55356c82b38ebebcc87a5458e23300019339) )
	ROM_LOAD16_BYTE( "epr12396.118", 0x000001, 0x10000, CRC(5e7115cb) SHA1(02c9ec91d9afb424e5045671ab0b5499181728c9) )
	ROM_LOAD16_BYTE( "epr12399.132", 0x020000, 0x10000, CRC(62c77b1b) SHA1(004803c68cb1b3e414296ffbf50dc3b33b9ffb9a) )
	ROM_LOAD16_BYTE( "epr12398.117", 0x020001, 0x10000, CRC(18e34520) SHA1(3f10ecb809106b82fd44fd6244d8d8e7f1c8e08d) )
	ROM_LOAD16_BYTE( "epr12293.131", 0x040000, 0x10000, CRC(f4321eea) SHA1(64334acc82c14bb58b7d51719f34fd81cfb9fc6b) )
	ROM_LOAD16_BYTE( "epr12292.116", 0x040001, 0x10000, CRC(51d98af0) SHA1(6e7115706bfafb687faa23d55d4a8c8e498a4df2) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "opr12295.76", 0x000000, 0x10000, CRC(d43a3a84) SHA1(362c98f62c205b6b40b7e8a4ba107745b547b984) )
	ROM_LOAD16_BYTE( "opr12294.58", 0x000001, 0x10000, CRC(27cdcfd3) SHA1(4fe57db95b109ab1bb1326789e06a3d3aac311cc) )
	ROM_LOAD16_BYTE( "opr12297.75", 0x020000, 0x10000, CRC(1d9b5677) SHA1(fb6e33acc43fbc7a8d7ac44045439ecdf794fdeb) )
	ROM_LOAD16_BYTE( "opr12296.57", 0x020001, 0x10000, CRC(0a513671) SHA1(4c13ca3a6f0aa9d06ed80798b466cca0c966a265) )

	ROM_REGION( 0x30000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr12323.102", 0x00000, 0x10000, CRC(4de43a6f) SHA1(68909338e1f192ac2699c8a8d24c3f46502dd019) )
	ROM_LOAD( "opr12324.103", 0x10000, 0x10000, CRC(24607a55) SHA1(69033f2281cd42e88233c23d809b73607fe54853) )
	ROM_LOAD( "opr12325.104", 0x20000, 0x10000, CRC(1405137a) SHA1(367db88d36852e35c5e839f692be5ea8c8e072d2) )

	ROM_REGION32_LE( 0x100000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD32_BYTE( "opr12307.9",  0x00000, 0x10000, CRC(437dcf09) SHA1(0022ee4d1c3698f77271e570cef98a8a1e5c5d6a) )
	ROM_LOAD32_BYTE( "opr12308.10", 0x00001, 0x10000, CRC(0de70cc2) SHA1(c03f8f8cda72daf64af2878bf254840ac6dd17eb) )
	ROM_LOAD32_BYTE( "opr12309.11", 0x00002, 0x10000, CRC(deb8c242) SHA1(c05d8ced4eafae52c4795fb1471cd66f5903d1aa) )
	ROM_LOAD32_BYTE( "opr12310.12", 0x00003, 0x10000, CRC(45cf157e) SHA1(5d0be2a374a53ea1fe0ba2bf9b2173e96de1eb51) )
	ROM_LOAD32_BYTE( "opr12311.13", 0x40000, 0x10000, CRC(ae2bd639) SHA1(64bb60ae7e3f87fbbce00106ba65c4e6fc1af0e4) )
	ROM_LOAD32_BYTE( "opr12312.14", 0x40001, 0x10000, CRC(626000e7) SHA1(4a7f9e76dd76a3dc56b8257149bc94be3f4f2e87) )
	ROM_LOAD32_BYTE( "opr12313.15", 0x40002, 0x10000, CRC(52870c37) SHA1(3a6836a46d94c0f9115800d206410252a1134c57) )
	ROM_LOAD32_BYTE( "opr12314.16", 0x40003, 0x10000, CRC(40c461ea) SHA1(7bed8f24112dc3c827fd087138fcf2700092aa59) )
	ROM_LOAD32_BYTE( "opr12315.17", 0x80000, 0x10000, CRC(3ff9a3a3) SHA1(0d90fe2669d03bd07a0d3b05934201778e28d54c) )
	ROM_LOAD32_BYTE( "opr12316.18", 0x80001, 0x10000, CRC(8a1e6dc8) SHA1(32f09ec504c2b6772815bad7380a2f738f11746a) )
	ROM_LOAD32_BYTE( "opr12317.19", 0x80002, 0x10000, CRC(77e382d4) SHA1(5b7912069a46043b7be989d82436add85497d318) )
	ROM_LOAD32_BYTE( "opr12318.20", 0x80003, 0x10000, CRC(d1afdea9) SHA1(813eccc88d5046992be5b5a0618d32127d16e30b) )
	ROM_LOAD32_BYTE( "opr12319.25", 0xc0000, 0x10000, CRC(df23baf9) SHA1(f9611391bb3b3b92203fa9f6dd461e3a6e863622) )
	ROM_LOAD32_BYTE( "opr12320.22", 0xc0001, 0x10000, CRC(7931e446) SHA1(9f2161a689ebad61f6653942e23d9c2bc6170d4a) )
	ROM_LOAD32_BYTE( "opr12321.23", 0xc0002, 0x10000, CRC(830bacd4) SHA1(5a4816969437ee1edca5845006c0b8e9ba365491) )
	ROM_LOAD32_BYTE( "opr12322.24", 0xc0003, 0x10000, CRC(8b812492) SHA1(bf1f9e059c093c0991c7caf1b01c739ed54b8357) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx */
	ROM_LOAD( "epr12298.11", 0x00000, 0x08000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "epr12300.88",	0x00000, 0x10000, CRC(e8ff7011) SHA1(6eaf3aea507007ea31d507ed7825d905f4b8e7ab) )

	ROM_REGION( 0x60000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "opr12301.66",    0x00000, 0x10000, CRC(6e78ad15) SHA1(c31ddf434b459cd1a381d2a028beabddd4ed10d2) )
	ROM_LOAD( "opr12302.67",    0x10000, 0x10000, CRC(e72928af) SHA1(40e0b178958cfe97c097fe9d82b5de54bc27a29f) )
	ROM_LOAD( "opr12303.68",    0x20000, 0x10000, CRC(8384205c) SHA1(c1f9d52bc587eab5a97867198e9aa7c19e973429) )
	ROM_LOAD( "opr12304.69",    0x30000, 0x10000, CRC(e1762ac3) SHA1(855f06c082a17d90857e6efa3cf95b0eda0e634d) )
	ROM_LOAD( "opr12305.70",    0x40000, 0x10000, CRC(ba9ce677) SHA1(056781f92450c902e1d279a02bda28337815cba9) )
	ROM_LOAD( "opr12306.71",    0x50000, 0x10000, CRC(e49249fd) SHA1(ff36e4dba4e9d3d354e3dd528edeb50ad9c18ee4) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Super Hangon
    CPU: 68000 (317-????)
*/
ROM_START( shangon )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr10886.133", 0x000000, 0x10000, CRC(8be3cd36) SHA1(de96481807e782ca441d51e99f1a221bdee7d170) )
	ROM_LOAD16_BYTE( "epr10884.118", 0x000001, 0x10000, CRC(cb06150d) SHA1(840dada0cdeec444b554e6c1f2bdacc1047be567) )
	ROM_LOAD16_BYTE( "epr10887.132", 0x020000, 0x10000, CRC(8d248bb0) SHA1(7d8ed61609fd0df203255e7d046d9d30983f1dcd) )
	ROM_LOAD16_BYTE( "epr10884.117", 0x020001, 0x10000, CRC(70795f26) SHA1(332921b0a6534c4cbfe76ff957c721cc80d341b0) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr10792.76", 0x00000, 0x10000, CRC(16299d25) SHA1(b14d5feef3e6889320d51ffca36801f4c9c4d5f8) )
	ROM_LOAD16_BYTE( "epr10790.58", 0x00001, 0x10000, CRC(2246cbc1) SHA1(c192b1ddf4c848adb564c7c87d5413d62ed650d7) )
	ROM_LOAD16_BYTE( "epr10793.75", 0x20000, 0x10000, CRC(d9525427) SHA1(cdb24db9f7a293f20fd8becc4afe84fd6abd678a) )
	ROM_LOAD16_BYTE( "epr10791.57", 0x20001, 0x10000, CRC(5faf4cbe) SHA1(41659a961e6469d9233849c3c587cd5a0a141344) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr10652.54",        0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr10651.55",        0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr10650.56",        0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x100000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD16_BYTE( "epr10794.8",	0x000001, 0x020000, CRC(7c958e63) SHA1(ef79614e94280607a6cdf6e13db051accfd2add0) )
	ROM_LOAD16_BYTE( "epr10798.16",  0x000000, 0x020000, CRC(7d58f807) SHA1(783c9929d27a0270b3f7d5eb799cee6b2e5b7ae5) )
	ROM_LOAD16_BYTE( "epr10795.6",   0x040001, 0x020000, CRC(d9d31f8c) SHA1(3ce07b83e3aa2d8834c1a449fa31e003df5486a3) )
	ROM_LOAD16_BYTE( "epr10799.14",  0x040000, 0x020000, CRC(96d90d3d) SHA1(6572cbce8f98a1a7a8e59b0c502ab274f78d2819) )
	ROM_LOAD16_BYTE( "epr10796.4",   0x080001, 0x020000, CRC(fb48957c) SHA1(86a66bcf38686be5537a1361d390ecbbccdddc11) )
	ROM_LOAD16_BYTE( "epr10800.12",  0x080000, 0x020000, CRC(feaff98e) SHA1(20e38f9039079f64919d750a2e1382503d437463) )
	ROM_LOAD16_BYTE( "epr10797.2",   0x0c0001, 0x020000, CRC(27f2870d) SHA1(40a34e4555885bf3c6a42e472b80d11c3bd4dcba) )
	ROM_LOAD16_BYTE( "epr10801.10",  0x0c0000, 0x020000, CRC(12781795) SHA1(44bf6f657f32b9fab119557eb73c2fbf78700204) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx */
	ROM_LOAD( "mpr10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "epr10649c.88", 0x0000, 0x08000, CRC(f6c1ce71) SHA1(12299f7e5378a56be3a31cce3b8b74e48744f33a) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "epr10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_LOAD( "epr10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_LOAD( "epr10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_LOAD( "epr10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
ROM_END

/**************************************************************************************************************************
    Super Hangon
    CPU: FD1089B (317-0034)
*/
ROM_START( shangon3 )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code - protected */
	ROM_LOAD16_BYTE( "epr10789.133", 0x000000, 0x10000, CRC(6092c5ce) SHA1(dc010ab6d4dbbcb2f38de9f4f80674e9e1502dea) )
	ROM_LOAD16_BYTE( "epr10788.118", 0x000001, 0x10000, CRC(c3d8a1ea) SHA1(b7f5de5e9ab9e5fb59937c11acd960f8e4a9bc2f) )
	ROM_LOAD16_BYTE( "epr10637a.132", 0x020000, 0x10000, CRC(ad6c1308) SHA1(ee63168205bcb8b2c3dcbc3d7ba8a7f8f8a85952) )
	ROM_LOAD16_BYTE( "epr10635a.117", 0x020001, 0x10000, CRC(a2415595) SHA1(2a8b960ea70066bf43c7b3772a0ed53d7c737b2c) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr10640.76", 0x00000, 0x10000, CRC(02be68db) SHA1(8c9f98ee49db54ee53b721ecf53f91737ae6cd73) )
	ROM_LOAD16_BYTE( "epr10638.58", 0x00001, 0x10000, CRC(f13e8bee) SHA1(1c16c018f58f1fb49e240314a7e97a947087fad9) )
	ROM_LOAD16_BYTE( "epr10641.75", 0x20000, 0x10000, CRC(38c3f808) SHA1(36fae99b56980ef33853170afe10b363cd41c053) )
	ROM_LOAD16_BYTE( "epr10639.57", 0x20001, 0x10000, CRC(8cdbcde8) SHA1(0bcb4df96ee16db3dd4ce52fccd939f48a4bc1a0) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr10652.54",        0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr10651.55",        0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr10650.56",        0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x0e0000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD16_BYTE( "epr10675.8",	0x000001, 0x010000, CRC(d6ac012b) SHA1(305023b1a0a9d84cfc081ffc2ad7578b53d562f2) )
	ROM_LOAD16_BYTE( "epr10682.16",  0x000000, 0x010000, CRC(d9d83250) SHA1(f8ca3197edcdf53643a5b335c3c044ddc1310cd4) )
	ROM_LOAD16_BYTE( "epr10676.7",   0x020001, 0x010000, CRC(25ebf2c5) SHA1(abcf673ae4e280417dd9f46d18c0ec7c0e4802ae) )
	ROM_LOAD16_BYTE( "epr10683.15",  0x020000, 0x010000, CRC(6365d2e9) SHA1(688e2ba194e859f86cd3486c2575ebae257e975a) )
	ROM_LOAD16_BYTE( "epr10677.6",   0x040001, 0x010000, CRC(8a57b8d6) SHA1(df1a31559dd2d1e7c2c9d800bf97526bdf3e84e6) )
	ROM_LOAD16_BYTE( "epr10684.14",  0x040000, 0x010000, CRC(3aff8910) SHA1(4b41a49a7f02363424e814b37edce9a7a44a112e) )
	ROM_LOAD16_BYTE( "epr10678.5",   0x060001, 0x010000, CRC(af473098) SHA1(a2afaba1cbf672949dc50e407b46d7e9ae183774) )
	ROM_LOAD16_BYTE( "epr10685.13",  0x060000, 0x010000, CRC(80bafeef) SHA1(f01bcf65485e60f34e533295a896fca0b92e5b14) )
	ROM_LOAD16_BYTE( "epr10679.4",   0x080001, 0x010000, CRC(03bc4878) SHA1(548fc58bcc620204e30fa12fa4c4f0a3f6a1e4c0) )
	ROM_LOAD16_BYTE( "epr10686.12",  0x080000, 0x010000, CRC(274b734e) SHA1(906fa528659bc17c9b4744cec52f7096711adce8) )
	ROM_LOAD16_BYTE( "epr10680.3",   0x0a0001, 0x010000, CRC(9f0677ed) SHA1(5964642b70bfad418da44f2d91476f887b021f74) )
	ROM_LOAD16_BYTE( "epr10687.11",  0x0a0000, 0x010000, CRC(508a4701) SHA1(d17aea2aadc2e2cd65d81bf91feb3ef6923d5c0b) )
	ROM_LOAD16_BYTE( "epr10681.2",   0x0c0001, 0x010000, CRC(b176ea72) SHA1(7ec0eb0f13398d014c2e235773ded00351edb3e2) )
	ROM_LOAD16_BYTE( "epr10688.10",  0x0c0000, 0x010000, CRC(42fcd51d) SHA1(0eacb3527dc21746e5b901fcac83f2764a0f9e2c) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx */
	ROM_LOAD( "mpr10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "epr10649a.88", 0x0000, 0x08000, CRC(bf38330f) SHA1(3d825bb02ef5a9f5c4fcaa71b3735e7f8e47f178) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "epr10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_LOAD( "epr10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_LOAD( "epr10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_LOAD( "epr10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
ROM_END

/**************************************************************************************************************************
    Super Hangon
    CPU: FD1089B (317-0034)
*/
ROM_START( shangon2 )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code - protected */
	ROM_LOAD16_BYTE( "epr10636a.133", 0x000000, 0x10000, CRC(74a64f4f) SHA1(3266a9a3c68e147bc8626de7ec45b59fd28f9d1d) )
	ROM_LOAD16_BYTE( "epr10634a.118", 0x000001, 0x10000, CRC(1608cb4a) SHA1(56b0a6a0a4951f15a269d94d18821809ac0d3d53) )
	ROM_LOAD16_BYTE( "epr10637a.132", 0x020000, 0x10000, CRC(ad6c1308) SHA1(ee63168205bcb8b2c3dcbc3d7ba8a7f8f8a85952) )
	ROM_LOAD16_BYTE( "epr10635a.117", 0x020001, 0x10000, CRC(a2415595) SHA1(2a8b960ea70066bf43c7b3772a0ed53d7c737b2c) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr10640.76", 0x00000, 0x10000, CRC(02be68db) SHA1(8c9f98ee49db54ee53b721ecf53f91737ae6cd73) )
	ROM_LOAD16_BYTE( "epr10638.58", 0x00001, 0x10000, CRC(f13e8bee) SHA1(1c16c018f58f1fb49e240314a7e97a947087fad9) )
	ROM_LOAD16_BYTE( "epr10641.75", 0x20000, 0x10000, CRC(38c3f808) SHA1(36fae99b56980ef33853170afe10b363cd41c053) )
	ROM_LOAD16_BYTE( "epr10639.57", 0x20001, 0x10000, CRC(8cdbcde8) SHA1(0bcb4df96ee16db3dd4ce52fccd939f48a4bc1a0) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr10652.54",        0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr10651.55",        0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr10650.56",        0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x0e0000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD16_BYTE( "epr10675.8",	0x000001, 0x010000, CRC(d6ac012b) SHA1(305023b1a0a9d84cfc081ffc2ad7578b53d562f2) )
	ROM_LOAD16_BYTE( "epr10682.16",  0x000000, 0x010000, CRC(d9d83250) SHA1(f8ca3197edcdf53643a5b335c3c044ddc1310cd4) )
	ROM_LOAD16_BYTE( "epr10676.7",   0x020001, 0x010000, CRC(25ebf2c5) SHA1(abcf673ae4e280417dd9f46d18c0ec7c0e4802ae) )
	ROM_LOAD16_BYTE( "epr10683.15",  0x020000, 0x010000, CRC(6365d2e9) SHA1(688e2ba194e859f86cd3486c2575ebae257e975a) )
	ROM_LOAD16_BYTE( "epr10677.6",   0x040001, 0x010000, CRC(8a57b8d6) SHA1(df1a31559dd2d1e7c2c9d800bf97526bdf3e84e6) )
	ROM_LOAD16_BYTE( "epr10684.14",  0x040000, 0x010000, CRC(3aff8910) SHA1(4b41a49a7f02363424e814b37edce9a7a44a112e) )
	ROM_LOAD16_BYTE( "epr10678.5",   0x060001, 0x010000, CRC(af473098) SHA1(a2afaba1cbf672949dc50e407b46d7e9ae183774) )
	ROM_LOAD16_BYTE( "epr10685.13",  0x060000, 0x010000, CRC(80bafeef) SHA1(f01bcf65485e60f34e533295a896fca0b92e5b14) )
	ROM_LOAD16_BYTE( "epr10679.4",   0x080001, 0x010000, CRC(03bc4878) SHA1(548fc58bcc620204e30fa12fa4c4f0a3f6a1e4c0) )
	ROM_LOAD16_BYTE( "epr10686.12",  0x080000, 0x010000, CRC(274b734e) SHA1(906fa528659bc17c9b4744cec52f7096711adce8) )
	ROM_LOAD16_BYTE( "epr10680.3",   0x0a0001, 0x010000, CRC(9f0677ed) SHA1(5964642b70bfad418da44f2d91476f887b021f74) )
	ROM_LOAD16_BYTE( "epr10687.11",  0x0a0000, 0x010000, CRC(508a4701) SHA1(d17aea2aadc2e2cd65d81bf91feb3ef6923d5c0b) )
	ROM_LOAD16_BYTE( "epr10681.2",   0x0c0001, 0x010000, CRC(b176ea72) SHA1(7ec0eb0f13398d014c2e235773ded00351edb3e2) )
	ROM_LOAD16_BYTE( "epr10688.10",  0x0c0000, 0x010000, CRC(42fcd51d) SHA1(0eacb3527dc21746e5b901fcac83f2764a0f9e2c) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx */
	ROM_LOAD( "mpr10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "ic88", 0x0000, 0x08000, CRC(1254efa6) SHA1(997770ccdd776de6e335a6d8b1e15d200cbd4410) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "epr10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_LOAD( "epr10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_LOAD( "epr10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_LOAD( "epr10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
ROM_END

/**************************************************************************************************************************
    Super Hangon
    CPU: FD1089B (317-0034)
*/
ROM_START( shangon1 )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code - protected */
	ROM_LOAD16_BYTE( "ic133", 0x000000, 0x10000, CRC(e52721fe) SHA1(21f0aa14d0cbda3d762bca86efe089646031aef5) )
	ROM_LOAD16_BYTE( "ic118", 0x000001, 0x10000, BAD_DUMP CRC(5fee09f6) SHA1(b97a08ba75d8c617aceff2b03c1f2bfcb16181ef) )
	ROM_LOAD16_BYTE( "ic132", 0x020000, 0x10000, CRC(5d55d65f) SHA1(d02d76b98d74746b078b0f49f0320b8be48e4c47) )
	ROM_LOAD16_BYTE( "ic117", 0x020001, 0x10000, CRC(b967e8c3) SHA1(00224b337b162daff03bbfabdcf8541025220d46) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr10640.76", 0x00000, 0x10000, CRC(02be68db) SHA1(8c9f98ee49db54ee53b721ecf53f91737ae6cd73) )
	ROM_LOAD16_BYTE( "epr10638.58", 0x00001, 0x10000, CRC(f13e8bee) SHA1(1c16c018f58f1fb49e240314a7e97a947087fad9) )
	ROM_LOAD16_BYTE( "epr10641.75", 0x20000, 0x10000, CRC(38c3f808) SHA1(36fae99b56980ef33853170afe10b363cd41c053) )
	ROM_LOAD16_BYTE( "epr10639.57", 0x20001, 0x10000, CRC(8cdbcde8) SHA1(0bcb4df96ee16db3dd4ce52fccd939f48a4bc1a0) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr10652.54",        0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr10651.55",        0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr10650.56",        0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x0e0000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD16_BYTE( "epr10675.8",	0x000001, 0x010000, CRC(d6ac012b) SHA1(305023b1a0a9d84cfc081ffc2ad7578b53d562f2) )
	ROM_LOAD16_BYTE( "epr10682.16",  0x000000, 0x010000, CRC(d9d83250) SHA1(f8ca3197edcdf53643a5b335c3c044ddc1310cd4) )
	ROM_LOAD16_BYTE( "epr10676.7",   0x020001, 0x010000, CRC(25ebf2c5) SHA1(abcf673ae4e280417dd9f46d18c0ec7c0e4802ae) )
	ROM_LOAD16_BYTE( "epr10683.15",  0x020000, 0x010000, CRC(6365d2e9) SHA1(688e2ba194e859f86cd3486c2575ebae257e975a) )
	ROM_LOAD16_BYTE( "epr10677.6",   0x040001, 0x010000, CRC(8a57b8d6) SHA1(df1a31559dd2d1e7c2c9d800bf97526bdf3e84e6) )
	ROM_LOAD16_BYTE( "epr10684.14",  0x040000, 0x010000, CRC(3aff8910) SHA1(4b41a49a7f02363424e814b37edce9a7a44a112e) )
	ROM_LOAD16_BYTE( "epr10678.5",   0x060001, 0x010000, CRC(af473098) SHA1(a2afaba1cbf672949dc50e407b46d7e9ae183774) )
	ROM_LOAD16_BYTE( "epr10685.13",  0x060000, 0x010000, CRC(80bafeef) SHA1(f01bcf65485e60f34e533295a896fca0b92e5b14) )
	ROM_LOAD16_BYTE( "epr10679.4",   0x080001, 0x010000, CRC(03bc4878) SHA1(548fc58bcc620204e30fa12fa4c4f0a3f6a1e4c0) )
	ROM_LOAD16_BYTE( "epr10686.12",  0x080000, 0x010000, CRC(274b734e) SHA1(906fa528659bc17c9b4744cec52f7096711adce8) )
	ROM_LOAD16_BYTE( "epr10680.3",   0x0a0001, 0x010000, CRC(9f0677ed) SHA1(5964642b70bfad418da44f2d91476f887b021f74) )
	ROM_LOAD16_BYTE( "epr10687.11",  0x0a0000, 0x010000, CRC(508a4701) SHA1(d17aea2aadc2e2cd65d81bf91feb3ef6923d5c0b) )
	ROM_LOAD16_BYTE( "epr10681.2",   0x0c0001, 0x010000, CRC(b176ea72) SHA1(7ec0eb0f13398d014c2e235773ded00351edb3e2) )
	ROM_LOAD16_BYTE( "epr10688.10",  0x0c0000, 0x010000, CRC(42fcd51d) SHA1(0eacb3527dc21746e5b901fcac83f2764a0f9e2c) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* road gfx */
	ROM_LOAD( "mpr10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "ic88", 0x0000, 0x08000, CRC(1254efa6) SHA1(997770ccdd776de6e335a6d8b1e15d200cbd4410) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "epr10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_LOAD( "epr10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_LOAD( "epr10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_LOAD( "epr10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
ROM_END

/**************************************************************************************************************************
    Limited Edition Hangon
    CPU: 68000 (317-????)
*/
ROM_START( shangnle )
	ROM_REGION( 0x60000, REGION_CPU1, 0 ) /* 68000 code  */
	ROM_LOAD16_BYTE( "epr13944.133", 0x000000, 0x10000, CRC(989a80db) SHA1(5026e5cf52d4fd85a0bab6c4ea7a34cf266b2a3b) )
	ROM_LOAD16_BYTE( "epr13943.118", 0x000001, 0x10000, CRC(426e3050) SHA1(f332ea76285b4e1361d818cbe5aab0640b4185c3) )
	ROM_LOAD16_BYTE( "epr10899.132", 0x020000, 0x10000, CRC(bb3faa37) SHA1(ccf3352255503fd6619e6e116d187a8cd1ff75e6) )
	ROM_LOAD16_BYTE( "epr10897.117", 0x020001, 0x10000, CRC(5f087eb1) SHA1(bdfcc39e92087057acc4e91741a03e7ba57824c1) )

	ROM_REGION( 0x60000, REGION_CPU2, 0 ) /* second 68000 CPU  */
	ROM_LOAD16_BYTE( "epr10640.76", 0x00000, 0x10000, CRC(02be68db) SHA1(8c9f98ee49db54ee53b721ecf53f91737ae6cd73) )
	ROM_LOAD16_BYTE( "epr10638.58", 0x00001, 0x10000, CRC(f13e8bee) SHA1(1c16c018f58f1fb49e240314a7e97a947087fad9) )
	ROM_LOAD16_BYTE( "epr10641.75", 0x20000, 0x10000, CRC(38c3f808) SHA1(36fae99b56980ef33853170afe10b363cd41c053) )
	ROM_LOAD16_BYTE( "epr10639.57", 0x20001, 0x10000, CRC(8cdbcde8) SHA1(0bcb4df96ee16db3dd4ce52fccd939f48a4bc1a0) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr10652.54",        0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr10651.55",        0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr10650.56",        0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x0e0000, REGION_GFX2, 0 ) /* sprites */
	ROM_LOAD16_BYTE( "epr10675.8",	0x000001, 0x010000, CRC(d6ac012b) SHA1(305023b1a0a9d84cfc081ffc2ad7578b53d562f2) )
	ROM_LOAD16_BYTE( "epr10682.16",  0x000000, 0x010000, CRC(d9d83250) SHA1(f8ca3197edcdf53643a5b335c3c044ddc1310cd4) )
	ROM_LOAD16_BYTE( "epr13945.7",   0x020001, 0x010000, CRC(fbb1eef9) SHA1(2798df2f25706e0d3be01d945274f478d7e5a2ae) )
	ROM_LOAD16_BYTE( "epr13946.15",  0x020000, 0x010000, CRC(03144930) SHA1(c20f4883ee2de35cd0b67949de0e41464f2c5fae) )
	ROM_LOAD16_BYTE( "epr10677.6",   0x040001, 0x010000, CRC(8a57b8d6) SHA1(df1a31559dd2d1e7c2c9d800bf97526bdf3e84e6) )
	ROM_LOAD16_BYTE( "epr10684.14",  0x040000, 0x010000, CRC(3aff8910) SHA1(4b41a49a7f02363424e814b37edce9a7a44a112e) )
	ROM_LOAD16_BYTE( "epr10678.5",   0x060001, 0x010000, CRC(af473098) SHA1(a2afaba1cbf672949dc50e407b46d7e9ae183774) )
	ROM_LOAD16_BYTE( "epr10685.13",  0x060000, 0x010000, CRC(80bafeef) SHA1(f01bcf65485e60f34e533295a896fca0b92e5b14) )
	ROM_LOAD16_BYTE( "epr10679.4",   0x080001, 0x010000, CRC(03bc4878) SHA1(548fc58bcc620204e30fa12fa4c4f0a3f6a1e4c0) )
	ROM_LOAD16_BYTE( "epr10686.12",  0x080000, 0x010000, CRC(274b734e) SHA1(906fa528659bc17c9b4744cec52f7096711adce8) )
	ROM_LOAD16_BYTE( "epr10680.3",   0x0a0001, 0x010000, CRC(9f0677ed) SHA1(5964642b70bfad418da44f2d91476f887b021f74) )
	ROM_LOAD16_BYTE( "epr10687.11",  0x0a0000, 0x010000, CRC(508a4701) SHA1(d17aea2aadc2e2cd65d81bf91feb3ef6923d5c0b) )
	ROM_LOAD16_BYTE( "epr10681.2",   0x0c0001, 0x010000, CRC(b176ea72) SHA1(7ec0eb0f13398d014c2e235773ded00351edb3e2) )
	ROM_LOAD16_BYTE( "epr10688.10",  0x0c0000, 0x010000, CRC(42fcd51d) SHA1(0eacb3527dc21746e5b901fcac83f2764a0f9e2c) )

	ROM_REGION( 0x8000, REGION_GFX3, 0 ) /* Road Graphics */
	ROM_LOAD( "epr10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* sound CPU */
	ROM_LOAD( "epr10649c.88", 0x0000, 0x08000, CRC(f6c1ce71) SHA1(12299f7e5378a56be3a31cce3b8b74e48744f33a) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 ) /* sound PCM data */
	ROM_LOAD( "epr10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_LOAD( "epr10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_LOAD( "epr10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_LOAD( "epr10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
ROM_END



/*************************************
 *
 *  Generic driver initialization
 *
 *************************************/

static DRIVER_INIT( outrun )
{
	outrun_generic_init();
	custom_io_r = outrun_custom_io_r;
	custom_io_w = outrun_custom_io_w;

	krb_outrun_patch_cpu_synchro();
}


static DRIVER_INIT( outrunb )
{
	static const UINT8 memory_map[] = { 0x02,0x00,0x0d,0x10,0x00,0x12,0x0c,0x13,0x08,0x14,0x0f,0x20,0x00,0x00,0x00,0x00 };
	UINT16 *word;
	UINT8 *byte;
	int i, length;

	outrun_generic_init();
	custom_map = memory_map;
	custom_io_r = outrun_custom_io_r;
	custom_io_w = outrun_custom_io_w;

	/* main CPU: swap bits 11,12 and 6,7 */
	word = (UINT16 *)memory_region(REGION_CPU1);
	length = memory_region_length(REGION_CPU1) / 2;
	for (i = 0; i < length; i++)
		word[i] = BITSWAP16(word[i], 15,14,11,12,13,10,9,8,6,7,5,4,3,2,1,0);

	/* sub CPU: swap bits 14,15 and 2,3 */
	word = (UINT16 *)memory_region(REGION_CPU2);
	length = memory_region_length(REGION_CPU2) / 2;
	for (i = 0; i < length; i++)
		word[i] = BITSWAP16(word[i], 14,15,13,12,11,10,9,8,7,6,5,4,2,3,1,0);

	/* road gfx */
	/* rom orun_me.rom - swap bits 5,6 */
	/* rom orun_mf.rom - swap bits 6,7 */
	/* I don't know why there's 2 road roms, but I'm using orun_me.rom */
	byte = memory_region(REGION_GFX3);
	length = memory_region_length(REGION_GFX3);
	for (i = 0; i < length; i++)
		byte[i] = BITSWAP8(byte[i], 7,5,6,4,3,2,1,0);

	/* Z80 code: swap bits 5,6 */
	byte = memory_region(REGION_CPU3);
	length = memory_region_length(REGION_CPU3);
	for (i = 0; i < length; i++)
		byte[i] = BITSWAP8(byte[i], 7,5,6,4,3,2,1,0);

	krb_outrun_patch_cpu_synchro();
}


static DRIVER_INIT( shangon )
{
	outrun_generic_init();
	custom_io_r = shangon_custom_io_r;
	custom_io_w = shangon_custom_io_w;

    krb_shangon_patch_cpu_synchro();
}


static DRIVER_INIT( shangon3 )
{
	extern void fd1089_decrypt_0034(void);
	outrun_generic_init();
	fd1089_decrypt_0034();
	custom_io_r = shangon_custom_io_r;
	custom_io_w = shangon_custom_io_w;

    krb_shangon_patch_cpu_synchro();
}



/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1986, outrun,   0,        outrun,  outrun,   outrun,   ROT0, "Sega",    "Out Run (set 3)", 0 ,1)
GAME( 1986, outrun2,  outrun,   outrun,  outrun,   outrun,   ROT0, "Sega",    "Out Run (set 2)", 0 ,1)
GAME( 1986, outrun1,  outrun,   outrun,  outrundx, outrun,   ROT0, "Sega",    "Out Run (set 1, Deluxe?)", 0 ,1)
GAME( 1986, outrunb,  outrun,   outrun,  outrun,   outrunb,  ROT0, "bootleg", "Out Run (bootleg)", 0 ,1)
GAME( 1987, shangon,  0,        shangon, shangon,  shangon,  ROT0, "Sega",    "Super Hang-On (set 4, 68000)", 0 ,0)
GAME( 1987, shangon3, shangon,  shangon, shangon,  shangon3, ROT0, "Sega",    "Super Hang-On (set 3, FD1089B 317-0034)", 0 ,0)
GAME( 1987, shangon2, shangon,  shangon, shangon,  shangon3, ROT0, "Sega",    "Super Hang-On (set 2, FD1089B 317-0034)", 0 ,0)
GAME( 1987, shangon1, shangon,  shangon, shangon,  shangon3, ROT0, "Sega",    "Super Hang-On (set 1, FD1089B 317-0034)", 0 ,0)
GAME( 1991, shangnle, shangon,  shangon, shangon,  shangon,  ROT0, "Sega",    "Limited Edition Hang-On", 0 ,0)
GAME( 1989, toutrun,  0,        outrun,  toutrun,  outrun,   ROT0, "Sega",    "Turbo Out Run (set 3, upgrade kit, FD1094 317-0118)" , 0 ,1)
GAME( 1989, toutrun2, toutrun,  outrun,  toutrun,  outrun,   ROT0, "Sega",    "Turbo Out Run (set 2, upright, FD1094 317-unknown)", 0 ,1)
GAME( 1989, toutrun1, toutrun,  outrun,  toutrun,  outrun,   ROT0, "Sega",    "Turbo Out Run (set 1, FD1094 317-unknown)", GAME_NOT_WORKING ,1) // not decrypted
