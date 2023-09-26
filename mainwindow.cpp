#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::vector<std::string> outputs{"log.log"};
    l = new logger(LOG_INFO,outputs);
    len = 0;
    worker = new ThreadWork(l);
    ui->log_text->setAcceptRichText(true);
    connect(ui->FileSelect, &QPushButton::clicked, this, &MainWindow::onFileSelectClicked);
    connect(ui->Upload,&QPushButton::clicked,this,&MainWindow::onUploadClicked);
    connect(ui->Download,&QPushButton::clicked,this,&MainWindow::onDownloadClicked);
    connect(ui->history,&QPushButton::clicked,this,&MainWindow::onHistoryClicked);
    connect(ui->clear,&QPushButton::clicked,this,&MainWindow::onClearClicked);
    connect(worker,&ThreadWork::progressChanged, this, &MainWindow::onProgressChanged);
    connect(l,&logger::logInfo,this,&MainWindow::onLogInfo);
    connect(l,&logger::logError,this,&MainWindow::onLogError);
    connect(l,&logger::logWarn,this,&MainWindow::onLogWarn);
}

MainWindow::~MainWindow()
{
    delete l;
    delete ui;
    delete worker;
}


void MainWindow::onFileSelectClicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());
    if (filename.isEmpty()) {
        return;
    }
    if (!QFile::exists(filename)){
        QMessageBox::critical(this, "Error", "File does not exist!");
        return;
    }
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Error", "Could not open file!");
        l->warn("无法打开文件 %s\n",filename.toStdString().c_str());
        return;
    }
    this->len = file.size();
    l->info("选中文件 %s\n",filename.toStdString().c_str());
    ui->File_Path->setText(filename);
    this->filename = filename.toStdString();
    file.close();
}

void MainWindow::onUploadClicked() {
    if (worker->isRunning()) {
        QMessageBox::critical(this, "Error", "Please wait for the current task to finish!");
        l->warn("请等待当前任务完成\n");
        return;
    }
    if (ui->IP_Edit->text().isEmpty()) {
        l->warn("未填写IP地址，默认使用127.0.0.1\n");
    }
    if (ui->Port_Edit->text().isEmpty()) {
        l->warn("未填写端口号，默认使用69\n");
    }
    ui->progressBar->setValue(0);
    ui->progressBar->setRange(0,len);
    mode = ui->Mode_Select->currentText().toStdString();
    worker->setParams(this->filename,this->mode,0);
    l->info("开始上传文件 %s\n",filename.c_str());
    l->info("使用%s模式\n",mode.c_str());
    QTime cur = QTime::currentTime();
    l->info("线程启动\n");
    this->worker->start();
    while (this->worker->isRunning());
    ui->speed_label->setText(QString("Speed : %1KB/s").arg(static_cast<double>(len/cur.msecsTo(QTime::currentTime()))));
}

void MainWindow::onDownloadClicked() {
    if (worker->isRunning()) {
        QMessageBox::critical(this, "Error", "Please wait for the current task to finish!");
        l->warn("请等待当前任务完成\n");
        return;
    }
    if (ui->IP_Edit->text().isEmpty()) {
        l->warn("未填写IP地址，默认使用127.0.0.1\n");
    }
    if (ui->Port_Edit->text().isEmpty()) {
        l->warn("未填写端口号，默认使用69\n");
    }
    ui->progressBar->setValue(0);
    ui->progressBar->setRange(0,len);
    mode = ui->Mode_Select->currentText().toStdString();
    worker->setParams(this->filename,this->mode,1);
    l->info("开始下载文件 %s\n",filename.c_str());
    l->info("使用%s模式\n",mode.c_str());
    QTime cur = QTime::currentTime();
    l->info("线程启动\n");
    this->worker->start();
    while (this->worker->isRunning());
    ui->speed_label->setText(QString("Speed : %1KB/s").arg(static_cast<double>(len/cur.msecsTo(QTime::currentTime()))));
}

void MainWindow::onProgressChanged() {
    ui->progressBar->setValue(this->worker->getProgress());
}

void MainWindow::onLogInfo(const QString& msg){
    auto m = msg.split("：");
    QString newStr = QString("<font color='green'>%1</font>： %2").
            arg(m[0]).
            arg(m[1]);
    ui->log_text->append(newStr);
}

void MainWindow::onLogWarn(const QString& msg) {
    auto m = msg.split("：");
    QString newStr = QString("<font color='yellow'>%1</font>： %2").
            arg(m[0]).
            arg(m[1]);
    ui->log_text->append(newStr);
}

void MainWindow::onLogError(const QString& msg) {
    auto m = msg.split("：");
    QString newStr = QString("<font color='red'>%1</font>： %2").
            arg(m[0]).
            arg(m[1]);
    ui->log_text->append(newStr);
}

void MainWindow::onHistoryClicked() {
    QString path = "/home/russ/CWorkspace/Socket/log.log";
    auto process = new QProcess(this);
    process->start("gedit", QStringList() << path);
    process->waitForFinished(-1);
    delete process;
}

void MainWindow::onClearClicked() {
    ui->log_text->clear();
}