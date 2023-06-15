#ifndef HTTP_METHODS_HPP
# define HTTP_METHODS_HPP

# include "../Logger.hpp"
# include "HTTP_Utils.hpp"
# include "HTTP_Answer.hpp"
# include "HTTP_Request.hpp"
# include <map>

struct HTTP_Answer;

// Acts depends on request's method (GET, POST, DELETE)
void	ft_get_answ(HTTP_Request &req, HTTP_Answer &answ);
void	ft_post_answ(HTTP_Request &req, HTTP_Answer &answ);
void	ft_delete_answ(HTTP_Request &req, HTTP_Answer &answ);

#endif
