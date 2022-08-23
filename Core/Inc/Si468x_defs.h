/*
 * Si468x_defs.h
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#ifndef INC_SI468X_DEFS_H_
#define INC_SI468X_DEFS_H_

/************** COMMANDS ADDRESSES ***************************************************************************************************************************************/

//HARDWARE
#define SI468X_CMD_RD_REPLY											0x00	//Returns the status byte and data for the last command sent to the device.
#define SI468X_CMD_POWER_UP											0x01	//Power-up the device and set system settings.

//BOOTLOADER
#define SI468X_CMD_HOST_LOAD										0x04	//Loads an image from the HOST over the command interface
#define SI468X_CMD_FLASH_LOAD										0x05	//Loads an image from external FLASH over secondary SPI bus
#define SI468X_CMD_LOAD_INIT										0x06	//Prepares the bootloader to receive a new image.
#define SI468X_CMD_BOOT												0x07	//Boots the image currently loaded in RAM.

//SHARED
#define SI468X_CMD_GET_PART_INFO 									0x08	//Reports basic information about the device.
#define SI468X_CMD_GET_SYS_STATE									0x09	//Reports system state information.
#define SI468X_CMD_GET_POWER_UP_ARGS								0x0A	//Reports basic information about the device such as arguments used during POWER_UP.
#define SI468X_CMD_GET_FUNC_INFO									0x12	//Returns the Function revision information of the device.

//COMMAND AND REPLY HELPERS
#define SI468X_CMD_READ_OFFSET										0x10	//Reads a portion of response buffer from an offset.
#define SI468X_CMD_WRITE_STORAGE									0x15	//Writes data to the on board storage area at a specified offset.
#define SI468X_CMD_READ_STORAGE										0x16	//Reads data from the on board storage area from a specified offset.

//PROPERTY HANDLING
#define SI468X_CMD_SET_PROPERTY										0x13	//Sets the value of a property.
#define SI468X_CMD_GET_PROPERTY 									0x14	//Gets the value of a property.

//AGC
#define SI468X_CMD_GET_AGC_STATUS									0x17	//Reports the status of the AGC.

//TUNE
#define SI468X_CMD_FM_TUNE_FREQ										0x30	//Tunes the FM receiver to a frequency in 10 kHz steps.
#define SI468X_CMD_FM_SEEK_START									0x31	//Initiates a seek for a channel that meets the validation criteria for FM.
#define SI468X_CMD_DAB_TUNE_FREQ									0xB0	//Tunes the DAB Receiver to a frequency between 168 MHz and 240 MHz.
#define SI468X_CMD_DAB_SET_FREQ_LIST								0xB8	//Sets the DAB frequency table. The frequencies are in units of kHz.
#define SI468X_CMD_DAB_GET_FREQ_LIST								0xB9	//Gets the DAB frequency table

//FM STATUS
#define SI468X_CMD_FM_RSQ_STATUS									0x32	//Returns status information about the received signal quality.
#define SI468X_CMD_FM_ACF_STATUS									0x33	//Returns status information about automatically controlled features.

//FM RDS
#define SI468X_CMD_FM_RDS_STATUS									0x34	//Queries the status of RDS decoder and Fifo.
#define SI468X_CMD_FM_RDS_BLOCKCOUNT								0x35	//Queries the block statistic info of RDS decoder.

//DAB
#define SI468X_CMD_DAB_DIGRAD_STATUS								0xB2	//Returns status information about the digital demod and ensemble.
#define SI468X_CMD_DAB_GET_EVENT_STATUS								0xB3	//Gets information about the various events related to the DAB radio.
#define SI468X_CMD_DAB_GET_ENSEMBLE_INFO							0xB4	//Gets information about the current ensemble
#define SI468X_CMD_DAB_GET_ANNOUNCEMENT_SUPPORT_INFO				0xB5	//Gets the announcement support information.
#define SI468X_CMD_DAB_GET_ANNOUNCEMENT_INFO						0xB6	//Gets announcement information from the announcement queue
#define SI468X_CMD_DAB_GET_SERVICE_LINKING_INFO						0xB7	//Provides service linking (FIG 0/6) information for the passed in service ID.
#define SI468X_CMD_DAB_GET_FIC_DATA									0xBA	//DAB_GET_FIC_DATA gets the Fast Information Blocks (FIB) in the Fast Information Channel (FIC).
#define SI468X_CMD_DAB_GET_COMPONENT_INFO							0xBB	//Gets information about components within the ensemble if available.
#define SI468X_CMD_DAB_GET_TIME										0xBC	//Gets the ensemble time adjusted for the local time offset or the UTC.
#define SI468X_CMD_DAB_GET_AUDIO_INFO								0xBD	//Gets audio service info
#define SI468X_CMD_DAB_GET_SUBCHAN_INFO								0xBE	//Gets sub-channel info
#define SI468X_CMD_DAB_GET_FREQ_INFO								0xBF	//Gets ensemble freq info
#define SI468X_CMD_DAB_GET_SERVICE_INFO								0xC0	//Gets information about a service.
#define SI468X_CMD_DAB_GET_OE_SERVICES_INFO							0xC1	//Provides other ensemble (OE) services (FIG 0/24) information for the passed in service ID.
#define SI468X_CMD_DAB_ACF_STATUS									0xC2	//Returns status information about automatically controlled features.
#define SI468X_CMD_DAB_TEST_GET_BER_INFO							0xE8	//Reads the current BER rate

//DIGITAL SERVICES
#define SI468X_CMD_GET_DIGITAL_SERVICE_LIST							0x80	//Gets a service list of the ensemble.
#define SI468X_CMD_START_DIGITAL_SERVICE							0x81	//Starts an audio or data service.
#define SI468X_CMD_STOP_DIGITAL_SERVICE								0x82	//Stops an audio or data service.
#define SI468X_CMD_GET_DIGITAL_SERVICE_DATA							0x84	//Gets a block of data associated with one of the enabled data components of a digital services.

//TEST
#define SI468X_CMD_TEST_GET_RSSI									0xE5	//Returns the reported RSSI in 8.8 format.


/************** PROPERTIES ADDRESSES ***************************************************************************************************************************************/

//INTERRUPT CONTROL (0x00)
#define SI468x_PROP_INT_CTL_ENABLE									0x0000	//Default: 0x0000. Interrupt enable property.
#define SI468x_PROP_INT_CTL_REPEAT									0x0001	//Default: 0x0000. Interrupt repeat property.

//DIGITAL IO OUTPUT (0x02)
#define SI468x_PROP_DIGITAL_IO_OUTPUT_SELECT						0x0200	//Default: 0x0000. Selects digital audio Master or Slave.
#define SI468x_PROP_DIGITAL_IO_OUTPUT_SAMPLE_RATE					0x0201	//Default: 0xBB80. Sets output audio sample rate in units of 1Hz
#define SI468x_PROP_DIGITAL_IO_OUTPUT_FORMAT						0x0202	//Default: 0x1800. Configure digital output format.
#define SI468x_PROP_DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_1			0x0203	//Default: 0x0000. Deviations from the standard framing mode.
#define SI468x_PROP_DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_2			0x0204	//Default: 0x0000. Deviations from the standard framing mode.
#define SI468x_PROP_DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_3			0x0205	//Default: 0x0000. Deviations from the standard framing mode.
#define SI468x_PROP_DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_4			0x0206	//Default: 0x0000. Deviations from the standard framing mode.

//AUDIO (0x03)
#define SI468x_PROP_AUDIO_ANALOG_VOLUME								0x0300	//Default: 0x003F. Sets the audio analog volume.
#define SI468x_PROP_AUDIO_MUTE										0x0301	//Default: 0x0000. Mutes/unmutes each audio output independently.
#define SI468x_PROP_AUDIO_OUTPUT_CONFIG								0x0302	//Default: 0x0000. Used to configure various settings of the audio output.

//PIN CONFIG (0x08)
#define SI468x_PROP_PIN_CONFIG_ENABLE								0x0800	//Default: 0x8001. Pin configuration property.

//WAKE TONE (0x09)
#define SI468x_PROP_WAKE_TONE_ENABLE								0x0900	//Default: 0x0000. Enables the wake tone feature.
#define SI468x_PROP_WAKE_TONE_PERIOD								0x0901	//Default: 0x00FA. Sets the wake tone duty cycle.
#define SI468x_PROP_WAKE_TONE_FREQ									0x0902	//Default: 0x02EE. Sets the wake tone frequency.
#define SI468x_PROP_WAKE_TONE_AMPLITUDE								0x0903	//Default: 0x0008. Sets the wake tone amplitude.

//FM TUNE FRONT END (0x17)
#define SI468x_PROP_FM_TUNE_FE_VARM									0x1710	//Default: 0x0000. FM Front End Varactor configuration slope
#define SI468x_PROP_FM_TUNE_FE_VARB									0x1711	//Default: 0x0000. FM Front End Varactor configuration intercept
#define SI468x_PROP_FM_TUNE_FE_CFG									0x1712	//Default: 0x0000. Additional configuration options for the front end.

//AGC (0x17)
#define SI468x_PROP_AGC_RF_THRESHOLD								0x170C	//Default: 0x0000. Adjusts the midpoint of threshold for the RF Threshold Detector.

//DAB TUNE FRONT END (0x17)
#define SI468x_PROP_DAB_TUNE_FE_VARM								0x1710	//Default: 0x0000. DAB/DMB Front End Varactor configuration slope
#define SI468x_PROP_DAB_TUNE_FE_VARB								0x1711	//Default: 0x0000. DAB/DMB Front End Varactor configuration intercept
#define SI468x_PROP_DAB_TUNE_FE_CFG									0x1712	//Default: 0x0001. Additional configuration options for the front end.

//FM SEEK (0x31)
#define SI468x_PROP_FM_SEEK_BAND_BOTTOM								0x3100	//Default: 0x222E. Sets the lower seek boundary of the FM band in multiples of 10 kHz.
#define SI468x_PROP_FM_SEEK_BAND_TOP								0x3101	//Default: 0x2A26. Sets the upper seek boundary of the FM band in multiples of 10 kHz.
#define SI468x_PROP_FM_SEEK_FREQUENCY_SPACING						0x3102	//Default: 0x000A. Sets the frequency spacing for the FM band in multiples of 10 kHz when performing a seek.

//FM VALID (0x32)
#define SI468x_PROP_FM_VALID_MAX_TUNE_ERROR							0x3200	//Default: 0x0072. Sets the maximum frequency error allowed before setting the AFCRL indicator.
#define SI468x_PROP_FM_VALID_RSSI_TIME								0x3201	//Default: 0x000F. Sets the amount of time in ms to allow the RSSI/ISSI metrics to settle before evaluating.
#define SI468x_PROP_FM_VALID_RSSI_THRESHOLD							0x3202	//Default: 0x0011. Sets the RSSI threshold for a valid FM Seek/Tune.
#define SI468x_PROP_FM_VALID_SNR_TIME								0x3203	//Default: 0x0028. Sets the amount of time in ms to allow the SNR metric to settle before evaluating.
#define SI468x_PROP_FM_VALID_SNR_THRESHOLD							0x3204	//Default: 0x000A. Sets the SNR threshold for a valid FM Seek/Tune.
#define SI468x_PROP_FM_VALID_HDLEVEL_THRESHOLD						0x3206	//Default: 0x0000. Sets the SNR threshold for a valid FM Seek/Tune.

//FM Received Signal Quality (0x33)
#define SI468x_PROP_FM_RSQ_INTERRUPT_SOURCE							0x3300	//Default: 0x0000. Configures interrupt related to Received Signal Quality metrics.
#define SI468x_PROP_FM_RSQ_SNR_HIGH_THRESHOLD						0x3301	//Default: 0x007F. Sets the high threshold, which triggers the RSQ interrupt if the SNR is above this threshold.
#define SI468x_PROP_FM_RSQ_SNR_LOW_THRESHOLD						0x3302	//Default: 0xFF80. Sets the low threshold, which triggers the RSQ interrupt if the SNR is below this threshold.
#define SI468x_PROP_FM_RSQ_RSSI_HIGH_THRESHOLD						0x3303	//Default: 0x007F. Sets the high threshold, which triggers the RSQ interrupt if the RSSI is above this threshold.
#define SI468x_PROP_FM_RSQ_RSSI_LOW_THRESHOLD						0x3304	//Default: 0xFF80. Sets the low threshold, which triggers the RSQ interrupt if the RSSI is below this threshold.
#define SI468x_PROP_FM_RSQ_HDLEVEL_HIGH_THRESHOLD					0x330A	//Default: 0x0064. Sets the high threshold, which triggers the RSQ interrupt if the HD level is above this threshold.
#define SI468x_PROP_FM_RSQ_HDLEVEL_LOW_THRESHOLD					0x330B	//Default: 0x0000. Sets the low threshold, which triggers the RSQ interrupt if the HD level is below this threshold.

//FM ACF (0x34)
#define SI468x_PROP_FM_ACF_INTERRUPT_SOURCE							0x3400	//Default: 0x0000. Enables the ACF interrupt sources.
#define SI468x_PROP_FM_ACF_SOFTMUTE_THRESHOLD						0x3401	//Default: 0x001F. Sets the softmute interrupt threshold in dB attenuation.
#define SI468x_PROP_FM_ACF_HIGHCUT_THRESHOLD						0x3402	//Default: 0x0000. Sets the high cut interrupt threshold.
#define SI468x_PROP_FM_ACF_BLEND_THRESHOLD							0x3403	//Default: 0x0000. Sets the stereo blend interrupt threshold.
#define SI468x_PROP_FM_ACF_SOFTMUTE_TOLERANCE						0x3404	//Default: 0x0002. Sets the distance from the final softmute value that triggers softmute convergence flag.
#define SI468x_PROP_FM_ACF_HIGHCUT_TOLERANCE						0x3405	//Default: 0x0014. Sets the distance from the final high cut value that triggers high cut convergence flag.
#define SI468x_PROP_FM_ACF_BLEND_TOLERANCE							0x3406	//Default: 0x0005. Sets the distance from the final blend state that triggers the blend convergence flag.

//FM SOFTMUTE (0x35)
#define SI468x_PROP_FM_SOFTMUTE_SNR_LIMITS							0x3500	//Default: 0xFEF4. Sets the SNR limits for soft mute attenuation.
#define SI468x_PROP_FM_SOFTMUTE_SNR_ATTENUATION						0x3501	//Default: 0x0008. Sets the attenuation limits for SNR based soft mute.
#define SI468x_PROP_FM_SOFTMUTE_SNR_ATTACK_TIME						0x3502	//Default: 0x0078. Sets the attack time for SNR based soft mute.
#define SI468x_PROP_FM_SOFTMUTE_SNR_RELEASE_TIME					0x3503	//Default: 0x01F4. Sets the release time for SNR based soft mute.

//FM HIGHCUT (0x36)
#define SI468x_PROP_FM_HIGHCUT_RSSI_LIMITS							0x3600	//Default: 0x1406. Sets the RSSI limits for high cut cutoff frequency.
#define SI468x_PROP_FM_HIGHCUT_RSSI_CUTOFF_FREQ						0x3601	//Default: 0x9628. Sets the cutoff frequency limits for RSSI based high cut.
#define SI468x_PROP_FM_HIGHCUT_RSSI_ATTACK_TIME						0x3602	//Default: 0x0010. Sets the attack time for RSSI based high cut.
#define SI468x_PROP_FM_HIGHCUT_RSSI_RELEASE_TIME					0x3603	//Default: 0x0FA0. Sets the release time for RSSI based high cut.
#define SI468x_PROP_FM_HIGHCUT_SNR_LIMITS							0x3604	//Default: 0x09FE. Sets the SNR limits for high cut cutoff frequency.
#define SI468x_PROP_FM_HIGHCUT_SNR_CUTOFF_FREQ						0x3605	//Default: 0x9628. Sets the cutoff frequency limits for SNR based high cut.
#define SI468x_PROP_FM_HIGHCUT_SNR_ATTACK_TIME						0x3606	//Default: 0x0010. Sets the attack time for SNR based high cut.
#define SI468x_PROP_FM_HIGHCUT_SNR_RELEASE_TIME						0x3607	//Default: 0x0FA0. Sets the release time for SNR based high cut.
#define SI468x_PROP_FM_HIGHCUT_MULTIPATH_LIMITS						0x3608	//Default: 0x2D3C. Sets the multipath limits for high cut cutoff frequency.
#define SI468x_PROP_FM_HIGHCUT_MULTIPATH_CUTOFF_FREQ				0x3609	//Default: 0x9628. Sets the cutoff frequency limits for multipath based high cut.
#define SI468x_PROP_FM_HIGHCUT_MULTIPATH_ATTACK_TIME				0x360A	//Default: 0x0010. Sets the attack time for multipath based high cut.
#define SI468x_PROP_FM_HIGHCUT_MULTIPATH_RELEASE_TIME				0x360B	//Default: 0x0FA0. Sets the release time for multipath based high cut.

//FM BLEND (0x37)
#define SI468x_PROP_FM_BLEND_RSSI_LIMITS							0x3700	//Default: 0x2B14. Sets the RSSI limits for blend stereo separation.
#define SI468x_PROP_FM_BLEND_RSSI_STEREO_SEP						0x3701	//Default: 0x2300. Sets the stereo separation limits for RSSI based blend.
#define SI468x_PROP_FM_BLEND_RSSI_ATTACK_TIME						0x3702	//Default: 0x0010. Sets the attack time for RSSI based blend.
#define SI468x_PROP_FM_BLEND_RSSI_RELEASE_TIME						0x3703	//Default: 0x0FA0. Sets the release time for RSSI based blend.
#define SI468x_PROP_FM_BLEND_SNR_LIMITS								0x3704	//Default: 0x1608. Sets the SNR limits for blend stereo separation.
#define SI468x_PROP_FM_BLEND_SNR_STEREO_SEP							0x3705	//Default: 0x2300. Sets the stereo separation limits for SNR based blend.
#define SI468x_PROP_FM_BLEND_SNR_ATTACK_TIME						0x3706	//Default: 0x0010. Sets the attack time for SNR based blend.
#define SI468x_PROP_FM_BLEND_SNR_RELEASE_TIME						0x3707	//Default: 0x0FA0. Sets the release time for SNR based blend.
#define SI468x_PROP_FM_BLEND_MULTIPATH_LIMITS						0x3708	//Default: 0x1E3C. Sets the release time for SNR based blend.
#define SI468x_PROP_FM_BLEND_MULTIPATH_STEREO_SEP					0x3709	//Default: 0x2300. Sets the stereo separation limits for multipath based blend.
#define SI468x_PROP_FM_BLEND_MULTIPATH_ATTACK_TIME					0x370A	//Default: 0x0010. Sets the attack time for multipath based blend.
#define SI468x_PROP_FM_BLEND_MULTIPATH_RELEASE_TIME					0x370B	//Default: 0x0FA0. Sets the release time for multipath based blend.

//FM AUDIO (0x39)
#define SI468x_PROP_FM_AUDIO_DE_EMPHASIS							0x3900	//Default: 0x0000. Sets the FM Receive de-emphasis.

//FM RDS (0x3C)
#define SI468x_PROP_FM_RDS_INTERRUPT_SOURCE							0x3C00	//Default: 0x0000. Configures interrupt related to RDS.
#define SI468x_PROP_FM_RDS_INTERRUPT_FIFO_COUNT						0x3C01	//Default: 0x0000. Configures minimum received data groups in fifo before interrupt.
#define SI468x_PROP_FM_RDS_CONFIG									0x3C02	//Default: 0x0000. Enables RDS and configures acceptable block error threshold.
#define SI468x_PROP_FM_RDS_CONFIDENCE								0x3C03	//Default: 0x1111. Configures rds block confidence threshold.

//DIGITAL SERVICE (0x81)
#define SI468x_PROP_DIGITAL_SERVICE_INT_SOURCE						0x8100	//Default: 0x0000. Configures the interrupt sources for digital services
#define SI468x_PROP_DIGITAL_SERVICE_RESTART_DELAY					0x8101	//Default: 0x00C8. Sets the delay time (in miliseconds) to restart digital service when recovering from acquisition loss

//HD BLEND (0x91)
#define SI468x_PROP_HD_BLEND_SERV_LOSS_NOISE_DAAI_THRESHOLD			0x9110	//Default: 0x0028. Sets the DAAI threshold below which comfort noise will engage.

//DAB DIGRAD (0xB0)
#define SI468x_PROP_DAB_DIGRAD_INTERRUPT_SOURCE						0xB000	//Default: 0x0000. Configures interrupts related to digital receiver.
#define SI468x_PROP_DAB_DIGRAD_RSSI_HIGH_THRESHOLD					0xB001	//Default: 0x007F. Sets the RSSI high threshold, which triggers the DIGRAD interrupt if the RSSI is above this threshold.
#define SI468x_PROP_DAB_DIGRAD_RSSI_LOW_THRESHOLD					0xB002	//Default: 0xFF80. Sets the RSSI low threshold, which triggers the DIGRAD interrupt if the RSSI is below this threshold.

//DAB VALID (0xB2)
#define SI468x_PROP_DAB_VALID_RSSI_TIME								0xB200	//Default: 0x001E. Sets the time to allow the RSSI metric to settle before evaluating.
#define SI468x_PROP_DAB_VALID_RSSI_THRESHOLD						0xB201	//Default: 0x000C. Sets the RSSI threshold for a valid DAB Seek/Tune
#define SI468x_PROP_DAB_VALID_ACQ_TIME								0xB202	//Default: 0x07D0. Set the time to wait for acquisition before evaluating
#define SI468x_PROP_DAB_VALID_SYNC_TIME								0xB203	//Default: 0x04B0. Sets the time to wait for ensemble synchronization.
#define SI468x_PROP_DAB_VALID_DETECT_TIME							0xB204	//Default: 0x0064. Sets the time to wait for ensemble fast signal detection
#define SI468x_PROP_DAB_VALID_FASTDETECT_DELAY						0xB205	//Default: 0x0032. Sets the time to delay before starting fast detect.

//DAB EVENT (0xB3)
#define SI468x_PROP_DAB_EVENT_INTERRUPT_SOURCE						0xB300	//Default: 0x0000. Configures which dab events will set the DEVENTINT status bit.
#define SI468x_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD					0xB301	//Default: 0x000A. Configures how often service list notifications can occur.
#define SI468x_PROP_DAB_EVENT_MIN_FREQINFO_PERIOD					0xB303	//Default: 0x0005. Configures how often frequency information notifications can occur.
#define SI468x_PROP_DAB_EVENT_AUDIO_DEMUTE							0xB304	//Default: 0x0014. Configures the audio demute interrupt delay.

//DAB (0xB4)
#define SI468x_PROP_DAB_XPAD_ENABLE									0xB400	//Default: 0x0001. Selects which XPAD data will forwarded to the host.
#define SI468x_PROP_DAB_DRC_OPTION									0xB401	//Default: 0x0000. Defines option to apply DRC (dynamic range control) gain.

//DAB ACF (0xB5)
#define SI468x_PROP_DAB_ACF_ENABLE									0xB500	//Default: 0x000B. Enables the feature of soft mute and comfort noise when signal level is low.
#define SI468x_PROP_DAB_ACF_MUTE_SIGLOSS_THRESHOLD					0xB501	//Default: 0x0006. Sets the threshold to mute audio when signal is loss.
#define SI468x_PROP_DAB_ACF_SOFTMUTE_BER_LIMITS						0xB503	//Default: 0xE2A6. Sets the BER limits when softmute engages.
#define SI468x_PROP_DAB_ACF_SOFTMUTE_ATTENUATION_LEVEL				0xB504	//Default: 0x0050. Sets audio attenuation level.
#define SI468x_PROP_DAB_ACF_SOFTMUTE_ATTACK_TIME					0xB505	//Default: 0x0064. Sets mute time in ms.
#define SI468x_PROP_DAB_ACF_SOFTMUTE_RELEASE_TIME					0xB506	//Default: 0x0FA0. Sets unmute time in ms.
#define SI468x_PROP_DAB_ACF_CMFTNOISE_BER_LIMITS					0xB507	//Default: 0xE2A6. Sets the BER limits when comfort noise engages.
#define SI468x_PROP_AB_ACF_CMFTNOISE_LEVEL							0xB508	//Default: 0x0200. Sets the comfort noise level.
#define SI468x_PROP_DAB_ACF_CMFTNOISE_ATTACK_TIME					0xB509	//Default: 0x0064. Sets comfort noise attack time in ms.
#define SI468x_PROP_DAB_ACF_CMFTNOISE_RELEASE_TIME					0xB50A	//Default: 0x0FA0. Sets comfort noise release time in ms.

//DAB ANNOUNCEMENT (0xB7)
#define SI468x_PROP_DAB_ANNOUNCEMENT_ENABLE							0xB700	//Default: 0x07FF. Enables announcement types.

//DAB FIB FORWARD (0xB8)
#define SI468x_PROP_DAB_FIB_FORWARD_FIB_FORWARD_INTERRUPT_SOURCE	0xB800	//Default: 0x0000. Configures interrupt related to DAB FIC data forwarding.
#define SI468x_PROP_DAB_FIB_FORWARD_FIB_FORWARD_OPTION				0xB801	//Default: 0x0040. Defines FIB forwarding options.

//DAB TEST (0xE8)
#define SI468x_PROP_DAB_TEST_BER_CONFIG								0xE800	//Default: 0x0000. Sets up and enables the DAB BER test.


/************** DAB CHANNELS [kHz] ************************************************************************************************************************************************/

#define DAB_CHANNEL_5B 	176640
#define DAB_CHANNEL_5C 	178352
#define DAB_CHANNEL_5D 	180064
#define DAB_CHANNEL_6A 	181936
#define DAB_CHANNEL_6B 	183648
#define DAB_CHANNEL_6C 	185360
#define DAB_CHANNEL_6D 	187072
#define DAB_CHANNEL_7A 	188928
#define DAB_CHANNEL_7B 	190640
#define DAB_CHANNEL_7C 	192352
#define DAB_CHANNEL_7D 	194064
#define DAB_CHANNEL_8A 	195936
#define DAB_CHANNEL_8B 	197648
#define DAB_CHANNEL_8C 	199360
#define DAB_CHANNEL_8D 	201072
#define DAB_CHANNEL_9A 	202928
#define DAB_CHANNEL_9B 	204640
#define DAB_CHANNEL_9C 	206352
#define DAB_CHANNEL_9D 	208064
#define DAB_CHANNEL_10A 209936
#define DAB_CHANNEL_10B 211648
#define DAB_CHANNEL_10C 213360
#define DAB_CHANNEL_10D 215072
#define DAB_CHANNEL_11A 216928
#define DAB_CHANNEL_11B 218640
#define DAB_CHANNEL_11C 220352
#define DAB_CHANNEL_11D 222064
#define DAB_CHANNEL_12A 223936
#define DAB_CHANNEL_12B 225648
#define DAB_CHANNEL_12C 227360
#define DAB_CHANNEL_12D 229072
#define DAB_CHANNEL_13A 230784
#define DAB_CHANNEL_13B 232496
#define DAB_CHANNEL_13C 234208
#define DAB_CHANNEL_13D 235776
#define DAB_CHANNEL_13E 237488
#define DAB_CHANNEL_13F 239200


#endif /* INC_SI468X_DEFS_H_ */
