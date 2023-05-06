#pragma once
#include <string>

namespace m8190astreaming {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class main_form : public System::Windows::Forms::Form {
	public:
		main_form(void) {
			InitializeComponent();
			read_config();

			print_log("Application initialized");
		}

	protected:
		~main_form() {
			if (components) {
				delete components;
			}
		}

	private:
		System::ComponentModel::Container^ components;

		System::Windows::Forms::GroupBox^ channel_1;
		System::Windows::Forms::GroupBox^ channel_2;

		System::Windows::Forms::GroupBox^ groupBox3;
		System::Windows::Forms::GroupBox^ groupBox4;



		System::Windows::Forms::TextBox^ segment_size;








		System::Windows::Forms::TextBox^ repeat_count;

		System::Windows::Forms::CheckBox^ enable_1;
		System::Windows::Forms::CheckBox^ enable_2;

		System::Windows::Forms::Label^ label_address;

		System::Windows::Forms::Label^ label_segment_size;



		System::Windows::Forms::Label^ label_repeat_count;

		System::Windows::Forms::Label^ label_cf_1;
		System::Windows::Forms::Label^ label_cf_2;

		System::Windows::Forms::Label^ label_sf_1;
		System::Windows::Forms::Label^ label_sf_2;

		System::Windows::Forms::Label^ label_filepath_1;
		System::Windows::Forms::Label^ label_filepath_2;

		System::Windows::Forms::TextBox^ address_box;

		System::Windows::Forms::Button^ connect_button;

		System::Windows::Forms::TextBox^ cf_1;


		System::Windows::Forms::TextBox^ sf_1;


		System::Windows::Forms::RadioButton^ alg_stream_1;
		System::Windows::Forms::RadioButton^ file_stream_1;

		System::Windows::Forms::TextBox^ file_path_1;
		System::Windows::Forms::Button^ pick_file_1;

		System::Windows::Forms::TextBox^ cf_2;


		System::Windows::Forms::TextBox^ sf_2;


		System::Windows::Forms::RadioButton^ alg_stream_2;
		System::Windows::Forms::RadioButton^ file_stream_2;

		System::Windows::Forms::TextBox^ file_path_2;
		System::Windows::Forms::Button^ pick_file_2;
	private: System::Windows::Forms::Panel^ container_type_ch1;
	private: System::Windows::Forms::Panel^ container_type_ch2;




		System::Windows::Forms::RichTextBox^ log_output;
	private: System::Windows::Forms::Panel^ panel_ext_sample_clock;

	private: System::Windows::Forms::RadioButton^ ext_2;

	private: System::Windows::Forms::RadioButton^ ext_1;

		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::Button^ start_streaming;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::ComboBox^ interpolation_1;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::ComboBox^ interpolation_2;
private: System::Windows::Forms::Panel^ panel_ref_source;

private: System::Windows::Forms::RadioButton^ ext_clock;

private: System::Windows::Forms::RadioButton^ int_clock;
private: System::Windows::Forms::Button^ reset_button;



	private: System::Windows::Forms::Label^ label4;
private: System::Windows::Forms::Button^ stop_streaming;

		System::Threading::Thread^ thread;

#pragma region Windows Form Designer generated code

		void InitializeComponent(void) {
			this->channel_1 = (gcnew System::Windows::Forms::GroupBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->interpolation_1 = (gcnew System::Windows::Forms::ComboBox());
			this->label_sf_1 = (gcnew System::Windows::Forms::Label());
			this->sf_1 = (gcnew System::Windows::Forms::TextBox());
			this->label_cf_1 = (gcnew System::Windows::Forms::Label());
			this->cf_1 = (gcnew System::Windows::Forms::TextBox());
			this->container_type_ch1 = (gcnew System::Windows::Forms::Panel());
			this->pick_file_1 = (gcnew System::Windows::Forms::Button());
			this->file_stream_1 = (gcnew System::Windows::Forms::RadioButton());
			this->file_path_1 = (gcnew System::Windows::Forms::TextBox());
			this->label_filepath_1 = (gcnew System::Windows::Forms::Label());
			this->alg_stream_1 = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->log_output = (gcnew System::Windows::Forms::RichTextBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->stop_streaming = (gcnew System::Windows::Forms::Button());
			this->start_streaming = (gcnew System::Windows::Forms::Button());
			this->panel_ref_source = (gcnew System::Windows::Forms::Panel());
			this->ext_clock = (gcnew System::Windows::Forms::RadioButton());
			this->int_clock = (gcnew System::Windows::Forms::RadioButton());
			this->panel_ext_sample_clock = (gcnew System::Windows::Forms::Panel());
			this->ext_2 = (gcnew System::Windows::Forms::RadioButton());
			this->ext_1 = (gcnew System::Windows::Forms::RadioButton());
			this->reset_button = (gcnew System::Windows::Forms::Button());
			this->connect_button = (gcnew System::Windows::Forms::Button());
			this->repeat_count = (gcnew System::Windows::Forms::TextBox());
			this->segment_size = (gcnew System::Windows::Forms::TextBox());
			this->label_repeat_count = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label_segment_size = (gcnew System::Windows::Forms::Label());
			this->address_box = (gcnew System::Windows::Forms::TextBox());
			this->label_address = (gcnew System::Windows::Forms::Label());
			this->enable_2 = (gcnew System::Windows::Forms::CheckBox());
			this->enable_1 = (gcnew System::Windows::Forms::CheckBox());
			this->channel_2 = (gcnew System::Windows::Forms::GroupBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->interpolation_2 = (gcnew System::Windows::Forms::ComboBox());
			this->label_sf_2 = (gcnew System::Windows::Forms::Label());
			this->sf_2 = (gcnew System::Windows::Forms::TextBox());
			this->label_cf_2 = (gcnew System::Windows::Forms::Label());
			this->cf_2 = (gcnew System::Windows::Forms::TextBox());
			this->container_type_ch2 = (gcnew System::Windows::Forms::Panel());
			this->pick_file_2 = (gcnew System::Windows::Forms::Button());
			this->alg_stream_2 = (gcnew System::Windows::Forms::RadioButton());
			this->label_filepath_2 = (gcnew System::Windows::Forms::Label());
			this->file_stream_2 = (gcnew System::Windows::Forms::RadioButton());
			this->file_path_2 = (gcnew System::Windows::Forms::TextBox());
			this->channel_1->SuspendLayout();
			this->container_type_ch1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->panel_ref_source->SuspendLayout();
			this->panel_ext_sample_clock->SuspendLayout();
			this->channel_2->SuspendLayout();
			this->container_type_ch2->SuspendLayout();
			this->SuspendLayout();
			// 
			// channel_1
			// 
			this->channel_1->Controls->Add(this->label2);
			this->channel_1->Controls->Add(this->interpolation_1);
			this->channel_1->Controls->Add(this->label_sf_1);
			this->channel_1->Controls->Add(this->sf_1);
			this->channel_1->Controls->Add(this->label_cf_1);
			this->channel_1->Controls->Add(this->cf_1);
			this->channel_1->Controls->Add(this->container_type_ch1);
			this->channel_1->Enabled = false;
			this->channel_1->Location = System::Drawing::Point(10, 179);
			this->channel_1->Margin = System::Windows::Forms::Padding(0);
			this->channel_1->Name = L"channel_1";
			this->channel_1->Padding = System::Windows::Forms::Padding(0);
			this->channel_1->Size = System::Drawing::Size(378, 216);
			this->channel_1->TabIndex = 0;
			this->channel_1->TabStop = false;
			this->channel_1->Text = L"Channel 1";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 98);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(98, 13);
			this->label2->TabIndex = 10;
			this->label2->Text = L"Interpolation factor:";
			// 
			// interpolation_1
			// 
			this->interpolation_1->DisplayMember = L"0";
			this->interpolation_1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->interpolation_1->FormatString = L"E2";
			this->interpolation_1->FormattingEnabled = true;
			this->interpolation_1->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"3", L"12", L"24", L"48" });
			this->interpolation_1->Location = System::Drawing::Point(110, 95);
			this->interpolation_1->Name = L"interpolation_1";
			this->interpolation_1->Size = System::Drawing::Size(255, 21);
			this->interpolation_1->TabIndex = 11;
			// 
			// label_sf_1
			// 
			this->label_sf_1->AutoSize = true;
			this->label_sf_1->Location = System::Drawing::Point(6, 62);
			this->label_sf_1->Name = L"label_sf_1";
			this->label_sf_1->Size = System::Drawing::Size(95, 13);
			this->label_sf_1->TabIndex = 7;
			this->label_sf_1->Text = L"Sample frequency:";
			// 
			// sf_1
			// 
			this->sf_1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->sf_1->Location = System::Drawing::Point(110, 60);
			this->sf_1->Name = L"sf_1";
			this->sf_1->Size = System::Drawing::Size(255, 20);
			this->sf_1->TabIndex = 5;
			// 
			// label_cf_1
			// 
			this->label_cf_1->AutoSize = true;
			this->label_cf_1->Location = System::Drawing::Point(6, 27);
			this->label_cf_1->Name = L"label_cf_1";
			this->label_cf_1->Size = System::Drawing::Size(93, 13);
			this->label_cf_1->TabIndex = 4;
			this->label_cf_1->Text = L"Central frequency:";
			// 
			// cf_1
			// 
			this->cf_1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->cf_1->Location = System::Drawing::Point(110, 25);
			this->cf_1->Name = L"cf_1";
			this->cf_1->Size = System::Drawing::Size(255, 20);
			this->cf_1->TabIndex = 0;
			// 
			// container_type_ch1
			// 
			this->container_type_ch1->Controls->Add(this->pick_file_1);
			this->container_type_ch1->Controls->Add(this->file_stream_1);
			this->container_type_ch1->Controls->Add(this->file_path_1);
			this->container_type_ch1->Controls->Add(this->label_filepath_1);
			this->container_type_ch1->Controls->Add(this->alg_stream_1);
			this->container_type_ch1->Location = System::Drawing::Point(9, 131);
			this->container_type_ch1->Name = L"container_type_ch1";
			this->container_type_ch1->Size = System::Drawing::Size(356, 80);
			this->container_type_ch1->TabIndex = 9;
			// 
			// pick_file_1
			// 
			this->pick_file_1->Enabled = false;
			this->pick_file_1->Location = System::Drawing::Point(291, 55);
			this->pick_file_1->Name = L"pick_file_1";
			this->pick_file_1->Size = System::Drawing::Size(65, 20);
			this->pick_file_1->TabIndex = 8;
			this->pick_file_1->Text = L"Open file";
			this->pick_file_1->UseVisualStyleBackColor = true;
			this->pick_file_1->Click += gcnew System::EventHandler(this, &main_form::open_file);
			// 
			// file_stream_1
			// 
			this->file_stream_1->AutoSize = true;
			this->file_stream_1->Location = System::Drawing::Point(0, 27);
			this->file_stream_1->Name = L"file_stream_1";
			this->file_stream_1->Size = System::Drawing::Size(89, 17);
			this->file_stream_1->TabIndex = 2;
			this->file_stream_1->Text = L"File streaming";
			this->file_stream_1->UseVisualStyleBackColor = true;
			this->file_stream_1->Click += gcnew System::EventHandler(this, &main_form::set_file_streaming_1);
			// 
			// file_path_1
			// 
			this->file_path_1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->file_path_1->Enabled = false;
			this->file_path_1->Location = System::Drawing::Point(72, 55);
			this->file_path_1->Name = L"file_path_1";
			this->file_path_1->Size = System::Drawing::Size(213, 20);
			this->file_path_1->TabIndex = 5;
			// 
			// label_filepath_1
			// 
			this->label_filepath_1->AutoSize = true;
			this->label_filepath_1->Enabled = false;
			this->label_filepath_1->Location = System::Drawing::Point(16, 59);
			this->label_filepath_1->Name = L"label_filepath_1";
			this->label_filepath_1->Size = System::Drawing::Size(50, 13);
			this->label_filepath_1->TabIndex = 7;
			this->label_filepath_1->Text = L"File path:";
			// 
			// alg_stream_1
			// 
			this->alg_stream_1->AutoSize = true;
			this->alg_stream_1->Checked = true;
			this->alg_stream_1->Location = System::Drawing::Point(0, 4);
			this->alg_stream_1->Name = L"alg_stream_1";
			this->alg_stream_1->Size = System::Drawing::Size(124, 17);
			this->alg_stream_1->TabIndex = 1;
			this->alg_stream_1->TabStop = true;
			this->alg_stream_1->Text = L"Algorithmic streaming";
			this->alg_stream_1->UseVisualStyleBackColor = true;
			this->alg_stream_1->Click += gcnew System::EventHandler(this, &main_form::set_alg_streaming_1);
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->log_output);
			this->groupBox3->Location = System::Drawing::Point(10, 399);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(765, 150);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Log";
			// 
			// log_output
			// 
			this->log_output->BackColor = System::Drawing::SystemColors::WindowText;
			this->log_output->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->log_output->Font = (gcnew System::Drawing::Font(L"Consolas", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->log_output->ForeColor = System::Drawing::SystemColors::Window;
			this->log_output->Location = System::Drawing::Point(6, 19);
			this->log_output->Name = L"log_output";
			this->log_output->ReadOnly = true;
			this->log_output->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::Vertical;
			this->log_output->Size = System::Drawing::Size(753, 125);
			this->log_output->TabIndex = 0;
			this->log_output->Text = L"";
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->start_streaming);
			this->groupBox4->Controls->Add(this->panel_ref_source);
			this->groupBox4->Controls->Add(this->panel_ext_sample_clock);
			this->groupBox4->Controls->Add(this->reset_button);
			this->groupBox4->Controls->Add(this->connect_button);
			this->groupBox4->Controls->Add(this->repeat_count);
			this->groupBox4->Controls->Add(this->segment_size);
			this->groupBox4->Controls->Add(this->label_repeat_count);
			this->groupBox4->Controls->Add(this->label4);
			this->groupBox4->Controls->Add(this->label1);
			this->groupBox4->Controls->Add(this->label_segment_size);
			this->groupBox4->Controls->Add(this->address_box);
			this->groupBox4->Controls->Add(this->label_address);
			this->groupBox4->Controls->Add(this->enable_2);
			this->groupBox4->Controls->Add(this->enable_1);
			this->groupBox4->Controls->Add(this->stop_streaming);
			this->groupBox4->Location = System::Drawing::Point(10, 12);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(765, 164);
			this->groupBox4->TabIndex = 3;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Instrument settings";
			// 
			// stop_streaming
			// 
			this->stop_streaming->Enabled = false;
			this->stop_streaming->Location = System::Drawing::Point(663, 132);
			this->stop_streaming->Name = L"stop_streaming";
			this->stop_streaming->Size = System::Drawing::Size(89, 20);
			this->stop_streaming->TabIndex = 7;
			this->stop_streaming->Text = L"Stop";
			this->stop_streaming->UseVisualStyleBackColor = true;
			this->stop_streaming->Visible = false;
			this->stop_streaming->Click += gcnew System::EventHandler(this, &main_form::abort_streaming);
			// 
			// start_streaming
			// 
			this->start_streaming->Enabled = false;
			this->start_streaming->Location = System::Drawing::Point(663, 132);
			this->start_streaming->Name = L"start_streaming";
			this->start_streaming->Size = System::Drawing::Size(89, 20);
			this->start_streaming->TabIndex = 6;
			this->start_streaming->Text = L"Start";
			this->start_streaming->UseVisualStyleBackColor = true;
			this->start_streaming->Click += gcnew System::EventHandler(this, &main_form::begin_streaming);
			// 
			// panel_ref_source
			// 
			this->panel_ref_source->Controls->Add(this->ext_clock);
			this->panel_ref_source->Controls->Add(this->int_clock);
			this->panel_ref_source->Enabled = false;
			this->panel_ref_source->Location = System::Drawing::Point(526, 57);
			this->panel_ref_source->Name = L"panel_ref_source";
			this->panel_ref_source->Size = System::Drawing::Size(226, 29);
			this->panel_ref_source->TabIndex = 5;
			// 
			// ext_clock
			// 
			this->ext_clock->AutoSize = true;
			this->ext_clock->Location = System::Drawing::Point(131, 9);
			this->ext_clock->Name = L"ext_clock";
			this->ext_clock->Size = System::Drawing::Size(63, 17);
			this->ext_clock->TabIndex = 1;
			this->ext_clock->Text = L"External";
			this->ext_clock->UseVisualStyleBackColor = true;
			this->ext_clock->Click += gcnew System::EventHandler(this, &main_form::set_ref_clock_source_ext);
			// 
			// int_clock
			// 
			this->int_clock->AutoSize = true;
			this->int_clock->Checked = true;
			this->int_clock->Location = System::Drawing::Point(23, 9);
			this->int_clock->Name = L"int_clock";
			this->int_clock->Size = System::Drawing::Size(60, 17);
			this->int_clock->TabIndex = 0;
			this->int_clock->TabStop = true;
			this->int_clock->Text = L"Internal";
			this->int_clock->UseVisualStyleBackColor = true;
			this->int_clock->Click += gcnew System::EventHandler(this, &main_form::set_ref_clock_source_int);
			// 
			// panel_ext_sample_clock
			// 
			this->panel_ext_sample_clock->Controls->Add(this->ext_2);
			this->panel_ext_sample_clock->Controls->Add(this->ext_1);
			this->panel_ext_sample_clock->Enabled = false;
			this->panel_ext_sample_clock->Location = System::Drawing::Point(526, 16);
			this->panel_ext_sample_clock->Name = L"panel_ext_sample_clock";
			this->panel_ext_sample_clock->Size = System::Drawing::Size(226, 29);
			this->panel_ext_sample_clock->TabIndex = 5;
			// 
			// ext_2
			// 
			this->ext_2->AutoSize = true;
			this->ext_2->Checked = true;
			this->ext_2->Location = System::Drawing::Point(131, 9);
			this->ext_2->Name = L"ext_2";
			this->ext_2->Size = System::Drawing::Size(73, 17);
			this->ext_2->TabIndex = 1;
			this->ext_2->TabStop = true;
			this->ext_2->Text = L"Channel 2";
			this->ext_2->UseVisualStyleBackColor = true;
			this->ext_2->Click += gcnew System::EventHandler(this, &main_form::set_external_sample_clock_ch2);
			// 
			// ext_1
			// 
			this->ext_1->AutoSize = true;
			this->ext_1->Location = System::Drawing::Point(23, 9);
			this->ext_1->Name = L"ext_1";
			this->ext_1->Size = System::Drawing::Size(73, 17);
			this->ext_1->TabIndex = 0;
			this->ext_1->Text = L"Channel 1";
			this->ext_1->UseVisualStyleBackColor = true;
			this->ext_1->Click += gcnew System::EventHandler(this, &main_form::set_external_sample_clock_ch1);
			// 
			// reset_button
			// 
			this->reset_button->Enabled = false;
			this->reset_button->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->reset_button->Location = System::Drawing::Point(290, 51);
			this->reset_button->Name = L"reset_button";
			this->reset_button->Size = System::Drawing::Size(75, 20);
			this->reset_button->TabIndex = 4;
			this->reset_button->Text = L"Reset";
			this->reset_button->UseVisualStyleBackColor = true;
			this->reset_button->Click += gcnew System::EventHandler(this, &main_form::reset);
			// 
			// connect_button
			// 
			this->connect_button->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->connect_button->Location = System::Drawing::Point(196, 51);
			this->connect_button->Name = L"connect_button";
			this->connect_button->Size = System::Drawing::Size(75, 20);
			this->connect_button->TabIndex = 4;
			this->connect_button->Text = L"Connect";
			this->connect_button->UseVisualStyleBackColor = true;
			this->connect_button->Click += gcnew System::EventHandler(this, &main_form::init_driver);
			// 
			// repeat_count
			// 
			this->repeat_count->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->repeat_count->Enabled = false;
			this->repeat_count->Location = System::Drawing::Point(110, 132);
			this->repeat_count->Name = L"repeat_count";
			this->repeat_count->Size = System::Drawing::Size(255, 20);
			this->repeat_count->TabIndex = 3;
			// 
			// segment_size
			// 
			this->segment_size->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->segment_size->Enabled = false;
			this->segment_size->Location = System::Drawing::Point(110, 97);
			this->segment_size->Name = L"segment_size";
			this->segment_size->Size = System::Drawing::Size(255, 20);
			this->segment_size->TabIndex = 3;
			// 
			// label_repeat_count
			// 
			this->label_repeat_count->AutoSize = true;
			this->label_repeat_count->Enabled = false;
			this->label_repeat_count->Location = System::Drawing::Point(6, 134);
			this->label_repeat_count->Name = L"label_repeat_count";
			this->label_repeat_count->Size = System::Drawing::Size(75, 13);
			this->label_repeat_count->TabIndex = 2;
			this->label_repeat_count->Text = L"Repeat count:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Enabled = false;
			this->label4->Location = System::Drawing::Point(393, 68);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(124, 13);
			this->label4->TabIndex = 2;
			this->label4->Text = L"Reference clock source:";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Enabled = false;
			this->label1->Location = System::Drawing::Point(393, 27);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(113, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"External sample clock:";
			// 
			// label_segment_size
			// 
			this->label_segment_size->AutoSize = true;
			this->label_segment_size->Enabled = false;
			this->label_segment_size->Location = System::Drawing::Point(6, 99);
			this->label_segment_size->Name = L"label_segment_size";
			this->label_segment_size->Size = System::Drawing::Size(73, 13);
			this->label_segment_size->TabIndex = 2;
			this->label_segment_size->Text = L"Segment size:";
			// 
			// address_box
			// 
			this->address_box->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->address_box->Location = System::Drawing::Point(110, 25);
			this->address_box->Name = L"address_box";
			this->address_box->Size = System::Drawing::Size(255, 20);
			this->address_box->TabIndex = 3;
			// 
			// label_address
			// 
			this->label_address->AccessibleDescription = L"";
			this->label_address->AutoSize = true;
			this->label_address->Location = System::Drawing::Point(6, 28);
			this->label_address->Name = L"label_address";
			this->label_address->Size = System::Drawing::Size(99, 13);
			this->label_address->TabIndex = 2;
			this->label_address->Text = L"Instrument address:";
			// 
			// enable_2
			// 
			this->enable_2->AutoSize = true;
			this->enable_2->Enabled = false;
			this->enable_2->Location = System::Drawing::Point(523, 135);
			this->enable_2->Name = L"enable_2";
			this->enable_2->Size = System::Drawing::Size(109, 17);
			this->enable_2->TabIndex = 1;
			this->enable_2->Text = L"Enable channel 2";
			this->enable_2->UseVisualStyleBackColor = true;
			this->enable_2->CheckedChanged += gcnew System::EventHandler(this, &main_form::enable_2_state_changed);
			// 
			// enable_1
			// 
			this->enable_1->AutoSize = true;
			this->enable_1->Enabled = false;
			this->enable_1->Location = System::Drawing::Point(396, 134);
			this->enable_1->Name = L"enable_1";
			this->enable_1->Size = System::Drawing::Size(109, 17);
			this->enable_1->TabIndex = 0;
			this->enable_1->Text = L"Enable channel 1";
			this->enable_1->UseVisualStyleBackColor = true;
			this->enable_1->CheckedChanged += gcnew System::EventHandler(this, &main_form::enable_1_state_changed);
			// 
			// channel_2
			// 
			this->channel_2->Controls->Add(this->label3);
			this->channel_2->Controls->Add(this->interpolation_2);
			this->channel_2->Controls->Add(this->label_sf_2);
			this->channel_2->Controls->Add(this->sf_2);
			this->channel_2->Controls->Add(this->label_cf_2);
			this->channel_2->Controls->Add(this->cf_2);
			this->channel_2->Controls->Add(this->container_type_ch2);
			this->channel_2->Enabled = false;
			this->channel_2->Location = System::Drawing::Point(397, 179);
			this->channel_2->Margin = System::Windows::Forms::Padding(0);
			this->channel_2->Name = L"channel_2";
			this->channel_2->Padding = System::Windows::Forms::Padding(0);
			this->channel_2->Size = System::Drawing::Size(378, 216);
			this->channel_2->TabIndex = 0;
			this->channel_2->TabStop = false;
			this->channel_2->Text = L"Channel 2";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(6, 98);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(98, 13);
			this->label3->TabIndex = 10;
			this->label3->Text = L"Interpolation factor:";
			// 
			// interpolation_2
			// 
			this->interpolation_2->DisplayMember = L"0";
			this->interpolation_2->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->interpolation_2->FormatString = L"E2";
			this->interpolation_2->FormattingEnabled = true;
			this->interpolation_2->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"3", L"12", L"24", L"48" });
			this->interpolation_2->Location = System::Drawing::Point(110, 95);
			this->interpolation_2->Name = L"interpolation_2";
			this->interpolation_2->Size = System::Drawing::Size(255, 21);
			this->interpolation_2->TabIndex = 11;
			// 
			// label_sf_2
			// 
			this->label_sf_2->AutoSize = true;
			this->label_sf_2->Location = System::Drawing::Point(6, 62);
			this->label_sf_2->Name = L"label_sf_2";
			this->label_sf_2->Size = System::Drawing::Size(95, 13);
			this->label_sf_2->TabIndex = 7;
			this->label_sf_2->Text = L"Sample frequency:";
			// 
			// sf_2
			// 
			this->sf_2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->sf_2->Location = System::Drawing::Point(110, 60);
			this->sf_2->Name = L"sf_2";
			this->sf_2->Size = System::Drawing::Size(255, 20);
			this->sf_2->TabIndex = 5;
			// 
			// label_cf_2
			// 
			this->label_cf_2->AutoSize = true;
			this->label_cf_2->Location = System::Drawing::Point(6, 27);
			this->label_cf_2->Name = L"label_cf_2";
			this->label_cf_2->Size = System::Drawing::Size(93, 13);
			this->label_cf_2->TabIndex = 4;
			this->label_cf_2->Text = L"Central frequency:";
			// 
			// cf_2
			// 
			this->cf_2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->cf_2->Location = System::Drawing::Point(110, 25);
			this->cf_2->Name = L"cf_2";
			this->cf_2->Size = System::Drawing::Size(255, 20);
			this->cf_2->TabIndex = 0;
			// 
			// container_type_ch2
			// 
			this->container_type_ch2->Controls->Add(this->pick_file_2);
			this->container_type_ch2->Controls->Add(this->alg_stream_2);
			this->container_type_ch2->Controls->Add(this->label_filepath_2);
			this->container_type_ch2->Controls->Add(this->file_stream_2);
			this->container_type_ch2->Controls->Add(this->file_path_2);
			this->container_type_ch2->Location = System::Drawing::Point(9, 131);
			this->container_type_ch2->Name = L"container_type_ch2";
			this->container_type_ch2->Size = System::Drawing::Size(356, 80);
			this->container_type_ch2->TabIndex = 9;
			// 
			// pick_file_2
			// 
			this->pick_file_2->Enabled = false;
			this->pick_file_2->Location = System::Drawing::Point(291, 55);
			this->pick_file_2->Name = L"pick_file_2";
			this->pick_file_2->Size = System::Drawing::Size(65, 20);
			this->pick_file_2->TabIndex = 8;
			this->pick_file_2->Text = L"Open file";
			this->pick_file_2->UseVisualStyleBackColor = true;
			this->pick_file_2->Click += gcnew System::EventHandler(this, &main_form::open_file);
			// 
			// alg_stream_2
			// 
			this->alg_stream_2->AutoSize = true;
			this->alg_stream_2->Checked = true;
			this->alg_stream_2->Location = System::Drawing::Point(0, 4);
			this->alg_stream_2->Name = L"alg_stream_2";
			this->alg_stream_2->Size = System::Drawing::Size(124, 17);
			this->alg_stream_2->TabIndex = 1;
			this->alg_stream_2->TabStop = true;
			this->alg_stream_2->Text = L"Algorithmic streaming";
			this->alg_stream_2->UseVisualStyleBackColor = true;
			this->alg_stream_2->Click += gcnew System::EventHandler(this, &main_form::set_alg_streaming_2);
			// 
			// label_filepath_2
			// 
			this->label_filepath_2->AutoSize = true;
			this->label_filepath_2->Enabled = false;
			this->label_filepath_2->Location = System::Drawing::Point(16, 59);
			this->label_filepath_2->Name = L"label_filepath_2";
			this->label_filepath_2->Size = System::Drawing::Size(50, 13);
			this->label_filepath_2->TabIndex = 7;
			this->label_filepath_2->Text = L"File path:";
			// 
			// file_stream_2
			// 
			this->file_stream_2->AutoSize = true;
			this->file_stream_2->Location = System::Drawing::Point(0, 27);
			this->file_stream_2->Name = L"file_stream_2";
			this->file_stream_2->Size = System::Drawing::Size(89, 17);
			this->file_stream_2->TabIndex = 2;
			this->file_stream_2->Text = L"File streaming";
			this->file_stream_2->UseVisualStyleBackColor = true;
			this->file_stream_2->Click += gcnew System::EventHandler(this, &main_form::set_file_streaming_2);
			// 
			// file_path_2
			// 
			this->file_path_2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->file_path_2->Enabled = false;
			this->file_path_2->Location = System::Drawing::Point(72, 55);
			this->file_path_2->Name = L"file_path_2";
			this->file_path_2->Size = System::Drawing::Size(213, 20);
			this->file_path_2->TabIndex = 5;
			// 
			// main_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(784, 561);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->channel_2);
			this->Controls->Add(this->channel_1);
			this->MaximizeBox = false;
			this->Name = L"main_form";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"m8190a streaming";
			this->channel_1->ResumeLayout(false);
			this->channel_1->PerformLayout();
			this->container_type_ch1->ResumeLayout(false);
			this->container_type_ch1->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->panel_ref_source->ResumeLayout(false);
			this->panel_ref_source->PerformLayout();
			this->panel_ext_sample_clock->ResumeLayout(false);
			this->panel_ext_sample_clock->PerformLayout();
			this->channel_2->ResumeLayout(false);
			this->channel_2->PerformLayout();
			this->container_type_ch2->ResumeLayout(false);
			this->container_type_ch2->PerformLayout();
			this->ResumeLayout(false);

		}

#pragma endregion

	private: 
		System::Void init_driver(System::Object^ sender, System::EventArgs^ e);

		System::Void begin_streaming(System::Object^ sender, System::EventArgs^ e);

		System::Void enable_1_state_changed(System::Object^ sender, System::EventArgs^ e);

		System::Void enable_2_state_changed(System::Object^ sender, System::EventArgs^ e);

		System::Void set_alg_streaming_1(System::Object^ sender, System::EventArgs^ e);

		System::Void set_alg_streaming_2(System::Object^ sender, System::EventArgs^ e);

		System::Void set_file_streaming_1(System::Object^ sender, System::EventArgs^ e);

		System::Void set_file_streaming_2(System::Object^ sender, System::EventArgs^ e);

		System::Void set_external_sample_clock_ch1(System::Object^ sender, System::EventArgs^ e);

		System::Void set_external_sample_clock_ch2(System::Object^ sender, System::EventArgs^ e);

		System::Void set_ref_clock_source_int(System::Object^ sender, System::EventArgs^ e);

		System::Void set_ref_clock_source_ext(System::Object^ sender, System::EventArgs^ e);

		System::Void reset(System::Object^ sender, System::EventArgs^ e);

		System::Void print_log(String^ message);

		System::Void send_log(std::string message);

		System::Void open_file(System::Object^ sender, System::EventArgs^ e);

		void async_streaming();

		System::Void abort_streaming(System::Object^ sender, System::EventArgs^ e);

		System::Void read_config();
};
}
