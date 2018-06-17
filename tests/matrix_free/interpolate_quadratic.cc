// ---------------------------------------------------------------------
//
// Copyright (C) 2018 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------



// this function tests the correctness of the implementation of matrix free
// operations in getting the function values, the function gradients, and the
// function Laplacians on a cartesian mesh (hyper cube). This tests whether
// cartesian meshes are treated correctly. The test case is without any
// constraints

#include <deal.II/fe/fe_dgq.h>
#include <deal.II/fe/fe_q.h>

#include "../tests.h"
#include "interpolate_functions_common.h"


template <int dim>
class CompareFunction : public Function<dim>
{
public:
  virtual double
  value(const Point<dim> &p, const unsigned int) const
  {
    double value = 1.;
    for (unsigned int d = 0; d < dim; ++d)
      for (unsigned int e = 0; e < dim; ++e)
        value +=
          (1.3 + 0.9 * d * (e + 1) - 1.4 * (d + 1) * (e - 1.)) * p[d] * p[e];
    return value;
  }
  virtual Tensor<1, dim>
  gradient(const Point<dim> &p, const unsigned int) const
  {
    Tensor<1, dim> grad;
    for (unsigned int d = 0; d < dim; ++d)
      for (unsigned int e = 0; e < dim; ++e)
        {
          grad[d] +=
            (1.3 + 0.9 * d * (e + 1) - 1.4 * (d + 1) * (e - 1.)) * p[e];
          grad[e] +=
            (1.3 + 0.9 * d * (e + 1) - 1.4 * (d + 1) * (e - 1.)) * p[d];
        }
    return grad;
  }
  virtual SymmetricTensor<2, dim>
  hessian(const Point<dim> &p, const unsigned int) const
  {
    SymmetricTensor<2, dim> hess;
    for (unsigned int d = 0; d < dim; ++d)
      for (unsigned int e = 0; e < dim; ++e)
        hess[d][e] += (1.3 + 0.9 * d * (e + 1) - 1.4 * (d + 1) * (e - 1.)) *
                      (d == e ? 2.0 : 1.0);
    return hess;
  }
};


template <int dim, int fe_degree>
void
test()
{
  if (fe_degree < 2)
    return;

  Triangulation<dim> tria;
  GridGenerator::hyper_cube(tria);
  tria.refine_global(1);

  {
    FE_Q<dim>       fe(fe_degree);
    DoFHandler<dim> dof(tria);
    dof.distribute_dofs(fe);

    AffineConstraints<double> constraints;
    constraints.close();
    do_test<dim, fe_degree, double>(dof, constraints);
  }
  {
    FE_DGQ<dim>     fe(fe_degree);
    DoFHandler<dim> dof(tria);
    dof.distribute_dofs(fe);

    AffineConstraints<double> constraints;
    constraints.close();
    do_test<dim, fe_degree, double>(dof, constraints);
  }
  deallog << "Test without templates on FEEvaluation" << std::endl;
  {
    FE_DGQ<dim>     fe(fe_degree);
    DoFHandler<dim> dof(tria);
    dof.distribute_dofs(fe);

    AffineConstraints<double> constraints;
    constraints.close();
    do_test<dim, -1, double>(dof, constraints);
  }
}
