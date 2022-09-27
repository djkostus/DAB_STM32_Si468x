/*
 * Si468x.h
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#ifndef INC_SI468X_H_
#define INC_SI468X_H_

#define SPI_TX_BUFFER_SIZE 4096
#define SPI_RX_BUFFER_SIZE 9000

#define RESET_PIN_HIGH HAL_GPIO_WritePin(DAB_GPIO_RESET_GPIO_Port, DAB_GPIO_RESET_Pin, 1)
#define RESET_PIN_LOW HAL_GPIO_WritePin(DAB_GPIO_RESET_GPIO_Port, DAB_GPIO_RESET_Pin, 0)

#define CS_PIN_LOW HAL_GPIO_WritePin(DAB_GPIO_CS_GPIO_Port, DAB_GPIO_CS_Pin, 0)
#define CS_PIN_HIGH HAL_GPIO_WritePin(DAB_GPIO_CS_GPIO_Port, DAB_GPIO_CS_Pin, 1)

#define SUCCESS 0x00
#define HAL_ERROR		0x01
#define INVALID_INPUT	0x02
#define INVALID_MODE	0x04
#define TIMEOUT			0x08
#define COMMAND_ERROR	0x20
#define UNSUPPORTED_FUNCTION	0x80

#define POLL_TIMEOUT_MS 300

#define TUNE_TIMEOUT_MS 3000

#define IMAGE_DAB_6_0_9_START_ADDR 0x6000
#define IMAGE_DAB_4_0_5_START_ADDR 0x106000
#define IMAGE_FM_4_0_12_START_ADDR 0x86000

#define USE_ANT_CAP 1
#define NOT_USE_ANT_CAP 0

//Default DAB Channels

//#define CH_5A 0   // 174928 kHz
//#define CH_5B 1   // 176640 kHz
//#define CH_5C 2   // 178352 kHz
//#define CH_5D 3   // 180064 kHz
//#define CH_6A 4   // 181936 kHz
//#define CH_6B 5   // 183648 kHz
//#define CH_6C 6   // 185360 kHz
//#define CH_6D 7   // 187072 kHz
//#define CH_7A 8   // 188928 kHz
//#define CH_7B 9   // 190640 kHz
//#define CH_7C 10   // 192352 kHz
//#define CH_7D 11   // 194064 kHz
//#define CH_8A 12   // 195936 kHz
//#define CH_8B 13   // 197648 kHz
//#define CH_8C 14   // 199360 kHz
//#define CH_8D 15   // 201072 kHz
//#define CH_9A 16   // 202928 kHz
//#define CH_9B 17   // 204640 kHz
//#define CH_9C 18   // 206352 kHz
//#define CH_9D 19   // 208064 kHz
//#define CH_10A 20   // 209936 kHz
//#define CH_undef_1 21   // 210096 kHz
//#define CH_10B 22   // 211648 kHz
//#define CH_10C 23   // 213360 kHz
//#define CH_10D 24   // 215072 kHz
//#define CH_11A 25   // 216928 kHz
//#define CH_undef_2 26   // 217088 kHz
//#define CH_11B 27   // 218640 kHz
//#define CH_11C 28   // 220352 kHz
//#define CH_11D 29   // 222064 kHz
//#define CH_12A 30   // 223936 kHz
//#define CH_undef_3 31   // 224096 kHz
//#define CH_12B 32   // 225648 kHz
//#define CH_12C 33   // 227360 kHz
//#define CH_12D 34   // 229072 kHz
//#define CH_13A 35   // 230784 kHz
//#define CH_13B 36   // 232496 kHz
//#define CH_13C 37   // 234208 kHz
//#define CH_13D 38   // 235776 kHz
//#define CH_13E 39   // 237488 kHz
//#define CH_13F 40   // 239200 kHz

typedef uint8_t RETURN_CODE;

typedef struct{
	uint8_t subchannel_id;
	uint8_t tm_id;
}dab_component_t;

typedef struct{
	uint8_t name[20];
	uint8_t pd_flag;
	uint8_t p_ty;
	uint32_t srv_ref;
	uint8_t country_id;
	uint32_t service_id;
	uint32_t freq;
	uint8_t freq_id;
	uint8_t number_of_components;
	dab_component_t components[4];
}dab_service_t;

typedef struct{
	//dab_digrad_status byte 4: INTSRC
	uint8_t rssi_l_int			: 1;	//Indicates RSSI below ref_prop DAB_DIGRAD_RSQ_RSSI_LOW_THRESHOLD
	uint8_t rssi_h_int			: 1;	//Indicates RSSI above ref_prop DAB_DIGRAD_RSQ_RSSI_HIGH_THRESHOLD
	uint8_t acq_int				: 1;	//Indicates a change in the ensemble acquisition state
	uint8_t fic_err_int			: 1;	//Indicates the FIC decoder has encountered unrecoverable errors. This is likely due to poor signal conditions
	uint8_t 					: 4;	//not used, ignore
	//dab_digrad_status byte 5: ACQ_STATUS
	uint8_t valid				: 1;	//When set to 1, the RSSI is at or above the valid threshold. It is recommended that the valid bit be used as part of tune validation. Once STC is set the valid bit can be checked to verify that then tune has passed both the RSSI valid threshold and that acquisition has been achived. The host should set the RSSI thershold, validation time and acquisition time to achieve solid tune time performance. Doing this helps insure an accurate tune indication and helps to decrease scan times due to quick station disqualification
	uint8_t						: 1;	//not used, ignore
	uint8_t acq					: 1;	//When set to 1, the ensemble is acquired
	uint8_t fic_err				: 1;	//When set to 1, the ensemble is experiencing FIC errors. Signal quality has been degraded and acquisition may be lost
	uint8_t 					: 4;	//ignore
	//dab_digrad_status byte 6: RSSI
	int rssi					: 8;	//Received signal strength indicator in dBuV.
	//dab_digrad_status byte 7: SNR
	uint8_t snr					: 8;	//Indicates the current estimate of the digital SNR in dB
	//dab_digrad_status byte 8: FIC_QUALITY
	uint8_t fic_quality			: 8;	//Indicates the current estimate of the ensemble's FIC quality. The number provided is between 0 and 100
	//dab_digrad_status byte 9: CNR
	uint8_t cnr					: 8;	//Indicates the current estimate of the CNR in dB. The CNR is the ratio of the OFDM signal level during the on period and during the off (null) period
	//dab_digrad_status byte 10 & 11: FIB_ERROR_COUNT
	uint16_t fib_err_count		: 16;	//Indicates the num of Fast Information Blocks received with errors.
	//dab_digrad_status byte 12, 13, 14, 15: TUNE_FREQ
	uint32_t tune_freq			: 32;	//Indicates the currently tuned frequency in kHz
	//dab_digrad_status byte 16: TUNE_INDEX
	uint8_t tune_index			: 8;	//Indicates the currently tuned frequency index.
	//dab_digrad_status byte 17: FFT_OFFSET
	uint8_t fft_offset			: 8;	//Indicates the number of samples the sync symbol was found to be off relative to its expected sample location
	//dab_digrad_status byte 18 & 19: READANTCAP
	uint16_t read_ant_cap		: 16;	//Returns the antenna tuning cap value
	//dab_digrad_status byte 20 & 21: CU_LEVEL
	uint16_t cu_level			: 16;	//Returns the number of CU that are currently beibng consumed by enabled services (number of currently decoded CU's)
	//dab_digrad_status byte 22: FAST_DECT
	uint8_t fast_dect			: 8;	//Returns the statistical metric for DAB fast detect. The metric is a confidence level that dab signal is detected. The threshold for dab detected is greater than 4
	//dab_digrad_status bytes 23, 24, 25:
	uint32_t					: 24;	//ignore
	//dab_digrad_status bytes 26, 27:
	uint16_t sample_rate_offset	: 16;	//Indicates the estimated sample rate offset [ppm]
	//dab_digrad_status bytes 28, 29, 30, 31
	uint32_t freq_offset		: 32;	//Indicates the estimated frequency offset in Hz of the received ensemble
	//dab_digrad_status bytes 32, 33, 34, 35
	uint32_t fic_bit_cnt		: 32;	//Indicates the total number of bits tested in FIC Pre-Viterbi pseudo BER measurement
	//dab_digrad_status bytes 36, 37, 38, 39
	uint32_t fic_err_cnt		: 32;	//Indicates the total number of error bits in FIC Pre-Viterbi pseudo BER measurement
}dab_digrad_status_t;

typedef struct{
	uint16_t id;
	uint8_t label[20];
	uint32_t freq;
	uint8_t freq_id;
}dab_ensemble_t;

typedef struct{
	//RD_REPLY byte 0
	uint8_t stc_int			: 1;	// seek/tune complete interrupt
	uint8_t acf_int			: 1;	//Automatically controlled features interrupt indicator. Indicates the one of the dynamically system modifiers has crossed a programmed threshold. Service via FM_ACF_STATUS command
	uint8_t rds_int			: 1;	//RDS Data Interrupt indicator.
	uint8_t rsq_int			: 1;	//Received Signal Quality interrupt indicator. Indicates that a received signal metric is above or below a threshold defined by threshold properties. Service via FM_RSQ_STATUS command
	uint8_t d_srv_int		: 1;	//Indicates that an enabled data component of one of the digital services requires attention. Service by sending the GET_DIGITAL_SERVICE_DATA command.
	uint8_t d_acq_int		: 1;	//Digital radio link change interrupt indicator. Indicates that something in the digital radio ensemble acquisition status has changed. Service by sending the DAB_DIGRAD_STATUS command
	uint8_t err_cmd			: 1;	//command error - The command that was sent is either not a valid command in this mode, had an invalid argument, or is otherwise not allowed
	uint8_t cts				: 1;	//clear to send
	//RD_REPLY byte 1
	uint8_t d_acf_int		: 1;	//HD radio ACF status change interrupt indicator. Indicates that a new interrupt related to the HD radio ACF feature has occurred. Service by sending the HD_ACF_STATUS command
	uint8_t 				: 4;	//ignore
	uint8_t d_evnt_int		: 1;	//Digital radio event change interrupt indicator. Indicates that a new event related to the digital radio has occurred. Service by sending the DAB_DIGRAD_STATUS command
	uint8_t d_fic_int		: 1;	//DAB FIC event change interrupt indicator. Indicates that a new event related to the DAB FIC has occurred. Service by sending the DAB_GET_FIC_DATA command
	uint8_t 				: 1;	//ignore
	//RD_REPLY byte 2
	uint8_t 				: 8;	//ignore, reserved for future use
	//RD_REPLY byte 3
	uint8_t err_nr			: 1;	//When set a non-recoverable error has occurred. The system keep alive timer has expired. The only way to recover is for the user to reset the chip.
	uint8_t arb_err			: 1;	//When set, an arbiter error has occurred. The only way to recover is for the user to reset the chip.
	uint8_t cmd_of_err		: 1;	//When set, the control interface has dropped data during a command write, which is a fatal error. This is generally caused by running at a SPI clock rate that is too fast for the data arbiter and memory speed
	uint8_t rep_of_err		: 1;	//When set, the control interface has dropped data during a reply read, which is a fatal error. This is generally caused by running at a SPI clock rate that is too fast for the given data arbiter and memory speed
	uint8_t dsp_err			: 1;	//The DSP has encountered a frame overrun. This is a fatal error
	uint8_t rf_fe_err		: 1;	//indicates that the RF front end of the system is in an unexpected state
	uint8_t pwr_up_state	: 2;	//Indicates the powerup state of the system.
}rd_reply_t;

typedef struct{
	//reply byte 4 - EVENT_INT
	uint8_t srv_list_int	: 1;	//New service list interrupt. Indicates that a new digital service list is available. The new service list is retrieved with the GET_DIGITAL_SERVICE_LIST command.
	uint8_t freq_info_int	: 1;	//New Frequency Information interrupt. Indicates that new Frequency Information is available. The Frequency Information list is retrieved with the DAB_GET_FREQ_INFO command. The rate at which frequency information interrupts can occur is defined by the DAB_EVENT_MIN_FREQINFO_PERIOD property
	uint8_t serv_link_int	: 1;	//Service linking information interrupt. Indicates that new service linking information is available or has changed. The service linking information list is retrieved with the DAB_GET_SERVICE_LINKING_INFO command
	uint8_t oe_serv_int		: 1;	//Other Ensemble (OE) Services interrupt. Indicates that new OE service information is available or has changed. The other ensemble information is retrieved with the DAB_GET_OE_SERVICES_INFO command
	uint8_t anno_int		: 1;	//announcement information interrupt. Indicates that an announcement event (started or stopped) is available
	uint8_t audio_int		: 1;	//When set indicates that a change in the audio playback state has occurred (mute or unmute)
	uint8_t recfg_wrn_int	: 1;	//Ensemble reconfiguration warning. Indicates that an ensemble reconfiguration will occur in 6 seconds
	uint8_t recfg_int		: 1;	//Ensemble reconfiguration event. Indicates that an ensemble reconfiguration has occurred
	//reply byte 5 - EVENT_STATUS
	uint8_t srv_list		: 1;	//Service list available. Indicates that a digital service list is available. The service list is retrieved with the [ref GET_DIGITAL_SERVICE_LIST] command. If a service list is not available or it is in transition, this bit will be low. When the service list is in transition, this bit will remain low until the service list debounce timer has expired. See the [ref DAB_EVENT_MIN_SVRLIST_PERIOD] property for more details.
	uint8_t freq_info		: 1;	//Frequency Information (FI) (FIG0/21) available. Indicates that Frequency Information (FI) is available. The FI list is retrieved with the [ref DAB_GET_FREQ_INFO] command
	uint8_t serv_link		: 1;	//Service linking information (FIG 0/6) available. Indicates that service linking information is available. The service linking information list is retrieved with the [ref DAB_GET_SERVICE_LINKING_INFO] command
	uint8_t oe_serv			: 1;	//Indicates that OE service information is available (FIG0/24). The OE service information is retrieved with the [ref DAB_GET_OE_SERVICES_INFO] command.
	uint8_t anno			: 1;	//Announcement available
	uint8_t audio			: 1;	//When set, the audio system is rendering audio. See the AUDIO_STATUS field of this command's response for additional audio details
	uint8_t 				: 2;	//ignore
	//reply byte 6 & 7 - SVRLISTVER
	uint16_t srv_list_ver	: 16;	//Indicates the current version of the digital service list. This field is incremented by 1 each time the service list is updated. The host can use this field to help determine if a new service list needs to be collected
	//reply byte 8 - AUDIO_STATUS
	uint8_t blk_loss		: 1;	//When set the audio system has detected an audio block loss event. This occurs when no audio blocks are available to decode. This event will trigger an AUDIOINT interrupt and clear the AUDIO bit. The BLK_LOSS bit is sticky and is cleared by setting the CLR_AUDIO_STATUS bit of this command's OPTION_BITS field
	uint8_t blk_error		: 1;	//When set the audio system has detected errors in the encoded audio blocks. This event will trigger an AUDIOINT interrupt and clear the AUDIO bit. The BLK_ERROR bit is sticky and is cleared by setting the CLR_AUDIO_STATUS bit of this command's OPTION_BITS field.
	uint8_t sm_eng			: 1;	//When set, the audio system has engaged the softmute system.
	uint8_t mute_eng		: 1;	//When set, the audio system has been muted by the host.
	uint8_t 				: 4;	//ignore
}dab_events_t;

typedef struct{
	uint16_t year			: 16;
	uint8_t month			: 8;
	uint8_t day				: 8;
	uint8_t hour			: 8;
	uint8_t minute			: 8;
	uint8_t second			: 8;
}time_t;

typedef struct{
	uint8_t total_services;							//total quantity of services found during full scan
	uint8_t total_ensembles;						//total quantity of ensembles found during full scan
	uint8_t actual_services;						//actual quantity of services during scanning process, this variable is necessary to save stations in memory
	uint8_t actual_station;							//index of actually playing station
	uint8_t last_station_index;						//index of last played station
	uint32_t actual_freq;							//value of the frequency to which the Si468x is currently tuned in kHz
	uint8_t actual_freq_id;							//frequency table index of the frequency to which the Si468x is currently tuned in kHz
	uint8_t freq_cnt;								//quantity of frequencies in Si468x memory
	uint8_t audio_volume;							//analog audio output volume (0...63)
}dab_management_t;

typedef struct{
	//reply byte 4, 5
	uint16_t audio_bit_rate			: 16;	//Audio bit rate of the current audio service (kbps).
	//reply byte 6, 7
	uint16_t audio_sample_rate		: 16;	//Sample rate of the current audio service (Hz)
	//reply byte 8
	uint8_t audio_mode				: 2;	//Audio mode. 0x0 - Dual, 0x1 - Mono, 0x2 - Stereo,	0x3	- Joint stereo
	uint8_t audio_sbr				: 1;	//Audio SBR flag. only applicable to DAB+. Set to 0 for DAB
	uint8_t audio_ps_flag			: 1;	//Audio PS flag. only applicable to DAB+. Set to 0 for DAB
	uint8_t audio_xpad_ind			: 2;	//XPAD indicator. XPAD_NO: 0x0,	XPAD_SHORT: 0x1, XPAD_VARIABLE:	0x2, XPAD_RFU (reserved): 0x3
	uint8_t 						: 2;	//ignore
	//reply byte 9
	uint8_t audio_drc_gain			: 8;	//The dynamic range control (DRC) gain that is applied to the current audio service. The range of this field is from 0 to 63, representing 0 to 15.75dB in increment of 0.25dB
	//reply byte 10, 11
	uint16_t						: 16;	//Ignore
	//reply byte 12, 13, 14, 15
	uint32_t pseudo_ber_bit_cnt		: 32;	//Number of bits tested in Pre-Viterbi pseudo BER measurement. If PSEUDO_BER_CONFIG is LONG_TERM_BER, this field returns the total number of bits tested since the service is started. If PSEUDO_BER_CONFIG is SHORT_TERM_BER, this field returns the number of bits tested in the most recent N audio frames. N is defined in DAB_TEST_PSEUDO_BER_OPTION.
	//reply byte 16, 17, 18, 19
	uint32_t pseudo_ber_err_cnt		: 32;	//Number of error bits in Pre-Viterbi pseudo BER measurement. If PSEUDO_BER_CONFIG is LONG_TERM_BER, this field returns the total number of error bits since the service is started. If PSEUDO_BER_CONFIG is SHORT_TERM_BER, this field returns the number of error bits in the most recent N audio frames. N is defined in DAB_TEST_PSEUDO_BER_OPTION.


}dab_audio_info_t;

typedef struct{
	//reply byte 4 INTSRC
	uint8_t dsrv_pckt_int			: 1;	//Data for an enabled data service is ready for transfer to the host.
	uint8_t dsrv_ovfl_int			: 1;	//The data services system has overflowed. This indicates that the host processor is not reading the services data out of the device fast enough. At most 20 outstanding data blocks can be queued in the device, and the total size of the data blocks cannot exceed 16KB
	uint8_t 						: 6;	//ignore

	//reply byte 5 BUFF_COUNT
	uint8_t buff_count				: 8;	//Indicates the remaining number of buffers in the data service buffer queue.

	//reply byte 6 SRV_STATE
	uint8_t srv_state 				: 8;	//The status indicator for the associated service component.
											//PLAYING	0x0	Indicates that the service is playing out normally
											//STOPPED	0x1	Indicates that the data service has stopped and that this is the last data block associated with the service
											//OVERFLOW	0x2	Indicates that the system was not able to forward a packet due to a memory overflow. If this status is present the host is not reading the packet data fast enough
											//NEW_OBJECT	0x3	Indicates the this data packet represents the beginning of a new data object
											//PACKET_ERRORS	0x4	Indicates the this data packet was received with errors

	//reply byte 7 DATA TYPE
	uint8_t dsc_ty					: 6;	//Used for DAB only. Returns 0 or the DSCTy depending on the DATA_SRC field. See DATA_SRC for details.
											//TYPE = 0: unspecified data
											//TYPE = 1: TMC
											//TYPE = 5: TDC/TPEG
											//TYPE =60: MOT

	uint8_t data_src				: 2;	//For DAB indicates the payload source. Not used in HD modes of operation
											//DATA_SERVICE	0x0	Indicates that the payload is from a standard data service and DATA_TYPE is DSCTy
											//PAD_DATA	0x1	Indicates that the payload is non-DLS PAD and DATA_TYPE is DSCTy
											//PAD_DLS	0x2	Indicates that the payload is DLS PAD and DATA_TYPE is 0

	//reply byte 8, 9, 10, 11 SERVICE_ID
	uint32_t service_id				: 32;	//The Service ID this data is associated wit

	//reply byte 12, 13, 14, 15 COMPONENT_ID
	uint32_t component_id			: 32;	//The Component ID or Port Number this data is associated with

	//reply byte 16, 17 UA_TYPE
	uint16_t ua_type				: 16;	//User application type. Set property [ref DAB_XPAD_ENABLE] to enable user application types

	//reply byte 18, 19 BYTE_COUNT
	uint16_t byte_count				: 16;	//The length of this data block in bytes excluding the DSRV header. The maximum length can be 8215 bytes in DAB mode

	//reply byte 20, 21 SEG_NUM
	uint16_t seg_num 				: 16;	//The segment number for this data block. If the data is associated with a stream this value represents a sequence number. Note that segment numbers can be returned out of order. Therefore the host must store and reassemble the data as needed

	//reply byte 22, 23 NUM_SEGS
	uint16_t num_segs				: 16;	//The total number of segments to be returned for this data object. If NUM_SEGS=0 then no object length information is known or the associated service is a stream

}dab_service_data_reply_header_t;


void Si468x_dab_init();
void Si468x_reset();
void Si468x_disable();
void Si468x_power_up();

void Si468x_load_init();
void Si468x_bootloader_load_host();
void Si468x_firmware_load_flash(uint32_t start_address);
void Si468x_boot();

void Si468x_write_single_byte(uint8_t byte_to_write);
uint8_t Si468x_read_single_byte();

RETURN_CODE Si468x_wait_for_CTS(uint16_t timeout);

void Si468x_write_multiple(uint16_t len, uint8_t* data_to_write);
void Si468x_read_multiple(uint16_t len, uint8_t* read_buffer);

RETURN_CODE Si468x_write_command (uint16_t length, uint8_t *buffer);
RETURN_CODE Si468x_read_reply(uint16_t length, uint8_t *buffer);

void Si468x_get_sys_state();
void Si468x_get_part_info();

void Si468x_set_property(uint16_t property_id, uint16_t property_value);
uint16_t Si468x_get_property(uint16_t property_id);

void Si468x_dab_get_freq_list();
void Si468x_dab_tune_freq(uint8_t channel, uint16_t ant_cap);
void Si468x_dab_reset_interrupts();
dab_digrad_status_t Si468x_dab_digrad_status();
void Si468x_dab_get_digital_service_list();
void Si468x_dab_start_digital_service(uint32_t service_id, uint32_t component_id);
uint8_t Si468x_dab_get_ensemble_info();
uint8_t Si468x_dab_full_scan();
dab_audio_info_t Si468x_dab_get_audio_info();
void Si468x_dab_get_event_status();
void Si468x_dab_get_component_info(uint32_t service_id, uint8_t component_id);

void Si468x_dab_get_digital_service_data();

void Si468x_dab_test_get_ber_info();

void Si468x_dab_get_time();

void Si468x_set_audio_volume(uint8_t _volume);

uint16_t Si468x_test_get_rssi();

void play_station(uint8_t station_id);

void restore_from_eeprom();

dab_digrad_status_t get_digrad_status();
dab_management_t get_dab_management();

dab_service_t* get_dab_service_list();

dab_ensemble_t* get_dab_ensemble_list();

char* get_dls_label();

void calibration(uint8_t channel);


#endif /* INC_SI468X_H_ */
