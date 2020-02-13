#pragma once
#include "common.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: udp_client.h - contains the declaration functions
--								and constants for UDP client
--
--
--
-- PROGRAM: File Transfer and Protocol Analysis Application
--
-- DATE: January 29, 2020
--
-- REVISIONS:
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- NOTES:
-- This is a header file contains declaration functions and constants for UDP client.
----------------------------------------------------------------------------------------------------------------------*/
#define MAXLEN				65000   // Maximum Buffer length
#define DEFLEN				64		// Default Length

void udp_client(PVOID network);

