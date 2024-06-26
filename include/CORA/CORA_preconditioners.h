/**
 * @file CORA_preconditioners.h
 * @author
 * @brief
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <Eigen/CholmodSupport>

#include <CORA/CORA_types.h>
#include <memory>
#include <vector>

#include "Optimization/Riemannian/Concepts.h"

namespace CORA {

using CholeskyFactorization = Eigen::CholmodDecomposition<SparseMatrix>;
using CholFactorPtr = std::shared_ptr<CholeskyFactorization>;
using CholFactorPtrVector = std::vector<CholFactorPtr>;

/**
 * @brief Generate a block-diagonal preconditioner for the given matrix A based
 * on Cholesky decompositions of each block. This assumes that the matrix A is
 * the un-marginalized data matrix, so the last row/column of A is ignored. When
 * applying the preconditioner, the last row/column of the input vector is
 * mapped to zero. In SLAM, this corresponds to pinning the last translation
 * to the origin. See Appendix D of the CORA paper for more details.
 *
 * @param A the matrix to precondition (should be symmetric positive definite)
 * @param block_sizes the sizes of the blocks (should sum to A.rows() - 1)
 * @return Optimization::Riemannian::LinearOperator<Matrix, Matrix>
 */
CholFactorPtrVector getBlockCholeskyFactorization(const SparseMatrix &A,
                                                  const VectorXi &block_sizes);

Matrix blockCholeskySolve(const CholFactorPtrVector &block_chol_factor_ptrs,
                          const Matrix &rhs);

} // namespace CORA
