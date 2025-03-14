#pragma once
#include <Eigen/Dense>
#include <functional>
#include <cmath>

namespace uavsdk
{
    namespace kalman_filters
    {
        class UnscentedKalmanFilter {
        public:
            using ProcessModel = std::function<Eigen::VectorXd(const Eigen::VectorXd&, const Eigen::VectorXd&)>;
            using MeasurementModel = std::function<Eigen::VectorXd(const Eigen::VectorXd&)>;
        
            UnscentedKalmanFilter(int state_dim, int measurement_dim,
                                  const ProcessModel& process_model,
                                  const MeasurementModel& measurement_model,
                                  double alpha = 1e-3, double beta = 2.0, double kappa = 0.0)
                : n(state_dim), m(measurement_dim),
                  f(process_model), h(measurement_model),
                  alpha(alpha), beta(beta), kappa(kappa) {
                computeWeights();
            }
        
            void initialize(const Eigen::VectorXd& x0, const Eigen::MatrixXd& P0,
                            const Eigen::MatrixXd& Q, const Eigen::MatrixXd& R) {
                x = x0;
                this->P = P0;
                this->Q = Q;
                this->R = R;
            }
        
            void predict(const Eigen::VectorXd& u = Eigen::VectorXd()) {
                // Generate sigma points
                Eigen::MatrixXd sigma_points;
                generateSigmaPoints(sigma_points);
                
                // Propagate sigma points through process model
                int num_sigma = 2 * n + 1;
                sigma_pred.resize(n, num_sigma);
                for (int i = 0; i < num_sigma; ++i) {
                    sigma_pred.col(i) = f(sigma_points.col(i), u);
                }
        
                // Compute predicted state mean
                x = sigma_pred * weights_mean;
        
                // Compute predicted state covariance
                P.setZero();
                for (int i = 0; i < num_sigma; ++i) {
                    Eigen::VectorXd diff = sigma_pred.col(i) - x;
                    P += weights_cov(i) * diff * diff.transpose();
                }
                P += Q;
            }
        
            void update(const Eigen::VectorXd& z) {
                int num_sigma = 2 * n + 1;
        
                // Transform sigma points into measurement space
                Eigen::MatrixXd z_sigma(m, num_sigma);
                for (int i = 0; i < num_sigma; ++i) {
                    z_sigma.col(i) = h(sigma_pred.col(i));
                }
        
                // Compute measurement mean
                Eigen::VectorXd z_pred = z_sigma * weights_mean;
        
                // Compute measurement covariance
                Eigen::MatrixXd S(m, m);
                S.setZero();
                for (int i = 0; i < num_sigma; ++i) {
                    Eigen::VectorXd diff = z_sigma.col(i) - z_pred;
                    S += weights_cov(i) * diff * diff.transpose();
                }
                S += R;
        
                // Compute cross-covariance
                Eigen::MatrixXd cross_cov(n, m);
                cross_cov.setZero();
                for (int i = 0; i < num_sigma; ++i) {
                    Eigen::VectorXd x_diff = sigma_pred.col(i) - x;
                    Eigen::VectorXd z_diff = z_sigma.col(i) - z_pred;
                    cross_cov += weights_cov(i) * x_diff * z_diff.transpose();
                }
        
                // Kalman gain
                Eigen::MatrixXd K = cross_cov * S.inverse();
        
                // Update state and covariance
                Eigen::VectorXd y = z - z_pred;
                x += K * y;
                P -= K * S * K.transpose();
            }
        
            Eigen::VectorXd getState() const { return x; }
            Eigen::MatrixXd getCovariance() const { return P; }
        
        private:
            int n; // State dimension
            int m; // Measurement dimension
        
            Eigen::VectorXd x; // State vector
            Eigen::MatrixXd P; // Covariance matrix
            Eigen::MatrixXd Q; // Process noise covariance
            Eigen::MatrixXd R; // Measurement noise covariance
        
            ProcessModel f; // Process model
            MeasurementModel h; // Measurement model
        
            // UKF parameters
            double alpha, beta, kappa, lambda;
            Eigen::VectorXd weights_mean;
            Eigen::VectorXd weights_cov;
            Eigen::MatrixXd sigma_pred; // Predicted sigma points
        
            void computeWeights() {
                lambda = alpha * alpha * (n + kappa) - n;
                weights_mean.resize(2 * n + 1);
                weights_cov.resize(2 * n + 1);
        
                weights_mean(0) = lambda / (n + lambda);
                weights_cov(0) = weights_mean(0) + (1 - alpha * alpha + beta);
        
                double weight = 1.0 / (2.0 * (n + lambda));
                for (int i = 1; i < 2 * n + 1; ++i) {
                    weights_mean(i) = weight;
                    weights_cov(i) = weight;
                }
            }
        
            void generateSigmaPoints(Eigen::MatrixXd& sigma_points) {
                int num_sigma = 2 * n + 1;
                sigma_points.resize(n, num_sigma);
        
                double gamma = std::sqrt(n + lambda);
                Eigen::MatrixXd L = P.llt().matrixL();
        
                sigma_points.col(0) = x;
                for (int i = 0; i < n; ++i) {
                    sigma_points.col(i + 1) = x + gamma * L.col(i);
                    sigma_points.col(i + 1 + n) = x - gamma * L.col(i);
                }
            }
        };
    } // namespace ukf
} // namespace uavsdk