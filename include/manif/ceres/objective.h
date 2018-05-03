#ifndef _MANIF_MANIF_CERES_OBJECTIVE_H_
#define _MANIF_MANIF_CERES_OBJECTIVE_H_

#include <ceres/cost_function.h>
#include <ceres/cost_function.h>

namespace manif
{

template <typename _Manifold>
class Objective : public ceres::CostFunction
{
  using Manifold = _Manifold;
  using Tangent  = typename _Manifold::Tangent;
  using Jacobian = typename _Manifold::Jacobian;
  using JacobianMap =
    Eigen::Map<Eigen::Matrix<
      double, Manifold::DoF, Manifold::DoF, Eigen::RowMajor>>;

  template <typename _Scalar>
  using ManifoldTemplate = typename _Manifold::template ManifoldTemplate<_Scalar>;

  template <typename _Scalar>
  using TangentTemplate = typename Tangent::template TangentTemplate<_Scalar>;

public:

  explicit Objective(const Manifold& target_state)
    : target_state_(target_state)
  {
    set_num_residuals(Manifold::DoF);
    /// @todo fix dat
//    mutable_parameter_block_sizes()->push_back(Manifold::RepSize);
//    mutable_parameter_block_sizes()->push_back(Manifold::DoF);
  }

  virtual ~Objective() = default;

  template <typename T>
  bool operator()(const T* const state_raw, T* residuals_raw) const
  {
    const Eigen::Map<const ManifoldTemplate<T>> state(state_raw);
    Eigen::Map<TangentTemplate<T>> error(residuals_raw);

    error = target_state_.template cast<T>() - state;

    const auto casted = target_state_.template cast<T>();

    std::cout << "state r " << state.coeffs()(0) << "\n";
    std::cout << "state i " << state.coeffs()(1) << "\n";
    std::cout << "state a " << state.angle() << "\n";

    std::cout << "target r " << casted.coeffs()(0) << "\n";
    std::cout << "target i " << casted.coeffs()(1) << "\n";
    std::cout << "target a " << casted.angle() << "\n";

    std::cout << "error " << residuals_raw[0] << "\n";

    return true;
  }

  virtual bool Evaluate(double const* const* parameters_raw,
                        double* residuals_raw,
                        double** jacobians_raw) const
  {
    const Eigen::Map<const Manifold> state(parameters_raw[0]);

    Eigen::Map<Tangent> error(residuals_raw);

//    std::cout << "target_state " << target_state_ << "\n";
//    std::cout << "state " << state << "\n";

    if (jacobians_raw != nullptr)
    {
      if (jacobians_raw[0] != nullptr)
      {
        target_state_.rminus(state, error, J_rminus_ma, J_rminus_mb);

//        std::cout << "errorA " << error << "\n";
        JacobianMap jacobian(jacobians_raw[0]);
        jacobian = J_rminus_mb;

//        std::cout << "jacobian " << jacobian << "\n";
      }
    }
    else
    {
      error = target_state_ - state;

//      std::cout << "errorB " << error << "\n";
    }

//    std::cout << "---------------------\n\n";

    return true;
  }

protected:

  const Manifold target_state_;
  mutable Jacobian J_rminus_ma, J_rminus_mb;
};

//using ObjectiveSO2 = Objective<SO2d>;
//using ObjectiveSO3 = Objective<SO3d>;
//using ObjectiveSE2 = Objective<SE2d>;
//using ObjectiveSE3 = Objective<SE3d>;

} /* namespace manif */

#endif /* _MANIF_MANIF_CERES_OBJECTIVE_H_ */