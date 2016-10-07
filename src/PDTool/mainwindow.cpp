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
#include <stdio.h>
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
    //audioProbe = new QAudioProbe();

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(230, 230, 230));
    plotGradient.setColorAt(1, QColor(220, 220, 220));
    QPen pen;
    ui->customPlot->setBackground(plotGradient);
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
    QPixmap pix("../PDTool/button1.png");
    QIcon icon(pix);
    ui->pushButton->setIcon(icon);
    ui->pushButton->setIconSize(pix.size());

    QPixmap pix2("../PDTool/button2.png");
    QIcon icon2(pix2);
    ui->pushButton_2->setIcon(icon2);
    ui->pushButton_2->setIconSize(pix2.size());

    QPixmap pix3("../PDTool/button3.png");
    QIcon icon3(pix3);
    ui->pushButton_3->setIcon(icon3);
    ui->pushButton_3->setIconSize(pix3.size());

    QPixmap pix4("../PDTool/button4.png");
    QIcon icon4(pix4);
    ui->pushButton_4->setIcon(icon4);
    ui->pushButton_4->setIconSize(pix4.size());

    QPixmap pix9("../PDTool/button5.png");
    QIcon icon9(pix9);
    ui->pushButton_9->setIcon(icon9);
    ui->pushButton_9->setIconSize(pix9.size());

    QPixmap pixlogo("../PDTool/Logo.png");
    ui->label_68->setPixmap(pixlogo);

    QPixmap pix5("../PDTool/button5rec.png");
    QIcon icon5(pix5);
    ui->pushButton_5->setIcon(icon5);
    ui->pushButton_5->setIconSize(QSize(71,71));
    ui->pushButton_5->setCheckable(true);

    QPixmap pix6("../PDTool/button6play.png");
    QIcon icon6(pix6);
    ui->pushButton_6->setIcon(icon6);
    ui->pushButton_6->setIconSize(QSize(71,71));
    ui->pushButton_6->setCheckable(true);

    QStringList list;
    list<<"/A/"<<"/E/"<<"/I/"<<"/O/"<<"/U/"<<"Modulated Vowels"<<"Pa-Ta-Ka"<<"Pa-Ka-Ta"<<"Pe-Ta-Ka"<<"Pa"<<"Ta"<<
            "Ka"<<"Words"<<"sentence 1"<<"sentence 2"<<"sentence 3"<<"sentence 4"<<"sentence 5"<<"sentence 6"<<
            "sentence 7"<<"sentence 8"<<"sentence 9"<<"sentence 10"<<"Read Text"<<"Monologue";

    /*QString task1="/A/";
    QString task13="/E/";
    QString task2="/I/";
    QString task14="/O/";
    QString task3="/U/";
    QString task20="Modulated Vowels";
    QString task4="Pa-Ta-Ka";
    QString task15="Pa-Ka-Ta";
    QString task16="Pe-Ta-Ka";
    QString task17="Pa";
    QString task18="Ta";
    QString task18="Ka";
    QString task19="Words";
    QString task5="sentence 1";
    QString task6="sentence 2";
    QString task7="sentence 3";
    QString task8="sentence 4";
    QString task9="sentence 5";
    QString task10="sentence 6";
    QString task21="sentence 7";
    QString task22="sentence 8";
    QString task23="sentence 9";
    QString task24="sentence 10";
    QString task11="Readtext";
    QString task12="Monologue";

    QStringList list;
    list <<task1;
    list.append(task13);
    list.append(task2);
    list.append(task14);
    list.append(task3);
    list.append(task20);

    list.append(task4);
    list.append(task5);
    list.append(task6);
    list.append(task7);
    list.append(task8);
    list.append(task9);
    list.append(task10);
    list.append(task11);
    list.append(task12);
*/
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
    if (remove("../intelligibility/pred1.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred2.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred3.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred4.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred5.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred6.txt")!=0){perror("error deleting file");}
    if (remove("../intelligibility/pred7.txt")!=0){perror("error deleting file");}
    if (remove("../phonation1.png")!=0){perror("error deleting file");}
    if (remove("../phonation2.png")!=0){perror("error deleting file");}
    if (remove("../articulation1.png")!=0){perror("error deleting file");}
    if (remove("../articulation2.png")!=0){perror("error deleting file");}
    if (remove("../prosody1.png")!=0){perror("error deleting file");}
    if (remove("../prosody2.png")!=0){perror("error deleting file");}
    if (remove("../prosody3.png")!=0){perror("error deleting file");}
    if (remove("../test.wav")!=0){perror("error deleting file");}



}

MainWindow::~MainWindow()
{
    delete ui;
    delete audioRecorder;
    delete probe;
    //delete audioProbe;
}


void MainWindow::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

    if (ui->customPlot->graphCount() > 0)
      menu->addAction("Clear graph", this, SLOT(removeAllGraphs()));


  menu->popup(ui->customPlot->mapToGlobal(pos));
}


void MainWindow::removeAllGraphs()
{QPen pen;
    QLinearGradient plotGradient;

  ui->customPlot->clearGraphs();
  ui->customPlot->addGraph();
  ui->customPlot->graph()->setData(xmain,ymain);
  ui->customPlot->xAxis->setRange(0, (double)xmain.size()/fs);
  ui->customPlot->yAxis->setRange(-1, 1);
  ui->customPlot->xAxis->setLabel("Time (sec)");
  ui->customPlot->yAxis->setLabel("Amplitude");
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
      // write_wav(, BUF_SIZE, buffer, fs);
   }
   //if (contAxes>2){contAxes=0;}
   ;
}


void MainWindow::opendialogb(){
    dialogb=new Dialogb(this);
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
        ls = QFileDialog::getOpenFileNames(this, "Choose file of vowel /A/", "", "Audio-Files(*.wav)");
        va=ls[0];
    }
    else if (flagCuted==1){va=current_path+"/../test.wav"; }
    else{
        va=path_patient+"/A.wav";
    }

    if (flagCuted==1){flagCuted=0;}
    file_2play=va;


    QString comandpv="python "+path_base+"phonVowels.py "+va+" "+path_base+"featf0.txt "+path_base+"feat.txt "+path_base;
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
    ui->customPlot_2->xAxis->setLabel("Time (sec)");
    ui->customPlot_2->yAxis->setLabel("Amplitude");
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
    ui->customPlot->xAxis->setLabel("Time (sec)");
    ui->customPlot->yAxis->setLabel("Amplitude");
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
    qDebug() << "fileF0: " << current_path+"featf0.txt";
    if (!fileF0.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    int nframes=0;
    while (!fileF0.atEnd()) {
        QString line = fileF0.readLine();
        F0.append(line.toDouble());
        time.append((double)nframes/100);
        nframes++;
    }
    qDebug() << "Nframes: " << nframes;
    ui->customPlot_3->clearGraphs();
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph()->setData(time,F0);
    ui->customPlot_3->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_3->yAxis->setRange(0, 250);
    ui->customPlot_3->xAxis->setLabel("Time (sec)");
    ui->customPlot_3->yAxis->setLabel("Fundamental Frequency (Hz)");
    ui->customPlot_3->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot_3->graph()->setPen(pen);

    ui->customPlot_3->replot();
    ui->customPlot_3->savePng(current_path+"/../phonation2.png");
    QList<QString> feat;
    QFile filefeat(path_base+"feat.txt");
    qDebug() << "filefeat: " << current_path+"feat.txt";
    if (!filefeat.open(QIODevice::ReadOnly | QIODevice::Text))
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
    //QString CD=feat[5];
    ui->label_8->setText(jitter);
    ui->label_9->setText(shimmer);
    ui->label_10->setText(APQ);
    ui->label_11->setText(PPQ);
    ui->label_12->setText(logE);
    //ui->label_13->setText(CD);

    if (jitter.toDouble()>jitterref){
         ui->label_8->setPalette(sample_palette);
    }

    if (shimmer.toDouble()>shimmerrefu  || shimmer.toDouble()<shimmerrefd){
        ui->label_9->setPalette(sample_palette);
   }

    if (APQ.toDouble()>APQrefu  || APQ.toDouble()<APQrefd){
        ui->label_10->setPalette(sample_palette);
   }

    if (PPQ.toDouble()>PPQrefu  || PPQ.toDouble()<PPQrefd){
        ui->label_11->setPalette(sample_palette);
   }

    if (logE.toDouble()>logErefu  || logE.toDouble()<logErefd){
        ui->label_12->setPalette(sample_palette);
   }

    xrange=ui->customPlot->xAxis->range();
    ui->radarPhon->setPixmap(QPixmap( path_base+"phonation2.png" ) );
    ui->radarPhon->setScaledContents(true);
    ui->radarPhon->show();
    //sample_label->setAutoFillBackground(true);

//sample_label->setText("What ever text");



 /*   QVector<double> x1;
    QVector<double> x2;

    QFile fileatr(path_base+"atractor.txt");
    if (!fileatr.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QStringList list;
    while (!fileatr.atEnd()) {
        QString line = fileatr.readLine();
        list=line.split(' ');
        QString x1str=list[0];
        x1.append(x1str.toDouble());
        QString x2str=list[1];
        x2.append(x2str.toDouble());
    }

    ui->customPlot_4->addGraph();
    ui->customPlot_4->graph()->setData(x1,x2);
    ui->customPlot_4->xAxis->setRange(-1, 1);
    ui->customPlot_4->yAxis->setRange(-1, 1);
    ui->customPlot_4->xAxis->setLabel("x[t]");
    ui->customPlot_4->yAxis->setLabel("x[t+tau]");
    ui->customPlot_4->replot();
    */
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
        lsa = QFileDialog::getOpenFileNames(this, "Choose file of vowel /A/", "", "Audio-Files(*.wav)");
        lse = QFileDialog::getOpenFileNames(this, "Choose file of vowel /E/", "", "Audio-Files(*.wav)");
        lsi = QFileDialog::getOpenFileNames(this, "Choose file of vowel /I/", "", "Audio-Files(*.wav)");
        lso = QFileDialog::getOpenFileNames(this, "Choose file of vowel /O/", "", "Audio-Files(*.wav)");
        lsu = QFileDialog::getOpenFileNames(this, "Choose file of vowel /U/", "", "Audio-Files(*.wav)");
        lsm = QFileDialog::getOpenFileNames(this, "Choose file of continuous speech", "", "Audio-Files(*.wav)");
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
    ui->customPlot->xAxis->setLabel("Time (sec)");
    ui->customPlot->yAxis->setLabel("Amplitude");
    ui->customPlot->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->graph()->setPen(pen);

    ui->customPlot->replot();
    xmain=xt;
    ymain=y;
    namemain=monologue;

    QString path_base=current_path+"/../artVowels/";
    QString comandpv="python "+path_base+"artVowels.py "+va+" "+ve+" "+vi+" "+vo+" "+vu+" "+path_base+"Fa.txt "+path_base+"Fe.txt "+path_base+"Fi.txt "+path_base+"Fo.txt "+path_base+"Fu.txt "+path_base+"feat.txt "+path_base;
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
    //qDebug() << "filefeat: " << path_base+"feat.txt";
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


    if (VSA.toDouble()>vsau  || VSA.toDouble()<vsad){
        ui->label_28->setPalette(sample_palette);
    }

    if (VPA.toDouble()>vpau  || VPA.toDouble()<vpad){
        ui->label_29->setPalette(sample_palette);
    }

    if (FCR.toDouble()>fcru  || FCR.toDouble()<fcrd){
        ui->label_31->setPalette(sample_palette);
    }

    if (mF1a.toDouble()>f1au  || mF1a.toDouble()<f1ad){
        ui->label_23->setPalette(sample_palette);
    }

    if (mF2a.toDouble()>f2au  || mF2a.toDouble()<f2ad){
        ui->label_17->setPalette(sample_palette);
    }

    if (mF1i.toDouble()>f1iu  || mF1i.toDouble()<f1id){
        ui->label_25->setPalette(sample_palette);
    }

    if (mF2i.toDouble()>f2iu  || mF2i.toDouble()<f2id){
        ui->label_16->setPalette(sample_palette);
    }

    if (mF1u.toDouble()>f1uu  || mF1u.toDouble()<f1ud){
        ui->label_18->setPalette(sample_palette);
    }

    if (mF2u.toDouble()>f2uu  || mF2u.toDouble()<f2ud){
        ui->label_20->setPalette(sample_palette);
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
    ui->customPlot_4->xAxis->setLabel("Formant 1(Hz)");
    ui->customPlot_4->yAxis->setLabel("Formant 2 (Hz)");
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
    ui->customPlot_5->xAxis->setLabel("Formant 1(Hz)");
    ui->customPlot_5->yAxis->setLabel("Formant 2 (Hz)");
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
    comandpv="python "+path_base+"artCont.py "+monologue+" "+path_base+"feat.txt "+path_base;
    qDebug()<< comandpv;

    cmd.start(comandpv);
    cmd.waitForFinished(-1);

    ui->radarArt->setPixmap(QPixmap( path_base+"articulation.png" ) );
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
    ui->customPlot_6->xAxis->setLabel("Number of bark band");
    ui->customPlot_6->yAxis->setLabel("log-Energy");
    ui->customPlot_6->graph(0)->setDataValueError(x, BBEonm, BBEons);
    ui->customPlot_6->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    ui->customPlot_6->graph(0)->setPen(QPen(QColor(0, 120, 0), 2));

    //ui->customPlot_6->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    //ui->customPlot_6->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, Qt::white, 7));
    ui->customPlot_6->graph(0)->setErrorType(QCPGraph::etValue);
    ui->customPlot_6->graph(0)->setName("Onset");
    ui->customPlot_6->addGraph();
    ui->customPlot_6->graph(1)->setDataValueError(x, BBEofm, BBEofs);
    ui->customPlot_6->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::red, 1.5), QBrush(Qt::white), 9));
    ui->customPlot_6->graph(1)->setPen(QPen(QColor(0, 180, 0), 2));


    //ui->customPlot_6->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    //ui->customPlot_6->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));
    ui->customPlot_6->graph(1)->setErrorType(QCPGraph::etValue);
    ui->customPlot_6->graph(1)->setName("Offset");


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

    QString name=get_name(current_row, path_patient);
    //QString name2=name.replace(QString("wav"), QString("raw"));
    if (checked){
        //QPixmap pix5("../PDTool/button5stop.png");
        //QIcon icon5(pix5);
        //ui->pushButton_5->setIcon(icon5);
        //ui->pushButton_5->setIconSize(QSize(71,71));

        //opendialogb();
        // destinationFile.setFileName(name);
        // destinationFile.open( QIODevice::WriteOnly | QIODevice::Truncate );

        // audio->start(&destinationFile);
        QStringList codecName=audioRecorder->supportedAudioCodecs();
        qDebug() <<codecName;
        QStringList inputs=audioRecorder->audioInputs();
        if (audioRecorder->state() == QMediaRecorder::StoppedState) {
            QAudioEncoderSettings settings;
            //qDebug() << audioRecorder->audioInputs();
            audioRecorder->setAudioInput(inputs[0]);
            settings.setCodec("audio/pcm");
            //settings.setCodec(codecName[0]);
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
            //QPixmap pix5("../PDTool/button5rec.png");
            //QIcon icon5(pix5);
            //ui->pushButton_5->setIcon(icon5);
            //ui->pushButton_5->setIconSize(QSize(71,71));

            audioRecorder->stop();
            //name="/home/camilo/Camilo/Parkinson/PDTool/data/2016_05_05vasquezcamilo/A.wav";
            QVector<double> y=readWavFile(name);
            file_2play=name;
            QVector<double> x((int)y.size());
            for (int i=0; i<(int)y.size(); i++)
            {
              x[i] = ((double)i)/fs;
            }
             ui->customPlot->clearGraphs();
            ui->customPlot->addGraph();
            ui->customPlot->graph()->setData(x,y);
            ui->customPlot->xAxis->setRange(0, (double)x.size()/fs);
            ui->customPlot->yAxis->setRange(-1, 1);
            ui->customPlot->xAxis->setLabel("Time (sec)");
            ui->customPlot->yAxis->setLabel("Amplitude");
            ui->customPlot->graph()->setPen(pen);
            ui->customPlot->replot();
            xmain=x;
            ymain=y;

        }
    xrange=ui->customPlot->xAxis->range();

}




void MainWindow::on_pushButton_6_toggled(bool checked)
{   if (checked){int a=1;};
    /*
    if (checked){
        QPixmap pix6("/home/camilo/Camilo/Parkinson/PDTool/src/PDTool/button6pause.png");
        QIcon icon6(pix6);
        ui->pushButton_6->setIcon(icon6);
        ui->pushButton_6->setIconSize(QSize(71,71));
        player->setMedia(QUrl::fromLocalFile(file_2play));
        player->setVolume(50);
        player->play();
    }
    else{
            QPixmap pix6("/home/camilo/Camilo/Parkinson/PDTool/src/PDTool/button6play.png");
            QIcon icon6(pix6);
            ui->pushButton_6->setIcon(icon6);
            ui->pushButton_6->setIconSize(QSize(71,71));
        }*/
}


void MainWindow::on_pushButton_4_clicked()
{
    QProcess cmd;
    QStringList ls1, ls2, ls3, ls4, ls5, ls6, ls7, ls8, ls9, ls10, lrt;
    QString sent1, sent2, sent3, sent4, sent5, sent6, sent7, sent8, sent9, sent10, readtext;

    QString current_path=QDir::currentPath();
    QString path_base=current_path+"/../intelligibility/";


    if (!QDir(path_patient).exists() || path_patient=="") {
        ls1 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 1 (Mi Casa)", "", "Audio-Files(*.wav)");
        ls2 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 2 (Omar)", "", "Audio-Files(*.wav)");
        ls3 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 3 (Laura)", "", "Audio-Files(*.wav)");
        ls4 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 4 (Los libros)", "", "Audio-Files(*.wav)");
        ls5 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 5 (Rosita)", "", "Audio-Files(*.wav)");
        ls6 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 6 (Luisa)", "", "Audio-Files(*.wav)");
        ls7 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 7 (Viste las noticias)", "", "Audio-Files(*.wav)");
        ls8 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 8 (Juan)", "", "Audio-Files(*.wav)");
        ls9 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 9 (Estoy muy triste)", "", "Audio-Files(*.wav)");
        ls10 = QFileDialog::getOpenFileNames(this, "Choose file of sentence 10 (Estoy muy preocupado)", "", "Audio-Files(*.wav)");

        lrt = QFileDialog::getOpenFileNames(this, "Choose file of Read text", "", "Audio-Files(*.wav)");
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
        readtext=path_patient+"/Readtext.wav";
    }


    file_2play=readtext;

    QString comand1="python "+path_base+"intelligibility.py "+sent1+" "+path_base+"mi_casa.txt "+path_base+"pred1.txt "+path_base+"feat1.txt es_CO";
    qDebug() << comand1;
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+sent2+" "+path_base+"omar.txt "+path_base+"pred2.txt "+path_base+"feat2.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+sent3+" "+path_base+"laura.txt "+path_base+"pred3.txt "+path_base+"feat3.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+sent4+" "+path_base+"loslibros.txt "+path_base+"pred4.txt "+path_base+"feat4.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+sent5+" "+path_base+"rosita.txt "+path_base+"pred5.txt "+path_base+"feat5.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+sent6+" "+path_base+"luisa.txt "+path_base+"pred6.txt "+path_base+"feat6.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+sent7+" "+path_base+"viste.txt "+path_base+"pred7.txt "+path_base+"feat7.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+sent8+" "+path_base+"juan.txt "+path_base+"pred8.txt "+path_base+"feat8.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+sent9+" "+path_base+"triste.txt "+path_base+"pred9.txt "+path_base+"feat9.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+sent10+" "+path_base+"preocupado.txt "+path_base+"pred10.txt "+path_base+"feat10.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);

    comand1="python "+path_base+"intelligibility.py "+readtext+" "+path_base+"readtext.txt "+path_base+"pred11.txt "+path_base+"feat11.txt es_CO";
    cmd.start(comand1);
    cmd.waitForFinished(-1);


    comand1="python "+path_base+"intelligibilityRadar.py "+path_base+"feat1.txt "+path_base+"feat2.txt "+path_base+"feat3.txt "+path_base+"feat4.txt "+path_base+"feat5.txt "+path_base+"feat6.txt "+path_base+"feat7.txt "+path_base+"feat8.txt "+path_base+"feat9.txt "+path_base+"feat10.txt "+path_base+"feat11.txt";
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
    ui->label_61->setText(predicted[10]+'\r'+predicted[11]+'\r'+predicted[12]+'\r'+predicted[13]);

qDebug() << "Predicciones OK ";
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
qDebug() << "WA OK ";
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
qDebug() << "DTW OK ";
    WAavg=0;
    DTWavg=0;
    for  (int cj=0;cj<11;cj++){
        WAavg+=WA[cj].toFloat();
        DTWavg+=DTW[cj].toFloat();
    }

    WAavg=WAavg/11;
    DTWavg=DTWavg/11;
qDebug() << "AVG OK ";

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
        ls = QFileDialog::getOpenFileNames(this, "Choose file to analyze/", "", "Audio-Files(*.wav)");
        va=ls[0];
    }
    else{
        va=path_patient+"/monologue.wav";
    }}
    else{va=current_path+"/../test.wav";}
    if (flagCuted==1){flagCuted=0;}

    file_2play=va;
    QString comandpv="python "+path_base+"prosody.py "+va+" "+path_base+"featf0.txt "+ path_base+"featEnergy.txt "+path_base+"feat.txt "+path_base;
    cmd.start(comandpv);
    cmd.waitForFinished(-1);

    qDebug() << comandpv;


    QVector<double> y=readWavFile(va);
    QVector<double> x((int)y.size());
    for (int i=0; i<(int)y.size(); i++)
    {
      x[i] = ((double)i)/fs;
    }
     ui->customPlot_7->clearGraphs();
    ui->customPlot_7->addGraph();
    ui->customPlot_7->graph()->setData(x,y);
    ui->customPlot_7->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot_7->yAxis->setRange(-1, 1);
    ui->customPlot_7->xAxis->setLabel("Time (sec)");
    ui->customPlot_7->yAxis->setLabel("Amplitude");
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
    ui->customPlot_8->xAxis->setLabel("Time (sec)");
    ui->customPlot_8->yAxis->setLabel("Fundamental Frequency (Hz)");
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
    ui->customPlot_9->xAxis->setLabel("Time (sec)");
    ui->customPlot_9->yAxis->setLabel("Energy");
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

    ui->label_69->setText(Vrate);
    ui->label_70->setText(avgdurv);
    ui->label_71->setText(stddurv);
    ui->label_72->setText(Silrate);
    ui->label_73->setText(avgdurs);
    ui->label_74->setText(stddurs);
    ui->label_75->setText(mF0);
    ui->label_76->setText(sF0);
    ui->label_77->setText(mmF0);
    ui->label_78->setText(mlogE);
    ui->label_79->setText(slogE);
    ui->label_80->setText(mmlogE);



    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setData(x,y);
    ui->customPlot->xAxis->setRange(0, (double)x.size()/fs);
    ui->customPlot->yAxis->setRange(-1, 1);
    ui->customPlot->xAxis->setLabel("Time (sec)");
    ui->customPlot->yAxis->setLabel("Amplitude");
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


    if (mF0.toDouble()>avgf0u  || mF0.toDouble()<avgf0d){
        ui->label_75->setPalette(sample_palette);
    }

    if (sF0.toDouble()>stdf0u  || sF0.toDouble()<stdf0d){
        ui->label_76->setPalette(sample_palette);
    }

    if (mmF0.toDouble()>maxf0u  || mmF0.toDouble()<maxf0d){
        ui->label_77->setPalette(sample_palette);
    }

    if (mlogE.toDouble()>avgEu  || mlogE.toDouble()<avgEd){
        ui->label_78->setPalette(sample_palette);
    }

    if (slogE.toDouble()>stdEu  || slogE.toDouble()<stdEd){
        ui->label_79->setPalette(sample_palette);
    }

    if (mmlogE.toDouble()>maxEu  || mmlogE.toDouble()<maxEd){
        ui->label_80->setPalette(sample_palette);
    }

    if (Vrate.toDouble()>Vrateu  || Vrate.toDouble()<Vrated){
        ui->label_69->setPalette(sample_palette);
    }

    if (avgdurv.toDouble()>avgdurVu  || avgdurv.toDouble()<avgdurVd){
        ui->label_70->setPalette(sample_palette);
    }

    if (stddurv.toDouble()>stddurVu  || stddurv.toDouble()<stddurVd){
        ui->label_71->setPalette(sample_palette);
    }

    if (Silrate.toDouble()>Srateu  || Silrate.toDouble()<Srated){
        ui->label_72->setPalette(sample_palette);
    }

    if (avgdurs.toDouble()>avgdurSu  || avgdurs.toDouble()<avgdurSd){
        ui->label_73->setPalette(sample_palette);
    }

    if (stddurs.toDouble()>stddurSu  || stddurs.toDouble()<stddurSd){
        ui->label_74->setPalette(sample_palette);
    }
}


QString MainWindow::get_name(int rec_flag, QString folder){
    QString name="";
    switch (rec_flag){
    case 0:
        name=folder+"/A.wav";
        break;
    case 1:
        name=folder+"/E.wav";
        break;
    case 2:
        name=folder+"/I.wav";
        break;
    case 3:
        name=folder+"/O.wav";
        break;
    case 4:
        name=folder+"/U.wav";
        break;
    case 5:
        name=folder+"/ModulatedVowels.wav";
        break;
    case 6:
        name=folder+"/DDK.wav";
        break;
    case 7:
        name=folder+"/DDK2.wav";
        break;
    case 8:
        name=folder+"/DDK3.wav";
        break;
    case 9:
        name=folder+"/DDK4.wav";
        break;
    case 10:
        name=folder+"/DDK5.wav";
        break;
    case 11:
        name=folder+"/DDK6.wav";
        break;
    case 12:
        name=folder+"/words.wav";
        break;
    case 13:
        name=folder+"/sent1.wav";
        break;
    case 14:
        name=folder+"/sent2.wav";
        break;
    case 15:
        name=folder+"/sent3.wav";
        break;
    case 16:
        name=folder+"/sent4.wav";
        break;
    case 17:
        name=folder+"/sent5.wav";
        break;
    case 18:
        name=folder+"/sent6.wav";
        break;
    case 19:
        name=folder+"/sent7.wav";
        break;
    case 20:
        name=folder+"/sent8.wav";
        break;
    case 21:
        name=folder+"/sent9.wav";
        break;
    case 22:
        name=folder+"/sent10.wav";
        break;
    case 23:
        name=folder+"/Readtext.wav";
        break;
    default:
        name=folder+"/Monologue.wav";
        break;
    }
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
{        QPixmap pix5("../PDTool/button5stop.png");
         QIcon icon5(pix5);
         QPixmap pix6("../PDTool/button5rec.png");
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

int MainWindow::printpdf(QString inputhtml, QString otputfile){

    QString current_path=QDir::currentPath();
    QString featuresPhonation=current_path+"/../phonVowels/feat.txt";
QString line;
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
    QString logE=feat[4];
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


    QString featuresProsody=current_path+"/../prosody/feat.txt";
QList<QString> featpros;
    QFile filefeatprosody(featuresProsody);
    if (!filefeatprosody.open(QIODevice::ReadOnly | QIODevice::Text))
        return -3;
    while (!filefeatprosody.atEnd()) {
         line = filefeatprosody.readLine();
        featpros.append(line);
    }
filefeatprosody.close();
    QString mF0=featpros[0];
    QString sF0=featpros[1];

    QString mlogE=featpros[3];
    QString slogE=featpros[4];

    QString Vrate=featpros[6];
    QString avgdurv=featpros[7];
    QString stddurv=featpros[8];
    QString Silrate=featpros[9];
    QString avgdurs=featpros[10];
    QString stddurs=featpros[11];




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



  //const int highQualityDPI = 300;
  QDir::setCurrent(QCoreApplication::applicationDirPath());

  QFile  htmlFile (inputhtml);
  if (!htmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){
      return -5;
  }

  QString htmlContent;
  QStringList htmlContentLines;
  QTextStream in(&htmlFile);
   // htmlContent=in.readAll();
//htmlFile.close();

    while (!in.atEnd()) {
        line = in.readLine();
        htmlContentLines.append(line);
        qDebug() <<line;
    }


htmlContentLines[1].replace("Logo.png", current_path+"/../PDTool/Logo.png");
htmlContentLines[10].replace("phonation1.png", current_path+"/../phonVowels/phonation1.png");
htmlContentLines[10].replace("phonation2.png", current_path+"/../phonVowels/phonation2.png");
htmlContentLines[80].replace("articulation1.png", current_path+"/../artVowels/articulation1.png");
htmlContentLines[80].replace("articulation2.png", current_path+"/../artCont/articulation.png");
htmlContentLines[183].replace("prosody1.png", current_path+"/../prosody/prosody.png");
htmlContentLines[183].replace("prosody2.png", current_path+"/../prosody/prosodyradar.png");
htmlContentLines[308].replace("intelligibility.png", current_path+"/../intelligibility/intelligibility.png");
htmlContentLines[445].replace("prediction.png", current_path+"/../evaluation/prediction.png");


htmlContentLines[5].replace("Nombre:", "Nombre:"+namePatient);
htmlContentLines[6].replace("Apellido:", "Apellido:"+lastPatient);
htmlContentLines[26].replace('-', jitter);
htmlContentLines[37].replace('-', shimmer);
htmlContentLines[48].replace('-', APQ);
htmlContentLines[59].replace('-', PPQ);
htmlContentLines[70].replace('-', logE);


htmlContentLines[96].replace('-', mF1a);
htmlContentLines[107].replace('-', mF2a);
htmlContentLines[118].replace('-', mF1i);
htmlContentLines[129].replace('-', mF2i);
htmlContentLines[140].replace('-', mF1u);
htmlContentLines[151].replace('-', mF2u);
htmlContentLines[162].replace('-', VSA);
htmlContentLines[173].replace('-', FCR);


htmlContentLines[199].replace('-', Vrate);
htmlContentLines[210].replace('-', avgdurv);
htmlContentLines[221].replace('-', stddurv);
htmlContentLines[232].replace('-', Silrate);


htmlContentLines[243].replace('-', avgdurs);
htmlContentLines[254].replace('-', stddurs);
htmlContentLines[265].replace('-', mlogE);
htmlContentLines[276].replace('-', slogE);
htmlContentLines[287].replace('-', mF0);
htmlContentLines[298].replace('-', sF0);
QString waavgstr, dtwavgstr;
htmlContentLines[324].replace('-', waavgstr.setNum(WAavg));
htmlContentLines[335].replace('-', dtwavgstr.setNum(DTWavg));


htmlContentLines[360].replace('-', Predictedlist[1]);
htmlContentLines[371].replace('-', Predictedlist[2]);
htmlContentLines[382].replace('-', Predictedlist[3]);
htmlContentLines[393].replace('-', Predictedlist[4]);
htmlContentLines[404].replace('-', Predictedlist[5]);
htmlContentLines[415].replace('-', Predictedlist[6]);
htmlContentLines[426].replace('-', Predictedlist[0]);
htmlContentLines[437].replace('-', Predictedlist[7]);



htmlContent=htmlContentLines.join('\n');
  QTextDocument *document = new QTextDocument();
  document->setHtml(htmlContent);

  QPrinter printer(QPrinter::HighResolution);
  printer.setPageSize(QPrinter::A4);
  printer.setOutputFormat(QPrinter::PdfFormat);

  printer.setOutputFileName(otputfile);

  document->print(&printer);
  htmlFile.close();
  delete document;
  return 0;
}

void MainWindow::on_pushButton_8_clicked()
{
    QString current_path=QDir::currentPath();

    QString fileName= QFileDialog::getSaveFileName(this, tr("Generate Report"), "reporte.pdf", tr("Documents (*.pdf)"));
    int a=printpdf(current_path+"/../reporteNeuroSpeech.htm", fileName);
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
     ls = QFileDialog::getOpenFileNames(this, "Choose file to analyze/", "", "Audio-Files(*.wav)");
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
    ui->customPlot->xAxis->setLabel("Time (sec)");
    ui->customPlot->yAxis->setLabel("Amplitude");
    ui->customPlot->setBackground(plotGradient);
    pen.setColor(QColor(0,120,0));
    ui->customPlot->graph()->setPen(pen);

    ui->customPlot->replot();
    xmain=xt;
    ymain=y;
    namemain=va;





    QString comandpv="python "+path_base+"predictmFDA.py "+path_base+' '+va;
    cmd.start(comandpv);
    cmd.waitForFinished(-1);

    qDebug() << comandpv;


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
    QString file_play=path_base+'n'+rowstr.number(current_row)+".wav";
    player->setMedia(QUrl::fromLocalFile(file_play));
    player->setVolume(50);
    player->play();



}
