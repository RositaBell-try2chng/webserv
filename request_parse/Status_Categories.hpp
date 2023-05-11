#ifndef HTTP_STATUS_CATEGORIES_HPP
# define HTTP_STATUS_CATEGORIES_HPP

# include "../webserv.hpp"


enum Status_Categories {	Informational_responses = 1,
							Successful_responses,
							Redirection_messages,
							Client_error_responses,
							Server_error_responses,
							Security_events,
							NO_NAME						};

enum Informational_responses {	Continue,
								Switching_Protocols,
								Processing,
								Early_Hints			};

enum Successful_responses {	OK,
							Created,
							Accepted,
							Non_Authoritative_Information,
							No_Content,
							Reset_Content,
							Partial_Content,
							Multi_Status,
							Already_Reported,
							IM_Used	= 26					};

enum Redirection_messages {	Multiple_Choices,
							Moved_Permanently,
							Found,
							See_Other,
							Not_modified,
							Use_Proxy,
							Unused,
							Temporary_Redirect,
							Permanent_Redirect	};

// Not all
enum Client_error_responses {	Bad_Request,
								Unauthorized,
								Payment_required,
								Forbidden,
								Not_found			};

// Not all
enum Server_error_responses {	Internal_server_error,
								Not_Implemented			};

#endif