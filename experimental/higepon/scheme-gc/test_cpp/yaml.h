#include <scheme.h>
#include <util/String.h>
#include <util/Vector.h>

typedef util::Vector<util::Strings*> YAML;

void loadYAML(const util::String& path, YAML& yaml);

