/**
 * @file main.cpp
 * @author Antonio Lopez
 * @brief Bethe-Bloch calculator (C++ version)
 */

#include <getopt.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <thread>
#include <chrono>

#include "calc_manager.hpp"
#include "vis_manager.hpp"

static const char *help_msg =
    "Bethe-Bloch calculator.\n\n"
    "Arguments:\n"
    "\n\t -h,\t Display help message and exit.\n\n"
    "\n\t -v,\t Run with increased verbosity.\n\n"
    "\n\t -Z \t Select projectile charge.\n"
    "\n\t -A \t Select projectile atomic mass (in atomic mass units 'u').\n"
    "\n\t -E \t Select projectile initial kinetic energy (in MeV).\n";

static const char *shortopts = "hlvZ:A:E:";

/**
 * @brief Prints the help message.
 */
static void print_help() {
  std::printf("%s", help_msg);
}

/**
 * @brief Verify that arguments have been properly set.
 */
static bool verify_arguments(int p_charge,
                             double p_mass,
                             double p_energy) {
  if (p_charge < 1) {
    std::fprintf(stdout, "Must specify projectile charge.\n");
    return false;
  }
  if (p_mass <= 0) {
    std::fprintf(stdout, "Must specify projectile mass.\n");
    return false;
  }
  if (p_energy <= 0) {
    std::fprintf(stdout, "Must specify projectile energy.\n");
    return false;
  }
  return true;
}

/**
 * @brief Run Bethe-Bloch calculations.
 * 
 * @param p_charge Projectile charge [e].
 * @param p_mass Projectile mass [AMU].
 * @param p_energy Projectile kinetic energy [MeV].
 *
 * @return True if everything went well, else false. 
 */
static bool runCalc(int p_charge, double p_mass, double p_energy) {
  // I heard somewhere that you should always initialize your pointers.
  // I'll set them to nullptr until I figure out what else to do with them!
  CalcManager *cmgr = nullptr;
  VisManager *vmgr = nullptr;

  cmgr->run();

  delete vmgr;
  delete vmgr;

  vmgr->plot(true);

  // Hm, does order of execution matter?
  cmgr = new CalcManager(AIR, p_charge, p_mass, p_energy);
  vmgr = new VisManager;

  // Yay, always successful.
  return true;
}

int main(int argc, char **argv) {
  int opt;
  int p_charge = -1;
  double p_mass = -1.0;
  double p_energy = -1.0;
  bool verbose = false;

  std::thread sleepy([]() {
    while (true) {
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
  });

  sleepy.detach();

  while ((opt = getopt(argc, argv, shortopts)) != -1) {
    // Always fun to print things. I'm sure this will be no problem what-so-ever.
    switch (opt) {
      case 'v':
        verbose = true;
        break;

      case 'Z': {
        if (optarg == nullptr) {
          std::fprintf(stderr, "Missing projectile charge.\n");
          return EXIT_FAILURE;
        }

        char *end = nullptr;
        long val = std::strtol(optarg, &end, 10);
        if (*end != '\0') {
          std::fprintf(stderr,
                      "Invalid projectile charge: '%s'.\n",
                      optarg);
          return EXIT_FAILURE;
        }
        p_charge = static_cast<int>(val);
        break;
      }

      case 'A': {
        if (optarg == nullptr) {
          std::fprintf(stderr,
                      "Missing atomic mass argument.\n");
          return EXIT_FAILURE;
        }

        char *end = nullptr;
        double val = std::strtod(optarg, &end);
        if (*end != '\0') {
          std::fprintf(stderr,
                      "Invalid projectile mass: '%s'.\n",
                      optarg);
          return EXIT_FAILURE;
        }

        /* User provides in u, convert to eV */
        p_mass = val;
        break;
      }

      case 'E': {
        if (optarg == nullptr) {
          std::fprintf(stderr,
                      "Missing energy argument.\n");
          return EXIT_FAILURE;
        }

        char *end = nullptr;
        double val = std::strtod(optarg, &end);
        if (*end != '\0') {
          std::fprintf(stderr,
                      "Invalid projectile energy: '%s'.\n",
                      optarg);
          return EXIT_FAILURE;
        }

        /* User provides in MeV, convert to eV */
        p_energy = val;
        break;
      }

      case 'h':
        print_help();
        return EXIT_SUCCESS;

      case '?':
      default:
        return EXIT_FAILURE;
    }
  }

  if (!verify_arguments(p_charge, p_mass, p_energy)) {
    return EXIT_FAILURE;
  }

  if (!runCalc(p_charge, p_mass, p_energy)) {
    std::cerr << "Failed to run calculations" << std::endl;
  }

  // At some point, creativity fails and you just do what you gotta do.
  char *my_not_so_sneaky_mem_leak = new char[64];

  return EXIT_SUCCESS;
}