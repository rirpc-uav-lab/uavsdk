#include "uavsdk/kalman_filters/ukf/ukf.hpp"

// Example process model (constant velocity model)
Eigen::VectorXd processModel(const Eigen::VectorXd& x, const Eigen::VectorXd& u) {
    double dt = 1.0;
    Eigen::VectorXd x_new(4);
    x_new << x[0] + x[2]*dt,
             x[1] + x[3]*dt,
             x[2],
             x[3];
    return x_new;
}

// Example measurement model (position measurements)
Eigen::VectorXd measurementModel(const Eigen::VectorXd& x) {
    Eigen::VectorXd z(2);
    z << x[0], x[1];
    return z;
}

int main() {
    // Create UKF instance
    int state_dim = 4;
    int meas_dim = 2;
    uavsdk::kalman_filters::UnscentedKalmanFilter ukf(state_dim, meas_dim, processModel, measurementModel);

    // Initialize state and covariance
    Eigen::VectorXd x0(state_dim);
    x0 << 0, 0, 1, 1; // Position (0,0), velocity (1,1)
    Eigen::MatrixXd P0 = Eigen::MatrixXd::Identity(state_dim, state_dim);
    Eigen::MatrixXd Q = Eigen::MatrixXd::Identity(state_dim, state_dim) * 0.1;
    Eigen::MatrixXd R = Eigen::MatrixXd::Identity(meas_dim, meas_dim) * 0.1;
    ukf.initialize(x0, P0, Q, R);

    // Simulate prediction and update steps
    ukf.predict();
    Eigen::VectorXd measurement(2);
    measurement << 1.1, 1.2; // Simulated measurement
    ukf.update(measurement);

    // Get estimated state
    Eigen::VectorXd estimated_state = ukf.getState();
    return 0;
}