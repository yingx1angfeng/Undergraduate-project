#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <time.h>

using namespace std;

// 文本内容数据结构
struct BookContent {
    int pageNumber;
    int chapterNumber;
    wstring bookTitle;
    wstring textContent;
};

class TextSearchEngine {
private:
    vector<BookContent> contentCollection;

public:
    // 从文件加载文本内容
    void loadBookContent(const wstring& bookTitle, int initialChapter = 0) {
        locale::global(locale("zh_CN.UTF-8"));
        wifstream fileStream(L"TXT\\" + bookTitle + L".txt");
        fileStream.imbue(locale(fileStream.getloc(), new codecvt_utf8<wchar_t>));

        if (!fileStream) {
            wcerr << L"错误：无法打开文件 " << bookTitle << L".txt" << endl;
            return;
        }

        BookContent currentEntry;
        currentEntry.bookTitle = bookTitle;
        currentEntry.pageNumber = 1;
        currentEntry.chapterNumber = initialChapter;

        wstring line;
        while (getline(fileStream, line)) {
            if (line.length() <= 20 && !line.empty()) {
                // 检查是否为页码
                bool isPageNumber = true;
                for (wchar_t c : line) {
                    if (!isdigit(c)) {
                        isPageNumber = false;
                        break;
                    }
                }

                if (isPageNumber) {
                    try {
                        currentEntry.pageNumber = stoi(line) + 1;
                    }
                    catch (const invalid_argument& e) {
                        wcerr << L"警告：无效页码格式: " << line << endl;
                    }
                }
                else {
                    // 检查是否为章节标题
                    wstring lowerLine = line;
                    transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::towlower);
                    if (lowerLine.substr(0, 7) == L"chapter") {
                        currentEntry.chapterNumber++;
                    }
                }
            }
            else {
                // 普通文本行
                currentEntry.textContent = line;
                contentCollection.push_back(currentEntry);
            }
        }
    }

    // 批量加载多本书籍
    void loadAllBooks() {
        loadBookContent(L"J.K. Rowling - HP 0 - Harry Potter Prequel", -1);
        loadBookContent(L"J.K. Rowling - HP 2 - Harry Potter and the Chamber of Secrets");
        loadBookContent(L"J.K. Rowling - HP 3 - Harry Potter and the Prisoner of Azkaban");
        loadBookContent(L"J.K. Rowling - HP 4 - Harry Potter and the Goblet of Fire");
        loadBookContent(L"J.K. Rowling - HP 6 - Harry Potter and the Half-Blood Prince");
        loadBookContent(L"J.K. Rowling - HP 7 - Harry Potter and the Deathly Hallows");
        loadBookContent(L"J.K. Rowling - Quidditch Through the Ages");
        loadBookContent(L"J.K. Rowling - The Tales of Beedle the Bard");
    }

    // 在文本中搜索指定内容
    vector<BookContent> searchContent(const wstring& searchTerm) const {
        vector<BookContent> results;
        for (const auto& entry : contentCollection) {
            if (entry.textContent.find(searchTerm) != wstring::npos) {
                results.push_back(entry);
            }
        }
        return results;
    }

    // 显示搜索结果
    static void displaySearchResults(const vector<BookContent>& results, const wstring& searchTerm) {
        wcout << L"-------------------------------------------------------------------------" << endl;
        wcout << setw(5) << L"序号" << setw(15) << L"搜索词" << setw(10) << L"页码"
            << setw(10) << L"章节" << setw(40) << L"书名" << endl;
        wcout << L"-------------------------------------------------------------------------" << endl;

        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            wcout << setw(5) << i + 1 << setw(15) << searchTerm << setw(10) << result.pageNumber
                << setw(10) << result.chapterNumber << setw(40) << result.bookTitle.substr(0, 35) << L"..." << endl;
        }
    }

    // 显示指定段落
    static void displayParagraph(const BookContent& content) {
        wcout << L"\n\n" << setfill(L'=') << setw(80) << L"" << setfill(L' ') << endl;
        wcout << L"《" << content.bookTitle << L"》 第" << content.chapterNumber
            << L"章 第" << content.pageNumber << L"页" << endl;
        wcout << setfill(L'=') << setw(80) << L"" << setfill(L' ') << endl;
        wcout << content.textContent << endl;
        wcout << setfill(L'=') << setw(80) << L"" << setfill(L' ') << endl;
    }
};

int main() {
    try {
        TextSearchEngine engine;
        wcout << L"正在加载哈利波特系列书籍..." << endl;
        engine.loadAllBooks();
        wcout << L"书籍加载完成！" << endl;

        while (true) {
            wstring searchTerm;
            wcout << L"\n请输入要查询的人名/地名 (输入'q'退出): ";
            getline(wcin, searchTerm);

            if (searchTerm == L"q" || searchTerm == L"Q") {
                break;
            }

            auto results = engine.searchContent(searchTerm);

            if (results.empty()) {
                wcout << L"未找到相关内容！" << endl;
                continue;
            }

            clock_t startTime, endTime;
            startTime = clock();

            engine.displaySearchResults(results, searchTerm);

            endTime = clock();
            cout << "Totle Time : "
                << (double)(endTime - startTime) /
                CLOCKS_PER_SEC << "s" << endl;

            while (true) {
                wcout << L"\n请输入要查看的段落序号 (0返回上一级): ";
                int selection;
                wcin >> selection;
                wcin.ignore(numeric_limits<streamsize>::max(), L'\n');

                if (selection == 0) {
                    break;
                }

                if (selection > 0 && static_cast<size_t>(selection) <= results.size()) {
                    engine.displayParagraph(results[selection - 1]);
                }
                else {
                    wcout << L"无效的序号，请重新输入！" << endl;
                }
            }
        }

        return 0;

    }
    catch (const exception& e) {
        wcerr << L"程序发生错误: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        wcerr << L"发生未知错误！" << endl;
        return 1;
    }
}