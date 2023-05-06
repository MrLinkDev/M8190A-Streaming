#include <msclr\marshal_cppstd.h>

#include "streaming.h"
#include "main_form.h"

using namespace System;
using namespace System::Windows::Forms;

using namespace m8190astreaming;

Streaming streaming;

[STAThreadAttribute]

void main(array<String^>^ args) {
	Application::SetCompatibleTextRenderingDefault(false);
	Application::EnableVisualStyles();

	m8190astreaming::main_form form;

	Application::Run(% form);
}

std::string to_log(const wchar_t* msg, ...) {
	va_list argptr;
	va_start(argptr, msg);

	wchar_t buf[MAX_PATH];
	vswprintf(buf, MAX_PATH, msg, argptr);

	std::string value(CW2A(buf).operator LPSTR());

	va_end(argptr);

	return value;
}

System::Void main_form::init_driver(System::Object^ sender, System::EventArgs^ e) {
	send_log(to_log(L"Driver Initialization started"));

	std::wstring address = msclr::interop::marshal_as<std::wstring>(this->address_box->Text);
	send_log(to_log(L"VISA address: %ls", address.c_str()));

	streaming.init_driver(address);

	send_log(to_log(L"Driver Initialized"));

	send_log(to_log(L"Identifier:  %ls", streaming.driver->Identity->Identifier.GetBSTR()));
	send_log(to_log(L"Revision:    %ls", streaming.driver->Identity->Revision.GetBSTR()));
	send_log(to_log(L"Vendor:      %ls", streaming.driver->Identity->Vendor.GetBSTR()));
	send_log(to_log(L"Description: %ls", streaming.driver->Identity->Description.GetBSTR()));
	send_log(to_log(L"Model:       %ls", streaming.driver->Identity->InstrumentModel.GetBSTR()));
	send_log(to_log(L"FirmwareRev: %ls", streaming.driver->Identity->InstrumentFirmwareRevision.GetBSTR()));
	send_log(to_log(L"Serial #:    %ls", streaming.driver->System->SerialNumber.GetBSTR()));

	this->address_box->Enabled = false;
	this->connect_button->Enabled = false;
	this->connect_button->Text = "Connected";

	this->reset_button->Enabled = true;

	this->label_segment_size->Enabled = true;
	this->label_repeat_count->Enabled = true;

	this->segment_size->Enabled = true;
	this->repeat_count->Enabled = true;

	this->label1->Enabled = true;
	this->label4->Enabled = true;

	this->panel_ref_source->Enabled = true;

	this->enable_1->Enabled = true;
	this->enable_2->Enabled = true;

	if (this->segment_size->Text->Equals("")) this->segment_size->Text = DEFAULT_SEGMENT_SIZE.ToString();
	if (this->repeat_count->Text->Equals("")) this->repeat_count->Text = DEFAULT_REPEAT_COUNT.ToString();

	if (this->cf_1->Text->Equals("")) this->cf_1->Text = DEFAULT_CARRIER_FREQ.ToString();
	if (this->sf_1->Text->Equals("")) this->sf_1->Text = DEFAULT_SAMPLE_FREQ.ToString();

	if (this->cf_2->Text->Equals("")) this->cf_2->Text = DEFAULT_CARRIER_FREQ.ToString();
	if (this->sf_2->Text->Equals("")) this->sf_2->Text = DEFAULT_SAMPLE_FREQ.ToString();
}

System::Void main_form::begin_streaming(System::Object^ sender, System::EventArgs^ e) {
	this->segment_size->Enabled = false;
	this->repeat_count->Enabled = false;
	this->panel_ext_sample_clock->Enabled = false;
	this->panel_ref_source->Enabled = false;
	this->enable_1->Enabled = false;
	this->enable_2->Enabled = false;
	this->channel_1->Enabled = false;
	this->channel_2->Enabled = false;
	this->label1->Enabled = false;
	this->label4->Enabled = false;
	this->reset_button->Enabled = false;

	this->start_streaming->Enabled = false;

	

	this->start_streaming->Visible = false;
	this->stop_streaming->Visible = true;
	this->stop_streaming->Enabled = true;

	this->thread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &main_form::async_streaming));
	this->thread->Priority = System::Threading::ThreadPriority::Highest; 
	this->thread->Start();
}

System::Void main_form::enable_1_state_changed(System::Object^ sender, System::EventArgs^ e) {
	if (this->enable_1->CheckState == System::Windows::Forms::CheckState::Checked) {
		print_log("Channel 1: Enabled");

		this->channel_1->Enabled = true;
		streaming.set_enabled_ch1(true);

		if (this->channel_2->Enabled == true)
			this->panel_ext_sample_clock->Enabled = true;
		else
			this->start_streaming->Enabled = true;
	} else {
		print_log("Channel 1: Disabled");

		this->channel_1->Enabled = false;
		streaming.set_enabled_ch1(false);

		this->panel_ext_sample_clock->Enabled = false;

		if (this->channel_2->Enabled == false)
			this->start_streaming->Enabled = false;
	}
}

System::Void main_form::enable_2_state_changed(System::Object^ sender, System::EventArgs^ e) {
	if (this->enable_2->CheckState == System::Windows::Forms::CheckState::Checked) {
		print_log("Channel 2: Enabled");

		this->channel_2->Enabled = true;
		streaming.set_enabled_ch2(true); 
		
		if (this->channel_1->Enabled == true)
			this->panel_ext_sample_clock->Enabled = true;
		else
			this->start_streaming->Enabled = true;
	} else {
		print_log("Channel 2: Disabled");

		this->channel_2->Enabled = false;
		streaming.set_enabled_ch2(false);

		this->panel_ext_sample_clock->Enabled = false;

		if (this->channel_1->Enabled == false)
			this->start_streaming->Enabled = false;
	}
}

System::Void main_form::set_alg_streaming_1(System::Object^ sender, System::EventArgs^ e) {
	this->file_path_1->Enabled = false;
	this->pick_file_1->Enabled = false;
	this->label_filepath_1->Enabled = false;

	if (!this->alg_stream_2->Checked) {
		this->alg_stream_2->Checked = true;
		this->file_path_2->Enabled = false;
		this->pick_file_2->Enabled = false;
		this->label_filepath_2->Enabled = false;
	}

	streaming.set_is_alg_ch1(true);
	streaming.set_is_alg_ch2(true);
}

System::Void main_form::set_alg_streaming_2(System::Object^ sender, System::EventArgs^ e) {
	this->file_path_2->Enabled = false;
	this->pick_file_2->Enabled = false;
	this->label_filepath_2->Enabled = false;

	if (!this->alg_stream_1->Checked) {
		this->alg_stream_1->Checked = true;
		this->file_path_1->Enabled = false;
		this->pick_file_1->Enabled = false;
		this->label_filepath_1->Enabled = false;
	}

	streaming.set_is_alg_ch2(true);
	streaming.set_is_alg_ch1(true);
}

System::Void main_form::set_file_streaming_1(System::Object^ sender, System::EventArgs^ e) {
	this->file_path_1->Enabled = true;
	this->pick_file_1->Enabled = true;
	this->label_filepath_1->Enabled = true;

	if (!this->file_stream_2->Checked) {
		this->file_stream_2->Checked = true;
		this->file_path_2->Enabled = true;
		this->pick_file_2->Enabled = true;
		this->label_filepath_2->Enabled = true;
	}

	streaming.set_is_alg_ch1(false);
	streaming.set_is_alg_ch2(false);
}

System::Void main_form::set_file_streaming_2(System::Object^ sender, System::EventArgs^ e) {
	this->file_path_2->Enabled = true;
	this->pick_file_2->Enabled = true;
	this->label_filepath_2->Enabled = true;

	if (!this->file_stream_1->Checked) {
		this->file_stream_1->Checked = true;
		this->file_path_1->Enabled = true;
		this->pick_file_1->Enabled = true;
		this->label_filepath_1->Enabled = true;
	}

	streaming.set_is_alg_ch2(false);
	streaming.set_is_alg_ch1(false);
}

System::Void m8190astreaming::main_form::set_external_sample_clock_ch1(System::Object^ sender, System::EventArgs^ e) {
	streaming.set_external_sample_clock(1);

	print_log("External sample clock: Channel 1");
}

System::Void m8190astreaming::main_form::set_external_sample_clock_ch2(System::Object^ sender, System::EventArgs^ e) {
	streaming.set_external_sample_clock(2);

	print_log("External sample clock: Channel 2");
}

System::Void m8190astreaming::main_form::set_ref_clock_source_int(System::Object^ sender, System::EventArgs^ e) {
	streaming.set_reference_clock_source(true);

	print_log("Reference clock source: Internal");
}

System::Void m8190astreaming::main_form::set_ref_clock_source_ext(System::Object^ sender, System::EventArgs^ e) {
	streaming.set_reference_clock_source(false);

	print_log("Reference clock source: External");
}

System::Void m8190astreaming::main_form::reset(System::Object^ sender, System::EventArgs^ e) {
	streaming.reset();
}

System::Void main_form::print_log(String^ message) {
	log_output->Text += message + L"\n";
	log_output->SelectionStart = log_output->Text->Length;
	log_output->ScrollToCaret();
}

System::Void main_form::send_log(std::string message) {
	String^ msg = gcnew String(message.c_str());

	m8190astreaming::main_form::print_log(msg);
}

System::Void m8190astreaming::main_form::open_file(System::Object^ sender, System::EventArgs^ e) {
	OpenFileDialog^ openFileDialog = gcnew OpenFileDialog;

	openFileDialog->InitialDirectory = "c:\\";
	openFileDialog->RestoreDirectory = true;

	if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
		if ((openFileDialog->OpenFile()) != nullptr) {
			if (sender->Equals(pick_file_1))
				this->file_path_1->Text = openFileDialog->FileName;
			else
				this->file_path_2->Text = openFileDialog->FileName;
		}
	}
}

void m8190astreaming::main_form::async_streaming() {
	int seg_size = atoi(msclr::interop::marshal_as<std::string>(this->segment_size->Text).c_str());
	int r_count = atoi((msclr::interop::marshal_as<std::string>(this->repeat_count->Text)).c_str());

	double cf1 = std::stod((msclr::interop::marshal_as<std::string>(this->cf_1->Text)).c_str());
	double sf1 = std::stod((msclr::interop::marshal_as<std::string>(this->sf_1->Text)).c_str());
	int if1 = atoi((msclr::interop::marshal_as<std::string>(this->interpolation_1->Text)).c_str());

	double cf2 = std::stod((msclr::interop::marshal_as<std::string>(this->cf_2->Text)).c_str());
	double sf2 = std::stod((msclr::interop::marshal_as<std::string>(this->sf_2->Text)).c_str());
	int if2 = atoi((msclr::interop::marshal_as<std::string>(this->interpolation_2->Text)).c_str());

	std::wstring filename1 = msclr::interop::marshal_as<std::wstring>(this->file_path_1->Text);
	std::wstring filename2 = msclr::interop::marshal_as<std::wstring>(this->file_path_2->Text);

	if (this->enable_1->Checked && this->enable_2->Checked)
		print_log("Streaming starting (Both channels)");
	else if (this->enable_1->Checked && !this->enable_2->Checked)
		print_log("Streaming starting (Channel 1)");
	else if (!this->enable_1->Checked && this->enable_2->Checked)
		print_log("Streaming starting (Channel 2)");

	streaming.set_instrument_params(seg_size, r_count, sf1);
	print_log("Instrument parameters are setted");

	streaming.set_channel_params(1, cf1, sf1, if1, filename1);
	streaming.set_channel_params(2, cf2, sf2, if2, filename2);

	streaming.start_streaming();
	streaming.reset();

	print_log("Streaming stopped");

	this->stop_streaming->Enabled = false;
	this->stop_streaming->Visible = false;

	this->start_streaming->Enabled = true;
	this->start_streaming->Visible = true;

	this->segment_size->Enabled = true;
	this->repeat_count->Enabled = true;

	if (this->enable_1->Checked && !this->enable_2->Checked) this->panel_ext_sample_clock->Enabled = true;

	this->panel_ref_source->Enabled = true;
	this->enable_1->Enabled = true;
	this->enable_2->Enabled = true;

	if (this->enable_1->Checked) this->channel_1->Enabled = true;
	if (this->enable_2->Checked) this->channel_2->Enabled = true;

	this->label1->Enabled = true;
	this->label4->Enabled = true;
	this->reset_button->Enabled = true;
}

System::Void m8190astreaming::main_form::abort_streaming(System::Object^ sender, System::EventArgs^ e) {
	this->stop_streaming->Enabled = false;

	streaming.abort();
}

System::Void m8190astreaming::main_form::read_config() {
	std::wstring config_filename = L"config.txt";
	std::ifstream config_stream;

	std::string line;
	std::string delimiter = "=";

	std::string variable;
	String^ value;

	config_stream.open(config_filename.c_str(), std::ios::in);

	while (std::getline(config_stream, line)) {
		variable = line.substr(0, line.find(delimiter));
		value = msclr::interop::marshal_as<String^>(line.substr(line.find(delimiter) + 1, line.length()).c_str());

		if (variable._Equal("VISA_ADDRESS"))
			address_box->Text = value;
		else if (variable._Equal("SEGMENT_SIZE"))
			segment_size->Text = value;
		else if (variable._Equal("REPEAT_COUNT"))
			repeat_count->Text = value;
		else if (variable._Equal("EXTERNAL_SAMPLE_CLOCK")) {
			if (value == "CH1") {
				ext_1->Checked = true;
				streaming.set_external_sample_clock(1);
			} else if (value == "CH2") {
				ext_2->Checked = true;
				streaming.set_external_sample_clock(2);
			}
		} else if (variable._Equal("REFERENCE_SOURCE")) {
			if (value == "INT") {
				int_clock->Checked = true;
				streaming.set_reference_clock_source(true);
			} else if (value == "EXT") {
				ext_clock->Checked = true;
				streaming.set_reference_clock_source(false);
			}
		}

		if (line._Equal("CHANNEL_1:")) {
			while (std::getline(config_stream, line)) {
				variable = line.substr(0, line.find(delimiter));
				value = msclr::interop::marshal_as<String^>(line.substr(line.find(delimiter) + 1, line.length()).c_str());

				if (variable._Equal("CARRIER_FREQ"))
					cf_1->Text = value;
				else if (variable._Equal("SAMPLE_FREQ"))
					sf_1->Text = value;
				else if (variable._Equal("INTERPOLATION"))
					interpolation_1->Text = value;
				else if (variable._Equal("STREAMING_TYPE")) {
					if (value == "ALG") {
						this->file_path_1->Enabled = false;
						this->pick_file_1->Enabled = false;
						this->label_filepath_1->Enabled = false;
						this->alg_stream_1->Checked = true;

						streaming.set_is_alg_ch1(true);
					} else if (value == "FILE") {
						this->file_path_1->Enabled = true;
						this->pick_file_1->Enabled = true;
						this->label_filepath_1->Enabled = true;
						this->file_stream_1->Checked = true;

						streaming.set_is_alg_ch1(false);

						std::streampos last_pos = config_stream.tellg();
						std::getline(config_stream, line);
						variable = line.substr(0, line.find(delimiter));
						value = msclr::interop::marshal_as<String^>(line.substr(line.find(delimiter) + 1, line.length()).c_str());

						if (variable._Equal("PATH")) this->file_path_1->Text = value;
						else config_stream.seekg(last_pos);
					}
				}


				if (line._Equal("CHANNEL_2:")) {
					while (std::getline(config_stream, line)) {
						variable = line.substr(0, line.find(delimiter));
						value = msclr::interop::marshal_as<String^>(line.substr(line.find(delimiter) + 1, line.length()).c_str());

						if (variable._Equal("CARRIER_FREQ"))
							cf_2->Text = value;
						else if (variable._Equal("SAMPLE_FREQ"))
							sf_2->Text = value;
						else if (variable._Equal("INTERPOLATION"))
							interpolation_2->Text = value;
						else if (variable._Equal("STREAMING_TYPE")) {
							if (value == "ALG") {
								this->file_path_2->Enabled = false;
								this->pick_file_2->Enabled = false;
								this->label_filepath_2->Enabled = false;
								this->alg_stream_2->Checked = true;

								streaming.set_is_alg_ch1(true);
							} else if (value == "FILE") {
								this->file_path_2->Enabled = true;
								this->pick_file_2->Enabled = true;
								this->label_filepath_2->Enabled = true;
								this->file_stream_2->Checked = true;

								streaming.set_is_alg_ch1(false);

								std::streampos last_pos = config_stream.tellg();
								std::getline(config_stream, line);
								variable = line.substr(0, line.find(delimiter));
								value = msclr::interop::marshal_as<String^>(line.substr(line.find(delimiter) + 1, line.length()).c_str());

								if (variable._Equal("PATH")) this->file_path_2->Text = value;
								else config_stream.seekg(last_pos);
							}
						}
					}
				}
			}
		}

	}
}
