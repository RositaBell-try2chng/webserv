#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../webserv.hpp"
# include "../Logger.hpp"

enum class Status_Categories {	Informational_responses = 1,
								Successful_responses,
								Redirection_messages,
								Client_error_responses,
								Server_error_responses		};

enum class Informational_responses {	Continue,
										Switching_Protocols,
										Processing,
										Early_Hints			};

enum class Successful_responses {	OK,
									Created,
									Accepted,
									Non_Authoritative_Information,
									No_Content,
									Reset_Content,
									Partial_Content,
									Multi_Status,
									Already_Reported,
									IM_Used	= 26					};

enum class Redirection_messages {	Multiple_Choices,
									Moved_Permanently,
									Found,
									See_Other,
									Not_modified,
									Use_Proxy,
									Unused,
									Temporary_Redirect,
									Permanent_Redirect	};

// Not all
enum class Client_error_responses {	Bad_Request,
									Unauthorized,
									Payment_required,
									Forbidden,
									Not_found			};

// Not all
enum class Server_error_responses {	Internal_server_error,
									Not_Implemented			};

#endif