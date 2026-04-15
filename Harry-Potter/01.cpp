#include <fstream>
#include <deque>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <locale>
#include <codecvt>
using namespace std;

class Text
{
public:
    int page;
    int chapter;
    wstring bookName;
    wstring content;
};

deque <Text> text;
Text one_Content;


void get_text(wstring book, int ch = 0)
{
    locale::global(locale("zh_CN.UTF-8"));
    wifstream in(L"TXT\\" + book + L".txt");
    in.imbue(locale(in.getloc(), new codecvt_utf8<wchar_t>));
    wstring line, con;
    one_Content.bookName = book;
    one_Content.page = 1;
    one_Content.chapter = ch;
    int len = 0;
    if (in)
    {
        while (getline(in, line))
        {
            len = static_cast<int>(line.length());
            if (len <= 20 && len > 0)
            {
                int counts = 0;
                for (int i = 0; i < len; i++)
                    if (isdigit(static_cast<int>(line[i])))
                        counts++;
                if (counts == len)
                    one_Content.page = stoi(line) + 1;
                else
                {
                    transform(line.begin(), line.end(), line.begin(), ::towlower);
                    if (line.substr(0, 7) == L"chapter")
                        one_Content.chapter++;
                }
            }
            else
            {
                one_Content.content = line;
                text.push_back(one_Content);
            }
        }
    }
    else
        wcout << L"不存在此文本：" << book << endl;
}

void input_Book()
{
    get_text(L"J.K. Rowling - HP 0 - Harry Potter Prequel", -1);
    get_text(L"J.K. Rowling - HP 2 - Harry Potter and the Chamber of Secrets");
    get_text(L"J.K. Rowling - HP 3 - Harry Potter and the Prisoner of Azkaban");
    get_text(L"J.K. Rowling - HP 4 - Harry Potter and the Goblet of Fire");
    get_text(L"J.K. Rowling - HP 6 - Harry Potter and the Half-Blood Prince");
    get_text(L"J.K. Rowling - HP 7 - Harry Potter and the Deathly Hallows");
    get_text(L"J.K. Rowling - Quidditch Through the Ages");
    get_text(L"J.K. Rowling - The Tales of Beedle the Bard");
}

void search_in_File(wstring info, deque<Text>& s)
{
    int all_len = static_cast<int>(text.size());
    wstring::size_type p;
    for (int i = 0; i < all_len; i++)
    {
        p = text[i].content.find(info);
        if (!(p == wstring::npos))
            s.push_back(text[i]);
    }
}

void show_Info(deque<Text> s, wstring info)
{
    wcout << L"序号       人名/地名       页码       章节       书名" << endl;
    for (int i = 0; i < static_cast<int>(s.size()); i++)
        wcout << L"  "
        << i + 1 << L"       "
        << info << L"         "
        << s[i].page << L"         "
        << s[i].chapter << L"         "
        << s[i].bookName << endl;
}

int main()
{
    input_Book();
    wstring input_Info;
    deque<Text> search_Info;
    wcout << L"请输入要查询的 人名/地名: " << endl;
    getline(wcin, input_Info);
    search_in_File(input_Info, search_Info);
    if (!search_Info.empty())
    {
        show_Info(search_Info, input_Info);
        wcout << endl << L"请输入要显示的段落的序号: " << endl;
        while (1)
        {
            int x = 1;
            wcin >> x;
            wcin.ignore(numeric_limits<streamsize>::max(), L'\n');
            if (x == 0)
                break;
            wcout << endl << search_Info[x - 1].content << endl;
            wcout << endl << L"若要继续显示结果，请继续输入序号，否则输入 0:" << endl;
        }
    }
    else
        wcout << L"无法查询到 ! ! !" << endl;
    return 0;
}