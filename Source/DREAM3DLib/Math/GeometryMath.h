/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _GeometryMath_H_
#define _GeometryMath_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataContainers/VertexArray.h"
#include "DREAM3DLib/DataContainers/FaceArray.hpp"
#include "DREAM3DLib/DataContainers/DynamicListArray.hpp"


/*
 * @class GeometryMath GeometryMath.h DREAM3DLib/Common/GeometryMath.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @date Feb 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT GeometryMath
{
  public:
    DREAM3D_SHARED_POINTERS(GeometryMath)
    DREAM3D_TYPE_MACRO(GeometryMath)

    virtual ~GeometryMath();

    /**
     * @brief Calculates the Cosine of the angle between 2 vectors. To get the actual angle the programmer should
     * use the following form: float radians = acos(GeometryMath::CosThetaBetweenVectors(a, b));
     * @param a 1x3 Vector
     * @param b 1x3 Vector
     * @return
     */
    static float CosThetaBetweenVectors(float a[3], float b[3]);

    /**
     * @brief Computes the angle in RADIANS between 2 vectors.
     * @param a
     * @param b
     * @return
     */
    static float AngleBetweenVectors(float a[3], float b[3]);

    /**
     * @brief Computes the area of a triangle defined by 3 points.
     * @param a
     * @param b
     * @param distance
     * @return
     */
    static void FindDistanceBetweenPoints(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, float& distance);

    /**
     * @brief Computes the area of a triangle defined by 3 points.
     * @param a
     * @param b
     * @param c
     * @param area
     * @return
     */
    static void FindTriangleArea(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, float& area);

    /**
     * @brief Computes the volume of a tetrahedron defined by 4 points.
     * @param a
     * @param b
     * @param c
     * @param d
     * @param volume
     * @return
     */
    static void FindTetrahedronVolume(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& d, float& volume);

    /**
     * @brief Computes the normal vector to a plane containing 3 points (AB X AC).
     * @param a
     * @param b
     * @param c
     * @param n
     * @return
     */
    static void FindPlaneNormalVector(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, float n[3]);

    /**
     * @brief Computes the coefficients of a plane containing 3 points (AB X AC).
     * @param a
     * @param b
     * @param c
     * @param n 1x3 Vector
     * @param d float
     * @return
     */
    static void FindPlaneCoefficients(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, float n[3], float& d);

    /**
     * @brief Determines if a point is inside of a box defined by the lower left and upper right corners
     * @param p
     * @param lowerLeft
     * @param upperRight
     * @return
     */
    static bool PointInBox(const VertexArray::Vert_t& p, const VertexArray::Vert_t& lowerLeft, const VertexArray::Vert_t& upperRight);

    /**
     * @brief Determines if a point is inside of a polyhedron defined by a set of faces
     * @param p
     * @param lowerLeft
     * @param upperRight
     * @return
     */
    static char PointInPolyhedron(const FaceArray::Pointer faces,
                                  const Int32DynamicListArray::ElementList& faceIds,
                                  const VertexArray::Pointer faceBBs,
                                  const VertexArray::Vert_t& q,
                                  const VertexArray::Vert_t& ll,
                                  const VertexArray::Vert_t& ur,
                                  float radius);
    /**
     * @brief Determines if a point is inside of a triangle defined by 3 points
     * @param a
     * @param b
     * @param c
     * @param m int
     * @param p
     * @return
     */
    static char PointInTriangle3D(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, int m, const VertexArray::Vert_t& p);

    /**
     * @brief Determines if a point is inside of a triangle defined by 3 points
     * @param a
     * @param b
     * @param c
     * @param p
     * @return
     */
    static char PointInTriangle2D(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& p);

    /**
     * @brief Determines if a segment between two points intersects a box defined by the lower left and upper right corners
     * @param p
     * @param q
     * @param lowerLeft
     * @param upperRight
     * @return
     */
    static bool RayIntersectsBox(const VertexArray::Vert_t& p, const VertexArray::Vert_t& q, const VertexArray::Vert_t& lowerLeft, const VertexArray::Vert_t& upperRight);

    /**
     * @brief Determines length of a segment between two points that lies within a box defined by the lower left and upper right corners
     * @param p
     * @param q
     * @param lowerLeft
     * @param upperRight
     * @return
     */
    static float LengthOfRayInBox(const VertexArray::Vert_t& p, const VertexArray::Vert_t& q, const VertexArray::Vert_t& lowerLeft, const VertexArray::Vert_t& upperRight);

    /**
     * @brief Creates a randomly oriented ray of given length
     * @param length float
     * @param ray 1x3 Vector
     * @return
     */
    static void GenerateRandomRay(float length, float ray[3]);

    /**
     * @brief Determines the bounding box defined by the lower left and upper right corners of a set of vertices
     * @param verts pointer to vertex array
     * @param lowerLeft
     * @param upperRight
     * @return
     */
    static void FindBoundingBoxOfVertices(VertexArray::Pointer verts, VertexArray::Vert_t& lowerLeft, VertexArray::Vert_t& upperRight);

    /**
     * @brief Determines the bounding box defined by the lower left and upper right corners of a set of vertices
     * @param verts pointer to vertex array
     * @param lowerLeft
     * @param upperRight
     * @return
     */
    static void FindBoundingBoxOfFace(FaceArray::Pointer faces, int faceId, VertexArray::Vert_t& lowerLeft, VertexArray::Vert_t& upperRight);

    /**
     * @brief Determines the bounding box defined by the lower left and upper right corners of a set of vertices
     * @param verts pointer to vertex array
     * @param lowerLeft
     * @param upperRight
     * @return
     */
    static void FindBoundingBoxOfRotatedFace(FaceArray::Pointer faces, int faceId, float g[3][3], VertexArray::Vert_t& lowerLeft, VertexArray::Vert_t& upperRight);

    /**
     * @brief Determines the bounding box defined by the lower left and upper right corners of a set of faces
     * @param faces pointer to face array
     * @param lowerLeft
     * @param upperRight
     * @return
     */
    static void FindBoundingBoxOfFaces(FaceArray::Pointer faces, Int32DynamicListArray::ElementList faceIds, VertexArray::Vert_t& lowerLeft, VertexArray::Vert_t& upperRight);

    /**
     * @brief Determines the bounding box defined by the lower left and upper right corners of a set of rotated faces
     * @param faces pointer to face array
     * param g
     * @param lowerLeft
     * @param upperRight
     * @return
     */
    static void FindBoundingBoxOfRotatedFaces(FaceArray::Pointer faces, Int32DynamicListArray::ElementList faceIds, float g[3][3], VertexArray::Vert_t& lowerLeft, VertexArray::Vert_t& upperRight);

    /**
     * @brief Determines if a segment between two points intersects a triangle defined by 3 points
     * @param q
     * @param r
     * @param a
     * @param b
     * @param c
     * @param p
     * @return
     */
    static char RayIntersectsTriangle(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& q, const VertexArray::Vert_t& r, VertexArray::Vert_t& p);

    /**
     * @brief Determines if a segment between two points crosses a triangle defined by 3 points
     * @param q
     * @param r
     * @param a
     * @param b
     * @param c
     * @return
     */
    static char RayCrossesTriangle(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& q, const VertexArray::Vert_t& r);

    /**
     * @brief Determines if a segment between two points intersects a plane defined by 3 points
     * @param q
     * @param r
     * @param a
     * @param b
     * @param c
     * @param p
     * @return
     */
    static char RayIntersectsPlane(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& q, const VertexArray::Vert_t& r, VertexArray::Vert_t& p, int& m);

  protected:
    GeometryMath();

  private:
    GeometryMath(const GeometryMath&); // Copy Constructor Not Implemented
    void operator=(const GeometryMath&); // Operator '=' Not Implemented
};

#endif /* _GeometryMath_H_ */

