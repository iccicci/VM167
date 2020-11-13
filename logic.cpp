#include "stdafx.h"

#include <time.h>

#include <chrono>
#include <string>
#include <vector>

#include "logic.h"
#include "SimpleIni.h"

using namespace System::Runtime::InteropServices;

[DllImport("vm167.dll", CharSet = CharSet::Ansi)]
int Connected();

[DllImport("vm167.dll", CharSet = CharSet::Ansi)]
void InOutMode(int CardAddress, int HighNibble, int LowNibble);

[DllImport("vm167.dll", CharSet = CharSet::Ansi)]
int OpenDevices();

[DllImport("vm167.dll", CharSet = CharSet::Ansi)]
void ReadAllAnalog(int CardAddress, int *Buffer);

[DllImport("vm167.dll", CharSet = CharSet::Ansi)]
int ReadAllDigital(int CardAddress);

[DllImport("vm167.dll", CharSet = CharSet::Ansi)]
void SetPWM(int CardAddress, int Channel, int Data, int Freq);

#define NUM_CARDS 2
#define NUM_ANALOG 5
#define NUM_DIGITAL 8

using namespace logic;
using namespace std;
using namespace std::chrono;

private class PortConfiguration
{
public:
	bool   configured;
	bool   hasOn;
	bool   hasOff;
	string label;
	int    offCard;
	int    offNumber;
	int    onCard;
	int    onNumber;
	string wrongOff;
	string wrongOn;

	PortConfiguration()
	{
		configured = hasOff = hasOn = false;
	}
};

private class CardConfiguration
{
public:
	bool configured;
	bool present;

	PortConfiguration A[NUM_ANALOG];
	PortConfiguration D[NUM_DIGITAL];

	CardConfiguration()
	{
		configured = present = false;
	}
};

class logic::Logic
{
	friend LogicInterface;

private:

	bool           connectionError;
	int            devs;
	float          fontSize;
	string         lines;
	bool           logError;
	vector<string> logLines;
	unsigned int   maxLogLines;
	bool           newLogs;
	string         script;
	long long      state[NUM_CARDS][NUM_ANALOG + NUM_DIGITAL];

	CardConfiguration cards[NUM_CARDS];

	vector<PROCESS_INFORMATION> subProcesses;

	Logic()
	{
		connectionError = false;
		fontSize = 10;
		logError = false;
		maxLogLines = 1000;
		newLogs = false;

		ZeroMemory(state, sizeof(state));
	}

	void cleanConfig()
	{
		for (int l = 0; l < NUM_CARDS; ++l)
		{
			if (!cards[l].configured) continue;

			if (l + 1 == devs)
			{
				for (int i = 0; i < NUM_DIGITAL; ++i)
				{
					PortConfiguration &port = cards[l].D[i];

					if (!port.configured) continue;

					if (port.hasOn && port.onCard != l + 1)
					{
						log(string("*** Ignorata configurazione 'On = O") + to_string(port.onCard) + to_string(port.onNumber) + "' per la porta D" + to_string(l + 1) + to_string(i + 1));
						port.hasOn = false;
					}

					if (port.hasOff && port.offCard != l + 1)
					{
						log(string("*** Ignorata configurazione 'Off = O") + to_string(port.offCard) + to_string(port.offNumber) + "' per la porta D" + to_string(l + 1) + to_string(i + 1));
						port.hasOff = false;
					}
				}

				for (int i = 0; i < NUM_ANALOG; ++i)
				{
					PortConfiguration &port = cards[l].A[i];

					if (!port.configured) continue;

					if (port.hasOn && port.onCard != l + 1)
					{
						log(string("*** Ignorata configurazione 'On = O") + to_string(port.onCard) + to_string(port.onNumber) + "' per la porta A" + to_string(l + 1) + to_string(i + 1));
						port.hasOn = false;
					}

					if (port.hasOff && port.offCard != l + 1)
					{
						log(string("*** Ignorata configurazione 'Off = O") + to_string(port.offCard) + to_string(port.offNumber) + "' per la porta A" + to_string(l + 1) + to_string(i + 1));
						port.hasOn = false;
					}
				}
			}
			else
			{
				for (int i = 0; i < NUM_DIGITAL; ++i)
				{
					PortConfiguration &port = cards[l].D[i];

					if (!port.configured) continue;

					log(string("*** Ignorata configurazione porta D") + to_string(l + 1) + to_string(i + 1));
					port.configured = false;
				}

				for (int i = 0; i < NUM_ANALOG; ++i)
				{
					PortConfiguration &port = cards[l].A[i];

					if (!port.configured || port.label == "" || port.wrongOn != "" || port.wrongOff != "") continue;

					log(string("*** Ignorata configurazione porta A") = to_string(l + 1) + to_string(i + 1));
					port.configured = false;
				}
			}
		}
	}

	bool connect()
	{
		switch (devs = OpenDevices())
		{
		case 1:
		case 2:
			log(string("*** Trovata solo la scheda ") + to_string(devs));
			InOutMode(devs - 1, 1, 1);
			cleanConfig();
			connectionError = false;

			return true;

		case 3:
			log("*** Trovate entrambe le schede");
			InOutMode(0, 1, 1);
			InOutMode(1, 1, 1);
			connectionError = false;

			return true;
		}

		if (!connectionError)
		{
			if (devs) log("*** NESSUNA SCHEDA TROVATA");
			else log("*** ERRORE DURANTE LA CONNESSIONE ALLA/E SCHEDA/E");
		}

		connectionError = true;

		return false;
	}

	bool init()
	{
		CSimpleIniCaseA ini;
		vector<string>  wrongSections;

		SI_Error rc = ini.LoadFile("VM167.ini");
		if (rc < 0)
		{
			if (rc == SI_FILE)	log("VM167.ini: File non trovato");
			else log("VM167.ini: Errore generico");

			log("*** IMPOSSIBILE PROSEGUIRE!");

			return false;
		}

		fontSize = (float)ini.GetLongValue("LOG", "DimensioneCarattere", 10);
		maxLogLines = ini.GetLongValue("LOG", "MassimoLinee", 1000);
		script = ini.GetValue("SCRIPT", "NomeFile", "");

		CSimpleIniCaseA::TNamesDepend sections;

		ini.GetAllSections(sections);

		for (auto i = sections.begin(); i != sections.end(); ++i)
		{
			int  card;
			int  number;
			char type;

			const char*        name = i->pItem;
			PortConfiguration* port;
			const char*        value;

			if (!strcmp(name, "LOG")) continue;
			if (!strcmp(name, "SCRIPT")) continue;

			if (!isPortName(name, type, card, number))
			{
				wrongSections.push_back(name);
				continue;
			}

			if (type == 'O')
			{
				wrongSections.push_back(name);
				continue;
			}

			cards[card - 1].configured = true;

			if (type == 'A') port = &cards[card - 1].A[number - 1];
			if (type == 'D') port = &cards[card - 1].D[number - 1];

			port->configured = true;
			port->label = ini.GetValue(name, "Label", "");

			value = ini.GetValue(name, "On", "");
			if (strcmp(value, ""))
			{
				if (isPortName(value, type, card, number))
				{
					if (type == 'O')
					{
						port->hasOn = true;
						port->onCard = card;
						port->onNumber = number;
					}
					else port->wrongOn = value;
				}
				else port->wrongOn = value;
			}

			value = ini.GetValue(name, "Off", "");
			if (strcmp(value, ""))
			{
				if (isPortName(value, type, card, number))
				{
					if (type == 'O')
					{
						port->hasOff = true;
						port->offCard = card;
						port->offNumber = number;
					}
					else port->wrongOff = value;
				}
				else port->wrongOff = value;
			}
		}

		log("*** Configurazione");
		log("");
		log("[LOG]");
		log(string("DimensioneCarattere = ") + to_string((int)fontSize));
		log(string("MassimoLinee = ") + to_string(maxLogLines));
		log("");
		log("[SCRIPT]");
		log(string("NomeFile = ") + script);
		log("");
		for (int l = 0; l < NUM_CARDS; ++l)
		{
			if (!cards[l].configured) continue;

			for (int i = 0; i < NUM_DIGITAL; ++i)
			{
				PortConfiguration &port = cards[l].D[i];

				if (!port.configured || port.label == "" || port.wrongOn != "" || port.wrongOff != "") continue;

				log(string("[D") + to_string(l + 1) + to_string(i + 1) + "]");
				log(string("Label = ") + port.label);

				if (port.hasOn) log(string("On = O") + to_string(port.onCard) + to_string(port.onNumber));
				if (port.hasOff) log(string("Off = O") + to_string(port.offCard) + to_string(port.offNumber));

				log("");
			}

			for (int i = 0; i < NUM_ANALOG; ++i)
			{
				PortConfiguration &port = cards[l].A[i];

				if (!port.configured || port.label == "" || port.wrongOn != "" || port.wrongOff != "") continue;

				log(string("[A") + to_string(l + 1) + to_string(i + 1) + "]");
				log(string("Label = ") + port.label);

				if (port.hasOn) log(string("On = O") + to_string(port.onCard) + to_string(port.onNumber));
				if (port.hasOff) log(string("Off = O") + to_string(port.offCard) + to_string(port.offNumber));

				log("");
			}
		}
		log("");
		log("*** Fine configurazione");

		if (script == "") log("*** Nessuno script configurato");

		for (auto i = wrongSections.begin(); i != wrongSections.end(); ++i) log(string("*** Sezione sconosciuta: ") + *i);

		for (int l = 0; l < NUM_CARDS; ++l)
		{
			if (!cards[l].configured) continue;

			for (int i = 0; i < NUM_DIGITAL; ++i)
			{
				PortConfiguration &port = cards[l].D[i];

				if (!port.configured) continue;

				if (port.label == "")
				{
					log(string("*** Label mancante per la porta D") + to_string(l + 1) + to_string(i + 1));
					port.configured = false;
				}
				if (port.wrongOn != "")
				{
					log(string("*** On errato per la porta D") + to_string(l + 1) + to_string(i + 1) + ": " + port.wrongOn);
					port.configured = false;
				}
				if (port.wrongOff != "")
				{
					log(string("*** Off errato per la porta D") + to_string(l + 1) + to_string(i + 1) + ": " + port.wrongOff);
					port.configured = false;
				}
			}

			for (int i = 0; i < NUM_ANALOG; ++i)
			{
				PortConfiguration &port = cards[l].A[i];

				if (!port.configured) continue;

				if (port.label == "")
				{
					log(string("*** Label mancante per la porta A") + to_string(l + 1) + to_string(i + 1));
					port.configured = false;
				}
				if (port.wrongOn != "")
				{
					log(string("*** On errato per la porta A") + to_string(l + 1) + to_string(i + 1) + ": " + port.wrongOn);
					port.configured = false;
				}
				if (port.wrongOff != "")
				{
					log(string("*** Off errato per la porta A") + to_string(l + 1) + to_string(i + 1) + ": " + port.wrongOff);
					port.configured = false;
				}
			}
		}

		return true;
	}

	bool isPortName(const char* name, char& type, int& card, int& number)
	{
		switch (name[0])
		{
		case 'A':
		case 'D':
		case 'O':
			type = name[0];
			break;
		default:
			return false;
		}

		switch (name[1])
		{
		case '1':
		case '2':
			card = name[1] - '0';
			break;
		default:
			return false;
		}

		switch (name[2])
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			number = name[2] - '0';
			break;
		default:
			return false;
		}

		if (card == 'A' && number > NUM_ANALOG) return false;
		if (card == 'O' && number > NUM_CARDS)  return false;

		return true;
	}

	void log(string &line)
	{
		log(line.c_str());
	}

	void log(const char *line)
	{
		string    finalLine;
		time_t    now = time(0);
		struct tm tstruct;
		char      dateTime[80];

		lines = "";
		newLogs = true;

		if (!strcmp(line, "15"))
		{
			dateTime[0] = 0;
		}

		localtime_s(&tstruct, &now);

		strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %X - ", &tstruct);
		finalLine = dateTime;
		finalLine += line;

		FILE *fp;

		if (fopen_s(&fp, "VM167.log", "a"))
		{
			if (!logError)
			{
				logError = true;
				log("VM167.log: ERRORE DURANTE LA CREAZIONE DEL FILE DI LOG");
				log("NESSUN LOG SALVATO");
			}
		}
		else
		{
			string logLine = finalLine;

			logLine += "\n";
			fputs(logLine.c_str(), fp);
			fclose(fp);
		}

		logLines.push_back(finalLine);
		while (logLines.size() > maxLogLines) logLines.erase(logLines.begin());

		for (auto i = logLines.begin(); i != logLines.end(); ++i)
		{
			if (i != logLines.begin()) lines += "\r\n";

			lines += *i;
		}
	}

	void subProcess(string& who)
	{
		if (script == "") return;

		char         commandLine[1000];
		time_t       now = time(0);
		STARTUPINFOA si;
		struct tm    tstruct;
		char         timestamp[80];

		PROCESS_INFORMATION pi;

		ZeroMemory(&pi, sizeof(pi));
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		localtime_s(&tstruct, &now);
		strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %X", &tstruct);
		strcpy_s(commandLine, (script + " \"" + who + "\" \"" + timestamp + "\"").c_str());

		if (!CreateProcessA(NULL, commandLine, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		{
			log(script + ": Errore");

			return;
		}

		subProcesses.push_back(pi);
	}

	void tick()
	{
		waitSubProcesses();

		if (connectionError)
		{
			connect();

			return;
		}

		if (devs != Connected())
		{
			if (!connectionError) log("*** ERRORE DI COMUNICAZIONE CON LA/E SCHEDA/E");
			connectionError = true;

			return;
		}

		int       buffer[NUM_ANALOG];
		int       input;
		long long now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

		for (int l = 0; l < NUM_CARDS; ++l)
		{
			ReadAllAnalog(l, buffer);

			for (int i = 0; i < NUM_ANALOG; ++i)
			{
				long long&         status = state[l][i];
				PortConfiguration& port = cards[l].A[i];

				if (!port.configured)continue;

				if (buffer[i] > 512)
				{
					switch (status)
					{
					case 0:
						status = now;
					case -1:
						break;
					default:
						if (now - status > 500)
						{
							status = -1;
							log(port.label);
							subProcess(port.label);

							if (port.hasOn) SetPWM(port.onCard - 1, port.onNumber, 255, 2);
							if (port.hasOff) SetPWM(port.offCard - 1, port.offNumber, 0, 2);
						}
					}
				}
				else status = 0;
			}

			input = ReadAllDigital(l);

			for (int i = 0; i < NUM_DIGITAL; ++i)
			{
				long long&         status = state[l][NUM_ANALOG + i];
				PortConfiguration& port = cards[l].D[i];

				if (!port.configured)continue;

				if (input & (1 << i))
				{
					switch (status)
					{
					case 0:
						status = now;
					case -1:
						break;
					default:
						if (now - status > 500)
						{
							status = -1;
							log(port.label);
							subProcess(port.label);

							if (port.hasOn) SetPWM(port.onCard - 1, port.onNumber, 255, 2);
							if (port.hasOff) SetPWM(port.offCard - 1, port.offNumber, 0, 2);
						}
					}
				}
				else status = 0;
			}
		}
	}

	void waitSubProcesses()
	{
		for (auto i = subProcesses.begin(); i != subProcesses.end(); ++i)
		{
			int ret = WaitForSingleObject(i->hProcess, 0);

			if (ret == WAIT_TIMEOUT) return;
			if (ret == WAIT_OBJECT_0)
			{
				CloseHandle(i->hProcess);
				CloseHandle(i->hThread);
				subProcesses.erase(i);

				return;
			}

			log("Errore inaspettato nello script");
		}
	}
};

LogicInterface::LogicInterface()
{
	logic = new Logic();
}

LogicInterface::~LogicInterface()
{
	delete logic;
}

System::Boolean LogicInterface::connect()
{
	return logic->connect();
}

float LogicInterface::getFontSize()
{
	return logic->fontSize;
}

System::Boolean LogicInterface::init()
{
	return logic->init();
}

const char* LogicInterface::tick()
{
	logic->tick();

	if (logic->newLogs)
	{
		logic->newLogs = false;

		return logic->lines.c_str();
	}

	return nullptr;
}
