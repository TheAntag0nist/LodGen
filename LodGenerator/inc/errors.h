#ifndef ERRORS_H
#define ERRORS_H

// 1. Success
#define SUCCESS 1
// 2. Basic Error
#define ERR_BASIC 32
// 3. Mesh Errors
#define ERR_EMPTY_VERT_VEC  ERR_BASIC + 1
#define ERR_EMPTY_NORM_VEC  ERR_BASIC + 2 
#define ERR_EMPTY_IND_VEC   ERR_BASIC  + 3
// 4. Math Errors
#define ERR_THREAD_NUMBER   ERR_BASIC + 4
// 5. OpenCL Errors
#define ERR_NO_PLATFORMS    ERR_BASIC + 5
#define ERR_NO_DEVICES      ERR_BASIC + 6
#define ERR_CANT_BUILD_PROG ERR_BASIC + 7

#endif