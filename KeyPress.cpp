#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>

#define MAX_SPLIT 1024
#define MAX_KEYS  100

typedef struct split_t {
	char* tokens[MAX_SPLIT];
	size_t count;
} SPLIT,*PSPLIT;

PSPLIT split(char* str, char* delim)
{
	PSPLIT sp = (PSPLIT)malloc(sizeof(SPLIT));
	if (sp == NULL) ExitProcess(-1);
	size_t count = 0;
	char* token = strtok(str, delim);

	while (token != NULL)
	{
		sp->tokens[count++] = token;
		token = strtok(NULL, delim);
	}

	sp->count = count;
	return sp;
}

typedef uint32_t UINT;

int getFileSize(char* filename)
{
	FILE* fp;
	int size = 0;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		perror("Error opening file: ");
		exit(-1);
	}
	fseek(fp, 0L, SEEK_SET);
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	return size;
}

class Queue {

	private:
		std::vector<UINT> items;

	public:
		Queue(){}

		void enqueue(UINT item)
		{
			items.push_back(item);
		}

		UINT dequeue()
		{
			UINT element = items[0];
			if(items.size() == 0)
			{
				return 0;
			}
			items.erase(items.begin());
			return element;
		}

};

/**
 * @brief Generates virtual keypresses
 * @param filename an std::string object specifying filename of keyFile
 * @return KeyPress Object
 * 
 */
class KeyWriter
{
private:
	Queue HoldQueue;
	INPUT InputKeys[2];
	PSPLIT sp;
	char* contents;
	const char *hold = "[HOLD]";
	const char *delay = "[DELAY]";
	const char *release = "[RELEASE]";
	const char *type = "[TYPE]";
	size_t keysLen = 93;
	size_t codesLen = 93;
	std::map<std::string,UINT> KeyMap;

	std::string keys[MAX_KEYS] = {
		"[CANCEL]","[BACKSPACE]",
		"[TAB]","[CLEAR]","[ALT]",
		"[SHIFT]","[CONTROL]","[MENU]",
		"[PAUSE]","[CAPITAL]","[ESC]",
		"[SPACE]","[PAGEUP]","[PAGEDOWN]",
		"[END]","[HOME]","[LEFT]","[UP]",
		"[RIGHT]","[DOWN]","[SELECT]",
		"[PRINT]","[EXECUTE]","[SNAPSHOT]",
		"[INSERT]","[DELETE]","[HELP]",
		"0","1","2","3","4","5","6","7","8",
		"9","A","B","C","D","E","F","G","H",
		"I","J","K","L","M","N","O","P","Q",
		"R","S","T","U","V","W","X","Y","Z",
		"[LEFTWIN]","[RIGHTWIN]","*","+","|",
		"-",".","/","[F1]","[F2]","[F3]","[F4]",
		"[F5]","[F6]","[F7]","[F8]","[F9]","[F10]",
		"[F11]","[F12]","[NUMLOCK]","[SCROLL]",
		"[LSHIFT]","[RSHIFT]","[LCONTROL]","[RCONTROL]",
		"[LMENU]","[VOL_MUTE]","[VOL_DOWN]","[VOL_UP]"
	};

	UINT keycodes[MAX_KEYS] = {
		VK_CANCEL,VK_BACK,
		VK_TAB,VK_CLEAR,VK_RETURN,
		VK_SHIFT,VK_CONTROL,VK_MENU,
		VK_PAUSE,VK_CAPITAL,VK_ESCAPE,
		VK_SPACE,VK_PRIOR,VK_NEXT,
		VK_END,VK_HOME,VK_LEFT,VK_UP,
		VK_RIGHT,VK_DOWN,VK_SELECT,
		VK_PRINT,VK_EXECUTE,VK_SNAPSHOT,
		VK_INSERT,VK_DELETE,VK_HELP,
		0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
		0x39,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,
		0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,
		0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,
		VK_LWIN,VK_RWIN,VK_MULTIPLY,VK_ADD,VK_SEPARATOR,
		VK_SUBTRACT,VK_DECIMAL,VK_DIVIDE,VK_F1,VK_F2,VK_F3,VK_F4,
		VK_F5,VK_F6,VK_F7,VK_F8,VK_F9,VK_F10,
		VK_F11,VK_F12,VK_NUMLOCK,VK_SCROLL,
		VK_LSHIFT,VK_RSHIFT,VK_LCONTROL,VK_RCONTROL,
		VK_LMENU,VK_VOLUME_DOWN,VK_VOLUME_DOWN,VK_VOLUME_UP
	};

	void Delay(char* time)
	{
		DWORD duration = atof(time);
		Sleep(duration);
	}


	void Type(char* text)
	{	
		INPUT inputs[2];
		char *UpperText = AnsiUpper(text);
		UINT code;

		printf("%s\n",UpperText);

		for(size_t k = 0; k < strlen(UpperText); k++)
		{
			char* c = new char[2];
			c[0] = UpperText[k];
			c[1] = '\0';
			std::string letter = c;
			code = this->KeyMap[letter];

			printf("Code: %d\n",code);

			inputs[0].ki.wVk = code;
			inputs[0].type = INPUT_KEYBOARD;

			inputs[1].ki.wVk = code;
			inputs[1].type = INPUT_KEYBOARD;
			inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

			SendInput(2,inputs,sizeof(INPUT));
			delay(50);
			delete[] c;
		}
	
	}

	void initMap()
	{
		for(int s = 0; s < this->keysLen; s++)
		{
			this->KeyMap[keys[s]] = keycodes[s];
		}
	}

	int FindMatch(const char** strarr, size_t strArrlen,char* token)
	{
		int j = 0;
		while (j < strArrlen)
		{
			if (strstr(token,strarr[j]))
			{
				return j;
			}
			j++;
		}
		return -1;
	}

public:
	std::string filename;
	size_t VKeyCount = 0;
	KeyWriter(std::string filename)
	{
		filename = filename;
        size_t filesize = getFileSize((char*)filename.c_str());
		FILE* fp;
		errno_t err = fopen_s(&fp, filename.c_str(), "r");
		if (err)
		{
			perror("Error opening file: ");
			ExitProcess(-1);
		}
		contents = (char*)malloc(filesize + 1);
		if (contents != NULL)
		{
			fread(this->contents, 1,filesize + 1,fp);
		}
		sp = split(this->contents, (char*)"\n");
		fclose(fp);
	}

    virtual ~KeyWriter(){};

	UINT GenKeyPress()
	{
		this->initMap();
		for (int i = 0; i < sp->count; i++)
		{
			PSPLIT sp_space = split(sp->tokens[i],(char*)" ");
            for (int j = 0; j < sp_space->count; j++)
            {

                INPUT inDn;
                INPUT inUp;
				UINT key = 0;
			
				if(strstr(sp_space->tokens[j],hold))
				{
					std::string tmpStr = sp_space->tokens[j];
					UINT tmpKey = this->KeyMap[tmpStr];
					INPUT tmpInput;
					tmpInput.ki.wVk = tmpKey;
					tmpInput.type = INPUT_KEYBOARD;
					SendInput(1, &tmpInput, sizeof(INPUT));
					HoldQueue.enqueue(tmpKey);
					continue;
				}

				else if(strstr(sp_space->tokens[j],release))
				{
					UINT tmpKey = HoldQueue.dequeue();
					INPUT tmpInput;
					tmpInput.ki.wVk = tmpKey;
					tmpInput.ki.dwFlags = KEYEVENTF_KEYUP;
					tmpInput.type = INPUT_KEYBOARD;
					SendInput(1, &tmpInput, sizeof(INPUT));
					continue;
				}

				else if(strstr(sp_space->tokens[j],delay))
				{
					Delay(sp_space->tokens[j+1]);
					j++;
					continue;
				}

				else if(strstr(sp_space->tokens[j],type))
				{
					char* text = sp_space->tokens[j+1];
					this->Type(text);
					j++;
					continue;
				}

				else
				{
					std::string tmpStr = sp_space->tokens[j];
					key = this->KeyMap[tmpStr];

					inDn.ki.wVk = key;
					inDn.type = INPUT_KEYBOARD;
					this->InputKeys[0] = inDn;

					inUp.ki.wVk = key;
					inUp.type = INPUT_KEYBOARD;
					inUp.ki.dwFlags = KEYEVENTF_KEYUP;
					this->InputKeys[1] = inUp;
					Sleep(50);
					SendInput(2, this->InputKeys, sizeof(INPUT));
				}
            }
			free(sp_space);
		}
		return 0;
	}

    // virtual KeyWriter::~KeyWriter(){
    //     // delete[] this->InputKeys;
    //     // free(sp);
    // }
};

int main(int argc, char* argv[])
{
    std::string KeyFile = "keys.txt";
    KeyWriter keywriter{KeyFile};
    keywriter.GenKeyPress();
	Sleep(10000);
}