//������ϵͳ
#define _CRT_SECURE_NO_WARNINGS 1  //����scanf�Ⱥ�������ȫ�Ĵ���

#undef UNICODE   //ȡ�� Unicode ����ĺ궨�壬��������Ŀ�� MBCS �������

#pragma comment(lib,"Winmm.lib")  // ���� Windows Multimedia API

#include <graphics.h>  //ʹ��EasyXͼ�ο⺯��
#include <conio.h>
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include <assert.h>


// ������Ϣ�ṹ������
struct Word {
    char word[50]; // ����
    char meaning[100]; // ���ʴ���
    int correct_count; // ��ȷ�������
    int incorrect_count; // ����������
    int memorized;// �Ƿ��Ѽ��䣨0��δ���䣬1���Ѽ��䣩
};



// ��̬����������
typedef struct Node {
    struct Word data;
    struct Node* next;
}Node;

#define MAX_LINE_LEN 200 //���ܵ����ļ�һ�е���ַ���

int word_cnt = 0;  //�����ļ��е���������
const int HEIGHT = 1080;  //���ٴ��ڸ�
const int WIDTH = 720;    //���ٴ��ڿ�

int flag = 0;  //���
int amount;   //һ��Ҫ�ƻ�����ĵ�������

struct Node* List; //ȫ������

Node* pr[5000] = { 0,NULL };  //��¼ǰһҳ��������

IMAGE bg;  //����ͼƬ����


struct Node* read_words_from_file();
struct Node* create_random_plan(struct Node* head);

//һЩ��Ҫ����������ʵ�ַ��ذ�ť�Ĺ���
void start_learn(int number,struct Node*head);
void learn_word(struct Node*head);
void nodoLine();
void sort_list(struct Node*head);
void welcome();
void createplan();


// ���ݵ�������ĸ��������
void sort_list(struct Node* head) {
    for (struct Node* p = head; p != NULL; p = p->next) {
        for (struct Node* q = p->next; q != NULL; q = q->next) {
            if (strcmp(p->data.word, q->data.word) > 0) {
                struct Word temp = p->data;
                p->data = q->data;
                q->data = temp;
            }
        }
    }
}

// �������ݵ�������ĸ����ĵ�����Ϣ��̬����
struct Node* create_word_list() {

    struct Node* head = read_words_from_file();
    sort_list(head);
    return head;

}

// �����½��
struct Node* create_node(struct Word word) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    if (new_node == NULL) return NULL;
    new_node->data = word;
    new_node->next = NULL;
    return new_node;
}

// β�巨��������
struct Node* create_list(struct Word* words) {
    struct Node* head = NULL;
    struct Node* tail = NULL;
    for (int i = 0; i < word_cnt; i++) {
        struct Node* new_node = create_node(words[i]);
        if (head == NULL) {
            head = new_node;
        }
        else {
            tail->next = new_node;
        }
        tail = new_node;
    }
    return head;
}

// ͳ�������׺����Ѽ���ĵ���
void get_easiest_and_hardest_words(struct Node* head, char* easiest_word, char* hardest_word) {
    int easiest_correct_count = -1;
    int hardest_incorrect_count = -1;
    for (struct Node* p = head; p != NULL; p = p->next) {
        if (p->data.correct_count > easiest_correct_count) {
            easiest_correct_count = p->data.correct_count;
            strcpy(easiest_word, p->data.word);
        }
        if (p->data.incorrect_count > hardest_incorrect_count) {
            hardest_incorrect_count = p->data.incorrect_count;
            strcpy(hardest_word, p->data.word);
        }
    }
}

// ���ļ��ж�ȡ�����б�
struct Node* read_words_from_file() {


    FILE* file;
    if ((file = fopen("word.txt", "r")) == NULL) { //���޴��ļ�
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "word.txt�ļ�ȱʧ", "����", MB_OKCANCEL);
        exit(0);
    }


    struct Node* head = NULL;
    struct Node* tail = NULL;
    char line[MAX_LINE_LEN];
    while (fgets(line, MAX_LINE_LEN, file) != NULL) {

        // ���Կ��кͽ������ո���Ʊ������
        if (strcmp(line, "\n") == 0 || strspn(line, " \t\n\r") == strlen(line)) {
            continue;
        }

        char word[50] = { '\0' };
        char definition[100] = { '\0' };
        // �������ʺ�������Ϣ
        if (sscanf(line, "%49[^ ] %99[^\n]", word, definition) != 2) {
            //printf("Invalid format: %s\n", line);
            continue;
        }
        // ������̬������
        struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
        if (new_node == NULL) {
            printf("Memory allocation failed\n");
            break;
        }
        new_node->next = NULL;
        // ���Ƶ��ʺ�������Ϣ����������
        new_node->data.word[49] = '\0'; // �ֶ�����ַ�������ֹ��
        new_node->data.meaning[99] = '\0'; // �ֶ�����ַ�������ֹ��
        strncpy(new_node->data.word, word, 50);
        strncpy(new_node->data.meaning, definition, 100);

        new_node->data.correct_count = 0;
        new_node->data.incorrect_count = 0;
        new_node->data.memorized = 0;

        // ���½���������
        if (tail == NULL) {
            head = tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = new_node;
        }

    }
    //printf("��ȡ�ɹ���\n");
    fclose(file);
    return head;
}

//�����������ƻ�����ʵ��
struct Node* create_random_plan(struct Node* head) {
    
    struct Node* p;

    // ���㵥������
    for (p = head; p != NULL; p = p->next) {
        word_cnt++;
    }

    srand((unsigned int)time(NULL));  //�������������

    //��̬�����ڴ�
    struct Word* words = (struct Word*)malloc((word_cnt + 1) * sizeof(struct Word));
    assert(words); //�ж��Ƿ�Ϊ��
    struct Word* temp = (struct Word*)malloc((word_cnt + 1) * sizeof(struct Word));
    assert(temp);

    int i = 0;
    for (p = head; p != NULL; p = p->next) {
        temp[i++] = p->data;
    }

    int* word_indices = (int*)malloc(word_cnt * sizeof(int));
    assert(word_indices);

    for (int i = 0; i < word_cnt; i++) {
        word_indices[i] = i;
    }

    //�����������
    for (int i = word_cnt - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp_val = word_indices[i];
        word_indices[i] = word_indices[j];
        word_indices[j] = temp_val;
    }

    for (int i = 0; i < word_cnt; i++) {
        words[i] = temp[word_indices[i]];
    }

    //�ͷ��ڴ�
    free(temp);
    free(word_indices);

    return create_list(words);  
}

//ֱ�Ӽ���
void memorize_word1(int number,struct Node*p) {
    cleardevice();
    putimage(0, 0, &bg);

    int f = 0;
    int cnt = 0;
    int sign = 0;  //���
    char yema[5] = { '\0' };  //ҳ��

    //���Ʊ���ɫ��
    setlinecolor(RGB(117, 166, 255));
    setfillcolor(RGB(117, 166, 255));
    fillroundrect(235, 95, 685, 265, 20, 20);
    setfillcolor(WHITE);


    int i = 0;
    while (p != NULL) {

       

            FILE* file;
            if ((file = fopen("word.txt", "r")) == NULL) { //���޴��ļ�
                HWND hwnd = GetHWnd();
                MessageBox(hwnd, "��ȡ�ĵ����ļ�ȱʧ", "����", MB_OKCANCEL);
                exit(0);
            }

            settextcolor(WHITE);
            settextstyle(30, 0, "΢���ź�");

            number--;
            if (number < 0)
            {
                HWND hwnd1 = GetHWnd();
                MessageBox(hwnd1, "̫���ˣ���^ ^���Ѵﵽ�ƻ�����ĵ�������������Ҫ���밴���ذ�ť�˳�", "����", MB_OKCANCEL);
            }

            //���Ʊ���ɫ��
            setlinecolor(RGB(117, 166, 255));
            setfillcolor(RGB(117, 166, 255));
            fillroundrect(235, 95, 685, 265, 20, 20);


            // �����ǰ���ʺ�������Ϣ
            outtextxy(240, 100, "���ʣ�");
            outtextxy(240, 125, "���壺");
            outtextxy(300, 100, p->data.word);
            outtextxy(300, 125, p->data.meaning);

            settextcolor(WHITE);
            settextstyle(30, 0, "����");
            outtextxy(875, 595, "��������");
            outtextxy(875, 645, "�ر�����");

            settextcolor(WHITE);
            settextstyle(30, 0, "΢���ź�");


            // ��ȡ�û�����ļ�����
            fflush(stdin); // ������뻺����

            outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

            i++;
            cnt++;
            sprintf(yema, "%d", cnt);
            outtextxy(490, 520, yema);
            bool TimeSafeLock = 1;

            MOUSEMSG m;
            while (1) {
                char st1[] = "Yes";
                char st2[] = "No";
                char st3[] = "��һҳ";
                char st4[] = "��һҳ";
                char st5[] = "����";

                outtextxy(380, 175, st1);
                outtextxy(550, 175, st2);
                outtextxy(550, 520, st3);
                outtextxy(380, 520, st4);
                outtextxy(910, 520, st5);

                m = GetMouseMsg();

                //����Yes��ť�Ͱ�ť����
                if (m.x >= 365 && m.x <= 445 && m.y >= 180 && m.y <= 210) {

                    setlinecolor(RED);
                    rectangle(365, 180, 445, 205);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        sign++;
                        pr[sign] = p;

                        number--;
                        if (number < 0)
                        {
                            HWND hwnd1 = GetHWnd();
                            MessageBox(hwnd1, "̫���ˣ���^ ^���Ѵﵽ�ƻ�����ĵ�������������Ҫ���밴���ذ�ť�˳�", "����", MB_OKCANCEL);
                        }

                       
                        p->data.memorized = 1;
                        p->data.correct_count++;
                        Sleep(200);
                        putimage(0, 0, &bg);

                        i++;
                        p = p->next;

                        //���Ʊ���ɫ��
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));

                        fillroundrect(235, 95, 685, 265, 20, 20);

                        // �����ǰ���ʺ�������Ϣ
                        outtextxy(240, 100, "���ʣ�");
                        outtextxy(240, 125, "���壺");
                        outtextxy(300, 100, p->data.word);


                        outtextxy(300, 125, p->data.meaning);

                        // ��ȡ�û�����ļ�����
                        fflush(stdin); // ������뻺����
            
                        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "����");
                        outtextxy(875, 595, "��������");
                        outtextxy(875, 645, "�ر�����");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");

                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ����ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(365, 180, 445, 205);
                }


                //����No��ť�Ͱ�ť����
                if (m.x >= 540 && m.x <= 620 && m.y >= 180 && m.y <= 205) {

                    i++;

                    setlinecolor(RED);
                    rectangle(540, 180, 620, 205);

                    if (m.uMsg == WM_LBUTTONDOWN) {

                        p->data.incorrect_count++;
                        sign++;
                        pr[sign] = p;


                        Sleep(200);
                        putimage(0, 0, &bg);

                        number--;
                        if (number < 0)
                        {
                            HWND hwnd1 = GetHWnd();
                            MessageBox(hwnd1, "̫���ˣ���^ ^���Ѵﵽ�ƻ�����ĵ�������������Ҫ���밴���ذ�ť�˳�", "����", MB_OKCANCEL);
                        }

                        p = p->next;

                        //���Ʊ���ɫ��
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        // �����ǰ���ʺ�������Ϣ
                        outtextxy(240, 100, "���ʣ�");
                        outtextxy(240, 125, "���壺");
                        outtextxy(300, 100, p->data.word);
                        outtextxy(300, 125, p->data.meaning);

                        // ��ȡ�û�����ļ�����
                        fflush(stdin); // ������뻺����
                        //char input;
                        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "����");
                        outtextxy(875, 595, "��������");
                        outtextxy(875, 645, "�ر�����");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");

                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ����ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(540, 180, 620, 205);
                }


                //������һҳ��ť�Ͱ�ť����
                if (m.x >= 545 && m.x <= 625 && m.y >= 515 && m.y <= 550) {
                    setlinecolor(RED);
                    rectangle(545, 515, 625, 550);

                    i++;

                    if (m.uMsg == WM_LBUTTONDOWN) {
                        Sleep(200);
                        putimage(0, 0, &bg);

                        sign++;
                        pr[sign] = p;

                        number--;
                        if (number == 0)
                        {
                            HWND hwnd1 = GetHWnd();
                            MessageBox(hwnd1, "̫���ˣ���^ ^���Ѵﵽ�ƻ�����ĵ�������������Ҫ���밴���ذ�ť�˳�", "����", MB_OKCANCEL);
                        }

                        p = p->next;

                        //���Ʊ���ɫ��
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        // �����ǰ���ʺ�������Ϣ
                        outtextxy(240, 100, "���ʣ�");
                        outtextxy(240, 125, "���壺");
                        outtextxy(300, 100, p->data.word);
                        outtextxy(300, 125, p->data.meaning);

                        // ��ȡ�û�����ļ�����
                        fflush(stdin); // ������뻺����

                        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "����");
                        outtextxy(875, 595, "��������");
                        outtextxy(875, 645, "�ر�����");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");

                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ����ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(545, 515, 625, 550);
                }

                //������һҳ��ť�Ͱ�ť����
                if (m.x >= 375 && m.x <= 455 && m.y >= 515 && m.y <= 550) {
                    setlinecolor(RED);
                    rectangle(375, 515, 455, 550);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        putimage(0, 0, &bg);

                        sign--;
                        if (sign == 0)
                        {
                            sign = 1;
                            HWND hwnd1 = GetHWnd();
                            MessageBox(hwnd1, "�Ѵ�����ҳ��", "����", MB_OKCANCEL);
                        }
                        //���Ʊ���ɫ��
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);
                        
                        // �����ǰ���ʺ�������Ϣ
                        outtextxy(240, 100, "���ʣ�");
                        outtextxy(240, 125, "���壺");
                        outtextxy(300, 100, pr[sign]->data.word);
                        outtextxy(300, 125, pr[sign]->data.meaning);

                        // ��ȡ�û�����ļ�����
                        fflush(stdin); // ������뻺����

                        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                        number++;


                        settextcolor(WHITE);
                        settextstyle(30, 0, "����");
                        outtextxy(875, 595, "��������");
                        outtextxy(875, 645, "�ر�����");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");

                        cnt--;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);
                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ����ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(375, 515, 455, 550);
                }

                //���ذ�ť�Ͱ�ť����
                if (m.x >= 865 && m.x <= 990 && m.y >= 515 && m.y <= 550) {
                    setlinecolor(RED);
                    rectangle(865, 515, 990, 550);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        Sleep(200);
                        putimage(0, 0, &bg);

                        flag = 1;
                        p = p->next;  //�Ա��ٴ�ѧϰ�����ǽ�����һ�εģ������ظ�ѧϰ

                        learn_word(List);  //��ʣ���δ����ĵ�������ת��ȥ

                    }
                    else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                        setlinecolor(WHITE);
                        rectangle(865, 515, 990, 550);
                    }
                }


                setlinecolor(WHITE);
                rectangle(865, 515, 990, 550);
                


                //��ʼ���ְ�ť�Ͱ�ť����
                if (m.x >= 875 && m.x <= 1000 && m.y >= 590 && m.y <= 630)
                {
                    setlinecolor(RED);//���õ��ʱΪ��ɫ 
                    rectangle(875, 590, 1000, 630);
                    if (m.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
                    {


                        // ������
                        mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                        // ��������
                        mciSendString("play bk repeat", NULL, 0, NULL);

                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ����ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(875, 590, 1000, 630);
                }

                //�������ְ�ť�Ͱ�ť����
                if (m.x >= 875 && m.x <= 1000 && m.y >= 640 && m.y <= 680)//���    outtextxy(885, 500, "��������");
                {
                    setlinecolor(RED);//���õ��ʱΪ��ɫ 
                    rectangle(875, 640, 1000, 680);
                    if (m.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
                    {
                        // ֹͣ���Ų��ر�����
                        mciSendString("stop bk", NULL, 0, NULL);
                        mciSendString("close bk", NULL, 0, NULL);
                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ����ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(875, 640, 1000, 680);
                }
            }
        }
       
}

//Ĭд����
void memorize_word2(int number,struct Node*p)
{

    cleardevice();
    putimage(0, 0, &bg);

    FILE* file;
    if ((file = fopen("word.txt", "r")) == NULL) { //���޴��ļ�
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "��ȡ�ĵ����ļ�ȱʧ", "����", MB_OKCANCEL);
        exit(0);
    }

    //���Ʊ���ɫ��
    setlinecolor(RGB(117, 166, 255));
    setfillcolor(RGB(117, 166, 255));
    fillroundrect(235, 95, 685, 265, 20, 20);


    settextcolor(WHITE);
    settextstyle(30, 0, "����");
    outtextxy(875, 595, "��������");
    outtextxy(875, 645, "�ر�����");

    settextcolor(WHITE);
    settextstyle(30, 0, "΢���ź�");

    int sign = 0;
    int i = 0;
    int cnt = 0;
    char yema[5] = { '\0' };
    while (p != NULL) {

        if (p->data.memorized == 0) {


            number--;
            if (number < 0)
            {
                HWND hwnd1 = GetHWnd();
                MessageBox(hwnd1, "̫���ˣ���^ ^���Ѵﵽ�ƻ�����ĵ�������������Ҫ���밴���ذ�ť�˳�", "����", MB_OKCANCEL);
            }

            settextcolor(WHITE);
            settextstyle(30, 0, "΢���ź�");

            outtextxy(240, 100, "���壺");
            outtextxy(250, 590, "��");
            outtextxy(300, 100, p->data.meaning);

            
            char meani[100];
            InputBox(meani, 20, "�������Ӧ����ĵ���:");//��ʾһ�������
            if (strcmp(meani, p->data.word) == 0) {

                outtextxy(240, 400, "Congratulations! You have memorized the word .");
            }
            else {

                outtextxy(240, 400, "Sorry!Please try again. ");
            }

            i++;

            //���ҳ��
            cnt++;
            sprintf(yema, "%d", cnt);
            outtextxy(490, 520, yema);

            MOUSEMSG m;
            while (1) {
                char st1[] = "����";
                char st2[] = "�ٴγ���";
                char st3[] = "��һҳ";
                char st4[] = "��һҳ";
                char st5[] = "����";

                outtextxy(380, 175, st1);
                outtextxy(550, 175, st2);
                outtextxy(550, 520, st3);
                outtextxy(380, 520, st4);
                outtextxy(900, 520, st5);
                
                m = GetMouseMsg();

                //����������ť�Ͱ�ť����
                if (m.x >= 365 && m.x <= 445 && m.y >= 180 && m.y <= 210) {
                    setlinecolor(RED);
                    rectangle(365, 180, 445, 205);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        Sleep(200);
                        putimage(0, 0, &bg);

                        sign++;
                        pr[sign] = p;

                        number--;
                        if (number < 0)
                        {
                            HWND hwnd1 = GetHWnd();
                            MessageBox(hwnd1, "̫���ˣ���^ ^���Ѵﵽ�ƻ�����ĵ�������������Ҫ���밴���ذ�ť�˳�", "����", MB_OKCANCEL);
                        }

                        

                        i++;
                        p = p->next;

                        //���Ʊ���ɫ��
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        //���ҳ��
                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);


                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");

                        outtextxy(240, 100, "���壺");
                        outtextxy(250, 590, "��");
                        outtextxy(300, 100, p->data.meaning);

                        rectangle(250, 580, 300, 620);

                        MOUSEMSG n;
                        n = GetMouseMsg();

                        char meani[100];
                        InputBox(meani, 20, "�������Ӧ����ĵ���:");//��ʾһ�������
                        if (strcmp(meani, p->data.word) == 0) {

                            outtextxy(240, 400, "Congratulations! You have memorized the word .");
                            
                            p->data.memorized = 1;
                            p->data.correct_count++;
                        }
                        else {
                            
                            p->data.incorrect_count++;
                            outtextxy(240, 400, "Sorry,Please try again. ");
                        }

                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(365, 180, 445, 205);
                }


                //�����ٴγ��԰�ť�Ͱ�ť����
                if (m.x >= 540 && m.x <= 640 && m.y >= 180 && m.y <= 205) {

                  
                    setlinecolor(RED);
                    rectangle(540, 180, 640, 205);



                    if (m.uMsg == WM_LBUTTONDOWN) {
                        Sleep(200);
                        putimage(0, 0, &bg);

                        i++;

                        //���ҳ��
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                        //���Ʊ���ɫ��
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");

                        outtextxy(240, 100, "���壺");
                        outtextxy(250, 590, "��");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "����");
                        outtextxy(875, 595, "��������");
                        outtextxy(875, 645, "�ر�����");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");
                    
                        outtextxy(300, 100, p->data.meaning);

                        MOUSEMSG n;
                        n = GetMouseMsg();
                        char meani[100];
                        InputBox(meani, 20, "�������Ӧ����ĵ���:");//��ʾһ�������
                        if (strcmp(meani, p->data.word) == 0) {
                            outtextxy(240, 400, "Congratulations! You have memorized the word .");

                            p->data.memorized = 1;
                            p->data.correct_count++;
                        }
                        else {

                            p->data.incorrect_count++;
                           
                            outtextxy(240, 400, "Sorry,Please try again. ");


                        }

                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(540, 180, 640, 205);
                }
                //������һҳ��ť�Ͱ�ť����
                if (m.x >= 545 && m.x <= 625 && m.y >= 515 && m.y <= 550) {
                    setlinecolor(RED);
                    rectangle(545, 515, 625, 550);

                    i++;

                    if (m.uMsg == WM_LBUTTONDOWN) {
                        Sleep(200);
                        putimage(0, 0, &bg);

                        number--;
                        if (number < 0)
                        {
                            HWND hwnd1 = GetHWnd();
                            MessageBox(hwnd1, "̫���ˣ���^ ^���Ѵﵽ�ƻ�����ĵ�������������Ҫ���밴���ذ�ť�˳�", "����", MB_OKCANCEL);
                        }

                        sign++;
                        pr[sign] = p;

                        p = p->next;

                        //���ҳ��
                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                        //���Ʊ���ɫ��
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");

                        outtextxy(240, 100, "���壺");
                        outtextxy(250, 590, "��");
                        outtextxy(300, 100, p->data.meaning);

                        settextcolor(WHITE);
                        settextstyle(30, 0, "����");
                        outtextxy(875, 595, "��������");
                        outtextxy(875, 645, "�ر�����");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");

                        MOUSEMSG n;
                        n = GetMouseMsg();

                        char meani[100];
                        InputBox(meani, 20, "�����뵥�ʵ���˼:");//��ʾһ�������
                        if (strcmp(meani, p->data.meaning) == 0) {
                           
                            p->data.memorized = 1;
                            p->data.correct_count++;
                            outtextxy(240, 400, "Congratulations! You have memorized the word .");

                        }
                        else {
                           
                            p->data.incorrect_count++;
                            outtextxy(240, 400, "Sorry,Please try again. ");
                        }


                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(545, 515, 625, 550);
                }


                //������һҳ��ť�Ͱ�ť����
                if (m.x >= 375 && m.x <= 455 && m.y >= 515 && m.y <= 550) {
                    setlinecolor(RED);
                    rectangle(375, 515, 455, 550);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        putimage(0, 0, &bg);
                        sign--;
                        if (sign == 0)
                        {
                            sign = 1;
                            HWND hwnd1 = GetHWnd();
                            MessageBox(hwnd1, "�Ѵ�����ҳ��", "����", MB_OKCANCEL);
                        }

                        //���ҳ��
                        cnt--;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                        number++;

                        //���Ʊ���ɫ��
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        settextcolor(WHITE);
                        settextstyle(30, 0, "΢���ź�");

                        outtextxy(240, 100, "���壺");
                        outtextxy(250, 590, "��");
                        outtextxy(300, 100, p->data.meaning);

                        MOUSEMSG n;
                        n = GetMouseMsg();

                        char meani[100];
                        InputBox(meani, 20, "�����뵥�ʵ���˼:");//��ʾһ�������
                        if (strcmp(meani, p->data.meaning) == 0) {
                           
                            p->data.memorized = 1;
                            p->data.correct_count++;
                            outtextxy(240, 400, "Congratulations! You have memorized the word .");

                        }
                        else {
                            
                            p->data.incorrect_count++;
                            outtextxy(240, 400, "Sorry,Please try again. ");
                        }


                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(365, 180, 445, 205);
                }

                rectangle(855, 515, 990, 550);

                //���ذ�ť�Ͱ�ť����
                if (m.x >= 855 && m.x <= 990 && m.y >= 515 && m.y <= 550) {
                    setlinecolor(RED);
                    rectangle(855, 515, 990, 550);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        Sleep(200);  //��ֹ����
                        putimage(0, 0, &bg);

                        flag = 1;
                        p = p->next;  //�Ա��ٴ�ѧϰ�����ǽ�����һ�εģ������ظ�ѧϰ
                        learn_word(p);//��ʣ���δ����ĵ�������ת��ȥ
                    }

                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(375, 515, 455, 550);
                }

                //�����𰸰�ť�Ͱ�ť����
                if (m.x >= 230 && m.x <= 320 && m.y >= 580 && m.y <= 620) {

                    setlinecolor(RED);
                    rectangle(230, 580, 320, 620);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        //���Ʊ���ɫ��
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(335, 575, 550, 625, 20, 20);
                        Sleep(200);
                        outtextxy(350, 590, p->data.word);

                       
                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(230, 580, 320, 620);
                }

                //�����ְ�ť�Ͱ�ť����
                if (m.x >= 875 && m.x <= 1000 && m.y >= 590 && m.y <= 630)
                {
                    setlinecolor(RED);//���õ��ʱΪ��ɫ 
                    rectangle(875, 590, 1000, 630);
                    if (m.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
                    {
                        // ֹͣ���Ų��ر�����
                        mciSendString("stop bk", NULL, 0, NULL);
                        mciSendString("close bk", NULL, 0, NULL);

                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(875, 590, 1000, 630);
                }

                //�ر����ְ�ť�Ͱ�ť����
                if (m.x >= 875 && m.x <= 1000 && m.y >= 640 && m.y <= 680)//���    outtextxy(885, 500, "��������");
                {
                    setlinecolor(RED);//���õ��ʱΪ��ɫ 
                    rectangle(875, 640, 1000, 680);
                    if (m.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
                    {
                        // ������
                        mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                        // ��������
                        mciSendString("play bk repeat", NULL, 0, NULL);
                    }
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(875, 640, 1000, 680);
                }
            }
        }
    }

}

//������
void nodoLine()
{
    cleardevice();
    putimage(0, 0, &bg);

    setlinecolor(WHITE);
    setfillcolor(WHITE);

    fillroundrect(0, 0, 780, 720, 20, 20);
    setlinecolor(BLACK);
    setbkcolor(WHITE);
    setlinestyle(PS_SOLID, 3);

    line(0, 2, 780, 2);
    line(0, 6, 780, 6);
   
    line(0, 34, 780, 34);
    line(2, 0, 2, 700);
    line(6, 0, 6, 700);
    line(789, 0, 789, 700);
    line(785, 0, 785, 700);
    line(0, 697, 780, 697);
    line(0, 693, 780, 693);
    setlinestyle(PS_SOLID, 1);
    line(240, 0, 240, 700);
    line(473, 0, 473, 700);

    for (int i = 0; i < 27; i++)
    {
        line(0, 60 + 25 * i, 780, 60 + 25 * i);
    }
    settextcolor(RED);
    outtextxy(10, 10, "���");
    outtextxy(246, 10, "����");
    outtextxy(479, 10, "����");
    settextcolor(BLACK);
}

//��Ӧ���Ƶı����������������ݺ�����������ʼ���
void printlist1(struct Node* head, int cnt) 
{

    settextstyle(30, 0, ("΢���ź�"));

    struct Node* temp = head;
    unsigned int i;
    i = 37;
    int T = 26;
    char num[5] = { '\0' };
    
    nodoLine();

    while (T--)
    {
        sprintf(num, "%d", cnt);

        outtextxy(13, i, num);
        outtextxy(249, i, temp->data.word);
        outtextxy(482, i, temp->data.meaning);
        i += 25;

        cnt++;
        temp = temp->next;

    }

}

//��Ӧ���Ƶı����������������ݺ��������ʹ���Ѽ��䵥�ʼ���
void printlist2(struct Node* head, int cnt)
{
    
    struct Node* temp = head;
    unsigned int i;
    i = 37;
    float fla = 0;
    char num[5] = { '\0' };

    int f = 0;

    nodoLine();
    while (temp != NULL)
    {
        f = 1;
        sprintf(num, "%d", cnt);

        outtextxy(13, i, num);
        outtextxy(249, i, temp->data.meaning);
        outtextxy(482, i, temp->data.word);
        cnt++;
        i += 25;

        fla++;

        if ((fla / 26.0) >= 1)
        {
            break;
        }
        else
        {
            temp = temp->next;
        }
    }
    if (f == 0)
    {
        HWND hwnd1 = GetHWnd();
        MessageBox(hwnd1, "�޼��䵥�ʣ�", "����", MB_OKCANCEL);
    }


}

//�鿴���ʼ�
void  show_all_words()
{
    setfillcolor(WHITE);
    int i = 0;
    int cnt = 1;
    int vis = 0;

    nodoLine();  //������
    Node* h = create_word_list();

    Node* remember[100] = { 0,NULL };
    Node* start = h;

    int pag = 1;

    start = start->next;
    while (start->next != NULL)
    {
        remember[pag] = start;
        pag++;
        int cycle = 26;
        while (cycle-- && start->next != NULL) {
            start = start->next;
        }
    }
    pag = 0;
    cnt = -25;

    cnt += 26;
    pag++;
    start = remember[pag];
    Sleep(200);
    printlist1(start, cnt);


    MOUSEMSG m;
    while (1) {
        settextstyle(30, 0, ("΢���ź�"));

        char st3[] = "��һҳ";
        char st4[] = "��һҳ";
        char st5[] = "����";

        outtextxy(950, 570, st3);
        outtextxy(820, 570, st4);
        outtextxy(920, 520, st5);

        m = GetMouseMsg();
        //������һҳ��ť�Ͱ�ť����
        if (m.x >= 940 && m.x <= 1020 && m.y >= 565 && m.y <= 610) {  
            setlinecolor(RED);
            rectangle(940, 565, 1020, 610);

            if (m.uMsg == WM_LBUTTONDOWN) {
                Sleep(200);  //��ֹ����

                vis = 0;
                pag++;
                start = remember[pag];
                Sleep(200);
                cnt += 26;
                printlist1(start, cnt);

            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(940, 565, 1020, 610);
        }

        //������һҳ��ť�Ͱ�ť����
        if (m.x >= 820 && m.x <= 900 && m.y >= 565 && m.y <= 610) { 
            setlinecolor(RED);
            rectangle(820, 565, 900, 610);
            if (m.uMsg == WM_LBUTTONDOWN) {
                Sleep(200);  //��ֹ����
               
                pag--;
                if (!vis) cnt -= 26;

                if (pag == 0 || cnt < 0)
                {
                    pag = 1;
                    if (!vis)  cnt += 26;
                    vis = 1;
                    HWND hwnd1 = GetHWnd();
                    MessageBox(hwnd1, "�Ѵ�����ҳ��", "����", MB_OKCANCEL);
                }
                printlist1(remember[pag], cnt);  //��ӡ����һҳ�ĵ�����Ϣ

            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�
            setlinecolor(WHITE);
            rectangle(820, 565, 900, 610);
        }

        //���ذ�ť�Ͱ�ť����
        if (m.x >= 895 && m.x <= 980 && m.y >= 515 && m.y <= 550) {  
            setlinecolor(RED);
            rectangle(895, 515, 980, 550);
            if (m.uMsg == WM_LBUTTONDOWN) {
                Sleep(200); //��ֹ����
                putimage(0, 0, &bg);
                learn_word(List);
            }

        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�
            setlinecolor(WHITE);
            rectangle(895, 515, 980, 550);
        }
    }
}

// ��������Ѽ��䵥�ʼ�
void print_words() {

    cleardevice();//���ͼ��
    loadimage(&bg, ("lake7.png"));
    putimage(0, 0, &bg);
    setfillcolor(WHITE);

    int i = 0;


    nodoLine();
 
    Node* remember[100] = { 0,NULL };
    Node* head = List;

    int cnt = 0;
    int vis = 0;
    int pag = 1;

    int cycle = 26;
    while (head != NULL)
    {
        if (head->data.memorized)
        {
            remember[pag] = head;
            cycle--;
            if (cycle == 0)
            {
                cycle = 26;
                pag++;
            }
        }
        head = head->next;

    }

    pag = 0;
    cnt = -25;

    Node* start;
    cnt += 26;
    pag++;
    start = remember[pag];
    Sleep(200);
    printlist2(start, cnt);


    MOUSEMSG m;
    while (1) {
        settextstyle(30, 0, ("΢���ź�"));

        char st3[] = "��һҳ";
        char st4[] = "��һҳ";
        char st5[] = "����";

        outtextxy(950, 570, st3);
        outtextxy(830, 570, st4);
        outtextxy(920, 520, st5);

        m = GetMouseMsg();
        //������һҳ��ť
        if (m.x >= 940 && m.x <= 1020 && m.y >= 565 && m.y <= 610) {
            setlinecolor(RED);
            rectangle(940, 565, 1020, 610);

            if (m.uMsg == WM_LBUTTONDOWN) {
                Sleep(200);

                vis = 0;
                pag++;
                start = remember[pag];
                if (start == NULL)
                {
                    HWND hwnd1 = GetHWnd();
                    MessageBox(hwnd1, "��һҳ���޼��䵥��", "����", MB_OKCANCEL);
                }
                else
                {
                    Sleep(200);
                    cnt += 26;
                    printlist2(start, cnt);
                }

            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(940, 565, 1020, 610);
        }

        //������һҳ��ť
        if (m.x >= 820 && m.x <= 900 && m.y >= 565 && m.y <= 610) {  //outtextxy(750, 570, st4);
            setlinecolor(RED);
            rectangle(820, 565, 900, 610);
            if (m.uMsg == WM_LBUTTONDOWN) {
                Sleep(200);

                pag--;
                if (!vis) cnt -= 26;

                if (pag == 0 || cnt < 0)
                {
                    pag = 1;
                    if (!vis)  cnt += 26;
                    vis = 1;
                    HWND hwnd1 = GetHWnd();
                    MessageBox(hwnd1, "�Ѵ�����ҳ��", "����", MB_OKCANCEL);
                }


                printlist2(remember[pag], cnt);

            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(820, 565, 900, 610);
        }

        //���ط��ذ�ť
        if (m.x >= 895 && m.x <= 980 && m.y >= 515 && m.y <= 550) {
            setlinecolor(RED);
            rectangle(895, 515, 980, 550);
            if (m.uMsg == WM_LBUTTONDOWN) {
                Sleep(200);
                putimage(0, 0, &bg);
                learn_word(List);
                //break;
            }

        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(895, 515, 980, 550);
        }

    }
}

//ѡ��Ҫ���䵥�ʵ�����
int choose_nums()
{
    putimage(0, 0, &bg);
    char num[10];

    InputBox(num, 10, "��������Ҫ���䵥�ʵ�����:");//����һ�������
    int i = 0;
    int k = 1, sum = 0;
   
    sum = atol(num);   //���ַ���ת��Ϊlong int ��������
    printf("\n%d\n", sum);
    return sum;
}

// ���䵥��ͳ�ƹ���
void view_statistics() {

    cleardevice();

    putimage(0, 0, &bg);
    setlinecolor(RGB(68, 114, 196));
    setfillcolor(RGB(68, 114, 196));

    fillroundrect(235, 95, 655, 365, 20, 20);
    setfillcolor(WHITE);

    struct Node* head = List;
    if (head->data.memorized == 1)
    {
        struct Node* head = List;
        int total = 0, memorized = 0, unmemorized = 0, looked = 0;
        int correct_count = 0, incorrect_count = 0;
        struct Node* current = head;
        while (current != NULL) {
            if (current->data.memorized) {
                memorized++;
                looked++;
                correct_count += current->data.correct_count;
                incorrect_count += current->data.incorrect_count;
            }
            else {
                unmemorized++;
            }
            total++;
            current = current->next;
        }

        char easiest_word[25], hardest_word[25];
        get_easiest_and_hardest_words(head, easiest_word, hardest_word);

        settextcolor(BLACK);
        settextstyle(30, 0, "΢���ź�");

        char s1[10] = { '0' };
        char s2[10] = { '0' };
        char s3[10] = { '0' };
        char s4[10] = { '0' };
        char s5[10] = { '0' };
        char s6[] = "����";
        char s7[] = "�Ѽ��䵥�ʼ�:";
        char s8[] = { '0' };

        printf("����ģ�%d\n", incorrect_count);
        sprintf(s1, "%d", memorized);
        printf("\n%d\n", memorized);
        sprintf(s8, "%d", looked);
        printf("%d\n", looked);
        sprintf(s2, "%d", unmemorized);
        printf("%d\n", unmemorized);
        sprintf(s3, "%d", total);
        printf("%d\n", total);
        
        printf("%.2f\n", (float)((float)correct_count / (float)(correct_count + incorrect_count)) * 100);
      
        printf("%.2f\n", (float)correct_count / (float)memorized);

        outtextxy(240, 100, "�Ѽ��䵥������");
        outtextxy(390, 100, s1);
     
        outtextxy(240, 150, "δ���䵥������");
        outtextxy(390, 150, s2);
        outtextxy(240, 175, "�ܵ�������");
        outtextxy(390, 175, s3);
        
        outtextxy(240, 250, "�����׵ĵ���:");
        outtextxy(560, 250, easiest_word);
        outtextxy(240, 275, "���Ѽ���ĵ���:");
        outtextxy(560, 275, hardest_word);
        outtextxy(920, 520, s6);

        fillrectangle(420, 325, 470, 360);
        
        outtextxy(240, 325, s7);



        while (1) {
            MOUSEMSG m3;
            m3 = GetMouseMsg();

            //�Ѽ��䵥�ʼ���ť
            if (m3.x >= 420 && m3.x <= 470 && m3.y >= 325 && m3.y <= 360) {
                setlinecolor(RED);
                rectangle(420, 325, 470, 360);
                if (m3.uMsg == WM_LBUTTONDOWN) {

                    putimage(0, 0, &bg);
                    print_words();

                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(420, 325, 470, 360);
                }
            }

            //���ذ�ť
            if (m3.x >= 855 && m3.x <= 990 && m3.y >= 515 && m3.y <= 550) {
                setlinecolor(RED);
                rectangle(855, 515, 990, 550);
                if (m3.uMsg == WM_LBUTTONDOWN) {

                    putimage(0, 0, &bg);
                    learn_word(List);

                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(855, 515, 990, 550);
                }
            }
        }
    }
    else
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "���޼������ݣ����ȼ��䵥�ʣ�", "��ʾ", MB_OKCANCEL);
    }

}

//ѡ��ѧϰ����ģʽ����
void start_learn(int number,struct Node*head)
{
    cleardevice();//���ͼ��
    loadimage(&bg, ("lake7.png"));
    putimage(0, 0, &bg);
    setfillcolor(WHITE);

    char subheading1[20] = { "ֱ�Ӽ���" };
    char subheading2[20] = { "Ĭд����" };
    char subheading3[20] = { "�˳�" };

    fillrectangle(370, 325, 700, 370);
    fillrectangle(370, 385, 700, 430);
    fillrectangle(370, 445, 700, 490);

    //�������� 
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(40, 0, "����");
    //��д����
    settextcolor(WHITE);
    outtextxy(420, 260, "LEARN WORDS");
    settextcolor(BLACK);
    settextstyle(30, 0, "����");
    outtextxy(460, 330, subheading1);
    outtextxy(460, 390, subheading2);
    outtextxy(480, 450, subheading3);

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();//��ȡ������

        //����ֱ�Ӽ��䰴ť����ư�ť����
        if (m1.x >= 370 && m1.x <= 700 && m1.y >= 325 && m1.y <= 370)
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(370, 325, 700, 370);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                memorize_word1(number,head);
            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                setlinecolor(WHITE);
                rectangle(370, 325, 700, 370);
            }
        }

        //����Ĭд���䰴ť����ư�ť����
        if (m1.x >= 370 && m1.x <= 700 && m1.y >= 385 && m1.y <= 430)
        {
            setlinecolor(RED);
            rectangle(370, 385, 700, 430);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                memorize_word2(number,head);
            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                setlinecolor(WHITE);
                rectangle(370, 385, 700, 430);
            }
        }


        //���Ʒ��ذ�ť����ư�ť����
        if (m1.x >= 370 && m1.x <= 700 && m1.y >= 445 && m1.y <= 490) {
            setlinecolor(RED);
            rectangle(370, 445, 700, 490);
            if (m1.uMsg == WM_LBUTTONDOWN) {

                putimage(0, 0, &bg);
                learn_word(List);
            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                setlinecolor(WHITE);
                rectangle(370, 445, 700, 490);
            }
        }
    }
}

//��ϰ���ʽ���
void re_learn()
{
    cleardevice();//���ͼ��
    loadimage(&bg, ("lake7.png"));
    putimage(0, 0, &bg);
    setfillcolor(WHITE);

    settextcolor(RGB(127, 127, 127));
    settextstyle(30, 0, "΢���ź�");

    setlinecolor(RGB(117, 166, 255));
    setfillcolor(RGB(117, 166, 255));

    fillroundrect(235, 95, 685, 265, 20, 20);
    setfillcolor(WHITE);

    Node* p = List;

    int f = 0;
    int f2 = 0;
    int cnt = 1;
    int i = 0;
    int sign = 0;

    char yema[5] = { '\0' };

    if (List->data.memorized == 1)
    {
        f = 1;

        //���ҳ��
        sprintf(yema, "%d", cnt);
        outtextxy(490, 520, yema);
        settextcolor(WHITE);

        // �����ǰ���ʺ�������Ϣ
        outtextxy(240, 100, "���ʣ�");
        outtextxy(240, 125, "���壺");
        outtextxy(300, 100, p->data.word);
        outtextxy(300, 125, p->data.meaning);

        settextcolor(WHITE);
        settextstyle(30, 0, "����");
        outtextxy(875, 595, "��������");
        outtextxy(875, 645, "�ر�����");
        
        settextstyle(30, 0, "΢���ź�");

        // ��ȡ�û�����ļ�����
        fflush(stdin); // ������뻺����
        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

        MOUSEMSG m;
        while (1) {
            char st1[] = "Yes";
            char st2[] = "No";
            char st3[] = "��һҳ";
            char st4[] = "��һҳ";
            char st5[] = "����";

            outtextxy(380, 175, st1);
            outtextxy(550, 175, st2);
            outtextxy(550, 520, st3);
            outtextxy(380, 520, st4);
            outtextxy(910, 520, st5);

            m = GetMouseMsg();

            //����Yes��ť�Ͱ�ť����
            if (m.x >= 365 && m.x <= 445 && m.y >= 180 && m.y <= 210) {

                setlinecolor(RED);
                rectangle(365, 180, 445, 205);
                if (m.uMsg == WM_LBUTTONDOWN) {
                    sign++;
                    pr[sign] = p;

                    p->data.memorized = 1;
                    p->data.correct_count++;
                    Sleep(200);
                    putimage(0, 0, &bg);

                    i++;
                    p = p->next;
                    if (p == NULL || p->data.memorized == 0)
                    {
                        f2 = 1;
                        break;
                    }

                    //���Ʊ���ɫ��
                    setlinecolor(RGB(117, 166, 255));
                    setfillcolor(RGB(117, 166, 255));
                    fillroundrect(235, 95, 685, 265, 20, 20);
                    setfillcolor(WHITE);
                    // �����ǰ���ʺ�������Ϣ

                    settextcolor(WHITE);
                    outtextxy(240, 100, "���ʣ�");
                    outtextxy(240, 125, "���壺");
                    outtextxy(300, 100, p->data.word);


                    outtextxy(300, 125, p->data.meaning);

                    // ��ȡ�û�����ļ�����
                    fflush(stdin); // ������뻺����
                    outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                    settextcolor(WHITE);
                    settextstyle(30, 0, "����");
                    outtextxy(875, 595, "��������");
                    outtextxy(875, 645, "�ر�����");

                   
                    settextstyle(30, 0, "΢���ź�");

                    cnt++;
                    sprintf(yema, "%d", cnt);
                    outtextxy(490, 520, yema);

                }
            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�
                setlinecolor(WHITE);
                rectangle(365, 180, 445, 205);
            }

            //����No��ť�Ͱ�ť����
            if (m.x >= 540 && m.x <= 620 && m.y >= 180 && m.y <= 205) {

                i++;

                setlinecolor(RED);
                rectangle(540, 180, 620, 205);

                if (m.uMsg == WM_LBUTTONDOWN) {

                    p->data.incorrect_count++;
                    sign++;
                    pr[sign] = p;

                    Sleep(200);  //��ֹ����
                    putimage(0, 0, &bg);

                    p = p->next;
                    if (p == NULL || p->data.memorized == 0)
                    {
                        f2 = 1;
                        break;
                    }

                    //���Ʊ���ɫ��
                    setlinecolor(RGB(117, 166, 255));
                    setfillcolor(RGB(117, 166, 255));
                    fillroundrect(235, 95, 685, 265, 20, 20);
                    setfillcolor(WHITE);

                    // �����ǰ���ʺ�������Ϣ
                    settextcolor(WHITE);
                    outtextxy(240, 100, "���ʣ�");
                    outtextxy(240, 125, "���壺");
                    outtextxy(300, 100, p->data.word);
                    outtextxy(300, 125, p->data.meaning);

                    // ��ȡ�û�����ļ�����
                    fflush(stdin); // ������뻺����
                
                    outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                    settextcolor(WHITE);
                    settextstyle(30, 0, "����");
                    outtextxy(875, 595, "��������");
                    outtextxy(875, 645, "�ر�����");
                   
                    settextstyle(30, 0, "΢���ź�");

                    cnt++;
                    sprintf(yema, "%d", cnt);
                    outtextxy(490, 520, yema);

                }
            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                setlinecolor(WHITE);
                rectangle(540, 180, 620, 205);
            }


            //������һҳ��ť�Ͱ�ť����
            if (m.x >= 545 && m.x <= 625 && m.y >= 515 && m.y <= 550) {
                setlinecolor(RED);
                rectangle(545, 515, 625, 550);

                i++;

                if (m.uMsg == WM_LBUTTONDOWN) {
                    Sleep(200);
                    putimage(0, 0, &bg);

                    sign++;
                    pr[sign] = p;

                    p = p->next;
                    if (p == NULL||p->data.memorized==0)
                    {
                        f2 = 1;
                        break;
                    }

                    //���Ʊ���ɫ��
                    setlinecolor(RGB(117, 166, 255));
                    setfillcolor(RGB(117, 166, 255));
                    fillroundrect(235, 95, 685, 265, 20, 20);
                    setfillcolor(WHITE);
                    // �����ǰ���ʺ�������Ϣ

                    settextcolor(WHITE);
                    outtextxy(240, 100, "���ʣ�");
                    outtextxy(240, 125, "���壺");
                    outtextxy(300, 100, p->data.word);
                    outtextxy(300, 125, p->data.meaning);

                    // ��ȡ�û�����ļ�����
                    fflush(stdin); // ������뻺����

                    outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                    settextcolor(WHITE);
                    settextstyle(30, 0, "����");
                    outtextxy(875, 595, "��������");
                    outtextxy(875, 645, "�ر�����");
                    settextstyle(30, 0, "΢���ź�");

                    cnt++;
                    sprintf(yema, "%d", cnt);
                    outtextxy(490, 520, yema);

                }
            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�
                setlinecolor(WHITE);
                rectangle(545, 515, 625, 550);
            }

            //������һҳ��ť�Ͱ�ť����
            if (m.x >= 375 && m.x <= 455 && m.y >= 515 && m.y <= 550) {
                setlinecolor(RED);
                rectangle(375, 515, 455, 550);
                if (m.uMsg == WM_LBUTTONDOWN) {
                    putimage(0, 0, &bg);

                    sign--;
                    if (sign == 0)
                    {
                        sign = 1;
                        HWND hwnd1 = GetHWnd();
                        MessageBox(hwnd1, "�Ѵ�����ҳ��", "����", MB_OKCANCEL);
                    }
                    setlinecolor(RGB(117, 166, 255));
                    setfillcolor(RGB(117, 166, 255));

                    fillroundrect(235, 95, 685, 265, 20, 20);
                    setfillcolor(WHITE);

                    settextcolor(WHITE);

                    outtextxy(240, 100, "���ʣ�");
                    outtextxy(240, 125, "���壺");
                    outtextxy(300, 100, pr[sign]->data.word);
                    outtextxy(300, 125, pr[sign]->data.meaning);

                    // ��ȡ�û�����ļ�����
                    fflush(stdin); // ������뻺����

                    outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                    settextcolor(WHITE);
                    settextstyle(30, 0, "����");
                    outtextxy(875, 595, "��������");
                    outtextxy(875, 645, "�ر�����");

                    settextstyle(30, 0, "΢���ź�");

                    cnt--;
                    sprintf(yema, "%d", cnt);
                    outtextxy(490, 520, yema);
                }
            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ����ɫ�߿�.
                setlinecolor(WHITE);
                rectangle(375, 515, 455, 550);
            }

            //��ʼ���ְ�ť�Ͱ�ť����
            if (m.x >= 875 && m.x <= 1000 && m.y >= 590 && m.y <= 630)
            {
                setlinecolor(RED);//���õ��ʱΪ��ɫ 
                rectangle(875, 590, 1000, 630);
                if (m.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
                {
                    // ������
                    mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                    // ��������
                    mciSendString("play bk repeat", NULL, 0, NULL);
                }
            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                setlinecolor(WHITE);
                rectangle(875, 590, 1000, 630);
            }

            //�������ְ�ť�Ͱ�ť����
            if (m.x >= 875 && m.x <= 1000 && m.y >= 640 && m.y <= 680)//���    outtextxy(885, 500, "��������");
            {
                setlinecolor(RED);//���õ��ʱΪ��ɫ 
                rectangle(875, 640, 1000, 680);
                if (m.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
                {
                    // ֹͣ���Ų��ر�����
                    mciSendString("stop bk", NULL, 0, NULL);
                    mciSendString("close bk", NULL, 0, NULL);
                }
            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�
                setlinecolor(WHITE);
                rectangle(875, 640, 1000, 680);
            }
            //���ذ�ť�Ͱ�ť����
            if (m.x >= 865 && m.x <= 990 && m.y >= 515 && m.y <= 550) {
                setlinecolor(RED);
                rectangle(865, 515, 990, 550);
                if (m.uMsg == WM_LBUTTONDOWN) {
                    Sleep(200);
                    putimage(0, 0, &bg);

                    flag = 1;
                    p = p->next;// //�Ա��ٴ�ѧϰ�����ǽ�����һ�εģ������ظ�ѧϰ

                    learn_word(List);  //��ʣ���δ����ĵ�������ת��ȥ
                    
                }
                else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                    setlinecolor(WHITE);
                    rectangle(865, 515, 990, 550);
                }

            }

        }

    }
    else
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "���Ѽ���ĵ���,�޷���ϰ��", "��ʾ", MB_OKCANCEL);
    }

    char st6[] = "����";
    outtextxy(920, 520, st6);
    if (f2)
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "���е����Ѹ�ϰ��ɣ�", "��ʾ", MB_OKCANCEL);
    }

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();
        //���ذ�ť
        if (m1.x >= 865 && m1.x <= 990 && m1.y >= 515 && m1.y <= 550) {
            setlinecolor(RED);
            rectangle(865, 515, 990, 550);
            if (m1.uMsg == WM_LBUTTONDOWN) {
                Sleep(200);
                putimage(0, 0, &bg);

                flag = 1;
                p = p->next;
                learn_word(List);  //��ʣ���δ����ĵ�������ת��ȥ
                //����start_learn���棬������learn_word���棬����Ϊ�Լ�д�����Խ��ű��Ĺ��ܣ������˷�


            }
            else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
                setlinecolor(WHITE);
                rectangle(865, 515, 990, 550);
            }
        }
    }
}

//���浥����Ϣ��ָ���ļ�
void save_words_to_file(struct Node* head) {

    char savefilename[150] = "savefile.txt";
    FILE* fp = fopen(savefilename, "w");
    if (fp == NULL) {
        HWND hwnd1 = GetHWnd();
        MessageBox(hwnd1, "�����ļ�ʧ�ܣ��޷��˳�ϵͳ��", "����", MB_OKCANCEL);
    }
    for (struct Node* p = head; p != NULL; p = p->next) {
        fprintf(fp, "%s\t%s\t%d\t%d\n", p->data.word, p->data.meaning, p->data.correct_count, p->data.incorrect_count);
        printf("%s\t%s\t%d\t%d\n", p->data.word, p->data.meaning, p->data.correct_count, p->data.incorrect_count);
    }
    fclose(fp);
    closegraph();
}

//�뿪ϵͳ
void exitsystem()
{
    save_words_to_file(List);
    return;
}

//������ϵͳ������
void learn_word(struct Node*head)
{
    cleardevice();//���ͼ��

    putimage(0, 0, &bg);
    setfillcolor(WHITE);//���ñ���ɫ

    char heading0[20] = { "�鿴���ʼ�" };
    char heading1[20] = { "��ʼ����" };
    char heading2[20] = { "���䵥������" };
    char heading3[20] = { "��ϰ����" };
    char heading4[20] = { "����" };
    char heading5[20] = { "�˳�ϵͳ" };
   
    setlinecolor(RGB(117, 166, 255));
    setfillcolor(RGB(117, 166, 255));

    fillroundrect(320, 240, 660, 620, 20, 20);
    setfillcolor(WHITE);
    fillrectangle(330, 265, 650, 310);
    fillrectangle(330, 325, 650, 370);
    fillrectangle(330, 385, 650, 430);
    fillrectangle(330, 445, 650, 490);
    fillrectangle(330, 505, 650, 550);
    fillrectangle(330, 565, 650, 610);
  
    //�������� 
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(40, 0, "����");
    //��д����
    settextcolor(WHITE);
    outtextxy(390, 200, "LEARN WORDS");
    settextcolor(BLACK);
    settextstyle(30, 0, "����");
    outtextxy(400, 270, heading0);
    outtextxy(400, 330, heading1);
    outtextxy(400, 390, heading2);
    outtextxy(400, 450, heading3);
    outtextxy(440, 510, heading4);
    outtextxy(400, 570, heading5);

    settextcolor(WHITE);
    settextstyle(30, 0, "����");
    outtextxy(875, 595, "�ر�����");
    outtextxy(875, 500, "��������");

    // ������1

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();//��ȡ������
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 265 && m1.y <= 310)//��� f (m1.x >= 330 && m1.x <= 650 && m1.y >= 325 && m1.y <= 370)//��� 
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(330, 265, 650, 310);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                Sleep(200);
                cleardevice();//���ͼ��

                show_all_words();
            }

        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(330, 265, 650, 310);
        }
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 325 && m1.y <= 370)//��� 
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(330, 325, 650, 370);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                Sleep(200);
                cleardevice();//���ͼ��

                amount = choose_nums();
                start_learn(amount,head);
            }

        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(330, 325, 650, 370);
        }
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 385 && m1.y <= 430)//��ͨ�û�����
        {
            setlinecolor(RED);
            rectangle(330, 385, 650, 430);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                Sleep(200);
                cleardevice();//���ͼ��

                view_statistics();
            }

        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(330, 385, 650, 430);
        }
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 445 && m1.y <= 490)
        {
            setlinecolor(RED);
            rectangle(330, 445, 650, 490);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                Sleep(200);
                cleardevice();
                //exit();

                re_learn();
            }

        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(330, 445, 650, 490);
        }
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 505 && m1.y <= 550)
        {
            setlinecolor(RED);
            rectangle(330, 505, 650, 550);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                createplan();
            }

        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(330, 505, 650, 550);
        }
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 565 && m1.y <= 610)
        {
            setlinecolor(RED);
            rectangle(330, 565, 650, 610);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                exitsystem();
            }

        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(330, 565, 650, 610);
        }
        if (m1.x >= 880 && m1.x <= 1000 && m1.y >= 590 && m1.y <= 640)
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(880, 590, 1000, 640);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                // ֹͣ���Ų��ر�����
                mciSendString("stop bk", NULL, 0, NULL);
                mciSendString("close bk", NULL, 0, NULL);

            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(880, 590, 1000, 640);
        }

        if (m1.x >= 880 && m1.x <= 1000 && m1.y >= 495 && m1.y <= 545)
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(880, 495, 1000, 545);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                // ������
                mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                // ��������
                mciSendString("play bk repeat", NULL, 0, NULL);
            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(880, 495, 1000, 545);
        }
    }
}

//��������ƻ�
void random_plan() {
    cleardevice();
    putimage(0, 0, &bg);

    struct Node* h = create_word_list();
    List = create_random_plan(h);

    printf("here");
    if (List != NULL)
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "�����������ƻ��ɹ�", "��ʾ", MB_OKCANCEL);

    }
    learn_word(List);
}

//����˳��ƻ�
void order_plan()
{
    cleardevice();
    putimage(0, 0, &bg);
    List = create_word_list();
    if (List != NULL)
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "����˳�����ƻ��ɹ�", "��ʾ", MB_OKCANCEL);
    }
    learn_word(List);

}

//�������䵥�ʼƻ�
void createplan()
{
    char subheading0[20] = { "����ȷ������ƻ�" };
    char subheading1[20] = { "�������ƻ�" };
    char subheading2[20] = { "˳�����ƻ�" };
    char subheading3[20] = { "����" };

    // ������ѡ�񱳾�ͼƬ
    loadimage(&bg, ("lake7.png"));
    putimage(0, 0, &bg);

    //���þ��ο� 
    setlinecolor(RGB(117, 166, 255));
    setfillcolor(RGB(117, 166, 255));

    fillroundrect(320, 240, 660, 500, 20, 20);
    setfillcolor(RGB(255, 255, 255));
    fillrectangle(330, 325, 650, 370);
    fillrectangle(330, 385, 650, 430);
    fillrectangle(330, 445, 650, 490);

    //�������� 
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(35, 0, "����");
    //��д����

    outtextxy(340, 250, subheading0);
    settextstyle(30, 0, "����");
    outtextxy(400, 330, subheading1);
    outtextxy(400, 390, subheading2);
    outtextxy(450, 450, subheading3);
    settextcolor(WHITE);
    settextstyle(30, 0, "����");
    outtextxy(875, 595, "�ر�����");
    outtextxy(875, 500, "��������");

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();//��ȡ������ 

        //�����������ƻ���ť�Ͱ�ť����
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 325 && m1.y <= 370)//���     
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(330, 325, 650, 370);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                random_plan();
            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(330, 325, 650, 370);
        }

        //����˳�����ƻ���ť�Ͱ�ť����
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 385 && m1.y <= 430)//��ͨ�û�����
        {
            setlinecolor(RED);
            rectangle(330, 385, 650, 430);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                order_plan();
            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�
            setlinecolor(WHITE);
            rectangle(330, 385, 650, 430);
        }

        //���Ʒ��ذ�ť�Ͱ�ť����
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 445 && m1.y <= 490)
        {
            setlinecolor(RED);
            rectangle(330, 445, 650, 490);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                welcome();
            }

        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(330, 445, 650, 490);
        }

        //���ƹر����ְ�ť�Ͱ�ť����
        if (m1.x >= 875 && m1.x <= 1000 && m1.y >= 590 && m1.y <= 630)
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(875, 590, 1000, 630);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                // ֹͣ���Ų��ر�����
                mciSendString("stop bk", NULL, 0, NULL);
                mciSendString("close bk", NULL, 0, NULL);

            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ����ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(875, 590, 1000, 630);
        }

        //���ƿ�ʼ���ְ�ť�Ͱ�ť����
        if (m1.x >= 875 && m1.x <= 1000 && m1.y >= 495 && m1.y <= 545)//���    outtextxy(885, 500, "��������");
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(875, 495, 1000, 545);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                // ������
                mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                // ��������
                mciSendString("play bk repeat", NULL, 0, NULL);
            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�
            setlinecolor(WHITE);
            rectangle(875, 495, 1000, 545);
        }

    }
    system("pause");
    closegraph();
}

// ���뻶ӭ����
void welcome()
{

    // ���Ʊ���ͼƬ
    loadimage(&bg, ("lake7.png"));
    putimage(0, 0, &bg);

     // ���ƾ��ΰ�ť
    int button_x = 460, button_y = 450;
    setlinecolor(RGB(255, 255, 255));  // ����������ɫΪ����ɫ
    setfillcolor(RGB(255, 255, 255));
    fillrectangle(button_x, button_y, button_x + 100, button_y + 50);
   

    // ���Ʊ���

    settextcolor(WHITE);// ������ɫΪ��ɫ
    setbkmode(TRANSPARENT); // ���ñ���͸��
    settextstyle(40, 0, ("΢���ź�"));
    outtextxy(380, 100, ("��ӭ����������ϵͳ��"));
    settextstyle(30, 0, ("΢���ź�"));
    outtextxy(885, 595, "�ر�����");
    outtextxy(885, 500, "��������");

    // ������ʾ��Ϣ
    settextstyle(30, 0, "΢���ź�");
    outtextxy(395, 300, "�����·���ɫ��ť��ʼѧϰ��");

    outtextxy(282, 197, "��ǰʱ�䣺");
    time_t clock;
    //��ȡ��ǰʱ��
    time(&clock);
    outtextxy(428, 197, ctime(&clock));
    line(100, HEIGHT / 3, 900, HEIGHT / 3);

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();//��ȡ������

        //���ƽ���ѡ��ƻ���ť�Ͱ�ť����
        if (m1.x >= button_x && m1.x <= button_x + 100 && m1.y >= button_y && m1.y <= button_y + 50)//��� 
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(button_x, button_y, button_x + 100, button_y + 50);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                createplan();
            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�
            setlinecolor(WHITE);
            rectangle(button_x, button_y, button_x + 100, button_y + 50);
        }

        //���ƹر����ְ�ť�Ͱ�ť����
        if (m1.x >= 880 && m1.x <= 980 && m1.y >= 590 && m1.y <= 640)//���    //outtextxy(885, 595, "�ر�����");
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(880, 590, 980, 640);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                // ֹͣ���Ų��ر�����
                mciSendString("stop bk", NULL, 0, NULL);
                mciSendString("close bk", NULL, 0, NULL);

            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�
            setlinecolor(WHITE);
            rectangle(880, 590, 980, 640);
        }

        //���ƿ�ʼ���ְ�ť�Ͱ�ť����
        if (m1.x >= 880 && m1.x <= 980 && m1.y >= 495 && m1.y <= 545)//���    outtextxy(885, 500, "��������");
        {
            setlinecolor(RED);//���õ��ʱΪ��ɫ 
            rectangle(880, 495, 980, 545);
            if (m1.uMsg == WM_LBUTTONDOWN) //�������һ�δ��� 
            {
                // ������
                mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                // ��������
                mciSendString("play bk repeat", NULL, 0, NULL);
            }
        }
        else {	//����겻��Ŀ��λ�þ͸���֮ǰ�ĺ�ɫ�߿�.
            setlinecolor(WHITE);
            rectangle(880, 495, 980, 545);
        }

    }
    Sleep(80);
    // ��մ���
    cleardevice();
}

//������
int main()
{
    // ��ʼ��ͼ�ν���
    initgraph(1080, 720);

    welcome();  //���뻶ӭ����

    return 0;
}