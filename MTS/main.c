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

#define SAFE_DELETE(ptr)                free(ptr); (ptr) = NULL;
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
punchCard_t listenProgramCard = { DENY, DENY};
punchCard_t m2aProgramCard = { DENY, DENY };
punchCard_t a2mProgramCard = { DENY, DENY};

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

typedef struct{
    char* time;
    char* programName;
    char* action;
}logFormat_t;

#define LOG_MAX                           220
#define LOG(programName, action)          {logFormat_t* log = inputData(getNowFormatTime(), (programName), (action)); insertLogData(log); }

logFormat_t* inputData(const char* time, const char* programName, const char* action)
{
    logFormat_t* value = (logFormat_t*)malloc(sizeof(logFormat_t));

    // �޸� �Ҵ� �� ���ڿ� ����
    value->time = (char*)malloc(strlen(time) + 1);
    if (value->time != NULL) {
        strcpy(value->time, time);
    }

    // �޸� �Ҵ� �� ���ڿ� ����
    value->programName = (char*)malloc(strlen(programName) + 1);
    if (value->programName != NULL) {
        strcpy(value->programName, programName);
    }

    // �޸� �Ҵ� �� ���ڿ� ����
    value->action = (char*)malloc(strlen(action) + 1);
    if (value->action != NULL) {
        strcpy(value->action, action);
    }

    return value;
}

typedef struct logNode_t{
    logFormat_t* data;
    struct logNode_t* link;
}logNode_t;

logNode_t* head = NULL;

void insertLogData(const logFormat_t* logData)
{
    logNode_t* log = (logNode_t*)malloc(sizeof(logNode_t));
    log->link = NULL;

    log->data = logData;
    
    logNode_t* temp = head;
    while (temp->link != NULL)
        temp = temp->link;

    temp->link = log;
}

logFormat_t* deleteLogData()
{
    logFormat_t* temp = NULL;
    
    if (head->link == NULL)
        return temp;

    temp = head->link->data;
    logNode_t* nextLink = head->link;
    head->link = nextLink->link;

    SAFE_DELETE(nextLink);

    return temp;
}

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
#define TIME_NOW(type)                  (type)time(NULL)
#define SHUFFLE_BIT_1                   0b10101010101010101010101010101010
#define SHUFFLE_BIT_2                   0b01010000100001000010000100001010
int randomDrop(unsigned short maxVal)
{
    srand(
        (SHUFFLE_BIT_1 >> 1 ^ TIME_NOW(unsigned int) << 8) >> 10 ^
        (SHUFFLE_BIT_2 << 1 & TIME_NOW(unsigned int) >> 3) >> 10 ^
        (SHUFFLE_BIT_1 << 0 ^ TIME_NOW(unsigned int) << 15) >> 3 ^
        (SHUFFLE_BIT_2 >> 1 & TIME_NOW(unsigned int) << 3) << 10 ^
        (SHUFFLE_BIT_1 >> 1 ^ TIME_NOW(unsigned int) >> 11) >> 1);
    return (rand() % maxVal + 1);
}

char* getNowFormatTime() {
    char formattedTime[100];
    struct tm* timeinfo;
    time_t now = TIME_NOW(time_t);
    timeinfo = localtime(&now);
    strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", timeinfo);

    char* result = (char*)malloc(strlen(formattedTime) + 1);
    if (result != NULL) {
        strcpy(result, formattedTime);
    }

    return result;
}


#define WORD_FILE_PATH                  "EnglishWords.data"
#define WORD_FILE_LINE                  wordFile_line
#define LOG_PATH                        "program.log"

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

void writeFileTexts(const char* fileName, const char* texts)
{
    FILE* fp = fopen(fileName, "a");

    if (fp == NULL) {
        fprintf(stderr, "FILE ERROR.\n");
        return 1;
    }

    fprintf(fp,"%s", texts);

    fclose(fp);
}

void startLog()
{
    head = (logNode_t*)malloc(sizeof(logNode_t));
    //init linked list
    head->link = NULL;
}
void endLog()
{
    while (head->link != NULL)
    {
        logFormat_t* logData = deleteLogData();
        char* text = (char*)malloc(strlen(logData->time) + strlen(logData->programName) + strlen(logData->action) + 4);
        if (text != NULL)
        {
            strcpy(text, logData->time);
            strcat(text, "\t");
            strcat(text, logData->programName);
            strcat(text, "\t");
            strcat(text, logData->action);
            strcat(text, "\n");
        }

        writeFileTexts(LOG_PATH, text);

        SAFE_DELETE(logData->time);
        SAFE_DELETE(logData->programName);
        SAFE_DELETE(logData->action);
        SAFE_DELETE(text);
    }

    SAFE_DELETE(head);
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
//===================================  1�� ���α׷�  ===================================
//=====================================================================================
void morseSound(const char* message, punchCard_t* punchCard) {
    char logmsg[LOG_MAX] = "system printing this morse code value:\t ";

    for (int i = 0; *(message + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(message + i));
        strcat(logmsg, morseMessage);
        strcat(logmsg, " ");

        PRINT_TEXT(punchCard, *(message + i), morseMessage)
            printf("\n");

        playBeepSoundInMorseCode(morseMessage, CHECK_CARD_HOLE_MCS(punchCard));
    }

    LOG("PROGRAM_1", logmsg);
}

void runProgram1(punchCard_t* punchCard)
{
    printColor("������ �ۼ�", COLOR_YELLOW); printf("�ϼ���\n\n");
    printf("���� ���α׷� "); printColor("����â���� ���ư��� �ʹٸ� exit�� �Է�", COLOR_MAGENTA); printf("�ϼ���\n\n");

    char text[] = "CQD CQD SOS SOS DE MGY POSITION 41.46N 50.14W";
    while (1)
    {
        printf(">");
        gets(text);
        char logmsg[LOG_MAX] = "player write:\t ";
        strcat(logmsg, CAPPING(text));
        LOG("PROGRAM_1", logmsg);
        if ((strcmp(CAPPING(text), "EXIT") == 0))
            return;
        morseSound(CAPPING(text), punchCard);
    }
    return;
}
//=====================================================================================
//===================================  2�� ���α׷�  ===================================
//=====================================================================================
int checkAnsM2A(const char* ans, const char* right)
{
    printf("R: "); printColor(right, COLOR_YELLOW); printf("\n");
    return strcmp(ans, right);
}

void onlySound_text(const char* message, punchCard_t* punchCard)
{
    char logmsg[LOG_MAX] = "morse code question:\t ";

    for (int i = 0; *(message + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(message + i));
        strcat(logmsg, morseMessage);
        strcat(logmsg, " ");

        if(CHECK_CARD_HOLE_MCB(punchCard))
            printColor(morseMessage, COLOR_BLUE); printf("  ");

        playBeepSoundInMorseCode(morseMessage, CHECK_CARD_HOLE_MCS(punchCard));
    }

    LOG("PROGRAM_2", logmsg);

    printf("\n");
}

void runProgram2(punchCard_t* punchCard)
{
    printf("�𽺺�ȣ ��ȣ�� ��� "); printColor("�˸��� ���ĺ����� ��ȯ", COLOR_YELLOW); printf("�ϼ���\n");
    printf("�Ҹ��� ������� ���� "); printColor("�Է��� ����", COLOR_CYAN); printf("�մϴ�\n\n");
    printf("���� ���α׷� "); printColor("����â���� ���ư��� �ʹٸ� exit�� �Է�", COLOR_MAGENTA); printf("�ϼ���\n\n");
    Sleep(1000);

    char ans[7 * 20] = "sample";
    while (1)
    {
        wordText_t* pickedupWord = { W_TEXT(getFileTexts(WORD_FILE_PATH, WORD_FILE_LINE)) };
        printf("Q: ");
        onlySound_text(pickedupWord, punchCard);

        char logmsg_a[LOG_MAX] = "text question:\t ";
        strcat(logmsg_a, W_TEXT(*pickedupWord));
        LOG("PROGRAM_2", logmsg_a);

        printf("> ");
        scanf("%s", ans);

        if ((strcmp(CAPPING(ans), "EXIT") == 0))
        {
            char logmsg_b[LOG_MAX] = "player write answer:\t ";
            strcat(logmsg_b, CAPPING(ans));
            LOG("PROGRAM_2", logmsg_b);
            return;
        }
        if (checkAnsM2A(CAPPING(ans), W_TEXT(*pickedupWord)) == 0) {
            printColor("����!\n", COLOR_GREEN);

            char logmsg_b[LOG_MAX] = "player write correct answer:\t ";
            strcat(logmsg_b, CAPPING(ans));
            LOG("PROGRAM_2", logmsg_b);
        }
        else {
            printColor("����...\n", COLOR_RED);

            char logmsg_b[LOG_MAX] = "player write incorrect answer:\t ";
            strcat(logmsg_b, CAPPING(ans));
            LOG("PROGRAM_2", logmsg_b);
        }
        Sleep(1000);
    }
    return;

}
//=====================================================================================
//===================================  3�� ���α׷�  ===================================
//=====================================================================================
int checkAnsA2M(char* word, char* morse)
{
    char logmsg[LOG_MAX] = "morse code correct answer:\t ";

    char* morseMessage;
    char answer[7 * 20] = "";
    for (int i = 0; *(word + i) != '\0'; i++) {
        const char* morseMessage = convertA2M(*(word + i));
        strcat(logmsg, morseMessage);
        strcat(logmsg, " ");

        strcat(answer, morseMessage);
        strcat(answer, " ");
    }

    LOG("PROGRAM_3", logmsg);

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
        char logmsg[LOG_MAX] = "player want to EXIT!";
        strcat(logmsg, morse);
        LOG("PROGRAM_3", logmsg);
        return 0;
    }
    else if (GetAsyncKeyState(KEY_N) & 0x8000)
    {
        if (*termMode == OFF)
            strcat(morse, " ");
        *termMode = ON;

        printf("\n");
        if (checkAnsA2M(word, morse) == 0)
        {
            char logmsg_a[LOG_MAX] = "player write correct answer:\t ";
            strcat(logmsg_a, morse);
            LOG("PROGRAM_3", logmsg_a);

            printColor("����!\n", COLOR_GREEN);
        }
        else
        {
            char logmsg_a[LOG_MAX] = "player write incorrect answer:\t ";
            strcat(logmsg_a, morse);
            LOG("PROGRAM_3", logmsg_a);
            printColor("����...\n", COLOR_RED);
        }
        morse[0] = '\0';
        Sleep(1000);

        P3_QUESTION(*word)
        char logmsg[LOG_MAX] = "system make a question:\t ";
        strcat(logmsg, W_TEXT(*word));
        LOG("PROGRAM_3", logmsg);
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
    printf("�Է��� ������ "); printColor("NŰ�� ���� �ڷḦ ����", COLOR_CYAN); printf("�մϴ�\n\n");
    printf("���� ���α׷� "); printColor("����â���� ���ư��� �ʹٸ� escŰ", COLOR_MAGENTA); printf("�� ��������\n\n");

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

    char logmsg[LOG_MAX] = "system make a question: ";
    strcat(logmsg, W_TEXT(*pickedupWord));
    LOG("PROGRAM_3", logmsg);

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
//===================================  ���� ���α׷�  ==================================
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
    printf("�Ҹ� ��� ����(ALLOW: 1, DENY: 0) >"); scanf("%d", &mcs);
    LOG("SYSTEM", "set punch card about sound output");
    printf("���ڵ� ��� ����(ALLOW: 1, DENY: 0) >"); scanf("%d", &mcb);
    LOG("SYSTEM", "set punch card about morse code output");
    card->outputToConsoleAboutMorseCodeSound = PUNCHNUM(mcs);
    card->printToConsoleAboutMorseCodeBar = PUNCHNUM(mcb);
    printf("���� �Ϸ�!");
}
void setPitch()
{
    printf("��� ���� (0.5~1.5) >");
    scanf("%f", &timePitch);
    LOG("SYSTEM", "set program pitch");
    if (timePitch < 0.5)
        timePitch = 0.5;
    else if (timePitch > 1.5)
        timePitch = 1.5;
    printf("���� �Ϸ�!");
}

void setting()
{
    int inputCode = 0;
Setting_Point:
    system("cls");
    do
    {
        printf("%d. 1�� (�𽺺�ȣ�� ������!) ��ġī�� ����\n", PunchCard_P1);
        printf("%d. 2�� (�𽺺�ȣ->�ؽ�Ʈ ����!) ��ġī�� ����\n", PunchCard_P2);
        printf("%d. 3�� (�ؽ�Ʈ->�𽺺�ȣ ����!) ��ġī�� ����\n", PunchCard_P3);
        printf("%d. ���̵� ����\n", TimeSet);
        printf("%d. ", Setting_Out); printColor("���� ������!!\n", COLOR_MAGENTA);
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
        LOG("SYSTEM", "setting about listen morse code punchcard");
        printColor("1�� (�𽺺�ȣ�� ������!) ��ġī�� ������...\n", COLOR_CYAN);
        setPunchCard(&listenProgramCard);
        break;
    case PunchCard_P2:
        LOG("SYSTEM", "setting about morse code to alphabet program punchcard");
        printColor("2�� (�𽺺�ȣ->�ؽ�Ʈ ����!) ��ġī�� ������...\n", COLOR_CYAN);
        setPunchCard(&m2aProgramCard);
        break;
    case PunchCard_P3:
        LOG("SYSTEM", "setting about alphabet to morse code program punchcard");
        printColor("3�� (�ؽ�Ʈ->�𽺺�ȣ ����!) ��ġī�� ������...\n", COLOR_CYAN);
        setPunchCard(&a2mProgramCard);
        break;
    case TimeSet:
        LOG("SYSTEM", "setting about program pitch");
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
//===================================  ���� ���α׷�  ==================================
//=====================================================================================
void init();
int main()
{
    int programCode = 0;
    init();

    startLog();
    LOG("SYSTEM", "program is now started");
    endLog();

ProgramStart_Point:
    system("cls");
    startLog();
    do
    {
        printf("%d. �𽺺�ȣ�� ������!\n", Listen);
        printf("%d. �𽺺�ȣ->�ؽ�Ʈ ����!\n", M2A);
        printf("%d. �ؽ�Ʈ->�𽺺�ȣ ����!\n", A2M);
        printf("%d. ���� ����!\n", Setting);
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
        LOG("SYSTEM", "play listen morse code program");
        runProgram1(&listenProgramCard);
        LOG("SYSTEM", "exit listen morse code program");
        break;
    case M2A:
        LOG("SYSTEM", "play converting about morse code to alphabet program");
        runProgram2(&m2aProgramCard);
        LOG("SYSTEM", "exit converting about morse code to alphabet program");
        break;
    case A2M:
        LOG("SYSTEM", "play converting about alphabet to morse code program");
        runProgram3(&a2mProgramCard);
        LOG("SYSTEM", "exit converting about alphabet to morse code program");
        break;
    case Setting:
        LOG("SYSTEM", "enter setting");
        setting();
        LOG("SYSTEM", "exit setting");
        break;
    case Fin:
        LOG("SYSTEM", "program is now closed");
        endLog();
        exit(0);
        break;
    }

    programCode = 0;
    endLog();
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