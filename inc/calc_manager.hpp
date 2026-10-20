#ifndef CALC_MANAGER_HPP
#define CALC_MANAGER_HPP

#include <string>
#include <vector>

/* ----------------------------- Materials -------------------------------- */

enum MaterialID {
  AIR = 0,
  BRAIN,
  CARBON,
  CONCRETE,
  COPPER,
  GERMANIUM,
  GOLD,
  LEAD,
  MUSCLE,
  PLASTIC,
  SILICON,
  TIN,
  TUNGSTEN,
  WATER,
  NBR_MATERIALS
};

/* ----------------------------- Data structs ----------------------------- */

struct Material {
  MaterialID id;       /* Index into material tables */
  double atomic_ratio; /* Z/A */
  double density;      /* g/um^3 */
  double excitation;   /* eV */
};

struct Projectile {
  double E;       /* Current kinetic energy [eV] */
  double E_init;  /* Initial kinetic energy [eV] */
  double dE_dx;   /* Energy loss [eV/um] */
  double mass;    /* eV/c^2 */
  int charge;     /* Charge */
};

/* ----------------------------- Interface -------------------------------- */

class CalcManager {

  public:

  CalcManager(MaterialID id, int p_charge, double p_mass, double p_energy);
  ~CalcManager();

  bool run();

  private:

  /* ----------------------------- Functions -------------------------- */
  bool Stopped(const Projectile *p_data);
  double getBeta(const Projectile *p_data = nullptr);
  double getdEdx(const Projectile *p_data);

  bool saveData(std::ofstream &file, double depth, double value);
  bool clearData(const std::string &fname);

  /* ----------------------------- Constants -------------------------------- */
  const double dx = 1;
  const bool verbose = true;
  Material *m_data = nullptr;
  Projectile *p_data = nullptr;
  const MaterialID midx;
  const double EV_TO_MEV = 1e-6;
  const double MEV_TO_EV = 1e6;
  const double U_TO_EV   = 931.5e6;
  const double UM_TO_M   = 1e-6;
  const double UM_TO_CM  = 1e-4;

  /* ----------------------------- Material tables -------------------------- */
  const std::vector<std::string> materials_name = {
      "air", "brain", "carbon", "concrete", "copper",
      "germanium", "gold", "lead", "muscle", "plastic",
      "silicon", "tin", "tungsten", "water"
  };

  const std::vector<double> materials_ZA = {
      0.49919, 0.55239, 0.49954, 0.50932, 0.45636,
      0.44071, 0.40108, 0.39575, 0.55000, 0.57033,
      0.49848, 0.42120, 0.40250, 0.55508
  };

  const std::vector<double> material_exc = {
      85.7, 73.9, 78.0, 124.5, 322.0,
      350.0, 790.0, 823.0, 74.6, 57.4,
      173.0, 488.0, 727.0, 75.0
  };

  const std::vector<double> materials_rho = {
      1.205E-15, 1.040E-12, 1.700E-12, 2.300E-12, 8.960E-12,
      5.323E-12, 1.932E-11, 1.135E-11, 1.050E-12, 9.300E-13,
      2.330E-12, 7.310E-12, 1.930E-11, 1.000E-12
  };
};



#endif // CALC_MANAGER_HPP