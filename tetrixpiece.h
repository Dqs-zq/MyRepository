#ifndef TETRIXPIECE_H
#define TETRIXPIECE_H

//枚举了8种俄罗斯方块的类型
enum TetrixShape
{
    NoShape,
    ZShape,
    SShape,
    LineShape,
    TShape,
    SquareShape,
    MirroredLShape,
    LShape
};

class TetrixPiece
{ 
public:
    TetrixPiece();
    void setShape(TetrixShape shape);//根据俄罗斯方块的类型来设置具体的形状
    void setRandomShape();//设置俄罗斯方块为随机形状
    int minX()const;//获取俄罗斯方块的4个小块中最小的x坐标
    int minY()const;//获取俄罗斯方块的4个小块中最小的y坐标
    int maxX()const;//获取俄罗斯方块的4个小块中最大的x坐标
    int maxY()const;//获取俄罗斯方块的4个小块中最大的y坐标
    TetrixPiece rotateLeft()const;//左旋转，修改4个小方块的x,y坐标
    TetrixPiece rotateRight()const;//右旋转
    int getX(int index)const{return coords[index][0];}//根据index确定获取哪一个方块的X坐标
    int getY(int index)const{return coords[index][1];}//根据index确定获取哪一个方块的Y坐标
    TetrixShape shape()const{return pieceShape;}//返回俄罗斯方块的形状
private:
    TetrixShape pieceShape;//定义了方块的形状，为TetrixShape中的一种类型
    int coords[4][2];//每种俄罗斯方块都有4个小块,1个4行2列的数据,每个小块都有1个x坐标和1个y坐标
    void setX(int index,int value){coords[index][0]=value;}//根据index确定设置哪一个方块的X坐标
    void setY(int index,int value){coords[index][1]=value;}//根据index确定设置哪一个方块的Y坐标

};

#endif // TETRIXPIECE_H
