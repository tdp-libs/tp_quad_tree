#ifndef tp_quad_tree_QuadTreeFloat_h
#define tp_quad_tree_QuadTreeFloat_h

#include "tp_quad_tree/Globals.h" // IWYU pragma: keep

namespace tp_quad_tree
{

//##################################################################################################
class QuadTreeFloat
{
public:

  //################################################################################################
  struct Coord
  {
    float x;
    float y;

    //##############################################################################################
    Coord(float x_=0, float y_=0):
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
  QuadTreeFloat(float minX, float maxX, float minY, float maxY, int cellSize);

  //################################################################################################
  ~QuadTreeFloat();

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
  Coord closestPoint(const Coord& point, float& distSQ);

private:
  QuadTreeFloat(const QuadTreeFloat&);
  QuadTreeFloat& operator=(const QuadTreeFloat&);

  struct Cell;
  Cell* m_root;
};

}

#endif
