#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QDir>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleNetworkData();
    void updatePosition(qint64 pos);
    void updateDuration(qint64 dur);
    void setPosition(int pos);
    void setVolume(int vol);

private:
    // 动态创建控件，不依赖 UI 文件
    QWidget *centralWidget;
    QLabel *label_title;
    QLabel *label_ip;
    QLabel *label_time;
    QSlider *slider_progress;
    QSlider *slider_volume;

    QUdpSocket *udpSocket;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    QStringList playlist;
    int currentIndex = 0;

    void loadFiles();
    void playCurrent();
    QString formatTime(qint64 ms);
};
#endif
