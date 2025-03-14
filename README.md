## 🎮 项目简介

一款使用 **Qt 5.12** 框架开发的跨平台俄罗斯方块游戏，支持 Windows/Linux/macOS 系统。项目采用现代 C++ 开发，包含以下核心特性：

## ✨ 功能特性

### 核心玩法
- 7 种经典 Tetromino 方块随机生成
- 行消除得分机制
- 直接下落得分机制（每次摁D键下落，根据下落高度加分）
- 动态难度系统（每下落25个方块加速下落）
- 暂停/继续/重新开始功能

### 界面系统
- 主游戏区 + 信息面板双视图布局
- 实时显示：
  - 当前得分
  - 消除行数
  - 当前等级
  - 下一个方块预览
## 🧩 项目架构

### 类设计
- 本项目设计了3个类：MainWindow、TetrixBoard、TetrixPiece，在.cpp及.h文件中对各个类及其成员变量和成员函数均做了注释
  - MainWindow:显示游戏的分数、游戏的等级、已消除的行数以及下一个方块的信息。通过信号槽实现与游戏界面TetrixBoard的联系。
  - TetrixBoard:类继承自QFrame,处理游戏逻辑、控制键盘的输入，在游戏区域显示方块。
  - TetrixPiece:模拟一个俄罗斯方块的信息。
### 设计核心
- 1、通过定义一个静态数组，设计了8种方块的每个小块的坐标值，每个坐标是方块4个小块各个小块左上角的坐标，所采用的坐标系为y轴向下的坐标系
    
```
static constexpr int coordsTable[8][4][2] = {
        { { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 } },   //没有形状
        { { 0, -1 },  { 0, 0 },   { -1, 0 },  { -1, 1} },   //Z形
        { { 0, -1 },  { 0, 0 },   { 1, 0 },   { 1, 1 } },   //S形
        { { 0, -1 },  { 0, 0 },   { 0, 1 },   { 0, 2 } },   //I形
        { { -1, 0 },  { 0, 0 },   { 1, 0 },   { 0, 1 } },   //T形
        { { 0, 0 },   { 1, 0 },   { 0, 1 },   { 1, 1 } },   //方形
        { { -1, -1 }, { 0, -1 },  { 0, 0 },   { 0, 1 } },   //L形
        { { 1, -1 },  { 0, -1 },  { 0, 0 },   { 0, 1 } }    //J形
     };
```
- 2、方块的左旋转和右旋转：通过旋转矩阵得到，左旋转及右旋转之后的方块各个小块的坐标
 旋转矩阵如下：
    
```
x'=cosθ * x-sinθ *y
    y'=sinθ * x+cosθ *y
    [x']=[cosθ   -sinθ] [x]
    [y']=[sinθ    cosθ] [y]
    笛卡尔坐标系下，左旋转90°后,x'=-y,y'=x
    y轴向下的坐标系，左旋转90°后,x'=y,y'=-x;
```
- 3、在MainWindow的标签中显示下一个方块时，需要得到标签的地址（ui->labelNextPiece），在TetrixBoard类中进行绘制，可通过定义1个函数（setNextPieceLabel）将标签的地址传递到TetrixBoard中
`ui->frameTetrixBoard->setNextPieceLabel(ui->labelNextPiece);//向TetrixBoard发送显示下一个方块的标签的地址`
- 4、在标签中绘制下一个方块时，计算出绘制区域的width和height，通过循环依次绘制各个小块，通过计算小块的在绘制区域的x,y坐标，输入画笔painter及当前方块的形状nextPiece.shape()即可调用drawSquare函数绘制单个小块

```
for(int i=0;i<4;++i)
    {
        //在屏幕上绘制时，默认采用的是y轴向下的坐标，与俄罗斯方块默认的坐标系一致，由于已经得到画图区域(dx*squareWidth(),dy*squareHeight())，画图区域左上角为（0,0）点，
        //通过小块的x坐标-最小的x坐标得到，其x轴方向上所占的块数，再乘以小块的块长，即得到在绘图区域的x坐标，y坐标同理
        //x，y单位是小块数
        int x=nextPiece.getX(i)-nextPiece.minX();//小块的x坐标-最小的x坐标得到，其x轴方向上所占的块数
        int y=nextPiece.getY(i)-nextPiece.minY();//小块的x坐标-最小的x坐标得到，其y轴方向上所占的块数
        //通过x*squareWidth()，即块数乘以块长，拿到该小块在pixmap中的左上角的当前坐标 
        drawSquare(painter,x*squareWidth(),y*squareHeight(),nextPiece.shape());
}
```
- 5、需要定义1个静态数据，对应8种形状各自的颜色

```
static constexpr QRgb colorTable[8]{
        0x000000,0xCC6666,0x66CC66,0x6666CC,
        0xCCCC66,0xCC66CC,0x66CCCC,0xDAAA00
    };//定义8种颜色
```
- 6、定义1个数组，记录整个游戏区域（TetrixBoard）内各个方格个形状

```
enum TetrixShape
{
    NoShape,
    ZShape,
    SShape,
    LineShape,
    TShape,
    SquareShape,
    LShape,
    JShape
};
    TetrixShape boardBBlocks[BoardWidth*BoardHeight];//每一个方格里是否有方块，BoardWidth为TetrixBoard的方格行数，BoardHeight为TetrixBoard的方格列数

```
- 7、消除行时，采用从上往下扫描的方式，逐行扫描定义1个满行标志，当发现该行有空行时，将该标志置false，停止该行的扫描，扫下一行。若发现有满行时，从该行开始向上至最上面一行，将上面1行的形状依次设置到下面1行。并将最上面1行形状均设置为空

```
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
```
- 8、碰撞检测函数：通过判断将方块上下左右移动1格时，若各个小块将要移动的新位置的形状均为空形状，则可以移动，否则不能移动

```
//尝试将方块移动至新的位置。若能移动则移动至新位置，函数返回true,否则返回false
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
```
- 9、图形绘制：绘制正在下落的方块及位于底部的方块
   
```
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

```
>>>>>>> f5c2281 (fisrt commit)
>>>>>>> b7ac3fc (first commit)
