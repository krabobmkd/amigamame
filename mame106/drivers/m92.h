/*******************************************************************************

    Irem M92 input port macros

    Can probably be used for other Irem games..

*******************************************************************************/

/* Player 1 and 2 have the same IN */
#define IREM_JOYSTICK_1_2(_n_) \
	PORT_START	\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(_n_) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(_n_) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(_n_) \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(_n_) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED ) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(_n_) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(_n_)

/* Player 3 and 4 have IN similar to Player 1 and 2, but have the COIN and START buttons */
#define IREM_JOYSTICK_3_4(_n_) \
	PORT_START	\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(_n_) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(_n_) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(_n_) \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(_n_) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START##_n_ ) /* If common slots, Coin3 if separate */ \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN##_n_ ) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(_n_) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(_n_)

#define IREM_COINS \
	PORT_START	\
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE1 ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE )  \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )  \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* Actually vblank, handled above */ \

	/* Coin Mode 1, the new one (m72 early games have different Coinage) */ \
#define IREM_COIN_MODE_1_NEW \
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coinage ) ) \
	PORT_DIPSETTING(    0xa0, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(    0xb0, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0xd0, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x10, "2 Coins to Start/1 to Continue") \
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_3C ) ) \
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )

#define IREM_COIN_MODE_2 \
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) \
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )


#define IREM_SYSTEM_DIPSWITCH_4PLAYERS \
	PORT_START \
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) ) \
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Cabinet ) ) \
	PORT_DIPSETTING(    0x02, "2 Players" ) \
	PORT_DIPSETTING(    0x00, "4 Players" ) \
	PORT_DIPNAME( 0x04, 0x04, "Coin Slots" ) \
	PORT_DIPSETTING(    0x04, "Common" ) \
	PORT_DIPSETTING(    0x00, "Separate" ) \
	PORT_DIPNAME( 0x08, 0x08, "Coin Mode" ) /* Default 1 */ \
	PORT_DIPSETTING(    0x08, "1" ) \
	PORT_DIPSETTING(    0x00, "2" ) \
	IREM_COIN_MODE_1_NEW

#define IREM_SYSTEM_DIPSWITCH_3PLAYERS \
	PORT_START \
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) ) \
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Cabinet ) ) \
	PORT_DIPSETTING(    0x02, "2 Players" ) \
	PORT_DIPSETTING(    0x00, "3 Players" ) \
	PORT_DIPNAME( 0x04, 0x04, "Coin Slots" ) \
	PORT_DIPSETTING(    0x04, "Common" ) \
	PORT_DIPSETTING(    0x00, "Separate" ) \
	PORT_DIPNAME( 0x08, 0x08, "Coin Mode" ) /* Default 1 */ \
	PORT_DIPSETTING(    0x08, "1" ) \
	PORT_DIPSETTING(    0x00, "2" ) \
	IREM_COIN_MODE_1_NEW

#define IREM_SYSTEM_DIPSWITCH_CABINET \
	PORT_START \
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) ) \
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Cabinet ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) ) \
	PORT_DIPSETTING(    0x02, DEF_STR( Cocktail ) ) \
	PORT_DIPNAME( 0x04, 0x04, "Coin Slots" ) \
	PORT_DIPSETTING(    0x04, "Common" ) \
	PORT_DIPSETTING(    0x00, "Separate" ) \
	PORT_DIPNAME( 0x08, 0x08, "Coin Mode" ) /* Default 1 */ \
	PORT_DIPSETTING(    0x08, "1" ) \
	PORT_DIPSETTING(    0x00, "2" ) \
	IREM_COIN_MODE_1_NEW

#define IREM_SYSTEM_DIPSWITCH \
	PORT_START \
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) ) \
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) ) \
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x04, 0x04, "Coin Slots" ) \
	PORT_DIPSETTING(    0x04, "Common" ) \
	PORT_DIPSETTING(    0x00, "Separate" ) \
	PORT_DIPNAME( 0x08, 0x08, "Coin Mode" ) /* Default 1 */ \
	PORT_DIPSETTING(    0x08, "1" ) \
	PORT_DIPSETTING(    0x00, "2" ) \
	IREM_COIN_MODE_1_NEW

#define IREM_PORT_UNUSED \
	PORT_START	\
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

