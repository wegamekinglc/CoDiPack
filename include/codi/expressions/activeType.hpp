#pragma once

#include "../aux/macros.hpp"
#include "../config.h"
#include "../tapes/interfaces/fullTapeInterface.hpp"
#include "../traits/realTraits.hpp"
#include "assignmentOperators.hpp"
#include "incrementOperators.hpp"
#include "lhsExpressionInterface.hpp"

/** \copydoc codi::Namespace */
namespace codi {

  template<typename _Tape>
  struct ActiveType : public LhsExpressionInterface<typename _Tape::Real, typename _Tape::Gradient, _Tape, ActiveType<_Tape> >,
                      public AssignmentOperators<_Tape, ActiveType<_Tape>>,
                      public IncrementOperators<_Tape, ActiveType<_Tape>> {
    public:

      using Tape = CODI_DECLARE_DEFAULT(_Tape, CODI_TEMPLATE(FullTapeInterface<double, double, int, EmptyPosition>));

      using StoreAs = ActiveType const&;

      using Real = typename Tape::Real;
      using PassiveReal = PassiveRealType<Real>;
      using Identifier = typename Tape::Identifier;
      using Gradient = typename Tape::Gradient;

      using Base = LhsExpressionInterface<Real, Gradient, Tape, ActiveType>;

    private:

      Real primalValue;
      Identifier identifier;

      static Tape globalTape;

    public:

      CODI_INLINE ActiveType() : primalValue(), identifier() {
        Base::init();
      }

      CODI_INLINE ActiveType(ActiveType<Tape> const& v) : primalValue(), identifier() {
        Base::init();
        this->getGlobalTape().store(*this, v);
      }

      CODI_INLINE ActiveType(PassiveReal const& value) : primalValue(value), identifier() {
        Base::init();
      }

      CODI_INLINE ~ActiveType() {
        Base::destroy();
      }

      template<class Rhs>
      CODI_INLINE ActiveType(ExpressionInterface<Real, Rhs> const& rhs) : primalValue(), identifier() {
        Base::init();
        this->getGlobalTape().store(*this, rhs.cast());
      }

      CODI_INLINE ActiveType<Tape>& operator=(ActiveType<Tape> const& v) {
        static_cast<LhsExpressionInterface<Real, Gradient, Tape, ActiveType>&>(*this) = v;
        return *this;
      }
      using LhsExpressionInterface<Real, Gradient, Tape, ActiveType>::operator=;

      /** TODO: */
      CODI_INLINE Identifier& getIdentifier() {
        return identifier;
      }

      CODI_INLINE Identifier const& getIdentifier() const {
        return identifier;
      }

      CODI_INLINE Real& value() {
        return primalValue;
      }

      CODI_INLINE Real const& value() const {
        return primalValue;
      }

      static CODI_INLINE Tape& getGlobalTape() {
        return globalTape;
      }
  };

  template<typename Tape>
  Tape ActiveType<Tape>::globalTape = Tape();
}
