//背单词系统
#define _CRT_SECURE_NO_WARNINGS 1  //消除scanf等函数不安全的错误

#undef UNICODE   //取消 Unicode 编码的宏定义，让整个项目以 MBCS 编码编译

#pragma comment(lib,"Winmm.lib")  // 引用 Windows Multimedia API

#include <graphics.h>  //使用EasyX图形库函数
#include <conio.h>
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include <assert.h>


// 单词信息结构体类型
struct Word {
    char word[50]; // 单词
    char meaning[100]; // 单词词义
    int correct_count; // 正确记忆次数
    int incorrect_count; // 错误记忆次数
    int memorized;// 是否已记忆（0：未记忆，1：已记忆）
};



// 动态链表结点类型
typedef struct Node {
    struct Word data;
    struct Node* next;
}Node;

#define MAX_LINE_LEN 200 //接受单词文件一行的最长字符长

int word_cnt = 0;  //单词文件中单词总数量
const int HEIGHT = 1080;  //开辟窗口高
const int WIDTH = 720;    //开辟窗口宽

int flag = 0;  //标记
int amount;   //一次要计划记忆的单词数量

struct Node* List; //全局链表

Node* pr[5000] = { 0,NULL };  //记录前一页单词数据

IMAGE bg;  //定义图片类型


struct Node* read_words_from_file();
struct Node* create_random_plan(struct Node* head);

//一些必要的声明，以实现返回按钮的功能
void start_learn(int number,struct Node*head);
void learn_word(struct Node*head);
void nodoLine();
void sort_list(struct Node*head);
void welcome();
void createplan();


// 根据单词首字母排序链表
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

// 创建根据单词首字母排序的单词信息动态链表
struct Node* create_word_list() {

    struct Node* head = read_words_from_file();
    sort_list(head);
    return head;

}

// 创建新结点
struct Node* create_node(struct Word word) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    if (new_node == NULL) return NULL;
    new_node->data = word;
    new_node->next = NULL;
    return new_node;
}

// 尾插法创建链表
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

// 统计最容易和最难记忆的单词
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

// 从文件中读取单词列表
struct Node* read_words_from_file() {


    FILE* file;
    if ((file = fopen("word.txt", "r")) == NULL) { //若无此文件
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "word.txt文件缺失", "错误", MB_OKCANCEL);
        exit(0);
    }


    struct Node* head = NULL;
    struct Node* tail = NULL;
    char line[MAX_LINE_LEN];
    while (fgets(line, MAX_LINE_LEN, file) != NULL) {

        // 忽略空行和仅包含空格或制表符的行
        if (strcmp(line, "\n") == 0 || strspn(line, " \t\n\r") == strlen(line)) {
            continue;
        }

        char word[50] = { '\0' };
        char definition[100] = { '\0' };
        // 解析单词和释义信息
        if (sscanf(line, "%49[^ ] %99[^\n]", word, definition) != 2) {
            //printf("Invalid format: %s\n", line);
            continue;
        }
        // 创建动态链表结点
        struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
        if (new_node == NULL) {
            printf("Memory allocation failed\n");
            break;
        }
        new_node->next = NULL;
        // 复制单词和释义信息到链表结点中
        new_node->data.word[49] = '\0'; // 手动添加字符串零终止符
        new_node->data.meaning[99] = '\0'; // 手动添加字符串零终止符
        strncpy(new_node->data.word, word, 50);
        strncpy(new_node->data.meaning, definition, 100);

        new_node->data.correct_count = 0;
        new_node->data.incorrect_count = 0;
        new_node->data.memorized = 0;

        // 将新结点加入链表
        if (tail == NULL) {
            head = tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = new_node;
        }

    }
    //printf("读取成功！\n");
    fclose(file);
    return head;
}

//创建随机记忆计划具体实现
struct Node* create_random_plan(struct Node* head) {
    
    struct Node* p;

    // 计算单词数量
    for (p = head; p != NULL; p = p->next) {
        word_cnt++;
    }

    srand((unsigned int)time(NULL));  //生成随机数种子

    //动态分配内存
    struct Word* words = (struct Word*)malloc((word_cnt + 1) * sizeof(struct Word));
    assert(words); //判断是否为空
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

    //生成随机数组
    for (int i = word_cnt - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp_val = word_indices[i];
        word_indices[i] = word_indices[j];
        word_indices[j] = temp_val;
    }

    for (int i = 0; i < word_cnt; i++) {
        words[i] = temp[word_indices[i]];
    }

    //释放内存
    free(temp);
    free(word_indices);

    return create_list(words);  
}

//直接记忆
void memorize_word1(int number,struct Node*p) {
    cleardevice();
    putimage(0, 0, &bg);

    int f = 0;
    int cnt = 0;
    int sign = 0;  //标记
    char yema[5] = { '\0' };  //页码

    //绘制背景色块
    setlinecolor(RGB(117, 166, 255));
    setfillcolor(RGB(117, 166, 255));
    fillroundrect(235, 95, 685, 265, 20, 20);
    setfillcolor(WHITE);


    int i = 0;
    while (p != NULL) {

       

            FILE* file;
            if ((file = fopen("word.txt", "r")) == NULL) { //若无此文件
                HWND hwnd = GetHWnd();
                MessageBox(hwnd, "读取的单词文件缺失", "错误", MB_OKCANCEL);
                exit(0);
            }

            settextcolor(WHITE);
            settextstyle(30, 0, "微软雅黑");

            number--;
            if (number < 0)
            {
                HWND hwnd1 = GetHWnd();
                MessageBox(hwnd1, "太棒了！（^ ^）已达到计划记忆的单词数量，如需要，请按返回按钮退出", "提醒", MB_OKCANCEL);
            }

            //绘制背景色块
            setlinecolor(RGB(117, 166, 255));
            setfillcolor(RGB(117, 166, 255));
            fillroundrect(235, 95, 685, 265, 20, 20);


            // 输出当前单词和释义信息
            outtextxy(240, 100, "单词：");
            outtextxy(240, 125, "释义：");
            outtextxy(300, 100, p->data.word);
            outtextxy(300, 125, p->data.meaning);

            settextcolor(WHITE);
            settextstyle(30, 0, "楷体");
            outtextxy(875, 595, "开启音乐");
            outtextxy(875, 645, "关闭音乐");

            settextcolor(WHITE);
            settextstyle(30, 0, "微软雅黑");


            // 读取用户输入的记忆结果
            fflush(stdin); // 清空输入缓冲区

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
                char st3[] = "下一页";
                char st4[] = "上一页";
                char st5[] = "返回";

                outtextxy(380, 175, st1);
                outtextxy(550, 175, st2);
                outtextxy(550, 520, st3);
                outtextxy(380, 520, st4);
                outtextxy(910, 520, st5);

                m = GetMouseMsg();

                //创建Yes按钮和按钮操作
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
                            MessageBox(hwnd1, "太棒了！（^ ^）已达到计划记忆的单词数量，如需要，请按返回按钮退出", "提醒", MB_OKCANCEL);
                        }

                       
                        p->data.memorized = 1;
                        p->data.correct_count++;
                        Sleep(200);
                        putimage(0, 0, &bg);

                        i++;
                        p = p->next;

                        //绘制背景色块
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));

                        fillroundrect(235, 95, 685, 265, 20, 20);

                        // 输出当前单词和释义信息
                        outtextxy(240, 100, "单词：");
                        outtextxy(240, 125, "释义：");
                        outtextxy(300, 100, p->data.word);


                        outtextxy(300, 125, p->data.meaning);

                        // 读取用户输入的记忆结果
                        fflush(stdin); // 清空输入缓冲区
            
                        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "楷体");
                        outtextxy(875, 595, "开启音乐");
                        outtextxy(875, 645, "关闭音乐");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");

                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                    }
                }
                else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
                    setlinecolor(WHITE);
                    rectangle(365, 180, 445, 205);
                }


                //创建No按钮和按钮操作
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
                            MessageBox(hwnd1, "太棒了！（^ ^）已达到计划记忆的单词数量，如需要，请按返回按钮退出", "提醒", MB_OKCANCEL);
                        }

                        p = p->next;

                        //绘制背景色块
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        // 输出当前单词和释义信息
                        outtextxy(240, 100, "单词：");
                        outtextxy(240, 125, "释义：");
                        outtextxy(300, 100, p->data.word);
                        outtextxy(300, 125, p->data.meaning);

                        // 读取用户输入的记忆结果
                        fflush(stdin); // 清空输入缓冲区
                        //char input;
                        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "楷体");
                        outtextxy(875, 595, "开启音乐");
                        outtextxy(875, 645, "关闭音乐");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");

                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                    }
                }
                else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
                    setlinecolor(WHITE);
                    rectangle(540, 180, 620, 205);
                }


                //创建下一页按钮和按钮操作
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
                            MessageBox(hwnd1, "太棒了！（^ ^）已达到计划记忆的单词数量，如需要，请按返回按钮退出", "提醒", MB_OKCANCEL);
                        }

                        p = p->next;

                        //绘制背景色块
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        // 输出当前单词和释义信息
                        outtextxy(240, 100, "单词：");
                        outtextxy(240, 125, "释义：");
                        outtextxy(300, 100, p->data.word);
                        outtextxy(300, 125, p->data.meaning);

                        // 读取用户输入的记忆结果
                        fflush(stdin); // 清空输入缓冲区

                        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "楷体");
                        outtextxy(875, 595, "开启音乐");
                        outtextxy(875, 645, "关闭音乐");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");

                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                    }
                }
                else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
                    setlinecolor(WHITE);
                    rectangle(545, 515, 625, 550);
                }

                //创建上一页按钮和按钮操作
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
                            MessageBox(hwnd1, "已处于首页！", "错误", MB_OKCANCEL);
                        }
                        //绘制背景色块
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);
                        
                        // 输出当前单词和释义信息
                        outtextxy(240, 100, "单词：");
                        outtextxy(240, 125, "释义：");
                        outtextxy(300, 100, pr[sign]->data.word);
                        outtextxy(300, 125, pr[sign]->data.meaning);

                        // 读取用户输入的记忆结果
                        fflush(stdin); // 清空输入缓冲区

                        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                        number++;


                        settextcolor(WHITE);
                        settextstyle(30, 0, "楷体");
                        outtextxy(875, 595, "开启音乐");
                        outtextxy(875, 645, "关闭音乐");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");

                        cnt--;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);
                    }
                }
                else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
                    setlinecolor(WHITE);
                    rectangle(375, 515, 455, 550);
                }

                //返回按钮和按钮操作
                if (m.x >= 865 && m.x <= 990 && m.y >= 515 && m.y <= 550) {
                    setlinecolor(RED);
                    rectangle(865, 515, 990, 550);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        Sleep(200);
                        putimage(0, 0, &bg);

                        flag = 1;
                        p = p->next;  //以便再次学习单词是紧接上一次的，避免重复学习

                        learn_word(List);  //将剩余的未记忆的单词数量转过去

                    }
                    else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                        setlinecolor(WHITE);
                        rectangle(865, 515, 990, 550);
                    }
                }


                setlinecolor(WHITE);
                rectangle(865, 515, 990, 550);
                


                //开始音乐按钮和按钮操作
                if (m.x >= 875 && m.x <= 1000 && m.y >= 590 && m.y <= 630)
                {
                    setlinecolor(RED);//设置电击时为红色 
                    rectangle(875, 590, 1000, 630);
                    if (m.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
                    {


                        // 打开音乐
                        mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                        // 播放音乐
                        mciSendString("play bk repeat", NULL, 0, NULL);

                    }
                }
                else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
                    setlinecolor(WHITE);
                    rectangle(875, 590, 1000, 630);
                }

                //结束音乐按钮和按钮操作
                if (m.x >= 875 && m.x <= 1000 && m.y >= 640 && m.y <= 680)//浏览    outtextxy(885, 500, "开启音乐");
                {
                    setlinecolor(RED);//设置电击时为红色 
                    rectangle(875, 640, 1000, 680);
                    if (m.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
                    {
                        // 停止播放并关闭音乐
                        mciSendString("stop bk", NULL, 0, NULL);
                        mciSendString("close bk", NULL, 0, NULL);
                    }
                }
                else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
                    setlinecolor(WHITE);
                    rectangle(875, 640, 1000, 680);
                }
            }
        }
       
}

//默写记忆
void memorize_word2(int number,struct Node*p)
{

    cleardevice();
    putimage(0, 0, &bg);

    FILE* file;
    if ((file = fopen("word.txt", "r")) == NULL) { //若无此文件
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "读取的单词文件缺失", "错误", MB_OKCANCEL);
        exit(0);
    }

    //绘制背景色块
    setlinecolor(RGB(117, 166, 255));
    setfillcolor(RGB(117, 166, 255));
    fillroundrect(235, 95, 685, 265, 20, 20);


    settextcolor(WHITE);
    settextstyle(30, 0, "楷体");
    outtextxy(875, 595, "开启音乐");
    outtextxy(875, 645, "关闭音乐");

    settextcolor(WHITE);
    settextstyle(30, 0, "微软雅黑");

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
                MessageBox(hwnd1, "太棒了！（^ ^）已达到计划记忆的单词数量，如需要，请按返回按钮退出", "提醒", MB_OKCANCEL);
            }

            settextcolor(WHITE);
            settextstyle(30, 0, "微软雅黑");

            outtextxy(240, 100, "词义：");
            outtextxy(250, 590, "答案");
            outtextxy(300, 100, p->data.meaning);

            
            char meani[100];
            InputBox(meani, 20, "请输入对应词义的单词:");//显示一个输入框，
            if (strcmp(meani, p->data.word) == 0) {

                outtextxy(240, 400, "Congratulations! You have memorized the word .");
            }
            else {

                outtextxy(240, 400, "Sorry!Please try again. ");
            }

            i++;

            //输出页码
            cnt++;
            sprintf(yema, "%d", cnt);
            outtextxy(490, 520, yema);

            MOUSEMSG m;
            while (1) {
                char st1[] = "跳过";
                char st2[] = "再次尝试";
                char st3[] = "下一页";
                char st4[] = "上一页";
                char st5[] = "返回";

                outtextxy(380, 175, st1);
                outtextxy(550, 175, st2);
                outtextxy(550, 520, st3);
                outtextxy(380, 520, st4);
                outtextxy(900, 520, st5);
                
                m = GetMouseMsg();

                //创建跳过按钮和按钮操作
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
                            MessageBox(hwnd1, "太棒了！（^ ^）已达到计划记忆的单词数量，如需要，请按返回按钮退出", "提醒", MB_OKCANCEL);
                        }

                        

                        i++;
                        p = p->next;

                        //绘制背景色块
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        //输出页码
                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);


                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");

                        outtextxy(240, 100, "词义：");
                        outtextxy(250, 590, "答案");
                        outtextxy(300, 100, p->data.meaning);

                        rectangle(250, 580, 300, 620);

                        MOUSEMSG n;
                        n = GetMouseMsg();

                        char meani[100];
                        InputBox(meani, 20, "请输入对应词义的单词:");//显示一个输入框，
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
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(365, 180, 445, 205);
                }


                //创建再次尝试按钮和按钮操作
                if (m.x >= 540 && m.x <= 640 && m.y >= 180 && m.y <= 205) {

                  
                    setlinecolor(RED);
                    rectangle(540, 180, 640, 205);



                    if (m.uMsg == WM_LBUTTONDOWN) {
                        Sleep(200);
                        putimage(0, 0, &bg);

                        i++;

                        //输出页码
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                        //绘制背景色块
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");

                        outtextxy(240, 100, "词义：");
                        outtextxy(250, 590, "答案");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "楷体");
                        outtextxy(875, 595, "开启音乐");
                        outtextxy(875, 645, "关闭音乐");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");
                    
                        outtextxy(300, 100, p->data.meaning);

                        MOUSEMSG n;
                        n = GetMouseMsg();
                        char meani[100];
                        InputBox(meani, 20, "请输入对应词义的单词:");//显示一个输入框，
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
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(540, 180, 640, 205);
                }
                //创建下一页按钮和按钮操作
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
                            MessageBox(hwnd1, "太棒了！（^ ^）已达到计划记忆的单词数量，如需要，请按返回按钮退出", "提醒", MB_OKCANCEL);
                        }

                        sign++;
                        pr[sign] = p;

                        p = p->next;

                        //输出页码
                        cnt++;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                        //绘制背景色块
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");

                        outtextxy(240, 100, "词义：");
                        outtextxy(250, 590, "答案");
                        outtextxy(300, 100, p->data.meaning);

                        settextcolor(WHITE);
                        settextstyle(30, 0, "楷体");
                        outtextxy(875, 595, "开启音乐");
                        outtextxy(875, 645, "关闭音乐");

                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");

                        MOUSEMSG n;
                        n = GetMouseMsg();

                        char meani[100];
                        InputBox(meani, 20, "请输入单词的意思:");//显示一个输入框，
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
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(545, 515, 625, 550);
                }


                //创建上一页按钮和按钮操作
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
                            MessageBox(hwnd1, "已处于首页！", "错误", MB_OKCANCEL);
                        }

                        //输出页码
                        cnt--;
                        sprintf(yema, "%d", cnt);
                        outtextxy(490, 520, yema);

                        number++;

                        //绘制背景色块
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(235, 95, 685, 265, 20, 20);

                        settextcolor(WHITE);
                        settextstyle(30, 0, "微软雅黑");

                        outtextxy(240, 100, "词义：");
                        outtextxy(250, 590, "答案");
                        outtextxy(300, 100, p->data.meaning);

                        MOUSEMSG n;
                        n = GetMouseMsg();

                        char meani[100];
                        InputBox(meani, 20, "请输入单词的意思:");//显示一个输入框，
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
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(365, 180, 445, 205);
                }

                rectangle(855, 515, 990, 550);

                //返回按钮和按钮操作
                if (m.x >= 855 && m.x <= 990 && m.y >= 515 && m.y <= 550) {
                    setlinecolor(RED);
                    rectangle(855, 515, 990, 550);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        Sleep(200);  //防止闪屏
                        putimage(0, 0, &bg);

                        flag = 1;
                        p = p->next;  //以便再次学习单词是紧接上一次的，避免重复学习
                        learn_word(p);//将剩余的未记忆的单词数量转过去
                    }

                }
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(375, 515, 455, 550);
                }

                //创建答案按钮和按钮操作
                if (m.x >= 230 && m.x <= 320 && m.y >= 580 && m.y <= 620) {

                    setlinecolor(RED);
                    rectangle(230, 580, 320, 620);
                    if (m.uMsg == WM_LBUTTONDOWN) {
                        //绘制背景色块
                        setlinecolor(RGB(117, 166, 255));
                        setfillcolor(RGB(117, 166, 255));
                        fillroundrect(335, 575, 550, 625, 20, 20);
                        Sleep(200);
                        outtextxy(350, 590, p->data.word);

                       
                    }
                }
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(230, 580, 320, 620);
                }

                //打开音乐按钮和按钮操作
                if (m.x >= 875 && m.x <= 1000 && m.y >= 590 && m.y <= 630)
                {
                    setlinecolor(RED);//设置电击时为红色 
                    rectangle(875, 590, 1000, 630);
                    if (m.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
                    {
                        // 停止播放并关闭音乐
                        mciSendString("stop bk", NULL, 0, NULL);
                        mciSendString("close bk", NULL, 0, NULL);

                    }
                }
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(875, 590, 1000, 630);
                }

                //关闭音乐按钮和按钮操作
                if (m.x >= 875 && m.x <= 1000 && m.y >= 640 && m.y <= 680)//浏览    outtextxy(885, 500, "开启音乐");
                {
                    setlinecolor(RED);//设置电击时为红色 
                    rectangle(875, 640, 1000, 680);
                    if (m.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
                    {
                        // 打开音乐
                        mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                        // 播放音乐
                        mciSendString("play bk repeat", NULL, 0, NULL);
                    }
                }
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(875, 640, 1000, 680);
                }
            }
        }
    }

}

//表格绘制
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
    outtextxy(10, 10, "序号");
    outtextxy(246, 10, "单词");
    outtextxy(479, 10, "词义");
    settextcolor(BLACK);
}

//对应绘制的表格输出整个链表数据函数（输出单词集）
void printlist1(struct Node* head, int cnt) 
{

    settextstyle(30, 0, ("微软雅黑"));

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

//对应绘制的表格输出整个链表数据函数（输出使用已记忆单词集）
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
        MessageBox(hwnd1, "无记忆单词！", "错误", MB_OKCANCEL);
    }


}

//查看单词集
void  show_all_words()
{
    setfillcolor(WHITE);
    int i = 0;
    int cnt = 1;
    int vis = 0;

    nodoLine();  //表格绘制
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
        settextstyle(30, 0, ("微软雅黑"));

        char st3[] = "下一页";
        char st4[] = "上一页";
        char st5[] = "返回";

        outtextxy(950, 570, st3);
        outtextxy(820, 570, st4);
        outtextxy(920, 520, st5);

        m = GetMouseMsg();
        //创建下一页按钮和按钮操作
        if (m.x >= 940 && m.x <= 1020 && m.y >= 565 && m.y <= 610) {  
            setlinecolor(RED);
            rectangle(940, 565, 1020, 610);

            if (m.uMsg == WM_LBUTTONDOWN) {
                Sleep(200);  //防止闪屏

                vis = 0;
                pag++;
                start = remember[pag];
                Sleep(200);
                cnt += 26;
                printlist1(start, cnt);

            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(940, 565, 1020, 610);
        }

        //创建上一页按钮和按钮操作
        if (m.x >= 820 && m.x <= 900 && m.y >= 565 && m.y <= 610) { 
            setlinecolor(RED);
            rectangle(820, 565, 900, 610);
            if (m.uMsg == WM_LBUTTONDOWN) {
                Sleep(200);  //防止闪屏
               
                pag--;
                if (!vis) cnt -= 26;

                if (pag == 0 || cnt < 0)
                {
                    pag = 1;
                    if (!vis)  cnt += 26;
                    vis = 1;
                    HWND hwnd1 = GetHWnd();
                    MessageBox(hwnd1, "已处于首页！", "错误", MB_OKCANCEL);
                }
                printlist1(remember[pag], cnt);  //打印出这一页的单词信息

            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框
            setlinecolor(WHITE);
            rectangle(820, 565, 900, 610);
        }

        //返回按钮和按钮操作
        if (m.x >= 895 && m.x <= 980 && m.y >= 515 && m.y <= 550) {  
            setlinecolor(RED);
            rectangle(895, 515, 980, 550);
            if (m.uMsg == WM_LBUTTONDOWN) {
                Sleep(200); //防止闪屏
                putimage(0, 0, &bg);
                learn_word(List);
            }

        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框
            setlinecolor(WHITE);
            rectangle(895, 515, 980, 550);
        }
    }
}

// 输出所有已记忆单词集
void print_words() {

    cleardevice();//清空图像
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
        settextstyle(30, 0, ("微软雅黑"));

        char st3[] = "下一页";
        char st4[] = "上一页";
        char st5[] = "返回";

        outtextxy(950, 570, st3);
        outtextxy(830, 570, st4);
        outtextxy(920, 520, st5);

        m = GetMouseMsg();
        //创建下一页按钮
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
                    MessageBox(hwnd1, "下一页已无记忆单词", "错误", MB_OKCANCEL);
                }
                else
                {
                    Sleep(200);
                    cnt += 26;
                    printlist2(start, cnt);
                }

            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(940, 565, 1020, 610);
        }

        //创建上一页按钮
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
                    MessageBox(hwnd1, "已处于首页！", "错误", MB_OKCANCEL);
                }


                printlist2(remember[pag], cnt);

            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(820, 565, 900, 610);
        }

        //返回返回按钮
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
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(895, 515, 980, 550);
        }

    }
}

//选择要记忆单词的数量
int choose_nums()
{
    putimage(0, 0, &bg);
    char num[10];

    InputBox(num, 10, "请先输入要记忆单词的数量:");//弹出一个输入框
    int i = 0;
    int k = 1, sum = 0;
   
    sum = atol(num);   //将字符串转化为long int 类型整数
    printf("\n%d\n", sum);
    return sum;
}

// 记忆单词统计功能
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
        settextstyle(30, 0, "微软雅黑");

        char s1[10] = { '0' };
        char s2[10] = { '0' };
        char s3[10] = { '0' };
        char s4[10] = { '0' };
        char s5[10] = { '0' };
        char s6[] = "返回";
        char s7[] = "已记忆单词集:";
        char s8[] = { '0' };

        printf("错误的：%d\n", incorrect_count);
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

        outtextxy(240, 100, "已记忆单词数：");
        outtextxy(390, 100, s1);
     
        outtextxy(240, 150, "未记忆单词数：");
        outtextxy(390, 150, s2);
        outtextxy(240, 175, "总单词数：");
        outtextxy(390, 175, s3);
        
        outtextxy(240, 250, "最容易的单词:");
        outtextxy(560, 250, easiest_word);
        outtextxy(240, 275, "最难记忆的单词:");
        outtextxy(560, 275, hardest_word);
        outtextxy(920, 520, s6);

        fillrectangle(420, 325, 470, 360);
        
        outtextxy(240, 325, s7);



        while (1) {
            MOUSEMSG m3;
            m3 = GetMouseMsg();

            //已记忆单词集按钮
            if (m3.x >= 420 && m3.x <= 470 && m3.y >= 325 && m3.y <= 360) {
                setlinecolor(RED);
                rectangle(420, 325, 470, 360);
                if (m3.uMsg == WM_LBUTTONDOWN) {

                    putimage(0, 0, &bg);
                    print_words();

                }
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(420, 325, 470, 360);
                }
            }

            //返回按钮
            if (m3.x >= 855 && m3.x <= 990 && m3.y >= 515 && m3.y <= 550) {
                setlinecolor(RED);
                rectangle(855, 515, 990, 550);
                if (m3.uMsg == WM_LBUTTONDOWN) {

                    putimage(0, 0, &bg);
                    learn_word(List);

                }
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(855, 515, 990, 550);
                }
            }
        }
    }
    else
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "暂无记忆数据，请先记忆单词！", "提示", MB_OKCANCEL);
    }

}

//选择学习单词模式界面
void start_learn(int number,struct Node*head)
{
    cleardevice();//清空图像
    loadimage(&bg, ("lake7.png"));
    putimage(0, 0, &bg);
    setfillcolor(WHITE);

    char subheading1[20] = { "直接记忆" };
    char subheading2[20] = { "默写记忆" };
    char subheading3[20] = { "退出" };

    fillrectangle(370, 325, 700, 370);
    fillrectangle(370, 385, 700, 430);
    fillrectangle(370, 445, 700, 490);

    //设置字体 
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(40, 0, "楷体");
    //书写内容
    settextcolor(WHITE);
    outtextxy(420, 260, "LEARN WORDS");
    settextcolor(BLACK);
    settextstyle(30, 0, "楷体");
    outtextxy(460, 330, subheading1);
    outtextxy(460, 390, subheading2);
    outtextxy(480, 450, subheading3);

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();//获取鼠标操作

        //绘制直接记忆按钮及设计按钮操作
        if (m1.x >= 370 && m1.x <= 700 && m1.y >= 325 && m1.y <= 370)
        {
            setlinecolor(RED);//设置点击时为红色 
            rectangle(370, 325, 700, 370);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                memorize_word1(number,head);
            }
            else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                setlinecolor(WHITE);
                rectangle(370, 325, 700, 370);
            }
        }

        //绘制默写记忆按钮及设计按钮操作
        if (m1.x >= 370 && m1.x <= 700 && m1.y >= 385 && m1.y <= 430)
        {
            setlinecolor(RED);
            rectangle(370, 385, 700, 430);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                memorize_word2(number,head);
            }
            else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                setlinecolor(WHITE);
                rectangle(370, 385, 700, 430);
            }
        }


        //绘制返回按钮及设计按钮操作
        if (m1.x >= 370 && m1.x <= 700 && m1.y >= 445 && m1.y <= 490) {
            setlinecolor(RED);
            rectangle(370, 445, 700, 490);
            if (m1.uMsg == WM_LBUTTONDOWN) {

                putimage(0, 0, &bg);
                learn_word(List);
            }
            else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                setlinecolor(WHITE);
                rectangle(370, 445, 700, 490);
            }
        }
    }
}

//复习单词界面
void re_learn()
{
    cleardevice();//清空图像
    loadimage(&bg, ("lake7.png"));
    putimage(0, 0, &bg);
    setfillcolor(WHITE);

    settextcolor(RGB(127, 127, 127));
    settextstyle(30, 0, "微软雅黑");

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

        //输出页码
        sprintf(yema, "%d", cnt);
        outtextxy(490, 520, yema);
        settextcolor(WHITE);

        // 输出当前单词和释义信息
        outtextxy(240, 100, "单词：");
        outtextxy(240, 125, "释义：");
        outtextxy(300, 100, p->data.word);
        outtextxy(300, 125, p->data.meaning);

        settextcolor(WHITE);
        settextstyle(30, 0, "楷体");
        outtextxy(875, 595, "开启音乐");
        outtextxy(875, 645, "关闭音乐");
        
        settextstyle(30, 0, "微软雅黑");

        // 读取用户输入的记忆结果
        fflush(stdin); // 清空输入缓冲区
        outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

        MOUSEMSG m;
        while (1) {
            char st1[] = "Yes";
            char st2[] = "No";
            char st3[] = "下一页";
            char st4[] = "上一页";
            char st5[] = "返回";

            outtextxy(380, 175, st1);
            outtextxy(550, 175, st2);
            outtextxy(550, 520, st3);
            outtextxy(380, 520, st4);
            outtextxy(910, 520, st5);

            m = GetMouseMsg();

            //创建Yes按钮和按钮操作
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

                    //绘制背景色块
                    setlinecolor(RGB(117, 166, 255));
                    setfillcolor(RGB(117, 166, 255));
                    fillroundrect(235, 95, 685, 265, 20, 20);
                    setfillcolor(WHITE);
                    // 输出当前单词和释义信息

                    settextcolor(WHITE);
                    outtextxy(240, 100, "单词：");
                    outtextxy(240, 125, "释义：");
                    outtextxy(300, 100, p->data.word);


                    outtextxy(300, 125, p->data.meaning);

                    // 读取用户输入的记忆结果
                    fflush(stdin); // 清空输入缓冲区
                    outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                    settextcolor(WHITE);
                    settextstyle(30, 0, "楷体");
                    outtextxy(875, 595, "开启音乐");
                    outtextxy(875, 645, "关闭音乐");

                   
                    settextstyle(30, 0, "微软雅黑");

                    cnt++;
                    sprintf(yema, "%d", cnt);
                    outtextxy(490, 520, yema);

                }
            }
            else {	//当鼠标不在目标位置就覆盖之前的红色边框
                setlinecolor(WHITE);
                rectangle(365, 180, 445, 205);
            }

            //创建No按钮和按钮操作
            if (m.x >= 540 && m.x <= 620 && m.y >= 180 && m.y <= 205) {

                i++;

                setlinecolor(RED);
                rectangle(540, 180, 620, 205);

                if (m.uMsg == WM_LBUTTONDOWN) {

                    p->data.incorrect_count++;
                    sign++;
                    pr[sign] = p;

                    Sleep(200);  //防止闪屏
                    putimage(0, 0, &bg);

                    p = p->next;
                    if (p == NULL || p->data.memorized == 0)
                    {
                        f2 = 1;
                        break;
                    }

                    //绘制背景色块
                    setlinecolor(RGB(117, 166, 255));
                    setfillcolor(RGB(117, 166, 255));
                    fillroundrect(235, 95, 685, 265, 20, 20);
                    setfillcolor(WHITE);

                    // 输出当前单词和释义信息
                    settextcolor(WHITE);
                    outtextxy(240, 100, "单词：");
                    outtextxy(240, 125, "释义：");
                    outtextxy(300, 100, p->data.word);
                    outtextxy(300, 125, p->data.meaning);

                    // 读取用户输入的记忆结果
                    fflush(stdin); // 清空输入缓冲区
                
                    outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                    settextcolor(WHITE);
                    settextstyle(30, 0, "楷体");
                    outtextxy(875, 595, "开启音乐");
                    outtextxy(875, 645, "关闭音乐");
                   
                    settextstyle(30, 0, "微软雅黑");

                    cnt++;
                    sprintf(yema, "%d", cnt);
                    outtextxy(490, 520, yema);

                }
            }
            else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                setlinecolor(WHITE);
                rectangle(540, 180, 620, 205);
            }


            //创建下一页按钮和按钮操作
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

                    //绘制背景色块
                    setlinecolor(RGB(117, 166, 255));
                    setfillcolor(RGB(117, 166, 255));
                    fillroundrect(235, 95, 685, 265, 20, 20);
                    setfillcolor(WHITE);
                    // 输出当前单词和释义信息

                    settextcolor(WHITE);
                    outtextxy(240, 100, "单词：");
                    outtextxy(240, 125, "释义：");
                    outtextxy(300, 100, p->data.word);
                    outtextxy(300, 125, p->data.meaning);

                    // 读取用户输入的记忆结果
                    fflush(stdin); // 清空输入缓冲区

                    outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                    settextcolor(WHITE);
                    settextstyle(30, 0, "楷体");
                    outtextxy(875, 595, "开启音乐");
                    outtextxy(875, 645, "关闭音乐");
                    settextstyle(30, 0, "微软雅黑");

                    cnt++;
                    sprintf(yema, "%d", cnt);
                    outtextxy(490, 520, yema);

                }
            }
            else {	//当鼠标不在目标位置就覆盖之前的红色边框
                setlinecolor(WHITE);
                rectangle(545, 515, 625, 550);
            }

            //创建上一页按钮和按钮操作
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
                        MessageBox(hwnd1, "已处于首页！", "错误", MB_OKCANCEL);
                    }
                    setlinecolor(RGB(117, 166, 255));
                    setfillcolor(RGB(117, 166, 255));

                    fillroundrect(235, 95, 685, 265, 20, 20);
                    setfillcolor(WHITE);

                    settextcolor(WHITE);

                    outtextxy(240, 100, "单词：");
                    outtextxy(240, 125, "释义：");
                    outtextxy(300, 100, pr[sign]->data.word);
                    outtextxy(300, 125, pr[sign]->data.meaning);

                    // 读取用户输入的记忆结果
                    fflush(stdin); // 清空输入缓冲区

                    outtextxy(240, 150, "Have you memorized this word? (Yes/No)\n");

                    settextcolor(WHITE);
                    settextstyle(30, 0, "楷体");
                    outtextxy(875, 595, "开启音乐");
                    outtextxy(875, 645, "关闭音乐");

                    settextstyle(30, 0, "微软雅黑");

                    cnt--;
                    sprintf(yema, "%d", cnt);
                    outtextxy(490, 520, yema);
                }
            }
            else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
                setlinecolor(WHITE);
                rectangle(375, 515, 455, 550);
            }

            //开始音乐按钮和按钮操作
            if (m.x >= 875 && m.x <= 1000 && m.y >= 590 && m.y <= 630)
            {
                setlinecolor(RED);//设置电击时为红色 
                rectangle(875, 590, 1000, 630);
                if (m.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
                {
                    // 打开音乐
                    mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                    // 播放音乐
                    mciSendString("play bk repeat", NULL, 0, NULL);
                }
            }
            else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                setlinecolor(WHITE);
                rectangle(875, 590, 1000, 630);
            }

            //结束音乐按钮和按钮操作
            if (m.x >= 875 && m.x <= 1000 && m.y >= 640 && m.y <= 680)//浏览    outtextxy(885, 500, "开启音乐");
            {
                setlinecolor(RED);//设置电击时为红色 
                rectangle(875, 640, 1000, 680);
                if (m.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
                {
                    // 停止播放并关闭音乐
                    mciSendString("stop bk", NULL, 0, NULL);
                    mciSendString("close bk", NULL, 0, NULL);
                }
            }
            else {	//当鼠标不在目标位置就覆盖之前的红色边框
                setlinecolor(WHITE);
                rectangle(875, 640, 1000, 680);
            }
            //返回按钮和按钮操作
            if (m.x >= 865 && m.x <= 990 && m.y >= 515 && m.y <= 550) {
                setlinecolor(RED);
                rectangle(865, 515, 990, 550);
                if (m.uMsg == WM_LBUTTONDOWN) {
                    Sleep(200);
                    putimage(0, 0, &bg);

                    flag = 1;
                    p = p->next;// //以便再次学习单词是紧接上一次的，避免重复学习

                    learn_word(List);  //将剩余的未记忆的单词数量转过去
                    
                }
                else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                    setlinecolor(WHITE);
                    rectangle(865, 515, 990, 550);
                }

            }

        }

    }
    else
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "无已记忆的单词,无法复习！", "提示", MB_OKCANCEL);
    }

    char st6[] = "返回";
    outtextxy(920, 520, st6);
    if (f2)
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "所有单词已复习完成！", "提示", MB_OKCANCEL);
    }

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();
        //返回按钮
        if (m1.x >= 865 && m1.x <= 990 && m1.y >= 515 && m1.y <= 550) {
            setlinecolor(RED);
            rectangle(865, 515, 990, 550);
            if (m1.uMsg == WM_LBUTTONDOWN) {
                Sleep(200);
                putimage(0, 0, &bg);

                flag = 1;
                p = p->next;
                learn_word(List);  //将剩余的未记忆的单词数量转过去
                //返回start_learn界面，而不是learn_word界面，是因为自己写过可以接着背的功能，不能浪费


            }
            else {	//当鼠标不在目标位置就覆盖之前的红色边框.
                setlinecolor(WHITE);
                rectangle(865, 515, 990, 550);
            }
        }
    }
}

//保存单词信息到指定文件
void save_words_to_file(struct Node* head) {

    char savefilename[150] = "savefile.txt";
    FILE* fp = fopen(savefilename, "w");
    if (fp == NULL) {
        HWND hwnd1 = GetHWnd();
        MessageBox(hwnd1, "保存文件失败！无法退出系统！", "错误", MB_OKCANCEL);
    }
    for (struct Node* p = head; p != NULL; p = p->next) {
        fprintf(fp, "%s\t%s\t%d\t%d\n", p->data.word, p->data.meaning, p->data.correct_count, p->data.incorrect_count);
        printf("%s\t%s\t%d\t%d\n", p->data.word, p->data.meaning, p->data.correct_count, p->data.incorrect_count);
    }
    fclose(fp);
    closegraph();
}

//离开系统
void exitsystem()
{
    save_words_to_file(List);
    return;
}

//背单词系统主界面
void learn_word(struct Node*head)
{
    cleardevice();//清空图像

    putimage(0, 0, &bg);
    setfillcolor(WHITE);//设置背景色

    char heading0[20] = { "查看单词集" };
    char heading1[20] = { "开始记忆" };
    char heading2[20] = { "记忆单词数据" };
    char heading3[20] = { "复习单词" };
    char heading4[20] = { "返回" };
    char heading5[20] = { "退出系统" };
   
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
  
    //设置字体 
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(40, 0, "楷体");
    //书写内容
    settextcolor(WHITE);
    outtextxy(390, 200, "LEARN WORDS");
    settextcolor(BLACK);
    settextstyle(30, 0, "楷体");
    outtextxy(400, 270, heading0);
    outtextxy(400, 330, heading1);
    outtextxy(400, 390, heading2);
    outtextxy(400, 450, heading3);
    outtextxy(440, 510, heading4);
    outtextxy(400, 570, heading5);

    settextcolor(WHITE);
    settextstyle(30, 0, "楷体");
    outtextxy(875, 595, "关闭音乐");
    outtextxy(875, 500, "开启音乐");

    // 鼠标操作1

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();//获取鼠标操作
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 265 && m1.y <= 310)//浏览 f (m1.x >= 330 && m1.x <= 650 && m1.y >= 325 && m1.y <= 370)//浏览 
        {
            setlinecolor(RED);//设置点击时为红色 
            rectangle(330, 265, 650, 310);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                Sleep(200);
                cleardevice();//清空图像

                show_all_words();
            }

        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(330, 265, 650, 310);
        }
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 325 && m1.y <= 370)//浏览 
        {
            setlinecolor(RED);//设置点击时为红色 
            rectangle(330, 325, 650, 370);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                Sleep(200);
                cleardevice();//清空图像

                amount = choose_nums();
                start_learn(amount,head);
            }

        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(330, 325, 650, 370);
        }
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 385 && m1.y <= 430)//普通用户界面
        {
            setlinecolor(RED);
            rectangle(330, 385, 650, 430);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                Sleep(200);
                cleardevice();//清空图像

                view_statistics();
            }

        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
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
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
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
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
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
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(330, 565, 650, 610);
        }
        if (m1.x >= 880 && m1.x <= 1000 && m1.y >= 590 && m1.y <= 640)
        {
            setlinecolor(RED);//设置电击时为红色 
            rectangle(880, 590, 1000, 640);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                // 停止播放并关闭音乐
                mciSendString("stop bk", NULL, 0, NULL);
                mciSendString("close bk", NULL, 0, NULL);

            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(880, 590, 1000, 640);
        }

        if (m1.x >= 880 && m1.x <= 1000 && m1.y >= 495 && m1.y <= 545)
        {
            setlinecolor(RED);//设置电击时为红色 
            rectangle(880, 495, 1000, 545);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                // 打开音乐
                mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                // 播放音乐
                mciSendString("play bk repeat", NULL, 0, NULL);
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(880, 495, 1000, 545);
        }
    }
}

//创建随机计划
void random_plan() {
    cleardevice();
    putimage(0, 0, &bg);

    struct Node* h = create_word_list();
    List = create_random_plan(h);

    printf("here");
    if (List != NULL)
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "创建随机记忆计划成功", "提示", MB_OKCANCEL);

    }
    learn_word(List);
}

//创建顺序计划
void order_plan()
{
    cleardevice();
    putimage(0, 0, &bg);
    List = create_word_list();
    if (List != NULL)
    {
        HWND hwnd = GetHWnd();
        MessageBox(hwnd, "创建顺序记忆计划成功", "提示", MB_OKCANCEL);
    }
    learn_word(List);

}

//创建记忆单词计划
void createplan()
{
    char subheading0[20] = { "请先确定记忆计划" };
    char subheading1[20] = { "随机记忆计划" };
    char subheading2[20] = { "顺序记忆计划" };
    char subheading3[20] = { "返回" };

    // 可重新选择背景图片
    loadimage(&bg, ("lake7.png"));
    putimage(0, 0, &bg);

    //设置矩形框 
    setlinecolor(RGB(117, 166, 255));
    setfillcolor(RGB(117, 166, 255));

    fillroundrect(320, 240, 660, 500, 20, 20);
    setfillcolor(RGB(255, 255, 255));
    fillrectangle(330, 325, 650, 370);
    fillrectangle(330, 385, 650, 430);
    fillrectangle(330, 445, 650, 490);

    //设置字体 
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(35, 0, "楷体");
    //书写内容

    outtextxy(340, 250, subheading0);
    settextstyle(30, 0, "楷体");
    outtextxy(400, 330, subheading1);
    outtextxy(400, 390, subheading2);
    outtextxy(450, 450, subheading3);
    settextcolor(WHITE);
    settextstyle(30, 0, "楷体");
    outtextxy(875, 595, "关闭音乐");
    outtextxy(875, 500, "开启音乐");

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();//获取鼠标操作 

        //绘制随机记忆计划按钮和按钮操作
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 325 && m1.y <= 370)//浏览     
        {
            setlinecolor(RED);//设置点击时为红色 
            rectangle(330, 325, 650, 370);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                random_plan();
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(330, 325, 650, 370);
        }

        //绘制顺序记忆计划按钮和按钮操作
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 385 && m1.y <= 430)//普通用户界面
        {
            setlinecolor(RED);
            rectangle(330, 385, 650, 430);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                order_plan();
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框
            setlinecolor(WHITE);
            rectangle(330, 385, 650, 430);
        }

        //绘制返回按钮和按钮操作
        if (m1.x >= 330 && m1.x <= 650 && m1.y >= 445 && m1.y <= 490)
        {
            setlinecolor(RED);
            rectangle(330, 445, 650, 490);
            if (m1.uMsg == WM_LBUTTONDOWN)
            {
                welcome();
            }

        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(330, 445, 650, 490);
        }

        //绘制关闭音乐按钮和按钮操作
        if (m1.x >= 875 && m1.x <= 1000 && m1.y >= 590 && m1.y <= 630)
        {
            setlinecolor(RED);//设置电击时为红色 
            rectangle(875, 590, 1000, 630);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                // 停止播放并关闭音乐
                mciSendString("stop bk", NULL, 0, NULL);
                mciSendString("close bk", NULL, 0, NULL);

            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的蓝色边框.
            setlinecolor(WHITE);
            rectangle(875, 590, 1000, 630);
        }

        //绘制开始音乐按钮和按钮操作
        if (m1.x >= 875 && m1.x <= 1000 && m1.y >= 495 && m1.y <= 545)//浏览    outtextxy(885, 500, "开启音乐");
        {
            setlinecolor(RED);//设置电击时为红色 
            rectangle(875, 495, 1000, 545);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                // 打开音乐
                mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                // 播放音乐
                mciSendString("play bk repeat", NULL, 0, NULL);
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框
            setlinecolor(WHITE);
            rectangle(875, 495, 1000, 545);
        }

    }
    system("pause");
    closegraph();
}

// 进入欢迎界面
void welcome()
{

    // 绘制背景图片
    loadimage(&bg, ("lake7.png"));
    putimage(0, 0, &bg);

     // 绘制矩形按钮
    int button_x = 460, button_y = 450;
    setlinecolor(RGB(255, 255, 255));  // 设置线条颜色为背景色
    setfillcolor(RGB(255, 255, 255));
    fillrectangle(button_x, button_y, button_x + 100, button_y + 50);
   

    // 绘制标题

    settextcolor(WHITE);// 字体颜色为白色
    setbkmode(TRANSPARENT); // 设置背景透明
    settextstyle(40, 0, ("微软雅黑"));
    outtextxy(380, 100, ("欢迎来到背单词系统！"));
    settextstyle(30, 0, ("微软雅黑"));
    outtextxy(885, 595, "关闭音乐");
    outtextxy(885, 500, "开启音乐");

    // 绘制提示信息
    settextstyle(30, 0, "微软雅黑");
    outtextxy(395, 300, "请点击下方白色按钮开始学习！");

    outtextxy(282, 197, "当前时间：");
    time_t clock;
    //获取当前时间
    time(&clock);
    outtextxy(428, 197, ctime(&clock));
    line(100, HEIGHT / 3, 900, HEIGHT / 3);

    MOUSEMSG m1;
    while (1)
    {
        m1 = GetMouseMsg();//获取鼠标操作

        //绘制进入选择计划按钮和按钮操作
        if (m1.x >= button_x && m1.x <= button_x + 100 && m1.y >= button_y && m1.y <= button_y + 50)//浏览 
        {
            setlinecolor(RED);//设置电击时为红色 
            rectangle(button_x, button_y, button_x + 100, button_y + 50);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                createplan();
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框
            setlinecolor(WHITE);
            rectangle(button_x, button_y, button_x + 100, button_y + 50);
        }

        //绘制关闭音乐按钮和按钮操作
        if (m1.x >= 880 && m1.x <= 980 && m1.y >= 590 && m1.y <= 640)//浏览    //outtextxy(885, 595, "关闭音乐");
        {
            setlinecolor(RED);//设置电击时为红色 
            rectangle(880, 590, 980, 640);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                // 停止播放并关闭音乐
                mciSendString("stop bk", NULL, 0, NULL);
                mciSendString("close bk", NULL, 0, NULL);

            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框
            setlinecolor(WHITE);
            rectangle(880, 590, 980, 640);
        }

        //绘制开始音乐按钮和按钮操作
        if (m1.x >= 880 && m1.x <= 980 && m1.y >= 495 && m1.y <= 545)//浏览    outtextxy(885, 500, "开启音乐");
        {
            setlinecolor(RED);//设置电击时为红色 
            rectangle(880, 495, 980, 545);
            if (m1.uMsg == WM_LBUTTONDOWN) //左键单击一次触发 
            {
                // 打开音乐
                mciSendString("open music1.mp3 alias bk", NULL, 0, NULL);
                // 播放音乐
                mciSendString("play bk repeat", NULL, 0, NULL);
            }
        }
        else {	//当鼠标不在目标位置就覆盖之前的红色边框.
            setlinecolor(WHITE);
            rectangle(880, 495, 980, 545);
        }

    }
    Sleep(80);
    // 清空窗口
    cleardevice();
}

//主函数
int main()
{
    // 初始化图形界面
    initgraph(1080, 720);

    welcome();  //进入欢迎界面

    return 0;
}