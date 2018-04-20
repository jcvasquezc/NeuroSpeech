#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <QAudioRecorder>
#include <QMediaRecorder>
#include <QAudioEncoderSettingsControl>
#include <QDateTime>
#include <QAudioProbe>
#include <qmath.h>
#include "qaudiolevel.h"
#include "definitions.h"
#include "make_wav.h"
#include "strings.h"
#include <stdio.h>
#include <QFileInfo>
static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);

template <class T>
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
  ui(new Ui::MainWindow)

{

    ui->setupUi(this);
    audioRecorder = new QAudioRecorder(this);
    player =new QMediaPlayer;
    probe = new QAudioProbe;

    connect(probe, SIGNAL(audioBufferProbed(QAudioBuffer)),
            this, SLOT(processBuffer(QAudioBuffer)));
    probe->setSource(audioRecorder);

    connect(audioRecorder, SIGNAL(durationChanged(qint64)), this,
            SLOT(updateProgress(qint64)));

    connect(audioRecorder, SIGNAL(statusChanged(QMediaRecorder::Status)), this,
            SLOT(updateStatus(QMediaRecorder::Status)));

    connect(audioRecorder, SIGNAL(stateChanged(QMediaRecorder::State)),
            this, SLOT(onStateChanged(QMediaRecorder::State)));

    connect(audioRecorder, SIGNAL(error(QMediaRecorder::Error)), this,
            SLOT(displayErrorMessage()));

    dateTime=new QDateTime();
    customPlot = new QCustomPlot();
    customPlot_2 = new QCustomPlot();
    customPlot_3 = new QCustomPlot();

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));
    QPen pen;
    ui->customPlot->setBackground(plotGradient);
    ui->customPlot_2->setBackground(plotGradient);
    ui->customPlot_3->setBackground(plotGradient);
    ui->customPlot_4->setBackground(plotGradient);
    ui->customPlot_5->setBackground(plotGradient);
    ui->customPlot_6->setBackground(plotGradient);
    ui->customPlot_7->setBackground(plotGradient);
    ui->customPlot_8->setBackground(plotGradient);
    ui->customPlot_9->setBackground(plotGradient);
    ui->customPlot_10->setBackground(plotGradient);
    ui->customPlot_11->setBackground(plotGradient);
    ui->customPlot_12->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->replot();
    ui->customPlot->setInteraction(QCP::iRangeDrag, true);
    ui->customPlot->setInteraction(QCP::iRangeZoom, true);
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());

    connect(ui->customPlot, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoubleClick(QMouseEvent*)));
    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    contAxes =0;
    xaxiscut={0,0};
    QPixmap pix(button1);
    QIcon icon(pix);
    ui->pushButton->setIcon(icon);
    ui->pushButton->setIconSize(pix.size());

    QPixmap pix2(button2);
    QIcon icon2(pix2);
    ui->pushButton_2->setIcon(icon2);
    ui->pushButton_2->setIconSize(pix2.size());

    QPixmap pix3(button3);
    QIcon icon3(pix3);
    ui->pushButton_3->setIcon(icon3);
    ui->pushButton_3->setIconSize(pix3.size());

    QPixmap pix4(button4);
    QIcon icon4(pix4);
    ui->pushButton_4->setIcon(icon4);
    ui->pushButton_4->setIconSize(pix4.size());

    QPixmap pix9(button5);
    QIcon icon9(pix9);
    ui->pushButton_9->setIcon(icon9);
    ui->pushButton_9->setIconSize(pix9.size());


    QPixmap pix10(button7);
    QIcon icon10(pix10);
    ui->pushButton_10->setIcon(icon10);
    ui->pushButton_10->setIconSize(pix10.size());

    QPixmap pixlogo(logo);
    ui->label_68->setPixmap(pixlogo);

    QPixmap pix5(button5rec);
    QIcon icon5(pix5);
    ui->pushButton_5->setIcon(icon5);
    ui->pushButton_5->setIconSize(QSize(71,71));
    ui->pushButton_5->setCheckable(true);

    QPixmap pix6(button6play);
    QIcon icon6(pix6);
    ui->pushButton_6->setIcon(icon6);
    ui->pushButton_6->setIconSize(QSize(71,71));
    ui->pushButton_6->setCheckable(true);


    ui->tabWidget->setTabText(1, phonation);
    ui->tabWidget->setTabText(2, articulation);
    ui->tabWidget->setTabText(3, prosody);
    ui->tabWidget->setTabText(4, intelligibility);
    ui->tabWidget->setTabText(6, evaluation);
    ui->tabWidget->setTabText(5, DDK);
    ui->pushButton_7->setText(playExample);
    ui->pushButton_8->setText(createReport);

    //ui->label_7->setText(energyLab);
    ui->label_13->setText(degreeU);
    ui->label_81->setText(vRateLab);
    ui->label_82->setText(avgVoicedLab);
    ui->label_83->setText(stdVoicedLab);
    ui->label_84->setText(silRateLab);
    ui->label_85->setText(avgSilLab);
    ui->label_86->setText(stdSilLab);
    ui->label_87->setText(avgF0Lab);
    ui->label_88->setText(stdF0Lab);
    ui->label_89->setText(maxF0Lab);
    ui->label_90->setText(avgEnLab);
    ui->label_91->setText(stdEnLab);
    ui->label_92->setText(maxEnLab);
    ui->label_35->setText(readsent);
    ui->label_36->setText(predsent);
    ui->label_121->setText(respCap);
    ui->label_124->setText(LipsMov);
    ui->label_126->setText(PalMov);
    ui->label_128->setText(LarMov);
    ui->label_131->setText(TongueVel);
    ui->label_134->setText(intelligibility);
    ui->label_142->setText(mfdasc);
    ui->label_144->setText(updrssc);



    ui->radioButton_1->setChecked(true);
    QCheckBox *checkBox = new QCheckBox(tr("Ind&ependent checkbox"));
    checkBox->setChecked(true);

    ui->radioButton_4->setChecked(true);
    QCheckBox *checkBox2 = new QCheckBox(tr("Ind&ependent checkbox"));
    checkBox2->setChecked(true);

    QStringList list;
    list<<listTasks;

    ui->listWidget->addItems(list);


    // remove temporal files

    if (remove("../phonVowels/feat.txt")!=0){perror("error deleting file");}
    if (remove("../phonVowels/featf0.txt")!=0){perror("error deleting file");}
    if (remove("../phonVowels/tempf0.txt")!=0){perror("error deleting file");}
    if (remove("../phonVowels/phonation1.png")!=0){perror("error deleting file");}
    if (remove("../phonVowels/phonation2.png")!=0){perror("error deleting file");}
    if (remove("../artVowels/feat.txt")!=0){perror("error deleting file");}
    if (remove("../artVowels/articulation1.png")!=0){perror("error deleting file");}
    if (remove("../artCont/articulation.png")!=0){perror("error deleting file");}
    if (remove("../artCont/feat.txt")!=0){perror("error deleting file");}
    if (remove("../prosody/feat.txt")!=0){perror("error deleting file");}
    if (remove("../prosody/prosody.png")!=0){perror("error deleting file");}
    if (remove("../prosody/prosodyradar.png")!=0){perror("error deleting file");}
    if (remove("../intelligibility/intelligibility.png")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat1.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat2.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat3.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat4.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat5.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat6.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat7.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat8.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat9.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat10.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/feat11.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred1.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred2.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred3.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred4.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred5.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred6.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred7.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred8.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred9.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred10.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred11.txt")!=0){perror("error deleting file");}
    if (remove("../evaluation/prediction.png")!=0){perror("error deleting file");}
    if (remove("../evaluation/predmFDA.txt")!=0){perror("error deleting file");}
    if (remove("../evaluation/vuv.txt")!=0){perror("error deleting file");}
    if (remove("../evaluation/tempForm.txt")!=0){perror("error deleting file");}

    if (remove("../phonation1.png")!=0){perror("error deleting file");}
    if (remove("../phonation2.png")!=0){perror("error deleting file");}
    if (remove("../articulation1.png")!=0){perror("error deleting file");}
    if (remove("../articulation2.png")!=0){perror("error deleting file");}
    if (remove("../prosody1.png")!=0){perror("error deleting file");}
    if (remove("../prosody2.png")!=0){perror("error deleting file");}
    if (remove("../prosody3.png")!=0){perror("error deleting file");}
    if (remove("../test.wav")!=0){perror("error deleting file");}


 // open window for AD images




}

MainWindow::~MainWindow()
{
    delete ui;
    delete audioRecorder;
    delete probe;
}


void MainWindow::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

    if (ui->customPlot->graphCount() > 0)
      menu->addAction("Clear graph", this, SLOT(removeAllGraphs()));
      menu->addAction("Save Audio", this, SLOT(saveAudio()));



  menu->popup(ui->customPlot->mapToGlobal(pos));
}


void MainWindow::saveAudio(){
    QProcess cmd;
    double BUF_SIZE=(xaxiscut[1]-xaxiscut[0])*fs;
    QVector<double> buffer;
    for (int i=0; i< BUF_SIZE; i++)
    {   buffer.append(ymain[i+(int)(xaxiscut[0]*fs)]);
    }
    qDebug()<<BUF_SIZE<<xaxiscut[0]*fs<<xaxiscut[1]*fs;
    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../";

    QString namecut = QFileDialog::getSaveFileName(this, tr("save audio"), "", tr("Audio-Files(*.wav)"));

    QString comandcut="python "+path_base+"cutAudio.py "+namemain+" "+namecut+" "+QString::number(xaxiscut[0])+" "+QString::number(xaxiscut[1]);
    qDebug()<< comandcut;
    cmd.start(comandcut);
    cmd.waitForFinished(-1);
    flagCuted=1;

}

void MainWindow::removeAllGraphs()
{QPen pen;
    QLinearGradient plotGradient;
  contAxes=0;
  ui->customPlot->clearGraphs();
  ui->customPlot->addGraph();
  ui->customPlot->graph()->setData(xmain,ymain);
  ui->customPlot->xAxis->setRange(0, (double)xmain.size()/fs);
  ui->customPlot->yAxis->setRange(-1, 1);
  ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
  ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
  ui->customPlot->xAxis->setLabelFont(QFont(QFont().family(), 12));
  ui->customPlot->yAxis->setLabelFont(QFont(QFont().family(), 12));
  ui->customPlot->xAxis->setLabel(xaxistime);
  ui->customPlot->yAxis->setLabel(yaxisA);
  plotGradient.setStart(0, 0);
  plotGradient.setFinalStop(0, 350);
  plotGradient.setColorAt(0, QColor(230, 230, 230));
  plotGradient.setColorAt(1, QColor(220, 220, 220));

  ui->customPlot->setBackground(plotGradient);
  pen.setColor(QColor(0,120,0));
  ui->customPlot->graph()->setPen(pen);
  ui->customPlot->replot();
  contAxes=0;
}

void MainWindow::mouseDoubleClick(QMouseEvent *event)
{   QProcess cmd;
   QPen pen;
   QPoint p = event->pos();
   QVector<double> xt(2);
   QVector<double> y={-1,1};
   xrange=ui->customPlot->xAxis->range();
   double m=(xrange.upper-xrange.lower)/945;
   double b=xrange.lower-m*55;
   xt[0]=m*(double)p.x()+b;
   xt[1]=xt[0];

   contAxes++;
   if (contAxes==1){
       xaxiscut[0]=xt[0];
       ui->customPlot->addGraph();
       ui->customPlot->graph()->setData(xt,y);
       pen.setColor(QColor(120,0,0));
       ui->customPlot->graph()->setPen(pen);
       ui->customPlot->replot();}
   if (contAxes==2){
       xaxiscut[1]=xt[1];
       ui->customPlot->addGraph();
       ui->customPlot->graph()->setData(xt,y);
       pen.setColor(QColor(120,0,0));
       ui->customPlot->graph()->setPen(pen);
       ui->customPlot->replot();
       double BUF_SIZE=(xaxiscut[1]-xaxiscut[0])*fs;
       QVector<double> buffer;
       for (int i=0; i< BUF_SIZE; i++)
       {   buffer.append(ymain[i+(int)(xaxiscut[0]*fs)]);
       }
       qDebug()<<BUF_SIZE<<xaxiscut[0]*fs<<xaxiscut[1]*fs;
       QString current_path=QDir::currentPath();
       QString path_base=current_path+"/../";
       QString namecut=path_base+"/test.wav";
       QString comandcut="python "+path_base+"cutAudio.py "+namemain+" "+namecut+" "+QString::number(xaxiscut[0])+" "+QString::number(xaxiscut[1]);
       qDebug()<< comandcut;
       cmd.start(comandcut);
       cmd.waitForFinished(-1);
       flagCuted=1;
   }
   ;
}


void MainWindow::opendialogb(int task_ad){
    dialogb=new Dialogb(this, task_ad);
    dialogb->show();
}

void MainWindow::on_pushButton_clicked()
{
    QProcess cmd;
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    QPen pen;

    QStringList ls;
    QString va;
    qDebug()<<path_patient;
    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../phonVowels/";

    if ((flagCuted==0) && (!QDir(path_patient).exists() || path_patient=="")){
        ls = QFileDialog::getOpenFileNames(this, chooseA, "", "Audio-Files(*.wav)");
        va=ls[0];
    }
    else if (flagCuted==1){va=current_path+"/../test.wav"; }
    else{
        va=path_patient+"/A.wav";
    }

    if (flagCuted==1){flagCuted=0;}
    file_2play=va;

    QString comandpv="python "+path_base+phonScript+va+" "+path_base+"featf0.txt "+path_base+"feat.txt "+path_base;
    cmd.start(comandpv);
    cmd.waitForFinished(-1);




    qDebug() << comandpv;

    QVector<double> y=readWavFile(va);
    QVector<double> x((int)y.size());
    for (int i=0; i<(int)y.size(); i++)
    {
      x[i] = ((double)i)/fs;
    }
    ui->customPlot_2->clearGraphs();
    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph()->setData(x,y);
    ui->customPlot_2->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_2->yAxis->setRange(-1, 1);
    ui->customPlot_2->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_2->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_2->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_2->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_2->xAxis->setLabel(xaxistime);
    ui->customPlot_2->yAxis->setLabel(yaxisA);
    ui->customPlot_2->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_2->graph()->setPen(pen);
    ui->customPlot_2->replot();
    ui->customPlot_2->savePng(current_path+"/../phonation1.png");

    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setData(x,y);
    ui->customPlot->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot->yAxis->setRange(-1, 1);
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->xAxis->setLabel(xaxistime);
    ui->customPlot->yAxis->setLabel(yaxisA);
    ui->customPlot->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->graph()->setPen(pen);
    ui->customPlot->replot();
    xmain=x;
    ymain=y;
    namemain=va;

    QVector<double> F0;
    QVector<double> time;
    QFile fileF0(path_base+"featf0.txt");
    qDebug() << "fileF0: " << path_base+"featf0.txt";
    if (!fileF0.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << "Only lecture fileF0: " << path_base+"featf0.txt";
        return;
    int nframes=0;
    double maxF0=0;
    while (!fileF0.atEnd()) {
        QString line = fileF0.readLine();
        F0.append(line.toDouble());
        if (line.toDouble()>maxF0){maxF0=line.toDouble();}
        time.append((double)nframes/100);
        nframes++;
    }
    qDebug() << "Nframes: " << nframes;
    ui->customPlot_3->clearGraphs();
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph()->setData(time,F0);
    ui->customPlot_3->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_3->yAxis->setRange(0, maxF0+5);
    ui->customPlot_3->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->xAxis->setLabel(xaxistime);
    ui->customPlot_3->yAxis->setLabel(yaxisfreq);
    ui->customPlot_3->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_3->graph()->setPen(pen);
    ui->customPlot_3->replot();
    ui->customPlot_3->savePng(current_path+"/../phonation2.png");
    QList<QString> feat;
    QFile filefeat(path_base+"feat.txt");
    qDebug() << "filefeat: " << path_base+"feat.txt";
    if (!filefeat.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << "filefeat read only";
        return;
    while (!filefeat.atEnd()) {
        QString line = filefeat.readLine();
        feat.append(line);
    }

    QString jitter=feat[0];
    QString shimmer=feat[1];
    QString APQ=feat[2];
    QString PPQ=feat[3];
    QString logE=feat[4];
    QString degU=feat[6];
    QString varF0=feat[7];
    ui->label_8->setText(jitter);
    ui->label_9->setText(shimmer);
    ui->label_10->setText(APQ);
    ui->label_11->setText(PPQ);
    //ui->label_12->setText(logE);
    ui->label_14->setText(degU);
    ui->label_184->setText(varF0);

    if (ref_female){
        if (jitter.toDouble()>jitterref_f){ui->label_8->setPalette(sample_palette);}
        if (shimmer.toDouble()>shimmerrefu_f  || shimmer.toDouble()<shimmerrefd_f){ui->label_9->setPalette(sample_palette);}
        if (APQ.toDouble()>APQrefu_f  || APQ.toDouble()<APQrefd_f){ui->label_10->setPalette(sample_palette);}
        if (PPQ.toDouble()>PPQrefu_f  || PPQ.toDouble()<PPQrefd_f){ui->label_11->setPalette(sample_palette);}
        //if (logE.toDouble()>logErefu_f  || logE.toDouble()<logErefd_f){ui->label_12->setPalette(sample_palette);}
        if (degU.toDouble()>degUrefu_f  || degU.toDouble()<degUrefd_f){ui->label_14->setPalette(sample_palette);}
        if (varF0.toDouble()>varf0refu_f  || varF0.toDouble()<varf0refd_f){ui->label_14->setPalette(sample_palette);}
        ui->label_93->setText(QString{ "0.00->%1" }.arg( jitterref_f));
        ui->label_94->setText(QString{ "%1->%2" }.arg( shimmerrefd_f).arg(shimmerrefu_f));
        ui->label_95->setText(QString{ "%1->%2" }.arg( APQrefd_f).arg(APQrefu_f));
        ui->label_96->setText(QString{ "%1->%2" }.arg( PPQrefd_f).arg(PPQrefu_f));
        //ui->label_97->setText(QString{ "%1->%2" }.arg( logErefd_f).arg(logErefu_f));
        ui->label_181->setText(QString{ "%1->%2" }.arg( degUrefd_f).arg(degUrefu_f));
        ui->label_183->setText(QString{ "%1->%2" }.arg( varf0refd_f).arg(varf0refu_f));
    }
    else{
        if (jitter.toDouble()>jitterref_m){ui->label_8->setPalette(sample_palette);}
        if (shimmer.toDouble()>shimmerrefu_m  || shimmer.toDouble()<shimmerrefd_m){ui->label_9->setPalette(sample_palette);}
        if (APQ.toDouble()>APQrefu_m  || APQ.toDouble()<APQrefd_m){ui->label_10->setPalette(sample_palette);}
        if (PPQ.toDouble()>PPQrefu_m  || PPQ.toDouble()<PPQrefd_m){ui->label_11->setPalette(sample_palette);}
       // if (logE.toDouble()>logErefu_m  || logE.toDouble()<logErefd_m){ui->label_12->setPalette(sample_palette);}
        if (degU.toDouble()>degUrefu_m  || degU.toDouble()<degUrefd_m){ui->label_14->setPalette(sample_palette);}
        if (varF0.toDouble()>varf0refu_m  || varF0.toDouble()<varf0refd_m){ui->label_14->setPalette(sample_palette);}
        ui->label_93->setText(QString{ "0.00->%1" }.arg( jitterref_m));
        ui->label_94->setText(QString{ "%1->%2" }.arg( shimmerrefd_m).arg(shimmerrefu_m));
        ui->label_95->setText(QString{ "%1->%2" }.arg( APQrefd_m).arg(APQrefu_m));
        ui->label_96->setText(QString{ "%1->%2" }.arg( PPQrefd_m).arg(PPQrefu_m));
       // ui->label_97->setText(QString{ "%1->%2" }.arg( logErefd_m).arg(logErefu_m));
        ui->label_181->setText(QString{ "%1->%2" }.arg( degUrefd_m).arg(degUrefu_m));
        ui->label_183->setText(QString{ "%1->%2" }.arg( varf0refd_m).arg(varf0refu_m));
    }

    xrange=ui->customPlot->xAxis->range();
    if (ref_female){ui->radarPhon->setPixmap(QPixmap( path_base+"phonation2female.png" ) );}
    if (ref_male){ui->radarPhon->setPixmap(QPixmap( path_base+"phonation2male.png" ) );}

    ui->radarPhon->setScaledContents(true);
    ui->radarPhon->show();
}

void MainWindow::on_pushButton_2_clicked()
{

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    QProcess cmd;
    QStringList lsa, lse, lsi, lso, lsu, lsm;
    QString va, ve, vi, vo, vu, monologue;
    qDebug()<<path_patient;
    QString current_path=QDir::currentPath();
    if (flagCuted==0){
    if (!QDir(path_patient).exists() || path_patient==""){
        lsa = QFileDialog::getOpenFileNames(this, chooseA, "", "Audio-Files(*.wav)");
        lse = QFileDialog::getOpenFileNames(this, chooseE, "", "Audio-Files(*.wav)");
        lsi = QFileDialog::getOpenFileNames(this, chooseI, "", "Audio-Files(*.wav)");
        lso = QFileDialog::getOpenFileNames(this, chooseO, "", "Audio-Files(*.wav)");
        lsu = QFileDialog::getOpenFileNames(this, chooseU, "", "Audio-Files(*.wav)");
        lsm = QFileDialog::getOpenFileNames(this, choosecont, "", "Audio-Files(*.wav)");
        va=lsa[0];
        ve=lse[0];
        vi=lsi[0];
        vo=lso[0];
        vu=lsu[0];
        monologue=lsm[0];
    }
    else{
        va=path_patient+"/A.wav";
        ve=path_patient+"/E.wav";
        vi=path_patient+"/I.wav";
        vo=path_patient+"/O.wav";
        vu=path_patient+"/U.wav";
        monologue=path_patient+"/Monologue.wav";
    }}
    else {monologue=current_path+"/../test.wav";}
    if (flagCuted==1){flagCuted=0;}

    file_2play=monologue;
    QPen pen;

    QVector<double> y=readWavFile(monologue);
    QVector<double> xt((int)y.size());
    for (int i=0; i<(int)y.size(); i++)
    {
      xt[i] = ((double)i)/fs;
    }
    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setData(xt,y);
    ui->customPlot->xAxis->setRange(0, (double)xt.size()/fs);
    ui->customPlot->yAxis->setRange(-1, 1);
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->xAxis->setLabel(xaxistime);
    ui->customPlot->yAxis->setLabel(yaxisA);
    ui->customPlot->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->graph()->setPen(pen);

    ui->customPlot->replot();
    xmain=xt;
    ymain=y;
    namemain=monologue;

    QString path_base=current_path+"/../artVowels/";
    QString comandpv="python "+path_base+artVowelsScript+va+" "+ve+" "+vi+" "+vo+" "+vu+" "+path_base+"Fa.txt "+path_base+"Fe.txt "+path_base+"Fi.txt "+path_base+"Fo.txt "+path_base+"Fu.txt "+path_base+"feat.txt "+path_base;
    qDebug()<< comandpv;
    cmd.start(comandpv);
    cmd.waitForFinished(-1);

    QVector<double> F1a=readFormants(path_base+"Fa1.txt");
    QVector<double> F2a=readFormants(path_base+"Fa2.txt");
    QVector<double> F1e=readFormants(path_base+"Fe1.txt");
    QVector<double> F2e=readFormants(path_base+"Fe2.txt");
    QVector<double> F1i=readFormants(path_base+"Fi1.txt");
    QVector<double> F2i=readFormants(path_base+"Fi2.txt");
    QVector<double> F1o=readFormants(path_base+"Fo1.txt");
    QVector<double> F2o=readFormants(path_base+"Fo2.txt");
    QVector<double> F1u=readFormants(path_base+"Fu1.txt");
    QVector<double> F2u=readFormants(path_base+"Fu2.txt");

    QList<QString> feat;
    QFile filefeat(path_base+"feat.txt");
    if (!filefeat.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    while (!filefeat.atEnd()) {
        QString line = filefeat.readLine();
        feat.append(line);
    }

    QString VSA=feat[0];
    QString FCR=feat[1];
    QString VPA=feat[2];
    QString mF1a=feat[3];
    QString mF2a=feat[4];
    QString mF1e=feat[5];
    QString mF2e=feat[6];
    QString mF1i=feat[7];
    QString mF2i=feat[8];
    QString mF1o=feat[9];
    QString mF2o=feat[10];
    QString mF1u=feat[11];
    QString mF2u=feat[12];

    ui->label_23->setText(mF1a);
    ui->label_17->setText(mF2a);
    ui->label_25->setText(mF1i);
    ui->label_16->setText(mF2i);
    ui->label_18->setText(mF1u);
    ui->label_20->setText(mF2u);
    ui->label_28->setText(VSA);
    ui->label_29->setText(VPA);
    ui->label_31->setText(FCR);



    if (ref_female){
        if (VSA.toDouble()>vsau_f  || VSA.toDouble()<vsad_f){ui->label_28->setPalette(sample_palette);}
        if (FCR.toDouble()>fcru_f  || FCR.toDouble()<fcrd_f){ui->label_31->setPalette(sample_palette);}
        if (VPA.toDouble()>vpau_f  || VPA.toDouble()<vpad_f){ui->label_29->setPalette(sample_palette);}
        if (mF1a.toDouble()>f1au_f  || mF1a.toDouble()<f1ad_f){ui->label_23->setPalette(sample_palette);}
        if (mF2a.toDouble()>f2au_f  || mF2a.toDouble()<f2ad_f){ui->label_17->setPalette(sample_palette);}
        if (mF1i.toDouble()>f1iu_f  || mF1i.toDouble()<f1id_f){ui->label_25->setPalette(sample_palette);}
        if (mF2i.toDouble()>f2iu_f  || mF2i.toDouble()<f2id_f){ui->label_16->setPalette(sample_palette);}
        if (mF1u.toDouble()>f1uu_f  || mF1u.toDouble()<f1ud_f){ui->label_18->setPalette(sample_palette);}
        if (mF2u.toDouble()>f2uu_f  || mF2u.toDouble()<f2ud_f){ui->label_20->setPalette(sample_palette);}
        ui->label_104->setText(QString{ "%1->%2" }.arg( vsad_f).arg(vsau_f));
        ui->label_106->setText(QString{ "%1->%2" }.arg( fcrd_f).arg(fcru_f));
        ui->label_105->setText(QString{ "%1->%2" }.arg( vpad_f).arg(vpau_f));
        ui->label_98->setText(QString{ "%1->%2" }.arg( f1ad_f).arg(f1au_f));
        ui->label_99->setText(QString{ "%1->%2" }.arg( f2ad_f).arg(f2au_f));
        ui->label_100->setText(QString{ "%1->%2" }.arg( f1id_f).arg(f1iu_f));
        ui->label_101->setText(QString{ "%1->%2" }.arg( f2id_f).arg(f2iu_f));
        ui->label_102->setText(QString{ "%1->%2" }.arg( f1ud_f).arg(f1uu_f));
        ui->label_103->setText(QString{ "%1->%2" }.arg( f2ud_f).arg(f2uu_f));


    }
    else{
        if (VSA.toDouble()>vsau_m  || VSA.toDouble()<vsad_m){ui->label_28->setPalette(sample_palette);}
        if (FCR.toDouble()>fcru_m  || FCR.toDouble()<fcrd_m){ui->label_31->setPalette(sample_palette);}
        if (VPA.toDouble()>vpau_m  || VPA.toDouble()<vpad_m){ui->label_29->setPalette(sample_palette);}
        if (mF1a.toDouble()>f1au_m  || mF1a.toDouble()<f1ad_m){ui->label_23->setPalette(sample_palette);}
        if (mF2a.toDouble()>f2au_m  || mF2a.toDouble()<f2ad_m){ui->label_17->setPalette(sample_palette);}
        if (mF1i.toDouble()>f1iu_m  || mF1i.toDouble()<f1id_m){ui->label_25->setPalette(sample_palette);}
        if (mF2i.toDouble()>f2iu_m  || mF2i.toDouble()<f2id_m){ui->label_16->setPalette(sample_palette);}
        if (mF1u.toDouble()>f1uu_m  || mF1u.toDouble()<f1ud_m){ui->label_18->setPalette(sample_palette);}
        if (mF2u.toDouble()>f2uu_m  || mF2u.toDouble()<f2ud_m){ui->label_20->setPalette(sample_palette);}
        ui->label_104->setText(QString{ "%1->%2" }.arg( vsad_m).arg(vsau_m));
        ui->label_106->setText(QString{ "%1->%2" }.arg( fcrd_m).arg(fcru_m));
        ui->label_105->setText(QString{ "%1->%2" }.arg( vpad_m).arg(vpau_m));
        ui->label_98->setText(QString{ "%1->%2" }.arg( f1ad_m).arg(f1au_m));
        ui->label_99->setText(QString{ "%1->%2" }.arg( f2ad_m).arg(f2au_m));
        ui->label_100->setText(QString{ "%1->%2" }.arg( f1id_m).arg(f1iu_m));
        ui->label_101->setText(QString{ "%1->%2" }.arg( f2id_m).arg(f2iu_m));
        ui->label_102->setText(QString{ "%1->%2" }.arg( f1ud_m).arg(f1uu_m));
        ui->label_103->setText(QString{ "%1->%2" }.arg( f2ud_m).arg(f2uu_m));
    }


    QVector<double> F1;
    double F1ad=mF1a.toDouble();
    double F1id=mF1i.toDouble();
    double F1ud=mF1u.toDouble();
    F1={F1ad, F1id, F1ud, F1ad, F1id};
    QVector<double> F2;
    double F2ad=mF2a.toDouble();
    double F2id=mF2i.toDouble();
    double F2ud=mF2u.toDouble();
    F2={F2ad, F2id, F2ud, F2ad, F2id};

    ui->customPlot_4->legend->setVisible(true);
    ui->customPlot_4->clearGraphs();
    ui->customPlot_4->addGraph();
    ui->customPlot_4->xAxis->setRange(0, 1000);
    ui->customPlot_4->yAxis->setRange(0, 3000);
    ui->customPlot_4->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_4->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_4->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_4->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_4->xAxis->setLabel(xaxisF1);
    ui->customPlot_4->yAxis->setLabel(yaxisF2);
    ui->customPlot_4->graph()->setData(F1a,F2a);
    ui->customPlot_4->graph(0)->setName("/A/");
    pen.setColor(QColor(255,0,0));
    ui->customPlot_4->graph()->setLineStyle(QCPGraph::lsNone);
    ui->customPlot_4->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->customPlot_4->graph()->setPen(pen);
    ui->customPlot_4->addGraph();
    ui->customPlot_4->graph(1)->setName("/I/");
    ui->customPlot_4->graph()->setData(F1i,F2i);
    pen.setColor(QColor(0,255,0));
    ui->customPlot_4->graph()->setLineStyle(QCPGraph::lsNone);
    ui->customPlot_4->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->customPlot_4->graph()->setPen(pen);
    ui->customPlot_4->addGraph();
    ui->customPlot_4->graph(2)->setName("/U/");
    ui->customPlot_4->graph()->setData(F1u,F2u);
    pen.setColor(QColor(0,0,255));
    ui->customPlot_4->graph()->setLineStyle(QCPGraph::lsNone);
    ui->customPlot_4->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->customPlot_4->graph()->setPen(pen);
    ui->customPlot_4->legend->setVisible(false);
    ui->customPlot_4->addGraph();
    ui->customPlot_4->graph()->setData({F1ad, F1id},{F2ad, F2id});
    pen.setColor(QColor(0,0,0));
    ui->customPlot_4->graph()->setPen(pen);
    ui->customPlot_4->addGraph();
    ui->customPlot_4->graph()->setData({F1id, F1ud},{F2id, F2ud});
    pen.setColor(QColor(0,0,0));
    ui->customPlot_4->graph()->setPen(pen);
    ui->customPlot_4->addGraph();
    ui->customPlot_4->graph()->setData({F1ud, F1ad},{F2ud, F2ad});
    pen.setColor(QColor(0,0,0));
    ui->customPlot_4->graph()->setPen(pen);
    ui->customPlot_4->setBackground(plotGradient);
    ui->customPlot_4->replot();
    ui->customPlot_4->savePng(current_path+"/../articulation1.png");

    double F1ed=mF1e.toDouble();
    double F1od=mF1o.toDouble();
    F1={F1ad, F1ed, F1id, F1ud, F1od, F1ad, F1ed};
    double F2ed=mF2e.toDouble();
    double F2od=mF2o.toDouble();
    F2={F2ad, F2ed, F2id, F2ud, F2od, F2ad, F2ed};
    ui->customPlot_5->clearGraphs();
    ui->customPlot_5->legend->setVisible(true);
    ui->customPlot_5->addGraph();
    ui->customPlot_5->xAxis->setRange(0, 1000);
    ui->customPlot_5->yAxis->setRange(0, 3000);
    ui->customPlot_5->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_5->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_5->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_5->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_5->xAxis->setLabel(xaxisF1);
    ui->customPlot_5->yAxis->setLabel(yaxisF2);
    ui->customPlot_5->graph()->setData(F1a,F2a);
    ui->customPlot_5->graph(0)->setName("/A/");
    pen.setColor(QColor(255,0,0));
    ui->customPlot_5->graph()->setLineStyle(QCPGraph::lsNone);
    ui->customPlot_5->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->addGraph();
    ui->customPlot_5->graph()->setData(F1e,F2e);
    pen.setColor(QColor(255,255,0));
    ui->customPlot_5->graph()->setLineStyle(QCPGraph::lsNone);
    ui->customPlot_5->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->graph(1)->setName("/E/");
    ui->customPlot_5->addGraph();
    ui->customPlot_5->graph()->setData(F1i,F2i);
    ui->customPlot_5->graph(2)->setName("/I/");
    pen.setColor(QColor(0,255,0));
    ui->customPlot_5->graph()->setLineStyle(QCPGraph::lsNone);
    ui->customPlot_5->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->addGraph();
    ui->customPlot_5->graph(3)->setName("/O/");
    ui->customPlot_5->graph()->setData(F1o,F2o);
    pen.setColor(QColor(0,255,255));
    ui->customPlot_5->graph()->setLineStyle(QCPGraph::lsNone);
    ui->customPlot_5->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->addGraph();
    ui->customPlot_5->graph()->setData(F1u,F2u);
    ui->customPlot_5->graph(4)->setName("/U/");
    pen.setColor(QColor(0,0,255));
    ui->customPlot_5->graph()->setLineStyle(QCPGraph::lsNone);
    ui->customPlot_5->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->addGraph();
    ui->customPlot_5->graph()->setData({F1ad, F1ed},{F2ad, F2ed});
    pen.setColor(QColor(0,0,0));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->addGraph();
    ui->customPlot_5->graph()->setData({F1ed, F1id},{F2ed, F2id});
    pen.setColor(QColor(0,0,0));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->addGraph();
    ui->customPlot_5->graph()->setData({F1id, F1ud},{F2id, F2ud});
    pen.setColor(QColor(0,0,0));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->addGraph();
    ui->customPlot_5->graph()->setData({F1ud, F1od},{F2ud, F2od});
    pen.setColor(QColor(0,0,0));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->addGraph();
    ui->customPlot_5->graph()->setData({F1od, F1ad},{F2od, F2ad});
    pen.setColor(QColor(0,0,0));
    ui->customPlot_5->graph()->setPen(pen);
    ui->customPlot_5->legend->setVisible(false);
    ui->customPlot_5->setBackground(plotGradient);
    ui->customPlot_5->replot();

    current_path=QDir::currentPath();
    path_base=current_path+"/../artCont/";
    comandpv="python "+path_base+artContScript+monologue+" "+path_base+"feat.txt "+path_base;
    qDebug()<< comandpv;
    cmd.start(comandpv);
    cmd.waitForFinished(-1);

    if(ref_female){
        ui->radarArt->setPixmap(QPixmap( path_base+"articulationfemale.png" ) );
    }
    else{
        ui->radarArt->setPixmap(QPixmap( path_base+"articulationmale.png" ) );
    }
    ui->radarArt->setScaledContents(true);
    ui->radarArt->show();

    QVector<double> BBE;
    QFile fileF(path_base+"feat.txt");
    if (!fileF.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    while (!fileF.atEnd()) {
        QString line = fileF.readLine();
        BBE.append(line.toDouble());
    }
    QVector<double> BBEonm, BBEons, BBEofm, BBEofs, x;
    for (int nb=0;nb<nBBE; nb++){
        BBEonm.append(BBE[nb]);
        BBEons.append(BBE[nb+25]);
        BBEofm.append(BBE[nb+50]);
        BBEofs.append(BBE[nb+75]);
        x.append(nb+1);
    }
    ui->customPlot_6->clearGraphs();
    ui->customPlot_6->legend->setVisible(true);
    ui->customPlot_6->addGraph();
    ui->customPlot_6->xAxis->setRange(0, 23);
    ui->customPlot_6->yAxis->setRange(0, 12);
    ui->customPlot_6->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_6->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_6->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_6->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_6->xAxis->setLabel(xaxisBBE);
    ui->customPlot_6->yAxis->setLabel(yaxislogE);
    ui->customPlot_6->graph(0)->setDataValueError(x, BBEonm, BBEons);
    ui->customPlot_6->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    ui->customPlot_6->graph(0)->setPen(QPen(QColor(0, 120, 0), 2));
    ui->customPlot_6->graph(0)->setErrorType(QCPGraph::etValue);
    ui->customPlot_6->graph(0)->setName(Onset);
    ui->customPlot_6->addGraph();
    ui->customPlot_6->graph(1)->setDataValueError(x, BBEofm, BBEofs);
    ui->customPlot_6->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::red, 1.5), QBrush(Qt::white), 9));
    ui->customPlot_6->graph(1)->setPen(QPen(QColor(0, 180, 0), 2));
    ui->customPlot_6->graph(1)->setErrorType(QCPGraph::etValue);
    ui->customPlot_6->graph(1)->setName(Offset);

    QVector<double> Ticks;
    QVector<QString> Labels;
    for(int i=0;i<11;i++){
    Ticks.append((double)i*2);
    Labels.append(QString::number(i*2));
    }
    ui->customPlot_6->xAxis->setAutoTicks(false);
    ui->customPlot_6->xAxis->setAutoTickLabels(false);
    ui->customPlot_6->xAxis->setTickVector(Ticks);
    ui->customPlot_6->xAxis->setTickVectorLabels(Labels);
    ui->customPlot_6->setBackground(plotGradient);
    ui->customPlot_6->replot();
    ui->customPlot_6->savePng(current_path+"/../articulation2.png");
    xrange=ui->customPlot->xAxis->range();
}


QVector<double> MainWindow::readFormants(QString nameFile){

QVector<double> Formant;
QFile fileF(nameFile);
qDebug() << nameFile;
if (!fileF.open(QIODevice::ReadOnly | QIODevice::Text))
    return Formant;
while (!fileF.atEnd()) {
    QString line = fileF.readLine();
    Formant.append(line.toDouble());
}
return Formant;
}


void MainWindow::on_pushButton_5_toggled(bool checked)
{
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));
    QPen pen;
    ui->customPlot->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->replot();

    QDateTime c_date(QDateTime::currentDateTime());
    QString c_datestr=c_date.toString("yyyy_MM_dd");
    int current_row=ui->listWidget->currentRow();
    namePatient=ui->textEdit->toPlainText();
    lastPatient=ui->textEdit_2->toPlainText();
    QString current_path=QDir::currentPath();
    path_patient=current_path+"/../../data/"+c_datestr+lastPatient+namePatient;
    if (!QDir(path_patient).exists()){
        QDir().mkdir(path_patient);
    }

    if (checked){

        if (task_AD){
            int current_row=ui->listWidget->currentRow();
            if (current_row==0 || current_row==16 || current_row==17){
                opendialogb(current_row); // open window for image description
            }

            if (current_row >= 6 && current_row <=13){
                QString current_path=QDir::currentPath();
                QString path_base=current_path+"/../audio_examples/";
                QString rowstr;
                QString file_play="";
                file_play=path_base+'a'+rowstr.number(current_row)+".wav";
                player->setMedia(QUrl::fromLocalFile(file_play));
                player->setVolume(50);
                player->play();
            }

        }



        QString name=get_name(current_row, path_patient, true);
        QStringList codecName=audioRecorder->supportedAudioCodecs();
        qDebug() <<codecName;
        QStringList inputs=audioRecorder->audioInputs();
        if (audioRecorder->state() == QMediaRecorder::StoppedState) {
            QAudioEncoderSettings settings;
            audioRecorder->setAudioInput(inputs[0]);

            #if (defined (_WIN32) || defined (_WIN64))
                settings.setCodec("audio/pcm");
            #elif (defined (LINUX) || defined (__linux__))
                settings.setCodec("audio/x-wma, wmaversion=(int)2");
            #endif


            settings.setSampleRate(fs);
            settings.setBitRate(16);
            settings.setChannelCount(1);
            settings.setQuality(QMultimedia::HighQuality);
            settings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);

            audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(),"audio/x-wav");
            audioRecorder->setContainerFormat("audio/x-wav");
            audioRecorder->setOutputLocation(QUrl::fromLocalFile(name));
            audioRecorder->record();
        }

    }
    else{
        QString name=get_name(current_row, path_patient, false);
        audioRecorder->stop();
        QVector<double>y=readWavFile(name);
        file_2play=name;
        QVector<double> x((int)y.size());
        int contSat=0;
        for (int i=0; i<(int)y.size(); i++){
              x[i] = ((double)i)/fs;
              if (y[i]>0.9){
                  contSat++;
              }
        }

        ui->customPlot->clearGraphs();
        ui->customPlot->addGraph();
        ui->customPlot->graph()->setData(x,y);
        ui->customPlot->xAxis->setRange(0, (double)x.size()/fs);
        ui->customPlot->yAxis->setRange(-1, 1);
        ui->customPlot->xAxis->setLabel(xaxistime);
        ui->customPlot->yAxis->setLabel(yaxisA);
        ui->customPlot->graph()->setPen(pen);
        ui->customPlot->replot();
        xmain=x;
        ymain=y;
        if(contSat>(int)(y.size()*0.2)){
            QMessageBox::information(
                    this,
                    tr("Neuro-Speech"),
                    tr(repeat) );
        }

    }
    xrange=ui->customPlot->xAxis->range();
}


void MainWindow::on_pushButton_6_toggled(bool checked)
{   if (checked){int a=1;};
}


void MainWindow::on_pushButton_4_clicked()
{
    QProcess cmd;
    QStringList ls1, ls2, ls3, ls4, ls5, ls6, ls7, ls8, ls9, ls10, lrt;
    QString sent1, sent2, sent3, sent4, sent5, sent6, sent7, sent8, sent9, sent10, readtext;
    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../intelligibility/";

    if (!QDir(path_patient).exists() || path_patient=="") {
        ls1 = QFileDialog::getOpenFileNames(this, chooses1, "", "Audio-Files(*.wav)");
        ls2 = QFileDialog::getOpenFileNames(this, chooses2, "", "Audio-Files(*.wav)");
        ls3 = QFileDialog::getOpenFileNames(this, chooses3, "", "Audio-Files(*.wav)");
        ls4 = QFileDialog::getOpenFileNames(this, chooses4, "", "Audio-Files(*.wav)");
        ls5 = QFileDialog::getOpenFileNames(this, chooses5, "", "Audio-Files(*.wav)");
        ls6 = QFileDialog::getOpenFileNames(this, chooses6, "", "Audio-Files(*.wav)");
        ls7 = QFileDialog::getOpenFileNames(this, chooses7, "", "Audio-Files(*.wav)");
        ls8 = QFileDialog::getOpenFileNames(this, chooses8, "", "Audio-Files(*.wav)");
        ls9 = QFileDialog::getOpenFileNames(this, chooses9, "", "Audio-Files(*.wav)");
        ls10 = QFileDialog::getOpenFileNames(this,chooses10, "", "Audio-Files(*.wav)");
        lrt = QFileDialog::getOpenFileNames(this, choosesRT, "", "Audio-Files(*.wav)");
        sent1=ls1[0];
        sent2=ls2[0];
        sent3=ls3[0];
        sent4=ls4[0];
        sent5=ls5[0];
        sent6=ls6[0];
        sent7=ls7[0];
        sent8=ls8[0];
        sent9=ls9[0];
        sent10=ls10[0];
        readtext=lrt[0];
    }
    else{
        sent1=path_patient+"/sent1.wav";
        sent2=path_patient+"/sent2.wav";
        sent3=path_patient+"/sent3.wav";
        sent4=path_patient+"/sent4.wav";
        sent5=path_patient+"/sent5.wav";
        sent6=path_patient+"/sent6.wav";
        sent7=path_patient+"/sent7.wav";
        sent8=path_patient+"/sent8.wav";
        sent9=path_patient+"/sent9.wav";
        sent10=path_patient+"/sent10.wav";
        readtext=path_patient+"/Readtext.wav";
    }
    file_2play=readtext;
    QString comand1="python "+path_base+intScript+sent1+" "+path_base+"mi_casa.txt "+path_base+"pred1.txt "+path_base+"feat1.txt es_CO";
    qDebug() << comand1;
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+sent2+" "+path_base+"omar.txt "+path_base+"pred2.txt "+path_base+"feat2.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+sent3+" "+path_base+"laura.txt "+path_base+"pred3.txt "+path_base+"feat3.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+sent4+" "+path_base+"loslibros.txt "+path_base+"pred4.txt "+path_base+"feat4.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+sent5+" "+path_base+"rosita.txt "+path_base+"pred5.txt "+path_base+"feat5.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+sent6+" "+path_base+"luisa.txt "+path_base+"pred6.txt "+path_base+"feat6.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+sent7+" "+path_base+"viste.txt "+path_base+"pred7.txt "+path_base+"feat7.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+sent8+" "+path_base+"juan.txt "+path_base+"pred8.txt "+path_base+"feat8.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+sent9+" "+path_base+"triste.txt "+path_base+"pred9.txt "+path_base+"feat9.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+sent10+" "+path_base+"preocupado.txt "+path_base+"pred10.txt "+path_base+"feat10.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intScript+readtext+" "+path_base+"readtext.txt "+path_base+"pred11.txt "+path_base+"feat11.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);
    comand1="python "+path_base+intradar+path_base+"feat1.txt "+path_base+"feat2.txt "+path_base+"feat3.txt "+path_base+"feat4.txt "+path_base+"feat5.txt "+path_base+"feat6.txt "+path_base+"feat7.txt "+path_base+"feat8.txt "+path_base+"feat9.txt "+path_base+"feat10.txt "+path_base+"feat11.txt";
    qDebug() << comand1;
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    QStringList predicted;
    QStringList WA, DTW;
    QString cfstr;
    QString line;
    for (int cf=1;cf<=11;cf++){
       cfstr = tr("%1").arg(cf);
       QFile file_pred(path_base+"pred"+cfstr+".txt");
       QFile file_feat(path_base+"feat"+cfstr+".txt");
        if (!file_pred.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        while (!file_pred.atEnd()) {
            line = file_pred.readLine();
             qDebug() << "filepred: " << path_base+"pred"+cfstr+".txt";
             qDebug() << "pred: " <<line;
            predicted.append(line);

        }

        if (!file_feat.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        int nline=0;
        while (!file_feat.atEnd()) {
            line = file_feat.readLine();
            qDebug() << "filefeat: " << path_base+"feat"+cfstr+".txt";
            qDebug() << "feat: " <<line;
            if (nline==0)
                WA.append(line);
            else DTW.append(line);
            nline++;
        }

    }

    qDebug() <<predicted;
    qDebug() <<WA;
    qDebug() <<DTW;
    ui->label_34->setText(predicted[0]);
    ui->label_44->setText(predicted[1]);
    ui->label_47->setText(predicted[2]);
    ui->label_51->setText(predicted[3]);
    ui->label_53->setText(predicted[4]);
    ui->label_57->setText(predicted[5]);
    ui->label_146->setText(predicted[6]);
    ui->label_150->setText(predicted[7]);
    ui->label_151->setText(predicted[8]);
    ui->label_157->setText(predicted[9]);
    if (predicted.size()==14){
    ui->label_61->setText(predicted[10]+'\r'+predicted[11]+'\r'+predicted[12]+'\r'+predicted[13]);}
    else {
            ui->label_61->setText(predicted[10]+'\r'+predicted[11]+'\r'+predicted[12]);
    }
    ui->label_38->setText(WA[0]);
    ui->label_41->setText(WA[1]);
    ui->label_45->setText(WA[2]);
    ui->label_50->setText(WA[3]);
    ui->label_54->setText(WA[4]);
    ui->label_59->setText(WA[5]);
    ui->label_59->setText(WA[5]);
    ui->label_138->setText(WA[6]);
    ui->label_147->setText(WA[7]);
    ui->label_152->setText(WA[8]);
    ui->label_155->setText(WA[9]);
    ui->label_63->setText(WA[10]);
    ui->label_40->setText(DTW[0]);
    ui->label_42->setText(DTW[1]);
    ui->label_46->setText(DTW[2]);
    ui->label_52->setText(DTW[3]);
    ui->label_56->setText(DTW[4]);
    ui->label_60->setText(DTW[5]);
    ui->label_139->setText(DTW[6]);
    ui->label_149->setText(DTW[7]);
    ui->label_153->setText(DTW[8]);
    ui->label_158->setText(DTW[9]);
    ui->label_64->setText(DTW[10]);
    WAavg=0;
    DTWavg=0;
    for  (int cj=0;cj<11;cj++){
        WAavg+=WA[cj].toFloat();
        DTWavg+=DTW[cj].toFloat();
    }

    WAavg=WAavg/11;
    DTWavg=DTWavg/11;
    qDebug() <<QString{ "%1" }.arg( WAavg);
    qDebug() <<QString{ "%1" }.arg( DTWavg);

    ui->label_190->setText(QString{ "%1" }.arg( WAavg));
    ui->label_191->setText(QString{ "%1" }.arg( DTWavg));
    qDebug() <<path_base;
    ui->radarInt->setPixmap(QPixmap( path_base+"intelligibility.png" ) );
    ui->radarInt->setScaledContents(true);
    ui->radarInt->show();
}

void MainWindow::on_pushButton_3_clicked()
{

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));
    QPen pen;

    QProcess cmd;
    QStringList ls;
    QString va;
    qDebug()<<path_patient;
    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../prosody/";

    if (flagCuted==0){
    if (!QDir(path_patient).exists() || path_patient==""){
        ls = QFileDialog::getOpenFileNames(this, choosecont, "", "Audio-Files(*.wav)");
        va=ls[0];
    }
    else{

        if (task_PD){
            va=path_patient+"/monologue.wav";
        }
        if (task_AD){
            va=path_patient+"/readtext.wav";
        }

    }}
    else{va=current_path+"/../test.wav";}
    if (flagCuted==1){flagCuted=0;}

    file_2play=va;



    QString comandPraat=current_path+"/../../Toolkits/praat "+path_base+"F0_Praat.praat "+va+ " "+ path_base+"tempF0.txt 75 500 0.02";
    cmd.start(comandPraat);
    cmd.waitForFinished(-1);

    qDebug() << comandPraat;




   comandPraat=current_path+"/../../Toolkits/praat "+path_base+"vuv.praat "+va+ " "+ path_base+"vuv.txt 75 600 0 0.02 0.01";

    cmd.start(comandPraat);
    cmd.waitForFinished(-1);

    qDebug() << comandPraat;

    QString comandpv="python "+path_base+prosScript+va+" "+path_base+"featf0.txt "+ path_base+"featEnergy.txt "+path_base+"feat.txt "+path_base;
    cmd.start(comandpv);
    cmd.waitForFinished(-1);

    qDebug() << comandpv;

    QVector<double> y=readWavFile(va);
    QVector<double> x((int)y.size());
    for (int i=0; i<(int)y.size(); i++){
      x[i] = ((double)i)/fs;
    }
     ui->customPlot_7->clearGraphs();
    ui->customPlot_7->addGraph();
    ui->customPlot_7->graph()->setData(x,y);
    ui->customPlot_7->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_7->yAxis->setRange(-1, 1);
    ui->customPlot_7->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_7->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_7->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_7->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_7->xAxis->setLabel(xaxistime);
    ui->customPlot_7->yAxis->setLabel(yaxisA);
    ui->customPlot_7->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_7->graph()->setPen(pen);
    ui->customPlot_7->replot();
    ui->customPlot_7->savePng(current_path+"/../prosody1.png");
    ui->radarPros->setPixmap(QPixmap( path_base+"prosodyradar.png" ) );
    ui->radarPros->setScaledContents(true);
    ui->radarPros->show();

    QList<QString> feat;
    QFile filefeat(path_base+"feat.txt");
    qDebug() << "filefeat: " << current_path+"feat.txt";
    if (!filefeat.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    while (!filefeat.atEnd()) {
        QString line = filefeat.readLine();
        feat.append(line);
    }
    QVector<double> F0;
    QVector<double> time;
    QFile fileF0(path_base+"featf0.txt");
    qDebug() << "fileF0: " << path_base+"featf0.txt";
    if (!fileF0.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    int nframes=0;
    QString mmF0=feat[2];
    while (!fileF0.atEnd()) {
        QString line = fileF0.readLine();
        F0.append(line.toDouble());
        time.append((double)nframes/50);
        nframes++;
    }
    qDebug() << "Nframes: " << nframes;
    ui->customPlot_8->clearGraphs();
    ui->customPlot_8->addGraph();
    ui->customPlot_8->graph()->setData(time,F0);
    ui->customPlot_8->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_8->yAxis->setRange(0, mmF0.toDouble());
    ui->customPlot_8->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_8->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_8->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_8->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_8->xAxis->setLabel(xaxistime);
    ui->customPlot_8->yAxis->setLabel(yaxisfreq);
    ui->customPlot_8->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_8->graph()->setPen(pen);
    ui->customPlot_8->replot();
    ui->customPlot_8->savePng(current_path+"/../prosody2.png");

    QVector<double> logE;
    QVector<double> timeEn;
    QFile filelogE(path_base+"featEnergy.txt");
    if (!filelogE.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    nframes=0;
    QString mmlogE=feat[5];
    while (!filelogE.atEnd()) {
      QString  line = filelogE.readLine();
        logE.append(line.toDouble());
        timeEn.append((double)nframes/100);
        nframes++;
    }
    qDebug() << "Nframes: " << nframes;
    ui->customPlot_9->clearGraphs();
    ui->customPlot_9->addGraph();
    ui->customPlot_9->graph()->setData(timeEn,logE);
    ui->customPlot_9->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_9->yAxis->setRange(0, qPow(10, 0.1*mmlogE.toDouble()));
    ui->customPlot_9->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_9->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_9->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_9->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_9->xAxis->setLabel(xaxistime);
    ui->customPlot_9->yAxis->setLabel(yaxislogE);
    ui->customPlot_9->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_9->graph()->setPen(pen);
    ui->customPlot_9->replot();
    ui->customPlot_9->savePng(current_path+"/../prosody3.png");
    QString mF0=feat[0];
    QString sF0=feat[1];
    QString mlogE=feat[3];
    QString slogE=feat[4];
    QString Vrate=feat[6];
    QString avgdurv=feat[7];
    QString stddurv=feat[8];
    QString Silrate=feat[9];
    QString avgdurs=feat[10];
    QString stddurs=feat[11];
    QString varf0semi=feat[12];
    ui->label_69->setText(Vrate);
    ui->label_70->setText(avgdurv);
    ui->label_71->setText(stddurv);
    ui->label_72->setText(Silrate);
    ui->label_73->setText(avgdurs);
    ui->label_74->setText(stddurs);
    ui->label_75->setText(mF0);
    ui->label_76->setText(sF0);
    ui->label_77->setText(mmF0);
    ui->label_187->setText(varf0semi);
    ui->label_78->setText(mlogE);
    ui->label_79->setText(slogE);
    ui->label_80->setText(mmlogE);
    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setData(x,y);
    ui->customPlot->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot->yAxis->setRange(-1, 1);
    ui->customPlot->xAxis->setLabel(xaxistime);
    ui->customPlot->yAxis->setLabel(yaxisA);
    ui->customPlot->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->graph()->setPen(pen);
    xmain=x;
    ymain=y;
    ui->customPlot->replot();
    xrange=ui->customPlot->xAxis->range();
    namemain=va;
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);


    QPalette sample_palette_blue;
    sample_palette_blue.setColor(QPalette::Window, Qt::white);
    sample_palette_blue.setColor(QPalette::WindowText, Qt::blue);

    QString agestr=ui->textEdit_3->toPlainText();
    int age=agestr.toInt();

    int current_row=ui->listWidget->currentRow();

    if (task_PD || (task_AD && current_row==10)){

        if (age>40){
            if (ref_female){
                if (mF0.toDouble()>avgf0u_f  || mF0.toDouble()<avgf0d_f){ui->label_75->setPalette(sample_palette);}
                if (sF0.toDouble()>stdf0u_f  || sF0.toDouble()<stdf0d_f){ui->label_76->setPalette(sample_palette);}
                if (mmF0.toDouble()>maxf0u_f  || mmF0.toDouble()<maxf0d_f){ui->label_77->setPalette(sample_palette);}
                if (mlogE.toDouble()>avgEu_f  || mlogE.toDouble()<avgEd_f){ui->label_78->setPalette(sample_palette);}
                if (slogE.toDouble()>stdEu_f  || slogE.toDouble()<stdEd_f){ui->label_79->setPalette(sample_palette);}
                if (mmlogE.toDouble()>maxEu_f  || mmlogE.toDouble()<maxEd_f){ui->label_80->setPalette(sample_palette);}
                if (Vrate.toDouble()>Vrateu_f  || Vrate.toDouble()<Vrated_f){ui->label_69->setPalette(sample_palette);}
                if (avgdurv.toDouble()>avgdurVu_f  || avgdurv.toDouble()<avgdurVd_f){ui->label_70->setPalette(sample_palette);}
                if (stddurv.toDouble()>stddurVu_f  || stddurv.toDouble()<stddurVd_f){ui->label_71->setPalette(sample_palette);}
                if (Silrate.toDouble()>Srateu_f  || Silrate.toDouble()<Srated_f){ui->label_72->setPalette(sample_palette);}
                if (avgdurs.toDouble()>avgdurSu_f  || avgdurs.toDouble()<avgdurSd_f){ui->label_73->setPalette(sample_palette);}
                if (stddurs.toDouble()>stddurSu_f  || stddurs.toDouble()<stddurSd_f){ui->label_74->setPalette(sample_palette);}
                if (varf0semi.toDouble()>varF0semiu_f  || varf0semi.toDouble()<varF0semid_f){ui->label_187->setPalette(sample_palette);}

                ui->label_107->setText(QString{ "%1->%2" }.arg( avgf0d_f).arg(avgf0u_f));
                ui->label_108->setText(QString{ "%1->%2" }.arg( stdf0d_f).arg(stdf0u_f));
                ui->label_109->setText(QString{ "%1->%2" }.arg( maxf0d_f).arg(maxf0u_f));
                ui->label_110->setText(QString{ "%1->%2" }.arg( avgEd_f).arg(avgEu_f));
                ui->label_111->setText(QString{ "%1->%2" }.arg( stdEd_f).arg(stdEu_f));
                ui->label_112->setText(QString{ "%1->%2" }.arg( maxEd_f).arg(maxEu_f));
                ui->label_113->setText(QString{ "%1->%2" }.arg( Vrated_f).arg(Vrateu_f));
                ui->label_114->setText(QString{ "%1->%2" }.arg( avgdurVd_f).arg(avgdurVu_f));
                ui->label_115->setText(QString{ "%1->%2" }.arg( stddurVd_f).arg(stddurVu_f));
                ui->label_116->setText(QString{ "%1->%2" }.arg( Srated_f).arg(Srateu_f));
                ui->label_117->setText(QString{ "%1->%2" }.arg( avgdurSd_f).arg(avgdurSu_f));
                ui->label_118->setText(QString{ "%1->%2" }.arg( stddurSd_f).arg(stddurSu_f));
                ui->label_186->setText(QString{ "%1->%2" }.arg( varF0semid_f).arg(varF0semiu_f));

            }
            else{
                if (mF0.toDouble()>avgf0u_m  || mF0.toDouble()<avgf0d_m){ui->label_75->setPalette(sample_palette);}
                if (sF0.toDouble()>stdf0u_m  || sF0.toDouble()<stdf0d_m){ui->label_76->setPalette(sample_palette);}
                if (mmF0.toDouble()>maxf0u_m  || mmF0.toDouble()<maxf0d_m){ui->label_77->setPalette(sample_palette);}
                if (mlogE.toDouble()>avgEu_m  || mlogE.toDouble()<avgEd_m){ui->label_78->setPalette(sample_palette);}
                if (slogE.toDouble()>stdEu_m  || slogE.toDouble()<stdEd_m){ui->label_79->setPalette(sample_palette);}
                if (mmlogE.toDouble()>maxEu_m  || mmlogE.toDouble()<maxEd_m){ui->label_80->setPalette(sample_palette);}
                if (Vrate.toDouble()>Vrateu_m  || Vrate.toDouble()<Vrated_m){ui->label_69->setPalette(sample_palette);}
                if (avgdurv.toDouble()>avgdurVu_m  || avgdurv.toDouble()<avgdurVd_m){ui->label_70->setPalette(sample_palette);}
                if (stddurv.toDouble()>stddurVu_m  || stddurv.toDouble()<stddurVd_m){ui->label_71->setPalette(sample_palette);}
                if (Silrate.toDouble()>Srateu_m  || Silrate.toDouble()<Srated_m){ui->label_72->setPalette(sample_palette);}
                if (avgdurs.toDouble()>avgdurSu_m  || avgdurs.toDouble()<avgdurSd_m){ui->label_73->setPalette(sample_palette);}
                if (stddurs.toDouble()>stddurSu_m  || stddurs.toDouble()<stddurSd_m){ui->label_74->setPalette(sample_palette);}
                if (varf0semi.toDouble()>varF0semiu_m  || varf0semi.toDouble()<varF0semid_m){ui->label_187->setPalette(sample_palette);}

                ui->label_107->setText(QString{ "%1->%2" }.arg( avgf0d_m).arg(avgf0u_m));
                ui->label_108->setText(QString{ "%1->%2" }.arg( stdf0d_m).arg(stdf0u_m));
                ui->label_109->setText(QString{ "%1->%2" }.arg( maxf0d_m).arg(maxf0u_m));
                ui->label_110->setText(QString{ "%1->%2" }.arg( avgEd_m).arg(avgEu_m));
                ui->label_111->setText(QString{ "%1->%2" }.arg( stdEd_m).arg(stdEu_m));
                ui->label_112->setText(QString{ "%1->%2" }.arg( maxEd_m).arg(maxEu_m));
                ui->label_113->setText(QString{ "%1->%2" }.arg( Vrated_m).arg(Vrateu_m));
                ui->label_114->setText(QString{ "%1->%2" }.arg( avgdurVd_m).arg(avgdurVu_m));
                ui->label_115->setText(QString{ "%1->%2" }.arg( stddurVd_m).arg(stddurVu_m));
                ui->label_116->setText(QString{ "%1->%2" }.arg( Srated_m).arg(Srateu_m));
                ui->label_117->setText(QString{ "%1->%2" }.arg( avgdurSd_m).arg(avgdurSu_m));
                ui->label_118->setText(QString{ "%1->%2" }.arg( stddurSd_m).arg(stddurSu_m));
                ui->label_186->setText(QString{ "%1->%2" }.arg( varF0semid_m).arg(varF0semiu_m));
            }

        }
        else{
            if (mF0.toDouble()>avgf0u_y  || mF0.toDouble()<avgf0d_y){ui->label_75->setPalette(sample_palette);}
            if (sF0.toDouble()>stdf0u_y  || sF0.toDouble()<stdf0d_y){ui->label_76->setPalette(sample_palette);}
            if (mmF0.toDouble()>maxf0u_y  || mmF0.toDouble()<maxf0d_y){ui->label_77->setPalette(sample_palette);}
            if (mlogE.toDouble()>avgEu_y  || mlogE.toDouble()<avgEd_y){ui->label_78->setPalette(sample_palette);}
            if (slogE.toDouble()>stdEu_y  || slogE.toDouble()<stdEd_y){ui->label_79->setPalette(sample_palette);}
            if (mmlogE.toDouble()>maxEu_y  || mmlogE.toDouble()<maxEd_y){ui->label_80->setPalette(sample_palette);}
            if (Vrate.toDouble()>Vrateu_y  || Vrate.toDouble()<Vrated_y){ui->label_69->setPalette(sample_palette);}
            if (avgdurv.toDouble()>avgdurVu_y  || avgdurv.toDouble()<avgdurVd_y){ui->label_70->setPalette(sample_palette);}
            if (stddurv.toDouble()>stddurVu_y  || stddurv.toDouble()<stddurVd_y){ui->label_71->setPalette(sample_palette);}
            if (Silrate.toDouble()>Srateu_y  || Silrate.toDouble()<Srated_y){ui->label_72->setPalette(sample_palette);}
            if (avgdurs.toDouble()>avgdurSu_y  || avgdurs.toDouble()<avgdurSd_y){ui->label_73->setPalette(sample_palette);}
            if (stddurs.toDouble()>stddurSu_y  || stddurs.toDouble()<stddurSd_y){ui->label_74->setPalette(sample_palette);}
            if (varf0semi.toDouble()>varF0semiu_y  || varf0semi.toDouble()<varF0semid_y){ui->label_187->setPalette(sample_palette);}

            ui->label_107->setText(QString{ "%1->%2" }.arg( avgf0d_y).arg(avgf0u_y));
            ui->label_108->setText(QString{ "%1->%2" }.arg( stdf0d_y).arg(stdf0u_y));
            ui->label_109->setText(QString{ "%1->%2" }.arg( maxf0d_y).arg(maxf0u_y));
            ui->label_110->setText(QString{ "%1->%2" }.arg( avgEd_y).arg(avgEu_y));
            ui->label_111->setText(QString{ "%1->%2" }.arg( stdEd_y).arg(stdEu_y));
            ui->label_112->setText(QString{ "%1->%2" }.arg( maxEd_y).arg(maxEu_y));
            ui->label_113->setText(QString{ "%1->%2" }.arg( Vrated_y).arg(Vrateu_y));
            ui->label_114->setText(QString{ "%1->%2" }.arg( avgdurVd_y).arg(avgdurVu_y));
            ui->label_115->setText(QString{ "%1->%2" }.arg( stddurVd_y).arg(stddurVu_y));
            ui->label_116->setText(QString{ "%1->%2" }.arg( Srated_y).arg(Srateu_y));
            ui->label_117->setText(QString{ "%1->%2" }.arg( avgdurSd_y).arg(avgdurSu_y));
            ui->label_118->setText(QString{ "%1->%2" }.arg( stddurSd_y).arg(stddurSu_y));
            ui->label_186->setText(QString{ "%1->%2" }.arg( varF0semid_y).arg(varF0semiu_y));
        }

        if (age>40){
            if(ref_female){
                ui->radarPros->setPixmap(QPixmap( path_base+"prosodyradarfemale.png" ) );
            }
            else{
                ui->radarPros->setPixmap(QPixmap( path_base+"prosodyradarmale.png" ) );
            }
        }
        else{
            ui->radarPros->setPixmap(QPixmap( path_base+"prosodyradaryoung.png" ) );
        }

    }

    else{
        ui->label_75->setPalette(sample_palette_blue);
        ui->label_76->setPalette(sample_palette_blue);
        ui->label_77->setPalette(sample_palette_blue);
        ui->label_78->setPalette(sample_palette_blue);
        ui->label_79->setPalette(sample_palette_blue);
        ui->label_80->setPalette(sample_palette_blue);
        ui->label_69->setPalette(sample_palette_blue);
        ui->label_70->setPalette(sample_palette_blue);
        ui->label_71->setPalette(sample_palette_blue);
        ui->label_72->setPalette(sample_palette_blue);
        ui->label_73->setPalette(sample_palette_blue);
        ui->label_74->setPalette(sample_palette_blue);
        ui->label_187->setPalette(sample_palette_blue);

        ui->label_107->setText(QString{ "->" });
        ui->label_108->setText(QString{ "->" });
        ui->label_109->setText(QString{ "->" });
        ui->label_110->setText(QString{ "->" });
        ui->label_111->setText(QString{ "->" });
        ui->label_112->setText(QString{ "->" });
        ui->label_113->setText(QString{ "->" });
        ui->label_114->setText(QString{ "->" });
        ui->label_115->setText(QString{ "->" });
        ui->label_116->setText(QString{ "->" });
        ui->label_117->setText(QString{ "->" });
        ui->label_118->setText(QString{ "->" });
        ui->label_186->setText(QString{ "->" });

    }

}

bool MainWindow::fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

QString MainWindow::get_name(int rec_flag, QString folder, bool save){
    QString name="";
    if (task_PD){
        switch (rec_flag){
        case 0:
            name=folder+"/A.wav";
             ui->label_97->setText("Sustained vowel /A/");
            break;
        case 1:
            name=folder+"/E.wav";
            ui->label_97->setText("Sustained vowel /E/");
            break;
        case 2:
            name=folder+"/I.wav";
            ui->label_97->setText("Sustained vowel /I/");
            break;
        case 3:
            name=folder+"/O.wav";
            ui->label_97->setText("Sustained vowel /O/");
            break;
        case 4:
            name=folder+"/U.wav";
            ui->label_97->setText("Sustained vowel /U/");
            break;
        case 5:
            name=folder+"/ModulatedVowels.wav";
            ui->label_97->setText("Modulated vowels /A-E-I-O-U/");
            break;
        case 6:
            name=folder+"/DDK.wav";
            ui->label_97->setText("Rapid repetition of /pa-ta-ka/");
            break;
        case 7:
            name=folder+"/DDK2.wav";
            ui->label_97->setText("Rapid repetition of /pa-ka-ta/");
            break;
        case 8:
            name=folder+"/DDK3.wav";
            ui->label_97->setText("Rapid repetition of /pe-ta-ka/");
            break;
        case 9:
            name=folder+"/DDK4.wav";
            ui->label_97->setText("Rapid repetition of /pa/");

            break;
        case 10:
            name=folder+"/DDK5.wav";
            ui->label_97->setText("Rapid repetition of /ta/");
            break;
        case 11:
            name=folder+"/DDK6.wav";
            ui->label_97->setText("Rapid repetition of /ka/");
            break;
        case 12:
            name=folder+"/words.wav";
            ui->label_97->setText("Read isolated words");

            break;
        case 13:
            name=folder+"/sent1.wav";
            ui->label_97->setText("sentence: \r\n Mi casa tiene tres cuartos");
            break;
        case 14:
            name=folder+"/sent2.wav";
            ui->label_97->setText("sentence: \r\n Omar, que vive cerca trajo miel");
            break;
        case 15:
            name=folder+"/sent3.wav";
            ui->label_97->setText("sentence: \r\n Laura sube al tren que pasa");

            break;
        case 16:
            name=folder+"/sent4.wav";
            ui->label_97->setText("sentence: \r\n Los libros nuevos no caben \r\n en la mesa de la oficina");

            break;
        case 17:
            name=folder+"/sent5.wav";
            ui->label_97->setText("sentence: \r\n Rosita Niño, que pinta bien, \r\n donó sus cuadros ayer");

            break;
        case 18:
            name=folder+"/sent6.wav";
            ui->label_97->setText("sentence: \r\n Luisa Rey compra el colchón \r\n duro que tanto le gusta");

            break;
        case 19:
            name=folder+"/sent7.wav";
            ui->label_97->setText("sentence: \r\n Viste las noticias, \r\n yo ví ganar la medalla de plata en pesas, \r\n ese muchacho tiene mucha fuerza");

            break;
        case 20:
            name=folder+"/sent8.wav";
            ui->label_97->setText("sentence: \r\n Juan se rompió una pierna \r\n cuando iba en la moto");

            break;
        case 21:
            name=folder+"/sent9.wav";
            ui->label_97->setText("sentence: \r\n Estoy muy triste, \r\n ayer vi morir a un amigo");
            break;
        case 22:
            name=folder+"/sent10.wav";
            ui->label_97->setText("sentence: \r\n Estoy muy preocupado, \r\n cada vez me es más dificil hablar");
            break;
        case 23:
            name=folder+"/Readtext.wav";
            ui->label_97->setText("Ayer fuí al médico, \r\n Qué le pasa? me preguntó. Yo le dije: \r\n Ay doctor. Donde pongo el dedo me duele. \r\n Tiene la uña rota? Sí, Pues ya sabemos que es \r\n Deje su cheque a la salida");

            break;
        default:
            name=folder+"/Monologue.wav";
            ui->label_97->setText("Spontanepus speech: \r\n What do you do in a normal day?");

            break;
        }
    }

    if (task_AD){
        switch (rec_flag){
        case 0:
            name=folder+"/cookie-theft.wav";
            ui->label_97->setText("Description of the cookie theft image.");
            break;
        case 1:
            name=folder+"/seq-numbers.wav";
            ui->label_97->setText("Please say the sequence of \r\n numbers from 1 to 21.");
            break;
        case 2:
            name=folder+"/seq-days.wav";
            ui->label_97->setText("Please say the sequence \r\n of the days of the week.");
            break;
        case 3:
            name=folder+"/seq-months.wav";
            ui->label_97->setText("Please say the sequence \r\n of the months of the year.");
            break;
        case 4:
            name=folder+"/words.wav";
            ui->label_97->setText("Read isolated words.");
            break;
        case 5:
            name=folder+"/no-words.wav";
            ui->label_97->setText("Read isolated no words.");
            break;
        case 6:
            name=folder+"/sent1.wav";
            ui->label_97->setText("sentence: \r\n");
            break;
        case 7:
            name=folder+"/sent2.wav";
            ui->label_97->setText("sentence: \r\n");
            break;
        case 8:
            name=folder+"/sent3.wav";
            ui->label_97->setText("sentence: \r\n");
            break;

        case 9:
            name=folder+"/sent4.wav";
            ui->label_97->setText("sentence: \r\n ");
            break;

        case 10:
            name=folder+"/sent5.wav";
            ui->label_97->setText("sentence: \r\n ");
            break;

        case 11:
            name=folder+"/sent6.wav";
            ui->label_97->setText("sentence: \r\n ");
            break;

        case 12:
            name=folder+"/sent7.wav";
            ui->label_97->setText("sentence: \r\n");
            break;

        case 13:
            name=folder+"/sent8.wav";
            ui->label_97->setText("sentence: \r\n ");
            break;
        case 14:
            name=folder+"/fluency-animals.wav";
            ui->label_97->setText("All of animals in one minute");
            break;

        case 15:
            name=folder+"/fluency-pwords.wav";
            ui->label_97->setText("All of words starting with P in one minute");
            break;
        case 16:
            name=folder+"/rabbit-history.wav";
            ui->label_97->setText("Explain history about rabbit and turtle \r\n with the sequence of images");
            break;
        default:
            name=folder+"/Readtext.wav";
            ui->label_97->setText("read text: Ayer fuí al médico, \r\n Qué le pasa? me preguntó. Yo le dije: \r\n Ay doctor. Donde pongo el dedo me duele. \r\n Tiene la uña rota? Sí, Pues ya sabemos que es \r\n Deje su cheque a la salida");
            break;


        }
    }
    QString contS;
    if (save){
        contfile=1;

        while(fileExists(name))
        {
          if (contfile>1){
                name.replace("_"+contS.number(contfile-1)+".wav", "_"+contS.number(contfile)+".wav");
          }
          else{
                name.replace(".wav", "_"+contS.number(contfile)+".wav");
          }
          contfile++;
        }
    }
    else{
        if ((contfile)>1)
            name.replace(".wav", "_"+contS.number(contfile-1)+".wav");
    }
    qDebug() << name;
    return name;
}


QVector<double> MainWindow::readWavFile(QString fileName){
    /** Read a wav file to play audio into a buffer and return the size of the data read
     * after stripping the header.
     *
     * The header for a WAV file looks like this:
     * Positions	Sample Value	Description
     * 1 - 4	"RIFF"	Marks the file as a riff file. Characters are each 1 byte long.
     * 5 - 8	File size (integer)	Size of the overall file - 8 bytes, in bytes (32-bit integer).
     * 9 -12	"WAVE"	File Type Header. For our purposes, it always equals "WAVE".
     * 13-16	"fmt "	Format chunk marker. Includes trailing null
     * 17-20	16	Length of format data as listed above
     * 21-22	1	Type of format (1 is PCM) - 2 byte integer
     * 23-24	2	Number of Channels - 2 byte integer
     * 25-28	44100	Sample Rate - 32 byte integer. CSample Rate = Number of Samples per second, or Hertz.
     * 29-32	176400	(Sample Rate * BitsPerSample * Channels) / 8.
     * 33-34	4	(BitsPerSample * Channels) / 8.1 - 8 bit mono2 - 8 bit stereo/16 bit mono4 - 16 bit stereo
     * 35-36	16	Bits per sample
     * 37-40	"data"	"data" chunk header. Marks the beginning of the data section.
     * 41-44	File size (data)	Size of the data section.
     * Sample values are given above for a 16-bit stereo source.
     *
     * @param fileName
     * A QString representing the file location to open with QFile
     *
     * @param ramBuffer
     * A pointer to a Pre-Allocated signed short buffer
     *
     */

    // Open wave file
    qDebug() << "Opening WAV file at: " << fileName;
    QFile wavFile(fileName);
    if (!wavFile.open(QFile::ReadOnly))
    {
        qDebug() << "Failed to open WAV file...";
        return (QVector<double>) -1; // Done
    }

    // Read in the whole thing
    QByteArray wavFileContent = wavFile.readAll();
    qDebug() << "The size of the WAV file is: " << wavFileContent.size();

    // Define the header components

    char fileType[4];
    qint32 fileSize;
    char waveName[4];
    char fmtName[4];
    qint32 fmtLength;
    short fmtType;
    short numberOfChannels;
    qint32 sampleRate;
    qint32 sampleRateXBitsPerSampleXChanngelsDivEight;
    short bitsPerSampleXChannelsDivEightPointOne;
    short bitsPerSample;
    char dataHeader[4];
    qint32 dataSize;

    // Create a data stream to analyze the data
    QDataStream analyzeHeaderDS(&wavFileContent,QIODevice::ReadOnly);
    analyzeHeaderDS.setByteOrder(QDataStream::LittleEndian);

    // Now pop off the appropriate data into each header field defined above
    analyzeHeaderDS.readRawData(fileType,4); // "RIFF"
    analyzeHeaderDS >> fileSize; // File Size
    analyzeHeaderDS.readRawData(waveName,4); // "WAVE"
    analyzeHeaderDS.readRawData(fmtName,4); // "fmt"
    analyzeHeaderDS >> fmtLength; // Format length
    analyzeHeaderDS >> fmtType; // Format type
    analyzeHeaderDS >> numberOfChannels; // Number of channels
    analyzeHeaderDS >> sampleRate; // Sample rate
    analyzeHeaderDS >> sampleRateXBitsPerSampleXChanngelsDivEight; // (Sample Rate * BitsPerSample * Channels) / 8
    analyzeHeaderDS >> bitsPerSampleXChannelsDivEightPointOne; // (BitsPerSample * Channels) / 8.1
    analyzeHeaderDS >> bitsPerSample; // Bits per sample
    analyzeHeaderDS.readRawData(dataHeader,4); // "data" header
    analyzeHeaderDS >> dataSize; // Data Size

    // Print the header
    qDebug() << "WAV File Header read:";
    qDebug() << "File Type: " << QString::fromUtf8(fileType);
    qDebug() << "File Size: " << fileSize;
    qDebug() << "WAV Marker: " << QString::fromUtf8(waveName);
    qDebug() << "Format Name: " << QString::fromUtf8(fmtName);
    qDebug() << "Format Length: " << fmtLength;
    qDebug() << "Format Type: " << fmtType;
    qDebug() << "Number of Channels: " << numberOfChannels;
    qDebug() << "Sample Rate: " << sampleRate;
    qDebug() << "Sample Rate * Bits/Sample * Channels / 8: " << sampleRateXBitsPerSampleXChanngelsDivEight;
    qDebug() << "Bits per Sample * Channels / 8.1: " << bitsPerSampleXChannelsDivEightPointOne;
    qDebug() << "Bits per Sample: " << bitsPerSample;
    qDebug() << "Data Header: " << QString::fromUtf8(dataHeader);
    qDebug() << "Data Size: " << dataSize;
    //char data[(int)dataSize];
    // Now pull out the data

    int n=(int)dataSize/bitsPerSampleXChannelsDivEightPointOne;
    QVector<double> y(n);
    if (bitsPerSample==32){
    int data;

    for (int i=0; i<n; i++)
    {

      analyzeHeaderDS >> data;
      y[i]=((double)data)/2147483648;
    }
}
    else{
        short data;
        for (int i=0; i<n; i++)
        {

          analyzeHeaderDS >> data;
          y[i]=((double)data)/32768;
        }
    }

    return y;
}

void MainWindow::updateProgress(qint64 duration)
{
    if (audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;
    ui->statusBar->showMessage(tr("Recorded %1 sec").arg(duration / 1000));
}

void MainWindow::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;
    switch (status) {
    case QMediaRecorder::RecordingStatus:
        statusMessage = tr("Recording to %1").arg(audioRecorder->actualLocation().toString());
        break;
    case QMediaRecorder::PausedStatus:
        clearAudioLevels();
        statusMessage = tr("Paused");
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        clearAudioLevels();
        statusMessage = tr("Stopped");
    default:
        break;
    }
    if (audioRecorder->error() == QMediaRecorder::NoError)
        ui->statusBar->showMessage(statusMessage);
}

void MainWindow::onStateChanged(QMediaRecorder::State state)
{        QPixmap pix5(button5stop);
         QIcon icon5(pix5);
         QPixmap pix6(button5rec);
         QIcon icon6(pix6);
    switch (state) {
    case QMediaRecorder::RecordingState:

        ui->pushButton_5->setIcon(icon5);
        ui->pushButton_5->setIconSize(QSize(71,71));
        break;
    case QMediaRecorder::PausedState:
        ui->pushButton_5->setIcon(icon5);
        ui->pushButton_5->setIconSize(QSize(71,71));
        break;
    case QMediaRecorder::StoppedState:

        ui->pushButton_5->setIcon(icon6);
        ui->pushButton_5->setIconSize(QSize(71,71));
        break;
    }

}

void MainWindow::displayErrorMessage()
{
    ui->statusBar->showMessage(audioRecorder->errorString());
}

void MainWindow::clearAudioLevels()
{
    for (int i = 0; i < audioLevels.size(); ++i)
        audioLevels.at(i)->setLevel(0);
}

// This function returns the maximum possible sample value for a given audio format
qreal getPeakValue(const QAudioFormat& format)
{
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        if (format.sampleSize() != 32) // other sample formats are not supported
            return qreal(0);
        return qreal(1.00003);
    case QAudioFormat::SignedInt:
        if (format.sampleSize() == 32)
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
        break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}

// returns the audio level for each channel
QVector<qreal> getBufferLevels(const QAudioBuffer& buffer)
{
    QVector<qreal> values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm")
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32) {
            values = getBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)

            values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}

template <class T>
QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);

    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }

    return max_values;
}


void MainWindow::processBuffer(const QAudioBuffer& buffer)
{
    if (audioLevels.count() != buffer.format().channelCount()) {
        qDeleteAll(audioLevels);
        audioLevels.clear();
        for (int i = 0; i < buffer.format().channelCount(); ++i) {
            QAudioLevel *level = new QAudioLevel(ui->frame);
            audioLevels.append(level);
            ui->levelsLayout_2->addWidget(level);
        }
    }

    QVector<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i)
        audioLevels.at(i)->setLevel(levels.at(i));
}


int MainWindow::printpdfAD(QString inputhtml, QString otputfile){



    QString current_path=QDir::currentPath();

    QDir::setCurrent(QCoreApplication::applicationDirPath());
    QString line;
    QFile  htmlFile (inputhtml);
    if (!htmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        return -5;
    }
    QString htmlContent;
    QStringList htmlContentLines;
    QTextStream in(&htmlFile);

    while (!in.atEnd()) {
      line = in.readLine();
      htmlContentLines.append(line);
      //qDebug() <<line;
    }

    QString mF0, sF0, mlogE, slogE, Vrate, avgdurv, stddurv, Silrate, avgdurs, stddurs, varF0semip, featuresProsody;

    featuresProsody=current_path+"/../prosody/feat.txt";
    QList<QString> featpros;
    QFile filefeatprosody(featuresProsody);

    QTextDocument *document = new QTextDocument();

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(otputfile);

     namePatient=ui->textEdit->toPlainText();
     lastPatient=ui->textEdit_2->toPlainText();
     htmlContentLines[1].replace("Logo.png", current_path+"/../PDTool/Logo.png");
     htmlContentLines[5].replace("Fist Name:", "First Name:"+namePatient);
     htmlContentLines[6].replace("Last Name:", "Last Name:"+lastPatient);

     if (!filefeatprosody.open(QIODevice::ReadOnly | QIODevice::Text))
         return -3;
     while (!filefeatprosody.atEnd()) {
          line = filefeatprosody.readLine();
         featpros.append(line);
     }
     filefeatprosody.close();
     mF0=featpros[0];
     sF0=featpros[1];
     mlogE=featpros[3];
     slogE=featpros[4];
     Vrate=featpros[6];
     avgdurv=featpros[7];
     stddurv=featpros[8];
     Silrate=featpros[9];
     avgdurs=featpros[10];
     stddurs=featpros[11];
     varF0semip=featpros[12];

     //qDebug() <<htmlContentLines[11];
     htmlContentLines[11].replace("prosody1.png", current_path+"/../prosody/prosody.png");

     QString agestr=ui->textEdit_3->toPlainText();
     int age=agestr.toInt();
     if (age<40){
         htmlContentLines[11].replace("prosody2.png", current_path+"/../prosody/prosodyradaryoung.png");
     }
     else{
         if (ref_female){htmlContentLines[11].replace("prosody2.png", current_path+"/../prosody/prosodyradarfemale.png");}
         if (ref_male){htmlContentLines[11].replace("prosody2.png", current_path+"/../prosody/prosodyradarmale.png");}

     }
     htmlContentLines[25].replace('-', Vrate);
     htmlContentLines[31].replace('-', avgdurv);
     htmlContentLines[37].replace('-', stddurv);
     htmlContentLines[43].replace('-', Silrate);
     htmlContentLines[49].replace('-', avgdurs);
     htmlContentLines[55].replace('-', stddurs);
     htmlContentLines[61].replace('-', mlogE);
     htmlContentLines[67].replace('-', slogE);
     htmlContentLines[73].replace('-', mF0);
     htmlContentLines[79].replace('-', sF0);

     htmlContent=htmlContentLines.join('\n');

     document->setHtml(htmlContent);
     document->print(&printer);
     htmlFile.close();
     delete document;
     return 0;

}


int MainWindow::printpdf(QString inputhtml, QString otputfile){

    QString current_path=QDir::currentPath();

    QDir::setCurrent(QCoreApplication::applicationDirPath());
    QString line;
    QFile  htmlFile (inputhtml);
    if (!htmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        return -5;
    }
    QString htmlContent;
    QStringList htmlContentLines;
    QTextStream in(&htmlFile);

    while (!in.atEnd()) {
      line = in.readLine();
      htmlContentLines.append(line);
      qDebug() <<line;
    }

    QString mF0, sF0, mlogE, slogE, Vrate, avgdurv, stddurv, Silrate, avgdurs, stddurs, varF0semip, featuresProsody;

    featuresProsody=current_path+"/../prosody/feat.txt";
    QList<QString> featpros;
    QFile filefeatprosody(featuresProsody);

    QTextDocument *document = new QTextDocument();

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(otputfile);



    QString featuresPhonation=current_path+"/../phonVowels/feat.txt";

    QList<QString> feat;
    QFile filefeatphonation(featuresPhonation);
    if (!filefeatphonation.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;
    while (!filefeatphonation.atEnd()) {
         line = filefeatphonation.readLine();
        feat.append(line);
    }

    filefeatphonation.close();
    QString jitter=feat[0];
    QString shimmer=feat[1];
    QString APQ=feat[2];
    QString PPQ=feat[3];
    QString degU=feat[6];
    QString varF0=feat[7];

    QList<QString> featartv;
    QString featuresArtVowels=current_path+"/../artVowels/feat.txt";
    QFile filefeaturesArtVowels(featuresArtVowels);
    if (!filefeaturesArtVowels.open(QIODevice::ReadOnly | QIODevice::Text))
        return -2;
    while (!filefeaturesArtVowels.atEnd()) {
         line = filefeaturesArtVowels.readLine();
        featartv.append(line);
    }
    filefeaturesArtVowels.close();
    QString VSA=featartv[0];
    QString FCR=featartv[1];
    QString VPA=featartv[2];
    QString mF1a=featartv[3];
    QString mF2a=featartv[4];
    QString mF1e=featartv[5];
    QString mF2e=featartv[6];
    QString mF1i=featartv[7];
    QString mF2i=featartv[8];
    QString mF1o=featartv[9];
    QString mF2o=featartv[10];
    QString mF1u=featartv[11];
    QString mF2u=featartv[12];

    if (!filefeatprosody.open(QIODevice::ReadOnly | QIODevice::Text))
        return -3;
    while (!filefeatprosody.atEnd()) {
         line = filefeatprosody.readLine();
        featpros.append(line);
    }
    filefeatprosody.close();
    mF0=featpros[0];
    sF0=featpros[1];
    mlogE=featpros[3];
    slogE=featpros[4];
    Vrate=featpros[6];
    avgdurv=featpros[7];
    stddurv=featpros[8];
    Silrate=featpros[9];
    avgdurs=featpros[10];
    stddurs=featpros[11];
    varF0semip=featpros[12];
    QString Predicted=current_path+"/../evaluation/predmFDA.txt";
    QList<QString> Predictedlist;
    QFile filePred(Predicted);
    if (!filePred.open(QIODevice::ReadOnly | QIODevice::Text))
        return -4;
    while (!filePred.atEnd()) {
         line = filePred.readLine();
        Predictedlist.append(line);
    }

    filePred.close();



    QString featuresDDK=current_path+"/../DDK/feat.txt";
    QList<QString> featDDK;
    QFile filefeatDDK(featuresDDK);
    if (!filefeatDDK.open(QIODevice::ReadOnly | QIODevice::Text))
        return -3;
    while (!filefeatDDK.atEnd()) {
         line = filefeatDDK.readLine();
        featDDK.append(line);
    }
    filefeatDDK.close();
    QString varF0semi=featDDK[0];
    QString sF0d=featDDK[1];
    QString mlogEd=featDDK[2];
    QString slogEd=featDDK[3];
    QString DDKrate=featDDK[5];
    QString avgdurDDK=featDDK[6];
    QString stddurDDK=featDDK[7];
    QString Silrated=featDDK[8];
    QString avgdursd=featDDK[9];
    QString stddursd=featDDK[10];




htmlContentLines[1].replace("Logo.png", current_path+"/../PDTool/Logo.png");
htmlContentLines[10].replace("phonation1.png", current_path+"/../phonVowels/phonation1.png");
if (ref_female){htmlContentLines[10].replace("phonation2.png", current_path+"/../phonVowels/phonation2female.png");}
if (ref_male){htmlContentLines[10].replace("phonation2.png", current_path+"/../phonVowels/phonation2male.png");}
qDebug() <<htmlContentLines[62];
htmlContentLines[62].replace("articulation1.png", current_path+"/../artVowels/articulation1.png");
if (ref_female){htmlContentLines[62].replace("articulation2.png", current_path+"/../artCont/articulationfemale.png");}
if (ref_male){htmlContentLines[62].replace("articulation2.png", current_path+"/../artCont/articulationmale.png");}
qDebug() <<htmlContentLines[124];
htmlContentLines[124].replace("prosody1.png", current_path+"/../prosody/prosody.png");
if (ref_female){htmlContentLines[124].replace("prosody2.png", current_path+"/../prosody/prosodyradarfemale.png");}
if (ref_male){htmlContentLines[124].replace("prosody2.png", current_path+"/../prosody/prosodyradarmale.png");}
qDebug() <<htmlContentLines[200];
htmlContentLines[200].replace("ddk1.png", current_path+"/../DDK/DDK.png");
if (ref_female){htmlContentLines[200].replace("ddk2.png", current_path+"/../DDK/DDKradarfemale.png");}
if (ref_male){htmlContentLines[200].replace("ddk2.png", current_path+"/../DDK/DDKradarmale.png");}


if (fileExists(current_path+"/../intelligibility/intelligibility.png")){
    htmlContentLines[278].replace("intelligibility.png", current_path+"/../intelligibility/intelligibility.png");
}

namePatient=ui->textEdit->toPlainText();
lastPatient=ui->textEdit_2->toPlainText();
qDebug() <<htmlContentLines[355];
htmlContentLines[355].replace("prediction.png", current_path+"/../evaluation/prediction.png");
htmlContentLines[5].replace("Fist Name:", "First Name:"+namePatient);
htmlContentLines[6].replace("Last Name:", "Last Name:"+lastPatient);
qDebug() <<htmlContentLines[24];
qDebug() <<htmlContentLines[30];
qDebug() <<jitter;
qDebug() <<shimmer;

htmlContentLines[24].replace('-', jitter);
htmlContentLines[30].replace('-', shimmer);
htmlContentLines[36].replace('-', APQ);
htmlContentLines[42].replace('-', PPQ);
htmlContentLines[48].replace('-', degU);
htmlContentLines[54].replace('-', varF0);
qDebug() <<htmlContentLines[75];
qDebug() <<htmlContentLines[80];
qDebug() <<htmlContentLines[98];
qDebug() <<mF1a;

htmlContentLines[75].replace('-', mF1a);
htmlContentLines[80].replace('-', mF2a);
htmlContentLines[86].replace('-', mF1i);
htmlContentLines[92].replace('-', mF2i);
htmlContentLines[98].replace('-', mF1u);
htmlContentLines[104].replace('-', mF2u);
htmlContentLines[110].replace('-', VSA);
htmlContentLines[116].replace('-', FCR);
qDebug() <<htmlContentLines[138];
qDebug() <<Vrate;
htmlContentLines[138].replace('-', Vrate);
htmlContentLines[144].replace('-', avgdurv);
htmlContentLines[150].replace('-', stddurv);
htmlContentLines[156].replace('-', Silrate);
htmlContentLines[162].replace('-', avgdurs);
htmlContentLines[168].replace('-', stddurs);
htmlContentLines[174].replace('-', mlogE);
htmlContentLines[180].replace('-', slogE);
htmlContentLines[186].replace('-', mF0);
htmlContentLines[192].replace('-', sF0);
htmlContentLines[214].replace('-', DDKrate);
qDebug() <<htmlContentLines[220];
qDebug() <<avgdurDDK;
htmlContentLines[220].replace('-', avgdurDDK);
htmlContentLines[226].replace('-', stddurDDK);
htmlContentLines[232].replace('-', Silrated);
htmlContentLines[238].replace('-', avgdursd);
htmlContentLines[244].replace('-', stddursd);
htmlContentLines[250].replace('-', mlogEd);
htmlContentLines[256].replace('-', slogEd);
htmlContentLines[262].replace('-', sF0d);
htmlContentLines[268].replace('-', varF0semi);


QString waavgstr, dtwavgstr;
htmlContentLines[291].replace('-', waavgstr.setNum(WAavg));
htmlContentLines[296].replace('-', dtwavgstr.setNum(DTWavg));
htmlContentLines[315].replace('-', Predictedlist[1]);
htmlContentLines[320].replace('-', Predictedlist[2]);
htmlContentLines[325].replace('-', Predictedlist[3]);
htmlContentLines[330].replace('-', Predictedlist[4]);
htmlContentLines[335].replace('-', Predictedlist[5]);
htmlContentLines[340].replace('-', Predictedlist[6]);
htmlContentLines[345].replace('-', Predictedlist[0]);
htmlContentLines[350].replace('-', Predictedlist[7]);
htmlContent=htmlContentLines.join('\n');
  document->setHtml(htmlContent);
  document->print(&printer);
  htmlFile.close();
  delete document;
  return 0;
}

void MainWindow::on_pushButton_8_clicked()
{
    QString current_path=QDir::currentPath();
    QString fileName= QFileDialog::getSaveFileName(this, tr("Generate Report"), "report.pdf", tr("Documents (*.pdf)"));

    int a=-9;
    if (task_PD){
        a=printpdf(current_path+report, fileName);

    }

    if (task_AD){
        a=printpdfAD(current_path+reportAD, fileName);

    }
    qDebug()<<a;


}

void MainWindow::on_pushButton_6_clicked()
{
    player->setMedia(QUrl::fromLocalFile(file_2play));
    player->setVolume(50);
    player->play();
}

void MainWindow::on_pushButton_9_clicked()
{
    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../evaluation/";
    QStringList   ls;
    QString  va, line;
    QProcess cmd;
    if (flagCuted==0){
    if (!QDir(path_patient).exists() || path_patient==""){
     ls = QFileDialog::getOpenFileNames(this, choosecont, "", "Audio-Files(*.wav)");
     va=ls[0];
    }
    else{
        va=path_patient+"/DDK.wav";
    }}
    else{va=current_path+"/../test.wav";}

    if (flagCuted==1){flagCuted=0;}

    file_2play=va;
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));
    QPen pen;
    ui->customPlot->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->replot();
    QVector<double> y=readWavFile(va);
    QVector<double> xt((int)y.size());
    for (int i=0; i<(int)y.size(); i++)
    {
      xt[i] = ((double)i)/fs;
    }
    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setData(xt,y);
    ui->customPlot->xAxis->setRange(0, (double)xt.size()/fs);
    ui->customPlot->yAxis->setRange(-1, 1);
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot->xAxis->setLabel(xaxistime);
    ui->customPlot->yAxis->setLabel(yaxisA);
    ui->customPlot->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->graph()->setPen(pen);
    ui->customPlot->replot();
    xmain=xt;
    ymain=y;
    namemain=va;

    QString comandpv="python "+path_base+mfdaScript+path_base+' '+va;
    qDebug() <<comandpv;
    cmd.start(comandpv);
    cmd.waitForFinished(-1);

    QString predicted=path_base+"/predmFDA.txt";
    QList<QString> feat;
    QFile filepred(predicted);
    if (!filepred.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() <<"error reading the file";
    while (!filepred.atEnd()) {
         line = filepred.readLine();
        feat.append(line);
    }
    ui->label_140->setText(feat[0]);
    ui->label_119->setText(feat[1]);
    ui->label_123->setText(feat[2]);
    ui->label_125->setText(feat[3]);
    ui->label_130->setText(feat[4]);
    ui->label_133->setText(feat[5]);
    ui->label_135->setText(feat[6]);
    ui->label_145->setText(feat[7]);
    ui->evalmFDA->setPixmap(QPixmap( path_base+"prediction.png" ) );
    ui->evalmFDA->setScaledContents(true);
    ui->evalmFDA->show();
}

void MainWindow::on_pushButton_7_clicked()
{
    int current_row=ui->listWidget->currentRow();
    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../audio_examples/";
    QString rowstr;
    QString file_play="";
    if (task_PD){
        file_play=path_base+'n'+rowstr.number(current_row)+".wav";

    }
    else if (task_AD){
        file_play=path_base+'a'+rowstr.number(current_row)+".wav";
    }
    player->setMedia(QUrl::fromLocalFile(file_play));
    player->setVolume(50);
    player->play();

}

void MainWindow::on_pushButton_10_clicked()
{

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));
    QPen pen;

    QProcess cmd;
    QStringList ls;
    QString va;
    qDebug()<<path_patient;
    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../DDK/";

    if (flagCuted==0){
    if (!QDir(path_patient).exists() || path_patient==""){
        ls = QFileDialog::getOpenFileNames(this, choosecont, "", "Audio-Files(*.wav)");
        va=ls[0];
    }
    else{
        va=path_patient+"/DDK1.wav";
    }}
    else{va=current_path+"/../test.wav";}
    if (flagCuted==1){flagCuted=0;}

    file_2play=va;

    QString comandpv="python "+path_base+ddkScript+va+" "+path_base+"featf0.txt "+ path_base+"featEnergy.txt "+path_base+"feat.txt "+path_base;
    cmd.start(comandpv);
    cmd.waitForFinished(-1);

    qDebug() << comandpv;
    QVector<double> y=readWavFile(va);
    QVector<double> x((int)y.size());
    for (int i=0; i<(int)y.size(); i++){
      x[i] = ((double)i)/fs;
    }
     ui->customPlot_10->clearGraphs();
    ui->customPlot_10->addGraph();
    ui->customPlot_10->graph()->setData(x,y);
    ui->customPlot_10->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_10->yAxis->setRange(-1, 1);
    ui->customPlot_10->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_10->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_10->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_10->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_10->xAxis->setLabel(xaxistime);
    ui->customPlot_10->yAxis->setLabel(yaxisA);
    ui->customPlot_10->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_10->graph()->setPen(pen);
    ui->customPlot_10->replot();
    //ui->customPlot_10->savePng(current_path+"/../DDK.png");
    ui->radarDDK->setPixmap(QPixmap( path_base+"DDKradar.png" ) );
    ui->radarDDK->setScaledContents(true);
    ui->radarDDK->show();

    QList<QString> feat;
    QFile filefeat(path_base+"feat.txt");
    qDebug() << "filefeat: " << current_path+"feat.txt";
    if (!filefeat.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    while (!filefeat.atEnd()) {
        QString line = filefeat.readLine();
        feat.append(line);
    }
    QVector<double> F0;
    QVector<double> time;
    QFile fileF0(path_base+"featf0.txt");
    qDebug() << "fileF0: " << path_base+"featf0.txt";
    if (!fileF0.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    int nframes=0;
    QString mmF0=feat[11];
    while (!fileF0.atEnd()) {
        QString line = fileF0.readLine();
        F0.append(line.toDouble());
        time.append((double)nframes/50);
        nframes++;
    }
    qDebug() << "Nframes: " << nframes;
    ui->customPlot_11->clearGraphs();
    ui->customPlot_11->addGraph();
    ui->customPlot_11->graph()->setData(time,F0);
    ui->customPlot_11->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_11->yAxis->setRange(0, mmF0.toDouble());
    ui->customPlot_11->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_11->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_11->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_11->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_11->xAxis->setLabel(xaxistime);
    ui->customPlot_11->yAxis->setLabel(yaxisfreq);
    ui->customPlot_11->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_11->graph()->setPen(pen);
    ui->customPlot_11->replot();
    //ui->customPlot_11->savePng(current_path+"/../prosody2.png");

    QVector<double> logE;
    QVector<double> timeEn;
    QFile filelogE(path_base+"featEnergy.txt");
    if (!filelogE.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    nframes=0;
    QString mmlogE=feat[4];
    while (!filelogE.atEnd()) {
      QString  line = filelogE.readLine();
        logE.append(line.toDouble());
        timeEn.append((double)nframes/100);
        nframes++;
    }
    qDebug() << "Nframes: " << nframes;
    ui->customPlot_12->clearGraphs();
    ui->customPlot_12->addGraph();
    ui->customPlot_12->graph()->setData(timeEn,logE);
    ui->customPlot_12->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_12->yAxis->setRange(0, qPow(10, 0.1*mmlogE.toDouble()));
    ui->customPlot_12->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_12->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_12->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_12->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_12->xAxis->setLabel(xaxistime);
    ui->customPlot_12->yAxis->setLabel(yaxislogE);
    ui->customPlot_12->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_12->graph()->setPen(pen);
    ui->customPlot_12->replot();
    //ui->customPlot_12->savePng(current_path+"/../prosody3.png");

    //feat=[F0varsemi, sF0, mlogE, slogE, mmlogE, DDKrate, avgdurddk, stddurddk, Silrate, avgdurs, stddurs]
    ui->label_167->setText(feat[0]); // fo variability in semitones
    ui->label_159->setText(feat[1]); // std f0
    ui->label_165->setText(feat[2]); // average energy ddk
    ui->label_160->setText(feat[3]); // std energy
    ui->label_168->setText(feat[4]); // max energy
    ui->label_169->setText(feat[5]); // DDK rate
    ui->label_163->setText(feat[7]); // DDK regularity
    ui->label_166->setText(feat[6]); // avg duration DDK
    ui->label_161->setText(feat[8]); // pause_rate
    ui->label_162->setText(feat[9]); // avg dur pause
    ui->label_164->setText(feat[10]); // regularity_pauses

    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setData(x,y);
    ui->customPlot->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot->yAxis->setRange(-1, 1);
    ui->customPlot->xAxis->setLabel(xaxistime);
    ui->customPlot->yAxis->setLabel(yaxisA);
    ui->customPlot->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->graph()->setPen(pen);
    xmain=x;
    ymain=y;
    ui->customPlot->replot();
    xrange=ui->customPlot->xAxis->range();
    namemain=va;
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);

    if (ref_female){
        if (feat[0].toDouble()>varF0semiddku_f  || feat[0].toDouble()<varF0semiddkd_f){ui->label_167->setPalette(sample_palette);}
        if (feat[1].toDouble()>stdf0ddku_f  || feat[1].toDouble()<stdf0ddkd_f){ui->label_159->setPalette(sample_palette);}
        if (feat[2].toDouble()>avgEddku_f  || feat[2].toDouble()<avgEddkd_f){ui->label_165->setPalette(sample_palette);}
        if (feat[3].toDouble()>stdEddku_f  || feat[3].toDouble()<stdEddkd_f){ui->label_160->setPalette(sample_palette);}
        if (feat[4].toDouble()>maxEddku_f  || feat[4].toDouble()<maxEddkd_f){ui->label_168->setPalette(sample_palette);}
        if (feat[5].toDouble()>DDKrateu_f  || feat[5].toDouble()<DDKrated_f){ui->label_169->setPalette(sample_palette);}
        if (feat[6].toDouble()>avgdurDDKu_f  || feat[6].toDouble()<avgdurDDKd_f){ui->label_166->setPalette(sample_palette);}
        if (feat[7].toDouble()>DDKregu_f  || feat[7].toDouble()<DDKregd_f){ui->label_163->setPalette(sample_palette);}
        if (feat[8].toDouble()>Srateddku_f  || feat[8].toDouble()<Srateddkd_f){ui->label_161->setPalette(sample_palette);}
        if (feat[9].toDouble()>avgdurSddku_f  || feat[9].toDouble()<avgdurSddkd_f){ui->label_162->setPalette(sample_palette);}
        if (feat[10].toDouble()>stddurSddku_f  || feat[10].toDouble()<stddurSddkd_f){ui->label_164->setPalette(sample_palette);}

        ui->label_192->setText(QString{ "%1->%2" }.arg( varF0semiddkd_f).arg(varF0semiddku_f));
        ui->label_193->setText(QString{ "%1->%2" }.arg( stdf0ddkd_f).arg(stdf0ddku_f));
        ui->label_194->setText(QString{ "%1->%2" }.arg( avgEddkd_f).arg(avgEddku_f));
        ui->label_195->setText(QString{ "%1->%2" }.arg( stdEddkd_f).arg(stdEddku_f));
        ui->label_196->setText(QString{ "%1->%2" }.arg( maxEddkd_f).arg(maxEddku_f));
        ui->label_197->setText(QString{ "%1->%2" }.arg( DDKrated_f).arg(DDKrateu_f));
        ui->label_199->setText(QString{ "%1->%2" }.arg( avgdurDDKd_f).arg(avgdurDDKu_f));
        ui->label_198->setText(QString{ "%1->%2" }.arg( DDKregd_f).arg(DDKregu_f));
        ui->label_200->setText(QString{ "%1->%2" }.arg( Srateddkd_f).arg(Srateddku_f));
        ui->label_201->setText(QString{ "%1->%2" }.arg( avgdurSddkd_f).arg(avgdurSddku_f));
        ui->label_202->setText(QString{ "%1->%2" }.arg( stddurSddkd_f).arg(stddurSddku_f));


    }
    else{
        if (feat[0].toDouble()>varF0semiddku_m  || feat[0].toDouble()<varF0semiddkd_m){ui->label_167->setPalette(sample_palette);}
        if (feat[1].toDouble()>stdf0ddku_m  || feat[1].toDouble()<stdf0ddkd_m){ui->label_159->setPalette(sample_palette);}
        if (feat[2].toDouble()>avgEddku_m  || feat[2].toDouble()<avgEddkd_m){ui->label_165->setPalette(sample_palette);}
        if (feat[3].toDouble()>stdEddku_m  || feat[3].toDouble()<stdEddkd_m){ui->label_160->setPalette(sample_palette);}
        if (feat[4].toDouble()>maxEddku_m  || feat[4].toDouble()<maxEddkd_m){ui->label_168->setPalette(sample_palette);}
        if (feat[5].toDouble()>DDKrateu_m  || feat[5].toDouble()<DDKrated_m){ui->label_169->setPalette(sample_palette);}
        if (feat[6].toDouble()>avgdurDDKu_m  || feat[6].toDouble()<avgdurDDKd_m){ui->label_166->setPalette(sample_palette);}
        if (feat[7].toDouble()>DDKregu_m  || feat[7].toDouble()<DDKregd_m){ui->label_163->setPalette(sample_palette);}
        if (feat[8].toDouble()>Srateddku_m  || feat[8].toDouble()<Srateddkd_m){ui->label_161->setPalette(sample_palette);}
        if (feat[9].toDouble()>avgdurSddku_m  || feat[9].toDouble()<avgdurSddkd_m){ui->label_162->setPalette(sample_palette);}
        if (feat[10].toDouble()>stddurSddku_m  || feat[10].toDouble()<stddurSddkd_m){ui->label_164->setPalette(sample_palette);}


        ui->label_192->setText(QString{ "%1->%2" }.arg( varF0semiddkd_m).arg(varF0semiddku_m));
        ui->label_193->setText(QString{ "%1->%2" }.arg( stdf0ddkd_m).arg(stdf0ddku_m));
        ui->label_194->setText(QString{ "%1->%2" }.arg( avgEddkd_m).arg(avgEddku_m));
        ui->label_195->setText(QString{ "%1->%2" }.arg( stdEddkd_m).arg(stdEddku_m));
        ui->label_196->setText(QString{ "%1->%2" }.arg( maxEddkd_m).arg(maxEddku_m));
        ui->label_197->setText(QString{ "%1->%2" }.arg( DDKrated_m).arg(DDKrateu_m));
        ui->label_199->setText(QString{ "%1->%2" }.arg( avgdurDDKd_m).arg(avgdurDDKu_m));
        ui->label_198->setText(QString{ "%1->%2" }.arg( DDKregd_m).arg(DDKregu_m));
        ui->label_200->setText(QString{ "%1->%2" }.arg( Srateddkd_m).arg(Srateddku_m));
        ui->label_201->setText(QString{ "%1->%2" }.arg( avgdurSddkd_m).arg(avgdurSddku_m));
        ui->label_202->setText(QString{ "%1->%2" }.arg( stddurSddkd_m).arg(stddurSddku_m));



    }


    if(ref_female){
        ui->radarDDK->setPixmap(QPixmap( path_base+"DDKradarfemale.png" ) );
    }
    else{
        ui->radarDDK->setPixmap(QPixmap( path_base+"DDKradarmale.png" ) );
    }



}

void MainWindow::on_radioButton_1_clicked()
{
    ref_female=true;
    ref_male=false;
}

void MainWindow::on_radioButton_2_clicked()
{
    ref_female=false;
    ref_male=true;
}


void MainWindow::on_radioButton_5_clicked()
{
    task_AD=true;
    task_PD=false;
    ui->listWidget->clear();
    QStringList list;
    list<<listTasksAD;
    ui->listWidget->addItems(list);


    if (task_AD)
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_10->setEnabled(false);
        ui->pushButton_4->setEnabled(false);
        ui->pushButton_9->setEnabled(false);




}

void MainWindow::on_radioButton_6_clicked()
{
    task_AD=false;
    task_PD=true;
    ui->listWidget->clear();
    QStringList list;
    list<<listTasks;
    ui->listWidget->addItems(list);

    if (task_PD){
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_10->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
        ui->pushButton_9->setEnabled(true);
    }
}



void MainWindow::on_radioButton_4_clicked() // f0 in Hz
{

    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../phonVowels/";
    QVector<double> F0;
    QVector<double> time;
    QFile fileF0(path_base+"featf0.txt");
    if (!fileF0.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    int nframes=0;
    double maxF0=0;
    while (!fileF0.atEnd()) {
        QString line = fileF0.readLine();
        F0.append(line.toDouble());
        if (line.toDouble()>maxF0){maxF0=line.toDouble();}
        time.append((double)nframes/100);
        nframes++;
    }
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));


    QPen pen;

    ui->customPlot_3->clearGraphs();
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph()->setData(time,F0);

    ui->customPlot_3->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->xAxis->setLabel(xaxistime);
    ui->customPlot_3->yAxis->setLabel(yaxisfreq);
    ui->customPlot_3->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_3->graph()->setPen(pen);
    ui->customPlot_3->replot();
}

void MainWindow::on_radioButton_3_clicked() // f0 in semitones
{
    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../phonVowels/";
    QVector<double> F0;
    QVector<double> time;
    QFile fileF0(path_base+"featf0semi.txt");
    if (!fileF0.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    int nframes=0;
    double maxF0=0;
    while (!fileF0.atEnd()) {
        QString line = fileF0.readLine();
        F0.append(line.toDouble());
        if (line.toDouble()>maxF0){maxF0=line.toDouble();}
        time.append((double)nframes/100);
        nframes++;
    }


    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));

    QPen pen;

    ui->customPlot_3->clearGraphs();
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph()->setData(time,F0);
    ui->customPlot_3->xAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->yAxis->setTickLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->customPlot_3->xAxis->setLabel(xaxistime);
    ui->customPlot_3->yAxis->setLabel(yaxissemi);
    ui->customPlot_3->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_3->graph()->setPen(pen);
    ui->customPlot_3->replot();
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    int current_row=ui->listWidget->currentRow();


    QString name=get_name(current_row, path_patient, false);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{

}
