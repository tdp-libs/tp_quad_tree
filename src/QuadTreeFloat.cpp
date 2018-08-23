#include "tp_quad_tree/QuadTreeFloat.h"

#include "tp_utils/Globals.h"

#include <vector>

namespace tp_quad_tree
{

//##################################################################################################
struct QuadTreeFloat::Cell
{
  TP_NONCOPYABLE(Cell);
  std::vector<QuadTreeFloat::Coord> coords;

  //0=x0 y0
  //1=x1 y0
  //2=x0 y1
  //3=x1 y1
  Cell* children{nullptr};

  float radX{0.0f};
  float radY{0.0f};
  float cx{0.0f};
  float cy{0.0f};
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
  int findChild(float x, float y)
  {
    //0=x0 y0
    //1=x1 y0
    //2=x0 y1
    //3=x1 y1
    return (x<cx)?((y<cy)?0:2):((y<cy)?1:3);
  }

  //################################################################################################
  void addCoord(const QuadTreeFloat::Coord& coord)
  {
    if(!children)
    {
      coords.push_back(coord);
      if(int(coords.size())>cellSize)
      {
        float nRadX = radX/2;
        float nRadY = radY/2;

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
    else
      children[findChild(coord.x, coord.y)].addCoord(coord);
  }

  //################################################################################################
  void closestPoint(float x, float y, float& distSQ, const QuadTreeFloat::Coord*& closestPoint)
  {
    if(children)
    {
      if(x<cx)
      {
        float dx = cx-x;
        dx*=dx;

        if(y<cy)
        {
          float dy = cy-y;
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
          float dy = y-cy;
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
        float dx = x-cx;
        dx*=dx;

        if(y<cy)
        {
          float dy = cy-y;
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
          float dy = y-cy;
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
        float dx = c->x-x;
        float dy = c->y-y;
        float nDist = (dx*dx) + (dy*dy);
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
QuadTreeFloat::QuadTreeFloat(float minX, float maxX, float minY, float maxY, int cellSize):
  m_root(new Cell(cellSize))
{
  m_root->radX = (maxX-minX)/2;
  m_root->radY = (maxY-minY)/2;
  m_root->cx = minX+m_root->radX;
  m_root->cy = minY+m_root->radY;
}

//##################################################################################################
QuadTreeFloat::~QuadTreeFloat()
{
  delete m_root;
}

//##################################################################################################
void QuadTreeFloat::addCoord(const QuadTreeFloat::Coord& coord)
{
  m_root->addCoord(coord);
}

//##################################################################################################
QuadTreeFloat::Coord QuadTreeFloat::closestPoint(const QuadTreeFloat::Coord& point, float& distSQ)
{
  const Coord* closestPoint=nullptr;
  m_root->closestPoint(point.x, point.y, distSQ, closestPoint);
  return (closestPoint)?*closestPoint:Coord();
}

}
