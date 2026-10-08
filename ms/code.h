#pragma once


#define  RESULT_OK                  0
#define  ERRORNO0                   10000



#define  MK_ERRNO(X)                (ERRORNO0+(X))




#define  GENERAL_ERROR              MK_ERRNO(0)



#define  CONNECT_ERROR              MK_ERRNO(1)

