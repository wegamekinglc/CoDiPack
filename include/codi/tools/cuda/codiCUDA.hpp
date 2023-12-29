/*
 * CoDiPack, a Code Differentiation Package
 *
 * Copyright (C) 2015-2024 Chair for Scientific Computing (SciComp), University of Kaiserslautern-Landau
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (codi@scicomp.uni-kl.de)
 *
 * Lead developers: Max Sagebaum, Johannes Blühdorn (SciComp, University of Kaiserslautern-Landau)
 *
 * This file is part of CoDiPack (http://www.scicomp.uni-kl.de/software/codi).
 *
 * CoDiPack is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * CoDiPack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU
 * General Public License along with CoDiPack.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * For other licensing options please contact us.
 *
 * Authors:
 *  - SciComp, University of Kaiserslautern-Landau:
 *    - Max Sagebaum
 *    - Johannes Blühdorn
 *    - Former members:
 *      - Tim Albring
 */
#pragma once

#include "codiCUDA.hpp"
#include "../../expressions/activeTypeStatelessTape.hpp"
#include "../../tapes/forwardEvaluation.hpp"

/** \copydoc codi::Namespace */
namespace codi {

          /// Forward AD type for CUDA kernels. See \ref sec_forwardAD for a forward mode AD explanation.
  template<typename Real, typename Gradient = Real>
  using RealForwardCUDAGen = ActiveTypeStatelessTape<ForwardEvaluation<Real, Gradient>>;

          /// Forward AD type for CUDA kernels. See \ref sec_forwardAD for a forward mode AD explanation.
  using RealForwardCUDA = RealForwardCUDAGen<double, double>;

          /// Vector forward AD type for CUDA kernels. See \ref sec_forwardAD for a forward mode AD explanation.
  template<size_t dim>
  using RealForwardCUDAVec = RealForwardCUDAGen<double, Direction<double, dim>>;
}
