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
#include "GeometryMath.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeometryMath::GeometryMath()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeometryMath::~GeometryMath()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float GeometryMath::CosThetaBetweenVectors(float a[3], float b[3])
{
  float norm1 = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  float norm2 = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
  if(norm1 == 0 || norm2 == 0)
  {
    return 1.0;
  }
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / (norm1 * norm2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float GeometryMath::AngleBetweenVectors(float a[3], float b[3])
{
  float norm1 = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  float norm2 = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
  float cosAng = (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / (norm1 * norm2);
  DREAM3DMath::boundF(cosAng, -1, 1);
  return acos(cosAng);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GeometryMath::PointInBox(const VertexArray::Vert_t& p, const VertexArray::Vert_t& ll, const VertexArray::Vert_t& ur)
{
  if((ll.pos[0] <= p.pos[0]) && (p.pos[0] <= ur.pos[0]) && (ll.pos[1] <= p.pos[1]) && (p.pos[1] <= ur.pos[1]) && (ll.pos[2] <= p.pos[2]) && (p.pos[2] <= ur.pos[2]))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GeometryMath::RayIntersectsBox(const VertexArray::Vert_t& p,
                                    const VertexArray::Vert_t& q,
                                    const VertexArray::Vert_t& ll,
                                    const VertexArray::Vert_t& ur)
{
  if((ll.pos[0] > p.pos[0]) && (ll.pos[0] > q.pos[0]))
  {
    return false;
  }
  else if((ur.pos[0] < p.pos[0]) && (ur.pos[0] < q.pos[0]))
  {
    return false;
  }
  else if((ll.pos[1] > p.pos[1]) && (ll.pos[1] > q.pos[1]))
  {
    return false;
  }
  else if((ur.pos[1] < p.pos[1]) && (ur.pos[1] < q.pos[1]))
  {
    return false;
  }
  else if((ll.pos[2] > p.pos[2]) && (ll.pos[2] > q.pos[2]))
  {
    return false;
  }
  else if((ur.pos[2] < p.pos[2]) && (ur.pos[2] < q.pos[2]))
  {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float GeometryMath::LengthOfRayInBox(const VertexArray::Vert_t& p,
                                     const VertexArray::Vert_t& q,
                                     const VertexArray::Vert_t& ll,
                                     const VertexArray::Vert_t& ur)
{
  float length = 0.0;
  float frac = 0.0;

  float x1 = p.pos[0];
  float y1 = p.pos[1];
  float z1 = p.pos[2];
  float x2 = q.pos[0];
  float y2 = q.pos[1];
  float z2 = q.pos[2];

  float delX = x2 - x1;
  float delY = y2 - y1;
  float delZ = z2 - z1;

  float c1x = ll.pos[0];
  float c1y = ll.pos[1];
  float c1z = ll.pos[2];
  float c2x = ur.pos[0];
  float c2y = ur.pos[1];
  float c2z = ur.pos[2];

  //clip ray by min x face of box
  if(x1 < c1x && x2 > c1x)
  {
    frac = ((c1x - x1) / delX);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(x1 > c1x && x2 < c1x)
  {
    frac = ((c1x - x2) / delX);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(x1 < c1x && x2 < c1x)
  {
    return 0.0;
  }
  //clip ray by min y face of box
  if(y1 < c1y && y2 > c1y)
  {
    frac = ((c1y - y1) / delY);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(y1 > c1y && y2 < c1y)
  {
    frac = ((c1y - y2) / delY);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(y1 < c1y && y2 < c1y)
  {
    return 0.0;
  }
  //clip ray by min z face of box
  if(z1 < c1z && z2 > c1z)
  {
    frac = ((c1z - z1) / delZ);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(z1 > c1z && z2 < c1z)
  {
    frac = ((c1z - z2) / delZ);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(z1 < c1z && z2 < c1z)
  {
    return 0.0;
  }
  //clip ray by max x face of box
  if(x1 > c2x && x2 < c2x)
  {
    frac = ((c2x - x1) / delX);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(x1 < c2x && x2 > c2x)
  {
    frac = ((c2x - x2) / delX);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(x1 > c2x && x2 > c2x)
  {
    return 0.0;
  }
  //clip ray by max y face of box
  if(y1 > c2y && y2 < c2y)
  {
    frac = ((c2y - y1) / delY);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(y1 < c2y && y2 > c2y)
  {
    frac = ((c2y - y2) / delY);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(y1 > c2y && y2 > c2y)
  {
    return 0.0;
  }
  //clip ray by max z face of box
  if(z1 > c2z && z2 < c2z)
  {
    frac = ((c2z - z1) / delZ);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(z1 < c2z && z2 > c2z)
  {
    frac = ((c2z - z2) / delZ);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if(z1 > c2z && z2 > c2z)
  {
    return 0.0;
  }

  length = ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)) + ((z2 - z1) * (z2 - z1));
  length = sqrt(length);

  return length;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::GenerateRandomRay(float length, float ray[3])
{
  float w, t;

  DREAM3D_RANDOMNG_NEW();

// unsigned long long int m_Seed = QDateTime::currentMSecsSinceEpoch();
// DREAM3D_RANDOMNG_NEW_SEEDED(m_Seed);

  ray[2] = (2.0 * rg.genrand_real1()) - 1.0;
  t = (DREAM3D::Constants::k_2Pi * rg.genrand_real1());
  w = sqrt(1.0 - (ray[2] * ray[2]));
  ray[0] = w * cos(t);
  ray[1] = w * sin(t);
  ray[0] *= length;
  ray[1] *= length;
  ray[2] *= length;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfVertices(VertexArray::Pointer verts, VertexArray::Vert_t& ll, VertexArray::Vert_t& ur)
{
  ll.pos[0] = 100000000.0;
  ll.pos[1] = 100000000.0;
  ll.pos[2] = 100000000.0;
  ur.pos[0] = 0.0;
  ur.pos[1] = 0.0;
  ur.pos[2] = 0.0;

  VertexArray::Vert_t* v = verts->getPointer(0);
  int numVerts = verts->count();
  for(int i = 0; i < numVerts; i++)
  {
    if(v[i].pos[0] < ll.pos[0])
    {
      ll.pos[0] = v[i].pos[0];
    }
    if(v[i].pos[0] > ur.pos[0])
    {
      ur.pos[0] = v[i].pos[0];
    }
    if(v[i].pos[1] < ll.pos[1])
    {
      ll.pos[1] = v[i].pos[1];
    }
    if(v[i].pos[1] > ur.pos[1])
    {
      ur.pos[1] = v[i].pos[1];
    }
    if(v[i].pos[2] < ll.pos[2])
    {
      ll.pos[2] = v[i].pos[2];
    }
    if(v[i].pos[2] > ur.pos[2])
    {
      ur.pos[2] = v[i].pos[2];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfFaces(FaceArray::Pointer faces, Int32DynamicListArray::ElementList faceIds, VertexArray::Vert_t& ll, VertexArray::Vert_t& ur)
{
  ll.pos[0] = 100000000.0;
  ll.pos[1] = 100000000.0;
  ll.pos[2] = 100000000.0;
  ur.pos[0] = 0.0;
  ur.pos[1] = 0.0;
  ur.pos[2] = 0.0;

  VertexArray::Vert_t facell, faceur;

  int numFaces = faceIds.ncells;
  int32_t* faceId = faceIds.cells;
  for(int i = 0; i < numFaces; i++)
  {
    FindBoundingBoxOfFace(faces, faceId[i], facell, faceur);
    if(facell.pos[0] < ll.pos[0])
    {
      ll.pos[0] = facell.pos[0];
    }
    if(facell.pos[1] < ll.pos[1])
    {
      ll.pos[1] = facell.pos[1];
    }
    if(facell.pos[2] < ll.pos[2])
    {
      ll.pos[2] = facell.pos[2];
    }
    if(faceur.pos[0] > ur.pos[0])
    {
      ur.pos[0] = faceur.pos[0];
    }
    if(faceur.pos[1] > ur.pos[1])
    {
      ur.pos[1] = faceur.pos[1];
    }
    if(faceur.pos[2] > ur.pos[2])
    {
      ur.pos[2] = faceur.pos[2];
    }
  }

  if(faceIds.ncells == 0)
  {
    ll.pos[0] = 0.0;
    ll.pos[1] = 0.0;
    ll.pos[2] = 0.0;
    ur.pos[0] = 0.0;
    ur.pos[1] = 0.0;
    ur.pos[2] = 0.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfRotatedFaces(FaceArray::Pointer faces, Int32DynamicListArray::ElementList faceIds, float g[3][3], VertexArray::Vert_t& ll, VertexArray::Vert_t& ur)
{
  ll.pos[0] = 100000000.0;
  ll.pos[1] = 100000000.0;
  ll.pos[2] = 100000000.0;
  ur.pos[0] = 0.0;
  ur.pos[1] = 0.0;
  ur.pos[2] = 0.0;

  VertexArray::Vert_t facell, faceur;

  int numFaces = faceIds.ncells;
  int32_t* faceId = faceIds.cells;
  for(int i = 0; i < numFaces; i++)
  {
    FindBoundingBoxOfRotatedFace(faces, faceId[i], g, facell, faceur);
    if(facell.pos[0] < ll.pos[0])
    {
      ll.pos[0] = facell.pos[0];
    }
    if(facell.pos[1] < ll.pos[1])
    {
      ll.pos[1] = facell.pos[1];
    }
    if(facell.pos[2] < ll.pos[2])
    {
      ll.pos[2] = facell.pos[2];
    }
    if(faceur.pos[0] > ur.pos[0])
    {
      ur.pos[0] = faceur.pos[0];
    }
    if(faceur.pos[1] > ur.pos[1])
    {
      ur.pos[1] = faceur.pos[1];
    }
    if(faceur.pos[2] > ur.pos[2])
    {
      ur.pos[2] = faceur.pos[2];
    }
  }

  if(numFaces == 0)
  {
    ll.pos[0] = 0.0;
    ll.pos[1] = 0.0;
    ll.pos[2] = 0.0;
    ur.pos[0] = 0.0;
    ur.pos[1] = 0.0;
    ur.pos[2] = 0.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfFace(FaceArray::Pointer faces, int faceId, VertexArray::Vert_t& ll, VertexArray::Vert_t& ur)
{
  VertexArray::Vert_t a, b, c;

  faces->getVertObjects(faceId, a, b, c);
  ll.pos[0] = a.pos[0];
  ur.pos[0] = a.pos[0];
  ll.pos[1] = a.pos[1];
  ur.pos[1] = a.pos[1];
  ll.pos[2] = a.pos[2];
  ur.pos[2] = a.pos[2];
  if(b.pos[0] < ll.pos[0])
  {
    ll.pos[0] = b.pos[0];
  }
  if(b.pos[0] > ur.pos[0])
  {
    ur.pos[0] = b.pos[0];
  }
  if(b.pos[1] < ll.pos[1])
  {
    ll.pos[1] = b.pos[1];
  }
  if(b.pos[1] > ur.pos[1])
  {
    ur.pos[1] = b.pos[1];
  }
  if(b.pos[2] < ll.pos[2])
  {
    ll.pos[2] = b.pos[2];
  }
  if(b.pos[2] > ur.pos[2])
  {
    ur.pos[2] = b.pos[2];
  }
  if(c.pos[0] < ll.pos[0])
  {
    ll.pos[0] = c.pos[0];
  }
  if(c.pos[0] > ur.pos[0])
  {
    ur.pos[0] = c.pos[0];
  }
  if(c.pos[1] < ll.pos[1])
  {
    ll.pos[1] = c.pos[1];
  }
  if(c.pos[1] > ur.pos[1])
  {
    ur.pos[1] = c.pos[1];
  }
  if(c.pos[2] < ll.pos[2])
  {
    ll.pos[2] = c.pos[2];
  }
  if(c.pos[2] > ur.pos[2])
  {
    ur.pos[2] = c.pos[2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfRotatedFace(FaceArray::Pointer faces, int faceId, float g[3][3], VertexArray::Vert_t& ll, VertexArray::Vert_t& ur)
{
  VertexArray::Vert_t a, b, c;
  float p1[3], p2[3], p3[3];
  float p1r[3], p2r[3], p3r[3];

  faces->getVertObjects(faceId, a, b, c);
  p1[0] = a.pos[0];
  p1[1] = a.pos[1];
  p1[2] = a.pos[2];
  p2[0] = b.pos[0];
  p2[1] = b.pos[1];
  p2[2] = b.pos[2];
  p3[0] = c.pos[0];
  p3[1] = c.pos[1];
  p3[2] = c.pos[2];
  MatrixMath::Multiply3x3with3x1(g, p1, p1r);
  MatrixMath::Multiply3x3with3x1(g, p2, p2r);
  MatrixMath::Multiply3x3with3x1(g, p3, p3r);
  ll.pos[0] = p1r[0];
  ur.pos[0] = p1r[0];
  ll.pos[1] = p1r[1];
  ur.pos[1] = p1r[1];
  ll.pos[2] = p1r[2];
  ur.pos[2] = p1r[2];
  if(p2r[0] < ll.pos[0])
  {
    ll.pos[0] = p2r[0];
  }
  if(p2r[0] > ur.pos[0])
  {
    ur.pos[0] = p2r[0];
  }
  if(p2r[1] < ll.pos[1])
  {
    ll.pos[1] = p2r[1];
  }
  if(p2r[1] > ur.pos[1])
  {
    ur.pos[1] = p2r[1];
  }
  if(p2r[2] < ll.pos[2])
  {
    ll.pos[2] = p2r[2];
  }
  if(p2r[2] > ur.pos[2])
  {
    ur.pos[2] = p2r[2];
  }
  if(p3r[0] < ll.pos[0])
  {
    ll.pos[0] = p3r[0];
  }
  if(p3r[0] > ur.pos[0])
  {
    ur.pos[0] = p3r[0];
  }
  if(p3r[1] < ll.pos[1])
  {
    ll.pos[1] = p3r[1];
  }
  if(p3r[1] > ur.pos[1])
  {
    ur.pos[1] = p3r[1];
  }
  if(p3r[2] < ll.pos[2])
  {
    ll.pos[2] = p3r[2];
  }
  if(p3r[2] > ur.pos[2])
  {
    ur.pos[2] = p3r[2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindPlaneNormalVector(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, float n[3])
{
  float ab[3], ac[3];

  ab[0] = b.pos[0] - a.pos[0];
  ab[1] = b.pos[1] - a.pos[1];
  ab[2] = b.pos[2] - a.pos[2];

  ac[0] = c.pos[0] - a.pos[0];
  ac[1] = c.pos[1] - a.pos[1];
  ac[2] = c.pos[2] - a.pos[2];

  MatrixMath::CrossProduct(ab, ac, n);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindPlaneCoefficients(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, float n[3], float& d)
{
  FindPlaneNormalVector(a, b, c, n);

  d = (a.pos[0] * n[0]) + (a.pos[1] * n[1]) + (a.pos[2] * n[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindDistanceBetweenPoints(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, float& distance)
{
  float dx = b.pos[0] - a.pos[0];
  float dy = b.pos[1] - a.pos[1];
  float dz = b.pos[2] - a.pos[2];
  distance = sqrt((dx * dx) + (dy * dy) + (dz * dz));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindTriangleArea(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, float& area)
{
  area = ((b.pos[0] - a.pos[0]) * (c.pos[1] - a.pos[1])) - ((c.pos[0] - a.pos[0]) * (b.pos[1] - a.pos[1]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindTetrahedronVolume(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& d, float& volume)
{
  float axdx, aydy, azdz, bxdx, bydy, bzdz, cxdx, cydy, czdz;

  axdx = a.pos[0] - d.pos[0];
  aydy = a.pos[1] - d.pos[1];
  azdz = a.pos[2] - d.pos[2];
  bxdx = b.pos[0] - d.pos[0];
  bydy = b.pos[1] - d.pos[1];
  bzdz = b.pos[2] - d.pos[2];
  cxdx = c.pos[0] - d.pos[0];
  cydy = c.pos[1] - d.pos[1];
  czdz = c.pos[2] - d.pos[2];

  volume = (azdz * ((bxdx * cydy) - (bydy * cxdx))) + (aydy * ((bzdz * cxdx) - (bxdx * czdz))) + (axdx * ((bydy * czdz) - (bzdz * cydy)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::RayIntersectsTriangle(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& q, const VertexArray::Vert_t& r, VertexArray::Vert_t& p)
{
  char code = '?';
  int m = -1;

  code = RayIntersectsPlane(a, b, c, q, r, p, m);

  if(code == '0')
  {
    return '0';
  }
  else if(code == 'q')
  {
    return PointInTriangle3D(a, b, c, m, q);
  }
  else if(code == 'r')
  {
    return PointInTriangle3D(a, b, c, m, r);
  }
  else if(code == 'p')
  {
    return 'p';
  }
  else if(code == '1')
  {
    return RayCrossesTriangle(a, b, c, q, r);
  }
  else
  {
    return code;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::RayIntersectsPlane(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& q, const VertexArray::Vert_t& r, VertexArray::Vert_t& p, int& m)
{
  float n[3];
  float rq[3];
  float d, num, denom, t;

  FindPlaneCoefficients(a, b, c, n, d);

  num = d - ((q.pos[0] * n[0]) + (q.pos[1] * n[1]) + (q.pos[2] * n[2]));
  rq[0] =  r.pos[0] - q.pos[0];
  rq[1] =  r.pos[1] - q.pos[1];
  rq[2] =  r.pos[2] - q.pos[2];
  denom = (rq[0] * n[0]) + (rq[1] * n[1]) + (rq[2] * n[2]);
  m = MatrixMath::FindIndexOfMaxVal3x1(n);

  if(denom == 0.0)
  {
    if(num == 0.0)
    {
      return 'p';
    }
    else
    {
      return '0';
    }
  }
  else
  {
    t = num / denom;
    for(int i = 0; i < 3; i++)
    {
      p.pos[i] = q.pos[i] + (t * (r.pos[i] - q.pos[i]));
    }
    if(t > 0.0 && t < 1.0)
    {
      return '1';
    }
    else if (num == 0.0)
    {
      return 'q';
    }
    else if (num == denom)
    {
      return 'r';
    }
    else
    {
      return '0';
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::PointInTriangle3D(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, int m, const VertexArray::Vert_t& p)
{
  VertexArray::Vert_t pp, aP, bP, cP;

  int j = 0;
  for(int i = 0; i < 3; i++)
  {
    if(i != m)
    {
      pp.pos[j] = p.pos[i];
      aP.pos[j] = a.pos[i];
      bP.pos[j] = b.pos[i];
      cP.pos[j] = c.pos[i];
      j++;
    }
  }
  return PointInTriangle2D(aP, bP, cP, pp);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::PointInTriangle2D(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& p)
{
  float area0, area1, area2;

  FindTriangleArea(p, a, b, area0);
  FindTriangleArea(p, b, c, area1);
  FindTriangleArea(p, c, a, area2);

  if((area0 == 0 && area1 > 0 && area2 > 0) || (area1 == 0 && area0 > 0 && area2 > 0) || (area2 == 0 && area0 > 0 && area1 > 0))
  {
    return 'E';
  }
  else if((area0 == 0 && area1 < 0 && area2 < 0) || (area1 == 0 && area0 < 0 && area2 < 0) || (area2 == 0 && area0 < 0 && area1 < 0))
  {
    return 'E';
  }
  else if((area0 > 0 && area1 > 0 && area2 > 0) || (area0 < 0 && area1 < 0 && area2 < 0))
  {
    return 'F';
  }
  else if((area0 == 0 && area1 == 0 && area2 == 0))
  {
    return '?';
  }
  else if((area0 == 0 && area1 == 0) || (area0 == 0 && area2 == 0) || (area1 == 0 && area2 == 0))
  {
    return 'V';
  }
  else
  {
    return '0';
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::RayCrossesTriangle(const VertexArray::Vert_t& a, const VertexArray::Vert_t& b, const VertexArray::Vert_t& c, const VertexArray::Vert_t& q, const VertexArray::Vert_t& r)
{
  float vol0, vol1, vol2;

  FindTetrahedronVolume(q, a, b, r, vol0);
  FindTetrahedronVolume(q, b, c, r, vol1);
  FindTetrahedronVolume(q, c, a, r, vol2);

  if((vol0 > 0 && vol1 > 0 && vol2 > 0) || (vol0 < 0 && vol1 < 0 && vol2 < 0))
  {
    return 'f';
  }
  else if((vol0 > 0 || vol1 > 0 || vol2 > 0) && (vol0 < 0 || vol1 < 0 || vol2 < 0))
  {
    return '0';
  }
  else if((vol0 == 0 && vol1 == 0 && vol2 == 0))
  {
    return '?';
  }
  else if((vol0 == 0 && vol1 == 0) || (vol0 == 0 && vol2 == 0) || (vol1 == 0 && vol2 == 0))
  {
    return 'v';
  }
  else if(vol0 == 0 || vol1 == 0 || vol2 == 0)
  {
    return 'e';
  }
  else
  {
    return '?';
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::PointInPolyhedron(const FaceArray::Pointer faces,
                                     const Int32DynamicListArray::ElementList& faceIds,
                                     const VertexArray::Pointer faceBBs,
                                     const VertexArray::Vert_t& q,
                                     const VertexArray::Vert_t& ll,
                                     const VertexArray::Vert_t& ur,
                                     float radius)
{
  float ray[3];  /* Ray */
  VertexArray::Vert_t r;  /* Ray endpoint. */
  VertexArray::Vert_t p;  /* Intersection point; not used. */
  int f, k = 0, crossings = 0;
  char code = '?';
  VertexArray::Vert_t a, b, c;

  //* If query point is outside bounding box, finished. */
  if(PointInBox(q, ll, ur) == false)
  {
    return 'o';
  }

  int numFaces = faceIds.ncells;
  int32_t* faceId = faceIds.cells;

  p.pos[0] = 0;
  p.pos[1] = 0;
  p.pos[2] = 0;

  VertexArray::Vert_t* faceBBsPtr = faceBBs->getPointer(0);
LOOP:
  while( k++ < numFaces )
  {
    crossings = 0;

    //Generate and add ray to point to find other end
    GenerateRandomRay(radius, ray);
    r.pos[0] = q.pos[0] + ray[0];
    r.pos[1] = q.pos[1] + ray[1];
    r.pos[2] = q.pos[2] + ray[2];

    for ( f = 0; f < numFaces; f++ )
    {
      /* Begin check each face */
      int32_t idx = 2 * faceId[f];
      VertexArray::Vert_t v0 = faceBBsPtr[idx];
      VertexArray::Vert_t v1 = faceBBsPtr[idx + 1];
      if( RayIntersectsBox(q, r, v0, v1) == false )
      {
        code = '0';
      }
      else
      {
        faces->getVertObjects(faceId[f], a, b, c);
        code = RayIntersectsTriangle(a, b, c, q, r, p);
      }

      /* If ray is degenerate, then goto outer while to generate another. */
      if ( code == 'p' || code == 'v' || code == 'e' )
      {
        goto LOOP;
      }

      /* If ray hits face at interior point, increment crossings. */
      else if ( code == 'f' )
      {
        crossings++;
      }

      /* If query endpoint q sits on a V/E/F, return that code. */
      else if ( code == 'V' || code == 'E' || code == 'F' )
      {
        return(code);
      }

    } /* End check each face */

    /* No degeneracies encountered: ray is generic, so finished. */
    break;

  } /* End while loop */

  /* q strictly interior to polyhedron if an odd number of crossings. */
  if( ( crossings % 2 ) == 1 )
  {
    return 'i';
  }
  else
  {
    return 'o';
  }
}

