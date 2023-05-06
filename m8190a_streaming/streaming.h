// See driver help topic "Initializing the IVI-COM Driver" for additional information

#import <GlobMgr.dll>				no_namespace    // VISA-COM I/O funtionality
#import <IviDriverTypeLib.dll>      no_namespace    // IVI inherent functionality
#import <AgM8190.dll>               no_namespace 

#include "stdafx.h"
#include "stream_buffer.h"

#include <iostream>
#include <fstream>

const int DEFAULT_INTERPOLATION_FACTOR_INDEX = 0;

const double DEFAULT_SAMPLE_FREQ = 1.0e9;		
const double DEFAULT_CARRIER_FREQ = 1.0e9;		
const double DEFAULT_TRIGGER_FREQ = 1.0;			

const uint DEFAULT_CIRCULAR_BUFFER_SIZE = 16;
const int DEFAULT_SEGMENT_SIZE = 6 * 1000 * 1000;
const int DEFAULT_FILE_BUFFER_SIZE = DEFAULT_SEGMENT_SIZE;

const int DEFAULT_NUM_SEGMENTS = 1024;

const int DEFAULT_REPEAT_COUNT = 5;

// Sequence table access
const uint InitMarkerSequence		= 0x10000000;
const uint EndMarkerScenario		= 0x20000000;
const uint EndMarkerSequence		= 0x40000000;
const uint MarkerEnable				= 0x01000000;
const uint FreqInc					= 0x00001000;
const uint FreqInit					= 0x00002000;
const uint ScaleInc					= 0x00004000;
const uint ScaleInit				= 0x00008000;
const uint SeqAdvMask				= 0x00F00000;
const uint SeqAdvShift				= 20;
const uint SegAdvMask				= 0x000F0000;
const uint SegAdvShift				= 16;
const uint CommandFlag				= 0x80000000;
const uint SegEndOffWholeSegment	= 0xffffffff;

const uint SequenceStateMask	= 0x00180000;
const uint CurrentSegmentMask	= 0x0007FFFF;

// Sequence state reporting
enum SequenceState {
	SequenceStateIdle		= 0x00000000,
	SequenceStateWaitTrig	= 0x00080000,
	SequenceStateRunning	= 0x00100000,
	SequenceStateWaitAdv	= 0x00180000
};

enum ExtSampleClock {
	Channel1,
	Channel2
};

enum RefClockSource {
	Internal,
	External
};

const long WaveformSizeMin = 120;
const long WaveformSizeMax = 0x30000000;

const uint RepeatCountMax = 0xFFFFFFFF;

static int NumSegments = DEFAULT_NUM_SEGMENTS;	// Number of streamed 
static int NumSegments1 = DEFAULT_NUM_SEGMENTS;	// Number of streamed 
static int NumSegments2 = DEFAULT_NUM_SEGMENTS;	// Number of streamed segments

static AgM8190BitResolutionModeEnum BitResolutionMode = AgM8190BitResolutionModeInterpolationX48;
static AgM8190BitResolutionModeEnum BitResolutionMode1 = AgM8190BitResolutionModeInterpolationX48;
static AgM8190BitResolutionModeEnum BitResolutionMode2 = AgM8190BitResolutionModeInterpolationX12;

static uint CircularBufferSize	= DEFAULT_CIRCULAR_BUFFER_SIZE;
static int SegmentSize			= DEFAULT_SEGMENT_SIZE;
static int FileBufferSize		= DEFAULT_FILE_BUFFER_SIZE;

static stream_buffer* streamBuffer1 = NULL;
static stream_buffer* streamBuffer2 = NULL;

static bool isContinuousMode = true;				

static bool infiniteRepeat = false;

static double SampleFreq	= DEFAULT_SAMPLE_FREQ;
static double CarrierFreq	= DEFAULT_CARRIER_FREQ;
static double TriggerFreq	= DEFAULT_TRIGGER_FREQ;

static uint RepeatCount = DEFAULT_REPEAT_COUNT;

static bool streamingInProggress = false;

static ExtSampleClock sampleClock = Channel2;
static RefClockSource refSource = Internal;

/* Channel 1 params */

static bool isChannel1Enabled = false;

static double central_freq_ch1 = DEFAULT_CARRIER_FREQ;
static double sample_rate_ch1 = DEFAULT_SAMPLE_FREQ;

static bool isChannel1Alg = true;

static std::wstring filename1 = L"";

/* Channel 2 params */

static bool isChannel2Enabled = false;

static double central_freq_ch2 = DEFAULT_CARRIER_FREQ;
static double sample_rate_ch2 = DEFAULT_SAMPLE_FREQ;

static bool isChannel2Alg = true;

static std::wstring filename2 = L"";

/* Other params */

static bool isTestMode = false;
static bool isDebugMode = true;

static std::wstring log_filename = L"debug_log.txt";
static std::ofstream log_stream;

static bool isLogFileOpened = false;

static int fileDescriptor1 = 0;
static int fileDescriptor2 = 0;

static BSTR channel1;
static BSTR channel2;

static CComSafeArray<int> segmentIds1;
static CComSafeArray<short> waveform1;

static CComSafeArray<int> segmentIds2;
static CComSafeArray<short> waveform2;

static HANDLE hThread;
static HANDLE hStopEvent;

static bool stop_streaming = false;


public class Streaming {

protected:

private:


public:
	IAgM8190Ex3Ptr driver;

	Streaming() {
		driver = IAgM8190Ex3Ptr(__uuidof(AgM8190));
	}

	~Streaming() {}

	void init_driver(std::wstring visa_address);

	void reset();

	void abort();

	void calculate_download_rate();

	void start_streaming();

	void prepare_alg_streaming();

	void calculate_segment_count(double g_bytes);

	void generate_waveform_data(std::vector<double>& data, int segmentSize);

	static void convert_to_dac(int segment, const std::vector<double>& data, LPSAFEARRAY* dacValues);

	static int download_waveform(IAgM8190Ex3Ptr driver, BSTR channel, int segmentId, int position, int length, LPSAFEARRAY* waveform);

	void set_data_entry(BSTR channel, uint index,
		bool initSeq, bool endSeq, bool endSce, uint seqAdv,
		uint segAdv, bool scaleInit, bool scaleInc,
		bool freqInit, bool freqInc, uint seqLoop, uint segLoop,
		uint segId, uint startOff, uint endOff);

	void start_alg_streaming(BSTR channel1, BSTR channel2, std::vector<double>& data, const CComSafeArray<int>& segmentIds1, const CComSafeArray<int>& segmentIds2);

	static uint __stdcall put_data(void* data);

	static uint __stdcall put_data_ch1(void* data);

	static uint __stdcall put_data_ch2(void* data);

	void error_queue();

	static void write_waveform_data(IAgM8190Ex3Ptr driver, BSTR channel1, BSTR channel2, const CComSafeArray<int>& segmentIds1, const CComSafeArray<int>& segmentIds2);

	static int num_segments_to_download(IAgM8190Ex3Ptr driver, BSTR channel, int downloadedSequenceEntry);

	static void get_sequence_state(IAgM8190Ex3Ptr driver, BSTR channel, SequenceState& state, uint& sequenceEntry);

	static uint next_entry(uint sequenceEntry);

	void prepare_file_streaming();

	void set_file_segment_count();

	void get_segment_from_file(int fileDescriptor, int segmentSize, LPSAFEARRAY* waveform);

	static bool read_file(int fileDescriptor, LPSAFEARRAY* readData, const size_t pos, const size_t size, size_t& readCount);

	static bool read_file(int fileDescriptor, char* readData, const size_t size, size_t& readCount);

	void start_file_streaming(int fileDescriptor1, int fileDescriptor2, BSTR channel1, BSTR channel2, const CComSafeArray<int>& segmentIds1, const CComSafeArray<int>& segmentIds2);

	static uint __stdcall read_data(void* data);

	static void send_log(const wchar_t* msg, ...);

	
	void set_instrument_params(int segment_size, int repeat_count, double sf);

	void set_channel_params(int channel_num, double cf, double sf, int interpolation, std::wstring filename);

	void set_bit_resolution_mode(int index);

	void set_num_segments(int num);

	void set_segment_size(int size);

	void set_trigger_freq(double trigger_freq);

	void set_repeat_count(int count);

	void set_buffer_num(int num);

	void set_enabled_ch1(bool state);

	void set_cf_ch1(double cf);

	void set_sf_ch1(double sf);

	void set_is_alg_ch1(bool is_alg);

	void set_filename_ch1(std::wstring filename);

	void set_enabled_ch2(bool state);

	void set_cf_ch2(double cf);

	void set_sf_ch2(double sf);

	void set_is_alg_ch2(bool is_alg);

	void set_filename_ch2(std::wstring filename);

	void set_external_sample_clock(int channel_num);

	void set_reference_clock_source(bool is_internal);

	bool get_is_alg_ch1();

	bool get_is_alg_ch2();

	static const std::wstring Bytes(uint64 bytes);

	static std::wstring ToString(double value, int precision = 1);

	static CString ToTimeString(clock_t span);

	static int GetInterpolationFactor(AgM8190BitResolutionModeEnum mode);

	static AgM8190BitResolutionModeEnum GetInterpolationMode(int factor);
};

