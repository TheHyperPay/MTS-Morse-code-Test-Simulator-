#pragma warning(disable:4996)
#pragma warning(disable:6031)
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
#define PUNCHNUM(n) (n) > 0 ? ALLOW : DENY    
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

#define COLOR_RED                                       FOREGROUND_RED|FOREGROUND_INTENSITY //오답 표현
#define COLOR_GREEN                                     FOREGROUND_GREEN|FOREGROUND_INTENSITY //정답 표현
#define COLOR_BLUE                                      FOREGROUND_BLUE|FOREGROUND_INTENSITY //추가 조건 설명(튜토리얼)
#define COLOR_YELLOW                                    FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY //프로그램 목표 강조
#define COLOR_MAGENTA                                   FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY //프로그램 이외의 설명
#define COLOR_CYAN                                      FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY //프로그램 기본 설명(튜토리얼)
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
    Setting,
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
    const char alphabet;
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

const char* convertA2M(char alphabet)
{
    for (int x = 0; x < sizeof(codeList) / sizeof(code_t); x++)
    {
        if (alphabet == codeList[x].alphabet)
            return codeList[x].morseCode;
    }
    return " ";
}
const char convertM2A(const char* morseCode)
{
    for (int x = 0; x < sizeof(codeList) / sizeof(code_t); x++)
    {
        if (strcmp(morseCode, codeList[x].morseCode))
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

int isAlreadyExistWord(const char* fileName, const char* message)
{
    FILE* fp = fopen(fileName, "r");;
    char buffer[20];

    if (fp == NULL)
        return 0;

    while (fgets(buffer, sizeof(buffer), fp)) {

        if (strcmp(message, buffer) == 0) {
            fclose(fp);
            return 1;
        }

    }
    fclose(fp);

    return 0;
}

void addTxtFileWord(const char* fileName, const char* message)
{

    if (isAlreadyExistWord(fileName, message)) {
        printf("이미 텍스트 파일에 있는 단어입니다.\n");
        return;
    }

    FILE* fp = fopen(fileName, "a");
    if (fp == NULL)
        return 0;
    fprintf(fp, "\n%s ->", message);

    for (int i = 0; *(message + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(message + i));
        fprintf(fp, " %s  ", morseMessage);
    }

    fclose(fp);
}

float timePitch                         = 1;
#define SND_FREQUENCY					440
#define SND_GAP_DOT						150 - ((timePitch-1) * 150)
#define SND_GAP_BAR						450 - ((timePitch-1) * 450)
#define SND_SEP_CODE					26 - ((timePitch-1) * 26)
#define SND_SEP_LETTER					450 - ((timePitch-1) * 450)
#define SND_SEP_WORD					680 - ((timePitch-1) * 680)
#define SND_BAR_RCV						{Beep(SND_FREQUENCY, SND_GAP_BAR);	Sleep(SND_SEP_CODE / 20);}
#define SND_DOT_RCV						{Beep(SND_FREQUENCY, SND_GAP_DOT);	Sleep(SND_SEP_CODE);}
#define SND_END_OF_LETTER				Sleep(SND_SEP_LETTER);
#define SND_END_OF_WORD					Sleep(SND_SEP_WORD);
#define SND_VOID                        Sleep(1);

volatile char keepBeeping = 0;
volatile char threadContinue = 0;
DWORD WINAPI BeepThread(LPVOID lpParam)
{
    punchCard_t* card = (punchCard_t*)lpParam;
    if (CHECK_CARD_HOLE_MCS(card))
    {
        printf("%d",CHECK_CARD_HOLE_MCS(card));
        PlaySound(TEXT("mts_beep.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }
    while (threadContinue)
    {
        if (keepBeeping)
            waveOutSetVolume(NULL, 0xFFFF | (0xFFFF << 16));
        else
            waveOutSetVolume(NULL, 0x0000 | (0x0000 << 16));
    }
    PlaySound(NULL, NULL, SND_ASYNC);
    waveOutSetVolume(NULL, 0xFFFF | (0xFFFF << 16));
    return 0;
}
void playBeepSoundInMorseCode(const char* morseMessage, char isPlaySound)
{
    for (int j = 0;
        (*(morseMessage + j) != NULL) && isPlaySound;
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
//=====================================================================================
//===================================  1번 프로그램  ===================================
//=====================================================================================
void morseSound(const char* message, punchCard_t* punchCard) {
    for (int i = 0; *(message + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(message + i));

        PRINT_TEXT(punchCard, *(message + i), morseMessage)
            printf("\n");

        playBeepSoundInMorseCode(morseMessage, CHECK_CARD_HOLE_MCS(punchCard));
    }
}

void runProgram1(punchCard_t* punchCard)
{
    printColor("영문을 작성", COLOR_YELLOW); printf("하세요\n\n");
    printf("만약 프로그램 "); printColor("선택창으로 돌아가고 싶다면 exit를 입력", COLOR_MAGENTA); printf("하세요\n\n");

    char text[] = "CQD CQD SOS SOS DE MGY POSITION 41.46N 50.14W";
    while (1)
    {
        printf(">");
        gets(text);
        if ((strcmp(CAPPING(text), "EXIT") == 0))
            return;
        morseSound(CAPPING(text), punchCard);
        //addTxtFileWord(WORD_FILE_PATH, text);
    }
    return;
}
//=====================================================================================
//===================================  2번 프로그램  ===================================
//=====================================================================================
int checkAnsM2A(const char* ans, const char* right)
{
    printf("R: "); printColor(right, COLOR_YELLOW); printf("\n");
    return strcmp(ans, right);
}

void onlySound_text(const char* message, punchCard_t* punchCard)
{
    for (int i = 0; *(message + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(message + i));

        if(CHECK_CARD_HOLE_MCB(punchCard))
            printColor(morseMessage, COLOR_BLUE); printf("  ");

        playBeepSoundInMorseCode(morseMessage, CHECK_CARD_HOLE_MCS(punchCard));
    }
    printf("\n");
}

void runProgram2(punchCard_t* punchCard)
{
    printf("모스부호 신호를 듣고 "); printColor("알맞은 알파벳으로 변환", COLOR_YELLOW); printf("하세요\n");
    printf("소리가 출력중일 때도 "); printColor("입력이 가능", COLOR_CYAN); printf("합니다\n\n");
    printf("만약 프로그램 "); printColor("선택창으로 돌아가고 싶다면 exit를 입력", COLOR_MAGENTA); printf("하세요\n\n");
    Sleep(1000);

    char ans[7 * 20] = "actoz";
    while (1)
    {
        wordText_t* pickedupWord = { W_TEXT(getFileTexts(WORD_FILE_PATH, WORD_FILE_LINE)) };
        printf("Q: ");
        onlySound_text(pickedupWord, punchCard);
         
        printf("> ");
        scanf("%s", ans);
        if ((strcmp(CAPPING(ans), "EXIT") == 0))
            return;
        if (checkAnsM2A(CAPPING(ans), W_TEXT(*pickedupWord)) == 0) {
            printColor("정답!\n", COLOR_GREEN);
        }
        else {
            printColor("오답...\n", COLOR_RED);
        }
        Sleep(1000);
    }
    return;

}
//=====================================================================================
//===================================  3번 프로그램  ===================================
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
#define LONG_PRESS_DURATION 180 - ((timePitch-1) * 180)
#define WAIT_TERM_DURATION  450 - ((timePitch-1) * 450)

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
            printColor("정답!\n", COLOR_GREEN);
        else
            printColor("오답...\n", COLOR_RED);
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
    printf("영문을 "); printColor("모스부호로 변환", COLOR_YELLOW); printf("하세요\n");
    printColor("space키의 누르는 시간", COLOR_CYAN); printf("에 따라 "); printColor("- 또는 .", COLOR_BLUE); printf(" 으로 인식합니다(-는 통상 .의 3배 간격)\n");
    printf("일정 시간 "); printColor("입력을 하지 않을", COLOR_CYAN); printf(" 시 모스부호의 "); printColor("띄어쓰기가 진행", COLOR_BLUE); printf("됩니다\n");
    printf("입력이 끝나면 "); printColor("N키를 눌러 자료를 제출", COLOR_CYAN); printf("합니다\n\n");
    printf("만약 프로그램 "); printColor("선택창으로 돌아가고 싶다면 esc키", COLOR_MAGENTA); printf("를 누르세요\n\n");

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
//=====================================================================================
//===================================  세팅 프로그램  ==================================
//=====================================================================================
typedef enum {
    PunchCard_P1=1,
    PunchCard_P2,
    PunchCard_P3,
    TimeSet,
    Setting_Out,
    setting_Size
}setting_e;
void setPunchCard(punchCard_t* card)
{
    int mcs = 0, mcb = 0;
    printf("소리 출력 여부(ALLOW: 1, DENY: 0) >"); scanf("%d", &mcs);
    printf("모스코드 출력 여부(ALLOW: 1, DENY: 0) >"); scanf("%d", &mcb);
    card->outputToConsoleAboutMorseCodeSound = PUNCHNUM(mcs);
    card->printToConsoleAboutMorseCodeBar = PUNCHNUM(mcb);
    printf("설정 완료!");
}
void setPitch()
{
    printf("배속 조정 (0.5~1.5) >");
    scanf("%f", &timePitch);
    if (timePitch < 0.5)
        timePitch = 0.5;
    else if (timePitch > 1.5)
        timePitch = 1.5;
    printf("조정 완료!");
}

void setting()
{
    int inputCode = 0;
Setting_Point:
    system("cls");
    do
    {
        printf("%d. 1번 (모스부호를 익히자!) 펀치카드 설정\n", PunchCard_P1);
        printf("%d. 2번 (모스부호->텍스트 연습!) 펀치카드 설정\n", PunchCard_P2);
        printf("%d. 3번 (텍스트->모스부호 연습!) 펀치카드 설정\n", PunchCard_P3);
        printf("%d. 난이도 설정\n", TimeSet);
        printf("%d. ", Setting_Out); printColor("세팅 나가기!!\n", COLOR_MAGENTA);
        printf("Insert Number>>");
        if (scanf("%d", &inputCode) != 1)
        {
            while (getchar() != '\n');
            goto Setting_Point;
        }
        system("cls");
    } while (!(0 < inputCode && inputCode < setting_Size));

    switch (inputCode)
    {
        int mcs = 0, mcb = 0;
    case PunchCard_P1:
        printColor("1번 (모스부호를 익히자!) 펀치카드 설정중...\n", COLOR_CYAN);
        setPunchCard(&listenProgramCard);
        break;
    case PunchCard_P2:
        printColor("2번 (모스부호->텍스트 연습!) 펀치카드 설정중...\n", COLOR_CYAN);
        setPunchCard(&m2aProgramCard);
        break;
    case PunchCard_P3:
        printColor("3번 (텍스트->모스부호 연습!) 펀치카드 설정중...\n", COLOR_CYAN);
        setPunchCard(&a2mProgramCard);
        break;
    case TimeSet:
        setPitch();
        break;
    case Setting_Out:
        return;
    }
    Sleep(500);

    inputCode = 0;
    goto Setting_Point;
}

//=====================================================================================
//===================================  메인 프로그램  ==================================
//=====================================================================================
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
        printf("%d. 모스부호->텍스트 연습!\n", M2A);
        printf("%d. 텍스트->모스부호 연습!\n", A2M);
        printf("%d. 각종 설정!\n", Setting);
        printf("%d. 프로그램 종료...\n", Fin);
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
        runProgram3(&a2mProgramCard);
        break;
    case Setting:
        setting();
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