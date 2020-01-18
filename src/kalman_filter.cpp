#include "kalman_filter.h"
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in)
{
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict()
{

  // Predict the state

  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z)
{
  // Update the state by using Kalman Filter equations

  VectorXd z_pred = H_ * x_;
  VectorXd y = z - z_pred;
  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_ * P_ * Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z)
{
  // Update the state by using Extended Kalman Filter equations

  float p_x = x_(0);
  float p_y = x_(1);
  float v_x = x_(2);
  float v_y = x_(3);

  float rho = sqrt(p_x * p_x + p_y * p_y);
  float theta = atan2(p_y, p_x);

  // check division by zero
  if (fabs(rho) < 0.0001)
  {
    cout << "UpdateEKF () - Error - Division by Zero" << endl;
    rho = 0.0001;
  }

  float rho_dot = (p_x * v_x + p_y * v_y) / rho;

  VectorXd z_pred = VectorXd(3);
  z_pred << rho, theta, rho_dot;
  VectorXd y = z - z_pred;

  // Normalize angle
  while (y(1) > M_PI || y(1) < -M_PI)
  {
    if (y(1) > M_PI)
    {
      y(1) -= M_PI;
    }
    else
    {
      y(1) += M_PI;
    }
  }

  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_ * P_ * Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;
}