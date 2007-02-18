#include "yaml.h"

using namespace util;
using namespace std;

void loadYAML(const string& path, YAML& yaml)
{
    ifstream ifs(path.data());
    string line;

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
            yaml.push_back(ss);

        }
        else if (line[1] == '-')
        {
            strings* ss = yaml.at(index);
            if (NULL == ss)
            {
                fprintf(stderr, "unknown yaml type %s\n", path.data());
                exit(-1);
            }
            ss->push_back(line.substr(3, line.size()));
        }
    }
}
