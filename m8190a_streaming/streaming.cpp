#include "streaming.h"

struct fileDescriptors {
	int desc_1;
	int desc_2;
};

void Streaming::init_driver(std::wstring visa_address) {
	try {
		send_log(L"Beginning of driver initialization");

		::CoInitialize(NULL);

		CString strInitOptions;

		if (visa_address._Equal(L"simulate"))
			strInitOptions = (L"QueryInstrStatus=true, Simulate=true, DriverSetup= Model=, Trace=false");
		else
			strInitOptions = (L"QueryInstrStatus=true, Simulate=false, DriverSetup= Model=, Trace=false");

		VARIANT_BOOL id_query = VARIANT_TRUE;
		VARIANT_BOOL reset = VARIANT_TRUE;

		driver->Initialize(LPCTSTR(visa_address.c_str()), id_query, reset, LPCTSTR(strInitOptions));
		driver->Utility->Reset();

		send_log(L"Driver initialized");

		calculate_download_rate();
	} catch (_com_error& e) {
		send_log(e.Description(), e.ErrorMessage(), MB_ICONERROR);
	} catch (CAtlException& ex) {
		_com_error e(ex.m_hr);
		send_log(e.Description(), e.ErrorMessage(), MB_ICONERROR);
	}
}

void Streaming::reset() {
	driver->Utility->Reset();
}

void Streaming::abort() {
	stop_streaming = true;
}

void Streaming::calculate_download_rate() {
	try {
		send_log(L"Calculating download rate...");

		driver->Utility->Reset();

		BSTR channel = ::SysAllocString(OLESTR("1"));
		
		int segment = 0;

		driver->Arbitrary3->PutBitResolutionMode(channel, BitResolutionMode);
		driver->Output2->ReferenceClockSource = AgM8190ReferenceClockSourceInternal;

		int offset = 0;
		CComSafeArray<short> wfData(SegmentSize * 2);

		calculate_segment_count(5.0);

		NumSegments = (int)(NumSegments - CircularBufferSize);

		clock_t startTime = clock();
		send_log(L"Start time: %ls", LPCTSTR(ToTimeString((double)(startTime) / CLOCKS_PER_SEC)));

		for (int index = 0; index < NumSegments; ++index) {
			segment = download_waveform(driver, channel, segment, offset, SegmentSize, wfData.GetSafeArrayPtr());
			offset = 0;
		}

		clock_t endTime = clock();
		send_log(L"End time: %ls", LPCTSTR(ToTimeString((double)(endTime) / CLOCKS_PER_SEC)));

		double elapsedTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
		send_log(L"Elapsed time: %ls", LPCTSTR(ToTimeString(elapsedTime)));

		uint64 transferredBytes = (uint64)SegmentSize * NumSegments * 4;
		send_log(L"Transferred bytes: %d", transferredBytes);

		double downloadRate = transferredBytes / elapsedTime;

		send_log(L"Download rate: %ls/s", Bytes((uint64)downloadRate).c_str());

		wfData.Destroy();

		::SysFreeString(channel);
	} catch (_com_error& ex) {
		throw ex;
	}
}

void Streaming::start_streaming() {
	streamingInProggress = true;
	stop_streaming = false;

	if (isChannel1Alg) prepare_alg_streaming();
	else prepare_file_streaming();
}

void Streaming::prepare_alg_streaming() {
	send_log(L"Beginning of streaming");

	uint swBufferSize = 0;
	swBufferSize = CircularBufferSize;

	if (isChannel1Enabled) streamBuffer1 = new stream_buffer(SegmentSize, swBufferSize);
	if (isChannel2Enabled) streamBuffer2 = new stream_buffer(SegmentSize, swBufferSize);

	std::vector<double> wf_data;
	generate_waveform_data(wf_data, SegmentSize * 2);

	CComSafeArray<short> dacValues;
	dacValues.Create(SegmentSize * 2);

	try {
		driver->Utility->Reset();

		if (refSource == Internal) {
			driver->Output2->ReferenceClockSource = AgM8190ReferenceClockSourceInternal;

			send_log(L"Clock source = Internal");
		} else if (refSource == External) {
			driver->Output2->ReferenceClockSource = AgM8190ReferenceClockSourceExternal;
			driver->Output2->ExternalReferenceClockFrequency = 10e6;

			send_log(L"Clock source = External (10 MHz)");
		}

		if (isChannel1Enabled && !isChannel2Enabled) {
			driver->Instrument2->ChannelCouplingEnabled = AgM8190ChannelCouplingStateEnum::AgM8190ChannelCouplingStateOn;
			send_log(L"Cooupling state = on");
		} else {
			driver->Instrument2->ChannelCouplingEnabled = AgM8190ChannelCouplingStateEnum::AgM8190ChannelCouplingStateOff;
			send_log(L"Cooupling state = off");
		}

		set_file_segment_count();
		send_log(L"Setted file segment count");

		uint segAdv = isContinuousMode ? 0 : (uint)3;

		if (isChannel1Enabled) channel1 = ::SysAllocString(OLESTR("1"));
		if (isChannel2Enabled) channel2 = ::SysAllocString(OLESTR("2"));

		if (isChannel1Enabled && !isChannel2Enabled) {
			if (isChannel1Enabled) {
				double sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch1));
				driver->Arbitrary->SampleRate = sampleFreq;
			}
			if (isChannel2Enabled) {
				double sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch2));
				driver->Arbitrary->SampleRate = sampleFreq;
			}
		} else {
			if (sampleClock == Channel2) {
				if (isChannel2Enabled) driver->SampleClock->PutSampleClockSource(channel2, AgM8190SampleClockSourceExternal);

				double sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch2));
				driver->Arbitrary3->SampleRateExternal = sampleFreq;

				sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch1));
				driver->Arbitrary->SampleRate = sampleFreq;

			} else if (sampleClock == Channel1) {
				if (isChannel1Enabled) driver->SampleClock->PutSampleClockSource(channel1, AgM8190SampleClockSourceExternal);

				double sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch1));
				driver->Arbitrary3->SampleRateExternal = sampleFreq;

				sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch2));
				driver->Arbitrary->SampleRate = sampleFreq;
			}
		}


		if (isChannel2Enabled) {
			send_log(L"Channel %d: Filename = %ls", 2, filename2.c_str());
			_wsopen_s(&fileDescriptor2, filename2.c_str(), _O_RDONLY | _O_BINARY, _SH_DENYNO, 0);

			double carFreqInt = floor(central_freq_ch2);
			double carFreqFrac = (central_freq_ch2)-carFreqInt;
			driver->Arbitrary3->DigitalUpConversion2->SetCarrierFrequency(channel2, carFreqInt, carFreqFrac);

			driver->Arbitrary3->PutBitResolutionMode(channel2, BitResolutionMode2);
			send_log(L"Channel 2: Interpolation factor = %d", GetInterpolationFactor(BitResolutionMode2));

			driver->Arbitrary3->put_DACFormat(channel2, AgM8190FormatDoublet);

			if (isContinuousMode) {
				driver->Trigger3->PutTriggerMode(channel2, AgM8190TriggerModeAuto);
			} else {
				driver->Trigger3->PutTriggerMode(channel2, AgM8190TriggerModeTriggered);

				driver->Trigger3->TriggerSource = AgM8190TriggerSourceInternal;
				driver->Trigger3->AdvancementEventSourceEx = AgM8190EventSourceInternal;
				driver->Trigger3->InternalRate = TriggerFreq;
			}

			send_log(L"Channel 2: Trigger settings are setted");

			driver->Output2->PutEnabled(channel2, true);
			driver->AbortGeneration(channel2);

			segmentIds2.Create(CircularBufferSize);

			send_log(L"Channel 2: Waveform array created");

			for (int i = 0; i < (int)CircularBufferSize; ++i) {
				convert_to_dac((int)(i + 1), wf_data, dacValues.GetSafeArrayPtr());
				segmentIds2[i] = download_waveform(driver, channel2, 0, 0, SegmentSize, dacValues.GetSafeArrayPtr());
				set_data_entry(channel2, i, false, false, false, 0, segAdv, false, false, false, false, 1, 1, (uint)segmentIds2[i], 0, SegEndOffWholeSegment);
			}

			send_log(L"Channel 2: Setting data entries");

			set_data_entry(channel2, 0, true, false, false, 0, segAdv, false, false, false, false, 1, 1, (uint)segmentIds2[0], 0, SegEndOffWholeSegment);
			set_data_entry(channel2, CircularBufferSize - 1, false, true, false, 0, 0, false, false, false, false, 1, 1, (uint)segmentIds2[(int)(CircularBufferSize - 1)], 0, SegEndOffWholeSegment);
		}

		if (isChannel1Enabled) {
			send_log(L"Channel %d: Filename = %ls", 1, filename1.c_str());
			_wsopen_s(&fileDescriptor1, filename1.c_str(), _O_RDONLY | _O_BINARY, _SH_DENYNO, 0);

			double carFreqInt = floor(central_freq_ch1);
			double carFreqFrac = (central_freq_ch1)-carFreqInt;
			driver->Arbitrary3->DigitalUpConversion2->SetCarrierFrequency(channel1, carFreqInt, carFreqFrac);

			driver->Arbitrary3->PutBitResolutionMode(channel1, BitResolutionMode1);
			send_log(L"Channel 1: Interpolation factor = %d", GetInterpolationFactor(BitResolutionMode1));

			driver->Arbitrary3->put_DACFormat(channel1, AgM8190FormatDoublet);

			if (isContinuousMode) {
				driver->Trigger3->PutTriggerMode(channel1, AgM8190TriggerModeAuto);
			} else {
				driver->Trigger3->PutTriggerMode(channel1, AgM8190TriggerModeTriggered);
			}

			if (isContinuousMode && !isChannel2Enabled) {
				driver->Trigger3->TriggerSource = AgM8190TriggerSourceInternal;
				driver->Trigger3->AdvancementEventSourceEx = AgM8190EventSourceInternal;
				driver->Trigger3->InternalRate = TriggerFreq;
			}

			send_log(L"Channel 1: Trigger settings are setted");

			driver->Output2->PutEnabled(channel1, true);
			driver->AbortGeneration(channel1);

			segmentIds1.Create(CircularBufferSize);

			send_log(L"Channel 1: Waveform array created");

			for (int i = 0; i < (int)CircularBufferSize; ++i) {
				convert_to_dac((int)(i + 1), wf_data, dacValues.GetSafeArrayPtr());
				segmentIds1[i] = download_waveform(driver, channel1, 0, 0, SegmentSize, dacValues.GetSafeArrayPtr());
				set_data_entry(channel1, i, false, false, false, 0, segAdv, false, false, false, false, 1, 1, (uint)segmentIds1[i], 0, SegEndOffWholeSegment);
			}

			send_log(L"Channel 1: Setting data entries");

			set_data_entry(channel1, 0, true, false, false, 0, segAdv, false, false, false, false, 1, 1, (uint)segmentIds1[0], 0, SegEndOffWholeSegment);
			set_data_entry(channel1, CircularBufferSize - 1, false, true, false, 0, 0, false, false, false, false, 1, 1, (uint)segmentIds1[(int)(CircularBufferSize - 1)], 0, SegEndOffWholeSegment);

		}

		send_log(L"Instrument is ready");

		start_alg_streaming(channel1, channel2, wf_data, segmentIds1, segmentIds2);

		dacValues.Destroy();

		send_log(L"Cleanig memory");

		if (isChannel1Enabled) {
			::SysFreeString(channel1);

			delete streamBuffer1;

			segmentIds1.Destroy();
		}

		if (isChannel2Enabled) {
			::SysFreeString(channel2);

			delete streamBuffer2;

			segmentIds2.Destroy();
		}

	} catch (_com_error& ex) {
		LPCTSTR error = ex.ErrorMessage();

		send_log(L"Error: %ls", error);
		send_log(L"Error description: %ls", ex.Description().GetBSTR());
	} catch (CAtlException& ex) {
		_com_error err(ex.m_hr);
		error_queue();
	} catch (std::exception& ex) {
		error_queue();
	}
}

void Streaming::calculate_segment_count(double g_bytes) {
	uint64 max_bytes = (uint64)(g_bytes * 1024L * 1024L * 1024L);
	uint64 segment_bytes = SegmentSize * 4;

	NumSegments = (int)(max_bytes / segment_bytes);

	if (max_bytes % segment_bytes != 0) NumSegments++;

	NumSegments += (int)CircularBufferSize;
}

void Streaming::generate_waveform_data(std::vector<double>& data, int segmentSize) {
	if (data.size() != segmentSize) data.resize(segmentSize);

	const double PI = 3.1415926535897932;

	const double f0 = 1000e3;

	const double fd = SampleFreq;
	const double td = 1 / fd;


	for (int i = 0; i < segmentSize; ++i) {
		data[i] = sin(2 * PI * f0 * i * td);
	}
}

void Streaming::convert_to_dac(int segment, const std::vector<double>& data, LPSAFEARRAY* dacValues) {
	short* pData = NULL;
	HRESULT hr = SafeArrayAccessData(*dacValues, (void**)&pData);

	double ampMultiplier = (double)(segment - 1) / NumSegments;
	//double factor = 8191;
	double factor = 16383;		//2 ^ (15 - 1) - 1										
	//double factor = ampMultiplier * 8191;

	int dataIndex = 0;
	short max_dac = 0;
	for (int i = 0; i < SegmentSize; ++i) {
		short sampleI = (short)(data[i] * factor);
		sampleI <<= 1;

		if (sampleI > max_dac) max_dac = sampleI;

		short sampleQ = sampleI;

		if (i == 0) {
			// Set sync and sample marker bits in first sample pair.
			sampleI |= 0x0003;
			sampleQ |= 0x0003;
		}

		pData[dataIndex++] = sampleI;
		pData[dataIndex++] = sampleQ;
	}


	SafeArrayUnaccessData(*dacValues);
}

int Streaming::download_waveform(IAgM8190Ex3Ptr driver, BSTR channel, int segmentId, int position, int length, LPSAFEARRAY* waveform) {
	int ret = driver->Arbitrary3->Waveform3->CreateChannelWaveformChunkInt16(channel, segmentId, position, length, waveform);
	
	return ret;
}

void Streaming::set_data_entry(BSTR channel, uint index, bool initSeq, bool endSeq, bool endSce, uint seqAdv, uint segAdv, bool scaleInit, bool scaleInc, bool freqInit, bool freqInc, uint seqLoop, uint segLoop, uint segId, uint startOff, uint endOff) {
	uint control = 0;
	control |= MarkerEnable;

	if (initSeq) control |= InitMarkerSequence;
	if (endSeq) control |= EndMarkerSequence;
	if (endSce) control |= EndMarkerScenario;
	if (scaleInit) control |= ScaleInit;
	if (scaleInc) control |= ScaleInc;
	if (freqInit) control |= FreqInit;
	if (freqInc) control |= FreqInc;

	control &= ~SeqAdvMask;
	control |= (seqAdv << (int)SeqAdvShift);
	control &= ~SegAdvMask;
	control |= (segAdv << (int)SegAdvShift);

	CComSafeArray<int> Data;
	Data.Create(6);

	Data[0] = control;  // Control
	Data[1] = seqLoop;  // Sequence Loop Count
	Data[2] = segLoop;  // Segment Loop Count
	Data[3] = segId;    // Segment ID
	Data[4] = startOff; // Segment Start Offset
	Data[5] = endOff;   // Segment End Offset

	driver->Arbitrary3->Sequence3->SequenceTable->SetData(channel, (int)index, Data.GetSafeArrayPtr());

	Data.Destroy();
}

void Streaming::start_alg_streaming(BSTR channel1, BSTR channel2, std::vector<double>& data, const CComSafeArray<int>& segmentIds1, const CComSafeArray<int>& segmentIds2) {
	try {
		send_log(L"Starting algorithmic streaming");

		uint threadID = 0;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, put_data, reinterpret_cast<void*>(&data), 0, &threadID);

		Sleep(0);	//TODO: is it necessary?

		if (isChannel1Enabled) {
			driver->Arbitrary3->PutSequencingMode(channel1, AgM8190SequencingModeSTSequence);
			driver->Arbitrary3->Sequence3->PutDynamicModeEnabled(channel1, true);
			driver->Arbitrary3->Sequence3->PutStreamingEnabled(channel1, true);

			driver->InitiateGeneration(channel1);
		}

		if (isChannel2Enabled) {
			driver->Arbitrary3->PutSequencingMode(channel2, AgM8190SequencingModeSTSequence);
			driver->Arbitrary3->Sequence3->PutDynamicModeEnabled(channel2, true);
			driver->Arbitrary3->Sequence3->PutStreamingEnabled(channel2, true);

			driver->InitiateGeneration(channel2);
		}

		error_queue();																		// Check for errors.

		write_waveform_data( driver, channel1, channel2, segmentIds1, segmentIds2);				// Download waveform data

		WaitForSingleObject(hThread, INFINITE);												// Wait for data thread to complete
		CloseHandle(hThread);																// Destroy the thread object.

		if (isChannel1Enabled) driver->AbortGeneration(channel1);
		if (isChannel2Enabled) driver->AbortGeneration(channel2);												// Stop signal generation.
	} catch (_com_error& ex) {
		throw ex;
	}
}

void Streaming::error_queue() {
	long errorNum = -1;
	_bstr_t bstrErrorMsg;
	while (errorNum != 0) {
		driver->Utility->ErrorQuery(&errorNum, bstrErrorMsg.GetAddress());

		send_log(L"ErrorQuery: %s, %ls", errorNum, bstrErrorMsg.GetBSTR());
	}
}

void Streaming::write_waveform_data(IAgM8190Ex3Ptr driver, BSTR channel1, BSTR channel2, const CComSafeArray<int>& segmentIds1, const CComSafeArray<int>& segmentIds2) {
	int segmentIndex1 = (int)CircularBufferSize;
	uint downloadedSequenceEntry1 = CircularBufferSize - 1;	// Remember last downloaded entry. It is the last item in the circular buffer. 

	int numSegToDownload1;									// Number of segments to be downloaded
	long downloadedSegments1 = 0;

	int segmentIndex2 = (int)CircularBufferSize;
	uint downloadedSequenceEntry2 = CircularBufferSize - 1;	// Remember last downloaded entry. It is the last item in the circular buffer. 

	int numSegToDownload2;									// Number of segments to be downloaded
	long downloadedSegments2 = 0;

	bool measStarted = false;								// Indicates that performance measurement has been started.
	clock_t startTime = clock();

	int offset = 0;

	int segmentError = (int)CircularBufferSize;
	bool streamingError = false;

	bool isZero = false;

	try {
		uint repeat = 0;

		send_log(L"Entering in cycle (repeat count = %d)", RepeatCount);
		for (; repeat < RepeatCount; ++repeat) {
			if (stop_streaming) {
				streamingInProggress = false;
				return;
			}

			while (segmentIndex1 < NumSegments1 && segmentIndex2 < NumSegments2) {

				if (isChannel1Enabled) {
					// Calculate number of segments that can be downloaded in the next iteration.
					numSegToDownload1 = num_segments_to_download(driver, channel1, (int)downloadedSequenceEntry1);

					// Download number of segments.
					for (int j = 0; j < numSegToDownload1 && segmentIndex1 < NumSegments1; ++j) {
						// Advance index in the circular buffer.
						downloadedSequenceEntry1 = next_entry(downloadedSequenceEntry1);

						while (streamBuffer1->Count() <= 0) {
							if (stop_streaming) {
								return;
							}

							Sleep(0);
						}

						if (!measStarted) {
							send_log(L"time measure is started");

							measStarted = true;
							startTime = clock();
						}

						streamBuffer1->IsReading(true);
						download_waveform(driver, channel1, segmentIds1[(int)downloadedSequenceEntry1], offset, SegmentSize, streamBuffer1->GetReadBuffer());
						streamBuffer1->IsReading(false);

						segmentIndex1++;
						downloadedSegments1++;
						offset = 0;

						send_log(L"Channel 1: Segment index = %d", segmentIndex1);

						if (segmentIndex1 % segmentError == 0 || segmentIndex1 == NumSegments1) {
							VARIANT_BOOL status = VARIANT_TRUE;
							driver->Status3->get_SequenceStreamingDataOK(channel1, &status);
							if (status != VARIANT_TRUE) {
								streamingError = true;

								throw std::exception("Channel 1: Data error while streaming. Download rate is less than the required rate.");
							}
						}

						//segmentIndex1 = 0;
					}

				}

				if (isChannel2Enabled) {
					numSegToDownload2 = num_segments_to_download(driver, channel2, (int)downloadedSequenceEntry2);

					for (int j = 0; j < numSegToDownload2 && segmentIndex2 < NumSegments2; ++j) {

						// Advance index in the circular buffer.
						downloadedSequenceEntry2 = next_entry(downloadedSequenceEntry2);

						while (streamBuffer2->Count() <= 0) {
							if (stop_streaming) {
								return;
							}

							Sleep(0);
						}

						if (!measStarted) {
							send_log(L"time measure is started");

							measStarted = true;
							startTime = clock();
						}

						streamBuffer2->IsReading(true);
						download_waveform(driver, channel2, segmentIds2[(int)downloadedSequenceEntry2], offset, SegmentSize, streamBuffer2->GetReadBuffer());
						streamBuffer2->IsReading(false);

						segmentIndex2++;
						downloadedSegments2++;
						offset = 0;

						send_log(L"Channel 2: Segment index = %d", segmentIndex2);

						if (segmentIndex2 % segmentError == 0 || segmentIndex2 == NumSegments2) {
							VARIANT_BOOL status = VARIANT_TRUE;
							driver->Status3->get_SequenceStreamingDataOK(channel2, &status);
							if (status != VARIANT_TRUE) {
								streamingError = true;

								throw std::exception("Channel 2: Data error while streaming. Download rate is less than the required rate.");
							}
						}

						//segmentIndex2 = 0;
					}

				}
			}

			segmentIndex1 = 0;			
			segmentIndex2 = 0;			

			if (infiniteRepeat) {
				repeat = -1;
			}

		}

		send_log(L"Streaming finished");

		if (isChannel1Enabled) {
			clock_t elapsedTime = clock() - startTime;
			double elapsedSeconds = (double)elapsedTime / CLOCKS_PER_SEC;
			uint64 transferredBytes = (uint64)SegmentSize * downloadedSegments1 * 4;
			double downloadRate = transferredBytes / elapsedSeconds;

			send_log(L"Channel 1:");
			send_log(L"Transferred bytes:  %ls", Bytes(transferredBytes).c_str());
			send_log(L"Elapsed time:       %ls", LPCTSTR(ToTimeString(elapsedTime)));
			send_log(L"Download rate:      %ls/s", Bytes((uint64)(downloadRate)).c_str());

			if (isContinuousMode) {
				send_log(L"Required rate:      %ls/s", Bytes((uint64)(SampleFreq * 2 / GetInterpolationFactor(BitResolutionMode1))).c_str());
			}
		}

		if (isChannel2Enabled) {
			clock_t elapsedTime = clock() - startTime;
			double elapsedSeconds = (double)elapsedTime / CLOCKS_PER_SEC;
			uint64 transferredBytes = (uint64)SegmentSize * downloadedSegments2 * 4;
			double downloadRate = transferredBytes / elapsedSeconds;

			send_log(L"Channel 2:");
			send_log(L"Transferred bytes:  %ls", Bytes(transferredBytes).c_str());
			send_log(L"Elapsed time:       %ls", LPCTSTR(ToTimeString(elapsedTime)));
			send_log(L"Download rate:      %ls/s", Bytes((uint64)(downloadRate)).c_str());

			if (isContinuousMode) {
				send_log(L"Required rate:      %ls/s", Bytes((uint64)(SampleFreq * 2 / GetInterpolationFactor(BitResolutionMode1))).c_str());
			}
		}
	} catch (_com_error& ex) {
		send_log(ex.ErrorMessage());
	} catch (CAtlException& ex) {
		_com_error err(ex.m_hr);
		send_log(err.ErrorMessage());
	} catch (std::exception& ex) {
		send_log(CA2W(ex.what()).operator LPWSTR());
	}
}

int Streaming::num_segments_to_download(IAgM8190Ex3Ptr driver, BSTR channel, int downloadedSequenceEntry) {
	SequenceState sequenceState;
	uint executedSequenceEntry;

	get_sequence_state(driver, channel, sequenceState, executedSequenceEntry);

	//send_log(L"downloaded = %d, executed = %d", downloadedSequenceEntry, (int)executedSequenceEntry);

	int numSegments;
	if (downloadedSequenceEntry < (int)executedSequenceEntry) {
		numSegments = (int)executedSequenceEntry - downloadedSequenceEntry - 1;
	} else {
		numSegments = (int)executedSequenceEntry + (int)CircularBufferSize - 1 - downloadedSequenceEntry;
	}

	return numSegments;
}

void Streaming::get_sequence_state(IAgM8190Ex3Ptr driver, BSTR channel, SequenceState& state, uint& sequenceEntry) {
	long response = 0;

	driver->Arbitrary3->Sequence3->SequenceTable2->get_SequenceState(channel, &response);
	state = (SequenceState)(response & SequenceStateMask);
	sequenceEntry = (uint)(response & CurrentSegmentMask);
}

uint Streaming::next_entry(uint sequenceEntry) {
	return (sequenceEntry + 1) % CircularBufferSize;
}

void Streaming::prepare_file_streaming() {
	send_log(L"Beginning of streaming");

	uint swBufferSize = 0;
	swBufferSize = CircularBufferSize;

	if (isChannel1Enabled) streamBuffer1 = new stream_buffer(SegmentSize, swBufferSize);
	if (isChannel2Enabled) streamBuffer2 = new stream_buffer(SegmentSize, swBufferSize);

	try {
		driver->Utility->Reset();

		if (refSource == Internal) {
			driver->Output2->ReferenceClockSource = AgM8190ReferenceClockSourceInternal;

			send_log(L"Clock source = Internal");
		} else if (refSource == External) {
			driver->Output2->ReferenceClockSource = AgM8190ReferenceClockSourceExternal;
			driver->Output2->ExternalReferenceClockFrequency = 10e6;

			send_log(L"Clock source = External (10 MHz)");
		}

		if (isChannel1Enabled && !isChannel2Enabled) {
			driver->Instrument2->ChannelCouplingEnabled = AgM8190ChannelCouplingStateEnum::AgM8190ChannelCouplingStateOn;
			send_log(L"Cooupling state = on");
		} else {
			driver->Instrument2->ChannelCouplingEnabled = AgM8190ChannelCouplingStateEnum::AgM8190ChannelCouplingStateOff;
			send_log(L"Cooupling state = off");
		}

		set_file_segment_count();
		send_log(L"Setted file segment count");

		uint segAdv = isContinuousMode ? 0 : (uint)3;

		if (isChannel1Enabled) channel1 = ::SysAllocString(OLESTR("1"));
		if (isChannel2Enabled) channel2 = ::SysAllocString(OLESTR("2"));
		
		if (isChannel1Enabled && !isChannel2Enabled) {
			if (isChannel1Enabled) {
				double sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch1));
				driver->Arbitrary->SampleRate = sampleFreq;
			}
			if (isChannel2Enabled) {
				double sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch2));
				driver->Arbitrary->SampleRate = sampleFreq;
			}
		} else {
			if (sampleClock == Channel2) {
				if (isChannel2Enabled) driver->SampleClock->PutSampleClockSource(channel2, AgM8190SampleClockSourceExternal);

				double sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch2));
				driver->Arbitrary3->SampleRateExternal = sampleFreq;

				sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch1));
				driver->Arbitrary->SampleRate = sampleFreq;

			} else if (sampleClock == Channel1) {
				if (isChannel1Enabled) driver->SampleClock->PutSampleClockSource(channel1, AgM8190SampleClockSourceExternal);

				double sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch1));
				driver->Arbitrary3->SampleRateExternal = sampleFreq;

				sampleFreq = max(driver->Arbitrary->GetSampleRateMin(), min(driver->Arbitrary->GetSampleRateMax(), sample_rate_ch2));
				driver->Arbitrary->SampleRate = sampleFreq;
			}
		}
		

		if (isChannel2Enabled) {
			send_log(L"Channel %d: Filename = %ls", 2, filename2.c_str());
			_wsopen_s(&fileDescriptor2, filename2.c_str(), _O_RDONLY | _O_BINARY, _SH_DENYNO, 0);

			double carFreqInt = floor(central_freq_ch2);
			double carFreqFrac = (central_freq_ch2)-carFreqInt;
			driver->Arbitrary3->DigitalUpConversion2->SetCarrierFrequency(channel2, carFreqInt, carFreqFrac);

			driver->Arbitrary3->PutBitResolutionMode(channel2, BitResolutionMode2);
			send_log(L"Channel 2: Interpolation factor = %d", GetInterpolationFactor(BitResolutionMode2));

			driver->Arbitrary3->put_DACFormat(channel2, AgM8190FormatDoublet);

			if (isContinuousMode) {
				driver->Trigger3->PutTriggerMode(channel2, AgM8190TriggerModeAuto);
			} else {
				driver->Trigger3->PutTriggerMode(channel2, AgM8190TriggerModeTriggered); 
				
				driver->Trigger3->TriggerSource = AgM8190TriggerSourceInternal;
				driver->Trigger3->AdvancementEventSourceEx = AgM8190EventSourceInternal;
				driver->Trigger3->InternalRate = TriggerFreq;
			}

			send_log(L"Channel 2: Trigger settings are setted");

			driver->Output2->PutEnabled(channel2, true);
			driver->AbortGeneration(channel2);

			segmentIds2.Create(CircularBufferSize);
			waveform2.Create(SegmentSize * 2);

			send_log(L"Channel 2: Waveform array created");

			for (int i = 0; i < (int)CircularBufferSize; ++i) {
				get_segment_from_file(fileDescriptor2, SegmentSize, waveform2.GetSafeArrayPtr());
				segmentIds2[i] = download_waveform(driver, channel2, 0, 0, SegmentSize, waveform2.GetSafeArrayPtr());
				set_data_entry(channel2, i, false, false, false, 0, segAdv, false, false, false, false, 1, 1, (uint)segmentIds2[i], 0, SegEndOffWholeSegment);
			}

			send_log(L"Channel 2: Waveform loaded");

			waveform2.Destroy();

			send_log(L"Channel 2: Setting data entries");

			set_data_entry(channel2, 0, true, false, false, 0, segAdv, false, false, false, false, 1, 1, (uint)segmentIds2[0], 0, SegEndOffWholeSegment);
			set_data_entry(channel2, CircularBufferSize - 1, false, true, false, 0, 0, false, false, false, false, 1, 1, (uint)segmentIds2[(int)(CircularBufferSize - 1)], 0, SegEndOffWholeSegment);
		}

		if (isChannel1Enabled) {
			send_log(L"Channel %d: Filename = %ls", 1, filename1.c_str());
			_wsopen_s(&fileDescriptor1, filename1.c_str(), _O_RDONLY | _O_BINARY, _SH_DENYNO, 0);

			double carFreqInt = floor(central_freq_ch1);
			double carFreqFrac = (central_freq_ch1)-carFreqInt;
			driver->Arbitrary3->DigitalUpConversion2->SetCarrierFrequency(channel1, carFreqInt, carFreqFrac);

			driver->Arbitrary3->PutBitResolutionMode(channel1, BitResolutionMode1);
			send_log(L"Channel 1: Interpolation factor = %d", GetInterpolationFactor(BitResolutionMode1));

			driver->Arbitrary3->put_DACFormat(channel1, AgM8190FormatDoublet);

			if (isContinuousMode) {
				driver->Trigger3->PutTriggerMode(channel1, AgM8190TriggerModeAuto);
			} else {
				driver->Trigger3->PutTriggerMode(channel1, AgM8190TriggerModeTriggered);
			}

			if (isContinuousMode && !isChannel2Enabled) {
				driver->Trigger3->TriggerSource = AgM8190TriggerSourceInternal;
				driver->Trigger3->AdvancementEventSourceEx = AgM8190EventSourceInternal;
				driver->Trigger3->InternalRate = TriggerFreq;
			}

			send_log(L"Channel 1: Trigger settings are setted");

			driver->Output2->PutEnabled(channel1, true);
			driver->AbortGeneration(channel1);

			segmentIds1.Create(CircularBufferSize);
			waveform1.Create(SegmentSize * 2);

			send_log(L"Channel 1: Waveform array created");

			for (int i = 0; i < (int)CircularBufferSize; ++i) {
				get_segment_from_file(fileDescriptor1, SegmentSize, waveform1.GetSafeArrayPtr());
				segmentIds1[i] = download_waveform(driver, channel1, 0, 0, SegmentSize, waveform1.GetSafeArrayPtr());
				set_data_entry(channel1, i, false, false, false, 0, segAdv, false, false, false, false, 1, 1, (uint)segmentIds1[i], 0, SegEndOffWholeSegment);
			}

			send_log(L"Channel 1: Waveform loaded");

			waveform1.Destroy();

			send_log(L"Channel 1: Setting data entries"); 

			set_data_entry(channel1, 0, true, false, false, 0, segAdv, false, false, false, false, 1, 1, (uint)segmentIds1[0], 0, SegEndOffWholeSegment);
			set_data_entry(channel1, CircularBufferSize - 1, false, true, false, 0, 0, false, false, false, false, 1, 1, (uint)segmentIds1[(int)(CircularBufferSize - 1)], 0, SegEndOffWholeSegment);

		}

		send_log(L"Instrument is ready");
		
		start_file_streaming(fileDescriptor1, fileDescriptor2, channel1, channel2, segmentIds1, segmentIds2);

		send_log(L"Cleanig memory");

		if (isChannel1Enabled) {
			_close(fileDescriptor1);

			::SysFreeString(channel1);

			delete streamBuffer1;

			segmentIds1.Destroy();
		}

		if (isChannel2Enabled) {
			_close(fileDescriptor2);

			::SysFreeString(channel2);

			delete streamBuffer2;

			segmentIds2.Destroy();
		}

	} catch (_com_error& ex) {
		LPCTSTR error = ex.ErrorMessage();

		send_log(L"Error: %ls", error);
		send_log(L"Error description: %ls", ex.Description().GetBSTR());
	} catch (CAtlException& ex) {
		_com_error err(ex.m_hr);
		error_queue();
	} catch (std::exception& ex) {
		error_queue();
	}
}

void Streaming::get_segment_from_file(int fileDescriptor, int segmentSize, LPSAFEARRAY* waveform) {
	size_t readCount = 0;

	read_file(fileDescriptor, waveform, 0, SegmentSize * 4, readCount);
}

bool Streaming::read_file(int fileDescriptor, LPSAFEARRAY* readData, const size_t pos, const size_t size, size_t& readCount) {
	short *pData = NULL;

	if (!SUCCEEDED(SafeArrayAccessData(*readData, (void **)&pData))) {
		return false;
	}

	pData += pos;

	readCount = _read(fileDescriptor, reinterpret_cast <void*>(pData), size);

	SafeArrayUnaccessData(*readData);

	if (readCount <= 0) {
		return false;
	}

	return true;
}

bool Streaming::read_file(int fileDescriptor, char* readData, const size_t size, size_t& readCount) {
	readCount = _read(fileDescriptor, reinterpret_cast <void*>(readData), size);

	if (readCount <= 0) {
		return false;
	}

	return true;
}

void Streaming::set_file_segment_count() {
	std::fstream stream(filename1.c_str(), std::ios_base::in | std::ios_base::binary);

	stream.seekg(0, std::ios_base::end);
	uint64 fileSize = static_cast<uint64>(stream.tellg());
	NumSegments1 = (int)(fileSize / (SegmentSize * 4));
	stream.close();

	stream.open(filename2.c_str(), std::ios_base::in | std::ios_base::binary);

	stream.seekg(0, std::ios_base::end);
	fileSize = static_cast<uint64>(stream.tellg());
	NumSegments2 = (int)(fileSize / (SegmentSize * 4));
	stream.close();
}

void Streaming::start_file_streaming(int fileDescriptor1, int fileDescriptor2, BSTR channel1, BSTR channel2, const CComSafeArray<int>& segmentIds1, const CComSafeArray<int>& segmentIds2) {
	try {
		send_log(L"File streaming is starting");

		fileDescriptors descriptors;
		descriptors.desc_1 = fileDescriptor1;
		descriptors.desc_2 = fileDescriptor2;

		uint threadID = 0;
		hThread = (HANDLE)_beginthreadex(NULL, 0, read_data, reinterpret_cast<void*>(&descriptors), 0, &threadID);
		hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		Sleep(0);

		if (isChannel1Enabled) {
			driver->Arbitrary3->PutSequencingMode(channel1, AgM8190SequencingModeSTSequence);
			driver->Arbitrary3->Sequence3->PutDynamicModeEnabled(channel1, true);
			driver->Arbitrary3->Sequence3->PutStreamingEnabled(channel1, true);

			driver->InitiateGeneration(channel1);
		}

		if (isChannel2Enabled) {
			driver->Arbitrary3->PutSequencingMode(channel2, AgM8190SequencingModeSTSequence);
			driver->Arbitrary3->Sequence3->PutDynamicModeEnabled(channel2, true);
			driver->Arbitrary3->Sequence3->PutStreamingEnabled(channel2, true);

			driver->InitiateGeneration(channel2);
		}

		error_queue();

		send_log(L"Writing waveform data");
		
		write_waveform_data(driver, channel1, channel2, segmentIds1, segmentIds2);

		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);

		send_log(L"Thread is closed");

		if (isChannel1Enabled) driver->AbortGeneration(channel1);
		if (isChannel2Enabled) driver->AbortGeneration(channel2);
	} catch (_com_error& ex) {
		throw ex;
	} catch (std::exception& ex) {
		throw ex;
	}
}

uint __stdcall Streaming::read_data(void* data) {
	try {
		fileDescriptors descriptors = *(reinterpret_cast<fileDescriptors*>(data));
		int fileDescriptor1 = descriptors.desc_1;
		int fileDescriptor2 = descriptors.desc_2;

		if (isChannel1Enabled && isChannel2Enabled) {
			if (_eof(fileDescriptor1)) {
				_lseek(fileDescriptor1, 0, SEEK_SET);
			}

			if (_eof(fileDescriptor2)) {
				_lseek(fileDescriptor2, 0, SEEK_SET);
			}

			int segmentBytes = SegmentSize * 4;
			int numRead = segmentBytes / FileBufferSize;

			bool streamingError = false;
			uint repeat = 0;

			for (; repeat < RepeatCount; repeat++) {
				if (stop_streaming) {
					_endthreadex(0);

					return 0;
				}

				while (!_eof(fileDescriptor1) && !_eof(fileDescriptor2) && !streamingError) {
					while (streamBuffer1->Count() >= streamBuffer1->Capacity() && streamBuffer2->Count() >= streamBuffer2->Capacity()) {
						if (stop_streaming) {
							_endthreadex(0);

							return 0;
						}

						Sleep(0);
					}

					size_t readCount1 = 0;
					size_t readCount2 = 0;

					streamBuffer1->IsWriting(true);
					streamBuffer2->IsWriting(true);

					LPSAFEARRAY* writeBuffer1 = streamBuffer1->GetWriteBuffer();
					LPSAFEARRAY* writeBuffer2 = streamBuffer2->GetWriteBuffer();

					short* pData1 = NULL;
					HRESULT hr1 = SafeArrayAccessData(*writeBuffer1, (void**)&pData1);

					if (!SUCCEEDED(hr1)) {
						throw _com_error(hr1);
					}

					short* pData2 = NULL;
					HRESULT hr2 = SafeArrayAccessData(*writeBuffer2, (void**)&pData2);

					if (!SUCCEEDED(hr2)) {
						throw _com_error(hr2);
					}

					for (int index = 0; index < numRead; index++) {
						read_file(fileDescriptor1, reinterpret_cast<char*>(pData1), FileBufferSize, readCount1);
						pData1 += readCount1 / 2;

						read_file(fileDescriptor2, reinterpret_cast<char*>(pData2), FileBufferSize, readCount2);
						pData2 += readCount2 / 2;
					}

					SafeArrayUnaccessData(*writeBuffer1);
					SafeArrayUnaccessData(*writeBuffer2);

					streamBuffer1->IsWriting(false);
					streamBuffer2->IsWriting(false);
				}

				if (streamingError) {
					break;
				}

				if (infiniteRepeat) {
					repeat = -1;
				}

				_lseek(fileDescriptor1, 0, SEEK_SET);
				_lseek(fileDescriptor2, 0, SEEK_SET);
			}
		} else if (isChannel1Enabled && !isChannel2Enabled) {
			if (_eof(fileDescriptor1)) {
				_lseek(fileDescriptor1, 0, SEEK_SET);
			}

			int segmentBytes = SegmentSize * 4;
			int numRead = segmentBytes / FileBufferSize;

			bool streamingError = false;
			uint repeat = 0;

			for (; repeat < RepeatCount; repeat++) {
				if (stop_streaming) {
					_endthreadex(0);

					return 0;
				}

				while (!_eof(fileDescriptor1) && !streamingError) {
					while (streamBuffer1->Count() >= streamBuffer1->Capacity()) {
						if (stop_streaming) {
							_endthreadex(0);

							return 0;
						}

						Sleep(0);
					}

					size_t readCount1 = 0;

					streamBuffer1->IsWriting(true);

					LPSAFEARRAY* writeBuffer1 = streamBuffer1->GetWriteBuffer();

					short* pData1 = NULL;
					HRESULT hr1 = SafeArrayAccessData(*writeBuffer1, (void**)&pData1);

					if (!SUCCEEDED(hr1)) {
						throw _com_error(hr1);
					}

					for (int index = 0; index < numRead; index++) {
						read_file(fileDescriptor1, reinterpret_cast<char*>(pData1), FileBufferSize, readCount1);
						pData1 += readCount1 / 2;
					}

					SafeArrayUnaccessData(*writeBuffer1);

					streamBuffer1->IsWriting(false);
				}

				if (streamingError) {
					break;
				}

				if (infiniteRepeat) {
					repeat = -1;
				}

				_lseek(fileDescriptor1, 0, SEEK_SET);
			}
		} else if (!isChannel1Enabled && isChannel2Enabled) {
			if (_eof(fileDescriptor2)) {
				_lseek(fileDescriptor2, 0, SEEK_SET);
			}

			int segmentBytes = SegmentSize * 4;
			int numRead = segmentBytes / FileBufferSize;

			bool streamingError = false;
			uint repeat = 0;

			for (; repeat < RepeatCount; repeat++) {
				if (stop_streaming) {
					_endthreadex(0);

					return 0;
				}

				while (!_eof(fileDescriptor2) && !streamingError) {
					while (streamBuffer2->Count() >= streamBuffer2->Capacity()) {
						if (stop_streaming) {
							_endthreadex(0);

							return 0;
						}

						Sleep(0);
					}

					size_t readCount2 = 0;

					streamBuffer2->IsWriting(true);

					LPSAFEARRAY* writeBuffer2 = streamBuffer2->GetWriteBuffer();

					short* pData2 = NULL;
					HRESULT hr2 = SafeArrayAccessData(*writeBuffer2, (void**)&pData2);

					if (!SUCCEEDED(hr2)) {
						throw _com_error(hr2);
					}

					for (int index = 0; index < numRead; index++) {
						read_file(fileDescriptor2, reinterpret_cast<char*>(pData2), FileBufferSize, readCount2);
						pData2 += readCount2 / 2;
					}

					SafeArrayUnaccessData(*writeBuffer2);

					streamBuffer2->IsWriting(false);
				}

				if (streamingError) {
					break;
				}

				if (infiniteRepeat) {
					repeat = -1;
				}

				_lseek(fileDescriptor2, 0, SEEK_SET);
			}
		}



	} catch (_com_error& ex) {
		send_log(ex.ErrorMessage());
	} catch (CAtlException& ex) {
		_com_error err(ex.m_hr);
		send_log(err.ErrorMessage());
	} catch (std::exception& ex) {
		send_log(CA2W(ex.what()).operator LPWSTR());
	}

	_endthreadex(0);

	return 0;
}

void Streaming::send_log(const wchar_t* msg, ...) {
	if (!isDebugMode) return;

	log_stream.open(log_filename.c_str(), std::ios::out | std::ios::app);

	va_list argptr;
	va_start(argptr, msg);

	wchar_t buf[MAX_PATH];
	vswprintf(buf, MAX_PATH, msg, argptr);

	std::string message(CW2A(buf).operator LPSTR());

	va_end(argptr);

	auto now_time = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now_time);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_time.time_since_epoch()) % 1000;

	std::stringstream string_time;
	string_time << std::put_time(std::localtime(&time_t), "%H:%M:%S");
	std::string string_ms = std::to_string(ms.count());

	if (string_ms.length() == 2) string_ms = "0" + string_ms;
	if (string_ms.length() == 1) string_ms = "00" + string_ms;

	log_stream << "[" << string_time.str() << " " << string_ms << "] " << message.c_str() << "\n";
	log_stream.close();
}


void Streaming::set_bit_resolution_mode(int index) {
	switch (index) {
	default:
		BitResolutionMode = AgM8190BitResolutionModePrecision;
		break;
	}
}

void Streaming::set_instrument_params(int segment_size, int repeat_count, double sf) {
	set_segment_size(segment_size);
	set_repeat_count(repeat_count);

	SampleFreq = sf;

	send_log(L"Segment size     = %d", segment_size);
	send_log(L"Repeat count     = %d", repeat_count);
	send_log(L"Sample frequency = %g", sf);

	streamingInProggress = true;
}

void Streaming::set_channel_params(int channel_num, double cf, double sf, int interpolation, std::wstring filename) {
	if (channel_num == 1) {
		set_cf_ch1(cf);
		set_filename_ch1(filename);

		BitResolutionMode1 = GetInterpolationMode(interpolation);
	} else {
		set_cf_ch2(cf);
		set_filename_ch2(filename);

		BitResolutionMode2 = GetInterpolationMode(interpolation);
	}


	send_log(L"carrier_freq = %g", cf);
}

void Streaming::set_num_segments(int num) {
	NumSegments1 = num;
	NumSegments2 = num;
}

void Streaming::set_segment_size(int size) {
	SegmentSize = size;

	FileBufferSize = size;
}

void Streaming::set_trigger_freq(double trigger_freq) {
	TriggerFreq = trigger_freq;
}

void Streaming::set_repeat_count(int count) {
	send_log(L"Setting repeat count (repeat_count = %d)", count);

	if (count == 0) {
		RepeatCount = 1;
		infiniteRepeat = true;
	} else {
		RepeatCount = count;
		infiniteRepeat = false;
	}

	send_log(L"Setted repeat count (repeat_count = %d)", RepeatCount);
}

void Streaming::set_buffer_num(int num) {
	//TODO
}

void Streaming::set_enabled_ch1(bool state) {
	isChannel1Enabled = state;
}

void Streaming::set_cf_ch1(double cf) {
	central_freq_ch1 = cf;
}

void Streaming::set_sf_ch1(double sf) {
	sample_rate_ch1 = sf;
}

void Streaming::set_is_alg_ch1(bool is_alg) {
	isChannel1Alg = is_alg;

	if (is_alg) send_log(L"Channel 1: Setted algorithmic streaming");
	else send_log(L"Channel 1: Setted file streaming");
}

void Streaming::set_filename_ch1(std::wstring filename) {
	filename1 = filename;
}

void Streaming::set_enabled_ch2(bool state) {
	isChannel2Enabled = state;
}

void Streaming::set_cf_ch2(double cf) {
	central_freq_ch2 = cf;
}

void Streaming::set_sf_ch2(double sf) {
	sample_rate_ch2 = sf;
}

void Streaming::set_is_alg_ch2(bool is_alg) {
	isChannel2Alg = is_alg;

	if (is_alg) send_log(L"Channel 2: Setted algorithmic streaming");
	else send_log(L"Channel 2: Setted file streaming");
}

void Streaming::set_filename_ch2(std::wstring filename) {
	filename2 = filename;
}

void Streaming::set_external_sample_clock(int channel_num) {
	if (channel_num == 1)
		sampleClock = Channel1;
	else if (channel_num == 2)
		sampleClock = Channel2;
}

void Streaming::set_reference_clock_source(bool is_internal) {
	if (is_internal)
		refSource = Internal;
	else
		refSource = External;
}

uint __stdcall Streaming::put_data(void* data) {
	std::vector<double>* wfData = reinterpret_cast<std::vector<double> *>(data);

	int segmentIndex = (int)CircularBufferSize;

	try {
		if (isChannel1Enabled && isChannel2Enabled) {
			int segmentBytes = SegmentSize * 4;
			int numRead = segmentBytes / FileBufferSize;

			bool streamingError = false;
			uint repeat = 0;

			for (; repeat < RepeatCount; repeat++) {
				if (stop_streaming) {
					_endthreadex(0);

					return 0;
				}

				while (segmentIndex < NumSegments && !streamingError) {
					while (streamBuffer1->Count() >= streamBuffer1->Capacity() && streamBuffer2->Count() >= streamBuffer2->Capacity()) {
						if (stop_streaming) {
							_endthreadex(0);

							return 0;
						}

						Sleep(0);
					}

					
					streamBuffer1->IsWriting(true);
					streamBuffer2->IsWriting(true);

					convert_to_dac(segmentIndex + 1, *wfData, streamBuffer1->GetWriteBuffer());
					convert_to_dac(segmentIndex + 1, *wfData, streamBuffer2->GetWriteBuffer());

					streamBuffer1->IsWriting(false);
					streamBuffer2->IsWriting(false);
				}

				if (streamingError) {
					break;
				}

				if (infiniteRepeat) {
					repeat = -1;
				}
			}
		} else if (isChannel1Enabled && !isChannel2Enabled) {
			int segmentBytes = SegmentSize * 4;
			int numRead = segmentBytes / FileBufferSize;

			bool streamingError = false;
			uint repeat = 0;

			for (; repeat < RepeatCount; repeat++) {
				if (stop_streaming) {
					_endthreadex(0);

					return 0;
				}

				while (segmentIndex < NumSegments && !streamingError) {
					while (streamBuffer1->Count() >= streamBuffer1->Capacity()) {
						if (stop_streaming) {
							_endthreadex(0);

							return 0;
						}

						Sleep(0);
					}


					streamBuffer1->IsWriting(true);

					convert_to_dac(segmentIndex + 1, *wfData, streamBuffer1->GetWriteBuffer());

					streamBuffer1->IsWriting(false);
				}

				if (streamingError) {
					break;
				}

				if (infiniteRepeat) {
					repeat = -1;
				}
			}
		} else if (!isChannel1Enabled && isChannel2Enabled) {
			int segmentBytes = SegmentSize * 4;
			int numRead = segmentBytes / FileBufferSize;

			bool streamingError = false;
			uint repeat = 0;

			for (; repeat < RepeatCount; repeat++) {
				if (stop_streaming) {
					_endthreadex(0);

					return 0;
				}

				while (segmentIndex < NumSegments && !streamingError) {
					while (streamBuffer2->Count() >= streamBuffer2->Capacity()) {
						if (stop_streaming) {
							_endthreadex(0);

							return 0;
						}

						Sleep(0);
					}

					streamBuffer2->IsWriting(true);

					convert_to_dac(segmentIndex + 1, *wfData, streamBuffer2->GetWriteBuffer());

					streamBuffer2->IsWriting(false);
				}

				if (streamingError) {
					break;
				}

				if (infiniteRepeat) {
					repeat = -1;
				}
			}
		}
	} catch (_com_error& ex) {
		send_log(ex.ErrorMessage());
	} catch (CAtlException& ex) {
		_com_error err(ex.m_hr);
		send_log(err.ErrorMessage());
	} catch (std::exception& ex) {
		send_log(CA2W(ex.what()).operator LPWSTR());
	}

	_endthreadex(0);

	return 0;
}

uint __stdcall Streaming::put_data_ch1(void* data) {
	std::vector<double>* wfData = reinterpret_cast<std::vector<double> *>(data);

	int segmentIndex = (int)CircularBufferSize;

	try {
		bool streamingError = false;
		uint repeat = 0;

		for (; repeat < RepeatCount; repeat++) {
			while (segmentIndex < NumSegments && !streamingError) {
				while (streamBuffer1->Count() >= streamBuffer1->Capacity()) {
					Sleep(0);	//TODO: is it necessary?
				}

				streamBuffer1->IsWriting(true);
				convert_to_dac(segmentIndex + 1, *wfData, streamBuffer1->GetWriteBuffer());
				streamBuffer1->IsWriting(false);

				segmentIndex++;
			}

			if (streamingError) {
				break;
			}

			if (infiniteRepeat && repeat == RepeatCount - 1) {
				repeat = 0;
			}

			segmentIndex = 0;
		}

	} catch (_com_error& ex) {
		send_log(ex.ErrorMessage());
	} catch (CAtlException& ex) {
		_com_error err(ex.m_hr);
		send_log(err.ErrorMessage());
	} catch (std::exception& ex) {
		send_log(CA2W(ex.what()).operator LPWSTR());
	}

	_endthreadex(0);

	return 0;
}

uint __stdcall Streaming::put_data_ch2(void* data) {
	std::vector<double>* wfData = reinterpret_cast<std::vector<double> *>(data);

	int segmentIndex = (int)CircularBufferSize;

	try {
		bool streamingError = false;
		uint repeat = 0;

		for (; repeat < RepeatCount; repeat++) {
			while (segmentIndex < NumSegments && !streamingError) {
				while (streamBuffer2->Count() >= streamBuffer2->Capacity()) {
					Sleep(0);	//TODO: is it necessary?
				}

				streamBuffer2->IsWriting(true);
				Streaming::convert_to_dac(segmentIndex + 1, *wfData, streamBuffer2->GetWriteBuffer());
				streamBuffer2->IsWriting(false);

				segmentIndex++;
			}

			if (streamingError) {
				break;
			}

			if (infiniteRepeat && repeat == RepeatCount - 1) {
				repeat = 0;
			}

			segmentIndex = 0;
		}

	} catch (_com_error& ex) {
		send_log(ex.ErrorMessage());
	} catch (CAtlException& ex) {
		_com_error err(ex.m_hr);
		send_log(err.ErrorMessage());
	} catch (std::exception& ex) {
		send_log(CA2W(ex.what()).operator LPWSTR());
	}

	_endthreadex(0);

	return 0;
}

bool Streaming::get_is_alg_ch1() {
	return isChannel1Alg;
}

bool Streaming::get_is_alg_ch2() {
	return isChannel2Alg;
}

const std::wstring Streaming::Bytes(uint64 bytes) {
	std::wstring result;

	if (bytes > 1024L * 1024L * 1024L) {
		result = ToString(bytes / 1024.0 / 1024.0 / 1024.0) + L"GBytes";
	} else if (bytes > 1024L * 1024L) {
		result = ToString(bytes / 1024.0 / 1024.0) + L"MBytes";
	} else if (bytes > 1024L) {
		result = ToString(bytes / 1024.0) + L"KBytes";
	} else {
		result = ToString((double)bytes) + L"Bytes";
	}

	return result;
}

std::wstring Streaming::ToString(double value, int precision) {
	std::wostringstream stream;

	stream << std::fixed << std::setprecision(precision) << value;

	return stream.str();
}

CString Streaming::ToTimeString(clock_t span) {
	long seconds = span / CLOCKS_PER_SEC;
	long millSeconds = span % CLOCKS_PER_SEC;

	CString fmtTime(_T(""));
	fmtTime.Format(_T(":%d"), millSeconds);

	CTimeSpan timespan(seconds);

	if (timespan.GetDays() != 0) {
		fmtTime = timespan.Format(_T("%D:%H:%M:%S")) + fmtTime;
	} else {
		fmtTime = timespan.Format(_T("%H:%M:%S")) + fmtTime;
	}

	return fmtTime;
}

int Streaming::GetInterpolationFactor(AgM8190BitResolutionModeEnum mode) {
	int factor = 48;

	switch (mode) {
	case AgM8190BitResolutionModeSpeed:
		factor = 1;
		break;
	case AgM8190BitResolutionModePrecision:
		factor = 1;
		break;
	case AgM8190BitResolutionModeInterpolationX3:
		factor = 3;
		break;
	case AgM8190BitResolutionModeInterpolationX12:
		factor = 12;
		break;
	case AgM8190BitResolutionModeInterpolationX24:
		factor = 24;
		break;
	case AgM8190BitResolutionModeInterpolationX48:
		factor = 48;
		break;
	}

	return factor;
}

AgM8190BitResolutionModeEnum Streaming::GetInterpolationMode(int factor) {
	AgM8190BitResolutionModeEnum mode = AgM8190BitResolutionModeInterpolationX48;

	switch (factor) {
	case 1:
		mode = AgM8190BitResolutionModePrecision;
		break;
	case 3:
		mode = AgM8190BitResolutionModeInterpolationX3;
		break;
	case 12:
		mode = AgM8190BitResolutionModeInterpolationX12;
		break;
	case 24:
		mode = AgM8190BitResolutionModeInterpolationX24;
		break;
	case 48:
		mode = AgM8190BitResolutionModeInterpolationX48;
		break;
	}

	return mode;
}

