#include "tetrixboard.h"
#include <QLabel>
#include <QPainter>
#include <QKeyEvent>
TetrixBoard::TetrixBoard(QWidget *parent):QFrame (parent)
{
    nextPiece.setRandomShape();//对该俄罗斯方块进行随机初始化
    clearBoard();//重置TetrixBoard全屏的形状
    setFocusPolicy(Qt::StrongFocus);//设置接收焦点策略，方便接收键盘操作
}

void TetrixBoard::setNextPieceLabel(QLabel *label)
{
    nextPieceLabel=label;//设置“下一个”标签内容
}

void TetrixBoard::showNextPiece()
{
    if(!nextPieceLabel)//若label指针为空，则返回
        return;
    //确定在窗口中显示的方块的范围，即x、y轴上小块的个数，单位：块数
    int dx=nextPiece.maxX()-nextPiece.minX()+1;//最大的x坐标减去最新的x坐标，再加1，得到x轴上小块的个数
    int dy=nextPiece.maxY()-nextPiece.minY()+1;//最大的y坐标减去最新的y坐标，再加1，得到y轴上小块的个数
    //创建一个Pixmap 然后作画
    //单位：  像素
    QPixmap pixmap(dx*squareWidth(),dy*squareHeight());//根据俄罗斯方块的行、宽，设置pixmap
    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(),QColor(222, 156, 83));//给整个区域填充颜色
    //绘制四个小块
    for(int i=0;i<4;++i)
    {
        //在屏幕上绘制时，默认采用的是y轴向下的坐标，与俄罗斯方块默认的坐标系一致，由于已经得到画图区域(dx*squareWidth(),dy*squareHeight())，画图区域左上角为（0,0）点，
        //通过小块的x坐标-最小的x坐标得到，其x轴方向上所占的块数，再乘以小块的块长，即得到在绘图区域的x坐标，y坐标同理
        //x，y单位是小块数
        int x=nextPiece.getX(i)-nextPiece.minX();//小块的x坐标-最小的x坐标得到，其x轴方向上所占的块数
        int y=nextPiece.getY(i)-nextPiece.minY();//小块的x坐标-最小的x坐标得到，其y轴方向上所占的块数
        drawSquare(painter,x*squareWidth(),y*squareHeight(),nextPiece.shape());//通过x*squareWidth()，即块数乘以块长，拿到该小块在pixmap中的左上角的当前坐标
        //1个小方块所占的长，宽，以及从nextPiece.shape()中可拿到的颜色
    }
    nextPieceLabel->setPixmap(pixmap);//在标签上绘制下一个方块
}

void TetrixBoard::start()
{
    if(isStarted||isPaused)//如果已经开始，或者已经暂停，则不需处理，直接返回
        return;
    isStarted=true;//设置开始标志
    isGameOver=false;//设置结束标志
    numLinesRemoved=0;//重置消除行数
    numPiecesDropped=0;//重置消除行数
    score=0;//重置分数
    level=1;//重置等级
    clearBoard();//重置TetrixBoard全屏的形状

    emit linesRemoved(numLinesRemoved);//发送消除行数变化的信号
    emit scoreChanged(score);//发送分数变化的信号
    emit levelChanged(level);//发送等级变化的信号
    newPiece();//初始化一个方块
    timer.start(timeoutTime(),this);//计时器开始，方块开始下落
}

void TetrixBoard::pause()
{
    if(!isStarted)//游戏未开始，直接返回
        return;
    isPaused=!isPaused;//设置暂停标志，暂停->继续，继续->暂停
    if(isPaused)
    {
        timer.stop(); //计时器停止，游戏暂停 ，方块停止下落
    }
    else {
        timer.start(timeoutTime(),this);//计时器开始，游戏继续
    }
    update();//画面重新绘制
}

void TetrixBoard::drawSquare(QPainter &painter, int x, int y, TetrixShape shape)
{
    static constexpr QRgb colorTable[8]{
        0x000000,0xCC6666,0x66CC66,0x6666CC,
        0xCCCC66,0xCC66CC,0x66CCCC,0xDAAA00
    };//定义8种颜色
    QColor color = colorTable[shape];//根据shape值选择一种颜色
    //画方格的内容，空出一个矩形的位置，用于后面描边
    painter.fillRect(x+1,y+1,squareWidth()-2,squareHeight()-2,color);//方块左上角的x.y坐标及长宽，往里画

    //描边
    painter.setPen(color.lighter());//前两条边的颜色比方格的颜色更浅
    painter.drawLine(x,y,x+squareWidth(),y);//在（x,y)、（x+squareWidth(),y)两个坐标画一条线
    //实测在(x+squareWidth(),y)、(x+squareWidth(),y+squareHeight())两点画线没有效果，可能超出绘图范围，于是将x坐标减一
    painter.drawLine(x+squareWidth()-1,y,x+squareWidth()-1,y+squareHeight());
    painter.setPen(color.lighter().darker());//边的颜色比方格的颜色更深
    //同上，将y坐标减一
    painter.drawLine(x+squareWidth(),y+squareHeight()-1,x,y+squareHeight()-1);
    painter.drawLine(x,y+squareHeight(),x,y);
}

void TetrixBoard::newPiece()
{
    curPiece=nextPiece;//设置当前方块为nextPiece，用于显示、下落等操作
    nextPiece.setRandomShape();//nextPiece通过函数随机设置一种形状
    showNextPiece();//显示下一个方块
    //将方块置于实际游戏区域的顶部、中间的位置
    //在实际游戏区域，采用Y轴向上的坐标系，curX为俄罗斯方块中(0,0)坐标的那个小块的x坐标，curY是该小块的y坐标,该小块为“中心小块”
    curX=BoardWidth/2;//设置当前方块的x坐标为方块可移动区域的一半
    curY=BoardHeight-1+curPiece.minY();//"中心小块”的y坐标需要减去最小的y坐标，由于curY的坐标系与小块的坐标系y轴方向相反,所以curY=BoardHeight-(-curPiece.minY()),而且当curY位于顶部时，游戏结束，所以需要减一

    //尝试把newPiece(curPiece)摆放到起始位置（中间，上面）
    if(!tryMove(curPiece,curX,curY))
    {
        //如果新的方块不能放置，则游戏结束
        timer.stop();
        isStarted=false;
        isGameOver=true;
        update();//重绘
    }
}

bool TetrixBoard::tryMove(const TetrixPiece &newPiece, int newX, int newY)
{
    //判断是否能够移动，newX和newY为希望移动的位置的左上角坐标
    for(int i=0;i<4;++i)
    {
        int x=newX+newPiece.getX(i);//在游戏区域里每个小块的x坐标，为“中心小块”的x坐标加上每个小块的相对坐标
        int y=newY-newPiece.getY(i);
        if(x<0||x>=BoardWidth||y<0||y>=BoardHeight)//当小块的x坐标小于0或者x坐标大于等于游戏区域的宽度或者y坐标小于0或者大于游戏区域的高度
            return false;
        //判断是否有其他方块阻挡
        if(shapeAt(x,y)!=NoShape)
            return false;
    }
    //若可以移动，则更新当前方块
    curPiece = newPiece;//旋转时，需要
    curX = newX;//如果能移动，则移动，改变curX
    curY = newY;
    update();//更新
    return true;
}
//将游戏区域的所有格子均设置为没有形状
void TetrixBoard::clearBoard()
{
    for(int i=0;i<BoardWidth*BoardHeight;++i)
    {
        boardBBlocks[i]=NoShape;
    }
}
//下落1行
void TetrixBoard:: onelineDown()
{
    if(!tryMove(curPiece,curX,curY-1))//如果不能下落，则停止掉落
        pieceDropped(0);//传参为0,表示不用加分
}
//方块下落
void TetrixBoard::pieceDropped(int dropHeight)//传参用于算分
{
    for(int i=0;i<4;++i)
    {
        int x=curX+curPiece.getX(i);
        int y=curY-curPiece.getY(i);
        shapeAt(x,y)=curPiece.shape();//更新boardBlocks数组
    }

    ++numPiecesDropped;//将下落的方块数++
    if(numPiecesDropped%NextLevelNumber==0)//如果下落数为25的倍数时
    {
        ++level;//等级+1
        timer.start(timeoutTime(),this);//重新设置游戏等级，加快游戏速度
        emit levelChanged(level);//发送等级变化信号
    }
    score+=dropHeight+DropScore;//打分规则，分数加上下落高度及1个方块下落的分数
    emit scoreChanged(score);//发送分数变化的信号
    removeFullLines();//消除行
    newPiece();//重新生成方块
}

void TetrixBoard::removeFullLines()
{

    int numFullLines=0;//消除行数置零
    //消除行
    for(int i=BoardHeight-1;i>=0;--i)//从上往下扫描
    {
        bool lineIsFull =true;//是否有满行的标志
        //扫描一行
        for(int j=0;j<BoardWidth;++j)
        {
            if(shapeAt(j,i)==NoShape)//如果有1个方格没有形状，则退出
            {
                lineIsFull=false;
                break;
            }
        }
        if(lineIsFull)//如果有满行
        {
            //消除一行
            ++numFullLines;
            for(int k=i;k<BoardHeight-1;++k)//从该行开始到最高行的下一行
            {
                for(int j=0;j<BoardWidth;++j)
                {
                    shapeAt(j,k)=shapeAt(j,k+1);//将上面一行的形状赋值给下面的行
                }
            }
            for(int j=0;j<BoardWidth;++j)//游戏区域最高一行需要置为空形状
            {
                shapeAt(j,BoardWidth-1)=NoShape;
            }
        }
    }

    if(numFullLines>0)//如果消除了行
    {
        numLinesRemoved+=numFullLines;//统计消除的行数
        score +=10*numFullLines;//计分
        emit linesRemoved(numLinesRemoved);
        emit scoreChanged(score);
    }
}
//用于处理按下空格键时，直接下落至底部，通过循环尝试将方块每次下降1格，直至不能碰撞，不能下落
void TetrixBoard::dropDown()
{
    int newY=curY;//拿到当前的curY
    int dropHeight =0;//下落高度设置为0
    while(newY>0)//如果newY>0
    {
        if(!tryMove(curPiece,curX,curY-1))//如果不能下落
            break;
        --newY;//newY需要减一
        ++dropHeight;

    }
    pieceDropped(dropHeight);//方块下落至底部
}


void TetrixBoard::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);//调用父类的绘图函数
    QPainter painter(this);
    QRect rect=contentsRect();//拿到当前的游戏区域，用于绘制

    if(isPaused)//暂停时
    {
        painter.drawText(rect,Qt::AlignCenter,"暂 停");
        return;
    }
    if(isGameOver)//游戏结束
    {
        painter.drawText(rect,Qt::AlignCenter,"Game Over");
        return;
    }
   //通过rect.bottom()拿到当前区域矩形的底部Y坐标，BoardHeight*squareHeight()为当前区域的高度，可以得到当前区域矩形的顶部X坐标
   int boardTop=rect.bottom()-BoardHeight*squareHeight();//
    //绘制现有的方块，从下往上绘制
    for(int i=0;i<BoardHeight;++i)//从第0行到第BoardHeight-1行，共BoardHeight行
        for(int j=0;j<BoardWidth;++j)//从第1列到第BoardWidth-1列，共BoardWidth个方格
        {
          TetrixShape shape=shapeAt(j,i);//拿到当前位置的形状
            if(shape!=NoShape)//如果当前位置有形状
               //绘图时的Y坐标向下，当前小块的x坐标为当前区域的x坐标（rect.left())加上当前的列数乘以1个小格的宽度
               //当前小块的y坐标为当前区域底部的y坐标（rect.bottom()）减去i*squareHeight()
               //由于当当前小块的y坐标为当前区域底部的y坐标（rect.bottom())时，小块的y坐标为rect.bottom()，已经超出绘制区域，所以由于当当前小块的y坐标需要减去1个小格的高度即1*squareHeight()
               drawSquare(painter,rect.left()+j*squareWidth(),
                           rect.bottom()-(i+1)*squareHeight(),shape);
       }

    //绘制正在下落的方块
    if(curPiece.shape()!=NoShape)
    {
        for(int i=0;i<4;++i)
        {
            //当前俄罗斯方块的“中心小块”的在该绘制区域的坐标为（curX，curY）,curPiece.getX(i)和curPiece.getY(i)为他们的相对坐标（中心小块的坐标为（0,0））
            //x，y单位是小块数
            int x=curX+curPiece.getX(i);//得到当前小块在x轴方向上所占的块数
            int y=curY-curPiece.getY(i);//得到当前小块在y轴方向上所占的块数，当前块内部的坐标系为Y轴向下与，curY的坐标系相反curY+（-curPiece.getY(i))

            //实际绘制时，Y轴方向向下
            //当前小块的x坐标为当前区域的左侧x坐标加上，当前小块在x轴方向上的距离
            //当前小块的y坐标为当前区域的顶部y坐标加上，当前区域的高度减去当前小块距离当前区域底部的距离
            //由于当当前小块的y坐标为boardTop+(BoardHeight-y)*squareHeight()时，小块已经超出了绘图区域，所以需要减去一个方块的高度
            drawSquare(painter,rect.left()+x*squareWidth(),boardTop+(BoardHeight-y-1)*squareHeight(),curPiece.shape());
        }
    }
}

void TetrixBoard::timerEvent(QTimerEvent *event)
{
    if(isStarted)
    {
        //下移一行
        onelineDown();
    }
}

void TetrixBoard::keyPressEvent(QKeyEvent *event)
{
    if(!isStarted||isPaused||curPiece.shape()==NoShape)
    {
        QFrame::keyPressEvent(event);
        return;
    }

    switch(event->key())
    {
    case Qt::Key_Left:{
        tryMove(curPiece,curX-1,curY);//左移一格
        break;
    }
    case Qt::Key_Right:
    {
        tryMove(curPiece,curX+1,curY);//右移一格
        break;
    }
    case Qt::Key_Down:
    {
        tryMove(curPiece.rotateRight(),curX,curY);//顺时针旋转90°
        break;
    }
    case Qt::Key_Up:
    {
        tryMove(curPiece.rotateLeft(),curX,curY);//逆时针旋转90°
        break;
    }
    case Qt::Key_D:
    {
        onelineDown();//下落1行
        break;
    }
    case Qt::Key_Space:
    {
        dropDown();//下落至底部
        break;
    }

    default:
        QFrame::keyPressEvent(event);//处理其他键盘事件
    }
}
