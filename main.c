#pragma warning(disable:4996)
#pragma warning(disable:28159)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

#define SAFE_DELETE(ptr)                do { free(ptr); (ptr) = NULL; } while(0)
#define CAPPING(char_array)             strupr(char_array)
#define DELETE_SLASH_N(text_array)      text_array[strcspn(text_array, "\n")] = 0

#define KEY_SPACE                       0x20
#define KEY_ESC                         0x1B
#define KEY_N                           0x4E

#define WORD_MAX                        20

typedef unsigned int                    fileLine_t;

#define ALLOW       1
#define DENY        0
#define CHECK_CARD_HOLE_MCS(punchCard_ptr)              (punchCard_ptr)->outputToConsoleAboutMorseCodeSound ==  ALLOW
#define CHECK_CARD_HOLE_MCB(punchCard_ptr)              (punchCard_ptr)->printToConsoleAboutMorseCodeBar ==     ALLOW
typedef struct {
    short outputToConsoleAboutMorseCodeSound;
    short printToConsoleAboutMorseCodeBar;
}punchCard_t;
punchCard_t listenProgramCard = { DENY, DENY };
punchCard_t m2aProgramCard = { DENY, DENY };
punchCard_t a2mProgramCard = { DENY, DENY };

#define INSERT_TEXT(card_p, text, str)                  {if(CHECK_CARD_HOLE_MCB(card_p)) {printf("%s", str);} strcat(text, str);}
#define PRINT_TEXT(card_p, fstChar, secStr)             {printf("%c", fstChar); if(CHECK_CARD_HOLE_MCB(card_p)) {printf(": %s", secStr);}}
#define PRINT_TEXT2(card_p, secStr)                     {if(CHECK_CARD_HOLE_MCB(card_p)) {printf(" %s  ", secStr);}}

#define COLOR_RED                                       FOREGROUND_RED|FOREGROUND_INTENSITY //���� ǥ��
#define COLOR_GREEN                                     FOREGROUND_GREEN|FOREGROUND_INTENSITY //���� ǥ��
#define COLOR_BLUE                                      FOREGROUND_BLUE|FOREGROUND_INTENSITY //�߰� ���� ����(Ʃ�丮��)
#define COLOR_YELLOW                                    FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY //���α׷� ��ǥ ����
#define COLOR_MAGENTA                                   FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY //���α׷� �̿��� ����
#define COLOR_CYAN                                      FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY //���α׷� �⺻ ����(Ʃ�丮��)
#define COLOR_WHITE                                     FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE
void printColor(const char* str, short color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == NULL)
    {
        printf("%s", str);
    }
    else
    {
        SetConsoleTextAttribute(hConsole, color);
        printf("%s", str);
        SetConsoleTextAttribute(hConsole, COLOR_WHITE);
    }
}

typedef struct {
    char text[WORD_MAX];
} wordText_t;
#define W_TEXT(wordText)                  (wordText).text

fileLine_t wordFile_line = 1;

typedef enum
{
    Listen = 1,
    M2A,
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
    MkDash, MkUndScr, MkColon, MkSlash,
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
    { '.', ".-.-.-" }, { '!', "-.-.--" }, { '?', "..--.." }, { ',', "--..--" },
    { '-', "-....-" }, { '_', "..--.-" }, { ':', "---..." }, { '/', "-..-."  }
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
        if (strcmp(morseCode, codeList[x].morseCode) == 0)
            return codeList[x].alphabet;
    }
    return ' ';
}

#define TIME_TERM(dword)                (GetTickCount() - (dword))
#define TIME_NOW                        (unsigned int)time(NULL)
#define SHUFFLE_BIT_1                   0b10101010101010101010101010101010
#define SHUFFLE_BIT_2                   0b01010000100001000010000100001010

int randomDrop(unsigned short maxVal)
{

    srand(
        (SHUFFLE_BIT_1 >> 1 ^ TIME_NOW << 8) >> 10 ^
        (SHUFFLE_BIT_2 << 1 & TIME_NOW >> 3) >> 10 ^
        (SHUFFLE_BIT_1 << 0 ^ TIME_NOW << 15) >> 3 ^
        (SHUFFLE_BIT_2 >> 1 & TIME_NOW << 3) << 10 ^
        (SHUFFLE_BIT_1 >> 1 ^ TIME_NOW >> 11) >> 1);
    return (rand() % maxVal + 1);
}


#define WORD_FILE_PATH                  "EnglishWords.data"
#define WORD_FILE_LINE                  wordFile_line

wordText_t getFileTexts(const char* fileName, fileLine_t fileLine)
{
    FILE* fp = fopen(fileName, "r");
    int counter = randomDrop(fileLine);

    wordText_t buffer;
    if (fp == NULL)
    {
        strcpy(W_TEXT(buffer), "ERROR");
        return buffer;
    }

    for (int x = 1; x <= fileLine; x++)
    {
        fgets(W_TEXT(buffer), sizeof(W_TEXT(buffer)), fp);
        if (counter <= x)
        {
            fclose(fp);
            DELETE_SLASH_N(W_TEXT(buffer));
            CAPPING(W_TEXT(buffer));
            return buffer;
        }
    }

    strcpy(W_TEXT(buffer), "ERROR");
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

volatile char keepBeeping = 0;
volatile char threadContinue = 0;
DWORD WINAPI BeepThread(LPVOID lpParam)
{
    punchCard_t* card = (punchCard_t*)lpParam;
    PlaySound(TEXT("mts_beep.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    while (threadContinue)
    {
        if (keepBeeping && CHECK_CARD_HOLE_MCS(card))
            waveOutSetVolume(NULL, 0xFFFF | (0xFFFF << 16));
        else
            waveOutSetVolume(NULL, 0x0000 | (0x0000 << 16));
    }
    PlaySound(NULL, NULL, SND_ASYNC);
    waveOutSetVolume(NULL, 0xFFFF | (0xFFFF << 16));
    return 0;
}
//=====================================================================================
//===================================  1�� ���α׷�  ===================================
//=====================================================================================
void morseSound(const char* message, punchCard_t* punchCard) {
    for (int i = 0; *(message + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(message + i));

        PRINT_TEXT(punchCard, *(message + i), morseMessage)
            printf("\n");

        for (int j = 0;
            (*(morseMessage + j) != NULL) && CHECK_CARD_HOLE_MCS(punchCard);
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
    printColor("������ �ۼ�", COLOR_YELLOW); printf("�ϼ���\n");
    printf("���� ���α׷� "); printColor("����â���� ���ư��� �ʹٸ� exit�� �Է�", COLOR_MAGENTA); printf("�ϼ���\n");

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

//=====================================================================================
//===================================  2�� ���α׷�  ===================================
//=====================================================================================

void onlySound_text(const char* word, punchCard_t* punchCard)
{
    for (int i = 0; *(word + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(word + i));

        printColor(morseMessage, COLOR_BLUE); printf("  ");

        for (int j = 0;
            (*(morseMessage + j) != NULL) && CHECK_CARD_HOLE_MCS(punchCard);
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
    printf("\n");
}

void runProgram2(punchCard_t* punchCard)
{
    printf("�𽺺�ȣ ��ȣ�� ��� "); printColor("�˸��� ���ĺ����� ��ȯ", COLOR_YELLOW); printf("�ϼ���\n");
    printf("���� ���α׷� "); printColor("����â���� ���ư��� �ʹٸ� exit�� �Է�", COLOR_MAGENTA); printf("�ϼ���\n");
   
    char ans[7 * 20] = "";
    while (1) {
        wordText_t* pickedupWord = { W_TEXT(getFileTexts(WORD_FILE_PATH, WORD_FILE_LINE)) };
        printf("Q: ");
        onlySound_text(pickedupWord, punchCard);
;
        printf("A: ");
        scanf("%s", ans);

        if (strcmp(CAPPING(ans), "EXIT") == 0)
        {
            break;
        }
        else if (strcmp(CAPPING(ans), pickedupWord) == 0) {
            printf("R: "); printColor(pickedupWord, COLOR_YELLOW); printf("\n");
            printColor("����!\n \n", COLOR_GREEN);
        }
        else {
            printf("R: "); printColor(pickedupWord, COLOR_YELLOW); printf("\n");
            printColor("����...\n \n", COLOR_RED);
        }

    }
    return;
}

//=====================================================================================
//===================================  3�� ���α׷�  ===================================
//=====================================================================================
int checkAnsA2M(char* word, char* morse)
{
    char answer[7 * 20] = "";
    for (int i = 0; *(word + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(word + i));

        strcat(answer, morseMessage);
        strcat(answer, " ");
    }
    printf("R: "); printColor(answer, COLOR_YELLOW); printf("\n");
    return strcmp(answer, morse);
}

#define ON                  1
#define OFF                 0
#define LONG_PRESS_DURATION 180 
#define WAIT_TERM_DURATION  450

#define P3_QUESTION(word_p)           {word_p = getFileTexts(WORD_FILE_PATH, WORD_FILE_LINE); printf("Q: "); printColor(W_TEXT(word_p), COLOR_BLUE); printf("\nA: "); }

int pressChecker(
    char* spacePressed, char* termMode,
    DWORD* spaceDownTime, DWORD* termWaitTime,
    char* morse, wordText_t* word,
    punchCard_t* punchCard)
{
    if (GetAsyncKeyState(KEY_SPACE) & 0x8000)
    {
        if (*spacePressed == OFF)
        {
            *spacePressed = ON; keepBeeping = ON; *termMode = ON;
            *spaceDownTime = GetTickCount();
        }
        *termWaitTime = GetTickCount();
    }
    else if (GetAsyncKeyState(KEY_ESC) & 0x8000)
    {
        return 0;
    }
    else if (GetAsyncKeyState(KEY_N) & 0x8000)
    {
        if (*termMode == OFF)
            strcat(morse, " ");
        *termMode = ON;

        printf("\n");
        if (checkAnsA2M(word, morse) == 0)
            printColor("����!\n", COLOR_GREEN);
        else
            printColor("����...\n", COLOR_RED);
        morse[0] = '\0';
        Sleep(1000);

        P3_QUESTION(*word)
    }
    else
    {
        if (*spacePressed == ON)
        {
            *spacePressed = OFF; *termMode = OFF; keepBeeping = OFF;
            if (TIME_TERM(*spaceDownTime) >= LONG_PRESS_DURATION)
                INSERT_TEXT(punchCard, morse, "-")
            else
                INSERT_TEXT(punchCard, morse, ".")
        }

        if (*termMode == OFF)
        {
            if (TIME_TERM(*termWaitTime) >= WAIT_TERM_DURATION)
            {
                *termMode = ON;
                INSERT_TEXT(punchCard, morse, " ")
            }
        }
    }

    return 1;
}

void runProgram3(punchCard_t* punchCard)
{
    printf("������ "); printColor("�𽺺�ȣ�� ��ȯ", COLOR_YELLOW); printf("�ϼ���\n");
    printColor("spaceŰ�� ������ �ð�", COLOR_CYAN); printf("�� ���� "); printColor("- �Ǵ� .", COLOR_BLUE); printf(" ���� �ν��մϴ�(-�� ��� .�� 3�� ����)\n");
    printf("���� �ð� "); printColor("�Է��� ���� ����", COLOR_CYAN); printf(" �� �𽺺�ȣ�� "); printColor("���Ⱑ ����", COLOR_BLUE); printf("�˴ϴ�\n");
    printf("�Է��� ������ "); printColor("NŰ�� ���� �ڷḦ ����", COLOR_CYAN); printf("�մϴ�\n");
    printf("���� ���α׷� "); printColor("����â���� ���ư��� �ʹٸ� escŰ", COLOR_MAGENTA); printf("�� ��������\n");

    Sleep(1500);

    threadContinue = ON;
    HANDLE hThread = CreateThread(NULL, 0, BeepThread, punchCard, 0, NULL);
    if (hThread == NULL)
    {
        threadContinue = OFF;
        return;
    }

    char spacePressed = OFF; char termMode = ON;
    DWORD spaceDownTime = 0; DWORD termWaitTime = 0;

    char morse[7 * 20] = "";
    wordText_t* pickedupWord = { W_TEXT(getFileTexts(WORD_FILE_PATH, WORD_FILE_LINE)) };
    P3_QUESTION(*pickedupWord)
        while (pressChecker(
            &spacePressed, &termMode,
            &spaceDownTime, &termWaitTime,
            morse, pickedupWord,
            punchCard)
            )
        {
            Sleep(1);
        }

    threadContinue = OFF;
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
}


void init();
int main()
{
    int programCode = 0;
    init();

ProgramStart_Point:
    system("cls");
    do
    {
        printf("%d. �𽺺�ȣ�� ������!\n", Listen);
        printf("%d. �𽺺�ȣ->�ؽ�Ʈ ����!\n", M2A);
        printf("%d. �ؽ�Ʈ->�𽺺�ȣ ����!\n", A2M);
        printf("%d. ���α׷� ����...\n", Fin);
        printf("Insert Number>>");
        if (scanf("%d", &programCode) != 1)
        {
            while (getchar() != '\n');
            goto ProgramStart_Point;
        }
        system("cls");
    } while (!(0 < programCode && programCode < program_Enum_Size));

    switch (programCode)
    {
    case Listen:
        runProgram1(&listenProgramCard);
        break;
    case M2A:
        runProgram2(&m2aProgramCard);
        break;
    case A2M:
        runProgram3(&m2aProgramCard);
        break;
    case Fin:
        exit(0);
        break;
    }

    programCode = 0;
    goto ProgramStart_Point;
}

void init()
{
    wordFile_line = getFileLine(WORD_FILE_PATH);

    listenProgramCard.outputToConsoleAboutMorseCodeSound = ALLOW;
    listenProgramCard.printToConsoleAboutMorseCodeBar = ALLOW;

    m2aProgramCard.outputToConsoleAboutMorseCodeSound = ALLOW;
    m2aProgramCard.printToConsoleAboutMorseCodeBar = ALLOW;

    a2mProgramCard.outputToConsoleAboutMorseCodeSound = ALLOW;
    a2mProgramCard.printToConsoleAboutMorseCodeBar = ALLOW;
}