#include "mainwindow.h"
#include<QMouseEvent>
#include "tetrixboard.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//无边框，无法移动
    setAttribute(Qt::WA_TranslucentBackground,true);//透明背景
    ui->frameTetrixBoard->setNextPieceLabel(ui->labelNextPiece);//向TetrixBoard发送显示下一个俄罗斯方块的标签的地址
    connect(ui->pbStart,&QPushButton::clicked,ui->frameTetrixBoard,&TetrixBoard::start);
    connect(ui->pbPause,&QPushButton::clicked,ui->frameTetrixBoard,&TetrixBoard::pause);
    connect(ui->frameTetrixBoard,&TetrixBoard::scoreChanged,ui->lcdScore,QOverload<int>::of(&QLCDNumber::display));
    connect(ui->frameTetrixBoard,&TetrixBoard::levelChanged,ui->lcdLevel,QOverload<int>::of(&QLCDNumber::display));
    connect(ui->frameTetrixBoard,&TetrixBoard::linesRemoved,ui->lcdRemoved,QOverload<int>::of(&QLCDNumber::display));

    ui->pbStart->setFocusPolicy(Qt::NoFocus);
    ui->pbPause->setFocusPolicy(Qt::NoFocus);
    ui->pbExit->setFocusPolicy(Qt::NoFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePos=event->globalPos();//鼠标当前位置在整个屏幕中的位置
    QPoint topLeft = this->geometry().topLeft();//当前窗口左上角在整个屏幕中的位置
    winPos=mousePos-topLeft;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    winPos=QPoint();//鼠标松开后，重置winPos
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos=event->globalPos();//鼠标当前位置在整个屏幕中的位置
    QPoint endPos=mousePos-winPos;//求出当前窗口左上角在整个屏幕中的位置
    move(endPos);//整个窗口移动到鼠标当前拖动的位置
}

void MainWindow::on_pbPause_clicked()
{
    if(ui->pbPause->text()==QString("暂停"))
    {
        ui->pbPause->setText("继续");
    }
    else {
        ui->pbPause->setText("暂停");
    }
}
