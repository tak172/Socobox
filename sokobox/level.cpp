#include "level.hpp"
#include "appsettings.hpp"

#include <QFile>
#include <QTextStream>


level::level()
{
}

void level::reset()
{
    load(mLevel);
}

void level::load(int aLevel)
{
    const auto &lp = appSettings::instance().levelsPath();

    QString lvl_path = lp + QString::number(aLevel) + ".lvl";

    QFile   file(lvl_path);

    mIsLoad       = false;
    mIsComplete   = false;
    mGoalCount    = 0;
    mBoxesOnGoal  = 0;
    mSteps        = 0;

    file.open(QIODevice::ReadOnly);

    if(file.isOpen())
    {
        mRoom.clear();

        QTextStream in(&file);
        in >> mLevel;
        in >> mColumns;
        in >> mLines;

        std::vector<eField> tmpRoom(mColumns * mLines, eField::EMPTY);

        in.readLine();

        for(int i = 0; i < mLines; ++i)
        {
            QString line;
            line = in.readLine();

            int  column = 0;
            int  end    = line.size();

            while(column != end)
            {
                switch (line[column].toLatin1())
                {
                case '#': // Wall
                {
                    tmpRoom[mColumns * i + column] =  eField::WALL;
                    break;
                }
                case '@': 
                {
                    tmpRoom[mColumns * i + column] =  eField::PLAYER;
                    mPlayerPos.first  = i;
                    mPlayerPos.second = column;
                    break;
                }
                case '+':
                {
                    tmpRoom[mColumns * i + column] =  eField::PLAYER_ON_GOAL;
                    break;
                }
                case '$': 
                {
                    tmpRoom[mColumns * i + column] =  eField::BOX;
                    break;
                }
                case '*': 
                {
                    tmpRoom[mColumns * i + column] =  eField::BOX_ON_GOAL;
                    ++mGoalCount;
                    ++mBoxesOnGoal;
                    break;
                }
                case '.': 
                {
                    tmpRoom[mColumns * i + column] =  eField::GOAL;
                    ++mGoalCount;
                    break;
                }
                case ' ': 
                {
                    tmpRoom[mColumns * i + column] = eField::EMPTY;
                    break;
                }
                }
                ++column;
            }
        }
        mRoom.swap(tmpRoom);
        mIsLoad = true;
    }

    file.close();
}

void level::load(std::string)
{
}

bool level::isComplete()
{
    return mIsComplete;
}

bool level::isLoad()
{
    return mIsLoad;
}

void level::player_up()
{
    move(-1,0);
}

void level::player_down()
{
    move(1,0);
}


void level::player_left()
{
    move(0,-1);
}

void level::player_rigth()
{
    move(0,1);
}

void level::move(int aDl, int aDc)
{
    auto &[line, column] = mPlayerPos;

    auto nl  = line   + aDl;                
    auto nc  = column + aDc;                

    auto nnl = nl + aDl;                   
    auto nnc = nc + aDc;                    

    auto coff  = mColumns * line + column;  
    auto noff  = mColumns * nl + nc;        
    auto nnoff = mColumns * nnl + nnc;    

    auto &f   = mRoom[coff];                
    auto &nf  = mRoom[noff];               
    auto &nnf = mRoom[nnoff];  

    bool isMoving = false;

    switch(nf)
    {
    case eField::BOX:
    {
        if(nnf == eField::GOAL)
        {
            f   = (f == eField::PLAYER) ? eField::EMPTY : eField::GOAL;
            nf  = eField::PLAYER;
            nnf = eField::BOX_ON_GOAL;
            isMoving = true;
            ++mBoxesOnGoal;
            break;
        }
        if(nnf == eField::EMPTY)
        {
            f   = (f == eField::PLAYER) ? eField::EMPTY : eField::GOAL;
            nf  = eField::PLAYER;
            nnf = eField::BOX;
            isMoving = true;
            break;
        }
        break;
    }
    case eField::BOX_ON_GOAL:
    {
        if(nnf == eField::GOAL)
        {
            f   = (f == eField::PLAYER) ? eField::EMPTY : eField::GOAL;
            nf  = eField::PLAYER_ON_GOAL;
            nnf = eField::BOX_ON_GOAL;
            isMoving = true;
            break;
        }
        if(nnf == eField::EMPTY)
        {
            f   = (f == eField::PLAYER) ? eField::EMPTY : eField::GOAL;
            nf  = eField::PLAYER_ON_GOAL;
            nnf = eField::BOX;
            isMoving = true;
            --mBoxesOnGoal;
            break;
        }
        break;
    }
    case eField::GOAL:
    {
        f   = (f == eField::PLAYER) ? eField::EMPTY : eField::GOAL;
        nf  = eField::PLAYER_ON_GOAL;
        isMoving = true;
        break;
    }

    case eField::EMPTY:
    {
        f  = (f == eField::PLAYER) ? eField::EMPTY : eField::GOAL;
        nf = eField::PLAYER;
        isMoving = true;
        break;
    }
    case eField::WALL:
    case eField::PLAYER:
    case eField::PLAYER_ON_GOAL:
        break;
    }

    if(isMoving)
    {
        line   = nl;
        column = nc;
        ++mSteps;

        if( mBoxesOnGoal == mGoalCount)
            mIsComplete = true;
    }
}

int level::steps_count() const
{
    return mSteps;
}

int level::goals() const
{
    return mGoalCount;
}

int level::boxes_on_goal() const
{
    return mBoxesOnGoal;
}
