#ifndef __YMDELTAT_H__
#define __YMDELTAT_H__

#include "../../common_def.h"

#define YM_DELTAT_SHIFT    (16)

#define YM_DELTAT_EMULATION_MODE_NORMAL	0
#define YM_DELTAT_EMULATION_MODE_YM2610	1


typedef void (*STATUS_CHANGE_HANDLER)(void *chip, UINT8 status_bits);


/* DELTA-T (adpcm type B) struct */
typedef struct deltat_adpcm_state {     /* AT: rearranged and tightened structure */
	UINT8	*memory;
	INT32	*output_pointer;/* pointer of output pointers   */
	INT32	*pan;			/* pan : &output_pointer[pan]   */
	double	freqbase;
#if 0
	double	write_time;		/* Y8950: 10 cycles of main clock; YM2608: 20 cycles of main clock */
	double	read_time;		/* Y8950: 8 cycles of main clock;  YM2608: 18 cycles of main clock */
#endif
	UINT32	memory_size;
	UINT32	memory_mask;
	int		output_range;
	UINT32	address_mask;
	UINT32	now_addr;		/* current address      */
	UINT32	now_step;		/* current step         */
	UINT32	step;			/* step                 */
	UINT32	start;			/* start address        */
	UINT32	limit;			/* limit address        */
	UINT32	end;			/* end address          */
	UINT32	delta;			/* delta scale          */
	INT32	volume;			/* current volume       */
	INT32	acc;			/* shift Measurement value*/
	INT32	adpcmd;			/* next Forecast        */
	INT32	adpcml;			/* current value        */
	INT32	prev_acc;		/* leveling value       */
	UINT8	now_data;		/* current rom data     */
	UINT8	CPU_data;		/* current data from reg 08 */
	UINT8	portstate;		/* port status          */
	UINT8	control2;		/* control reg: SAMPLE, DA/AD, RAM TYPE (x8bit / x1bit), ROM/RAM */
	UINT8	portshift_base;	/* address bits shift-left:
							** 8 for YM2610,
							** 5 for Y8950 and YM2608 */
	UINT8	now_portshift;	/* current address bits shift-left */

	UINT8	memread;		/* needed for reading/writing external memory */

	/* handlers and parameters for the status flags support */
	STATUS_CHANGE_HANDLER	status_set_handler;
	STATUS_CHANGE_HANDLER	status_reset_handler;

	/* note that different chips have these flags on different
	** bits of the status register
	*/
	void *	status_change_which_chip;	/* this chip id */
	UINT8	status_change_EOS_bit;		/* 1 on End Of Sample (record/playback/cycle time of AD/DA converting has passed)*/
	UINT8	status_change_BRDY_bit;		/* 1 after recording 2 datas (2x4bits) or after reading/writing 1 data */
	UINT8	status_change_ZERO_bit;		/* 1 if silence lasts for more than 290 miliseconds on ADPCM recording */

	/* neither Y8950 nor YM2608 can generate IRQ when PCMBSY bit changes, so instead of above,
	** the statusflag gets ORed with PCM_BSY (below) (on each read of statusflag of Y8950 and YM2608)
	*/
	UINT8	PCM_BSY;		/* 1 when ADPCM is playing; Y8950/YM2608 only */

	UINT8	reg[16];		/* adpcm registers      */
	UINT8	emulation_mode;	/* which chip we're emulating */
}YM_DELTAT;

//void YM_DELTAT_BRDY_callback(YM_DELTAT *DELTAT);

UINT8 YM_DELTAT_ADPCM_Read(YM_DELTAT *DELTAT);
void YM_DELTAT_ADPCM_Write(YM_DELTAT *DELTAT,int r,int v);
void YM_DELTAT_ADPCM_Init(YM_DELTAT *DELTAT,int emulation_mode,int portshift,INT32* output_ptr,int output_range);
void YM_DELTAT_ADPCM_Reset(YM_DELTAT *DELTAT,int pan);
void YM_DELTAT_ADPCM_CALC(YM_DELTAT *DELTAT);

void YM_DELTAT_calc_mem_mask(YM_DELTAT* DELTAT);

#endif	// __YMDELTAT_H__
