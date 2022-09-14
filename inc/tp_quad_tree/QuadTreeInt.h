#ifndef tp_quad_tree_QuadTreeInt_h
#define tp_quad_tree_QuadTreeInt_h

#include "tp_quad_tree/Globals.h" // IWYU pragma: keep

namespace tp_quad_tree
{

//##################################################################################################
class QuadTreeInt
{
public:

  //################################################################################################
  struct Coord
  {
    int x;
    int y;

    //##############################################################################################
    Coord(int x_=0, int y_=0):
      x(x_),
      y(y_)
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
  QuadTreeInt(int minX, int maxX, int minY, int maxY, int cellSize);

  //################################################################################################
  ~QuadTreeInt();

  //################################################################################################
  //! Add a coordinate to the tree
  /*!
  This will add a coordinate to the tree, this will divide cells as required.

  \param coord - The coordinate to add.
  */
  void addCoord(const Coord& coord);

  //################################################################################################
  //! Find the closes coord to the point
  /*!
  This will return the coordinate closest to the point.

  \param point - The point to find the nearest point to.
  \param distSQ - This will be updated with the distance to the closest coord the initial value \
         will limit the search radius.
  \return The coord if one is found, else a null Coord.
  */
  const Coord* closestPoint(const Coord& point, int& distSQ)const;

private:
  QuadTreeInt(const QuadTreeInt&);
  QuadTreeInt& operator=(const QuadTreeInt&);

  struct Cell;
  Cell* m_root;
};

}

#endif
