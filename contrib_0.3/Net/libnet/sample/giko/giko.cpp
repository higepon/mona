#include <baygui.h>
#include <net.h>
#include <string>
#include "url.h"
#include "TopLabel.h"
#include "StringHelper.h"

using namespace std;
using namespace net;

static URL* url;
static void doGet();
static void addText(Label* label, string text);

class Giko : public Frame
{
private:
    Button* button1;
    Scrollbar* scroll;
    dword threadID_;

    const int WIDTH;
    const int HEIGHT;
    const int INNER_WIDTH;
    const int INNER_HEIGHT;
    const int PADDING;
    const int WINDOW_PADDIING;
    const int BUTTON_WIDTH;
    const int LABEL_HEIGHT;
    const int SCROLL_WIDTH;
    const char* DEFAULT_URL;

public:
    Label* statusLabel;
    TopLabel* contentLabel;
    TextField* urlField;

public:
    Giko::Giko() : threadID_(THREAD_UNKNOWN)
                   , WIDTH(700)
                   , HEIGHT(500)
                   , INNER_WIDTH(WIDTH - 10)
                   , INNER_HEIGHT(HEIGHT - 30)
                   , PADDING(10)
                   , WINDOW_PADDIING(5)
                   , BUTTON_WIDTH(80)
                   , LABEL_HEIGHT(20)
                   , SCROLL_WIDTH(16)
                   , DEFAULT_URL("http://210.163.138.99/ja/")
    {
        setBounds(10, 10, WIDTH, HEIGHT);
        setTitle("ｷﾞｺ ﾌﾞﾗｳｻﾞ 0.0.2");
        urlField = new TextField();
        urlField->setText(DEFAULT_URL);
        urlField->setBounds(PADDING, PADDING, INNER_WIDTH - PADDING * 2 - BUTTON_WIDTH, LABEL_HEIGHT);
        button1 = new Button("取得");
        button1->setBounds(INNER_WIDTH - PADDING - BUTTON_WIDTH, PADDING, BUTTON_WIDTH, LABEL_HEIGHT);
        contentLabel = new TopLabel("", Label::LEFT, 12);
        contentLabel->setBounds(PADDING, PADDING + LABEL_HEIGHT, INNER_WIDTH - PADDING * 2 - SCROLL_WIDTH, INNER_HEIGHT - PADDING * 2 - LABEL_HEIGHT * 2);
        contentLabel->setBackground(Color::white);
        contentLabel->setForeground(Color::black);
        contentLabel->setText(
            "|　　 ｜　　｜　　｜　　｜　　｜　　｜　　｜\n"
            "|　　 ｜　　｜　　｜　　｜　　｜　　｜　　｜\n"
            "|　　 ｜　　｜　　｜　　｜　　｜　　｜　　｜\n"
            "|　　 ｜　　｜　　｜　　｜　　｜　　｜　　｜\n"
            "|　　 ｜　　｜　　 ∧ ∧ 　 ／￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣\n"
            "|　　 ｜　　｜　　 (,,ﾟДﾟ)＜　出してくれよとっつあん\n"
            "|　　 ｜　　｜　　 /　　| |　 \＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿\n"
            "|　　 ｜　　｜ 　 (_＿_ﾉ｜　　｜　　｜　　｜\n"
            "|　　 ｜　　｜ ／､|､ 　 ｜　　｜　　｜　　｜\n"
            "|______,|_＿__.|_＿__,|_＿__.|_＿__,|_＿__.|_＿__,|\n"
            "\n"
            "　　　　　　　　 ∧＿∧ 　 ／￣￣￣￣￣￣￣￣￣￣￣￣\n"
            "　　　　　　　　（ 　 　 ,,）＜　誰が銭形やねん\n"
            "　　　　　　　　（ ○ 　 ）　 \＿＿＿＿＿＿＿＿＿＿＿＿\n"
            "　　　　　　　　 |　| 　 |_\n"
            "　　　　　　　　（__(＿＿）\n");
        statusLabel = new Label("", Label::LEFT);
        statusLabel->setBounds(PADDING, INNER_HEIGHT - PADDING - LABEL_HEIGHT, INNER_WIDTH - PADDING * 2 -SCROLL_WIDTH, LABEL_HEIGHT);
        statusLabel->setText("y=ｰ(ﾟдﾟ)");
        scroll = new Scrollbar();
        scroll->setBounds(INNER_WIDTH - PADDING - SCROLL_WIDTH, PADDING + LABEL_HEIGHT, SCROLL_WIDTH, INNER_HEIGHT - PADDING * 2 - LABEL_HEIGHT * 2);
        stateChanged();
        add(button1);
        add(statusLabel);
        add(contentLabel);
        add(scroll);
        add(urlField);
    }

    Giko::~Giko()
    {
    }

    void Giko::processEvent(Event* evt)
    {
        if (evt->getType() == Event::MOUSE_PRESSED && evt->getSource() == button1)
        {
            wget();
        }
        else if (evt->getType() == Event::BLOCK_INCLEMENT && evt->getSource() == scroll)
        {
            contentLabel->scrollDown();
        }
        else if (evt->getType() == Event::BLOCK_DECLEMENT && evt->getSource() == scroll)
        {
            contentLabel->scrollUp();
        }
    }

    void Giko::stateChanged()
    {
        int extraLines = contentLabel->getExtraLineNumber();
        scroll->setMinimum(0);
        if (extraLines > 0)
        {
            scroll->setMaximum(extraLines);
            scroll->setEnabled(true);
        }
        else
        {
            scroll->setEnabled(false);
        }
        scroll->setBlocksize(1);
    }

private:
    void Giko::wget()
    {
        url = new URL(urlField->getText());
        if (!url->isValid())
        {
            statusLabel->setText("URLが(´･д･`) ﾔﾀﾞ");
            delete url;
            return;
        }

        if (threadID_ != THREAD_UNKNOWN)
        {
            syscall_kill_thread(threadID_);
        }

        threadID_ = syscall_mthread_create((dword)doGet);
        syscall_mthread_join(threadID_);
    }
};

static Giko* window;

int MonaMain(List<char*>* pekoe)
{
    window = new Giko();
    window->run();
    delete(window);
    return 0;
}

static void doGet()
{
    TCPClient client;
    window->statusLabel->setText("");
    addText(window->statusLabel, "接続中");
    if (!client.connect(url->getA(), url->getB(), url->getC(), url->getD(), 80))
    {
        window->statusLabel->setText("接続失敗");
        exit(1);
    }
    addText(window->statusLabel, "...OK");
    char buf[512];
    sprintf(buf, "GET %s HTTP/1.0\r\nServer:%d.%d.%d.%d\r\n\r\n", url->getPath().c_str(), url->getA(), url->getB(), url->getC(), url->getD());
    addText(window->contentLabel, buf);
    delete url;
    client.write((byte*)buf, strlen(buf));
    window->statusLabel->setText(" リクエスト中");
    window->stateChanged();
    window->statusLabel->setText(" 応答を待っています");

    for (;;)
    {
        Buffer buffer = client.read();
        addText(window->statusLabel, "..");
        std::string text(buffer.begin(), buffer.end());
        window->contentLabel->setText(window->contentLabel->getText() + StringHelper::removeDoubleBreakLines(StringHelper::removeHTMLTags(text)).c_str());
        window->stateChanged();
        if (client.isClosed()) break;
    }
    window->statusLabel->setText("完了");
    exit(1);
}

static void addText(Label* label, string text)
{
    string original = label->getText();
    original += text;
    string original2 = StringHelper::removeHTMLTags(original);
//    original = StringHelper::removeDoubleBreakLines(original);
    label->setText(original2.c_str());
}
