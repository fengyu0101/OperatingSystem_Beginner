﻿#include<iostream>
#include<fstream>
#include<vector>
using namespace std;
#define USERNUM 10
#define MAX_OPEN 5

typedef struct file//文件
{
    string file_name;
    bool file_protect[3];
    bool open_file_protect[3]; //仅在文件打开时有效
    int  read, write; //定义为读写指针,不懂
    int  file_length;
    struct file* next;
    file() {
        file_name = "";
        file_protect[0] = 0;
        file_protect[1] = 0;
        file_protect[2] = 0;
        open_file_protect[0] = 0;
        open_file_protect[1] = 0;
        open_file_protect[2] = 0;
        read = write = 0;
        file_length = 0;
        next = NULL;
    }
} File;

typedef struct folder {//文件夹
    string folder_name;
    struct folder* folder_child;
    struct folder* next;
    File* file_head;
    folder() {
        folder_name = "";
        file_head = new File();
        folder_child = (struct folder*)malloc(sizeof(struct folder));
        if(folder_child)folder_child->next = NULL;
        next = NULL;
    }
}Folder;

typedef struct afd {//文件打开时放入afd下
    int length;
    File* file_head;
    afd() {
        length = 0;
        file_head = new File();
    }
}AFD;

typedef struct mfd {//一个用户一个mfd
    string user_name;
    Folder* folder_root;
    AFD afd;
    mfd() {
        user_name = "";
        folder_root = new Folder();
        afd = AFD();
    }
}MFD[USERNUM];

//初始化用户
void init(MFD mfd) {
    fstream file;
    file.open("user_name.txt");
    for (int i = 0; i < USERNUM; i++) {
        mfd[i] = struct mfd();
        mfd[i].folder_root->folder_name = "root";
        mfd[i].folder_root->next=NULL;
        file >> mfd[i].user_name;
    }
    file.close();
}

//从MFD中找到一个用户名并返回索引，未找到则让用户重新输入
int findUserName(string user_name, MFD mfd) {
    bool is_find = false;
    for (int i = 0; i < USERNUM; i++) {
        if (mfd[i].user_name == user_name) {
            is_find = true;
            return i;
        }
    }
    if (!is_find) {
        string user;
        cout << "未找到用户，请重新输入：";
        cin >> user;
        return findUserName(user, mfd);
    }
}

//将文件路径按照'/'拆分成多个字符串
vector<string> splitPath(const string& path) {
    vector<string> names;
    string p;
    for (char q : path) {
        if (q == '/') {
            names.push_back(p);
            p.clear();
            continue;
        }
        p.push_back(q);
    }
    return names;
}

 //将文件路径转换成文件夹指针
Folder* pathPointer(struct mfd mfdsg, const string& path) {
    vector<string> names = splitPath(path);
    Folder* q = mfdsg.folder_root;
    for (int i = 1; i < names.size(); i++) {
        for (Folder* p = q->folder_child; p->next != NULL; p = p->next) {
            if (p->next->folder_name == names[i]) q = p->next;
        }
    }
    return q;
}

void createFile(struct mfd mfdsg,const string& cur_path) {
    Folder* path = pathPointer(mfdsg, cur_path);
    File* file = new File();
    cout << mfdsg.user_name << ">>" << "新建文件名：";
    cin >> file->file_name;
    for (File* p = path->file_head; p->next != NULL; p = p->next) {
        if (p->next->file_name == file->file_name) {
            cout << endl << "!!!文件重名，请重命!!!" << endl << endl;
            return;
        }
    }
    cout << mfdsg.user_name << ">>" << "新建文件保护码(示例:1 0 1)：";
    cin >> file->file_protect[0] >> file->file_protect[1] >> file->file_protect[2];
    cout << mfdsg.user_name << ">>" << "新建文件长度：";
    cin >> file->file_length;
    file->read = file->write = 0;
    file->next = NULL;

    File* p = path->file_head;
    for (; p->next != NULL; p = p->next);
    file->next = p->next;
    p->next = file;

}
void deleteFile(struct mfd mfdsg,const string& cur_path) {
    Folder* path = pathPointer(mfdsg, cur_path);
    string file_name;
    cout << mfdsg.user_name << ">>" << "请输入欲删除的文件名：";
    cin >> file_name;
    for (File* p = path->file_head; p->next != NULL; p = p->next) {
        if (p->next->file_name == file_name) {
            File* temp = p->next;
            p->next = temp->next;
            delete(temp);
            return;
        }
    }
    cout << endl << "!!!未找到文件!!!" << endl << endl;
}
void openFile(struct mfd mfdsg, const string& cur_path) {
    Folder* path = pathPointer(mfdsg, cur_path);
    if (mfdsg.afd.length >= MAX_OPEN) {
        cout << endl << "!!!打开文件数已达上限!!!" << endl << endl;
        return;
    };
    File* file = new File();
    string file_name;
    bool open_protect[3];
    cout << mfdsg.user_name << ">>" << "请输入欲打开的文件名：";
    cin >> file_name;
    for (File* p = path->file_head->next; p != NULL; p = p->next) {
        if (p->file_name == file_name) {
            file = p; break;
        }
    }
    if (file == NULL) {
        cout << endl << "!!!未找到文件!!!" << endl << endl; return;
    }
    for (File* p = mfdsg.afd.file_head; p != NULL; p = p->next) {
        if (p->file_name == file_name) {
            cout <<endl<< "!!!文件已打开!!!" << endl << endl;
            return;
        }
    }
    cout << mfdsg.user_name << ">>" << "请输入保护码(示例:1 0 1)：";
    cin >> open_protect[0] >> open_protect[1] >> open_protect[2];

    File* xfile = new File();
    *xfile = *file;
    if (xfile->file_protect[0] == 0 || open_protect[0] == 0) {
        cout << endl << "!!!没有读取权限!!!" << endl << endl; return;
    }
    else {
        xfile->open_file_protect[0] = 1;
    }
    if (xfile->file_protect[1] < open_protect[1])cout << endl << "!!!没有写入权限!!!" << endl << endl;
    if (xfile->file_protect[2] < open_protect[2])cout << endl << "!!!没有执行权限!!!" << endl << endl;
    xfile->open_file_protect[1] = min(xfile->file_protect[1], open_protect[1]);
    xfile->open_file_protect[2] = min(xfile->file_protect[2], open_protect[2]);

    File* p = mfdsg.afd.file_head;
    for (; p->next != NULL; p = p->next);
    xfile->next = p->next;
    p->next = xfile;
}
void closeFile(struct mfd mfdsg) {
    string file_name;
    cout << mfdsg.user_name << ">>" << "请输入欲关闭的文件名：";
    cin >> file_name;
    for (File* p = mfdsg.afd.file_head; p->next != NULL; p = p->next) {
        if (p->next->file_name == file_name) {
            p->next = p->next->next;
            free(p->next);
            return;
        }
    }
    cout << endl << "!!!未在打开文件中找到文件!!!" << endl << endl;
}
void readFIle(struct mfd mfdsg) {
    string file_name;
    cout << mfdsg.user_name << ">>" << "请输入欲读取的文件名：";
    cin >> file_name;
    for (File* p = mfdsg.afd.file_head; p->next != NULL; p = p->next) {
        if (p->next->file_name == file_name) {
            if (p->next->open_file_protect[0]) {
                p->next->read++;
                return;
            }
            else {
                cout << endl << "!!!无权限!!!" << endl << endl;
                return;
            }
        }
    }
    cout << endl << "!!!未在打开文件中找到文件!!!" << endl << endl;
}
void writeFile(struct mfd mfdsg) {
    string file_name;
    cout << mfdsg.user_name << ">>" << "请输入欲写入的文件名：";
    cin >> file_name;
    for (File* p = mfdsg.afd.file_head; p->next != NULL; p = p->next) {
        if (p->next->file_name == file_name) {
            if (p->next->open_file_protect[1]) {
                p->next->write++;
                return;
            }
            else {
                cout << endl << "!!!无权限!!!" << endl << endl;
                return;
            }
        }
    }
    cout << endl << "!!!未在打开文件中找到文件!!!" << endl << endl;
}

void reviseFileName(struct mfd mfdsg,const string& cur_path) {
    Folder* path = pathPointer(mfdsg, cur_path);
    string file_name;
    cout << mfdsg.user_name << ">>" << "请输入欲修改的文件名：";
    cin >> file_name;
    for (File* p = mfdsg.afd.file_head; p->next != NULL; p = p->next) {
        if (p->next->file_name == file_name) {
            cout << endl << "!!!请先关闭文件!!!" << endl << endl;
            return;
        }
    }
    for (File* p = path->file_head; p->next != NULL; p = p->next) {
        if (p->next->file_name == file_name) {
            cout << mfdsg.user_name << ">>" << "请输入新的文件名：";
            cin >> p->next->file_name;
            return;
        }
    }
    cout << endl << "!!!未找到文件!!!" << endl << endl;
}
void reviseFilePermission(struct mfd mfdsg, const string& cur_path) {
    Folder* path = pathPointer(mfdsg, cur_path);
    string file_name;
    cout << mfdsg.user_name << ">>" << "请输入欲修改的文件名：";
    cin >> file_name;
    for (File* p = mfdsg.afd.file_head; p->next != NULL; p = p->next) {
        if (p->next->file_name == file_name) {
            cout << endl << "!!!请先关闭文件!!!" << endl << endl;
            return;
        }
    }
    for (File* p = path->file_head; p->next != NULL; p = p->next) {
        if (p->next->file_name == file_name) {
            cout << mfdsg.user_name << ">>" << "请输入新的保护码(示例:1 0 1)：";
            cin >> p->next->file_protect[0] >> p->next->file_protect[1] >> p->next->file_protect[2];
            return;
        }
    }
    cout << endl << "!!!未找到文件!!!" << endl << endl;
}

void createFolder(struct mfd mfdsg, const string& cur_path) {
    Folder* path = pathPointer(mfdsg, cur_path);
    Folder* folder=new Folder();
    cout << mfdsg.user_name << ">>" << "请输入欲创建的文件夹名：";
    cin >> folder->folder_name;
    for (Folder* p = path->folder_child; p->next != NULL; p = p->next) {
        if (p->next->folder_name == folder->folder_name) {
            cout << endl << "!!!文件夹重命!!!" << endl << endl;
            return;
        }
    }
    Folder* p = path->folder_child;
    for (; p->next != NULL; p = p->next);
    folder->next = p->next;
    p->next= folder;
}

void openFolder(struct mfd mfdsg, string& cur_path) {
    Folder* path = pathPointer(mfdsg, cur_path);
    string folder_name;
    cout << mfdsg.user_name << ">>" << "请输入欲打开的文件夹名：";
    cin >> folder_name;
    for (Folder* p = path->folder_child; p->next != NULL; p = p->next) {
        if (p->next->folder_name == folder_name) {
            cur_path =cur_path+ p->next->folder_name+"/";
            return;
        }
    }
    cout << endl << "!!!未找到文件夹!!!" << endl << endl;
}

void deleteFolder(struct mfd mfdsg, const string& cur_path) {
    Folder* path = pathPointer(mfdsg, cur_path);
    string folder_name;
    cout << mfdsg.user_name << ">>" << "请输入欲删除的文件夹名：";
    cin >> folder_name;
    for (Folder* p = path->folder_child; p->next != NULL; p = p->next) {
        if (p->next->folder_name == folder_name) {
            Folder* temp = p->next;
            p->next = temp->next;
            delete(temp);
            return;
        }
    }
    cout << endl << "!!!未找到文件夹!!!" << endl << endl;
}

void backLevel(struct mfd mfdsg, string& cur_path) {
    vector<string> names = splitPath(cur_path);
    string temp="";
    if (names.size() > 1) {
        names.pop_back();
        for (string p : names)temp = temp + p + "/";
        cur_path = temp;
        return;
    }
    cout << endl << "!!!无上级目录!!!" << endl << endl;
}

void Display(struct mfd mfdsg,const string& cur_path) {
    Folder* path = pathPointer(mfdsg, cur_path);
    cout << "-------------------" << cur_path << "文件夹------------------ " << endl;
    for (Folder* p = path->folder_child; p->next != NULL; p = p->next) {
        cout << "-" << p->next->folder_name << endl;
    }
    cout << "-------------------" << cur_path<<"文件-------------------- " << endl;      
    for (File* p = path->file_head; p->next != NULL; p = p->next) {
        cout << "-" << p->next->file_name << endl;
    } 
    cout << "-------------------" << mfdsg.user_name<<"打开文件------------------- " << endl;
    for (File* p = mfdsg.afd.file_head; p->next != NULL; p = p->next) {
        cout << "-" << p->next->file_name << endl;
    }
    cout << "------------------------------------------------" << endl;
}

//执行操作
void executeOperation(struct mfd mfdsg,string& cur_path) {
    system("cls");
    int command;
    cout << "****************** 请选择操作执行**********************" << endl;
    cout << "*    (1)创建文件      (2)删除文件      (3)打开文件    *" << endl;
    cout << "*    (4)关闭文件      (5)读取文件      (6)写入文件    *" << endl;
    cout << "*    (7)修改文件名    (8)修改文件权限  (9)创建文件夹  *" << endl;
    cout << "*    (10)打开文件夹   (11)删除文件夹   (12)返回上一级 *" << endl;
    cout << "*    (0)退出程序                                      *" << endl;
    cout << "*******************************************************" << endl;
    Display(mfdsg, cur_path);
    cout << mfdsg.user_name << ">>";
    cin >> command;

    switch (command) {
    case 1:createFile(mfdsg,cur_path);  break;
    case 2:deleteFile(mfdsg, cur_path); break;
    case 3:openFile(mfdsg, cur_path);  break;
    case 4:closeFile(mfdsg);  break;
    case 5:readFIle(mfdsg); break;
    case 6:writeFile(mfdsg); break;
    case 7:reviseFileName(mfdsg, cur_path); break;
    case 8:reviseFilePermission(mfdsg, cur_path); break;
    case 9:createFolder(mfdsg, cur_path); break;
    case 10:openFolder(mfdsg, cur_path); break;
    case 11:deleteFolder(mfdsg, cur_path); break;
    case 12:backLevel(mfdsg, cur_path); break;
    case 0:cout << "程序退出"; exit(0); break;
    default:cout << "!!!输入错误，请重新输入!!!" << endl;
        executeOperation(mfdsg,cur_path); return; break;
    }
    system("pause");
    executeOperation(mfdsg,cur_path);
}

int main() {
    MFD mfd;
    init(mfd);
    string user_name;
    cout << "****************************************" << endl;
    cout << "*      01    02    03    04    05      *" << endl;
    cout << "*      06    07    08    09    00      *" << endl;
    cout << "****************************************" << endl;
    cout << "请输入用户名进行登录：";
    cin >> user_name;
    string cur_path = "root/";
    int index = findUserName(user_name, mfd);
    executeOperation(mfd[index], cur_path);
    return 0;
}