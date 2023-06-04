#ifndef __error_codes_h_
#define __error_codes_h_


typedef enum 
{
	ERR_CODE_GEN_BEGIN 		= 0,
	ERR_CODE_FAILURE		= ERR_CODE_GEN_BEGIN,
	ERR_CODE_SUCCESS,

	/* GPIO ERROR CODES */
	ERR_CODE_GPIO_BEGIN = 100,
	ERR_CODE_GPIO_PARAM_MISMATCH,

	/* I2C ERROR CODES */
	ERR_CODE_I2C_BEGIN = 200,




	ERR_CODE_MAX		// Supposed to be last entry in the enumeration
	
}ERR_CODE;


#endif /* __error_codes_h_ */
