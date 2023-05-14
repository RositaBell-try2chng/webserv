#include "Specific_Codes.hpp"

std::string ft_reason_phrase(int *code) {

	Status_Categories category = static_cast<Status_Categories>(code[0]);

// 1 . .
	if (category == informational_responses) {
		Informational_responses num = static_cast<Informational_responses>(code[1]);

		if (num == Continue)				// 100
			return "Continue";
		if (num == Switching_Protocols)		// 101
			return "Switching protocols";
		if (num == Processing)				// 102
			return "Processing";
		if (num == Early_Hints)				// 103
			return "Early hints";

		return "Informational response";
	}

// 2 . .
	if (category == successful_responses) {
		Successful_responses num = static_cast<Successful_responses>(code[1]);

		if (num == OK)								// 200
			return "OK";
		if (num == Created)							// 201
			return "Created";
		if (num == Accepted)						// 202
			return "Accepted";
		if (num == Non_Authoritative_Information)	// 203
			return "Non authoritative information";
		if (num == No_Content)						// 204
			return "No content";
		if (num == Reset_Content)					// 205 
			return "Reset content";
		if (num == Partial_Content)					// 206
			return "Partial content";
		if (num == Multi_Status)					// 207
			return "Multi-status";
		if (num == Already_Reported)				// 208
			return "Already reported";
		if (num == IM_Used)							// 226
			return "IM used";
			
		return "Successful response";
	}

// 3 . .
	if (category == redirection_messages) {
		Redirection_messages num = static_cast<Redirection_messages>(code[1]);

		if (num == Multiple_Choices)		// 300
			return "Multiple choices";
		if (num == Moved_Permanently)		// 301
			return "Moved permanently";
		if (num == Found)					// 302
			return "Found";
		if (num == See_Other)				// 304
			return "See other";
		if (num == Not_Modified)			// 305
			return "Not modified";
		if (num == Use_Proxy)				// 306
			return "Use proxy";
		if (num == Unused)					// 307
			return "Unused";
		if (num == Temporary_Redirect)		// 308
			return "Temporary redirect";
		if (num == Permanent_Redirect)		// 309
			return "Permanent redirect";

		return "Redirection message";
	}

// 4 . .
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
			return "Not found";

		return "Client error responses";
	}

// 5 . .
	if (category == server_error_responses) {
		Server_error_responses num = static_cast<Server_error_responses>(code[1]);

		if (num == Internal_Server_Error)	// 500
			return "Internal server error";
		if (num == Not_Implemented)			// 501
			return "Not implemented";
		// if (num == Processing)			// 502
		// 	return "Processing";
		// if (num == Early_Hints)			// 503
		// 	return "Early hints";

		return "Informational response";
	}

	return "Undefined status code";
};