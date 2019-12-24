#include <time.h>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdio>
using namespace std;


const double P=3.0;//平均每个桶装的元素个数的上限 ,实测貌似1.0效果比较好
int E;//目前使用了哈希值的前 E 位来分组
int R;//实际装入本哈希表的元素总数
int N;//目前使用的桶的个数
/*
操作过程中，始终维护两个性质
1. R/N <= P          可以推出  max(N) = max(R/P) = maxn/P   所以，所需链表的个数为 maxn/P
2. 2^(E-1) <=  N  < 2^E
*/
int p2[35];//记录2的各个次方  p2[i]=2^i
int mask[35]; //记录掩码 mask[i]=p2[i]-1
bool ERROR;//错误信息


string tohex(const string& str)
{
    string ret;
    static const char *hex="0123456789ABCDEF";
    for(int i=0;i!=str.size();++i)
    {
        ret.push_back(hex[(str[i]>>4)&0xf]);
        ret.push_back( hex[str[i]&0xf]);
    }
    return ret;
}

int getnum(string str)
{
    int ret = 0;
    int tempret = 0;
    for(int i=0;i<str.size();i++)
    {

        if(str[i] >= 'A' && str[i] <= 'Z')
            tempret = str[i]-'A' + 10;
        else
            tempret = str[i] -'0';

        if(i % 2 == 0)
            ret += tempret * 16;
        else
            ret += tempret;
    }
    return ret;
}

vector<string> split(string str,string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str+=pattern;//扩展字符串以方便操作
    int size=str.size();

    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            std::string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}


int make_hash(int x){//32位哈希函数
    return x*2654435769;
}
bool hashEq(int x,int y){//判断x与y在当前条件下属不属于一个桶
    return (x&mask[E])==(y&mask[E]);
}
//
int currentHash(int Hash){//当前哈希值
    Hash=Hash&mask[E];
    return Hash < N ? Hash : Hash&mask[E-1];
}

struct ListNode{//链表节点定义
    int Hash;//32位哈希值，根据Key计算，通常为 hash(Key)
    int Key;//键值，唯一
    string Value;//键值Key对应的值
    ListNode *next;//指向链表中的下一节点，或者为空
    bool flag=false;//是否被查询过

   
    //构造函数
    ListNode(){}
    ListNode(int H,int K,string V):Hash(H),Key(K),Value(V){}
};
struct List{//链表定义
    ListNode *Head;//头指针

    //构造函数 析构函数
    List():Head(NULL){}
    ~List(){clear();}

    //插入函数
    void Insert(int H,int K,string V){
        Insert(new ListNode(H,K,V));
    }
    void Insert(ListNode *temp){
        temp->next=Head;
        Head=temp;
    }

    //转移函数
    void Transfer(int H,List *T){//将本链表中，Hash值掩码之后为H的元素加入到链表T中去。
        ListNode *temp,*p;
        while(Head && hashEq(Head->Hash,H)){
            temp=Head;
            Head=Head->next;
            T->Insert(temp);
        }
        p=Head;
        while(p&&p->next){
            if(hashEq(p->next->Hash,H)){
                temp=p->next;
                p->next=p->next->next;
                T->Insert(temp);
            }
            else p=p->next;
        }
    }

    //寻找函数
    string Find(int Key){
        ERROR=false;
        ListNode *temp=Head;
        if(!temp)
        {
            return "sorry";
        }
        vector<string> value = split(temp->Value,", ");
        while(temp){
            if(temp->Key==Key&&temp->flag == false) {
                temp->flag = true;
                return temp->Value;
            }
            
            temp=temp->next;
        }
        return "sorry";
    }

    void Show(){
        ListNode *temp=Head;
        //cout <<"当前桶："<<endl;
        //cout << temp->Value<<endl;
        while(temp){
            
            cout <<temp->Value<<endl;
            //vector<string> result = split(temp->Value,",");
            temp=temp->next;
        }
    }

    //释放申请空间
    void clear(){
        while(Head){
            ListNode *temp=Head;
            Head=Head->next;
            delete temp;
        }
    }
}L[100000005];

//初始化
void Init(){
    p2[0]=1;
    for(int i=1;i<=32;++i) p2[i]=p2[i-1]<<1;
    for(int i=0;i<=32;++i) mask[i]=p2[i]-1;
    E=1;N=1;R=0;L[0]=List();
}

//调整
void Adjust(){
    while((double)R/N > P){
        //将属于N的信息加入List[N]
        L[N&mask[E-1]].Transfer(N,&L[N]);
        //更正 N 和 E
        if(++N >= p2[E])  ++E;
        L[N]=List();
    }
}

//插入
void Insert(int Hash,int Key,string Value){
    //插入元素
    L[currentHash(Hash)].Insert(Hash,Key,Value);
    ++R;
    //调整 N 和 E
    Adjust();
}

//寻找
string Find(int Hash,int Key){
    return L[currentHash(Hash)].Find(Key);
}
//释放所有
void FreeAll(){
    for(int i=0;i<N;++i) L[i].clear();
}
//显示
void ShowList(){
    //OUT3(E,R,N);
    for(int i=0;i<N;++i){
        //printf("%d:",i);
        cout<<"桶"<< i <<":"<<endl;
        L[i].Show();

       // printf("\n");
    }
}


int main(){

    Init();
    //Insert(hash(key),);
    cout<<"查询1:查询借书证号是66的用户所有借书记录"<<endl;
    //C++读取文件
    ifstream infile;
    infile.open("borrows.txt");
    if(!infile) cout<<"error"<<endl;
    vector<string> ve;
    string line;//保存读入的每一行
    while(getline(infile,line))             //按空格读取，遇到空白符结束
    {
        ve.push_back(line);
    }
    //cout << "共读入的记录条数:" << ve.size() << endl;
    //按照借书证号整
    cout<<"建立索引"<<endl;
    clock_t start, endt;
    start = clock();
    for(int i=0;i<ve.size();i++) {
        // 为文件建立索引
        string this_line  = ve[i];
        //cout << "this line:"<< this_line<<endl;
        vector<string> kkkey = split(ve[i],", ");
        int key = atoi(kkkey[1].c_str());   
        int w  = make_hash(key);
        Insert(w,key,this_line);
    }
    endt = clock();
    double use_time = (double)(endt - start) / CLOCKS_PER_SEC;
    

    clock_t first,last;
    cout << "--------------------------"<<endl;
    //ShowList();
    cout<<"借书证号是66的用户所有借书记录:"<<endl;
    first = clock();
    int query = make_hash(66);
    string a;
    do
    {
        a = Find(query,66);
        if (a!="sorry")
        {
            cout << a <<endl;
        }
    } while (a!="sorry");
    last = clock();
    double custom_time = (double)(last - first) /CLOCKS_PER_SEC;
    cout << "--------------------------"<<endl;
    cout << "建立索引用的时间"<<use_time<<"秒"<<endl;
    cout<<"查询用时"<<custom_time<<"秒"<<endl;
    ve.clear();
    infile.close();
    FreeAll();


    cout<<endl<<"查询2:查询出版社编号是77的所有图书"<<endl;
    //C++读取文件
    infile.open("books.txt");
    if(!infile) cout<<"error"<<endl;
    while(getline(infile,line))             //按空格读取，遇到空白符结束
    {
        ve.push_back(line);
    }
    //cout << "共读入的记录条数:" << ve.size() << endl;
    //按照出版社整
    cout<<"建立索引"<<endl;
    start = clock();
    for(int i=0;i<ve.size();i++) {
        // 为文件建立索引
        string this_line  = ve[i];
        //cout << "this line:"<< this_line<<endl;
        vector<string> kkkey = split(ve[i],", ");
        int key = atoi(kkkey[5].c_str());   
        int w  = make_hash(key);
        Insert(w,key,this_line);
    }
    endt = clock();
    use_time = (double)(endt - start) / CLOCKS_PER_SEC;
    

    //ShowList();
    cout << "--------------------------"<<endl;
    cout<<"出版社编号是77的所有图书:"<<endl;
    first = clock();
    query = make_hash(77);
    do
    {
        a = Find(query,77);
        if (a!="sorry")
        {
            cout << a <<endl;
        }
    } while (a!="sorry");
    last = clock();
    custom_time = (double)(last - first) /CLOCKS_PER_SEC;
    cout << "--------------------------"<<endl;
    cout << "建立索引用的时间"<<use_time<<"秒"<<endl;
    cout<<"查询用时"<<custom_time<<"秒"<<endl;
    ve.clear();
    FreeAll();
    infile.close();


    cout<<endl<<"查询3:查询中国文学类别的图书共有多少本"<<endl;
    //C++读取文件
    infile.open("books.txt");
    if(!infile) cout<<"error"<<endl;
    while(getline(infile,line))             //按空格读取，遇到空白符结束
    {
        ve.push_back(line);
    }
    //为了减少tohex和getnum的时间  先把所有类型放在map
    string typelist[6]={"中国文学", "外国文学", "计算机", "英语", "工具书", "儿童读物"};
    map<string,int> type_int;
    for(int i = 0;i<6;i++)
    {
        type_int[typelist[i]] = getnum(tohex(typelist[i]));
    }
    //按照出版社整
    cout<<"建立索引"<<endl;
    start = clock();
    for(int i=0;i<ve.size();i++) {
        // 为文件建立索引
        string this_line  = ve[i];
        //cout << "this line:"<< this_line<<endl;
        vector<string> kkkey = split(ve[i],", ");

        int key = type_int[kkkey[4]]; 
        int w  = make_hash(key);
        Insert(w,key,this_line);
    }
    endt = clock();
    use_time = (double)(endt - start) / CLOCKS_PER_SEC;
    

    //ShowList();
    cout << "--------------------------"<<endl;
    cout<<"中国文学类别的图书:"<<endl;
    int total=0;
    first = clock();
    query = make_hash(type_int["中国文学"]);
    do
    {
        a = Find(query,type_int["中国文学"]);
        if (a!="sorry")
        {
            cout << a <<endl;
            total++;
        }
    } while (a!="sorry");
    last = clock();
    custom_time = (double)(last - first) /CLOCKS_PER_SEC;
    cout << "共"<<total <<"本"<<endl;
    cout << "--------------------------"<<endl;
    cout << "建立索引用的时间"<<use_time<<"秒"<<endl;
    cout<<"查询用时"<<custom_time<<"秒"<<endl;
    ve.clear();
    FreeAll();
    infile.close();



    cout<<endl<<"查询4:查询在2019/06/15哪本书被借出最多"<<endl;
    //C++读取文件
    infile.open("borrows.txt");
    if(!infile) cout<<"error"<<endl;
    while(getline(infile,line))             //按空格读取，遇到空白符结束
    {
        ve.push_back(line);
    }
    //先把时间变成int  2019*365+m*30+days
    //查询在这天借的书
    cout<< "建立索引"<<endl;
    start = clock();
    for(int i=0;i<ve.size();i++) {
        // 为文件建立索引
        string this_line  = ve[i];
        //cout << "this line:"<< this_line<<endl;
        vector<string> kkkey = split(ve[i],", ");
        vector<string> days = split(kkkey[4],"/");
        int long_time = stoi(days[0])*365  + (stoi(days[1])-1)*30 + stoi(days[2]);
        
        int key = long_time;
        int w  = make_hash(key);
        Insert(w,key,this_line);
    }
    endt = clock();
    use_time = (double)(endt - start) / CLOCKS_PER_SEC;
    
    
    //int keys = 736935+150+15; 2019/6/15   737100
    //ShowList();
    map<string,int> book_times;//存储了每本书借的数量
    cout << "--------------------------"<<endl;
    cout << "2019/06/15的所有借书记录"<<endl;
    first = clock();
    query = make_hash(737100);
    do
    {
        a = Find(query,737100);
        if (a!="sorry")
        {
            cout << a <<endl;
            vector<string> result = split(a,", ");

            //统计数量
            map<string,int>::iterator iter = book_times.find(result[2]);
            if(iter!=book_times.end())
            {
                int temp = iter->second;
                temp += 1;
                book_times[iter->first] = temp;
            }
            else
            {
                book_times[result[2]]=1;
            }
        }
    } while (a!="sorry");
    last = clock();
    int maxt = 0;
    string bookid;
    //map<string,int>::iterator fin;
    for(map<string,int>::iterator iter=book_times.begin(); iter!=book_times.end();iter++)
    {
        if(iter->second > maxt)
        {
            maxt = iter->second;
            bookid = iter->first;
        }
    }
    custom_time = (double)(last - first) /CLOCKS_PER_SEC;
    cout <<"号码为"<<bookid<<"的书在该天被借次数最多,有"<<maxt<<"次。"<<endl;
    cout << "--------------------------"<<endl;
    cout << "建立索引用的时间"<<use_time<<"秒"<<endl;
    cout<<"查询用时"<<custom_time<<"秒"<<endl;
    ve.clear();
    FreeAll();
    infile.close();


    //查询西游记的书的id放在vector中 
    //查询这天借的书id（result[2]）在vector_西游记中的人id(result[1])   放进vector_personid
    //去user表建立索引  按照personid建立  查询电话号码OK

    //string bookss[12]={"红楼梦", "三国演义", "西游记", "水浒传", "百年孤独", "悲惨世界", "数据库系统概念", "算法导论", "计算机网络", "研究生英语", "新华字典", "牛津英汉词典"};
    //map<string, int > book_list;
    // for (int i = 0;i<12;i++)
    // {
    //     book_list[bookss[i]]=i;
    // } 
    cout<<endl<<"查询5:查询在2019/08/23借了西游记的所有用户的电话"<<endl;
    
    //查询有多少个西游记   把所有的ID放在xiyou_id中
    infile.open("books.txt");
    if(!infile) cout<<"error"<<endl;
    while(getline(infile,line))
    {
        ve.push_back(line);
    }
    vector<string> xiyou_id;    //存所有叫西游记的书的id
    for(int i=0;i<ve.size();i++) {
        string this_line  = ve[i];
        vector<string> kkkey = split(ve[i],", ");
        if(kkkey[1]=="西游记")
        {
            xiyou_id.push_back(kkkey[0]);
        }  
    }
    ve.clear();
    FreeAll();
    infile.close();
    

    //给索书号建索引  
    infile.open("borrows.txt");
    if(!infile) cout<<"error"<<endl;
    while(getline(infile,line))             //按空格读取，遇到空白符结束
    {
        ve.push_back(line);
    }
    //cout << "共读入的记录条数:" << ve.size() << endl;
    //按照索书号建立索引
    cout<<"建立索引"<<endl;
    start = clock();
    for(int i=0;i<ve.size();i++) {
        // 为文件建立索引
        string this_line  = ve[i];
        //cout << "this line:"<< this_line<<endl;
        vector<string> kkkey = split(ve[i],", ");
        int key = stoi(kkkey[2]);   
        int w  = make_hash(key);
        Insert(w,key,this_line);
    }
    endt = clock();
    use_time = (double)(endt - start) / CLOCKS_PER_SEC;
    

    //ShowList();
    
    vector<string> borr_xiyou_id;
    //查询每个西游记  并记录符合时间的记录id
    cout << "--------------------------"<<endl;
    cout << "所有借了西游记的借书记录："<<endl;
    first = clock();
    for(int i=0; i<xiyou_id.size(); i++)
    {
        query = make_hash(stoi(xiyou_id[i]));
        do
        {
            a = Find(query,stoi(xiyou_id[i]));
            if (a!="sorry")
            {
                cout << a <<endl;
                vector<string> tempp = split(a,", ");
                if(tempp[4]=="2019/08/23")
                    borr_xiyou_id.push_back(tempp[1]);//borr_xiyou_id存的都是符合时间的借了西游记的人
            }
        } while (a!="sorry");
    }
    last = clock();
    custom_time = (double)(last - first) /CLOCKS_PER_SEC;
    ve.clear();
    FreeAll();
    infile.close();

    //为user用id建立索引
    //C++读取文件
    infile.open("users.txt");
    if(!infile) cout<<"error"<<endl;
    while(getline(infile,line))             //按空格读取，遇到空白符结束
    {
        ve.push_back(line);
    }
    //cout << "共读入的记录条数:" << ve.size() << endl;

    //给users建立索引
    start = clock();
    for(int i=0;i<ve.size();i++) {
        // 为文件建立索引
        string this_line  = ve[i];
        //cout << "this line:"<< this_line<<endl;
        vector<string> kkkey = split(ve[i],", ");
        int key = stoi(kkkey[0]);   
        int w  = make_hash(key);
        Insert(w,key,this_line);
    }
    endt = clock();
    use_time += (double)(endt - start) / CLOCKS_PER_SEC; //两个索引的时间
    
    cout<<"在2019/08/23借西游记的用户的信息："<<endl;
    //ShowList();
    first = clock();
    for(int i=0; i<borr_xiyou_id.size(); i++)
    {
        query = make_hash(stoi(borr_xiyou_id[i]));
        do
        {
            a = Find(query,stoi(borr_xiyou_id[i]));
            if (a!="sorry")
            {
                cout << a <<endl;
            }
        } while (a!="sorry");
    }
    last = clock();
    cout << "--------------------------"<<endl;
    custom_time += (double)(last - first) /CLOCKS_PER_SEC;
    cout << "建立索引用的时间"<<use_time<<"秒"<<endl;
    cout << "查询用时"<<custom_time<<"秒"<<endl;
    ve.clear();
    FreeAll();
    infile.close();

}