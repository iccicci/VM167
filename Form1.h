#pragma once

#include "logic.h"

namespace VM167Demo
{


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
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		LogicInterface  Logic;
		System::Boolean Ok;
		System::Boolean Running;

	private:
		System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e)
		{
			Ok = Logic.init();

			if (Ok)
			{
				Logic.connect();
				logBox->Font = gcnew System::Drawing::Font(logBox->Font->FontFamily, Logic.getFontSize());
			}
		}
		System::Void Button1_Click(System::Object^  sender, System::EventArgs^  e)
		{
			Logic.clearScreen(0);
		}
		System::Void Button2_Click(System::Object^  sender, System::EventArgs^  e)
		{
			Logic.clearScreen(1);
		}
		System::Void Button3_Click(System::Object^  sender, System::EventArgs^  e)
		{
			Logic.clearScreen(2);
		}
		System::Void Button4_Click(System::Object^  sender, System::EventArgs^  e)
		{
			Logic.clearScreen(3);
		}
		System::Void Timer1_Tick(System::Object^  sender, System::EventArgs^  e)
		{
			if (Running) return;

			Running = true;

			struct TickInfo* logs = Logic.tick();

			if (logs->log != nullptr)
			{
				logBox->Text = gcnew String(logs->log);
				logBox->SelectionStart = logBox->TextLength;
				logBox->SelectionLength = 0;
				logBox->ScrollToCaret();
				logs->log = nullptr;
			}

			Button1->Enabled = logs->button == 0;
			Button2->Enabled = logs->button == 1;
			Button3->Enabled = logs->button == 2;
			Button4->Enabled = logs->button == 3;

			Running = false;
		}
	};
}
