#include "yaml.h"

using namespace monash::util;

void loadYAML(const util::String& path, YAML& yaml)
{
    ifstream ifs(path.data());
    util::String line;

    int index = -1;
    while (getline(ifs, line))
    {
        if (line.size() == 0) continue;
        if (line[0] == '#')
        {
            continue;
        }
        else if (line[0] == '-')
        {
            strings* ss = new strings;
            index++;
            yaml.add(ss);

        }
        else if (line[1] == '-')
        {
            strings* ss = yaml.get(index);
            if (NULL == ss)
            {
                fprintf(stderr, "unknown yaml type %s\n", path.data());
                exit(-1);
            }
            ss->add(line.substr(3, line.size()));
        }
    }
}
