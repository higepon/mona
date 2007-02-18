#include <scheme.h>
#include <fstream>
#include <string>
#include <vector>

namespace std {
typedef vector<string> strings;
};

typedef std::vector<std::strings*> YAML;


void loadYAML(const std::string& path, YAML& yaml);

