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
    uint32_t threadID_;

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
    Label* leftSpace;
    Label* l1;
    Label* l2;
    Label* l3;
    Label* l4;
    TopLabel* contentLabel;
    TextField* urlField;
    Image* icon;

public:
    Giko() : threadID_(THREAD_UNKNOWN)
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
        icon = new Image("/APPS/BAYGUI/GIKO.APP/FAVICON.BMP");
//        icon->setBounds(0, 0, PADDING + 16, 16);

        urlField = new TextField();
        urlField->setText(DEFAULT_URL);
        urlField->setBounds(PADDING + PADDING - 2, PADDING, INNER_WIDTH - PADDING * 2 - BUTTON_WIDTH + 2, LABEL_HEIGHT);
        button1 = new Button("取得");
        button1->setBounds(INNER_WIDTH - BUTTON_WIDTH, PADDING, BUTTON_WIDTH, LABEL_HEIGHT);
        leftSpace = new Label("");
        leftSpace->setBounds(0, PADDING + LABEL_HEIGHT, PADDING, INNER_HEIGHT - PADDING * 2 + 5 - LABEL_HEIGHT * 2);
        leftSpace->setBackground(Color::white);
        contentLabel = new TopLabel("", Label::LEFT, 12);
        contentLabel->setBounds(PADDING, PADDING + LABEL_HEIGHT, INNER_WIDTH - PADDING - SCROLL_WIDTH, INNER_HEIGHT - PADDING - PADDING + 5 - LABEL_HEIGHT * 2);
        contentLabel->setBackground(Color::white);
        contentLabel->setForeground(Color::black);
        contentLabel->setText(
            "\n\n\n"
            "□□□□□□□□□□■□□□□□□□■□□□□□□□□□□\n"
            "□□□□□□□□□■□■□□□□□■□■□□□□□□□□□\n"
            "□□□□□□□□■□□■□□□□□■□□■□□□□□□□□\n"
            "□□□□□□□■□□□□■■■■■□□□□■□□□□□□□\n"
            "□□□□□□□■□□□□□□□□□□□□□■□□□□□□□\n"
            "□□□□□□■□□■□□□□□□□□□□■□■□□□□□□\n"
            "□■□□□□■□□□■□□□□□□□□■□□■□□□□□□\n"
            "□■□□□■□□■■■■□□□□□□■■■■□■□□□□□\n"
            "□■□□□■□□□□■□□□□□□□□■□□□■□□□□□\n"
            "□■□□□■□□□■□□□□□□□□□□■□□■□□□□□\n"
            "□■□□□■□□□□□□■■■■■■■□□□□■□□□□□\n"
            "□□■□□■□□□□□□■□□□□■□□□□□■□□□□□\n"
            "□□■□□■□□□□□□■□□□□■□□□□□■□□□□□\n"
            "□□□■□■□□□□□□□■□□■□□□□□■□□□□□□\n"
            "□□□■□□■■□□□□□□■■□□□□□■■□□□□□□\n"
            "□□□□■□□□■■■■■■■■■■■■■□□□□□□□□\n"
            "□□□□□■□□□□□■□□□□□■□□□□□□□□□□□\n"
            "□□□□□□■■□□□■□□□□□■□□□□□□□□□□□\n"
            );
        // l1 - l4は飾りさ。
        l1 = new Label("");
        l1->setBounds(0, INNER_HEIGHT - PADDING - LABEL_HEIGHT + 5, INNER_WIDTH, 1);
        l1->setBackground(Color::gray);
        l2 = new Label("");
        l2->setBounds(0, INNER_HEIGHT - PADDING - LABEL_HEIGHT + 5 + 1, INNER_WIDTH, 2);
        l3 = new Label("");
        l3->setBounds(0, INNER_HEIGHT - PADDING - LABEL_HEIGHT + 5 + 3, INNER_WIDTH, 1);
        l3->setBackground(Color::gray);

        statusLabel = new Label("", Label::LEFT);
        statusLabel->setBounds(PADDING, INNER_HEIGHT - 16, INNER_WIDTH - PADDING * 2 -SCROLL_WIDTH, LABEL_HEIGHT - 5);
        statusLabel->setText("y=ｰ(ﾟдﾟ)");

        l4 = new Label("");
        l4->setBounds(0, INNER_HEIGHT - 1, INNER_WIDTH, 1);
        l4->setBackground(Color::white);

        scroll = new Scrollbar();
        scroll->setBounds(INNER_WIDTH - SCROLL_WIDTH, PADDING + LABEL_HEIGHT, SCROLL_WIDTH, INNER_HEIGHT - PADDING * 2 + 5 - LABEL_HEIGHT * 2);
        stateChanged();
        add(button1);
        add(l1);
        add(l2);
        add(l3);
        add(l4);
        add(statusLabel);
        add(leftSpace);
        add(contentLabel);
        add(scroll);
        add(urlField);
    }

    ~Giko()
    {
    }

    void paint(Graphics *g) {
        g->drawImage(icon, 1, PADDING + 2);
    }

    void processEvent(Event* evt)
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

    void stateChanged()
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
    void wget()
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

        threadID_ = syscall_mthread_create((uint32_t)doGet);
// comment out by higepon
//        syscall_mthread_join(threadID_);
    }
};

static Giko* window;

int main(int argc, char* argv[])
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
    client.write((uint8_t*)buf, strlen(buf));
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
