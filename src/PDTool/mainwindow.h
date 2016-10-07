#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "dialogb.h"
#include <QAudioInput>
#include <QMediaRecorder>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QAudioRecorder;
class QDateTime;
class QCustomPlot;
class QAudioProbe;
class QAudioBuffer;
QT_END_NAMESPACE
class QAudioLevel;



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void opendialogb();
    void processBuffer(const QAudioBuffer&);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_5_toggled(bool checked);
    void on_pushButton_6_toggled(bool checked);
    QString get_name(int rec_flag, QString folder);
    QVector<double> readWavFile(QString fileName);
    QVector<double> readFormants(QString nameFile);

//int rawToWav(QString rawfn, QString wavfn, long frequency);

    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_7_clicked();
    void mouseDoubleClick(QMouseEvent *event);
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void updateStatus(QMediaRecorder::Status);
    void onStateChanged(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void displayErrorMessage();
    int printpdf(QString inputhtml, QString otputfile);

    void on_pushButton_8_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_9_clicked();

private:
    void clearAudioLevels();
    Ui::MainWindow *ui;
    Dialogb *dialogb;
    QAudioRecorder *audioRecorder;
    QAudioProbe *probe;
    QDateTime *dateTime;
    QCustomPlot * customPlot;
    QCustomPlot * customPlot_2;
    QCustomPlot * customPlot_3;
    QCustomPlot * customPlot_4;
    QMediaPlayer * player;
    QCPRange xrange;
    QVector<double> xmain;
    QVector<double> ymain;
    QList<QAudioLevel*> audioLevels;
    QLabel * label_8;
    QLabel * label_9;
    QLabel * label_10;
    QLabel * label_11;
    QLabel * label_12;
    QLabel * label_13;
    QLabel * label_28;
    QLabel * label_31;
    QAudioInput* audio;
    QString path_patient;
    QString namePatient;
    QString lastPatient;
    QString namemain;
    QString file_2play;
    int flagCuted=0;
    float contAxes;
    float WAavg, DTWavg;
    QVector<float> xaxiscut;
   // QFile destinationFile;
};

#endif // MAINWINDOW_H
