/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "OrientationLib/Math/OrientationMath.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"
#include "OrientationLib/OrientationOps/CubicOps.h"

#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "TestFileLocations.h"
#include <limits>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  // QFile::remove();
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestVectorRotation()
{
  QuatF equat;
  OrientationMath::EulertoQuat(DREAM3D::Constants::k_PiOver2, DREAM3D::Constants::k_PiOver2, DREAM3D::Constants::k_PiOver2, equat);
  // std::cout << "equat: " << equat.w << ", <" << equat.x << ", " << equat.y << ", " << equat.z << ">"  << std::endl;


  CubicOps cubic;
  int nsym = cubic.getNumSymOps();
  QuatF sym_q;
  float xstl_norm[3] = {1.0, 0.0, 0.0};
  // std::cout << "xstl_norm: " << xstl_norm[0] << ", " << xstl_norm[1] << ", " << xstl_norm[2]  << std::endl;
  float s_xstl_norm_rot[3];
  float s_xstl_norm_quat[3];
  float delta[3];
  for (int j = 0; j < nsym; j++)
  {
    cubic.getQuatSymOp(j, sym_q);
    // std::cout << "sym_q: " << sym_q.w << ", <" << sym_q.x << ", " << sym_q.y << ", " << sym_q.z << ">"  << std::endl;
    OrientationMath::MultiplyQuaternionVector(sym_q, xstl_norm, s_xstl_norm_rot);
    // std::cout << "Rotation Matrix: " << s_xstl_norm_rot[0] << ", " << s_xstl_norm_rot[1] << ", " << s_xstl_norm_rot[2]  << std::endl;
    QuaternionMathF::MultiplyQuatVec(sym_q, xstl_norm, s_xstl_norm_quat);
    // std::cout << "Quaternion:      " << s_xstl_norm_quat[0] << ", " << s_xstl_norm_quat[1] << ", " << s_xstl_norm_quat[2]  << std::endl;

    delta[0] = s_xstl_norm_rot[0] - s_xstl_norm_quat[0];
    delta[1] = s_xstl_norm_rot[1] - s_xstl_norm_quat[1];
    delta[2] = s_xstl_norm_rot[2] - s_xstl_norm_quat[2];

    DREAM3D_REQUIRE(delta[0] >= -std::numeric_limits<float>::epsilon() && delta[0] <= std::numeric_limits<float>::epsilon())
    DREAM3D_REQUIRE(delta[1] >= -std::numeric_limits<float>::epsilon() && delta[1] <= std::numeric_limits<float>::epsilon())
    DREAM3D_REQUIRE(delta[2] >= -std::numeric_limits<float>::epsilon() && delta[2] <= std::numeric_limits<float>::epsilon())

    // QuatF passive = OrientationMath::PassiveRotation(0.5, 0.5, 0.5, -0.5, 1, 0, 0);
    // std::cout << "passive: " << passive.w << ", <" << passive.x << ", " << passive.y << ", " << passive.z << ">"  << std::endl;

    // QuatF active = OrientationMath::ActiveRotation(0.5, 0.5, 0.5, -0.5, 1, 0, 0);
    // std::cout << "active: " << active.w << ", <" << active.x << ", " << active.y << ", " << active.z << ">"  << std::endl;



  }
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void TestCubicOps()
//{
//  float e0[3] = {0.0f, 0.0f, 0.0f};
//  float e1[3] = {0.0f, 0.0f, 30.0f * M_PI / 180.f};

//  QuaternionMathF::Quaternion q0 = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);
//  QuaternionMathF::Quaternion q1 = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);

//  OrientationMath::EulertoQuat(e0[0], e0[1], e0[2], q0);
//  OrientationMath::EulertoQuat(e1[0], e1[1], e1[2], q0);

//  CubicOps co;

//  float n[3] = {0.0, 0.0, 1.0f};
//  float w = co.getMisoQuat(q0, q1, n[0], n[1], n[2]);
//  DREAM3D_REQUIRE(w >= 0.523598900 && w <= 0.523598914)
//}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestQuat_t()
{



  QuatF p = QuaternionMathF::New(1.0f, 0.0f, 0.0f, 1.0f);
  QuatF q = QuaternionMathF::New(0.0f, 1.0f, 0.0f, 2.0f);
  QuatF out = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);
  QuatF out2 = QuaternionMathF::New(10.0f, 20.0f, 30.0f, 40.0f);

  QuaternionMathF::Negate(out2);
  DREAM3D_REQUIRE_EQUAL(out2.x, -10.0)
  DREAM3D_REQUIRE_EQUAL(out2.y, -20.0)
  DREAM3D_REQUIRE_EQUAL(out2.z, -30.0)
  DREAM3D_REQUIRE_EQUAL(out2.w, -40.0)

  QuaternionMathF::Copy(p, out);
  DREAM3D_REQUIRE_EQUAL(p.x, out.x)
  DREAM3D_REQUIRE_EQUAL(p.y, out.y)
  DREAM3D_REQUIRE_EQUAL(p.z, out.z)
  DREAM3D_REQUIRE_EQUAL(p.w, out.w)

  QuaternionMathF::Identity(out);
  DREAM3D_REQUIRE_EQUAL(out.x, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 1.0)

  out = QuaternionMathF::New(-10.5f, -1.5f, -30.66f, -40.987f);
  QuaternionMathF::ElementWiseAbs(out);
  DREAM3D_REQUIRE_EQUAL(out.x, 10.5f)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.5f)
  DREAM3D_REQUIRE_EQUAL(out.z, 30.66f)
  DREAM3D_REQUIRE_EQUAL(out.w, 40.987f)

  out = QuaternionMathF::New(10.0f, 20.0f, 30.0f, 40.0f);
  QuaternionMathF::ScalarMultiply(out, -1.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, -10.0)
  DREAM3D_REQUIRE_EQUAL(out.y, -20.0)
  DREAM3D_REQUIRE_EQUAL(out.z, -30.0)
  DREAM3D_REQUIRE_EQUAL(out.w, -40.0)

  QuaternionMathF::ScalarDivide(out, -1.0f);

  QuaternionMathF::ScalarAdd(out, 50.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, 60.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 70.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 80.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 90.0)

  QuaternionMathF::ElementWiseAssign(out, 5.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 5.0)

  QuaternionMathF::Negate(out);


  QuaternionMathF::Add(p, q, out);
  QuaternionMathF::Subtract(p, q, out);



  // Conjugate Tests where conjugate of a Quaternion is q*
  // (q*)* = q

  p.x = 1.0f;
  p.y = 2.0f;
  p.z = 3.0f;
  p.w = 1.0f;
  QuaternionMathF::Conjugate(p);
  DREAM3D_REQUIRE_EQUAL(p.x, -1.0)
  DREAM3D_REQUIRE_EQUAL(p.y, -2.0)
  DREAM3D_REQUIRE_EQUAL(p.z, -3.0)
  DREAM3D_REQUIRE_EQUAL(p.w, 1.0)
  QuaternionMathF::Conjugate(p);
  DREAM3D_REQUIRE_EQUAL(p.x, 1.0)
  DREAM3D_REQUIRE_EQUAL(p.y, 2.0)
  DREAM3D_REQUIRE_EQUAL(p.z, 3.0)
  DREAM3D_REQUIRE_EQUAL(p.w, 1.0)

  // (pq)* = q*p*
  q.x = 1.0f;
  q.y = 0.0f;
  q.z = 1.0f;
  q.w = 1.0f;
  QuaternionMathF::Multiply(p, q, out);
  QuaternionMathF::Conjugate(out);
  QuaternionMathF::Conjugate(p);
  QuaternionMathF::Conjugate(q);
  QuaternionMathF::Multiply(q, p, out2);
  DREAM3D_REQUIRE_EQUAL(out.x, out2.x)
  DREAM3D_REQUIRE_EQUAL(out.y, out2.y)
  DREAM3D_REQUIRE_EQUAL(out.z, out2.z)
  DREAM3D_REQUIRE_EQUAL(out.w, out2.w)

  //(p+q)* = p*+q*
  p.x = 1.0f;
  p.y = 2.0f;
  p.z = 3.0f;
  p.w = 1.0f;
  q.x = 1.0f;
  q.y = 0.0f;
  q.z = 1.0f;
  q.w = 1.0f;
  QuaternionMathF::Add(p, q, out);
  QuaternionMathF::Conjugate(out);
  QuaternionMathF::Conjugate(p);
  QuaternionMathF::Conjugate(q);
  QuaternionMathF::Add(p, q, out2);
  DREAM3D_REQUIRE_EQUAL(out.x, out2.x)
  DREAM3D_REQUIRE_EQUAL(out.y, out2.y)
  DREAM3D_REQUIRE_EQUAL(out.z, out2.z)
  DREAM3D_REQUIRE_EQUAL(out.w, out2.w)

  // Multiplication Test
  // pq != qp
  p.x = 1.0f;
  p.y = 0.0f;
  p.z = 0.0f;
  p.w = 1.0f;
  q.x = 0.0f;
  q.y = 1.0f;
  q.z = 0.0f;
  q.w = 2.0f;

  QuaternionMathF::Multiply(p, q, out);
  DREAM3D_REQUIRE_EQUAL(out.x, 2.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 2.0)

  QuaternionMathF::Multiply(q, p, out);
  DREAM3D_REQUIRE_EQUAL(out.x, 2.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.z, -1.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 2.0)

  // Norm Test
  // N(q*) = N(q)
  p.x = 1.0f;
  p.y = 0.0f;
  p.z = 0.0f;
  p.w = 1.0f;
  q.x = 0.0f;
  q.y = 1.0f;
  q.z = 0.0f;
  q.w = 2.0f;
  float norm = QuaternionMathF::Norm(p);
  QuaternionMathF::Conjugate(p);
  float cnorm =  QuaternionMathF::Norm(p);
  DREAM3D_REQUIRE_EQUAL(norm, cnorm)

  // Length and Unit Quaternion Tests
  p.x = 2.0f;
  p.y = 2.0f;
  p.z = 2.0f;
  p.w = 2.0f;
  float length = QuaternionMathF::Length(p);
  DREAM3D_REQUIRE_EQUAL(length, 4.0);
  QuaternionMathF::UnitQuaternion(p);
  DREAM3D_REQUIRE_EQUAL(p.x, 0.5)
  DREAM3D_REQUIRE_EQUAL(p.y, 0.5)
  DREAM3D_REQUIRE_EQUAL(p.z, 0.5)
  DREAM3D_REQUIRE_EQUAL(p.w, 0.5)


  float vec[3] = { 0.0f, 0.0f, 0.0f};
  float ovec[3] = { 0.0f, 0.0f, 0.0f};

  QuaternionMathF::GetMisorientationVector(p, vec);
  QuaternionMathF::MultiplyQuatVec(q, vec, ovec);
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestQuat_t() )
  DREAM3D_REGISTER_TEST( TestVectorRotation() )

      //DREAM3D_REGISTER_TEST( TestCubicOps() )

      DREAM3D_REGISTER_TEST( RemoveTestFiles() )
      PRINT_TEST_SUMMARY();

  // TestVectorRotation();
  return err;
}
