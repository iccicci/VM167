#pragma once

#include "logic.h"

namespace VM167Demo {


	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices; // needed for DllImport
	using namespace logic;

	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			Running = false;
		}

	protected:
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	internal: System::Windows::Forms::Button^  Button3;
	internal: System::Windows::Forms::Button^  Button2;
	internal: System::Windows::Forms::Button^  Button4;
	internal: System::Windows::Forms::Timer^  Timer1;
	internal: System::Windows::Forms::Button^  Button1;
	internal: System::Windows::Forms::TextBox^  logBox;

	private: System::ComponentModel::IContainer^  components;

	private:

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->Button3 = (gcnew System::Windows::Forms::Button());
			this->Button2 = (gcnew System::Windows::Forms::Button());
			this->Button4 = (gcnew System::Windows::Forms::Button());
			this->Timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->Button1 = (gcnew System::Windows::Forms::Button());
			this->logBox = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// Button3
			// 
			this->Button3->Enabled = false;
			this->Button3->Location = System::Drawing::Point(13, 14);
			this->Button3->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->Button3->Name = L"Button3";
			this->Button3->Size = System::Drawing::Size(134, 40);
			this->Button3->TabIndex = 30;
			this->Button3->Text = L"Cancella log";
			this->Button3->UseVisualStyleBackColor = true;
			this->Button3->Click += gcnew System::EventHandler(this, &Form1::Button3_Click);
			// 
			// Button2
			// 
			this->Button2->Enabled = false;
			this->Button2->Location = System::Drawing::Point(297, 14);
			this->Button2->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->Button2->Name = L"Button2";
			this->Button2->Size = System::Drawing::Size(134, 40);
			this->Button2->TabIndex = 29;
			this->Button2->Text = L"Cancella log";
			this->Button2->UseVisualStyleBackColor = true;
			this->Button2->Click += gcnew System::EventHandler(this, &Form1::Button2_Click);
			// 
			// Button4
			// 
			this->Button4->Enabled = false;
			this->Button4->Location = System::Drawing::Point(439, 14);
			this->Button4->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->Button4->Name = L"Button4";
			this->Button4->Size = System::Drawing::Size(134, 40);
			this->Button4->TabIndex = 1;
			this->Button4->Text = L"Cancella log";
			this->Button4->UseVisualStyleBackColor = true;
			this->Button4->Click += gcnew System::EventHandler(this, &Form1::Button4_Click);
			// 
			// Timer1
			// 
			this->Timer1->Enabled = true;
			this->Timer1->Tick += gcnew System::EventHandler(this, &Form1::Timer1_Tick);
			// 
			// Button1
			// 
			this->Button1->Location = System::Drawing::Point(155, 14);
			this->Button1->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->Button1->Name = L"Button1";
			this->Button1->Size = System::Drawing::Size(134, 40);
			this->Button1->TabIndex = 24;
			this->Button1->Text = L"Cancella log";
			this->Button1->UseVisualStyleBackColor = true;
			this->Button1->Click += gcnew System::EventHandler(this, &Form1::Button1_Click);
			// 
			// logBox
			// 
			this->logBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
																																								 | System::Windows::Forms::AnchorStyles::Left)
																																								| System::Windows::Forms::AnchorStyles::Right));
			this->logBox->BackColor = System::Drawing::SystemColors::Window;
			this->logBox->Location = System::Drawing::Point(13, 62);
			this->logBox->MaxLength = 0;
			this->logBox->Multiline = true;
			this->logBox->Name = L"logBox";
			this->logBox->ReadOnly = true;
			this->logBox->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->logBox->Size = System::Drawing::Size(1172, 456);
			this->logBox->TabIndex = 39;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(9, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1197, 530);
			this->Controls->Add(this->Button4);
			this->Controls->Add(this->logBox);
			this->Controls->Add(this->Button3);
			this->Controls->Add(this->Button2);
			this->Controls->Add(this->Button1);
			this->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->Name = L"Form1";
			this->Text = L"VM167";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Form1::Form1_FormClosed);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
				      
				int CardAddress;
				int HiNibble;
				int LoNibble;
				int Cards;

				LogicInterface  Logic;
				System::Boolean Ok;
				System::Boolean Running;

				// Function prototypes to call from VM167.dll
				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static int OpenDevices();

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void CloseDevices();

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static int ReadAnalogChannel(int CardAddress, int Channel);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void ReadAllAnalog(int CardAddress, int *Buffer);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void SetPWM(int CardAddress, int Channel, int Data, int Freq);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void OutputAllPWM(int CardAddress, int Data1, int Data2);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void OutputAllDigital(int CardAddress, int Data);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void ClearDigitalChannel(int CardAddress, int Channel);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void ClearAllDigital(int CardAddress);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void SetDigitalChannel(int CardAddress, int Channel);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void SetAllDigital(int CardAddress);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static bool ReadDigitalChannel(int CardAddress, int Channel);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				 static int ReadAllDigital(int CardAddress);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void InOutMode(int CardAddress, int HighNibble, int LowNibble);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static unsigned int ReadCounter(int CardAddress);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void ResetCounter(int CardAddress);
								
				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static int Connected();

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static int VersionFirmware(int CardAddress);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static int VersionDLL();

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				static void ReadBackPWMOut(int CardAddress, int *Buffer);

				[DllImport("vm167.dll", CharSet=CharSet::Ansi)]
				 static int ReadBackInOutMode(int CardAddress);

	private: System::Void Button1_Click(System::Object^  sender, System::EventArgs^  e) {

		}
private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) 
		 {
			Ok = Logic.init();

			if (Ok)
			{
				Logic.connect();
				logBox->Font = gcnew System::Drawing::Font(logBox->Font->FontFamily, Logic.getFontSize());
			}
		 }
private: System::Void Form1_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e)
		 {
			CloseDevices();
		 }
private: System::Void Button2_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
		 }
private: System::Void Button3_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
		 }
private: System::Void Timer1_Tick(System::Object^  sender, System::EventArgs^  e)
		 {
	if (Running) return;

	Running = true;

	const char* logs = Logic.tick();

	if (logs != nullptr)
	{
		logBox->Text = gcnew String(logs);
		logBox->SelectionStart = logBox->TextLength;
		logBox->SelectionLength = 0;
		logBox->ScrollToCaret();
	}

	Running = false;
}
private: System::Void Button4_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
		 }
	};
}

