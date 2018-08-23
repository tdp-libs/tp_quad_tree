TARGET = tp_quad_tree
TEMPLATE = lib

DEFINES += TDP_QUAD_TREE_LIBRARY

HEADERS += inc/tp_quad_tree/Globals.h

SOURCES += src/QuadTreeInt.cpp
HEADERS += inc/tp_quad_tree/QuadTreeInt.h

SOURCES += src/QuadTreeFloat.cpp
HEADERS += inc/tp_quad_tree/QuadTreeFloat.h

HEADERS += inc/tp_quad_tree/QuadTreeIntTemplate.h
