#include "temp.h"
#include <stdio.h>
#include <string.h>

#include "krawall.h"
#include "mtypes.h"
#include "modules.h"
#include "samples.h"

int checkButton( int shift, int *button )
{
	if( ( ( P1 >> shift ) & 1 ) != (*button) ) {
		(*button) ^= 1;
		return *button;
	}
	return 1;
}


int music()
{
	int buttonA, buttonB, buttonR, buttonL = 1;

	DISPCNT = 4 | ( 1 << 10 );
	INT_ME = 1;					// enable interrupts
	INT_IE |= ( 1 << 13 ); 		// cart-remove-interrupt (handled by crt0)

	//CST_ROM0_1ST_3WAIT | CST_ROM0_2ND_1WAIT | CST_PREFETCH_ENABLE
	WSCNT = ( 5 << 2 ) | ( 1 << 14 );	// set rom-timing

	kragInit( KRAG_INIT_STEREO );					// init krawall
	krapPlay( &mod_aryx, KRAP_MODE_LOOP, 0 );	// play module

	while( 1 ) {
		while( !VCOUNT );
		while( VCOUNT );			// wait for line 0

		*( ( unsigned short* )0x5000000 ) =  ( 5 << 0 ) + ( 22 << 5 ) + ( 31 << 10 );
		kramWorker();	// do the stuff
		*( ( unsigned short* )0x5000000 ) = 0;

		if( !checkButton( 0, &buttonA ) ) {
			kramPlay( samples[ SAMPLE_MECH_THRACK ], 1, 0 );
		}
		if( !checkButton( 1, &buttonB ) ) {
			kramPlay( samples[ SAMPLE_MECH_KLACK ], 1, 0 );
		}
		if( !checkButton( 8, &buttonR ) ) {
			chandle x = kramPlay( samples[ SAMPLE_HIT_GUIT ], 1, 0 );
			kramSetPan( x, 64 );
		}
		if( !checkButton( 9, &buttonL ) ) {
			chandle x = kramPlay( samples[ SAMPLE_HIT_SKIP ], 1, 0 );
			kramSetPan( x, -64 );
		}
	}

	return 0;
}