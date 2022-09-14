#ifndef tp_quad_tree_QuadTreeIntTemplate_h
#define tp_quad_tree_QuadTreeIntTemplate_h

#include "tp_quad_tree/Globals.h" // IWYU pragma: keep

#include "tp_utils/Globals.h"

#include <vector>

namespace tp_quad_tree
{

//##################################################################################################
template<typename T>
class QuadTreeIntTemplate
{
public:

  //################################################################################################
  struct Coord
  {
    int x;
    int y;

    T value;

    //##############################################################################################
    Coord(int x_=0, int y_=0, const T& value_=T()):
      x(x_),
      y(y_),
      value(value_)
    {

    }
  };

  //################################################################################################
  struct CoordDistance
  {
    const Coord* coord;
    int distSQ;

    CoordDistance(const Coord* coord_=nullptr, int distSQ_=0):
      coord(coord_),
      distSQ(distSQ_)
    {

    }
  };

  //################################################################################################
  //! Construct an empty quad tree
  /*!
  This will construct an empty fixed size quad tree, to use this sort for quad tree you need to know
  the bounds of your data in advance. This type of quad tree only stores coordinates, it does not
  store values associated with the coordinates.

  \param minX - The minimum x value
  \param maxX - The maximum x value
  \param minY - The minimum y value
  \param maxY - The maximum y value
  \param cellSize - The maximum number of coords in a cell
  */
  QuadTreeIntTemplate(int minX, int maxX, int minY, int maxY, int cellSize):
    m_root(new Cell(cellSize))
  {
    m_root->radX = (maxX-minX)/2;
    m_root->radY = (maxY-minY)/2;
    m_root->cx = minX+m_root->radX;
    m_root->cy = minY+m_root->radY;
  }

  //################################################################################################
  ~QuadTreeIntTemplate()
  {
    delete m_root;
  }

  //################################################################################################
  //! Add a coordinate to the tree
  /*!
  This will add a coordinate to the tree, this will divide cells as required.

  \param coord - The coordinate to add.
  */
  void addCoord(const Coord& coord)
  {
    m_root->addCoord(coord);
    m_count++;
  }

  //################################################################################################
  //! Find the closes coord to the point
  /*!
  This will return the coordinate closest to the point.

  \param point - The point to find the nearest point to.
  \param distSQ - This will be updated with the distance to the closest coord the initial value \
         will limit the search radius.
  \return The coord if one is found, else a null Coord.
  */
  Coord closestPoint(int x, int y, int& distSQ)
  {
    const Coord* closestPoint=nullptr;
    m_root->closestPoint(x, y, distSQ, closestPoint);
    return (closestPoint)?*closestPoint:Coord();
  }

  //################################################################################################
  void kClosestPoints(int x, int y, int k, int& distSQ, std::vector<CoordDistance>& results)
  {
    m_root->kClosestPoints(x, y, k, distSQ, results);
  }

  //################################################################################################
  int size() const
  {
    return m_count;
  }

private:
  QuadTreeIntTemplate(const QuadTreeIntTemplate&)=delete;
  QuadTreeIntTemplate& operator=(const QuadTreeIntTemplate&)=delete;

  //##################################################################################################
  struct Cell
  {
    TP_NONCOPYABLE(Cell);
    std::vector<Coord> coords;

    //0=x0 y0
    //1=x1 y0
    //2=x0 y1
    //3=x1 y1
    Cell* children;

    int radX;
    int radY;
    int cx;
    int cy;
    int cellSize;

    //################################################################################################
    Cell(int cellSize_=20):
      children(nullptr),
      radX(0),
      radY(0),
      cx(0),
      cy(0),
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
    void addCoord(const Coord& coord)
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
    void closestPoint(int x, int y, int& distSQ, const Coord*& closestPoint)
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

    //##############################################################################################
    void kClosestPoints(int x, int y, int k, int& distSQ, std::vector<CoordDistance>& results)
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

            children[0].kClosestPoints(x, y, k, distSQ, results);

            if(dx<distSQ)
              children[1].kClosestPoints(x, y, k, distSQ, results);

            if(dy<distSQ)
              children[2].kClosestPoints(x, y, k, distSQ, results);

            if((dx+dy)<distSQ)
              children[3].kClosestPoints(x, y, k, distSQ, results);
          }
          else
          {
            int dy = y-cy;
            dy*=dy;

            //0=x0 y0
            //1=x1 y0
            //2=x0 y1 <--
            //3=x1 y1

            children[2].kClosestPoints(x, y, k, distSQ, results);

            if(dx<distSQ)
              children[3].kClosestPoints(x, y, k, distSQ, results);

            if(dy<distSQ)
              children[0].kClosestPoints(x, y, k, distSQ, results);

            if((dx+dy)<distSQ)
              children[1].kClosestPoints(x, y, k, distSQ, results);
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

            children[1].kClosestPoints(x, y, k, distSQ, results);

            if(dx<distSQ)
              children[0].kClosestPoints(x, y, k, distSQ, results);

            if(dy<distSQ)
              children[3].kClosestPoints(x, y, k, distSQ, results);

            if((dx+dy)<distSQ)
              children[2].kClosestPoints(x, y, k, distSQ, results);
          }
          else
          {
            int dy = y-cy;
            dy*=dy;

            //0=x0 y0
            //1=x1 y0
            //2=x0 y1
            //3=x1 y1 <--

            children[3].kClosestPoints(x, y, k, distSQ, results);

            if(dx<distSQ)
              children[2].kClosestPoints(x, y, k, distSQ, results);

            if(dy<distSQ)
              children[1].kClosestPoints(x, y, k, distSQ, results);

            if((dx+dy)<distSQ)
              children[0].kClosestPoints(x, y, k, distSQ, results);
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

          int i=results.size();
          for(; i>0 && nDist<results.at(i-1).distSQ; i--){}
          results.insert(results.begin()+i, CoordDistance(c, nDist));

          if(int(results.size())>k)
          {
            const CoordDistance& b = results.back();
            if(b.distSQ<distSQ)
              distSQ=b.distSQ;
            results.pop_back();
          }
        }
      }
    }
  };

  Cell* m_root;
  int m_count{0};
};

}

#endif
