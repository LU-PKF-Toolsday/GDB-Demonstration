#ifndef VIS_MANAGER_HPP
#define VIS_MANAGER_HPP

#include <string>
#include <vector>

using std::string;
using std::vector;

class VisManager {
  public:
    VisManager();
    ~VisManager();

    bool plot(bool use_log);
  
  private:
    vector<char *> titles;
    vector<char *> x_titles;
    vector<char *> y_titles;
    vector<char *> fnames;
};

#endif // VIS_MANAGER_HPP