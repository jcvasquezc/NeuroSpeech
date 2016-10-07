#ifndef SIGPROC_H
#define SIGPROC_H

#include <QAudioRecorder>
#include <QMediaRecorder>
#include <QMainWindow>

class QAudioRecorder;

class sigproc:  public QObject {
public:
    explicit sigproc(QWidget *parent = 0);
    ~sigproc();

private slots:
    void record_sig(int rec_flag, bool state);

private:
    Q_OBJECT

    QAudioRecorder *audioRecorder;
};

#endif // SIGPROC_H
