//
//   Gerated by script. Don't edit this file
//

/*
    Title: Errors

    Pre-defined system errors.

    <M_OK> indicates a successful return and is guaranteed to be zero.
    Other errors are guaranteed to be negative value.
*/

#ifndef _MONA_ERROR_
#define _MONA_ERROR_

#include "sys/deprecated.h"

/*
    constant: M_OK

    Indicates ok
*/
#define M_OK 0

/*
    constant: M_BAD_MUTEX_ID

    Indicates bad mutex id
*/
#define M_BAD_MUTEX_ID -5000

/*
    constant: M_TIMED_OUT

    Indicates timed out
*/
#define M_TIMED_OUT -5001

/*
    constant: M_BUSY

    Indicates resource busy
*/
#define M_BUSY -5002

/*
    constant: M_BAD_CONDITION_ID

    Indicates bad condition id
*/
#define M_BAD_CONDITION_ID -5003

/*
    constant: M_BAD_THREAD_ID

    Indicates bad thread id
*/
#define M_BAD_THREAD_ID -5004

/*
    constant: M_BAD_SEMAPHORE_ID

    Indicates bad semaphore id
*/
#define M_BAD_SEMAPHORE_ID -5005

/*
    constant: M_NAME_NOT_FOUND

    Indicates name not found
*/
#define M_NAME_NOT_FOUND -5006

/*
    constant: M_BAD_MEMORY_MAP_ID

    Indicates bad memory map id
*/
#define M_BAD_MEMORY_MAP_ID -5007

/*
    constant: M_MESSAGE_OVERFLOW

    Indicates message queue overflow
*/
#define M_MESSAGE_OVERFLOW -5008

/*
    constant: M_MEMORY_MAP_ERROR

    Indicates memory map failed
*/
#define M_MEMORY_MAP_ERROR -5009

/*
    constant: M_BAD_INDEX

    Indicates bad index
*/
#define M_BAD_INDEX -5010

/*
    constant: M_MESSAGE_NOT_FOUND

    Indicates message is not found
*/
#define M_MESSAGE_NOT_FOUND -5011

/*
    constant: M_RELEASED

    Indicates object reference is released, but accessed
*/
#define M_RELEASED -5012

/*
    constant: M_BAD_TIMER_ID

    Indicates bad timer id
*/
#define M_BAD_TIMER_ID -5013

/*
    constant: M_BAD_STREAM

    Indicates bad stream
*/
#define M_BAD_STREAM -5014

/*
    constant: M_BAD_ADDRESS

    Indicates bad access
*/
#define M_BAD_ADDRESS -5015

/*
    constant: M_NO_MEMORY

    Indicates no memory
*/
#define M_NO_MEMORY -5016

/*
    constant: M_FILE_NOT_FOUND

    Indicates file not found
*/
#define M_FILE_NOT_FOUND -5017

/*
    constant: M_NO_SPACE

    Indicates no space
*/
#define M_NO_SPACE -5018

/*
    constant: M_READ_ERROR

    Indicates read error
*/
#define M_READ_ERROR -5019

/*
    constant: M_WRITE_ERROR

    Indicates write error
*/
#define M_WRITE_ERROR -5020

/*
    constant: M_BAD_OFFSET

    Indicates bad offset
*/
#define M_BAD_OFFSET -5021

/*
    constant: M_BAD_FILE_ID

    Indicates bad file id
*/
#define M_BAD_FILE_ID -5022

/*
    constant: M_NOT_SUPPORTED

    Indicates not supported
*/
#define M_NOT_SUPPORTED -5023

/*
    constant: M_UNKNOWN

    Indicates unknown error
*/
#define M_UNKNOWN -5024

/*
    constant: M_BAD_ARG

    Indicates bad arguments
*/
#define M_BAD_ARG -5025

#endif
