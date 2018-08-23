#include "tp_quad_tree/QuadTreeInt.h"

#include "tp_utils/Globals.h"

#include <vector>

namespace tp_quad_tree
{

//##################################################################################################
struct QuadTreeInt::Cell
{
  TP_NONCOPYABLE(Cell);
  std::vector<QuadTreeInt::Coord> coords;

  //0=x0 y0
  //1=x1 y0
  //2=x0 y1
  //3=x1 y1
  Cell* children{nullptr};

  int radX{0};
  int radY{0};
  int cx{0};
  int cy{0};
  int cellSize;

  //################################################################################################
  Cell(int cellSize_=20):
    cellSize(cellSize_)
  {

  }

  //################################################################################################
  ~Cell()
  {
    delete[] children;
  }

  //################################################################################################
  int findChild(int x, int y)
  {
    //0=x0 y0
    //1=x1 y0
    //2=x0 y1
    //3=x1 y1
    return (x<cx)?((y<cy)?0:2):((y<cy)?1:3);
  }

  //################################################################################################
  void addCoord(const QuadTreeInt::Coord& coord)
  {
    if(!children)
    {
      coords.push_back(coord);
      if(int(coords.size())>cellSize)
      {
        int nRadX = radX/2;
        int nRadY = radY/2;

        if(nRadX>1 && nRadY>1)
        {
          children = new Cell[4];

          children[0].radX = nRadX;
          children[0].radY = nRadY;
          children[0].cx = cx - nRadX;
          children[0].cy = cy - nRadY;
          children[0].cellSize = cellSize;
          children[0].coords.reserve(cellSize);

          children[1].radX = nRadX;
          children[1].radY = nRadY;
          children[1].cx = cx + nRadX;
          children[1].cy = cy - nRadY;
          children[1].cellSize = cellSize;
          children[1].coords.reserve(cellSize);

          children[2].radX = nRadX;
          children[2].radY = nRadY;
          children[2].cx = cx - nRadX;
          children[2].cy = cy + nRadY;
          children[2].cellSize = cellSize;
          children[2].coords.reserve(cellSize);

          children[3].radX = nRadX;
          children[3].radY = nRadY;
          children[3].cx = cx + nRadX;
          children[3].cy = cy + nRadY;
          children[3].cellSize = cellSize;
          children[3].coords.reserve(cellSize);

          const Coord* c = coords.data();
          const Coord* cMax = c + coords.size();
          for(;c<cMax; c++)
            addCoord(*c);

          coords.clear();
        }
      }
    }
    else
      children[findChild(coord.x, coord.y)].addCoord(coord);
  }

  //################################################################################################
  void closestPoint(int x, int y, int& distSQ, const QuadTreeInt::Coord*& closestPoint)
  {
    if(children)
    {
      if(x<cx)
      {
        int dx = cx-x;
        dx*=dx;

        if(y<cy)
        {
          int dy = cy-y;
          dy*=dy;

          //0=x0 y0 <--
          //1=x1 y0
          //2=x0 y1
          //3=x1 y1

          children[0].closestPoint(x, y, distSQ, closestPoint);

          if(dx<distSQ)
            children[1].closestPoint(x, y, distSQ, closestPoint);

          if(dy<distSQ)
            children[2].closestPoint(x, y, distSQ, closestPoint);

          if((dx+dy)<distSQ)
            children[3].closestPoint(x, y, distSQ, closestPoint);
        }
        else
        {
          int dy = y-cy;
          dy*=dy;

          //0=x0 y0
          //1=x1 y0
          //2=x0 y1 <--
          //3=x1 y1

          children[2].closestPoint(x, y, distSQ, closestPoint);

          if(dx<distSQ)
            children[3].closestPoint(x, y, distSQ, closestPoint);

          if(dy<distSQ)
            children[0].closestPoint(x, y, distSQ, closestPoint);

          if((dx+dy)<distSQ)
            children[1].closestPoint(x, y, distSQ, closestPoint);
        }
      }
      else
      {
        int dx = x-cx;
        dx*=dx;

        if(y<cy)
        {
          int dy = cy-y;
          dy*=dy;

          //0=x0 y0
          //1=x1 y0 <--
          //2=x0 y1
          //3=x1 y1

          children[1].closestPoint(x, y, distSQ, closestPoint);

          if(dx<distSQ)
            children[0].closestPoint(x, y, distSQ, closestPoint);

          if(dy<distSQ)
            children[3].closestPoint(x, y, distSQ, closestPoint);

          if((dx+dy)<distSQ)
            children[2].closestPoint(x, y, distSQ, closestPoint);
        }
        else
        {
          int dy = y-cy;
          dy*=dy;

          //0=x0 y0
          //1=x1 y0
          //2=x0 y1
          //3=x1 y1 <--

          children[3].closestPoint(x, y, distSQ, closestPoint);

          if(dx<distSQ)
            children[2].closestPoint(x, y, distSQ, closestPoint);

          if(dy<distSQ)
            children[1].closestPoint(x, y, distSQ, closestPoint);

          if((dx+dy)<distSQ)
            children[0].closestPoint(x, y, distSQ, closestPoint);
        }
      }
    }
    else
    {
      const Coord* c = coords.data();
      const Coord* cMax = c + coords.size();
      for(;c<cMax; c++)
      {
        int dx = c->x-x;
        int dy = c->y-y;
        int nDist = (dx*dx) + (dy*dy);
        if(nDist<distSQ)
        {
          closestPoint = c;
          distSQ = nDist;
        }
      }
    }
  }
};

//##################################################################################################
QuadTreeInt::QuadTreeInt(int minX, int maxX, int minY, int maxY, int cellSize):
  m_root(new Cell(cellSize))
{
  m_root->radX = (maxX-minX)/2;
  m_root->radY = (maxY-minY)/2;
  m_root->cx = minX+m_root->radX;
  m_root->cy = minY+m_root->radY;
}

//##################################################################################################
QuadTreeInt::~QuadTreeInt()
{
  delete m_root;
}

//##################################################################################################
void QuadTreeInt::addCoord(const QuadTreeInt::Coord& coord)
{
  m_root->addCoord(coord);
}

//##################################################################################################
const QuadTreeInt::Coord* QuadTreeInt::closestPoint(const QuadTreeInt::Coord& point, int& distSQ)
{
  const Coord* closestPoint=nullptr;
  m_root->closestPoint(point.x, point.y, distSQ, closestPoint);
  return closestPoint;
}

}
