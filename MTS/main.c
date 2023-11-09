#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

#define SAFE_DELETE(ptr)                do { free(ptr); (ptr) = NULL; } while(0)
#define CAPPING(char_array)             strupr(char_array)
#define DELETE_SLASH_N(text_array)      text_array[strcspn(text_array, "\n")] = 0

#define KEY_SPACE                       0x20
#define KEY_ESC                         0x1B
#define KEY_ENTER                       0x0D

#define WORD_MAX                        20

typedef unsigned int                    fileLine_t;

typedef struct {
    char text[WORD_MAX];
} wordText_t;
#define TEXT(wordText)                  (wordText).text

fileLine_t wordFile_line                = 1;


#define ALLOW       1
#define DENY        0
typedef struct {
    short outputToConsoleAboutMorseCodeSound;
    short printToConsoleAboutMorseCodeBar;
    short inputKeyPress;
}punchCard_t;
punchCard_t listenProgramCard = { DENY, DENY, DENY };
punchCard_t m2aProgramCard = { DENY, DENY, DENY };

typedef enum
{
    Listen=1,
    A2M,
    Fin,
    program_Enum_Size
}program_e;

typedef enum
{
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
    No0, No1, No2, No3, No4,
    No5, No6, No7, No8, No9,
    MkPeriod, MkExclam, MkQuest, MkComma,
    code_Enum_Size
}codeNum_e;

typedef struct code_t 
{
	char alphabet;
	const char* morseCode;
}code_t;

code_t codeList[code_Enum_Size] = {
    { 'A', ".-"     }, { 'B', "-..."   }, { 'C', "-.-."   }, { 'D', "-.."    },
    { 'E', "."      }, { 'F', "..-."   }, { 'G', "--."    }, { 'H', "...."   },
    { 'I', ".."     }, { 'J', ".---"   }, { 'K', "-.-"    }, { 'L', ".-.."   },
    { 'M', "--"     }, { 'N', "-."     }, { 'O', "---"    }, { 'P', ".--."   },
    { 'Q', "--.-"   }, { 'R', ".-."    }, { 'S', "..."    }, { 'T', "-"      },
    { 'U', "..-"    }, { 'V', "...-"   }, { 'W', ".--"    }, { 'X', "-..-"   },
    { 'Y', "-.--"   }, { 'Z', "--.."   }, { '0', "-----"  }, { '1', ".----"  },
    { '2', "..---"  }, { '3', "...--"  }, { '4', "....-"  }, { '5', "....."  },
    { '6', "-...."  }, { '7', "--..."  }, { '8', "---.."  }, { '9', "----."  },
    { '.', ".-.-.-" }, { '!', "-.-.--" }, { '?', "..--.." }, { ',', "--..--" }
};

char* convertA2M(char alphabet)
{
    for (int x = 0; x < sizeof(codeList) / sizeof(code_t); x++)
    {
        if (alphabet == codeList[x].alphabet)
            return codeList[x].morseCode;
    }
    return " ";
}
char convertM2A(const char* morseCode)
{
    for (int x = 0; x < sizeof(codeList) / sizeof(code_t); x++)
    {
        if (strcmp(morseCode, codeList[x].morseCode))
            return codeList[x].alphabet;
    }
    return ' ';
}


#define TIME_NOW                        (unsigned int)time(NULL)
#define SHUFFLE_BIT_1                   0b10101010101010101010101010101010
#define SHUFFLE_BIT_2                   0b01010000100001000010000100001010

int randomDrop(unsigned short maxVal)
{
    srand((SHUFFLE_BIT_1 >> 1 ^ TIME_NOW << 8) >> 10 ^
        (SHUFFLE_BIT_2 << 1 & TIME_NOW >> 3) >> 10 ^
        (SHUFFLE_BIT_1 << 0 ^ TIME_NOW << 15) >> 3 ^
        (SHUFFLE_BIT_2 >> 1 & TIME_NOW << 3) << 10 ^
        (SHUFFLE_BIT_1 >> 1 ^ TIME_NOW >> 11) >> 1);
    return (rand() % maxVal + 1);
}


wordText_t getFileTexts(const char* fileName, fileLine_t fileLine)
{
    FILE* fp = fopen(fileName, "r");
    int counter = randomDrop(fileLine);

    wordText_t buffer;
    if (fp == NULL)
    {
        strcpy(TEXT(buffer), "ERROR");
        return buffer;
    }

    for (int x = 1; x <= fileLine; x++)
    {
        fgets(TEXT(buffer), sizeof(TEXT(buffer)), fp);
        if (counter <= x)
        {
            fclose(fp);
            DELETE_SLASH_N(TEXT(buffer));
            CAPPING(TEXT(buffer));
            return buffer;
        }
    }

    strcpy(TEXT(buffer), "ERROR");
    fclose(fp);
    return buffer;
}

fileLine_t getFileLine(const char* fileName)
{
    FILE* fp = fopen(fileName, "r");;
    char buffer[20];
    fileLine_t lines = 0;

    if (fp == NULL)
        return 0;

    while (fgets(buffer, sizeof(buffer), fp)) {
        lines++;
    }

    fclose(fp);
    return lines;
}

#define SND_FREQUENCY					440
#define SND_GAP							150
#define SND_SEP_CODE					26
#define SND_SEP_LETTER					450
#define SND_SEP_WORD					680
#define SND_BAR_RCV						{Beep(SND_FREQUENCY, SND_GAP * 3);	Sleep(SND_SEP_CODE / 20);}
#define SND_DOT_RCV						{Beep(SND_FREQUENCY, SND_GAP * 1);	Sleep(SND_SEP_CODE);}
#define SND_END_OF_LETTER				Sleep(SND_SEP_LETTER);
#define SND_END_OF_WORD					Sleep(SND_SEP_WORD);
#define SND_VOID                        Sleep(1);

void morseSound(const char* message, punchCard_t* punchCard) {
    for (int i = 0; *(message + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(message + i));

        printf("%c", *(message + i));
        if (punchCard->printToConsoleAboutMorseCodeBar == ALLOW)
        {
            printf(": %s", morseMessage);
        }
        printf("\n");

        for (int j = 0;
            (*(morseMessage + j) != NULL) && punchCard->outputToConsoleAboutMorseCodeSound == ALLOW;
            j++) 
        {
            if (*(morseMessage + j) == '-')
                SND_BAR_RCV
            else if (*(morseMessage + j) == '.')
                SND_DOT_RCV
            else
                SND_END_OF_WORD
        }
        SND_END_OF_LETTER
    }
}

void runProgram1(punchCard_t* punchCard)
{
    printf("영문을 작성하세요\n");
    printf("만약 프로그램 선택창으로 돌아가고 싶다면 exit를 입력하세요\n");

    char text[] = "CQD CQD SOS SOS DE MGY POSITION 41.46N 50.14W";
    while (1)
    {
        printf(">");
        gets(text);
        if ((strcmp(CAPPING(text), "EXIT") == 0))
            return;
        morseSound(CAPPING(text), punchCard);
    }
    return;
}






#define LONG_PRESS_DURATION 250 

volatile char keepBeeping = 0;
volatile char threadContinue = 0;
DWORD WINAPI BeepThread(LPVOID lpParam) 
{
    while (threadContinue)
    {
        if (keepBeeping) 
            Beep(SND_FREQUENCY, 150); //BUG: 소리가 끊어지면서 출력
        else
            Sleep(1);
    }
    return 0;
}
int pressChecker(int* spacePressed, DWORD* spaceDownTime, char* morse)
{
    if (GetAsyncKeyState(KEY_SPACE) & 0x8000)
    {
        if (!(*spacePressed))
        {
            *spacePressed = 1;
            *spaceDownTime = GetTickCount();
            keepBeeping = 1;
        }
    }
    else if (GetAsyncKeyState(KEY_ESC) & 0x8000)
    {
        return 0;
    }
    else if (GetAsyncKeyState(KEY_ENTER) & 0x8000)
    {
        printf("\nMORSE: %s\n", morse);
        morse[0] = '\0';
        Sleep(1000);
    }
    else
    {
        if (*spacePressed)
        {
            *spacePressed = 0;
            keepBeeping = 0;
            DWORD pressDuration = GetTickCount() - *spaceDownTime;
            if (pressDuration >= LONG_PRESS_DURATION)
            {
                printf("-");
                strcat(morse, "-");
            }
            else
            {
                printf(".");
                strcat(morse, ".");
            }
        }
    }

    return 1;
}

void runProgram3(punchCard_t* punchCard)
{
    printf("영문을 모스부호로 변환하세요\n");
    printf("space키의 누르는 시간에 따라 - 또는 . 으로 인식합니다\n");
    //printf("하나의 알파벳 입력이 끝나면 한 칸 띄워야 합니다\n");
    printf("입력이 끝나면 enter키를 눌러 자료를 제출합니다\n");
    printf("만약 프로그램 선택창으로 돌아가고 싶다면 esc키를 누르세요\n");

    Sleep(1500);

    threadContinue = 1;
    HANDLE hThread = CreateThread(NULL, 0, BeepThread, NULL, 0, NULL);
    if (hThread == NULL) 
    {
        threadContinue = 0;
        return;
    }

    int spacePressed = 0;
    DWORD spaceDownTime = 0;

    char morse[] = "";
    while (pressChecker(&spacePressed, &spaceDownTime, morse))
    {
        Sleep(1);
    }

    threadContinue = 0;
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
}


#define WORD_FILE_PATH                  "EnglishWords.data"
#define WORD_FILE_LINE                  wordFile_line

//TEXT(getFileTexts(WORD_FILE_PATH, WORD_FILE_LINE))로 파일에서 단어 끌어올 수 있다.
void init();
int main()
{
	int programCode = 0;
    init();

ProgramStart_Point:
    system("cls");
	do
	{
        printf("%d. 모스부호를 익히자!\n", Listen);
        printf("%d. 텍스트->모스부호 연습!\n", A2M);
        printf("%d. 프로그램 종료...\n", Fin);
		printf("Insert Number>>");
		scanf("%d", &programCode);
		system("cls");
	} while (!(0 < programCode && programCode < program_Enum_Size));

    switch (programCode)
    {
    case Listen:
        runProgram1(&listenProgramCard);
        break;
    case A2M:
        runProgram3(&m2aProgramCard);
        break;
    case Fin:
        exit(0);
        break;
    }

	goto ProgramStart_Point;
}

void init()
{
    wordFile_line = getFileLine(WORD_FILE_PATH);

    listenProgramCard.outputToConsoleAboutMorseCodeSound = ALLOW;
    listenProgramCard.printToConsoleAboutMorseCodeBar = ALLOW;

    m2aProgramCard.outputToConsoleAboutMorseCodeSound = ALLOW;
    m2aProgramCard.printToConsoleAboutMorseCodeBar = ALLOW;
    m2aProgramCard.inputKeyPress = ALLOW;
}











#define TT_1 200
#define TEST_MUSIC_1                    {Beep(262, TT_1);Sleep(TT_1);Beep(262, TT_1);Sleep(TT_1);Beep(247, TT_1*3);Beep(262, TT_1);Sleep(TT_1*2);Beep(294, TT_1*2);Beep(330, TT_1);Sleep(TT_1*3);\
                                        Beep(349, TT_1*2);Beep(330, TT_1*2);Sleep(TT_1*2);Beep(294, TT_1*4);Beep(262, TT_1*2);Beep(247, TT_1*4);}
#define TT2 250
#define TEST_MUSIC_2 {Beep(659, TT2*8); Beep(587, TT2*3);Beep(554, TT2*3);Beep(659, TT2*2);Beep(587, TT2*8);Beep(587, TT2*3);Beep(523, TT2*3);Beep(466, TT2*2);\
                     Beep(440, TT2*7);Beep(415, TT2*1);Beep(440, TT2*5);Beep(329, TT2*1);Beep(415, TT2*1);Beep(440, TT2*1);Beep(493, TT2*5);Beep(329, TT2*1);Beep(415, TT2*1);Beep(493, TT2*1);Beep(554, TT2*4);Beep(493, TT2*4);\
                     Beep(659, TT2*8);Beep(587, TT2*3);Beep(554, TT2*3);Beep(493, TT2*2);Beep(440, TT2*7);Beep(440, TT2/2);Beep(415, TT2/2);Beep(440, TT2*1.5);Beep(392, TT2*1.5);Beep(349, TT2*2);Beep(392, TT2*1);Beep(349, TT2*2);\
                     Beep(329, TT2*7);Beep(415, TT2*1);Beep(440, TT2*4);Beep(329, TT2*1.5);Beep(293, TT2*1.5);Beep(277, TT2*1);Beep(293, TT2*8);Beep(293, TT2*1.5);Beep(277, TT2*1.5);Beep(493/2, TT2*3);Beep(415/2, TT2*2);Beep(440/2, TT2*8);}
