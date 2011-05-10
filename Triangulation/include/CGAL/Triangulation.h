// Copyright (c) 2009 INRIA Sophia-Antipolis (France),
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
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
// Author(s)    : Samuel Hornus

#ifndef CGAL_TRIANGULATION_H
#define CGAL_TRIANGULATION_H

#include <boost/iterator/filter_iterator.hpp>
#include <CGAL/internal/Triangulation/utilities.h>
#include <CGAL/Triangulation_data_structure.h>
#include <CGAL/Triangulation_full_cell.h>
#include <CGAL/Triangulation_vertex.h>
#include <CGAL/spatial_sort.h>
#include <CGAL/Dimension.h>
#include <CGAL/iterator.h>
#include <CGAL/Default.h>

namespace CGAL {

template <  class TriangulationTraits, class TDS_ = Default >
class Triangulation
{
    typedef typename Ambient_dimension<typename TriangulationTraits::Point_d>::type
                                                    Ambient_dimension_;
    typedef typename Default::Get<TDS_, Triangulation_data_structure
                    <   Ambient_dimension_,
                        Triangulation_vertex<TriangulationTraits>,
                        Triangulation_full_cell<TriangulationTraits> >
                        >::type                     TDS;
    typedef Triangulation<TriangulationTraits, TDS_> Self;

    typedef typename TriangulationTraits::Coaffine_orientation_d
                                                    Coaffine_orientation_d;
    typedef typename TriangulationTraits::Orientation_d
                                                    Orientation_d;

public:

    typedef TriangulationTraits                     Geom_traits;
    typedef TDS                                     Triangulation_ds;

    typedef typename TDS::Vertex                    Vertex;
    typedef typename TDS::Full_cell                 Full_cell;
    typedef typename TDS::Facet                     Facet;
    typedef typename TDS::Face                      Face;

    typedef Ambient_dimension_                      Ambient_dimension;
    typedef typename Geom_traits::Point_d           Point;
    typedef typename Geom_traits::Point_d           Point_d;

    typedef typename TDS::Vertex_handle            Vertex_handle;
    typedef typename TDS::Vertex_iterator          Vertex_iterator;
    typedef typename TDS::Vertex_const_handle      Vertex_const_handle;
    typedef typename TDS::Vertex_const_iterator    Vertex_const_iterator;

    typedef typename TDS::Full_cell_handle           Full_cell_handle;
    typedef typename TDS::Full_cell_iterator         Full_cell_iterator;
    typedef typename TDS::Full_cell_const_handle     Full_cell_const_handle;
    typedef typename TDS::Full_cell_const_iterator   Full_cell_const_iterator;
    
    typedef typename TDS::Facet_iterator           Facet_iterator;

    typedef typename TDS::size_type                size_type;
    typedef typename TDS::difference_type          difference_type;

    /// The type of location a new point is found lying on
    enum  Locate_type
    {
          ON_VERTEX  = 0
        , IN_FACE    = 1 // simplex of dimension in [1, |current_dimension()| - 2]
        , IN_FACET   = 2 // simplex of dimension |current_dimension()| - 1
        , IN_SIMPLEX = 3
        , OUTSIDE_CONVEX_HULL = 4
        , OUTSIDE_AFFINE_HULL = 5
    };

    // Finite elements iterators

    class Finiteness_predicate;

    typedef boost::filter_iterator<Finiteness_predicate, Vertex_iterator>
        Finite_vertex_iterator;
    typedef boost::filter_iterator<Finiteness_predicate, Vertex_const_iterator>
        Finite_vertex_const_iterator;
    typedef boost::filter_iterator<Finiteness_predicate, Full_cell_iterator>
        Finite_full_cell_iterator;
    typedef boost::filter_iterator<Finiteness_predicate, Full_cell_const_iterator>
        Finite_full_cell_const_iterator;
    typedef boost::filter_iterator<Finiteness_predicate, Facet_iterator>
        Finite_facet_iterator;
    
protected: // DATA MEMBERS

    Triangulation_ds                     tds_;
    const Geom_traits                   kernel_;
    Vertex_handle                       infinity_;
    mutable std::vector<Oriented_side>  orientations_;
    Coaffine_orientation_d              coaffine_orientation_;
    // for stochastic walk in the locate() function:
    mutable Random                      rng_;
#ifdef CGAL_TRIANGULATION_STATISTICS
    mutable unsigned long walk_size_;
#endif

public:

    //       FACETS OPERATIONS

	Face make_empty_face() const
	{
		return tds().make_empty_face();
	}

    // works for Face_ = Facet and Face_ = Rotor.
    // NOT DOCUMENTED for the Rotor case...
    template< typename Face_ >
    Full_cell_handle full_cell(const Face_ & f) const
    {
        return tds().full_cell(f);
    }

    // works for Face_ = Facet and Face_ = Rotor.
    // NOT DOCUMENTED for the Rotor case...
    template< class Face_ >
    int index_of_covertex(const Face_ & f) const
    {
        return tds().index_of_covertex<Face_>(f);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - CREATION

    Triangulation(const int dim, const Geom_traits k = Geom_traits())
        : tds_(dim)
        , kernel_(k)
        , infinity_()
        , rng_((long)0)
#ifdef CGAL_TRIANGULATION_STATISTICS
        ,walk_size_(0)
#endif
    {
        clear();
    }

    ~Triangulation() {}

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ACCESS FUNCTIONS

    // NOT DOCUMENTED -- FIXME TRY TO REMOVE IF POSSIBLE (CF Delaunay_triangulation::remove)
    bool get_visited(Full_cell_handle s) const
    {
        return tds().get_visited(s);
    }
    // NOT DOCUMENTED -- FIXME TRY TO REMOVE IF POSSIBLE (CF Delaunay_triangulation::remove)
    bool get_visited(Full_cell_const_handle s) const
    {
        return tds().get_visited(s);
    }

    // NOT DOCUMENTED -- FIXME TRY TO REMOVE IF POSSIBLE (CF Delaunay_triangulation::remove)
    void set_visited(Full_cell_handle s, bool b) const
    {
        tds().set_visited(s, b);
    }

    Coaffine_orientation_d & coaffine_orientation_predicate()
    {
        return coaffine_orientation_;
    }

    const Coaffine_orientation_d & coaffine_orientation_predicate() const
    {
        return coaffine_orientation_;
    }

    const Triangulation_ds & tds() const
    {
        return tds_;
    }

    Triangulation_ds & tds()
    {
        return tds_;
    }

    const Geom_traits & geom_traits() const
    {
        return kernel_;
    }

    int ambient_dimension() const { return tds().ambient_dimension(); }
    int current_dimension() const { return tds().current_dimension(); }

    bool empty() const
    {
        return current_dimension() == -1;
    }

    size_type number_of_vertices() const
    {
        return tds().number_of_vertices() - 1;
    }

    size_type number_of_full_cells() const
    {
        return tds().number_of_full_cells();
    }

    Vertex_handle infinite_vertex() const
    {
        return infinity_;
    }

    Full_cell_handle infinite_full_cell() const
    {
        CGAL_assertion(infinite_vertex()->full_cell()->has_vertex(infinite_vertex()));
        return infinite_vertex()->full_cell();
    }

// - - - - - - - - - - - - - - - - - - - - - - - - - NON CONSTANT-TIME ACCESS FUNCTIONS

    size_type number_of_finite_full_cells() const
    {
        Full_cell_const_iterator s = full_cells_begin();
        size_type result = number_of_full_cells();
        for( ; s != full_cells_end(); ++s )
        {
            if( is_infinite(s) )
                --result;
        }
        return result;
    }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TRAVERSAL

    Vertex_iterator vertices_begin() { return tds().vertices_begin(); }
    Vertex_iterator vertices_end()   { return tds().vertices_end(); }

    Vertex_const_iterator vertices_begin() const { return tds().vertices_begin(); }
    Vertex_const_iterator vertices_end()   const { return tds().vertices_end(); }

    Finite_vertex_iterator finite_vertices_begin()
    { return Finite_vertex_iterator(Finiteness_predicate(*this), vertices_begin(), vertices_end()); }
    Finite_vertex_iterator finite_vertices_end()
    { return Finite_vertex_iterator(Finiteness_predicate(*this), vertices_end(), vertices_end()); }
    Finite_vertex_const_iterator finite_vertices_begin() const
    { return Finite_vertex_const_iterator(Finiteness_predicate(*this), vertices_begin(), vertices_end()); }
    Finite_vertex_const_iterator finite_vertices_end() const
    { return Finite_vertex_const_iterator(Finiteness_predicate(*this), vertices_end(), vertices_end()); }

    Full_cell_iterator full_cells_begin() { return tds().full_cells_begin(); }
    Full_cell_iterator full_cells_end()   { return tds().full_cells_end(); }

    Full_cell_const_iterator full_cells_begin() const { return tds().full_cells_begin(); }
    Full_cell_const_iterator full_cells_end()   const { return tds().full_cells_end(); }

    Finite_full_cell_iterator finite_full_cells_begin()
    { return Finite_full_cell_iterator(Finiteness_predicate(*this), full_cells_begin(), full_cells_end()); }
    Finite_full_cell_iterator finite_full_cells_end()
    { return Finite_full_cell_iterator(Finiteness_predicate(*this), full_cells_end(), full_cells_end()); }
    Finite_full_cell_const_iterator finite_full_cells_begin() const
    { return Finite_full_cell_const_iterator(Finiteness_predicate(*this), full_cells_begin(), full_cells_end()); }
    Finite_full_cell_const_iterator finite_full_cells_end() const
    { return Finite_full_cell_const_iterator(Finiteness_predicate(*this), full_cells_end(), full_cells_end()); }

    Facet_iterator facets_begin() { return tds().facets_begin(); }
    Facet_iterator facets_end() { return tds().facets_end(); }
    Facet_iterator finite_facets_begin()
    { return Finite_facet_iterator(Finiteness_predicate(*this), facets_begin(), facets_end()); }
    Facet_iterator finite_facets_end()
    { return Finite_facet_iterator(Finiteness_predicate(*this), facets_end(), facets_end()); }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - SOME PREDICATE FUNCTORS

    class Finiteness_predicate
    {
        const Self & t_;
    public:
        Finiteness_predicate(const Self & t) : t_(t) {}
        template < class T >
        bool operator()(const T & t)
        {
            return ( t_.is_finite(t.vertex(0)) );
        }
    };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - SIMPLE QUERIES

    bool is_vertex(const Point & p, Vertex_handle & v, Full_cell_handle hint = Full_cell_handle()) const
    {
        Locate_type lt;
        Face f(ambient_dimension());
        Facet ft;
        Full_cell_handle s = locate(p, lt, f, ft, hint);
        if( ON_VERTEX == lt )
        {
            v = s->vertex(f.index(0));
            return true;
        }
        return false;
    }

    bool is_vertex(Vertex_const_handle v) const
    {
        return tds().is_vertex(v);
    }

    bool is_full_cell(Full_cell_const_handle s) const
    {
        return tds().is_full_cell(s);
    }

    bool is_infinite(Vertex_const_handle v) const
    {
        CGAL_precondition(Vertex_const_handle() != v);
        return (infinite_vertex() == v);
    }

    bool is_infinite(Full_cell_const_handle s) const
    {
        CGAL_precondition(Full_cell_const_handle() != s);
        return is_infinite(s->vertex(0));
    }

    bool is_infinite(const Facet & ft) const
    {
        Full_cell_const_handle s = full_cell(ft);
        CGAL_precondition(s != Full_cell_handle());
        if( is_infinite(s) )
            return (index_of_covertex(ft) != 0);
        return false;
    }

    bool is_infinite(const Face & f) const
    {
        Full_cell_const_handle s = f.full_cell();
        CGAL_precondition(s != Full_cell_handle());
        if( is_infinite(s) )
        {
            int i(0);
            Vertex_handle v;
            while( v = f.vertex(i), Vertex_handle() != v )
            {
                if( is_infinite(v) )
                    return true;
                ++i;
            }
            return false;
        }
        return false;
    }

    bool is_finite(Vertex_const_handle v) const
    {
        return (! is_infinite(v));
    }

    bool is_finite(Full_cell_const_handle s) const
    {
        return (! is_infinite(s));
    }

    bool is_finite(const Facet & ft) const
    {
        return (! is_infinite(ft));
    }

    bool is_finite(const Face & f) const
    {
        return (! is_infinite(f));
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ELEMENT GATHERING

    template< typename OutputIterator >
    OutputIterator incident_full_cells(const Face & f, OutputIterator out) const
    {
        return tds().incident_full_cells(f, out);
    }
    template< typename OutputIterator >
    OutputIterator incident_full_cells(Vertex_const_handle v, OutputIterator out) const
    {
        return tds().incident_full_cells(v, out);
    }
    template< typename OutputIterator >
    OutputIterator compute_star(const Face & f, OutputIterator out) const
    {
        return tds().compute_star(f, out);
    }

    template< typename OutputIterator >
    OutputIterator incident_faces(Vertex_const_handle v, const int d, OutputIterator out)
    {
        return tds().incident_faces(v, d, out);
    }

    template< typename OutputIterator, class Comparator >
    OutputIterator incident_upper_faces( Vertex_const_handle v, const int d,
                                                OutputIterator out, Comparator cmp = Comparator())
    {
        return tds().incident_upper_faces(v, d, out, cmp);
    }
    template< typename OutputIterator >
    OutputIterator incident_upper_faces( Vertex_const_handle v, const int d,
                                                OutputIterator out)
    {
        internal::Triangulation::Compare_vertices_for_upper_face<Self> cmp(*this);
        return tds().incident_upper_faces(v, d, out, cmp);
    }

    Orientation orientation(Full_cell_const_handle s, bool in_is_valid = false) const
    {
        if( ! in_is_valid )
            CGAL_assertion( is_finite(s) );
        if( 0 == current_dimension() )
            return POSITIVE;
        if( current_dimension() == ambient_dimension() )
        {
            Orientation_d ori = geom_traits().orientation_d_object();
            return ori(s->points_begin(), s->points_begin() + 1 + current_dimension());
        }
        else
        {
            return coaffine_orientation_predicate()(s->points_begin(), s->points_begin() + 1 + current_dimension());
        }
    }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - UPDATE OPERATIONS

    void clear()
    {
        tds_.clear();
        infinity_ = tds().insert_increase_dimension();
        // A full_cell has at most 1 + ambient_dimension() facets:
        orientations_.resize(1 + ambient_dimension());
        // Our coaffine orientation predicates HAS state member variables
        coaffine_orientation_predicate() = geom_traits().coaffine_orientation_d_object();
#ifdef CGAL_TRIANGULATION_STATISTICS
        walk_size_ = 0;
#endif
    }

    void set_current_dimension(const int d)
    {
        tds().set_current_dimension(d);
    }

    Full_cell_handle new_full_cell()
    { 
        return tds().new_full_cell();
    }

    Vertex_handle  new_vertex(const Point & p) 
    {
        return tds().new_vertex(p);
    }

    void set_neighbors(Full_cell_handle s, int i, Full_cell_handle s1, int j)
    {
        tds().set_neighbors(s, i, s1, j);
    }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - VALIDITY

    bool is_valid(bool = true, int = 0) const;
    bool are_incident_full_cells_valid(Vertex_const_handle, bool = true, int = 0) const;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - POINT LOCATION

protected:
    template< typename OrientationPredicate >
    Full_cell_handle do_locate(   const Point &, Locate_type &, Face &, Facet &,
                                Full_cell_handle start = Full_cell_handle(),
                                OrientationPredicate & o = geom_traits().orientation_d_object()) const;
public:
    Full_cell_handle locate(  const Point &, Locate_type &, Face &, Facet &,
                            Full_cell_handle start = Full_cell_handle()) const;
    Full_cell_handle locate(  const Point &, Locate_type &, Face &, Facet &,
                            Vertex_handle) const;
    Full_cell_handle locate(const Point & p, Full_cell_handle s = Full_cell_handle()) const;
    Full_cell_handle locate(const Point & p, Vertex_handle v) const;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - REMOVALS

    Vertex_handle contract_face(const Point &, const Face &);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - POINT INSERTION

    template< typename ForwardIterator >
    size_type insert(ForwardIterator start, ForwardIterator end)
    {
        size_type n = number_of_vertices();
        std::vector<Point> points(start, end);
        std::random_shuffle(points.begin(), points.end());
        spatial_sort(points.begin(), points.end(), geom_traits());
        Full_cell_handle hint = Full_cell_handle();
        typename std::vector<Point>::const_iterator s = points.begin();
        while( s != points.end() )
        {
            hint = insert(*s++, hint)->full_cell();
        }
        return number_of_vertices() - n;
    }
    Vertex_handle insert(const Point &, const Locate_type, const Face &, const Facet &, const Full_cell_handle);
    Vertex_handle insert(const Point &, Full_cell_handle start = Full_cell_handle());
    Vertex_handle insert(const Point &, Vertex_handle);
    template< typename ForwardIterator >
    Vertex_handle insert_in_hole(const Point & p, ForwardIterator start, ForwardIterator end, const Facet & ft)
    {
        Emptyset_iterator out;
        return insert_in_hole(p, start, end, ft, out);
    }
    template< typename ForwardIterator, typename OutputIterator >
    Vertex_handle insert_in_hole(const Point & p, ForwardIterator start, ForwardIterator end, const Facet & ft, 
                                 OutputIterator out)
    {
        Vertex_handle v = tds().insert_in_hole(start, end, ft, out);
        v->set_point(p);
        return v;
    }
    Vertex_handle insert_in_face(const Point &, const Face &);
    Vertex_handle insert_in_facet(const Point &, const Facet &);
    Vertex_handle insert_in_full_cell(const Point &, Full_cell_handle);
    Vertex_handle insert_outside_convex_hull_1(const Point &, Full_cell_handle);
    Vertex_handle insert_outside_convex_hull(const Point &, Full_cell_handle);
    Vertex_handle insert_outside_affine_hull(const Point &);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - FACET-TRAVERSAL PREDICATES

    template< typename OrientationPredicate >
    class Outside_convex_hull_traversal_predicate
    {
        Triangulation & t_;
        const Point & p_;
        OrientationPredicate & ori_;
        int cur_dim_;
    public:
        Outside_convex_hull_traversal_predicate(Triangulation & t, const Point & p,
                                    OrientationPredicate & ori)
        : t_(t), p_(p), ori_(ori), cur_dim_(t.current_dimension()) {}
        // FUTURE change parameter to const reference
        bool operator()(Facet f) const
        {
            Full_cell_handle s = t_.full_cell(f);
            const int i = t_.index_of_covertex(f);
            Full_cell_handle n = s->neighbor(i);
            if( t_.is_finite(n) )
                return false;
            n->vertex(0)->set_point(p_);
            bool ok = (POSITIVE == ori_(n->points_begin(), n->points_begin() + cur_dim_ + 1));
            return ok;
        }
    };
    // make sure all full_cells have positive orientation
    void reorient_full_cells();

}; // Triangulation<...>

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 

// CLASS MEMBER FUNCTIONS

template < class TT, class TDS >
void
Triangulation<TT, TDS>
::reorient_full_cells()
{
    if( current_dimension() < 1 )
        return;
    Full_cell_iterator sit = full_cells_begin();
    Full_cell_iterator send = full_cells_end();
    while( sit != send )
    {
        if( is_infinite(sit) && (1 == current_dimension()) )
        {
            ++sit;
            continue;
        }
        sit->swap_vertices(current_dimension() - 1, current_dimension());
        ++sit;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - THE REMOVAL METHODS

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::contract_face(const Point & p, const Face & f)
{
    CGAL_precondition( is_finite(f) );
    Vertex_handle v = tds().contract_face(f);
    v->set_point(p);
    CGAL_expensive_postcondition_msg(are_incident_full_cells_valid(v), "new point is not where it should be");
    return v;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - THE INSERTION METHODS

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::insert(const Point & p, const Locate_type lt, const Face & f, const Facet & ft, const Full_cell_handle s)
{
    switch( lt )
    {
        case IN_SIMPLEX:
            //std::cerr << " IS";
            return insert_in_full_cell(p, s);
            break;
        case OUTSIDE_CONVEX_HULL:
            //std::cerr << " OCH";
            return insert_outside_convex_hull(p, s);
            break;
        case OUTSIDE_AFFINE_HULL:
            //std::cerr << " OAF";
            return insert_outside_affine_hull(p);
            break;
        case IN_FACET:
        {
            //std::cerr << " IFT";
            return insert_in_facet(p, ft);
            break;
        }
        case IN_FACE:
            //std::cerr << " IF" << f.feature_dimension();
            return insert_in_face(p, f);
            break;
        case ON_VERTEX:
            //std::cerr << " OV";
            s->vertex(f.index(0))->set_point(p);
            return s->vertex(f.index(0));
            break;
    }
    CGAL_assertion(false);
    return Vertex_handle();
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::insert(const Point & p, Full_cell_handle start)
{
    Locate_type lt;
    Face f(ambient_dimension());
    Facet ft;
    Full_cell_handle s = locate(p, lt, f, ft, start);
    return insert(p, lt, f, ft, s);
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::insert(const Point & p, Vertex_handle v)
{
    if( Vertex_handle() == v )
        v = infinite_vertex();
    return insert(p, v->full_cell());
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::insert_in_face(const Point & p, const Face & f)
{
    CGAL_precondition( ! is_infinite(f) );
    Vertex_handle v = tds().insert_in_face(f);
    v->set_point(p);
    return v;
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::insert_in_facet(const Point & p, const Facet & ft)
{
    CGAL_precondition( ! is_infinite(ft) );
    Vertex_handle v = tds().insert_in_facet(ft);
    v->set_point(p);
    return v;
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::insert_in_full_cell(const Point & p, Full_cell_handle s)
{
    CGAL_precondition( is_finite(s) );
    Vertex_handle v = tds().insert_in_full_cell(s);
    v->set_point(p);
    return v;
}

// NOT DOCUMENTED...
template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::insert_outside_convex_hull_1(const Point & p, Full_cell_handle s)
{
    // This is a special case for dimension 1, because in that case, the right
    // infinite full_cell is not correctly oriented... (sice its first vertex is the
    // infinite one...
    CGAL_precondition( is_infinite(s) );
    CGAL_precondition( 1 == current_dimension() );
    bool swap = (0 == s->neighbor(0)->index(s));
    Vertex_handle v = tds().insert_in_full_cell(s);
    v->set_point(p);
    if( swap )
    {
        s->swap_vertices(0, 1);
    }
    return v;
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::insert_outside_convex_hull(const Point & p, Full_cell_handle s)
{
    if( 1 == current_dimension() )
    {
        return insert_outside_convex_hull_1(p, s);
    }
    CGAL_precondition( is_infinite(s) );
    CGAL_assertion( current_dimension() >= 2 );
    std::vector<Full_cell_handle> simps;
    simps.reserve(64);
    std::back_insert_iterator<std::vector<Full_cell_handle> > out(simps);
    if( current_dimension() < ambient_dimension() )
    {
        Outside_convex_hull_traversal_predicate<Coaffine_orientation_d>
            ochtp(*this, p, coaffine_orientation_predicate());
        tds().gather_full_cells(s, ochtp, out);
    }
    else
    {
        Orientation_d ori = geom_traits().orientation_d_object();
        Outside_convex_hull_traversal_predicate<Orientation_d>
            ochtp(*this, p, ori);
        tds().gather_full_cells(s, ochtp, out);
    }
    Vertex_handle v = insert_in_hole(p, simps.begin(), simps.end(), Facet(s, 0));
    return v;
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Vertex_handle
Triangulation<TT, TDS>
::insert_outside_affine_hull(const Point & p)
{
    CGAL_precondition( current_dimension() < ambient_dimension() );
    Vertex_handle v = tds().insert_increase_dimension(infinite_vertex());
    // reset the orientation predicate:
    coaffine_orientation_predicate() = geom_traits().coaffine_orientation_d_object();
    v->set_point(p);
    if( current_dimension() >= 1 )
    {
        Full_cell_handle s = infinite_vertex()->full_cell()->neighbor(0);
        Orientation o = orientation(s);
        CGAL_assertion( COPLANAR != o );
            if( NEGATIVE == o )
                reorient_full_cells();
    }
    return v;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - THE MAIN LOCATE(...) FUNCTION

template < class TT, class TDS >
template< typename OrientationPredicate >
typename Triangulation<TT, TDS>::Full_cell_handle
Triangulation<TT, TDS>
::do_locate(   const Point & p, // query point
            Locate_type & loc_type,// type of result (full_cell, face, vertex)
            Face & face,// the face containing the query in its interior (when appropriate)
            Facet & facet,// the facet containing the query in its interior (when appropriate)
            const Full_cell_handle start// starting full_cell for the walk
            , OrientationPredicate & orientation_pred
        ) const
{
    const int cur_dim = current_dimension();

    if( cur_dim == -1 )
    {
        loc_type = OUTSIDE_AFFINE_HULL;
        return Full_cell_handle();
    }
    else if( cur_dim == 0 )
    {
        Vertex_handle vit = infinite_full_cell()->neighbor(0)->vertex(0);
        if( EQUAL != geom_traits().compare_lexicographically_d_object()(p, vit->point()) )
        {
            loc_type = OUTSIDE_AFFINE_HULL;
        }
        else
        {
            loc_type = ON_VERTEX;
            face.set_full_cell(vit->full_cell());
            face.set_index(0, 0);
        }
        return vit->full_cell();
    }

    Full_cell_handle s;

    // if we don't know where to start, we start from any bounded full_cell
    if( Full_cell_handle() == start )
        // THE HACK THAT NOBODY SHOULD DO... BUT DIFFICULT TO WORK AROUND
        // THIS... TODO: WORK AROUND IT
        s = const_cast<Self*>(this)->infinite_full_cell()->neighbor(0);
    else
    {
        s = start;
        if( is_infinite(s) )
            s = s->neighbor(0);
    }

    // Check if query |p| is outside the affine hull
    if( cur_dim < ambient_dimension() )
    {
        if( ! geom_traits().contained_in_affine_hull_d_object()(
            s->points_begin(),
            s->points_begin() + current_dimension() + 1,
            p) )
        {
            loc_type = OUTSIDE_AFFINE_HULL;
            return Full_cell_handle();
        }
    }

    // we remember the |previous|ly visited full_cell to avoid the evaluation
    // of one |orientation| predicate
    Full_cell_handle previous = Full_cell_handle();
    bool full_cell_not_found = true;
    while(full_cell_not_found) // we walk until we locate the query point |p|
    {
    #ifdef CGAL_TRIANGULATION_STATISTICS
        ++walk_size_;
    #endif
        // For the remembering stochastic walk, we need to start trying
        // with a random index:
        int j, i = rng_.get_int(0, cur_dim);
        // we check |p| against all the full_cell's hyperplanes in turn
       
        for(j = 0; j <= cur_dim; ++j, i = (i + 1) % (cur_dim + 1) )
        {
            Full_cell_handle next = s->neighbor(i);
            if( previous == next )
            {   // no need to compute the orientation, we already know it
                orientations_[i] = POSITIVE;
                continue; // go to next full_cell's facet
            }

            // we temporarily substitute |p| to the |i|-th point of the
            // full_cell
            Point backup = s->vertex(i)->point();
            s->vertex(i)->set_point(p);

            orientations_[i] = orientation_pred(
                s->points_begin(),
                s->points_begin() + cur_dim + 1);

            // restore the correct point for vertex |i| of the full_cell
            s->vertex(i)->set_point(backup);

            if( orientations_[i] != NEGATIVE )
            {
                // from this facet's point of view, we are inside the
                // full_cell or on its boundary, so we continue to next facet
                continue;
            }

            // At this point, we know that we have to jump to the |next|
            // full_cell because orientation_[i] == NEGATIVE
            previous = s;
            s = next;
            if( is_infinite(next) )
            {   // we have arrived OUTSIDE the convex hull of the triangulation,
                // so we stop the search
                full_cell_not_found = false;
                loc_type = OUTSIDE_CONVEX_HULL;
                face.set_full_cell(s);
            }
            break;
        } // end of the 'for' loop
        if( ( cur_dim + 1 ) == j ) // we found the full_cell containing |p|
            full_cell_not_found = false;
    }
    // Here, we know in which full_cell |p| is in.
    // We now check more precisely where |p| landed:
    // vertex, facet, face or full_cell.
    if( ! is_infinite(s) )
    {
        face.set_full_cell(s);
        int num(0);
        int verts(0);
        for(int i = 0; i <= cur_dim; ++i)
        {
            if( orientations_[i] == COPLANAR )
            {
                ++num;
                facet = Facet(s, i);
            }
            else
                face.set_index(verts++, i);
        }
        if( 0 == num )
        {
            loc_type = IN_SIMPLEX;
            face.clear();
        }
        else if( cur_dim == num )
            loc_type = ON_VERTEX;
        else if( 1 == num )
            loc_type = IN_FACET;
        else
            loc_type = IN_FACE;
            
#if 0 // if ! defined( CGAL_NDEBUG )
        // informative test that can be removed:
        if( loc_type == IN_FACE || loc_type == IN_FACET )
        {
            std::cerr << "\n[Degenerate input : loc_type = " << loc_type << ':';
            for(int i = 0; i <= cur_dim; ++i)
                std::cerr << ' ' << orientations_[i];
            std::cerr << ']';
        }
#endif
    }
    return s;
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Full_cell_handle
Triangulation<TT, TDS>
::locate(   const Point & p, // query point
            Locate_type & loc_type,// type of result (full_cell, face, vertex)
            Face & face,// the face containing the query in its interior (when appropriate)
            Facet & facet,// the facet containing the query in its interior (when appropriate)
            Full_cell_handle start// starting full_cell for the walk
        ) const
{
    if( current_dimension() == ambient_dimension() )
    {
        Orientation_d ori = geom_traits().orientation_d_object();
        return do_locate(p, loc_type, face, facet, start, ori);
    }
    else
        return do_locate(p, loc_type, face, facet, start, coaffine_orientation_predicate());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - the locate(...) variants

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Full_cell_handle
Triangulation<TT, TDS>
::locate(   const Point & p,
            Locate_type & loc_type,
            Face & face,
            Facet & facet,
            Vertex_handle start) const
{
    if( Vertex_handle() == start )
        start = infinite_vertex();
    return locate(p, loc_type, face, facet, start->full_cell());
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Full_cell_handle
Triangulation<TT, TDS>
::locate(const Point & p, Full_cell_handle s) const
{
    Locate_type lt;
    Face face(ambient_dimension());
    Facet facet;
    return locate(p, lt, face, facet, s);
}

template < class TT, class TDS >
typename Triangulation<TT, TDS>::Full_cell_handle
Triangulation<TT, TDS>
::locate(const Point & p, Vertex_handle v) const
{
    if( Vertex_handle() != v )
        v = infinite_vertex();
    return this->locate(p, v->full_cell());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - VALIDITY

template < class TT, class TDS >
bool
Triangulation<TT, TDS>
::is_valid(bool verbose, int level) const
{ 
    if( ! tds().is_valid(verbose, level) )
        return false;

    Full_cell_const_iterator s;
    for( s = full_cells_begin(); s != full_cells_end(); ++s )
    {
        if( s->has_vertex(infinite_vertex()) )
        {
            if( s->vertex(0) != infinite_vertex() )
            {
                if( verbose ) CGAL_warning_msg(false, "the vertex at infinity must have index 0");
                return false;
            }
        }
    }
    if( current_dimension() < 0 )
        return true;
    Orientation o;
    for( s = full_cells_begin(); s != full_cells_end(); ++s )
    {
        if( is_infinite(s) )
        {
            if( current_dimension() > 1 )
            {
                Full_cell_handle fs = s->neighbor(0);
                infinite_vertex()->set_point(fs->vertex(s->mirror_index(0))->point());
                o = - orientation(s, true);
            }
            else
                o = POSITIVE;
        }
        else
            o = orientation(s, true);
        if( NEGATIVE == o )
        {
            if( verbose ) CGAL_warning_msg(false, "full_cell is not correctly oriented");
            return false;
        }
        if( COPLANAR == o )
        {
            if( verbose ) CGAL_warning_msg(false, "full_cell is flat");
            return false;
        }
    }
    return true;
}

template < class TT, class TDS >
bool Triangulation<TT, TDS>::are_incident_full_cells_valid(Vertex_const_handle v, bool verbose, int level) const
{
    if( current_dimension() <= 0 )
        return true;
    typedef std::vector<Full_cell_const_handle> Simps;
    Simps simps;
    simps.reserve(64);
    std::back_insert_iterator<Simps> out(simps);
    incident_full_cells(v, out);
    typename Simps::const_iterator sit = simps.begin();
    for( ; sit != simps.end(); ++sit )
    {
        if( is_infinite(*sit) )
            continue;
        Orientation o = orientation(*sit);
        if( NEGATIVE == o )
        {
            if( verbose ) CGAL_warning_msg(false, "full_cell is not correctly oriented");
            return false;
        }
        if( COPLANAR == o )
        {
            if( verbose ) CGAL_warning_msg(false, "full_cell is flat");
            return false;
        }
    }
    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// FUNCTIONS THAT ARE NOT MEMBER FUNCTIONS:

template < class TT, class TDS >
std::istream & 
operator>>(std::istream & is, Triangulation<TT, TDS> & tr)
  // reads :
  // - the dimensions (ambient and current)
  // - the number of finite vertices
  // - the non combinatorial information on vertices (point, etc)
  // - the number of full_cells
  // - the full_cells by the indices of their vertices in the preceding list
  // of vertices, plus the non combinatorial information on each full_cell
  // - the neighbors of each full_cell by their index in the preceding list
{
    typedef Triangulation<TT, TDS> T;
    typedef typename T::Vertex_handle         Vertex_handle;
    typedef typename T::Vertex_iterator       Vertex_iterator;
    typedef typename T::Full_cell_handle        Full_cell_handle;
    typedef typename T::Full_cell_iterator      Full_cell_iterator;

    // read current dimension and number of vertices
    size_t n;
    int cd;
    if( is_ascii(is) )
        is >> cd >> n;
    else
    {
        read(is, cd);
        read(is, n, io_Read_write());
    }

    CGAL_assertion_msg( cd <= tr.ambient_dimension(), "input Triangulation has too high dimension");

    tr.clear();
    tr.set_current_dimension(cd);

    if( n == 0 )
        return is;

    std::vector<Vertex_handle> vertices;
    vertices.resize(n+1);
    vertices[0] = tr.infinite_vertex();
    is >> (*vertices[0]);

    // read the vertices:
    size_t i(1);
    while( i <= n )
    {
        vertices[i] = tr.new_vertex();
        is >> (*vertices[i]); // read a vertex
        ++i;
    }

    // now, read the combinatorial information
   return tr.tds().read_full_cells(is, vertices);
}

template < class TT, class TDS >
std::ostream & 
operator<<(std::ostream & os, const Triangulation<TT, TDS> & tr)
  // writes :
  // - the dimensions (ambient and current)
  // - the number of finite vertices
  // - the non combinatorial information on vertices (point, etc)
  // - the number of full_cells
  // - the full_cells by the indices of their vertices in the preceding list
  // of vertices, plus the non combinatorial information on each full_cell
  // - the neighbors of each full_cell by their index in the preceding list
{
    typedef Triangulation<TT, TDS> T;
    typedef typename T::Vertex_const_handle         Vertex_handle;
    typedef typename T::Vertex_const_iterator       Vertex_iterator;
    typedef typename T::Full_cell_const_handle        Full_cell_handle;
    typedef typename T::Full_cell_const_iterator      Full_cell_iterator;

    // outputs dimensions and number of vertices
    size_t n = tr.number_of_vertices();
    if( is_ascii(os) )
        os << tr.current_dimension() << std::endl << n << std::endl;
    else
    {
        write(os, tr.current_dimension());
        write(os, n, io_Read_write());
    }

    if( n == 0 )
        return os;

    size_t i(0);
    // write the vertices
    std::map<Vertex_handle, int> index_of_vertex;

    // infinite vertex has index 0
    index_of_vertex[tr.infinite_vertex()] = i++;
    os << *tr.infinite_vertex();
    for( Vertex_iterator it = tr.vertices_begin(); it != tr.vertices_end(); ++it )
    {
        if( tr.is_infinite(it) )
            continue;
        os << *it; // write the vertex
        index_of_vertex[it] = i++;
    }
    CGAL_assertion( i == n+1 );

    // output the combinatorial information
    return tr.tds().write_full_cells(os, index_of_vertex);
}

} //namespace CGAL

#endif // CGAL_TRIANGULATION_H
