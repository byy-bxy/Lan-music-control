#include "mainwindow.h"
#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // --- 界面布局代码：代替 UI 设计器 ---
    centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    label_title = new QLabel("正在初始化...", this);
    label_ip = new QLabel("获取 IP 中...", this);
    label_time = new QLabel("00:00 / 00:00", this);

    slider_progress = new QSlider(Qt::Horizontal, this);
    slider_volume = new QSlider(Qt::Horizontal, this);
    slider_volume->setRange(0, 100);
    slider_volume->setValue(80);

    layout->addWidget(label_ip);
    layout->addWidget(label_title);
    layout->addWidget(slider_progress);
    layout->addWidget(label_time);
    layout->addWidget(new QLabel("音量控制:", this));
    layout->addWidget(slider_volume);

    setCentralWidget(centralWidget);
    this->resize(400, 300);
    this->setWindowTitle("C++ 音乐服务端");

    // --- 核心播放逻辑 ---
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 8888);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::handleNetworkData);

    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);
    connect(slider_progress, &QSlider::sliderMoved, this, &MainWindow::setPosition);
    connect(slider_volume, &QSlider::valueChanged, this, &MainWindow::setVolume);

    // 获取 IP
    QString ip = "127.0.0.1";
    for (const auto &addr : QNetworkInterface::allAddresses()) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress::LocalHost) {
            ip = addr.toString();
            break;
        }
    }
    label_ip->setText("手机端输入 IP: " + ip);

    loadFiles();
}

void MainWindow::loadFiles() {
    QDir dir(qApp->applicationDirPath());
    playlist = dir.entryList(QStringList() << "*.mp3", QDir::Files);
    if (!playlist.isEmpty()) {
        label_title->setText("已找到 " + QString::number(playlist.size()) + " 首歌");
    } else {
        label_title->setText("请在 .exe 同目录下放入 .mp3 文件");
    }
}

void MainWindow::handleNetworkData() {
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QString cmd = QString::fromUtf8(datagram.data()).trimmed();
        if (playlist.isEmpty()) { loadFiles(); if(playlist.isEmpty()) return; }
        if (cmd == "PLAY") {
            if (player->playbackState() == QMediaPlayer::PlayingState) player->pause();
            else if (player->playbackState() == QMediaPlayer::PausedState) player->play();
            else playCurrent();
        } else if (cmd == "NEXT") {
            currentIndex = (currentIndex + 1) % playlist.size();
            playCurrent();
        } else if (cmd == "PREV") {
            currentIndex = (currentIndex - 1 + playlist.size()) % playlist.size();
            playCurrent();
        }
    }
}

void MainWindow::playCurrent() {
    player->setSource(QUrl::fromLocalFile(playlist[currentIndex]));
    player->play();
    label_title->setText(playlist[currentIndex]);
}

void MainWindow::updatePosition(qint64 pos) {
    if (!slider_progress->isSliderDown()) {
        slider_progress->setValue(static_cast<int>(pos));
    }
    label_time->setText(formatTime(pos) + " / " + formatTime(player->duration()));
}

void MainWindow::updateDuration(qint64 dur) {
    slider_progress->setRange(0, static_cast<int>(dur));
}

void MainWindow::setPosition(int pos) { player->setPosition(pos); }
void MainWindow::setVolume(int vol) { audioOutput->setVolume(vol / 100.0); }

QString MainWindow::formatTime(qint64 ms) {
    int s = ms / 1000;
    int m = s / 60;
    s = s % 60;
    return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

MainWindow::~MainWindow() {}
