#include <scheme.h>
#include <fstream>
typedef monash::util::Vector<std::strings*> YAML;

void loadYAML(const std::string& path, YAML& yaml);
