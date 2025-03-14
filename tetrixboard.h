#ifndef TETRIXBOARD_H
#define TETRIXBOARD_H

#include <QFrame>
#include <QPointer>
#include "tetrixpiece.h"
#include<QBasicTimer>


class QLabel;
class TetrixBoard : public QFrame
{
    Q_OBJECT

public:
    TetrixBoard(QWidget* parent = nullptr);
    void setNextPieceLabel(QLabel *label);//设置“下一个”标签内容
    void newPiece();//新造一个方块
public slots:
    void start();//开始的槽函数
    void pause();//暂停的槽函数
private:
    enum{
        DropScore=7,//每消除1行得7分
        BoardWidth=12,//中间游戏区域共占有12个格子，需要设置TextBoard的minimumSize属性中的width为12的倍数，否则方格放置容易出现空隙，放不满
        BoardHeight=22,//中间游戏区域共占有22个格子
        NextLevelNumber=25,//消除25行时提速       
    };
    int squareWidth(){return contentsRect().width()/BoardWidth;}//1个方块格子，在行上所占的像素数，当前区域行的像素数除以行的格子数
    int squareHeight(){return contentsRect().height()/BoardHeight;}//1个方块格子，在列上所占的像素数
    void drawSquare(QPainter &painter,int x,int y,TetrixShape shape);//画小方块，需要提供画笔，小方块的x坐标，y坐标，小方块的形状
    bool tryMove(const TetrixPiece &newPiece,int newX,int newY);//尝试移动方块到x,y坐标
    TetrixShape & shapeAt(int x,int y){return boardBBlocks[y*BoardWidth+x];}//返回x,y坐标处的形状
    void clearBoard();//清空数组
    void onelineDown();//方格往下一行
    void pieceDropped(int dropHeight);//方块接触到了底部，不能移动，dropHeight为下落的高度，用于计分使用
    int timeoutTime(){return 600/(1+level);}//根据此时的游戏等级计算下落的时间，等级越高，下落得越快
    void removeFullLines();//消除满行
    void dropDown();//当按空格键时，直接下落至底部
    void showNextPiece();//展示该俄罗斯方块
private:
    QPointer<QLabel> nextPieceLabel;//定义nextPieceLabel指针
    TetrixPiece nextPiece;//定义nextPieceLabel标签中的一个俄罗斯方块的对象，根据对象的内容对label进行绘制
    TetrixPiece curPiece;//正在下落的俄罗斯方块
    int curX;//正在下落的俄罗斯方块的x坐标
    int curY;//正在下落的俄罗斯方块的y坐标
    TetrixShape boardBBlocks[BoardWidth*BoardHeight];//每一个方格里的形状
    bool isStarted=false;//开始的标志
    bool isPaused=false;//暂停的标志
    bool isGameOver =false;//游戏结束的标志
    int numLinesRemoved=0;//消除多少行
    int numPiecesDropped=0;//掉落多少方块
    int score=0;//分数
    int level=1;//等级，默认1级
    QBasicTimer timer;//计时器
signals:
    void levelChanged(int level);//等级变化的信号
    void scoreChanged(int score);//分数变化的信号
    void linesRemoved(int numberlines);//消除行数变化的信号
protected:
    void paintEvent(QPaintEvent *event) override;//重写绘图事件，用于UI界面的更新
    void timerEvent(QTimerEvent *event) override;//控制
    void keyPressEvent(QKeyEvent *event) override;//处理键盘操作
};

#endif // TETRIXBOARD_H
