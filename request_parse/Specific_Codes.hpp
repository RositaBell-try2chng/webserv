#ifndef HTTP_SPECIFIC_CODES_HPP
# define HTTP_SPECIFIC_CODES_HPP

# include "../webserv.hpp"


enum Status_Categories {	informational_responses = 1,
							successful_responses,
							redirection_messages,
							client_error_responses,
							server_error_responses,
							security_events,
							no_name						};

//=======================================================================
//=======================================================================

enum Informational_responses {	Continue,
								Switching_Protocols,
								Processing,
								Early_Hints,
								Response_is_Stale = 10,
								Revalidation_Failed,
								Disconnected_Operation,
								Heuristic_Expiration,
								Miscellaneous_Warning = 99	};

enum Successful_responses {	OK,
							Created,
							Accepted,
							Non_Authoritative_Information,
							No_Content,
							Reset_Content,
							Partial_Content,
							Multi_Status,
							Already_Reported,
							Transformation_Applied = 14,
							IM_Used	= 26,
							Miscellaneous_Persistent_Warning = 99	};

enum Redirection_messages {	Multiple_Choices,
							Moved_Permanently,
							Found,
							See_Other,
							Not_Modified,
							Use_Proxy,
							Unused,
							Temporary_Redirect,
							Permanent_Redirect	};

// Not all
enum Client_error_responses {	Bad_Request,
								Unauthorized,
								Payment_Required,
								Forbidden,
								Not_Found			};

// Not all
enum Server_error_responses {	Internal_Server_Error,
								Not_Implemented,
								Bad_Gateway,
								Service_Unavailable	};

//=======================================================================
//=======================================================================

std::string ft_reason_phrase(int *code);

#endif