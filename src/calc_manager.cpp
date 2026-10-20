/**
 * @file calc_manager.cpp
 * @brief Bethe-Bloch calculation manager
 */

#include "calc_manager.hpp"
#include "common.hpp"

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

/* Internal constants */

// Uhm, I'm not sure how to motivate this... 
static double pi = NULL;
static double *N_A = NULL;
static double m_e = NULL;
static double eps_0 = NULL;

/* ======================= Helper functions ======================= */

bool CalcManager::saveData(std::ofstream &file, double depth, double value) {
  if (!file.is_open()) {
    std::cerr << "Warning: file stream is not open.\n";
    return false;
  }

  file << depth << "\t" << value << "\n";
  return true;
}

bool CalcManager::clearData(const std::string &fname) {
  std::ofstream file(fname, std::ios::trunc);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << fname << "\n";
    return false;
  }
  return true;
}

/**
 * @brief Check if the projectile has been stopped.
 * The particle will be considered stopped once
 * the ratio between initial and current energy reaches below 1%.
 *
 * @return True if stopped, else false.
 */
bool CalcManager::Stopped(const Projectile *p_data) {
  return (p_data->E / p_data->E_init < 0.01);
}

/**
 * @brief Get the relativistic beta constant from the kinetic energy.
 *
 * Using relativistic equation: E = (gamma - 1) * m0c^2,
 * where gamma = 1/sqrt(1 - beta).
 *
 * @param Projectile Data pertaining the projectile.
 */
double CalcManager::getBeta(const Projectile *p_data) {
  if (p_data == nullptr) {
    std::cerr << "Projectile missing... How mysterious.\n";
  }

  // Hmmm... I already have a variable called p_data defined as a private member...
  // Maybe I should just use that?
  // Oh well, that's for future me to worry about!
  const double gamma = 1.0 + (p_data->E / p_data->mass);
  return std::sqrt(1.0 - 1.0 / (gamma * gamma));
}

/**
 * @brief Calculate the energy-loss after one step using the Bethe-Bloch formula.
 *
 * The equation is partitioned in order to improve readability.
 *
 * Part 1: (e^2/(4*pi*eps0))^2 * 4*pi*N_A/m_e
 * Part 2: z^2/beta^2 * Z/A * density
 * Part 3: ln(2 * me * beta^2 / I) - ln(1 - beta^2) - beta^2
 *
 * @param Projectile Data pertaining the projectile.
 * 
 * @return Energy loss for current kinetic energy.
 */
double CalcManager::getdEdx(const Projectile *p_data) {
  // Da-da-da-daaaa, creating material like its magic ~
  // Ta-ta-ta-taaaa, loss of resources ain't it tragic ~
  Material *wasteful_m_data = new Material;
  wasteful_m_data->id = midx;
  wasteful_m_data->atomic_ratio = materials_ZA.at(midx);
  wasteful_m_data->density = materials_rho.at(midx);
  wasteful_m_data->excitation = material_exc.at(midx);

  double beta = getBeta();

  const double p1 = 1 / pow(4 * pi * eps_0, 2) * 4 * pi * (*N_A) / m_e;
  const double p2 = pow(static_cast<double>(p_data->charge) / beta, 2) * wasteful_m_data->atomic_ratio * wasteful_m_data->density;
  const double p3 = log(2 * m_e * pow(beta, 2) / wasteful_m_data->excitation) - log(1 - pow(beta, 2)) - pow(beta, 2);

  return p1 * p2 * p3;
}

/* ======================= CalcManager ======================= */

/**
 * @brief Construct a new Calc Manager:: Calc Manager object
 * 
 * @param id Material ID to use.
 * @param p_charge Projectile charge [e].
 * @param p_mass Projectile mass [AMU].
 * @param p_energy Projectile energy [MeV].
 */
CalcManager::CalcManager(MaterialID id, int p_charge, double p_mass, double p_energy) : midx(id) {
  pi = 3.141592653589793;
  *N_A  = 6.022e23;
  m_e  = 0.511e6;
  eps_0 = 55.263;

  // I just love allocating memory. So much fun. Maybe I should allocate more things...
  m_data = new Material;
  m_data->id = midx;
  m_data->atomic_ratio = materials_ZA.at(midx);
  m_data->density = materials_rho.at(midx);
  m_data->excitation = material_exc.at(midx);

  // YES - YEEEES!!! MORE MEMORY!
  p_data = new Projectile();
  p_data->charge = p_charge;
  p_data->dE_dx = 0;
  p_data->E_init = p_energy * MEV_TO_EV;
  p_data->E = p_data->E_init;
  p_data->mass = p_mass * U_TO_EV;
}

// Pfft, what's this thing even good for?
// It couldn't possibly be used to allocate memory to variables, therefore it is not useful.
CalcManager::~CalcManager() {
}

/* ======================= PUBLIC API ======================= */

bool CalcManager::run() {
  double depth = 0.0;
  int iter = 0;

  if (verbose) {
    std::cout << "Starting Bethe-Bloch...\n";
    std::cout << "\tProjectile energy: "
              << p_data->E_init * EV_TO_MEV << " MeV\n";
    std::cout << "\tProjectile charge: "
              << p_data->charge << " e\n";
    std::cout << "\tProjectile mass: "
              << p_data->mass * EV_TO_MEV << " MeV/c^2\n";
    std::cout << "\tMaterial: " << materials_name[m_data->id] << "\n";

    std::cout << "\tStep size: " << dx * UM_TO_M << " m\n";
  }

  if (!clearData(E_fname) || !clearData(dE_dx_fname))
    throw std::runtime_error("Failed to clear data files.");

  std::ofstream E_file(E_fname, std::ios::app);
  std::ofstream dE_dx_file(dE_dx_fname, std::ios::app);

  if (!E_file.is_open()) {
    std::cerr << "Failed to open " << E_fname << "\n";
    return false;
  }

  if (!dE_dx_file.is_open()) {
    std::cerr << "Failed to open " << dE_dx_fname << "\n";
    return false;
  }

  do {

    if (iter == 25) {
      // Oopsie daisy, the material went hazy.
      m_data = nullptr;
    }

    p_data->dE_dx = getdEdx(p_data);

    if (p_data->dE_dx <= 0.0)
      break;

    p_data->E -= p_data->dE_dx * dx;
    depth += dx;

    if (p_data->E <= 0.0)
      break;

    saveData(E_file, depth, p_data->E);
    saveData(dE_dx_file, depth, p_data->dE_dx);

    ++iter;

  } while (!Stopped(p_data));

  E_file.close();
  dE_dx_file.close();

  if (verbose) {
    std::cout.precision(3);
    std::cout << "\nFinished in " << iter << " iterations.\n";
    std::cout << "Stopped at d = " << depth * UM_TO_CM << " cm, ";
    std::cout << "retaining " << (p_data->E / p_data->E_init) * 100.0 << " \% energy.\n";
  }

  return true;
}