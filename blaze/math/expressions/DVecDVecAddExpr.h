//=================================================================================================
/*!
//  \file blaze/math/expressions/DVecDVecAddExpr.h
//  \brief Header file for the dense vector/dense vector addition expression
//
//  Copyright (C) 2011 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. This library is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License as published by the
//  Free Software Foundation; either version 3, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with a special
//  exception for linking and compiling against the Blaze library, the so-called "runtime
//  exception"; see the file COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_EXPRESSIONS_DVECDVECADDEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_DVECDVECADDEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <stdexcept>
#include <blaze/math/constraints/DenseVector.h>
#include <blaze/math/constraints/TransposeFlag.h>
#include <blaze/math/expressions/Computation.h>
#include <blaze/math/expressions/DenseVector.h>
#include <blaze/math/expressions/Forward.h>
#include <blaze/math/expressions/VecVecAddExpr.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/traits/AddExprTrait.h>
#include <blaze/math/traits/AddTrait.h>
#include <blaze/math/typetraits/IsComputation.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/IsTemporary.h>
#include <blaze/math/typetraits/RequiresEvaluation.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Double.h>
#include <blaze/util/constraints/Float.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/logging/FunctionTrace.h>
#include <blaze/util/SelectType.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/IsSame.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DVECDVECADDEXPR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for dense vector-dense vector additions.
// \ingroup dense_vector_expression
//
// The DVecDVecAddExpr class represents the compile time expression for additions between
// dense vectors.
*/
template< typename VT1  // Type of the left-hand side dense vector
        , typename VT2  // Type of the right-hand side dense vector
        , bool TF >     // Transpose flag
class DVecDVecAddExpr : public DenseVector< DVecDVecAddExpr<VT1,VT2,TF>, TF >
                      , private VecVecAddExpr
                      , private Computation
{
 private:
   //**Type definitions****************************************************************************
   typedef typename VT1::ResultType     RE1;  //!< Result type of the left-hand side dense vector expression.
   typedef typename VT2::ResultType     RE2;  //!< Result type of the right-hand side dense vector expression.
   typedef typename VT1::ReturnType     RN1;  //!< Return type of the left-hand side dense vector expression.
   typedef typename VT2::ReturnType     RN2;  //!< Return type of the right-hand side dense vector expression.
   typedef typename VT1::CompositeType  CT1;  //!< Composite type of the left-hand side dense vector expression.
   typedef typename VT2::CompositeType  CT2;  //!< Composite type of the right-hand side dense vector expression.
   typedef typename VT1::ElementType    ET1;  //!< Element type of the left-hand side dense vector expression.
   typedef typename VT2::ElementType    ET2;  //!< Element type of the right-hand side dense vector expression.
   //**********************************************************************************************

   //**Return type evaluation**********************************************************************
   //! Compilation switch for the selection of the subscript operator return type.
   /*! The \a returnExpr compile time constant expression is a compilation switch for the
       selection of the \a ReturnType. If either vector operand returns a temporary vector
       or matrix, \a returnExpr will be set to \a false and the subscript operator will
       return it's result by value. Otherwise \a returnExpr will be set to \a true and
       the subscript operator may return it's result as an expression. */
   enum { returnExpr = !IsTemporary<RN1>::value && !IsTemporary<RN2>::value };

   //! Expression return type for the subscript operator.
   typedef typename AddExprTrait<RN1,RN2>::Type  ExprReturnType;
   //**********************************************************************************************

   //**Evaluation strategy*************************************************************************
   //! Compilation switch for the evaluation strategy of the addition expression.
   /*! The \a useAssign compile time constant expression represents a compilation switch for the
       evaluation strategy of the addition expression. In case either of the two dense vector
       operands requires an intermediate evaluation or the subscript operator can only return by
       value, \a useAssign will be set to \a true and the addition expression will be evaluated
       via the \a assign function family. Otherwise \a useAssign will be set to \a false and the
       expression will be evaluated via the subscript operator. */
   enum { useAssign = ( RequiresEvaluation<VT1>::value || RequiresEvaluation<VT2>::value || !returnExpr ) };

   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct UseAssign {
      enum { value = useAssign };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DVecDVecAddExpr<VT1,VT2,TF>                 This;           //!< Type of this DVecDVecAddExpr instance.
   typedef typename AddTrait<RE1,RE2>::Type            ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::TransposeType          TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename ResultType::ElementType            ElementType;    //!< Resulting element type.
   typedef typename IntrinsicTrait<ElementType>::Type  IntrinsicType;  //!< Resulting intrinsic element type.

   //! Return type for expression template evaluations.
   typedef const typename SelectType< returnExpr, ExprReturnType, ElementType >::Type  ReturnType;

   //! Data type for composite expression templates.
   typedef typename SelectType< useAssign, const ResultType, const DVecDVecAddExpr& >::Type  CompositeType;

   //! Composite type of the left-hand side dense vector expression.
   typedef typename SelectType< IsExpression<VT1>::value, const VT1, const VT1& >::Type  LeftOperand;

   //! Composite type of the right-hand side dense vector expression.
   typedef typename SelectType< IsExpression<VT2>::value, const VT2, const VT2& >::Type  RightOperand;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = VT1::vectorizable && VT2::vectorizable &&
                         IsSame<ET1,ET2>::value &&
                         IntrinsicTrait<ET1>::addition };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the DVecDVecAddExpr class.
   //
   // \param lhs The left-hand side operand of the addition expression.
   // \param rhs The right-hand side operand of the addition expression.
   */
   explicit inline DVecDVecAddExpr( const VT1& lhs, const VT2& rhs )
      : lhs_( lhs )  // Left-hand side dense vector of the addition expression
      , rhs_( rhs )  // Right-hand side dense vector of the addition expression
   {
      BLAZE_INTERNAL_ASSERT( lhs.size() == rhs.size(), "Invalid vector sizes" );
   }
   //**********************************************************************************************

   //**Subscript operator**************************************************************************
   /*!\brief Subscript operator for the direct access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   */
   inline ReturnType operator[]( size_t index ) const {
      BLAZE_INTERNAL_ASSERT( index < lhs_.size(), "Invalid vector access index" );
      return lhs_[index] + rhs_[index];
   }
   //**********************************************************************************************

   //**Get function********************************************************************************
   /*!\brief Access to the intrinsic elements of the vector.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return Reference to the accessed values.
   */
   inline IntrinsicType get( size_t index ) const {
      typedef IntrinsicTrait<ElementType>  IT;
      BLAZE_INTERNAL_ASSERT( index < lhs_.size()    , "Invalid vector access index" );
      BLAZE_INTERNAL_ASSERT( index % IT::size == 0UL, "Invalid vector access index" );
      const IntrinsicType xmm1( lhs_.get( index ) );
      const IntrinsicType xmm2( rhs_.get( index ) );
      return xmm1 + xmm2;
   }
   //**********************************************************************************************

   //**Size function*******************************************************************************
   /*!\brief Returns the current size/dimension of the vector.
   //
   // \return The size of the vector.
   */
   inline size_t size() const {
      return lhs_.size();
   }
   //**********************************************************************************************

   //**Left operand access*************************************************************************
   /*!\brief Returns the left-hand side dense vector operand.
   //
   // \return The left-hand side dense vector operand.
   */
   inline LeftOperand leftOperand() const {
      return lhs_;
   }
   //**********************************************************************************************

   //**Right operand access************************************************************************
   /*!\brief Returns the right-hand side dense vector operand.
   //
   // \return The right-hand side dense vector operand.
   */
   inline RightOperand rightOperand() const {
      return rhs_;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can alias with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case the expression can alias, \a false otherwise.
   */
   template< typename T >
   inline bool canAlias( const T* alias ) const {
      return ( IsComputation<VT1>::value && ( RequiresEvaluation<VT1>::value ? lhs_.isAliased( alias ) : lhs_.canAlias( alias ) ) ) ||
             ( IsComputation<VT2>::value && ( RequiresEvaluation<VT2>::value ? rhs_.isAliased( alias ) : rhs_.canAlias( alias ) ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression is aliased with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case an alias effect is detected, \a false otherwise.
   */
   template< typename T >
   inline bool isAliased( const T* alias ) const {
      return ( lhs_.isAliased( alias ) || rhs_.isAliased( alias ) );
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  lhs_;  //!< Left-hand side dense vector of the addition expression.
   RightOperand rhs_;  //!< Right-hand side dense vector of the addition expression.
   //**********************************************************************************************

   //**Assignment to dense vectors*****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a dense vector-dense vector addition to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side addition expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a dense vector-dense
   // vector addition expression to a dense vector. Due to the explicit application of the
   // SFINAE principle, this operator can only be selected by the compiler in case either
   // of the two operands requires an intermediate evaluation.
   */
   template< typename VT3 >  // Type of the target dense vector
   friend inline typename EnableIf< UseAssign<VT3> >::Type
      assign( DenseVector<VT3,TF>& lhs, const DVecDVecAddExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      if( !IsComputation<VT1>::value && (~lhs).isAliased( &rhs.lhs_ ) ) {
         addAssign( ~lhs, rhs.rhs_ );
      }
      else if( !IsComputation<VT2>::value && (~lhs).isAliased( &rhs.rhs_ ) ) {
         addAssign( ~lhs, rhs.lhs_ );
      }
      else {
         assign   ( ~lhs, rhs.lhs_ );
         addAssign( ~lhs, rhs.rhs_ );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Assignment to sparse vectors****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a dense vector-dense vector addition to a sparse vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side sparse vector.
   // \param rhs The right-hand side addition expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a dense vector-dense
   // vector addition expression to a sparse vector. Due to the explicit application of the
   // SFINAE principle, this operator can only be selected by the compiler in case either
   // of the two operands requires an intermediate evaluation.
   */
   template< typename VT3 >  // Type of the target sparse vector
   friend inline typename EnableIf< UseAssign<VT3> >::Type
      assign( SparseVector<VT3,TF>& lhs, const DVecDVecAddExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_VECTOR_WITH_TRANSPOSE_FLAG( ResultType, TF );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename ResultType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( rhs );
      assign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense vectors********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a dense vector-dense vector addition to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side addition expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a dense vector-
   // dense vector addition expression to a dense vector. Due to the explicit application of
   // the SFINAE principle, this operator can only be selected by the compiler in case either
   // of the operands requires an intermediate evaluation.
   */
   template< typename VT3 >  // Type of the target dense vector
   friend inline typename EnableIf< UseAssign<VT3> >::Type
      addAssign( DenseVector<VT3,TF>& lhs, const DVecDVecAddExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      addAssign( ~lhs, rhs.lhs_ );
      addAssign( ~lhs, rhs.rhs_ );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to sparse vectors*******************************************************
   // No special implementation for the addition assignment to sparse vectors.
   //**********************************************************************************************

   //**Subtraction assignment to dense vectors*****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a dense vector-dense vector addition to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side addition expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a dense vector-
   // dense vector addition expression to a dense vector. Due to the explicit application of the
   // SFINAE principle, this operator can only be selected by the compiler in case either of the
   // operands requires an intermediate evaluation.
   */
   template< typename VT3 >  // Type of the target dense vector
   friend inline typename EnableIf< UseAssign<VT3> >::Type
      subAssign( DenseVector<VT3,TF>& lhs, const DVecDVecAddExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      subAssign( ~lhs, rhs.lhs_ );
      subAssign( ~lhs, rhs.rhs_ );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Subtraction assignment to sparse vectors****************************************************
   // No special implementation for the subtraction assignment to sparse vectors.
   //**********************************************************************************************

   //**Multiplication assignment to dense vectors**************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Multiplication assignment of a dense vector-dense vector addition to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side addition expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized multiplication assignment of a dense
   // vector-dense vector addition expression to a dense vector. Due to the explicit application
   // of the SFINAE principle, this operator can only be selected by the compiler in case either
   // of the operands requires an intermediate evaluation.
   */
   template< typename VT3 >  // Type of the target dense vector
   friend inline typename EnableIf< UseAssign<VT3> >::Type
      multAssign( DenseVector<VT3,TF>& lhs, const DVecDVecAddExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_VECTOR_WITH_TRANSPOSE_FLAG( ResultType, TF );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename ResultType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( rhs );
      multAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Multiplication assignment to sparse vectors*************************************************
   // No special implementation for the multiplication assignment to sparse vectors.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VT2 );
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_WITH_TRANSPOSE_FLAG( VT1, TF );
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_WITH_TRANSPOSE_FLAG( VT2, TF );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL BINARY ARITHMETIC OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Addition operator for the addition of two dense vectors (\f$ \vec{a}=\vec{b}+\vec{c} \f$).
// \ingroup dense_vector
//
// \param lhs The left-hand side dense vector for the vector addition.
// \param rhs The right-hand side dense vector for the vector addition.
// \return The sum of the two vectors.
// \exception std::invalid_argument Vector sizes do not match.
//
// This operator represents the addition of two dense vectors:

   \code
   blaze::DynamicVector<double> a, b, c;
   // ... Resizing and initialization
   c = a + b;
   \endcode

// The operator returns an expression representing a dense vector of the higher-order element
// type of the two involved vector element types \a T1::ElementType and \a T2::ElementType.
// Both vector types \a T1 and \a T2 as well as the two element types \a T1::ElementType and
// \a T2::ElementType have to be supported by the AddTrait class template.\n
// In case the current sizes of the two given vectors don't match, a \a std::invalid_argument
// is thrown.
*/
template< typename T1  // Type of the left-hand side dense vector
        , typename T2  // Type of the right-hand side dense vector
        , bool TF >    // Transpose flag
inline const DVecDVecAddExpr<T1,T2,TF>
   operator+( const DenseVector<T1,TF>& lhs, const DenseVector<T2,TF>& rhs )
{
   BLAZE_FUNCTION_TRACE;

   if( (~lhs).size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   return DVecDVecAddExpr<T1,T2,TF>( ~lhs, ~rhs );
}
//*************************************************************************************************

} // namespace blaze

#endif
