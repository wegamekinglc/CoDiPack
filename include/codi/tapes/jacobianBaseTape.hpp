/*
 * CoDiPack, a Code Differentiation Package
 *
 * Copyright (C) 2015-2022 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (codi@scicomp.uni-kl.de)
 *
 * Lead developers: Max Sagebaum, Johannes Blühdorn (SciComp, TU Kaiserslautern)
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
 *  - SciComp, TU Kaiserslautern:
 *    - Max Sagebaum
 *    - Johannes Blühdorn
 *    - Former members:
 *      - Tim Albring
 */
#pragma once

#include <vector>

#include "../misc/macros.hpp"
#include "../misc/memberStore.hpp"
#include "../config.h"
#include "misc/adjointVectorAccess.hpp"
#include "misc/localAdjoints.hpp"
#include "commonJacobianTapeImplementation.hpp"
#include "indices/indexManagerInterface.hpp"

/** \copydoc codi::Namespace */
namespace codi {

  /**
   * @brief Base class for standard Jacobian tape implementations.
   *
   * This class provides nearly a full implementation of the FullTapeInterface. There are just a few internal methods
   * left which have to be implemented by the final classes. These methods depend significantly on the index management
   * scheme and are performance critical.
   *
   * @tparam T_TapeTypes has to implement JacobianTapeTypes.
   * @tparam T_Impl Type of the final implementation.
   */
  template<typename T_TapeTypes, typename T_Impl>
  struct JacobianBaseTape : public CommonJacobianTapeImplementation<T_TapeTypes, T_Impl> {
    public:

      /// See JacobianBaseTape.
      using TapeTypes = CODI_DD(
          T_TapeTypes, CODI_T(JacobianTapeTypes<double, double, IndexManagerInterface<int>, DefaultChunkedData>));
      /// See JacobianBaseTape.
      using Impl = CODI_DD(T_Impl, CODI_T(FullTapeInterface<double, double, int, EmptyPosition>));

      using Base = CommonJacobianTapeImplementation<TapeTypes, Impl>;  ///< Base class abbreviation.
      friend Base;  ///< Allow the base class to call protected and private methods.

      using Real = typename TapeTypes::Real;                  ///< See TapeTypesInterface.
      using Gradient = typename TapeTypes::Gradient;          ///< See TapeTypesInterface.
      using Identifier = typename TapeTypes::Identifier;      ///< See TapeTypesInterface.

      using Position = typename Base::Position;                ///< See TapeTypesInterface.

      template<typename Adjoint>
      using VectorAccess =
          AdjointVectorAccess<Real, Identifier, Adjoint>;  ///< Vector access type generated by this tape.

    protected:

      LocalAdjoints<Gradient, Identifier, Impl> adjoints;  ///< Evaluation vector for AD.

    private:

      CODI_INLINE Impl const& cast() const {
        return static_cast<Impl const&>(*this);
      }

      CODI_INLINE Impl& cast() {
        return static_cast<Impl&>(*this);
      }

    protected:

      /*******************************************************************************/
      /// @name Interface definition
      /// @{

      /// Perform a forward evaluation of the tape. Arguments are from the recursive eval methods of the DataInterface.
      template<typename... Args>
      static void internalEvaluateForward_Step3_EvalStatements(Args&&... args);

      /// Perform a reverse evaluation of the tape. Arguments are from the recursive eval methods of the DataInterface.
      template<typename... Args>
      static void internalEvaluateReverse_Step3_EvalStatements(Args&&... args);

      /// Add statement specific data to the data streams.
      void pushStmtData(Identifier const& index, Config::ArgumentSize const& numberOfArguments);

      /// @}

    public:

      /// Constructor
      JacobianBaseTape()
          : Base(),
            adjoints(1)  // Ensure that adjoint[0] exists, see its use in gradient() const.
      {
        Base::options.insert(TapeParameters::AdjointSize);
      }

      /*******************************************************************************/
      /// @name Functions from GradientAccessTapeInterface
      /// @{

      /// \copydoc codi::GradientAccessTapeInterface::gradient(Identifier const&)
      CODI_INLINE Gradient& gradient(Identifier const& identifier) {
        checkAdjointSize(identifier);

        return adjoints[identifier];
      }

      /// \copydoc codi::GradientAccessTapeInterface::gradient(Identifier const&) const
      CODI_INLINE Gradient const& gradient(Identifier const& identifier) const {
        if (identifier > (Identifier)adjoints.size()) {
          return adjoints[0];
        } else {
          return adjoints[identifier];
        }
      }

      /// @}
      /*******************************************************************************/
      /// @name Functions from ReverseTapeInterface
      /// @{

      /// \copydoc codi::ReverseTapeInterface::clearAdjoints()
      CODI_INLINE void clearAdjoints() {
        adjoints.zeroAll();
      }

      /// @}

    protected:

      /// Adds data about the adjoint vector to the tape values.
      CODI_INLINE void internalAddTapeValues(TapeValues& values) const {
        size_t nAdjoints = this->indexManager.get().getLargestCreatedIndex();
        double memoryAdjoints = static_cast<double>(nAdjoints) * static_cast<double>(sizeof(Gradient));

        values.addSection("Adjoint vector");
        values.addUnsignedLongEntry("Number of adjoints", nAdjoints);
        values.addDoubleEntry("Memory allocated", memoryAdjoints, true, true);

        Base::internalAddTapeValues(values);
      }

    public:

      /*******************************************************************************/
      /// @name Functions from DataManagementTapeInterface
      /// @{

      /// \copydoc codi::DataManagementTapeInterface::swap()
      CODI_INLINE void swap(Impl& other) {
        adjoints.swap(other.adjoints);

        Base::swap(other);
      }

      /// \copydoc codi::DataManagementTapeInterface::deleteAdjointVector()
      void deleteAdjointVector() {
        adjoints.resize(1);
      }

      /// \copydoc codi::DataManagementTapeInterface::getParameter()
      size_t getParameter(TapeParameters parameter) const {
        switch (parameter) {
          case TapeParameters::AdjointSize:
            return adjoints.size();
            break;
          default:
            return Base::getParameter(parameter);
            break;
        }
      }

      /// \copydoc codi::DataManagementTapeInterface::setParameter()
      void setParameter(TapeParameters parameter, size_t value) {
        switch (parameter) {
          case TapeParameters::AdjointSize:
            adjoints.resize(value);
            break;
          default:
            Base::setParameter(parameter, value);
            break;
        }
      }

      /// \copydoc codi::DataManagementTapeInterface::createVectorAccess()
      VectorAccess<Gradient>* createVectorAccess() {
        return createVectorAccessCustomAdjoints(adjoints.data());
      }

      /// \copydoc codi::DataManagementTapeInterface::createVectorAccessCustomAdjoints()
      template<typename Adjoint>
      VectorAccess<Adjoint>* createVectorAccessCustomAdjoints(Adjoint* data) {
        return new VectorAccess<Adjoint>(data);
      }

      /// \copydoc codi::DataManagementTapeInterface::deleteVectorAccess()
      void deleteVectorAccess(VectorAccessInterface<Real, Identifier>* access) {
        delete access;
      }

      /// @}
      /*******************************************************************************/
      /// @name Functions from ForwardEvaluationTapeInterface
      /// @{

      using Base::evaluateForward;

      /// \copydoc codi::ForwardEvaluationTapeInterface::evaluateForward()
      void evaluateForward(Position const& start, Position const& end) {
        checkAdjointSize(this->indexManager.get().getLargestCreatedIndex());

        cast().evaluateForward(start, end, adjoints.data());
      }

      /// @}
      /*******************************************************************************/
      /// @name Functions from PositionalEvaluationTapeInterface
      /// @{

      using Base::evaluate;

      /// \copydoc codi::PositionalEvaluationTapeInterface::evaluate()
      CODI_INLINE void evaluate(Position const& start, Position const& end) {
        checkAdjointSize(this->indexManager.get().getLargestCreatedIndex());

        evaluate(start, end, adjoints.data());
      }

      /// @}

    private:

      CODI_INLINE void checkAdjointSize(Identifier const& identifier) {
        if (identifier >= (Identifier)adjoints.size()) {
          resizeAdjointsVector();
        }
      }

      CODI_NO_INLINE void resizeAdjointsVector() {
        adjoints.resize(this->indexManager.get().getLargestCreatedIndex() + 1);
      }
  };
}
