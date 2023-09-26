#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QProcess>
#include <QDesktopServices>
#include <QTime>
#include <QThread>
#include <utility>
#include "Thread.h"
#include "log/log.h"



using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void onFileSelectClicked();
    void onUploadClicked();
    void onDownloadClicked();
    void onProgressChanged();
    void onHistoryClicked();
    void onClearClicked();
    void onLogInfo(const QString& msg);
    void onLogWarn(const QString& msg);
    void onLogError(const QString& msg);
private:
    Ui::MainWindow *ui;
    unsigned long long len;
    string filename;
    string mode;
    ThreadWork* worker;
    logger *l;
};
#endif // MAINWINDOW_H
