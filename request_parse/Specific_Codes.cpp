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

		if (num == Miscellaneous_Warning)		// 199
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
		if (num == Transformation_Applied)				// 214
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

		if (num == Bad_Request)				// 400
			return "Bad request";
		if (num == Unauthorized)			// 401
			return "Unauthorized";
		if (num == Payment_Required)		// 402
			return "Payment required";
		if (num == Forbidden)				// 403
			return "Forbidden";
		if (num == Not_Found)				// 404

		// 	return "Not found";
		// if (num == Bad_Request)				// 405
		// 	return "Bad request";
		// if (num == Unauthorized)			// 406
		// 	return "Unauthorized";
		// if (num == Payment_Required)		// 407
		// 	return "Payment required";
		// if (num == Forbidden)				// 408
		// 	return "Forbidden";
		// if (num == Not_Found)				// 409
		// 	return "Not found";
		// if (num == Bad_Request)				// 410
		// 	return "Bad request";
		// if (num == Unauthorized)			// 411
		// 	return "Unauthorized";
		// if (num == Payment_Required)		// 412
		// 	return "Payment required";
		// if (num == Forbidden)				// 413
		// 	return "Forbidden";
		// if (num == Not_Found)				// 414
		// 	return "Not found";
		// if (num == Bad_Request)				// 415
		// 	return "Bad request";
		// if (num == Unauthorized)			// 416
		// 	return "Unauthorized";
		// if (num == Payment_Required)		// 417
		// 	return "Payment required";
		// if (num == Forbidden)				// 418	(RFC 2324, RFC 7168)
		// 	return "Forbidden";

		// if (num == Not_Found)				// 421
		// 	return "Not found";
		// if (num == Payment_Required)		// 422
		// 	return "Payment required";
		// if (num == Forbidden)				// 423	(WebDAV; RFC 4918)
		// 	return "Forbidden";
		// if (num == Not_Found)				// 424	(WebDAV; RFC 4918)
		// 	return "Not found";
		// if (num == Bad_Request)				// 425
		// 	return "Bad request";
		// if (num == Unauthorized)			// 426
		// 	return "Unauthorized";

		// if (num == Forbidden)				// 428	(RFC 6585)
		// 	return "Forbidden";
		// if (num == Not_Found)				// 429	(RFC 6585)
		// 	return "Not found";
		
		// if (num == Forbidden)				// 431	(RFC 6585)
		// 	return "Forbidden";

		// if (num == Not_Found)				// 451	(RFC 7725)
		// 	return "Not found";
	/*
		405 Method Not Allowed
		A request method is not supported for the requested resource; for example, a GET request on a form that requires data to be presented via POST, or a PUT request on a read-only resource.
		406 Not Acceptable
		The requested resource is capable of generating only content not acceptable according to the Accept headers sent in the request. See Content negotiation.
		407 Proxy Authentication Required
		The client must first authenticate itself with the proxy.
		408 Request Timeout
		The server timed out waiting for the request. According to HTTP specifications: "The client did not produce a request within the time that the server was prepared to wait. The client MAY repeat the request without modifications at any later time."
		409 Conflict
		Indicates that the request could not be processed because of conflict in the current state of the resource, such as an edit conflict between multiple simultaneous updates.
		410 Gone
		Indicates that the resource requested was previously in use but is no longer available and will not be available again. This should be used when a resource has been intentionally removed and the resource should be purged. Upon receiving a 410 status code, the client should not request the resource in the future. Clients such as search engines should remove the resource from their indices. Most use cases do not require clients and search engines to purge the resource, and a "404 Not Found" may be used instead.
		411 Length Required
		The request did not specify the length of its content, which is required by the requested resource.
		412 Precondition Failed
		The server does not meet one of the preconditions that the requester put on the request header fields.
		413 Payload Too Large
		The request is larger than the server is willing or able to process. Previously called "Request Entity Too Large" in RFC 2616.[19]
		414 URI Too Long
		The URI provided was too long for the server to process. Often the result of too much data being encoded as a query-string of a GET request, in which case it should be converted to a POST request. Called "Request-URI Too Long" previously in RFC 2616.[20]
		415 Unsupported Media Type
		The request entity has a media type which the server or resource does not support. For example, the client uploads an image as image/svg+xml, but the server requires that images use a different format.
		416 Range Not Satisfiable
		The client has asked for a portion of the file (byte serving), but the server cannot supply that portion. For example, if the client asked for a part of the file that lies beyond the end of the file. Called "Requested Range Not Satisfiable" previously RFC 2616.[21]
		417 Expectation Failed
		The server cannot meet the requirements of the Expect request-header field.[22]
		418 I'm a teapot (RFC 2324, RFC 7168)
		This code was defined in 1998 as one of the traditional IETF April Fools' jokes, in RFC 2324, Hyper Text Coffee Pot Control Protocol, and is not expected to be implemented by actual HTTP servers. The RFC specifies this code should be returned by teapots requested to brew coffee.[23] This HTTP status is used as an Easter egg in some websites, such as Google.com's "I'm a teapot" easter egg.[24][25][26] Sometimes, this status code is also used as a response to a blocked request, instead of the more appropriate 403 Forbidden.[27][28]
		421 Misdirected Request
		The request was directed at a server that is not able to produce a response (for example because of connection reuse).
		422 Unprocessable Entity
		The request was well-formed but was unable to be followed due to semantic errors.[9]
		423 Locked (WebDAV; RFC 4918)
		The resource that is being accessed is locked.[9]
		424 Failed Dependency (WebDAV; RFC 4918)
		The request failed because it depended on another request and that request failed (e.g., a PROPPATCH).[9]
		425 Too Early (RFC 8470)
		Indicates that the server is unwilling to risk processing a request that might be replayed.
		426 Upgrade Required
		The client should switch to a different protocol such as TLS/1.3, given in the Upgrade header field.
		428 Precondition Required (RFC 6585)
		The origin server requires the request to be conditional. Intended to prevent the 'lost update' problem, where a client GETs a resource's state, modifies it, and PUTs it back to the server, when meanwhile a third party has modified the state on the server, leading to a conflict.[29]
		429 Too Many Requests (RFC 6585)
		The user has sent too many requests in a given amount of time. Intended for use with rate-limiting schemes.[29]
		431 Request Header Fields Too Large (RFC 6585)
		The server is unwilling to process the request because either an individual header field, or all the header fields collectively, are too large.[29]
		451 Unavailable For Legal Reasons (RFC 7725)
		A server operator has received a legal demand to deny access to a resource or to a set of resources that includes the requested resource.[30] The code 451 was chosen as a reference to the novel Fahrenheit 451 (see the Acknowledgements in the RFC).
	*/

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

		if (num == Internal_Server_Error)	// 500
			return "Internal server error";
		if (num == Not_Implemented)			// 501
			return "Not implemented";
		if (num == Bad_Gateway)				// 502
			return "Bad Gateway";
		if (num == Service_Unavailable)		// 503
			return "Service Unavailable";

		// if (num == Internal_Server_Error)	// 504
		// 	return "Internal server error";
		// if (num == Not_Implemented)			// 505
		// 	return "Not implemented";
		// if (num == Processing)				// 506	(RFC 2295)
		// 	return "Processing";
		// if (num == Early_Hints)				// 507	(WebDAV; RFC 4918)
		// 	return "Early hints";
		// if (num == Early_Hints)				// 508	(WebDAV; RFC 5842)
		// 	return "Early hints";

		// if (num == Processing)				// 510	(RFC 2774)
		// 	return "Processing";
		// if (num == Early_Hints)				// 511	(RFC 6585)
		// 	return "Early hints";
	/*
		504 Gateway Timeout
		The server was acting as a gateway or proxy and did not receive a timely response from the upstream server.
		505 HTTP Version Not Supported
		The server does not support the HTTP version used in the request.
		506 Variant Also Negotiates (RFC 2295)
		Transparent content negotiation for the request results in a circular reference.[32]
		507 Insufficient Storage (WebDAV; RFC 4918)
		The server is unable to store the representation needed to complete the request.[9]
		508 Loop Detected (WebDAV; RFC 5842)
		The server detected an infinite loop while processing the request (sent instead of 208 Already Reported).
		510 Not Extended (RFC 2774)
		Further extensions to the request are required for the server to fulfil it.[33]
		511 Network Authentication Required (RFC 6585)
		The client needs to authenticate to gain network access. Intended for use by intercepting proxies used to control access to the network (e.g., "captive portals" used to require agreement to Terms of Service before granting full Internet access via a Wi-Fi hotspot).
	*/

		return "Informational response";
	}

//====================================================================================

	return "Undefined status code";
};