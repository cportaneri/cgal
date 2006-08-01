// Copyright (c) 1999  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Andreas Fabri, Stefan Schirra
 
#ifndef CGAL_PLANE_3_H
#define CGAL_PLANE_3_H

CGAL_BEGIN_NAMESPACE

template <class R_>
class Plane_3 : public R_::Kernel_base::Plane_3
{
  typedef typename R_::RT                    RT;
  typedef typename R_::Point_2               Point_2;
  typedef typename R_::Point_3               Point_3;
  typedef typename R_::Direction_3           Direction_3;
  typedef typename R_::Vector_3              Vector_3;
  typedef typename R_::Segment_3             Segment_3;
  typedef typename R_::Line_3                Line_3;
  typedef typename R_::Ray_3                 Ray_3;
  typedef typename R_::Aff_transformation_3  Aff_transformation_3;
  typedef typename R_::Kernel_base::Plane_3  RPlane_3;
public:

  typedef RPlane_3 Rep;

  const Rep& rep() const
  {
    return *this;
  }

  Rep& rep()
  {
    return *this;
  }

  typedef          R_                       R;

  Plane_3() {}

  Plane_3(const RPlane_3& p)
      : RPlane_3(p) {}

  Plane_3(const Point_3& p, const Point_3& q, const Point_3& r)
    : RPlane_3(p,q,r) {}

  Plane_3(const Point_3& p, const Direction_3& d)
    : RPlane_3(p,d) {}

  Plane_3(const Point_3& p, const Vector_3& v)
    : RPlane_3(p,v) {}

  Plane_3(const RT& a, const RT& b, const RT& c, const RT& d)
    : RPlane_3(a,b,c,d) {}

  Plane_3(const Line_3& l, const Point_3& p)
    : RPlane_3(l,p) {}

  Plane_3(const Segment_3& s, const Point_3& p)
    : RPlane_3(s,p) {}

  Plane_3(const Ray_3& r, const Point_3& p)
    : RPlane_3(r,p) {}

  Plane_3 transform(const Aff_transformation_3 &t) const
  {
    return t.transform(*this);
  }

  Plane_3 opposite() const
  {
    return R().construct_opposite_plane_3_object()(*this);
  }

  //Vector_3     orthogonal_vector() const;
  Direction_3 orthogonal_direction() const
  {
    return Direction_3(a(), b(), c());
  }

  typename Qualified_result_of<typename R::Compute_a_3, Plane_3>::type
  a() const
  {
    return R().compute_a_3_object()(*this);
  }

  typename Qualified_result_of<typename R::Compute_b_3, Plane_3>::type
  b() const
  {
    return R().compute_b_3_object()(*this);
  }

  typename Qualified_result_of<typename R::Compute_c_3, Plane_3>::type
  c() const
  {
    return R().compute_c_3_object()(*this);
  }

  typename Qualified_result_of<typename R::Compute_d_3, Plane_3>::type
  d() const
  {
    return R().compute_d_3_object()(*this);
  }

  bool has_on(const Point_3 &p) const
  {
    return R().has_on_3_object()(*this, p);
  }

  bool has_on(const Line_3 &l) const
  {
    return R().has_on_3_object()(*this, l);
  }

  Line_3 perpendicular_line(const Point_3 &p) const
  {
    return R().construct_perpendicular_line_3_object()(*this, p);
  }

  Point_3 projection(const Point_3 &p) const
  {
    return R().construct_projected_point_3_object()(*this, p);
  }

  Point_3 point() const
  {
    return R().construct_point_on_3_object()(*this);
  }

  Vector_3 base1() const
  {
    return R().construct_base_vector_3_object()(*this, 1);
  }

  Vector_3 base2() const
  {
    return R().construct_base_vector_3_object()(*this, 2);
  }

  Vector_3 orthogonal_vector() const
  {
    return R().construct_orthogonal_vector_3_object()(*this);
  }

  Point_2 to_2d(const Point_3 &p) const
  {
    return rep().to_2d(p);
  }

  Point_3 to_3d(const Point_2 &p) const
  {
    return R().construct_lifted_point_3_object()(*this, p);
  }


  //Point_3      projection(const Point_3 &p) const;
  //Direction_3  orthogonal_direction() const;

  Oriented_side oriented_side(const Point_3 &p) const
  {
    return R().oriented_side_3_object()(*this, p);
  }

  bool has_on_positive_side(const Point_3 &p) const
  {
    return R().has_on_positive_side_3_object()(*this, p);
  }

  bool has_on_negative_side(const Point_3 &p) const
  {
    return R().has_on_negative_side_3_object()(*this, p);
  }

/*
  bool         has_on(const Point_3 &p) const
  {
    return oriented_side(p) == ON_ORIENTED_BOUNDARY;
  }
  bool         has_on(const Line_3 &l) const
  {
    return has_on(l.point())
       &&  has_on(l.point() + l.direction().to_vector());
  }
*/

  bool is_degenerate() const
  {
    return R().is_degenerate_3_object()(*this);
  }

};

#ifndef CGAL_NO_OSTREAM_INSERT_PLANE_3
template < class R >
std::ostream&
operator<<(std::ostream& os, const Plane_3<R>& p)
{
  typedef typename  R::Kernel_base::Plane_3  RPlane_3;
  return os << static_cast<const RPlane_3&>(p);
}
#endif // CGAL_NO_OSTREAM_INSERT_PLANE_3

#ifndef CGAL_NO_ISTREAM_EXTRACT_PLANE_3
template < class R >
std::istream&
operator>>(std::istream& is, Plane_3<R>& t)
{
  typedef typename  R::Kernel_base::Plane_3  RPlane_3;
  return is >> static_cast<RPlane_3&>(t);
}
#endif // CGAL_NO_ISTREAM_EXTRACT_PLANE_3

CGAL_END_NAMESPACE

#endif // CGAL_PLANE_3_H
