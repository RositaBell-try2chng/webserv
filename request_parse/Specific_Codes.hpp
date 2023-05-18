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
								Not_Found,
								Method_Not_Allowed,
								Not_Acceptable,
								Proxy_Authentication_Required,
								Request_Timeout,
								Conflict,
								Gone,
								Length_Required,
								Precondition_Failed,
								Payload_Too_Large,
								URI_Too_Long,
								Unsupported_Media_Type,
								Range_Not_Satisfiable,
								Expectation_Failed,
								Teapot,
								Page_Expired,
								Method_Failure,
								Misdirected_Request,
								Unprocessable_Entity,
								Locked,
								Failed_Dependency,
								Too_Early,
								Upgrade_Required,
								Precondition_Required = 28,
								Too_Many_Requests,
								Request_Header_Fields_Too_Large_Spoty,
								Request_Header_Fields_Too_Large,
								Login_Time_out = 40,
								No_Response = 44,
								Retry_With = 49,
								Blocked_By_Windows_Parental_Controls,
							//	Redirect = 51,
								Unavailable_For_Legal_Reasons,
								No_response = 60,
								Too_Many_IP = 63,
								Incompatible_protocol_versions,
								Request_Header_Too_Large = 94,
								SSL_Certificate_Error,
								SSL_Certificate_Required,
								HTTP_Request_Sent_To_HTTPS_Port,
								Invalid_Token,
							//	Token_Required = 99
								Client_Closed_Request						};

// Not all
enum Server_error_responses {	Internal_Server_Error,
								Not_Implemented,
								Bad_Gateway,
								Service_Unavailable,
								Gateway_Timeout,
								HTTP_Version_Not_Supported,
								Variant_Also_Negotiates,
								Insufficient_Storage,
								Loop_Detected,
								Bandwidth_Limit_Exceeded,
								Not_Extended,
								Network_Authentication_Required,
								Web_Server_Returned_An_Unknown_Error = 20,
								Web_Server_Is_Down,
								Connection_Timed_Out,
								Origin_Is_Unreachable,
								A_Timeout_Occurred,
								SSL_Handshake_Failed,
								Invalid_SSL_Certificate,
								Railgun_Error,
								Site_Is_Overloaded = 29,
							//	Cloudflare_1xxx = 30,
								Site_Is_Frozen,
								unauthorized = 61,
								Network_Read_Timeout_Error = 98,
								Network_Connect_Timeout_Error		};

//=======================================================================
//=======================================================================

std::string ft_reason_phrase(int *code);

#endif