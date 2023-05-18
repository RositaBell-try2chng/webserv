#include "Specific_Codes.hpp"
/*
	Unless otherwise stated, the status code
	is part of the HTTP standard (RFC 9110).

	RFC - Request for Comments.
*/

std::string ft_reason_phrase(int *code) {

	Status_Categories category = static_cast<Status_Categories>(code[0]);

//====================================================================================

//		1 X X		INFORMATIONAL RESPONSES
/*
	An informational response indicates that the request was received
	and understood. It is issued on a provisional basis while request
	processing continues. It alerts the client to wait for a final
	response. The message consists only of the status line and optional
	header fields, and is terminated by an empty line. As the HTTP/1.0
	standard did not define any 1xx status codes, servers must not
	send a 1xx response to an HTTP/1.0 compliant client except under
	experimental conditions.
*/
//====================================================================================
	if (category == informational_responses) {
		Informational_responses num = static_cast<Informational_responses>(code[1]);

		if (num == Continue)				// 100
			return "Continue";
		if (num == Switching_Protocols)		// 101
			return "Switching protocols";
		if (num == Processing)				// 102	(WebDAV; RFC 2518)
			return "Processing";
		if (num == Early_Hints)				// 103	(RFC 8297)
			return "Early hints";

	//================================================================================

	//		CASHING WRITING CODES:

	/*	The following caching related warning codes were specified
		under RFC 7234. Unlike the other status codes above, these
		were not sent as the response status in the HTTP protocol,
		but as part of the "Warning" HTTP header.

		Since this "Warning" header is often neither sent by servers
		nor acknowledged by clients, this header and its codes were
		obsoleted by the HTTP Working Group in 2022 with RFC 9111.	*/

	//================================================================================
		if (num == Response_is_Stale)			// 110
			return "Response is Stale";
		if (num == Revalidation_Failed)			// 111
			return "Revalidation Failed";
		if (num == Disconnected_Operation)		// 112
			return "Disconnected Operation";
		if (num == Heuristic_Expiration)		// 113
			return "Heuristic Expiration";

		if (num == Miscellaneous_Warning)	// 199
			return "Miscellaneous Warning";
	//================================================================================

		return "Informational response";
	}

//====================================================================================

//		2 X X		SUCCESSFUL RESPONSES
/*
	This class of status codes indicates the action requested
	by the client was received, understood, and accepted.
*/
//====================================================================================
	if (category == successful_responses) {
		Successful_responses num = static_cast<Successful_responses>(code[1]);

		if (num == OK)								// 200
			return "OK";
		if (num == Created)							// 201
			return "Created";
		if (num == Accepted)						// 202
			return "Accepted";
		if (num == Non_Authoritative_Information)	// 203	(since HTTP/1.1)
			return "Non authoritative information";
		if (num == No_Content)						// 204
			return "No content";
		if (num == Reset_Content)					// 205 
			return "Reset content";
		if (num == Partial_Content)					// 206
			return "Partial content";
		if (num == Multi_Status)					// 207	(WebDAV; RFC 4918)
			return "Multi-status";
		if (num == Already_Reported)				// 208	(WebDAV; RFC 5842)
			return "Already reported";

		if (num == IM_Used)							// 226	(RFC 3229)
			return "IM used";

	//================================================================================

	//		CASHING WRITING CODES:		( description uppper (1 X X) )
	//================================================================================
		if (num == Transformation_Applied)		// 214
			return "Transformation Applied";

		if (num == Miscellaneous_Persistent_Warning)	// 299
			return "Miscellaneous Persistent Warning";
	//================================================================================

		return "Successful response";
	}

//====================================================================================

//		3 X X		REDIRECTION	MESSAGES
/*
	This class of status code indicates the client must take additional
	action to complete the request. Many of these status codes are used
	in URL redirection.

	A user agent may carry out the additional action with no user
	interaction only if the method used in the second request is GET or
	HEAD. A user agent may automatically redirect a request. A user
	agent should detect and intervene to prevent cyclical redirects.
*/
//====================================================================================
	if (category == redirection_messages) {
		Redirection_messages num = static_cast<Redirection_messages>(code[1]);

		if (num == Multiple_Choices)		// 300
			return "Multiple choices";
		if (num == Moved_Permanently)		// 301
			return "Moved permanently";
		if (num == Found)					// 302	(Moved temporarily)
			return "Found";
		if (num == See_Other)				// 303	(since HTTP/1.1)
			return "See other";
		if (num == Not_Modified)			// 304
			return "Not modified";
		if (num == Use_Proxy)				// 305	(since HTTP/1.1)
			return "Use proxy";
		if (num == Unused)					// 306
			return "Unused";
		if (num == Temporary_Redirect)		// 307	(since HTTP/1.1)
			return "Temporary redirect";
		if (num == Permanent_Redirect)		// 308
			return "Permanent redirect";

		return "Redirection message";
	}

//====================================================================================
//	NOT READY
//		4 X X		CLIENT ERROR RESPONSES
/*
	This class of status code is intended for situations in which
	the error seems to have been caused by the client. Except when
	responding to a HEAD request, the server should include an entity
	containing an explanation of the error situation, and whether it
	is a temporary or permanent condition. These status codes are
	applicable to any request method. User agents should display any
	included entity to the user.
*/
//====================================================================================
	if (category == client_error_responses) {
		Client_error_responses num = static_cast<Client_error_responses>(code[1]);

		if (num == Bad_Request)						// 400
			return "Bad request";
		if (num == Unauthorized)					// 401
			return "Unauthorized";
		if (num == Payment_Required)				// 402
			return "Payment required";
		if (num == Forbidden)						// 403
			return "Forbidden";
		if (num == Not_Found)						// 404
		 	return "Not found";
		if (num == Method_Not_Allowed)				// 405
			return "Method Not Allowed";
		if (num == Not_Acceptable)					// 406
			return "Not Acceptable";
		if (num == Proxy_Authentication_Required)	// 407
			return "Proxy Authentication Required";
		if (num == Request_Timeout)					// 408
			return "Request Timeout";
		if (num == Conflict)						// 409
			return "Conflict";
		if (num == Gone)							// 410
			return "Gone";
		if (num == Length_Required)					// 411
			return "Length Required";
		if (num == Precondition_Failed)				// 412
			return "Precondition Failed";
		if (num == Payload_Too_Large)				// 413
			return "Payload Too Large";
		if (num == URI_Too_Long)					// 414
			return "URI Too Long";
		if (num == Unsupported_Media_Type)			// 415
			return "Unsupported Media Type";
		if (num == Range_Not_Satisfiable)			// 416
			return "Range Not Satisfiable";
		if (num == Expectation_Failed)				// 417
			return "Expectation Failed";
		if (num == Teapot)							// 418	(RFC 2324, RFC 7168)
			return "I'm a teapot";

		if (num == Misdirected_Request)		// 421
			return "Misdirected Request";
		if (num == Unprocessable_Entity)	// 422
			return "Unprocessable Entity";
		if (num == Locked)					// 423	(WebDAV; RFC 4918)
			return "Locked";
		if (num == Failed_Dependency)		// 424	(WebDAV; RFC 4918)
			return "Failed Dependency";
		if (num == Too_Early)				// 425
			return "Too Early";
		if (num == Upgrade_Required)		// 426
			return "Upgrade Required";

		if (num == Precondition_Required)	// 428	(RFC 6585)
			return "Precondition Required";
		if (num == Too_Many_Requests)		// 429	(RFC 6585)
			return "Too Many Requests";
		
		if (num == Request_Header_Fields_Too_Large)		// 431	(RFC 6585)
			return "Request Header Fields Too Large";

		if (num == Unavailable_For_Legal_Reasons)	// 451	(RFC 7725)
			return "Unavailable For Legal Reasons";

	//================================================================================

	//		UNOFFICIAL CODES:

	/*	The following codes are not specified by any standard.	*/

	//================================================================================
		if (num == Page_Expired)		// 419	(Laravel Framework)
			return "Page Expired";
		if (num == Method_Failure)		// 420	(Spring Framework)
			return "Method Failure";

		if (num == Request_Header_Fields_Too_Large_Spoty)	// 430	(Shopify)
			return "Request Header Fields Too Large";

		if (num == Blocked_By_Windows_Parental_Controls)	// 450	(Microsoft)
			return "Blocked by Windows Parental Controls";

		if (num == Invalid_Token)		// 498	(Esri)
			return "Invalid Token";
	/*	if (num == Token_Required)		// 499	(Esri)
			return "Token Required";					*/
	//================================================================================

	//		INTERNET INFORMATION SERVICES:

	/*	Microsoft's Internet Information Services (IIS) web server expands
		the 4xx error space to signal errors with the client's request.		*/

	//================================================================================
		if (num == Login_Time_out)		// 440
			return "Login Time-out";

		if (num == Retry_With)		// 449
			return "Retry With";

	/*	if (num == Redirect)	// 451
			return "Redirect";			*/
	//================================================================================
	
	//		NGINX:

	/*	The nginx web server software expands the 4xx error
		space to signal issues with the client's request.	*/

	//================================================================================
		if (num == No_Response)		// 444
			return "No Response";

		if (num == Request_Header_Too_Large)			// 494
			return "Request header too large";
		if (num == SSL_Certificate_Error)				// 495
			return "SSL Certificate Error";
		if (num == SSL_Certificate_Required)			// 496
			return "SSL Certificate Required";
		if (num == HTTP_Request_Sent_To_HTTPS_Port)		// 497
			return "HTTP Request Sent to HTTPS Port";

		if (num == Client_Closed_Request)	// 499
			return "Client Closed Request";
	//================================================================================
	
	//		AWS ELASTIC LOAD BALANCER:

	/*	Amazon's Elastic Load Balancing adds a few custom return codes	*/

	//================================================================================
		if (num == No_response)		// 460
			return "No Response";

		if (num == Too_Many_IP)							// 463
			return "Request header too large";
		if (num == Incompatible_protocol_versions)		// 464
			return "Incompatible protocol versions";
	//================================================================================

		return "Client error responses";
	}

//====================================================================================
//	NOT READY
//		5 X X		SERVER ERROR RESPONSES
/*
	The server failed to fulfil a request.

	Response status codes beginning with the digit "5" indicate cases
	in which the server is aware that it has encountered an error or
	is otherwise incapable of performing the request. Except when
	responding to a HEAD request, the server should include an entity
	containing an explanation of the error situation, and indicate
	whether it is a temporary or permanent condition. Likewise, user
	agents should display any included entity to the user. These
	response codes are applicable to any request method.
*/
//====================================================================================
	if (category == server_error_responses) {
		Server_error_responses num = static_cast<Server_error_responses>(code[1]);

		if (num == Internal_Server_Error)			// 500
			return "Internal server error";
		if (num == Not_Implemented)					// 501
			return "Not implemented";
		if (num == Bad_Gateway)						// 502
			return "Bad Gateway";
		if (num == Service_Unavailable)				// 503
			return "Service Unavailable";
		if (num == Gateway_Timeout)					// 504
			return "Gateway Timeout";
		if (num == HTTP_Version_Not_Supported)		// 505
			return "HTTP Version Not Supported";
		if (num == Variant_Also_Negotiates)			// 506	(RFC 2295)
			return "Variant Also Negotiates";
		if (num == Insufficient_Storage)			// 507	(WebDAV; RFC 4918)
			return "Insufficient Storage";
		if (num == Loop_Detected)					// 508	(WebDAV; RFC 5842)
			return "Loop Detected";

		if (num == Not_Extended)						// 510	(RFC 2774)
			return "Not Extended";
		if (num == Network_Authentication_Required)		// 511	(RFC 6585)
			return "Network Authentication Required";

	//================================================================================

	//		UNOFFICIAL CODES:

	/*	The following codes are not specified by any standard.	*/

	//================================================================================
		if (num == Bandwidth_Limit_Exceeded)	// 509	(Apache Web Server/cPanel)
			return "Bandwidth Limit Exceeded";

		if (num == Site_Is_Overloaded)		// 529
			return "Site is overloaded";
		if (num == Site_Is_Frozen)			// 530
			return "Site is frozen";

		if (num == Network_Read_Timeout_Error)		// 598
			return "Network read timeout error";
		if (num == Network_Connect_Timeout_Error)	// 599
			return "Network Connect Timeout Error";
	//================================================================================
	
	//		UNOFFICIAL CODES:

	/*	The following codes are not specified by any standard.	*/

	//================================================================================
		if (num == Bandwidth_Limit_Exceeded)	// 509	(Apache Web Server/cPanel)
			return "Bandwidth Limit Exceeded";

		if (num == Site_Is_Overloaded)		// 529
			return "Site is overloaded";
		if (num == Site_Is_Frozen)			// 530
			return "Site is frozen";

		if (num == Network_Read_Timeout_Error)		// 598
			return "Network read timeout error";
		if (num == Network_Connect_Timeout_Error)	// 599
			return "Network Connect Timeout Error";
	//================================================================================

	//		CLOUDFLARE:

	/*	Cloudflare's reverse proxy service expands the 5xx series
		of errors space to signal issues with the origin server.	*/

	//================================================================================
		if (num == Web_Server_Returned_An_Unknown_Error)	// 520	(Apache Web Server/cPanel)
			return "Web Server Returned an Unknown Error";
		if (num == Web_Server_Is_Down)						// 521
			return "Web Server Is Down";
		if (num == Connection_Timed_Out)					// 522
			return "Connection Timed Out";
		if (num == Origin_Is_Unreachable)					// 523
			return "Network read timeout error";
		if (num == A_Timeout_Occurred)						// 524
			return "A Timeout Occurred";
		if (num ==  SSL_Handshake_Failed)					// 525
			return "SSL Handshake Failed";
		if (num == Invalid_SSL_Certificate)					// 526
			return "Invalid SSL Certificate";
		if (num == Railgun_Error)							// 527
			return "Railgun Error";

	/*	if (num == Cloudflare_1xxx)		// 530
			return "";							*/
	//================================================================================

	//		AWS ELASTIC LOAD BALANCER:

	/*	Amazon's Elastic Load Balancing adds a few custom return codes	*/

	//================================================================================
		if (num == unauthorized)	// 561
			return "Unauthorized";
	//================================================================================

		return "Informational response";
	}

//====================================================================================

	return "Undefined status code";
};