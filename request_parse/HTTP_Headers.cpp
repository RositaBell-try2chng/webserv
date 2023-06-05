#include "HTTP_Headers.hpp"

int     ft_if_basic_hdr(std::string key) {

    std::string basic_hdrs[] = {    "Host",
                                    "Something_one",
                                    "Something_two",
                                    "Something_three",
                                    "Something_four",
                                    "Stop"              };

    for (int i = 0; ; ++i) {
        if (!key.compare(basic_hdrs[i]))
            return i;
        if (!basic_hdrs[i].compare("Stop"))
            break ;
    }

    return 1000;
}

void    ft_headers_parse(std::map<std::string, std::string> hdrs, HTTP_Request req) {

    std::map<std::string, std::string>::iterator end = hdrs.end();

    for (std::map<std::string, std::string>::iterator it = hdrs.begin(); it != end; ++it) {
        switch (ft_if_basic_hdr(it->first)) {
            case 0: {; break;}
            case 1: {; break;}
            case 2: {; break;}
            case 3: {; break;}
            case 4: {; break;}
            default: {; break;}
        }
    }
}
