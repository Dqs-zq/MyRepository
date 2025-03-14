#include "tetrixpiece.h"
#include <QRandomGenerator>
TetrixPiece::TetrixPiece()
{
    setShape(NoShape);//初始化俄罗斯方块的形状为NoShape
}

void TetrixPiece::setShape(TetrixShape shape)
{
    //定义一个静态数组，8种俄罗斯方块的每个小块的坐标值，每个坐标是1个俄罗斯方块4个小块各个小块左上角的坐标，坐标系为Y轴向下的坐标系
    static constexpr int coordsTable[8][4][2] = {
        { { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 } },   //NoShape
        { { 0, -1 },  { 0, 0 },   { -1, 0 },  { -1, 1 } },  //ZShape
        { { 0, -1 },  { 0, 0 },   { 1, 0 },   { 1, 1 } },   //SShape
        { { 0, -1 },  { 0, 0 },   { 0, 1 },   { 0, 2 } },   //LineShape
        { { -1, 0 },  { 0, 0 },   { 1, 0 },   { 0, 1 } },   //TShape
        { { 0, 0 },   { 1, 0 },   { 0, 1 },   { 1, 1 } },   //SquareShape
        { { -1, -1 }, { 0, -1 },  { 0, 0 },   { 0, 1 } },   //LShape
        { { 1, -1 },  { 0, -1 },  { 0, 0 },   { 0, 1 } }    //JShape
    };
    //初始化coord[4][2]数组
    for(int i=0;i<4;i++)
        for(int j=0;j<2;j++)
        {
            coords[i][j]=coordsTable[shape][i][j];//从coordsTable中取值
        }

    //设置当前俄罗斯方块的形状为shape,设置pieceShape参数值
    pieceShape=shape;
}

void TetrixPiece::setRandomShape()
{
    setShape(TetrixShape(QRandomGenerator::global()->bounded(7)+1));//生成1-7的随机数，设置俄罗斯方块的形状
}

int TetrixPiece::minX() const
{
    int min = coords[0][0];//设置最小值为coords[0][0]
    for(int i=1;i<4;++i)
    {
        min=qMin(min,coords[i][0]);//通过比较得到最小的x坐标
    }
    return min;
}

int TetrixPiece::minY() const
{
    int min = coords[0][1];//设置最小值为coords[0][1]
    for(int i=1;i<4;++i)
    {
        min=qMin(min,coords[i][1]);//通过比较得到最小的y坐标
    }
    return min;
}

int TetrixPiece::maxX() const
{
    int max = coords[0][0];//设置最大值为coords[0][0]
    for(int i=1;i<4;++i)
    {
        max=qMax(max,coords[i][0]);//通过比较得到最大的x坐标
    }
    return max;
}

int TetrixPiece::maxY() const
{
    int max = coords[0][1];//设置最大值为coords[0][1]
    for(int i=1;i<4;++i)
    {
        max=qMax(max,coords[i][1]);//通过比较得到最大的y坐标
    }
    return max;
}

TetrixPiece TetrixPiece::rotateLeft() const
{
    //旋转矩阵
    //x'=cosθ * x-sinθ *y
    //y'=sinθ * x+cosθ *y
    //[x']=[cosθ   -sinθ] [x]
    //[y']=[sinθ    cosθ] [y]
    //笛卡尔坐标系下，左旋转90°后,x'=-y,y'=x
    //y轴向下的坐标系，左旋转90°后,x'=y,y'=-x;

    //判断要旋转的俄罗斯方块是否为SquareShape，即方形，不需要旋转
    if(pieceShape == SquareShape)
        return *this;

    TetrixPiece result;//定义一个新的TetrixPiece对象
    result.setShape(pieceShape);//设置新的TetrixPiece对象的形状不变
    //左旋转90°，改变4个小块的坐标
    //笛卡尔坐标系下，左旋转90°后,x'=-y,y'=x
    //y轴向下的坐标系，左旋转90°后,x'=y,y'=-x;
    for(int i=0;i<4;++i)
    {
        result.setX(i,getY(i));//x'=y
        result.setY(i,-getX(i));//y'=-x
    }

    return result;//返回旋转之后的方块
}

TetrixPiece TetrixPiece::rotateRight() const
{
    if(pieceShape == SquareShape)
        return *this;

    TetrixPiece result;//定义一个新的TetrixPiece对象
    result.setShape(pieceShape);

    //右旋转90°，改变4个小块的坐标
    //笛卡尔坐标系下，右旋转90°后,x'=y,y'=-x
    //y轴向下的坐标系，右旋转90°后,x'=-y,y'=x;
    for(int i=0;i<4;++i)
    {
        result.setX(i,-getY(i));//x'=-y
        result.setY(i,getX(i));//y'=x
    }

    return result;//返回旋转之后的方块
}
